import model.OutInstrument
import model.Song
import javax.sound.midi.MidiSystem
import java.io.File

enum class NoteType { NOTE_OFF, NOTE_ON, POLY_AFTER, CONTROL_CHANGE, PROGRAM_CHANGE, CHANNEL_AFTER, PITCH_BEND, SYSEX, TEMPO }
class Note(
    var type: NoteType,
    var channel: Int,
    var data1: UByte,
    var data2: UByte,
    var time: Long
) {
    fun print() {
        val typestr = when (type) {
            NoteType.NOTE_OFF ->       "Note off   "
            NoteType.NOTE_ON ->        "Note on    "
            NoteType.POLY_AFTER ->     "Poly touch "
            NoteType.CONTROL_CHANGE -> "CC         "
            NoteType.PROGRAM_CHANGE -> "PC         "
            NoteType.CHANNEL_AFTER ->  "Aftertouch "
            NoteType.PITCH_BEND ->     "Pitch bend "
            NoteType.SYSEX ->          "SYSEX      "
            NoteType.TEMPO ->          "TEMPO      "
        }
        println("  " + time.toInt() + ": ch" + channel + "  " + typestr + " (" + data1.toInt() + ", " + data2.toInt() + ")")
    }

    fun write(bytes: ArrayList<UByte>, fVerbose: Boolean) {
        val typebyte: UByte = when (type) {
            NoteType.NOTE_OFF ->       0.toUByte()
            NoteType.NOTE_ON ->        1.toUByte()
            NoteType.POLY_AFTER ->     2.toUByte()
            NoteType.CONTROL_CHANGE -> 3.toUByte()
            NoteType.PROGRAM_CHANGE -> 4.toUByte()
            NoteType.CHANNEL_AFTER ->  5.toUByte()
            NoteType.PITCH_BEND ->     6.toUByte()
            NoteType.TEMPO ->          7.toUByte()
            NoteType.SYSEX ->          255.toUByte()
        }
        val timebyte0 = (time % 256)
        val time1 = (time - timebyte0) / 256
        val time2 = (time - (timebyte0 + time1 * 256)) / 65536
        bytes.add(timebyte0.toUByte())
        bytes.add(time1.toUByte())
        bytes.add(time2.toUByte())
        bytes.add(channel.toUByte())
        bytes.add(typebyte)
        bytes.add(data1)
        bytes.add(data2)
    }
}

class MidiParser(val song: Song, val filepath: String, val instrumentCount: Int, val fVerbose: Boolean) {

    val outBytes = ArrayList<UByte>()

    fun parse() {

        val events = ArrayList<Note>()
        var resolution = 192
        if (song.notes.size > 0) {
            song.notes.forEach {
                events.add(Note(when (it.event) {
                    "NOTE_ON" -> NoteType.NOTE_ON
                    "NOTE_OFF" -> NoteType.NOTE_OFF
                    "CONTROL_CHANGE" -> NoteType.CONTROL_CHANGE
                    "PITCH_BEND" -> NoteType.PITCH_BEND
                    else -> throw RuntimeException("illegal note type")
                }, it.chan, it.d1.toUByte(), it.d2.toUByte(), it.tick.toLong()))
            }
        } else {
            val sequence = MidiSystem.getSequence(File(filepath))
            resolution = sequence.resolution
            for (track in sequence.getTracks()) {
                if (fVerbose) println("  MIDI track size: " + track.size())
                for (i in 0..track.size() - 1) {
                    val message = track.get(i).message.message
                    val len = track.get(i).message.length
                    val time = track.get(i).tick
                    val status = message[0] + 256
                    var data1: UByte = 0.toUByte()
                    var data2: UByte = 0.toUByte()
                    if (len > 1) data1 = message[1].toUByte()
                    if (len > 2) data2 = message[2].toUByte()
                    val note = when (status) {
                        in 128..143 -> Note(NoteType.NOTE_OFF, status - 128, data1, data2, time)
                        in 144..159 -> Note(NoteType.NOTE_ON, status - 144, data1, data2, time)
                        in 160..175 -> Note(NoteType.POLY_AFTER, status - 160, data1, data2, time)
                        in 176..191 -> Note(NoteType.CONTROL_CHANGE, status - 176, data1, data2, time)
                        in 192..207 -> Note(NoteType.PROGRAM_CHANGE, status - 192, data1, data2, time)
                        in 208..223 -> Note(NoteType.CHANNEL_AFTER, status - 208, data1, data2, time)
                        in 224..239 -> Note(NoteType.PITCH_BEND, status - 224, data1, data2, time)
                        else -> Note(NoteType.SYSEX, status, data1, data2, time)
                    }
                    if ((note.type == NoteType.SYSEX) && (note.data1 == 81.toUByte()) && (note.data2 == 3.toUByte())) {
                        note.type = NoteType.TEMPO
                        var tempo = message[3].toInt() * 65536 + message[4].toInt() * 256 + message[5];
                        tempo = tempo / 1000;
                        note.data1 = (tempo / 256).toUByte()
                        note.data2 = (tempo - note.data1.toInt() * 256).toUByte()
                    }
                    events.add(note)
                }
            }
        }


        // Interleave all events sequentially
        events.sortBy { it.time }

        // Remap channels
        val channelMap = HashMap<Int,Int>()
        events.forEach {
            channelMap[it.channel] = 255
        }
        var outChannel = 0
        channelMap.keys.forEach { inChannel ->
            channelMap[inChannel] = outChannel
            outChannel++
        }
        events.forEach {
            it.channel = channelMap[it.channel]!!
        }


        // Write event count as 3 bytes
        val count = events.size
        val count0 = (count % 256)
        val count1 = (count - count0) / 256
        val count2 = (count - (count0 + count1 * 256)) / 65536
        outBytes.add(count0.toUByte())
        outBytes.add(count1.toUByte())
        outBytes.add(count2.toUByte())

        // Write ticks-per-beat as 2 bytes
        val res0 = (resolution % 256)
        val res1 = (resolution - res0) / 256
        outBytes.add(res0.toUByte())
        outBytes.add(res1.toUByte())

        // Write instrument count as 1 byte
        outBytes.add(instrumentCount.toUByte())

        events.forEach {
            if (fVerbose) it.print()
            it.write(outBytes, fVerbose)
        }

        println("    " + filepath + ": " + events.size + " notes on " + channelMap.keys.size + " channels at " + resolution + " tpq")
    }

    fun writeToFile(writer: Writer, songConfig: Song, instruments: HashMap<String,OutInstrument>) {
        writer.writeMarker("SNG")
        writer.writeMarker(songConfig.id.toString())
        writer.writeBytes(outBytes)
        println("  wrote song " + songConfig.id)
        writer.writeMarker("INS")
        songConfig.voices.forEachIndexed { i, instrument ->
            val outInstrument = instruments[instrument] ?: throw RuntimeException("song uses undefined instrument")
            outInstrument.writeToFile(writer, songConfig.volumes[i], songConfig.pans[i])
        }
    }
}

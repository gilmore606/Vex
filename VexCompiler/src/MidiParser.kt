import javax.sound.midi.MidiSystem
import java.io.File

enum class TuneMsgType { NOTE_OFF, NOTE_ON, POLY_AFTER, CONTROL_CHANGE, PROGRAM_CHANGE, CHANNEL_AFTER, PITCH_BEND, SYSEX, TEMPO }
class TuneMsg(
        var type: TuneMsgType,
        var channel: Int,
        var data1: UByte,
        var data2: UByte,
        var time: Long
) {
    fun print() {
        val typestr = when (type) {
            TuneMsgType.NOTE_OFF ->       "Note off        "
            TuneMsgType.NOTE_ON ->        "Note on         "
            TuneMsgType.POLY_AFTER ->     "Poly aftertouch "
            TuneMsgType.CONTROL_CHANGE -> "CC              "
            TuneMsgType.PROGRAM_CHANGE -> "PC              "
            TuneMsgType.CHANNEL_AFTER ->  "Aftertouch      "
            TuneMsgType.PITCH_BEND ->     "Pitch bend      "
            TuneMsgType.SYSEX ->          "SYSEX           "
            TuneMsgType.TEMPO ->          "TEMPO           "
        }
        println("  " + time.toInt() + ": " + channel + "  " + typestr + " (" + data1.toInt() + ", " + data2.toInt() + ")")
    }

    fun write(bytes: ArrayList<UByte>) {
        val typebyte: UByte = when (type) {
            TuneMsgType.NOTE_OFF ->       0.toUByte()
            TuneMsgType.NOTE_ON ->        1.toUByte()
            TuneMsgType.POLY_AFTER ->     2.toUByte()
            TuneMsgType.CONTROL_CHANGE -> 3.toUByte()
            TuneMsgType.PROGRAM_CHANGE -> 4.toUByte()
            TuneMsgType.CHANNEL_AFTER ->  5.toUByte()
            TuneMsgType.PITCH_BEND ->     6.toUByte()
            TuneMsgType.TEMPO ->          7.toUByte()
            TuneMsgType.SYSEX ->          255.toUByte()
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
        println(timebyte0.toUByte().toString() + "," + time1.toUByte().toString() + "," + time2.toUByte().toString() + ", " + channel.toUByte()
            + ", " + typebyte.toString() + ", " + data1.toString() + "," + data2.toString())
    }
}

class MidiParser {


    fun parse() {

        val sequence = MidiSystem.getSequence(File("data/sotndra1.mid"))
        val events = ArrayList<TuneMsg>()
        for (track in sequence.getTracks()) {
            println("track size: " + track.size())
            for (i in 0..track.size()-1) {
                val message = track.get(i).message.message
                val len = track.get(i).message.length
                val time = track.get(i).tick
                val status = message[0] + 256
                var data1: UByte = 0.toUByte()
                var data2: UByte = 0.toUByte()
                if (len > 1) data1 = message[1].toUByte()
                if (len > 2) data2 = message[2].toUByte()
                val tuneMsg = when (status) {
                    in 128..143 -> TuneMsg(TuneMsgType.NOTE_OFF, status-128, data1, data2, time)
                    in 144..159 -> TuneMsg(TuneMsgType.NOTE_ON, status-144, data1, data2, time)
                    in 160..175 -> TuneMsg(TuneMsgType.POLY_AFTER, status-160, data1, data2, time)
                    in 176..191 -> TuneMsg(TuneMsgType.CONTROL_CHANGE, status-176, data1, data2, time)
                    in 192..207 -> TuneMsg(TuneMsgType.PROGRAM_CHANGE, status-192, data1, data2, time)
                    in 208..223 -> TuneMsg(TuneMsgType.CHANNEL_AFTER, status-208, data1, data2, time)
                    in 224..239 -> TuneMsg(TuneMsgType.PITCH_BEND, status-224, data1, data2, time)
                    else -> TuneMsg(TuneMsgType.SYSEX, status, data1, data2, time)
                }
                if ((tuneMsg.type == TuneMsgType.SYSEX) && (tuneMsg.data1 == 81.toUByte()) && (tuneMsg.data2 == 3.toUByte())) {
                    tuneMsg.type = TuneMsgType.TEMPO
                    var tempo = message[3].toInt() * 65536 + message[4].toInt() * 256 + message[5];
                    tempo = tempo / 1000;
                    tuneMsg.data1 = (tempo / 256).toUByte()
                    tuneMsg.data2 = (tempo - tuneMsg.data1.toInt() * 256).toUByte()
                }
                events.add(tuneMsg)
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

        // Generate byte stream
        val outBytes = ArrayList<UByte>()

        // Write event count as 3 bytes
        val count = events.size
        val count0 = (count % 256)
        val count1 = (count - count0) / 256
        val count2 = (count - (count0 + count1 * 256)) / 65536
        outBytes.add(count0.toUByte())
        outBytes.add(count1.toUByte())
        outBytes.add(count2.toUByte())

        // Write ticks-per-beat as 2 bytes
        val resolution = sequence.resolution
        val res0 = (resolution % 256)
        val res1 = (resolution - res0) / 256
        outBytes.add(res0.toUByte())
        outBytes.add(res1.toUByte())

        println("writing " + events.size + " events")
        events.forEach {
            //it.print()
            it.write(outBytes)
        }

        File("data/sotndra1.vexm").writeBytes(outBytes.toUByteArray().toByteArray())
    }

}

import javax.sound.midi.MidiSystem
import java.io.File
import java.lang.RuntimeException

enum class TuneMsgType { NOTE_OFF, NOTE_ON, POLY_AFTER, CONTROL_CHANGE, PROGRAM_CHANGE, CHANNEL_AFTER, BEND_RANGE, SYSEX }
class TuneMsg(
        val type: TuneMsgType,
        var channel: Int,
        val data1: Byte,
        val data2: Byte,
        val time: Long
) {
    fun print() {
        val typestr = when (type) {
            TuneMsgType.NOTE_OFF ->       "Note off        "
            TuneMsgType.NOTE_ON ->        "Note on         "
            TuneMsgType.POLY_AFTER ->     "Poly aftertouch "
            TuneMsgType.CONTROL_CHANGE -> "CC              "
            TuneMsgType.PROGRAM_CHANGE -> "PC              "
            TuneMsgType.CHANNEL_AFTER ->  "Aftertouch      "
            TuneMsgType.BEND_RANGE ->     "Pitchbend range "
            TuneMsgType.SYSEX ->          "SYSEX           "
        }
        println("  " + time.toInt() + ": " + channel + "  " + typestr + " (" + data1.toInt() + ", " + data2.toInt() + ")")
    }

    fun write(): String {
        val typeint = when (type) {
            TuneMsgType.NOTE_OFF ->       0
            TuneMsgType.NOTE_ON ->        1
            TuneMsgType.POLY_AFTER ->     2
            TuneMsgType.CONTROL_CHANGE -> 3
            TuneMsgType.PROGRAM_CHANGE -> 4
            TuneMsgType.CHANNEL_AFTER ->  5
            TuneMsgType.BEND_RANGE ->     6
            TuneMsgType.SYSEX ->          255
        }
        return time.toString() + "," + channel.toString() + "," + typeint.toString() + "," + data1.toString() + "," + data2.toString()
    }
}

class MidiParser {


    fun parse() {

        val sequence = MidiSystem.getSequence(File("data/MoonPatrol.mid"))
        val events = ArrayList<TuneMsg>();

        for (track in sequence.getTracks()) {
            println("track size: " + track.size())
            for (i in 0..track.size()-1) {
                val message = track.get(i).message.message
                val len = track.get(i).message.length
                val time = track.get(i).tick
                val status = message[0] + 256
                var data1: Byte = 0
                var data2: Byte = 0
                if (len > 1) data1 = message[1]
                if (len > 2) data2 = message[2]
                val tuneMsg = when (status) {
                    in 128..143 -> TuneMsg(TuneMsgType.NOTE_OFF, status-128, data1, data2, time)
                    in 144..159 -> TuneMsg(TuneMsgType.NOTE_ON, status-144, data1, data2, time)
                    in 160..175 -> TuneMsg(TuneMsgType.POLY_AFTER, status-160, data1, data2, time)
                    in 176..191 -> TuneMsg(TuneMsgType.CONTROL_CHANGE, status-176, data1, data2, time)
                    in 192..207 -> TuneMsg(TuneMsgType.PROGRAM_CHANGE, status-192, data1, data2, time)
                    in 208..223 -> TuneMsg(TuneMsgType.CHANNEL_AFTER, status-208, data1, data2, time)
                    in 224..239 -> TuneMsg(TuneMsgType.BEND_RANGE, status-224, data1, data2, time)
                    else -> TuneMsg(TuneMsgType.SYSEX, status, data1, data2, time)
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

        var outString = events.size.toString() + "\n"
        events.forEach {
            //it.print()
            outString += it.write() + "\n"
        }
        File("data/moonpatrol.vexm").writeText(outString)
        println(outString)
    }

}

import java.io.File

class OutFile(val filename: String, val gameTitle: String) {

    lateinit var file: File
    val outBytes = ArrayList<UByte>()

    fun open() {
        file = File(filename + ".vexo")
        writeMarker("VEXO")
        writeMarker(gameTitle)
        println("opened " + filename + " for writing")
    }

    fun close() {
        writeMarker("EOF")
        file.writeBytes(outBytes.toUByteArray().toByteArray())
        println("closed " + filename)
    }

    fun writeMarker(s: String) {
        val buf = ArrayList<UByte>()
        buf.add(255.toUByte())
        s.forEach { buf.add(it.toInt().toUByte()) }
        buf.add(255.toUByte())
        outBytes.addAll(buf)
    }

    fun writeBytes(b: ArrayList<UByte>) {
        outBytes.addAll(b)
    }
}

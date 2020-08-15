import java.io.File

class OutFile(val filename: String, val gameTitle: String, val aspectRatio: ArrayList<Int>) {

    lateinit var file: File
    val outBytes = ArrayList<UByte>()

    fun open() {
        file = File(filename + ".vexo")
        writeMarker("VEX")
        writeMarker(gameTitle)
        writeByte(aspectRatio[0].toUByte())
        writeByte(aspectRatio[1].toUByte())
        println("opened " + filename + " for writing")
    }

    fun close() {
        writeMarker("EOF")
        writeByte(255.toUByte())
        writeByte(255.toUByte())
        writeByte(255.toUByte())
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

    fun writeByte(b: UByte) {
        outBytes.add(b)
    }
    fun writeBytes(b: ArrayList<UByte>) {
        outBytes.addAll(b)
    }

    fun write2ByteInt(i: Int) {
        val b0 = (i % 256)
        val b1 = (i - b0) / 256
        outBytes.add(b0.toUByte())
        outBytes.add(b1.toUByte())
    }
}

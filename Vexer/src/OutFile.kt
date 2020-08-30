import compiler.Value
import compiler.ValueType.*
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

    fun write3ByteInt(i: Int) {
        val b0 = (i % 256)
        val b1 = ((i - b0) / 256) % 256
        val b2 = (i - (b1 * 256 + b0)) / 65536
        outBytes.add(b0.toUByte())
        outBytes.add(b1.toUByte())
        outBytes.add(b2.toUByte())
    }
    fun write4ByteInt(i: Int) {
        val b0 = i and 0xFF;
        val b1 = (i shr 8) and 0xFF;
        val b2 = (i shr 16) and 0xFF;
        val b3 = (i shr 24) and 0xFF;
        outBytes.add(b0.toUByte())
        outBytes.add(b1.toUByte())
        outBytes.add(b2.toUByte())
        outBytes.add(b3.toUByte())
    }

    fun write2ByteFloats(floats: ArrayList<Float>) {
        floats.forEach { f ->
            var s = f
            if (f >= 1.0f) s = 0.9999f
            if (f <= -1.0f) s = -0.9999f
            val i = ((s + 1.0f) * 0.5f * 65536.0f).toInt()
            write2ByteInt(i)
        }
    }

    fun write4ByteFloat(f: Float) {
        write4ByteInt(f.toBits())
    }
    fun write4ByteFloats(floats: ArrayList<Float>) {
        floats.forEach { f ->
            write4ByteInt(f.toBits())
        }
    }

    fun writeString(s: String) {
        writeByte(254.toUByte())
        for (i in 0 until s.length) {
            writeByte(s[i].toByte().toUByte())
        }
        writeByte(254.toUByte())
    }

    fun writeValue(v: Value) {
        when (v.type) {
            // nil  1 byte = 0 (nil type)
            VAL_NIL -> {
                writeByte(0.toUByte()) // type nil
            }
            // bool  1 byte = 1 (false) 2 (true)
            VAL_BOOL -> {
                writeByte(if (v.boolean) 2.toUByte() else 1.toUByte()) // type boolfalse (1) or booltrue (2)
            } // int   4 bytes = 3 (int type) + 3 LSB byte int
            VAL_INT -> {
                writeByte(3.toUByte()) // type int
                write3ByteInt(v.integer)
            }
            VAL_FLOAT -> {
                writeByte(4.toUByte())  // 1 byte = 4 (float) 4 bytes float
                write4ByteInt(v.fp.toBits())
            }
            VAL_VECTOR -> {
                writeByte(5.toUByte())  // 1 byte = 5 (vector) 8 bytes 2 floats
                write4ByteInt(v.v1.toBits())
                write4ByteInt(v.v2.toBits())
            }
            VAL_COLOR -> {
                writeByte(6.toUByte())   // 1 byte = 6 (color) 12 bytes 3 floats
                write4ByteInt(v.c1.toBits())
                write4ByteInt(v.c2.toBits())
                write4ByteInt(v.c3.toBits())
            }
            VAL_STRING -> {
                writeByte(6.toUByte())
                writeString(v.string)
            }
        }
    }
}

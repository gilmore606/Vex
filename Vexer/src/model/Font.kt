package model

import Writer

data class Font(
        val id: Int,
        val glyphs: HashMap<String,ArrayList<Float>>
) {

    fun writeToFile(writer: Writer) {
        writer.writeMarker("FNT")
        writer.writeMarker(id.toString())
        // 2 bytes for glyph count
        writer.writeByte(glyphs.size.toUByte())
        glyphs.forEach {
            val cstr = it.key
            val c = cstr.toCharArray()[0];
            val data = it.value
            // 1 byte for glyph char
            writer.writeByte(c.toByte().toUByte())
            // 2 bytes for float count
            writer.write2ByteInt(data.size)
            writer.write2ByteFloats(data)
        }
        println("wrote FNT " + id + " (" + glyphs.size + " glyphs)")
    }
}

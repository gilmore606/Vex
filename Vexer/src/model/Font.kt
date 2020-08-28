package model

import OutFile

data class Font(
        val id: Int,
        val glyphs: HashMap<String,ArrayList<Float>>
) {

    fun writeToFile(outFile: OutFile) {
        outFile.writeMarker("FNT")
        outFile.writeMarker(id.toString())
        // 2 bytes for glyph count
        outFile.writeByte(glyphs.size.toUByte())
        glyphs.forEach {
            val cstr = it.key
            val c = cstr.toCharArray()[0];
            val data = it.value
            // 1 byte for glyph char
            outFile.writeByte(c.toByte().toUByte())
            // 2 bytes for float count
            outFile.write2ByteInt(data.size)
            outFile.writeFloats(data)
        }
        println("wrote FNT " + id + " (" + glyphs.size + " glyphs)")
    }
}

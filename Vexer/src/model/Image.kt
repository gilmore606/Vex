package model

import Writer

data class Image(
        val id: Int,
        val name: String,
        val lines: ArrayList<Float> = ArrayList(),
        val points: ArrayList<Float> = ArrayList()
) {

    fun writeToFile(writer: Writer) {
        writer.writeMarker("IMG")
        writer.writeMarker(id.toString())
        // 2 bytes for line float count
        writer.write2ByteInt(lines.size)
        writer.write2ByteFloats(lines)
        // 2 bytes for point float count
        writer.write2ByteInt(points.size)
        writer.write2ByteFloats(points)
        println("wrote IMG " + id + " (" + lines.size + " linefloat " + points.size + " pointfloat)")
    }
}

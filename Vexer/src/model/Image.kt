package model

import OutFile

data class Image(
        val id: Int,
        val name: String,
        val lines: ArrayList<Float> = ArrayList(),
        val points: ArrayList<Float> = ArrayList()
) {

    fun writeToFile(outFile: OutFile) {
        outFile.writeMarker("IMG")
        outFile.writeMarker(id.toString())
        // 2 bytes for line float count
        outFile.write2ByteInt(lines.size)
        outFile.write2ByteFloats(lines)
        // 2 bytes for point float count
        outFile.write2ByteInt(points.size)
        outFile.write2ByteFloats(points)
        println("wrote IMG " + id + " (" + lines.size + " linefloat " + points.size + " pointfloat)")
    }
}

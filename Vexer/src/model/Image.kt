package model

import OutFile

data class Image(
        val id: Int,
        val data: ArrayList<Float>
) {

    fun writeToFile(outFile: OutFile) {
        outFile.writeMarker("IMG")
        outFile.writeMarker(id.toString())
        // 2 bytes for float count
        outFile.write2ByteInt(data.size)
        outFile.writeFloats(data)
        println("wrote IMG " + id + " (" + data.size + " floats)")
    }
}

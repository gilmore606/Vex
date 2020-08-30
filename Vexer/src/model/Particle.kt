package model

import OutFile

data class Particle(
        val id: Int,
        val name: String,
        val image: String,
        val v: ArrayList<Float> = ArrayList<Float>().apply { add(0.0f); add(0.0f) },
        val vv: ArrayList<Float> = ArrayList<Float>().apply { add(0.0f); add(0.0f) },
        val color0: ArrayList<Float> = ArrayList<Float>().apply { add(1.0f); add(1.0f); add(1.0f) },
        val color1: ArrayList<Float> = ArrayList<Float>().apply { add(0.0f); add(0.0f); add(0.0f) },
        val scale0: ArrayList<Float> = ArrayList<Float>().apply { add(0.1f); add(0.1f); },
        val scale1: ArrayList<Float> = ArrayList<Float>().apply { add(0.1f); add(0.1f); },
        val rot: Float = 0.0f,
        val rotv: Float = 0.0f,
        val lifetime: Float = 1.0f
) {

    fun writeToFile(outFile: OutFile, game: Game) {
        outFile.writeMarker("PRT")
        outFile.writeMarker(id.toString())
        outFile.write2ByteInt(game.images.find{it.name == image}!!.id)
        outFile.write4ByteFloats(v)
        outFile.write4ByteFloats(vv)
        outFile.write4ByteFloats(color0)
        outFile.write4ByteFloats(color1)
        outFile.write4ByteFloats(scale0)
        outFile.write4ByteFloats(scale1)
        outFile.write4ByteFloats(ArrayList<Float>().apply { add(rot); add(rotv); add(lifetime); })
        println("wrote PRT " + name + " (" + id + ")")
    }
}

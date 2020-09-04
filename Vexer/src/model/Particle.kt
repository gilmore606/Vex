package model

import Writer

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

    fun writeToFile(writer: Writer, game: Game) {
        writer.writeMarker("PRT")
        writer.writeMarker(id.toString())
        writer.write2ByteInt(game.images.find{it.name == image}!!.id)
        writer.write4ByteFloats(v)
        writer.write4ByteFloats(vv)
        writer.write4ByteFloats(color0)
        writer.write4ByteFloats(color1)
        writer.write4ByteFloats(scale0)
        writer.write4ByteFloats(scale1)
        writer.write4ByteFloats(ArrayList<Float>().apply { add(rot); add(rotv); add(lifetime); })
        println("wrote PRT " + name + " (" + id + ")")
    }
}

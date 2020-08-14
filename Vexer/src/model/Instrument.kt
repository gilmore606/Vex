package model

data class Instrument(
    val name: String,
    val waves: ArrayList<String>,
    val adsr: ArrayList<Int>,
    val detune: Int = 0,
    val phase: Int = 0
)

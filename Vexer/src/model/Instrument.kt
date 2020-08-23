package model

data class Instrument(
    val name: String,
    val adsr: ArrayList<Int>,
    val wave: ArrayList<String>,
    val pulseWidth: ArrayList<Int> = ArrayList<Int>(2).apply { repeat(2) { add(0)} },
    val transpose: ArrayList<Int> = ArrayList<Int>(2).apply { repeat(2) { add(0)} },
    val detune: Int = 0,
    val phase: Int = 0,
    val oscMix: Int = 127,
    val distortion: Int = 0,
    val filter: String = "NO_FILTER",
    val filterF: Int = 0,
    val filterQ: Int = 0,
    val adsrFilter: ArrayList<Int> = ArrayList<Int>(4).apply { repeat(4) { add(0)} },
    val adsrFilterAmount: Int = 0,
    val echo: ArrayList<Int> = ArrayList<Int>(3).apply { repeat(3) { add(0)} },
    val lfoFreq: Int = 0,
    val lfoWave: String = "TRIANGLE",
    val lfoAmount: Int = 0,
    val lfoTarget: String = "M_NOTARGET",
    val adsrAux: ArrayList<Int> = ArrayList<Int>(4).apply { repeat(4) { add(0)} },
    val adsrAuxAmount: Int = 0,
    val adsrAuxTarget: String = "M_NOTARGET"
)

package model

import OutFile
import java.lang.RuntimeException

class OutInstrument() {
    var name: String = ""
    val outBytes: ArrayList<UByte> = ArrayList()

    fun compile(model: Instrument): OutInstrument {
        name = model.name

        // 2 bytes for osc waveforms
        model.waves.forEach { wave ->
            outBytes.add(when (wave) {
                "TRIANGLE" -> 0
                "SAWTOOTH" -> 1
                "PULSE" -> 2
                "SINE" -> 3
                "NOISE" -> 4
                else -> throw RuntimeException("unknown waveform")
            }.toUByte())
        }
        // 4 bytes for ADSR
        model.adsr.forEach { stageVal -> outBytes.add(stageVal.toUByte()) }

        // 2 bytes for detune + phase
        outBytes.add(model.detune.toUByte())
        outBytes.add(model.phase.toUByte())

        return this
    }

    fun writeToFile(outFile: OutFile, volume: Int?, pan: Int?) {
        if (volume == null || pan == null) throw RuntimeException("unspecified voice parameter")

        outFile.writeBytes(outBytes)

        // 2 bytes for volume and pan
        outFile.writeByte(volume.toUByte())
        outFile.writeByte(pan.toUByte())

        println("    wrote voice " + name)
    }
}

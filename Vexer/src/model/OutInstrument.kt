package model

import OutFile
import java.lang.RuntimeException

class OutInstrument() {
    var name: String = ""
    val outBytes: ArrayList<UByte> = ArrayList()

    fun compile(model: Instrument): OutInstrument {
        name = model.name

        // 1 byte marker (== 254)
        outBytes.add(254.toUByte())

        // 2 bytes for osc waveforms
        model.wave.forEach { outBytes.add(stringToWave(it)) }

        // 2 bytes for osc pwidth
        model.pulseWidth.forEach { outBytes.add(it.toUByte()) }

        // 2 bytes for osc transpose
        model.transpose.forEach { outBytes.add((it + 12).toUByte()) }

        // 4 bytes for ADSR
        model.adsr.forEach { stageVal -> outBytes.add(stageVal.toUByte()) }

        // 4 bytes for detune, phase, oscmix, distortion
        outBytes.add(model.detune.toUByte())
        outBytes.add(model.phase.toUByte())
        outBytes.add(model.oscMix.toUByte())
        outBytes.add(model.distortion.toUByte())

        // 3 bytes for filter type, freq, q
        outBytes.add(when (model.filter) {
                "LOWPASS_12" -> 1
                "HIGHPASS_12" -> 2
                "BANDPASS_12" -> 3
                "LOWPASS_24" -> 4
                "HIGHPASS_24" -> 5
                "BANDPASS_24" -> 6
            else -> 0
        }.toUByte())
        outBytes.add(model.filterF.toUByte())
        outBytes.add(model.filterQ.toUByte())

        // 4 bytes for ADSRfilter
        model.adsrFilter.forEach { stageVal -> outBytes.add(stageVal.toUByte()) }

        // 1 byte for ADSRfilter amount
        outBytes.add(model.adsrFilterAmount.toUByte())

        // 3 bytes for echo level, time, regen
        model.echo.forEach { echoVal -> outBytes.add(echoVal.toUByte()) }

        // 4 bytes for LFO freq, waveform, amount, target
        outBytes.add(model.lfoFreq.toUByte())
        outBytes.add(stringToWave(model.lfoWave))
        outBytes.add(model.lfoAmount.toUByte())
        outBytes.add(stringToTarget(model.lfoTarget))

        // 6 bytes for ADSRaux adsr, amount, target
        model.adsrAux.forEach { stageVal -> outBytes.add(stageVal.toUByte()) }
        outBytes.add((model.adsrAuxAmount + 127).toUByte())
        outBytes.add(stringToTarget(model.adsrAuxTarget))

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

    fun stringToWave(s: String): UByte = when (s) {
            "TRIANGLE" -> 0
            "SAWTOOTH" -> 1
            "PULSE" -> 2
            "SINE" -> 3
            "NOISE" -> 4
            else -> throw RuntimeException("unknown waveform")
        }.toUByte()

    fun stringToTarget(s: String): UByte = when (s) {
            "M_PITCH" -> 1
            "M_VOLUME" -> 2
            "M_PW1" -> 3
            "M_PW2" -> 4
            "M_OSCMIX" -> 5
            "M_DIST" -> 6
            "M_FILTER" -> 7
            "M_LFO_AMT" -> 8
            "M_LFO_FREQ" -> 9
            "M_PHASE" -> 10
            else -> 0
        }.toUByte()
}

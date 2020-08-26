package model

import OutFile

data class Control(
    val name: String,
    val key: String,
    val type: String = "button",
    val debounce: Int = 0
) {
    fun writeToFile(outFile: OutFile) {
        outFile.write2ByteInt(keyToGL(key))
        outFile.writeByte(when (type) {
            "switch" -> 0
            "button" -> 1
            else -> throw RuntimeException("unknown control type")
        }.toUByte())
        outFile.write2ByteInt(debounce)
        println("    control " + name + " " + key)
    }

    fun keyToGL(k: String): Int = when (k) {
        "esc" -> 256
        "escape" -> 256
        "backspace" -> 259
        "insert" -> 260
        "ins" -> 260
        "delete" -> 261
        "del" -> 261
        "right" -> 262
        "left" -> 263
        "down" -> 264
        "up" -> 265
        "pgup" -> 266
        "pgdn" -> 267
        "space" -> 32
        "spacebar" -> 32
        "'" -> 39
        "," -> 44
        "-" -> 45
        "." -> 46
        "/" -> 47
        "0" -> 48
        "1" -> 49
        "2" -> 50
        "3" -> 51
        "4" -> 52
        "5" -> 53
        "6" -> 54
        "7" -> 55
        "8" -> 56
        "9" -> 57
        ";" -> 59
        "=" -> 61
        "[" -> 91
        "\\" -> 92
        "]" -> 93
        "`" -> 96
        "enter" -> 257
        "tab" -> 258
        "f1" -> 290
        "f2" -> 291
        "f3" -> 292
        "f4" -> 293
        "f5" -> 294
        "f6" -> 295
        "f7" -> 296
        "f8" -> 297
        "f9" -> 298
        "f10" -> 299
        "f11" -> 300
        "f12" -> 301
        else -> {
            if (k.length > 1) throw RuntimeException("invalid key spec")
            (k.get(0).toInt() - 32)
        }
    }
}

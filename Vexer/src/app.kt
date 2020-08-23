import com.beust.klaxon.Klaxon
import compiler.Compiler
import model.Game
import model.Image
import model.OutInstrument
import java.io.File
import java.lang.RuntimeException

fun main() {
    println("SHALL WE COMPILE A GAME?")
    println("")

    val startTime = System.currentTimeMillis()
    val romName = "demogame"
    val fVerbose = true
    val sourceDir = romName

    // Parse the manifest

    val gameConfig: Game = Klaxon().parse<Game>(File(sourceDir + "/" + romName + ".json"))
        ?: throw RuntimeException("invalid json in manifest")

    println("Compiling " + gameConfig.title + " from " + sourceDir + "/" + romName + ".json")

    val outFile = OutFile(romName, gameConfig.title, gameConfig.aspectRatio)
    outFile.open()


    // Compile the main code block

    val compiler = Compiler(sourceDir + "/" + gameConfig.code, "MAIN", fVerbose)
    compiler.compile()
    compiler.writeToFile(outFile)


    // Write controls

    outFile.writeMarker("CTR")
    outFile.writeMarker("x")
    outFile.writeByte(gameConfig.controls.size.toUByte())
    gameConfig.controls.forEach { it.writeToFile(outFile) }
    println("  wrote controls")


    // Write images

    gameConfig.images.sortBy { it.id }
    gameConfig.images.forEach { image ->
        image.writeToFile(outFile)
    }


    // Write fonts

    gameConfig.fonts.forEach { font ->
        font.writeToFile(outFile)
    }

    // Compile instruments and map for insertion into songs

    val outInstruments = HashMap<String,OutInstrument>()
    gameConfig.instruments.forEach { instrument ->
        outInstruments[instrument.name] = OutInstrument().compile(instrument)
    }


    // Write songs, replacing voice names with compiled instruments

    gameConfig.songs.forEach { song ->
        val midi = MidiParser(song, sourceDir + "/" + song.file, song.voices.size, fVerbose)
        midi.parse()
        midi.writeToFile(outFile, song, outInstruments)
    }


    // Finish

    outFile.close()
    println("Compiled " + romName + ".vexo in " + (System.currentTimeMillis() - startTime) + "ms.")
}

import com.beust.klaxon.Klaxon
import compiler.Compiler
import model.Game
import model.OutInstrument
import java.io.File
import java.lang.RuntimeException

fun main() {
    println("SHALL WE COMPILE A GAME?")
    println("")

    val startTime = System.currentTimeMillis()
    val romName = "game"
    val fVerbose = false
    val sourceDir = romName

    val gameConfig: Game = Klaxon().parse<Game>(File(sourceDir + "/" + romName + ".json"))
        ?: throw RuntimeException("invalid json in manifest")

    println("Compiling " + gameConfig.title + " from " + sourceDir + "/" + romName + ".json")

    val outFile = OutFile(romName + ".vexo", gameConfig.title)
    outFile.open()

    val compiler = Compiler(sourceDir + "/" + gameConfig.code, "MAIN", fVerbose)
    compiler.compile()
    compiler.writeToFile(outFile)

    gameConfig.sprites.forEach { sprite ->

    }

    val outInstruments = HashMap<String,OutInstrument>()
    gameConfig.instruments.forEach { instrument ->
        outInstruments[instrument.name] = OutInstrument().compile(instrument)
    }

    gameConfig.songs.forEach { song ->
        val midi = MidiParser(sourceDir + "/" + song.file, fVerbose)
        midi.parse()
        midi.writeToFile(outFile, song, outInstruments)
    }

    outFile.close()
    println("Compiled " + romName + ".vexo in " + (System.currentTimeMillis() - startTime) + "ms.")
}

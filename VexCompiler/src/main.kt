import java.io.File
import java.lang.System.currentTimeMillis

class main {


    companion object {

        val fVerbose = false
        val romName = "game"

        var startTime: Long = 0

        @JvmStatic fun main(args: Array<String>) {

            println("SHALL WE COMPILE A GAME?")
            println("")
            startTime = currentTimeMillis()

            val outFile = OutFile(romName)
            outFile.open()

            val midi = MidiParser("archon.mid", fVerbose)
            midi.parse()
            midi.writeToFile(outFile)

            val compiler = Compiler("asteroids.vexc", fVerbose)
            compiler.compile()
            compiler.writeToFile(outFile)

            outFile.close()
            println("Compiled " + romName + ".vexo in " + (currentTimeMillis() - startTime) + "ms.")
        }
    }
}

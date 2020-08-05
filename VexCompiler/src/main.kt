import java.io.File

class main {

    companion object {
        @JvmStatic fun main(args: Array<String>) {
            println("hello world!  let's compile asteroids")

            // Open source file
            val inStream = File("data/asteroids.vexc").inputStream()

            // Stream source to lexer
            val lexer = Lexer()
            var nextChar = inStream.read()
            while (nextChar > -1) {
                lexer.process(nextChar.toChar())
                nextChar = inStream.read()
            }

            // Send lexer tokens to parser
            val parser = Parser(lexer.outBuffer)
            try {
                parser.parse()
                parser.dumpTree()
            } catch (e: Exception) {
                parser.dumpTree()
                throw e
            }
        }
    }
}

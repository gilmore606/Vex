import java.io.File

class main {

    companion object {
        @JvmStatic fun main(args: Array<String>) {
            println("SHALL WE COMPILE A GAME?")
            println("")

            val inStream = File("data/asteroids.vexc").inputStream()

            // Stream source to lexer
            val lexer = Lexer()
            var nextChar = inStream.read()
            while (nextChar > -1) {
                val c = nextChar.toChar()
                try {
                    lexer.process(c)
                } catch (e: LexException) {
                    println("Syntax error at line " + e.lexer.linePos + "," + e.lexer.charPos + ": ")
                    println("  " + e.m)
                }
                nextChar = inStream.read()
            }

            // Send lexer tokens to parser
            val parser = Parser(lexer.outBuffer)
            try {
                parser.parse()
            } catch (e: ParseException) {
                println("")
                println("Syntax error at line " + e.parser.linePos() + "," + e.parser.charPos() + ": ")
                println("  " + e.m)
            }
            parser.dumpTree()

            // Fill in AST with action tree

        }
    }
}

class LexException(val lexer: Lexer, val m: String): Exception(m)

class ParseException(val parser: Parser, val m: String): Exception(m)

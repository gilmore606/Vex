import java.io.File

class main {

    companion object {
        @JvmStatic fun main(args: Array<String>) {
            println("hello world!  let's compile asteroids")

            val inStream = File("data/asteroids.vexc").inputStream()

            // Stream source to lexer
            val lexer = Lexer()
            var nextChar = inStream.read()
            while (nextChar > -1) {
                val c = nextChar.toChar()
                try {
                    lexer.process(c)
                } catch (e: LexException) {
                    println("Tokenize error at line " + e.lexer.linePos + "," + e.lexer.charPos + ": ")
                    println("  " + e.m)
                }
                nextChar = inStream.read()
            }

            // Send lexer tokens to parser
            val parser = Parser(lexer.outBuffer)
            try {
                parser.parse()
                parser.dumpTree()
            } catch (e: ParseException) {
                parser.dumpTree()
                println("")
                println("Parse error at line " + e.parser.linePos() + "," + e.parser.charPos() + ": ")
                println("  " + e.m)
            }
        }
    }
}

class LexException(val lexer: Lexer, val m: String): Exception(m)

class ParseException(val parser: Parser, val m: String): Exception(m)

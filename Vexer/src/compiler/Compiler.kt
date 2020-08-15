package compiler

import OutFile
import java.io.File

class Compiler(val filepath: String, val chunkName: String, val fVerbose: Boolean) {

    val outBytes = ArrayList<UByte>()

    fun compile() {

        val inStream = File(filepath).inputStream();

        // compiler.Lexer breaks source text into tokens
        val lexer = Lexer(fVerbose)
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

        // Let peeper find method names
        val peeper = Peeper(lexer.outBuffer, fVerbose)
        peeper.peep()

        // Send lexer tokens to parser
        val parser = Parser(lexer.outBuffer, peeper.methods, fVerbose)
        try {
            parser.parse()
        } catch (e: ParseException) {
            println("")
            println("Syntax error at line " + e.parser.linePos() + "," + e.parser.charPos() + ": ")
            println("  " + e.m)
        }
        if (fVerbose) parser.dumpTree()

        // Fill in symbols and types
        val meaner = Meaner(parser.ast)
    }

    fun writeToFile(outFile: OutFile) {
        outFile.writeMarker("COD")
        outFile.writeMarker(chunkName)
        println("  wrote code " + chunkName)
    }
}

class LexException(val lexer: Lexer, val m: String): Exception(m)

class ParseException(val parser: Parser, val m: String): Exception(m)

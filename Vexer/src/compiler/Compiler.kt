package compiler

import OutFile
import java.io.File

class Compiler(val filepath: String, val chunkName: String, val fVerbose: Boolean) {

    val outBytes = ArrayList<UByte>()

    lateinit var lexer: Lexer
    lateinit var peeper: Peeper
    lateinit var parser: Parser
    lateinit var salter: Salter
    lateinit var meaner: Meaner
    lateinit var coder: Coder

    fun compile() {

        val inStream = File(filepath).inputStream();

        // Break source text into tokens
        lexer = Lexer(fVerbose)
        var nextChar = inStream.read()
        while (nextChar > -1) {
            val c = nextChar.toChar()
            try {
                lexer.process(c)
            } catch (e: LexException) {
                println("Syntax error at line " + e.lexer.linePos + "," + e.lexer.charPos + ": ")
                println("  " + e.m)
                throw RuntimeException(e.m)
            }
            nextChar = inStream.read()
        }

        // Peep for method names
        peeper = Peeper(lexer.outBuffer, fVerbose)
        peeper.peep()

        // Send lexer tokens to parser
        parser = Parser(lexer.outBuffer, peeper.methods, fVerbose)
        try {
            parser.parse()
        } catch (e: ParseException) {
            println("")
            println("Syntax error at line " + e.parser.linePos() + "," + e.parser.charPos() + ": ")
            println("  " + e.m)
            throw RuntimeException(e.m)
        }
        if (fVerbose) parser.dumpTree()

        try {

            // Desugar syntax
            salter = Salter(parser.ast)
            salter.salt()

            // Fill in symbols and types
            meaner = Meaner(parser.ast, fVerbose)
            meaner.mean()

            // Generate code
            coder = Coder(parser.ast)
            coder.generate()

        } catch (e: CompileException) {
            println("")
            println("Compile error at line ???: ")
            println("  " + e.m)
            throw RuntimeException(e.m)
        }
    }

    fun writeToFile(outFile: OutFile) {
        outFile.writeMarker("COD")
        outFile.writeMarker(chunkName)

        coder.writeToFile(outFile, meaner.constants, meaner.variables)

        println("  wrote code " + chunkName)
    }
}

class LexException(val lexer: Lexer, val m: String): Exception(m)

class ParseException(val parser: Parser, val m: String): Exception(m)

class CompileException(val m: String): Exception(m)

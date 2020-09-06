package compiler

import Writer
import compiler.nodes.Node
import model.Game
import java.io.File
import kotlin.system.exitProcess

class Compiler(val filepath: String, val chunkName: String, val fVerbose: Boolean) {

    val outBytes = ArrayList<UByte>()

    lateinit var lexer: Lexer
    lateinit var peeper: Peeper
    lateinit var parser: Parser
    lateinit var salter: Salter
    lateinit var meaner: Meaner
    lateinit var coder: Coder

    fun compile(config: Game) {

        val inStream = File(filepath).inputStream();

        // Break source text into tokens
        lexer = Lexer(fVerbose)
        var nextChar = inStream.read()
        while (nextChar > -1) {
            val c = nextChar.toChar()
            try {
                lexer.process(c)
            } catch (e: LexException) {
                reportError("Token", e.lexer.linePos, e.lexer.charPos, e.m)
            }
            nextChar = inStream.read()
        }

        if (fVerbose) {
            lexer.outBuffer.forEach { print(it.toString() + " ") }
        }

        // Peep for method names
        peeper = Peeper(lexer.outBuffer, fVerbose)
        peeper.peep()

        // Send lexer tokens to parser
        parser = Parser(lexer.outBuffer, peeper.methods, fVerbose)
        try {
            parser.parse()
        } catch (e: ParseException) {
            reportError("Syntax", e.parser.linePos(), e.parser.charPos(), e.m)
        }

        try {

            // Desugar syntax
            salter = Salter(parser.ast)
            salter.salt()

            // Fill in symbols and types
            meaner = Meaner(parser.ast, fVerbose)
            meaner.mean(config)

            // Generate code
            coder = Coder(parser.ast)
            coder.generate()

        } catch (e: CompileException) {
            reportError("Compile", e.node.linePos, e.node.charPos, e.m)
        }
    }

    fun writeToFile(writer: Writer) {
        writer.writeMarker("COD")
        writer.writeMarker(chunkName)

        coder.writeToFile(writer, meaner.constants, meaner.variables)

        println("  wrote code " + chunkName)
    }

    fun reportError(type: String, linePos: Int, charPos: Int, message: String) {
        println("\n\n" + type + " error at " + linePos + "," + charPos + ":")
        lexer.dumpCodeAt(linePos, charPos)
        println("  " + message)
        exitProcess(0)
    }
}

class LexException(val lexer: Lexer, val m: String): Exception(m)

class ParseException(val parser: Parser, val m: String): Exception(m)

class CompileException(val node: Node, val m: String): Exception(m)

package compiler

import OutFile
import compiler.Opcode.*
import compiler.ValueType.*
import compiler.Value

class Coder(
        val ast: ArrayList<Node>
) {

    val outBytes = ArrayList<UByte>()

    fun generate() {

        ast[0].generate(this)

        code(OP_DEBUG)
        code(OP_EXIT)
    }

    fun code(op: Opcode) {
        outBytes.add(op.ordinal.toUByte())
    }

    fun arg2i(iarg: Int) {
        val b0 = (iarg % 256)
        val b1 = (iarg - b0) / 256
        outBytes.add(b0.toUByte())
        outBytes.add(b1.toUByte())
    }

    fun writeToFile(outFile: OutFile, constants: ArrayList<Value>) {

        // Write constants
        outFile.writeMarker("CONST")
        // 2 bytes for constant count
        outFile.write2ByteInt(constants.size)
        constants.forEach {
            outFile.writeValue(it)
        }

        // Write code
        outFile.writeMarker("BC")
        // 3 bytes for bytecode count
        outFile.write3ByteInt(outBytes.size)
        outFile.writeBytes(outBytes)
    }
}

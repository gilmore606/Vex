package compiler

import OutFile
import compiler.Opcode.*
import compiler.nodes.Node

class Coder(
        val ast: ArrayList<Node>
) {

    val outBytes = ArrayList<UByte>()
    val entries = HashMap<String,Int>()
    val jumps = ArrayList<Int>()
    val variables = ArrayList<String>()

    fun generate() {

        ast[0].code(this)

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

    // Nodes call this back during codegen to register entry / jump points
    fun addEntryPoint(name: String) {
        val addr = outBytes.size
        entries[name] = addr
    }
    fun addJumpPoint(): Int {
        val addr = outBytes.size
        jumps.add(addr)
        return jumps.size - 1
    }
    fun variableToID(name: String): Int {
        // TODO: pass in scope, keep separate scope lists
        val i = variables.indexOf(name)
        if (i >= 0) return i
        variables.add(name)
        return variables.lastIndex
    }

    fun writeToFile(outFile: OutFile, constants: ArrayList<Value>) {

        // Write constants
        outFile.writeMarker("CONST")
        // 2 bytes for constant count
        outFile.write2ByteInt(constants.size)
        constants.forEach {
            outFile.writeValue(it)
            println("  wrote constant " + it.toString())
        }

        // Write entries
        outFile.writeMarker("ENTRY")
        // 1 byte for entry count
        outFile.writeByte(entries.size.toUByte())
        entries.keys.forEach {
            outFile.writeString(it)
            outFile.write3ByteInt(entries[it]!!)
        }

        // Write jumps
        outFile.writeMarker("JUMP")
        // 2 bytes for jump count
        outFile.write2ByteInt(jumps.size)
        jumps.forEach {
            outFile.write3ByteInt(it)
        }

        // Write code
        outFile.writeMarker("BC")
        // 3 bytes for bytecode count
        outFile.write3ByteInt(outBytes.size)
        outFile.writeBytes(outBytes)
    }
}

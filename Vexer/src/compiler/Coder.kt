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

    val futureJumps = HashMap<String,ArrayList<Int>>()

    fun generate() {

        ast[0].code(this)

    }

    fun code(op: Opcode, arg1: Int? = null, arg2: Int? = null, arg3: Int? = null) {
        outBytes.add(op.ordinal.toUByte())
        arg1?.also { arg2i(it) }
        arg2?.also { arg2i(it) }
        arg3?.also { arg2i(it) }
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
    // Write placeholder jump ID for a jump we'll mark in the future
    fun jumpFuture(name: String) {
        if (futureJumps.containsKey(name)) {
            futureJumps[name]!!.add(outBytes.size)
        } else {
            futureJumps[name] = ArrayList<Int>().apply { add(outBytes.size) }
        }
        arg2i(0)  // placeholder
    }
    // Resolve jump ID and write into past placeholders
    fun reachFuture(name: String) {
        val jumpID = addJumpPoint()
        val b0 = (jumpID % 256)
        val b1 = ((jumpID - b0) / 256)
        futureJumps[name]!!.forEach {
            outBytes[it] = b0.toUByte()
            outBytes[it+1] = b1.toUByte()
        }
        futureJumps.remove(name)
    }

    fun writeToFile(outFile: OutFile, constants: ArrayList<Value>, variables: ArrayList<Variable>) {

        // Write constants
        outFile.writeMarker("CONST")
        // 2 bytes for constant count
        outFile.write2ByteInt(constants.size)
        constants.forEachIndexed { i, it ->
            outFile.writeValue(it)
            println("  constant " + i.toString() + ":  " + it.toString())
        }

        // Write variables
        outFile.writeMarker("VARS")
        // 2 bytes for var count
        outFile.write2ByteInt(variables.size)

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
        jumps.forEachIndexed { i, it ->
            outFile.write3ByteInt(it)
            println("  jump " + i.toString() + ":  " + it.toString())
        }

        // Write code
        outFile.writeMarker("BC")
        // 3 bytes for bytecode count
        outFile.write3ByteInt(outBytes.size)
        outFile.writeBytes(outBytes)
        disassemble(constants, variables)
    }

    fun disassemble(constants: ArrayList<Value>, variables: ArrayList<Variable>) {
        var ip = 0
        while (ip < outBytes.size) {
            var out = ""
            val ji = jumps.indexOf(ip)
            if (ji >= 0) out = "j" + ji.toString() + " " else out = "   "
            out += ip.toString() + ":  "

            val op: Opcode = Opcode.values()[outBytes[ip].toInt()]
            out += op.toString()
            if (op==OP_JUMP || op==OP_IF || op==OP_VAR || op==OP_CONST || op==OP_INCVAR || op==OP_DECVAR || op==OP_SETVAR
                    || op==OP_SETSYS || op==OP_JUMPZ || op==OP_JUMPNZ || op==OP_FUN || op==OP_SFUN || op==OP_SMETH) {
                val a1 = outBytes[ip+1].toInt() + outBytes[ip+2].toInt()*256
                if (op==OP_JUMP || op==OP_IF) {
                    out += " j" + a1.toString() + " [" + jumps[a1].toString() + "]"
                } else if (op==OP_CONST) {
                    out += " c" + a1.toString() + " [" + constants[a1].toString() + "]"
                } else if (op==OP_FUN || op==OP_SFUN || op==OP_SMETH) {
                    out += " f" + a1.toString()
                } else {
                    out += " v" + a1.toString() + " [" + variables[a1].name + "]"
                }
                ip += 2
                if (op==OP_JUMPZ || op==OP_JUMPNZ) {
                    val a2 = outBytes[ip+1].toInt() + outBytes[ip+2].toInt()*256
                    out += " j" + a2.toString() + " [" + jumps[a2].toString() + "]"
                    ip += 2
                }
            }

            println(out)
            ip++
        }
    }
}

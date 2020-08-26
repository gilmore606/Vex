package compiler.nodes

import compiler.Coder
import compiler.CompileException
import compiler.Opcode.*
import compiler.Parser
import compiler.ValueType
import compiler.ValueType.*


open class Node {

    var linePos = 0
    var charPos = 0

    fun tag(p: Parser) {
        this.linePos = p.linePos()
        this.charPos = p.charPos()
    }
    open fun print(lvl: Int) {
        println("  ".repeat(lvl) + this.toString())
        printMine(lvl + 1)
    }
    open fun printMine(lvl: Int) { }
    open fun code(coder: Coder) { }
    open fun flatten(): ArrayList<Node> {
        val l = ArrayList<Node>();
        l.add(this)
        return l
    }
    open fun setType(): ValueType {
        flatten().forEach { it.setType() }
        return VAL_NIL
    }

    // Nodes

    // Values

    abstract class N_EXPRESSION: Node() {
        var type: ValueType? = null
        override fun setType(): ValueType { return VAL_NIL }
    }




    // Operators

    abstract class N_BINOP(val arg1: N_EXPRESSION, val arg2: N_EXPRESSION): N_EXPRESSION() {
        override fun printMine(lvl: Int) {
            arg1.print(lvl)
            arg2.print(lvl)
        }
        override fun flatten(): ArrayList<Node> {
            val l = super.flatten()
            l.addAll(arg1.flatten())
            l.addAll(arg2.flatten())
            return l
        }
    }



    // Statements

    abstract class N_STATEMENT: Node()

    class N_ASSIGN(val variable: N_VALUE, val value: N_EXPRESSION): N_STATEMENT() {
        override fun printMine(lvl: Int) {
            variable.print(lvl)
            value.print(lvl)
        }
        override fun setType(): ValueType {
            super.setType()
            if (variable.type != value.type) throw CompileException("type error: assigned " + value.type.toString() + " to " + variable.type.toString() + " var")
            return VAL_NIL;
        }
    }

    class N_REPEAT(val count: N_EXPRESSION, val code: N_CODEBLOCK): N_STATEMENT() {
        override fun printMine(lvl: Int) {
            count.print(lvl)
            code.print(lvl)
        }
        override fun setType(): ValueType {
            super.setType()
            if (count.type != VAL_INT) throw CompileException("type error: repeat statement expects int count")
            return VAL_NIL;
        }
    }

    class N_EACH(val iterator: N_VARIABLE, val code: N_CODEBLOCK): N_STATEMENT() {
        override fun printMine(lvl: Int) {
            iterator.print(lvl)
            code.print(lvl)
        }
    }

    class N_IFELSE(val condition: N_EXPRESSION, val ifblock: N_CODEBLOCK, val elseblock: N_CODEBLOCK): N_STATEMENT() {
        override fun printMine(lvl: Int) {
            condition.print(lvl)
            ifblock.print(lvl)
            elseblock.print(lvl)
        }
        override fun setType(): ValueType {
            super.setType()
            if (condition.type != VAL_BOOL) throw CompileException("type error: if statement expects boolean expr")
            return VAL_NIL;
        }
    }

    class N_SOUND(val soundid: N_EXPRESSION): N_STATEMENT() {
        override fun printMine(lvl: Int) {
            soundid.print(lvl)
        }
        override fun flatten(): ArrayList<Node> {
            val l = super.flatten()
            l.addAll(soundid.flatten())
            return l
        }
        override fun setType(): ValueType {
            super.setType()
            if (soundid.type != VAL_INT) throw CompileException("type error: sound statement expects int id")
            return VAL_NIL;
        }
        override fun code(coder: Coder) {
            soundid.code(coder)
            coder.code(OP_SONG)
        }
    }


    abstract class N_BLOCK: Node()

    class N_SETTINGS(val assigns: List<N_ASSIGN>): N_BLOCK() {
        override fun printMine(lvl: Int) {
            assigns.forEach { it.print(lvl) }
        }
    }
    class N_CODEBLOCK(val statements: List<N_STATEMENT>): N_BLOCK() {
        override fun printMine(lvl: Int) {
            statements.forEach { it.print(lvl) }
        }
        override fun flatten(): ArrayList<Node> {
            val l = super.flatten()
            statements.forEach { l.addAll(it.flatten()) }
            return l
        }
        override fun code(coder: Coder) {
            statements.forEach { it.code(coder) }
        }
    }
    class N_FUNCTION(val name: String, val code: N_CODEBLOCK): N_BLOCK() {
        override fun toString() = super.toString() + " (" + name + ")"
        override fun printMine(lvl: Int) {
            code.print(lvl)
        }
        override fun flatten(): ArrayList<Node> {
            val l = super.flatten()
            l.addAll(code.flatten())
            return l
        }
        override fun code(coder: Coder) {
            coder.addEntryPoint(name)
            code.code(coder)
            coder.code(OP_EXIT)
        }
    }

    class N_PROGRAM(val blocks: List<N_BLOCK>): Node() {
        override fun printMine(lvl: Int) {
            blocks.forEach { it.print(lvl) }
        }
        override fun flatten(): ArrayList<Node> {
            val l = super.flatten()
            blocks.forEach { l.addAll(it.flatten()) }
            return l
        }
        override fun code(coder: Coder) {
            blocks.forEach { it.code(coder) }
        }
    }
}

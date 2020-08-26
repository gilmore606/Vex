package compiler

import compiler.Opcode.*


sealed class Node {

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
    open fun generate(coder: Coder) { }
    open fun flatten(): ArrayList<Node> {
        val l = ArrayList<Node>();
        l.add(this)
        return l
    }

    // Nodes

    // Values

    abstract class EXPRESSION: Node()

    abstract class VALUE: EXPRESSION()

    abstract class LITERAL: VALUE() {
        var constID: Int = 0
        override fun generate(coder: Coder) {
            coder.code(OP_CONST)
            coder.arg2i(constID)
        }
    }

    abstract class VARREF: VALUE()

    class BOOLEAN(val value: Boolean): LITERAL() {
        override fun toString() = if (value) "TRUE" else "FALSE"
    }
    class INTEGER(val value: Int): LITERAL() {
        override fun toString() = super.toString() + " (" + value.toString() + ")"
    }
    class FLOAT(val value: Float): LITERAL() {
        override fun toString() = super.toString() + " (" + value.toString() + ")"
    }
    class STRING(val value: String): LITERAL() {
        override fun toString() = super.toString() + " (" + value + ")"
    }

    class VARIABLE(val name: String): VARREF() {
        override fun toString() = super.toString() + " (" + name + ")"
    }
    class PROPREF(val objectname: String, val propname: String): VARREF() {
        override fun toString() = super.toString() + " (" + objectname + "." + propname + ")"
    }

    class CALL(val name: String, val args: List<EXPRESSION>): VALUE() {
        override fun toString() = super.toString() + " (" + name + ")"
        override fun printMine(lvl: Int) {
            args.forEach { it.print(lvl) }
        }
    }


    // Operators

    abstract class UNOP(val arg: EXPRESSION): EXPRESSION() {
        override fun printMine(lvl: Int) {
            arg.print(lvl)
        }
    }
    class NEGATE(arg: EXPRESSION): UNOP(arg)
    class INVERSE(arg: EXPRESSION): UNOP(arg)

    abstract class BINOP(val arg1: EXPRESSION, val arg2: EXPRESSION): EXPRESSION() {
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
    class ADD(arg1: EXPRESSION, arg2: EXPRESSION): BINOP(arg1, arg2)
    class SUBTRACT(arg1: EXPRESSION, arg2: EXPRESSION): BINOP(arg1, arg2)
    class MULTIPLY(arg1: EXPRESSION, arg2: EXPRESSION): BINOP(arg1, arg2)
    class DIVIDE(arg1: EXPRESSION, arg2: EXPRESSION): BINOP(arg1, arg2)
    class POWER(arg1: EXPRESSION, arg2: EXPRESSION): BINOP(arg1, arg2)
    class LOGIC_OR(arg1: EXPRESSION, arg2: EXPRESSION): BINOP(arg1, arg2)
    class LOGIC_AND(arg1: EXPRESSION, arg2: EXPRESSION): BINOP(arg1, arg2)
    class LOGIC_EQUAL(arg1: EXPRESSION, arg2: EXPRESSION): BINOP(arg1, arg2)
    class LOGIC_NOTEQUAL(arg1: EXPRESSION, arg2: EXPRESSION): BINOP(arg1, arg2)
    class LOGIC_GREATER(arg1: EXPRESSION, arg2: EXPRESSION): BINOP(arg1, arg2)
    class LOGIC_LESS(arg1: EXPRESSION, arg2: EXPRESSION): BINOP(arg1, arg2)
    class LOGIC_GREATER_EQUAL(arg1: EXPRESSION, arg2: EXPRESSION): BINOP(arg1, arg2)
    class LOGIC_LESS_EQUAL(arg1: EXPRESSION, arg2: EXPRESSION): BINOP(arg1, arg2)


    // Statements

    abstract class STATEMENT: Node()

    class ASSIGN(val variable: VALUE, val value: EXPRESSION): STATEMENT() {
        override fun printMine(lvl: Int) {
            variable.print(lvl)
            value.print(lvl)
        }
    }

    class REPEAT(val count: EXPRESSION, val code: CODEBLOCK): STATEMENT() {
        override fun printMine(lvl: Int) {
            count.print(lvl)
            code.print(lvl)
        }
    }

    class EACH(val iterator: VARIABLE, val code: CODEBLOCK): STATEMENT() {
        override fun printMine(lvl: Int) {
            iterator.print(lvl)
            code.print(lvl)
        }
    }

    class IFELSE(val condition: EXPRESSION, val ifblock: CODEBLOCK, val elseblock: CODEBLOCK): STATEMENT() {
        override fun printMine(lvl: Int) {
            condition.print(lvl)
            ifblock.print(lvl)
            elseblock.print(lvl)
        }
    }

    class SOUND(val soundid: EXPRESSION): STATEMENT() {
        override fun printMine(lvl: Int) {
            soundid.print(lvl)
        }
        override fun flatten(): ArrayList<Node> {
            val l = super.flatten()
            l.addAll(soundid.flatten())
            return l
        }
        override fun generate(coder: Coder) {
            soundid.generate(coder)
            coder.code(OP_SONG)
        }
    }


    abstract class BLOCK: Node()

    class SETTINGS(val assigns: List<ASSIGN>): BLOCK() {
        override fun printMine(lvl: Int) {
            assigns.forEach { it.print(lvl) }
        }
    }
    class CODEBLOCK(val statements: List<STATEMENT>): BLOCK() {
        override fun printMine(lvl: Int) {
            statements.forEach { it.print(lvl) }
        }
        override fun flatten(): ArrayList<Node> {
            val l = super.flatten()
            statements.forEach { l.addAll(it.flatten()) }
            return l
        }
        override fun generate(coder: Coder) {
            statements.forEach { it.generate(coder) }
        }
    }
    class FUNCTION(val name: String, val code: CODEBLOCK): BLOCK() {
        override fun toString() = super.toString() + " (" + name + ")"
        override fun printMine(lvl: Int) {
            code.print(lvl)
        }
        override fun flatten(): ArrayList<Node> {
            val l = super.flatten()
            l.addAll(code.flatten())
            return l
        }
        override fun generate(coder: Coder) {
            code.generate(coder)
        }
    }

    class PROGRAM(val blocks: List<BLOCK>): Node() {
        override fun printMine(lvl: Int) {
            blocks.forEach { it.print(lvl) }
        }
        override fun flatten(): ArrayList<Node> {
            val l = super.flatten()
            blocks.forEach { l.addAll(it.flatten()) }
            return l
        }
        override fun generate(coder: Coder) {
            blocks.forEach { it.generate(coder) }
        }
    }
}

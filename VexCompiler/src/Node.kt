sealed class Node {

    var linePos = 0
    var charPos = 0


    open fun print(lvl: Int) {
        println("  ".repeat(lvl) + this.toString())
        printMine(lvl + 1)
    }
    open fun printMine(lvl: Int) { }

    abstract class EXPRESSION: Node()
    abstract class VALUE: EXPRESSION()
    abstract class LITERAL: VALUE()

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
    class VARIABLE(val name: String): VALUE() {
        override fun toString() = super.toString() + " (" + name + ")"
    }
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
    }
    class ADD(arg1: EXPRESSION, arg2: EXPRESSION): BINOP(arg1, arg2)
    class SUBTRACT(arg1: EXPRESSION, arg2: EXPRESSION): BINOP(arg1, arg2)
    class MULTIPLY(arg1: EXPRESSION, arg2: EXPRESSION): BINOP(arg1, arg2)
    class DIVIDE(arg1: EXPRESSION, arg2: EXPRESSION): BINOP(arg1, arg2)
    class POWER(arg1: EXPRESSION, arg2: EXPRESSION): BINOP(arg1, arg2)

    abstract class STATEMENT: Node()

    class ASSIGN(val variable: VARIABLE, val value: EXPRESSION): STATEMENT() {
        override fun printMine(lvl: Int) {
            variable.print(lvl)
            value.print(lvl)
        }
    }

    class CONTROLDEF(val name: String, val params: List<CONTROLPARAM>): STATEMENT() {
        override fun toString() = super.toString() + " (" + name + ")"
        override fun printMine(lvl: Int) {
            params.forEach { it.print(lvl) }
        }
    }
    abstract class CONTROLPARAM: Node()
    class CONTROL_SWITCH: CONTROLPARAM()
    class CONTROL_BUTTON: CONTROLPARAM()
    class CONTROL_DEBOUNCE(val ms: Int): CONTROLPARAM() {
        override fun toString() = super.toString() + " (" + ms.toString() + "ms)"
    }

    abstract class BLOCK: Node()

    class SETTINGS(val assigns: List<ASSIGN>): BLOCK() {
        override fun printMine(lvl: Int) {
            assigns.forEach { it.print(lvl) }
        }
    }
    class CONTROLS(val controls: List<CONTROLDEF>): BLOCK() {
        override fun printMine(lvl: Int) {
            controls.forEach { it.print(lvl) }
        }
    }
    class CODEBLOCK(val name: String, val statements: List<STATEMENT>): BLOCK() {
        override fun toString() = super.toString() + " (" + name + ")"
        override fun printMine(lvl: Int) {
            statements.forEach { it.print(lvl) }
        }
    }

    class PROGRAM(val blocks: List<BLOCK>): Node() {
        override fun printMine(lvl: Int) {
            blocks.forEach { it.print(lvl) }
        }
    }
}

package compiler.nodes

import compiler.Coder
import compiler.Opcode

abstract class N_VALUE: Node.N_EXPRESSION()

abstract class N_LITERAL: N_VALUE() {
    var constID: Int = 0
    override fun code(coder: Coder) {
        coder.code(Opcode.OP_CONST)
        coder.arg2i(constID)
    }
}

abstract class N_VARREF: N_VALUE()

class N_BOOLEAN(val value: Boolean): N_LITERAL() {
    override fun toString() = if (value) "TRUE" else "FALSE"
}
class N_INTEGER(val value: Int): N_LITERAL() {
    override fun toString() = super.toString() + " (" + value.toString() + ")"
}
class N_FLOAT(val value: Float): N_LITERAL() {
    override fun toString() = super.toString() + " (" + value.toString() + ")"
}
class N_STRING(val value: String): N_LITERAL() {
    override fun toString() = super.toString() + " (" + value + ")"
}
class N_VECTOR(val v1: Float, val v2: Float): N_LITERAL() {
    override fun toString() = super.toString() + " (" + v1 + "," + v2 + ")"
}

class N_VARIABLE(val name: String): N_VARREF() {
    var varID: Int = 0
    override fun toString() = super.toString() + " (" + name + ")"
    override fun code(coder: Coder) {
        coder.code(Opcode.OP_VAR)
        coder.arg2i(varID)
    }
}

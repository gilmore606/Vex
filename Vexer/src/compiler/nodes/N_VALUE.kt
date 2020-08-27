package compiler.nodes

import compiler.Coder
import compiler.Opcode.*

abstract class N_VALUE: Node.N_EXPRESSION()

abstract class N_LITERAL: N_VALUE() {
    var constID: Int = 0
    override fun code(coder: Coder) {
        coder.code(OP_CONST)
        coder.arg2i(constID)
    }
}


class N_BOOLEAN(val value: Boolean): N_LITERAL() {
    override fun toString() = if (value) "TRUE" else "FALSE"
}
class N_INTEGER(val value: Int): N_LITERAL() {
    override fun toString() = "INT (" + value.toString() + ")"
}
class N_FLOAT(val value: Float): N_LITERAL() {
    override fun toString() = "FLOAT (" + value.toString() + ")"
}
class N_STRING(val value: String): N_LITERAL() {
    override fun toString() = "STRING (" + value + ")"
}
class N_VECTOR(val v1: Float, val v2: Float): N_LITERAL() {
    override fun toString() = "VECTOR (" + v1 + "," + v2 + ")"
}

abstract class N_VARREF(): N_VALUE() {
    abstract fun codeSet(coder: Coder)
}

class N_VARIABLE(val name: String): N_VARREF() {
    var varID: Int = 0
    override fun toString() = "VARIABLE (" + name + "):" + varID
    override fun scopeVars(scope: Node, coder: Coder) {
        varID = coder.variableToID(name)
    }
    override fun code(coder: Coder) {
        coder.code(OP_VAR)
        coder.arg2i(varID)
    }
    override fun codeSet(coder: Coder) {
        coder.code(OP_SETVAR)
        coder.arg2i(varID)
    }
}

package compiler.nodes

import compiler.*
import compiler.Opcode.*
import compiler.ValueType.*
import compiler.nodes.Node.*

abstract class N_BINOP(val arg1: N_EXPRESSION, val arg2: N_EXPRESSION): N_EXPRESSION() {
    override fun kids(): NODES = super.kids().apply { add(arg1); add(arg2) }
}

class N_ROTATE(arg1: N_EXPRESSION, arg2: N_EXPRESSION): N_BINOP(arg1, arg2) {
    override fun setType(meaner: Meaner): Boolean {
        val oldtype = this.type
        this.type = VAL_VECTOR
        return this.type == oldtype
    }

    override fun checkTypeSane() {
        if ((arg1.type == VAL_VECTOR) && (arg2.type == VAL_FLOAT)) return
        if ((arg1.type == VAL_FLOAT) && (arg2.type == VAL_FLOAT)) return
        throw CompileException(this, "type error: can only rotate vector by float, or float by float")
    }

    override fun code(coder: Coder) {
        arg1.code(coder)
        arg2.code(coder)
        if (arg1.type == VAL_VECTOR) coder.code(OP_ROTVF)
        else coder.code(OP_ROTFFV)
    }
}

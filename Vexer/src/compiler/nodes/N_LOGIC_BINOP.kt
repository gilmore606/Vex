package compiler.nodes

import compiler.Coder
import compiler.CompileException
import compiler.Meaner
import compiler.Opcode.*
import compiler.ValueType.*

abstract class N_LOGIC_BINOP(arg1: N_EXPRESSION, arg2: N_EXPRESSION) : Node.N_BINOP(arg1, arg2) {
    override fun setType(meaner: Meaner): Boolean {
        if (this.type != VAL_BOOL) {
            this.type = VAL_BOOL
            return false
        }
        return true
    }

    override fun checkTypeSane() {
        if (arg1.type != VAL_BOOL)
            throw CompileException(this, "type error: logic op only accepts bool, not " + arg1.type.toString())
        if (arg2.type != VAL_BOOL)
            throw CompileException(this, "type error: logic op only accepts bool, not " + arg2.type.toString())
    }

    override fun code(coder: Coder) {
        arg2.code(coder)
        arg1.code(coder)
    }
}

class N_LOGIC_OR(arg1: N_EXPRESSION, arg2: N_EXPRESSION): N_LOGIC_BINOP(arg1, arg2) {
    override fun code(coder: Coder) {
        super.code(coder)
        coder.code(OP_OR)
    }
}

class N_LOGIC_AND(arg1: N_EXPRESSION, arg2: N_EXPRESSION): N_LOGIC_BINOP(arg1, arg2) {
    override fun code(coder: Coder) {
        super.code(coder)
        coder.code(OP_AND)
    }
}

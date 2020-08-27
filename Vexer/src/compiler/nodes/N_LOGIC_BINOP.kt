package compiler.nodes

import compiler.Coder
import compiler.CompileException
import compiler.Opcode.*
import compiler.ValueType.*

abstract class N_LOGIC_BINOP(arg1: N_EXPRESSION, arg2: N_EXPRESSION) : Node.N_BINOP(arg1, arg2) {
    override fun setType() {
        super.setType()
        if (arg1.type != VAL_BOOL || arg2.type != VAL_BOOL) throw CompileException("type error: logic op accepts only booleans")
        this.type = VAL_BOOL
    }
    override fun code(coder: Coder) {
        arg1.code(coder)
        arg2.code(coder)
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

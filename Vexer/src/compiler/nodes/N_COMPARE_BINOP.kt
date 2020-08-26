package compiler.nodes

import compiler.Coder
import compiler.Opcode
import compiler.ValueType

abstract class N_COMPARE_BINOP(arg1: N_EXPRESSION, arg2: N_EXPRESSION): Node.N_BINOP(arg1, arg2) {
    override fun setType(): ValueType {
        super.setType()
        this.type = ValueType.VAL_BOOL
        return this.type!!
    }
    abstract fun codeFloat(coder: Coder)
    abstract fun codeInt(coder: Coder)
    override fun code(coder: Coder) {
        arg1.code(coder)
        var a1type = arg1.type
        if (arg1.type == ValueType.VAL_NIL) { coder.code(Opcode.OP_N2I); a1type = ValueType.VAL_INT }
        if (arg1.type == ValueType.VAL_BOOL) { coder.code(Opcode.OP_B2I); a1type = ValueType.VAL_INT }
        if (arg1.type == ValueType.VAL_VECTOR) { coder.code(Opcode.OP_V2F); a1type = ValueType.VAL_FLOAT }
        when (arg2.type) {
            ValueType.VAL_NIL -> {
                arg2.code(coder)
                coder.code(Opcode.OP_N2I)
                if (a1type == ValueType.VAL_FLOAT) coder.code(Opcode.OP_I2F)
            }
            ValueType.VAL_BOOL -> {
                arg2.code(coder)
                coder.code(Opcode.OP_B2I)
                if (a1type == ValueType.VAL_FLOAT) coder.code(Opcode.OP_I2F)
            }
            ValueType.VAL_FLOAT -> {
                if (a1type == ValueType.VAL_INT) coder.code(Opcode.OP_I2F)
                arg2.code(coder)
            }
            ValueType.VAL_VECTOR -> {
                if (a1type == ValueType.VAL_INT) coder.code(Opcode.OP_I2F)
                arg2.code(coder)
                coder.code(Opcode.OP_V2F)
            }
            ValueType.VAL_INT -> {
                arg2.code(coder)
                if (a1type != ValueType.VAL_INT) coder.code(Opcode.OP_I2F)
            }
        }
        if (a1type == ValueType.VAL_FLOAT) codeFloat(coder) else codeInt(coder)
    }
}
class N_EQUAL(arg1: N_EXPRESSION, arg2: N_EXPRESSION): N_COMPARE_BINOP(arg1, arg2) {
    override fun codeFloat(coder: Coder) { coder.code(Opcode.OP_EQF) }
    override fun codeInt(coder: Coder) { coder.code(Opcode.OP_EQI) }
}
class N_NOTEQUAL(arg1: N_EXPRESSION, arg2: N_EXPRESSION): N_COMPARE_BINOP(arg1, arg2) {
    override fun codeFloat(coder: Coder) { coder.code(Opcode.OP_EQF); coder.code(Opcode.OP_NOT) }
    override fun codeInt(coder: Coder) { coder.code(Opcode.OP_EQI); coder.code(Opcode.OP_NOT) }
}
class N_GREATER(arg1: N_EXPRESSION, arg2: N_EXPRESSION): N_COMPARE_BINOP(arg1, arg2) {
    override fun codeFloat(coder: Coder) { coder.code(Opcode.OP_GTF) }
    override fun codeInt(coder: Coder) { coder.code(Opcode.OP_GTI) }
}
class N_LESS(arg1: N_EXPRESSION, arg2: N_EXPRESSION): N_COMPARE_BINOP(arg1, arg2) {
    override fun codeFloat(coder: Coder) { coder.code(Opcode.OP_LTF) }
    override fun codeInt(coder: Coder) { coder.code(Opcode.OP_LTI) }
}
class N_GREATER_EQUAL(arg1: N_EXPRESSION, arg2: N_EXPRESSION): N_COMPARE_BINOP(arg1, arg2) {
    override fun codeFloat(coder: Coder) { coder.code(Opcode.OP_GEF) }
    override fun codeInt(coder: Coder) { coder.code(Opcode.OP_GEI) }
}
class N_LESS_EQUAL(arg1: N_EXPRESSION, arg2: N_EXPRESSION): N_COMPARE_BINOP(arg1, arg2) {
    override fun codeFloat(coder: Coder) { coder.code(Opcode.OP_LEF) }
    override fun codeInt(coder: Coder) { coder.code(Opcode.OP_LEI) }
}

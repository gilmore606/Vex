package compiler.nodes

import compiler.Coder
import compiler.CompileException
import compiler.Meaner
import compiler.Opcode.*
import compiler.ValueType.*

abstract class N_COMPARE_BINOP(arg1: N_EXPRESSION, arg2: N_EXPRESSION): Node.N_BINOP(arg1, arg2) {

    override fun setType(meaner: Meaner): Boolean {
        if (this.type != VAL_BOOL) {
            this.type = VAL_BOOL
            return false
        }
        return true
    }

    abstract fun codeFloat(coder: Coder)
    abstract fun codeInt(coder: Coder)

    override fun code(coder: Coder) {

        val incomparable = CompileException(this, "incomparable types " + arg1.type.toString() + " and " + arg2.type.toString())

        var a1type = when (arg1.type) {
            VAL_INT, VAL_NIL, VAL_BOOL -> VAL_INT
            VAL_FLOAT, VAL_VECTOR, VAL_COLOR -> VAL_FLOAT
            else -> throw incomparable
        }
        arg2.code(coder)
        when (arg2.type) {
            VAL_NIL -> {
                coder.code(OP_N2I)
                if (a1type == VAL_FLOAT) coder.code(OP_I2F)
            }
            VAL_BOOL -> {
                coder.code(OP_B2I)
                if (a1type == VAL_FLOAT) coder.code(OP_I2F)
            }
            VAL_VECTOR -> coder.code(OP_V2F)
            VAL_COLOR -> coder.code(OP_C2F)
            VAL_INT -> if (a1type == VAL_FLOAT) coder.code(OP_I2F)
            VAL_STRING -> throw incomparable
        }
        arg1.code(coder)
        when (arg1.type) {
            VAL_NIL -> { coder.code(OP_N2I); if (arg2.type != VAL_INT) coder.code(OP_I2F) }
            VAL_BOOL -> { coder.code(OP_B2I); if (arg2.type != VAL_INT) coder.code(OP_I2F) }
            VAL_INT -> { if (arg2.type != VAL_INT) coder.code(OP_I2F) }
            VAL_VECTOR -> { coder.code(OP_V2F) }
            VAL_COLOR -> { coder.code(OP_C2F) }
        }
        if (a1type == VAL_FLOAT) codeFloat(coder) else codeInt(coder)
    }
}

class N_EQUAL(arg1: N_EXPRESSION, arg2: N_EXPRESSION): N_COMPARE_BINOP(arg1, arg2) {
    override fun codeFloat(coder: Coder) { coder.code(OP_EQF) }
    override fun codeInt(coder: Coder) { coder.code(OP_EQI) }

    override fun code(coder: Coder) {
        if ((arg1.type == VAL_STRING) && (arg2.type == VAL_STRING)) {
            arg1.code(coder)
            arg2.code(coder)
            coder.code(OP_EQS)
        } else if ((arg1.type == VAL_VECTOR) && (arg2.type == VAL_VECTOR)) {
            arg1.code(coder)
            arg2.code(coder)
            coder.code(OP_EQV)
        } else if ((arg1.type == VAL_COLOR) && (arg2.type == VAL_COLOR)) {
            arg1.code(coder)
            arg2.code(coder)
            coder.code(OP_EQC)
        } else super.code(coder)
    }
}

class N_NOTEQUAL(arg1: N_EXPRESSION, arg2: N_EXPRESSION): N_COMPARE_BINOP(arg1, arg2) {
    override fun codeFloat(coder: Coder) { coder.code(OP_EQF); coder.code(OP_NOT) }
    override fun codeInt(coder: Coder) { coder.code(OP_EQI); coder.code(OP_NOT) }
}

class N_GREATER(arg1: N_EXPRESSION, arg2: N_EXPRESSION): N_COMPARE_BINOP(arg1, arg2) {
    override fun codeFloat(coder: Coder) { coder.code(OP_GTF) }
    override fun codeInt(coder: Coder) { coder.code(OP_GTI) }
}

class N_LESS(arg1: N_EXPRESSION, arg2: N_EXPRESSION): N_COMPARE_BINOP(arg1, arg2) {
    override fun codeFloat(coder: Coder) { coder.code(OP_LTF) }
    override fun codeInt(coder: Coder) { coder.code(OP_LTI) }
}

class N_GREATER_EQUAL(arg1: N_EXPRESSION, arg2: N_EXPRESSION): N_COMPARE_BINOP(arg1, arg2) {
    override fun codeFloat(coder: Coder) { coder.code(OP_GEF) }
    override fun codeInt(coder: Coder) { coder.code(OP_GEI) }
}

class N_LESS_EQUAL(arg1: N_EXPRESSION, arg2: N_EXPRESSION): N_COMPARE_BINOP(arg1, arg2) {
    override fun codeFloat(coder: Coder) { coder.code(OP_LEF) }
    override fun codeInt(coder: Coder) { coder.code(OP_LEI) }
}

package compiler.nodes

import compiler.Coder
import compiler.CompileException
import compiler.Meaner
import compiler.Opcode.*
import compiler.ValueType.*

abstract class N_MATH_BINOP(arg1: N_EXPRESSION, arg2: N_EXPRESSION): Node.N_BINOP(arg1, arg2) {
    override fun setType(meaner: Meaner) {
        val e = CompileException("type error: math op accepts only math types")
        this.type = when (arg1.type) {
            VAL_INT -> when (arg2.type) {
                VAL_INT -> VAL_INT
                VAL_FLOAT -> VAL_FLOAT
                VAL_VECTOR -> VAL_VECTOR
                else -> throw e
            }
            VAL_FLOAT -> when (arg2.type) {
                VAL_INT, VAL_FLOAT -> VAL_FLOAT
                VAL_VECTOR -> VAL_VECTOR
                else -> throw e
            }
            VAL_VECTOR -> when (arg2.type) {
                VAL_INT, VAL_FLOAT -> VAL_VECTOR
                VAL_VECTOR -> if (this is N_MULTIPLY) VAL_FLOAT else VAL_VECTOR
                else -> throw e
            }
            else -> throw e
        }
    }
    abstract fun codeInt(coder: Coder)
    abstract fun codeFloat(coder: Coder)
    abstract fun codeVector(coder: Coder)
    abstract fun codeVecF(coder: Coder)
    override fun code(coder: Coder) {
        when (arg1.type) {
            VAL_INT -> when (arg2.type) {
                VAL_INT -> {
                    arg1.code(coder)
                    arg2.code(coder)
                    codeInt(coder)
                }
                VAL_FLOAT -> {
                    arg1.code(coder)
                    coder.code(OP_I2F)
                    arg2.code(coder)
                    codeFloat(coder)
                }
                VAL_VECTOR -> {
                    arg2.code(coder)
                    arg1.code(coder)
                    coder.code(OP_I2F)
                    codeVecF(coder)
                }
            }
            VAL_FLOAT -> when (arg2.type) {
                VAL_INT -> {
                    arg1.code(coder)
                    arg2.code(coder)
                    coder.code(OP_I2F)
                    codeFloat(coder)
                }
                VAL_FLOAT -> {
                    arg1.code(coder)
                    arg2.code(coder)
                    codeFloat(coder)
                }
                VAL_VECTOR -> {
                    arg2.code(coder)
                    arg1.code(coder)
                    codeVecF(coder)
                }
            }
            VAL_VECTOR -> when (arg2.type) {
                VAL_INT -> {
                    arg1.code(coder)
                    arg2.code(coder)
                    coder.code(OP_I2F)
                    codeVecF(coder)
                }
                VAL_FLOAT -> {
                    arg1.code(coder)
                    arg2.code(coder)
                    codeVecF(coder)
                }
                VAL_VECTOR -> {
                    arg1.code(coder)
                    arg2.code(coder)
                    codeVector(coder)
                }
            }
        }
    }
}
class N_ADD(arg1: N_EXPRESSION, arg2: N_EXPRESSION): N_MATH_BINOP(arg1, arg2) {
    override fun codeInt(coder: Coder) { coder.code(OP_ADDI) }
    override fun codeFloat(coder: Coder) { coder.code(OP_ADDF) }
    override fun codeVector(coder: Coder) { coder.code(OP_ADDV) }
    override fun codeVecF(coder: Coder) { coder.code(OP_ADDVF) }
}
class N_SUBTRACT(arg1: N_EXPRESSION, arg2: N_EXPRESSION): N_MATH_BINOP(arg1, arg2) {
    override fun codeInt(coder: Coder) { coder.code(OP_SUBI) }
    override fun codeFloat(coder: Coder) { coder.code(OP_SUBF) }
    override fun codeVector(coder: Coder) {
        coder.code(OP_NEGV)
        coder.code(OP_ADDV)
    }
    override fun codeVecF(coder: Coder) {
        coder.code(OP_NEGF)
        coder.code(OP_ADDVF)
    }
}
class N_MULTIPLY(arg1: N_EXPRESSION, arg2: N_EXPRESSION): N_MATH_BINOP(arg1, arg2) {
    override fun codeInt(coder: Coder) { coder.code(OP_MULTI) }
    override fun codeFloat(coder: Coder) { coder.code(OP_MULTF) }
    override fun codeVector(coder: Coder) { coder.code(OP_MULTV) }
    override fun codeVecF(coder: Coder) { coder.code(OP_MULTVF) }
}
class N_DIVIDE(arg1: N_EXPRESSION, arg2: N_EXPRESSION): N_MATH_BINOP(arg1, arg2) {
    override fun setType(meaner: Meaner) {
        val e = CompileException("type error: math op accepts only math types")
        this.type = when (arg1.type) {
            VAL_INT -> when (arg2.type) {
                VAL_INT -> VAL_INT
                VAL_FLOAT -> VAL_FLOAT
                VAL_VECTOR -> VAL_VECTOR
                else -> throw e
            }
            VAL_FLOAT -> when (arg2.type) {
                VAL_INT, VAL_FLOAT -> VAL_FLOAT
                VAL_VECTOR -> throw CompileException("type error: can't divide float by vector")
                else -> throw e
            }
            VAL_VECTOR -> when (arg2.type) {
                VAL_INT, VAL_FLOAT -> VAL_FLOAT
                VAL_VECTOR -> VAL_VECTOR
                else -> throw e
            }
            else -> throw e
        }
    }
    override fun codeInt(coder: Coder) { coder.code(OP_DIVI) }
    override fun codeFloat(coder: Coder) { coder.code(OP_DIVF) }
    override fun codeVector(coder: Coder) { coder.code(OP_DIVV) }
    override fun codeVecF(coder: Coder) { coder.code(OP_DIVVF) }
}
class N_POWER(arg1: N_EXPRESSION, arg2: N_EXPRESSION): N_MATH_BINOP(arg1, arg2) {
    override fun codeInt(coder: Coder) {  }
    override fun codeFloat(coder: Coder) {  }
    override fun codeVector(coder: Coder) { throw CompileException("type error: can't apply powers to vectors") }
    override fun codeVecF(coder: Coder) { throw CompileException("type error: can't apply powers to vectors") }
}

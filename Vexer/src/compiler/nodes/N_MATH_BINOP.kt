package compiler.nodes

import compiler.Coder
import compiler.CompileException
import compiler.Meaner
import compiler.Opcode.*
import compiler.ValueType.*

abstract class N_MATH_BINOP(arg1: N_EXPRESSION, arg2: N_EXPRESSION): Node.N_BINOP(arg1, arg2) {

    override fun setType(meaner: Meaner): Boolean {
        val oldtype = this.type
        this.type = when (arg1.type) {
            VAL_INT -> when (arg2.type) {
                VAL_INT -> VAL_INT
                VAL_FLOAT -> VAL_FLOAT
                VAL_VECTOR -> VAL_VECTOR
                else -> null
            }
            VAL_FLOAT -> when (arg2.type) {
                VAL_INT, VAL_FLOAT -> VAL_FLOAT
                VAL_VECTOR -> VAL_VECTOR
                VAL_COLOR -> VAL_COLOR
                else -> null
            }
            VAL_VECTOR -> when (arg2.type) {
                VAL_INT, VAL_FLOAT -> VAL_VECTOR
                VAL_VECTOR -> if (this is N_MULTIPLY) VAL_FLOAT else VAL_VECTOR
                else -> null
            }
            VAL_COLOR -> when (arg2.type) {
                VAL_FLOAT, VAL_COLOR -> VAL_COLOR
                else -> null
            }
            VAL_STRING -> VAL_STRING
            else -> null
        }
        return this.type == oldtype
    }

    fun typeE() = CompileException(this, "illegal types (" + arg1.type.toString() + ", " + arg2.type.toString() + ") in math op")

    open fun codeInt(coder: Coder) { throw typeE() }
    open fun codeFloat(coder: Coder) { throw typeE() }
    open fun codeVector(coder: Coder) { throw typeE() }
    open fun codeVecF(coder: Coder) { throw typeE() }
    open fun codeCF(coder: Coder) { throw typeE() }
    open fun codeColor(coder: Coder) { throw typeE() }

    override fun code(coder: Coder) {
        when (arg1.type) {
            VAL_STRING -> {
                if (this !is N_ADD) throw CompileException(this, "illegal string op -- can only add strings")
                arg1.code(coder)
                when (arg2.type) {
                    VAL_INT -> coder.code(OP_I2S)
                    VAL_FLOAT -> coder.code(OP_F2S)
                    else -> throw CompileException(this, "can only add int and float to string, not " + arg2.type.toString())
                }
                arg2.code(coder)
                coder.code(OP_CAT)
            }
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
                    arg1.code(coder)
                    coder.code(OP_I2F)
                    arg2.code(coder)
                    codeVecF(coder)
                }
                else -> throw typeE()
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
                    arg1.code(coder)
                    codeVecF(coder)
                    arg2.code(coder)
                }
                VAL_COLOR -> {
                    arg1.code(coder)
                    arg2.code(coder)
                    codeCF(coder)
                }
                else -> throw typeE()
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
                else -> throw typeE()
            }
            VAL_COLOR -> when (arg2.type) {
                VAL_FLOAT -> {
                    arg1.code(coder)
                    arg2.code(coder)
                    codeCF(coder)
                }
                VAL_COLOR -> {
                    arg1.code(coder)
                    arg2.code(coder)
                    codeColor(coder)
                }
                else -> throw typeE()
            }
            else -> throw typeE()
        }
    }
}

class N_ADD(arg1: N_EXPRESSION, arg2: N_EXPRESSION): N_MATH_BINOP(arg1, arg2) {
    override fun toString() = " + "
    override fun codeInt(coder: Coder) { coder.code(OP_ADDI) }
    override fun codeFloat(coder: Coder) { coder.code(OP_ADDF) }
    override fun codeVector(coder: Coder) { coder.code(OP_ADDV) }
    override fun codeVecF(coder: Coder) { coder.code(OP_ADDVF) }
    override fun codeCF(coder: Coder) { coder.code(OP_ADDCF) }
    override fun codeColor(coder: Coder) { coder.code(OP_ADDC) }
}

class N_SUBTRACT(arg1: N_EXPRESSION, arg2: N_EXPRESSION): N_MATH_BINOP(arg1, arg2) {
    override fun toString() = " - "
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
    override fun codeCF(coder: Coder) {
        coder.code(OP_NEGV)
        coder.code(OP_ADDCF)
    }
}

class N_MULTIPLY(arg1: N_EXPRESSION, arg2: N_EXPRESSION): N_MATH_BINOP(arg1, arg2) {
    override fun toString() = " * "
    override fun codeInt(coder: Coder) { coder.code(OP_MULI) }
    override fun codeFloat(coder: Coder) { coder.code(OP_MULF) }
    override fun codeVector(coder: Coder) { coder.code(OP_MULV) }
    override fun codeVecF(coder: Coder) { coder.code(OP_MULVF) }
}

class N_DIVIDE(arg1: N_EXPRESSION, arg2: N_EXPRESSION): N_MATH_BINOP(arg1, arg2) {
    override fun toString() = " / "
    override fun setType(meaner: Meaner): Boolean {
        val oldtype = this.type
        this.type = when (arg1.type) {
            VAL_INT -> when (arg2.type) {
                VAL_INT -> VAL_INT
                VAL_FLOAT -> VAL_FLOAT
                VAL_VECTOR -> VAL_VECTOR
                else -> null
            }
            VAL_FLOAT -> when (arg2.type) {
                VAL_INT, VAL_FLOAT -> VAL_FLOAT
                VAL_VECTOR -> throw CompileException(this, "type error: can't divide float by vector")
                else -> null
            }
            VAL_VECTOR -> when (arg2.type) {
                VAL_INT, VAL_FLOAT -> VAL_FLOAT
                VAL_VECTOR -> VAL_VECTOR
                else -> null
            }
            else -> null
        }
        return this.type == oldtype
    }
    override fun codeInt(coder: Coder) { coder.code(OP_DIVI) }
    override fun codeFloat(coder: Coder) { coder.code(OP_DIVF) }
    override fun codeVector(coder: Coder) { coder.code(OP_DIVV) }
    override fun codeVecF(coder: Coder) { coder.code(OP_DIVVF) }
}

class N_MODULO(arg1: N_EXPRESSION, arg2: N_EXPRESSION): N_MATH_BINOP(arg1, arg2) {
    override fun toString() = " % "
    override fun setType(meaner: Meaner): Boolean {
        val oldtype = this.type
        this.type = when (arg1.type) {
            VAL_INT -> when (arg2.type) {
                VAL_INT -> VAL_INT
                VAL_FLOAT -> VAL_FLOAT
                else -> null
            }
            VAL_FLOAT -> when (arg2.type) {
                VAL_INT,VAL_FLOAT -> VAL_FLOAT
                else -> null
            }
            else -> null
        }
        return this.type == oldtype
    }
    override fun codeInt(coder: Coder) { coder.code(OP_MODI) }
    override fun codeFloat(coder: Coder) { coder.code(OP_MODF) }
}

class N_POWER(arg1: N_EXPRESSION, arg2: N_EXPRESSION): N_MATH_BINOP(arg1, arg2) {
    override fun toString() = " ^ "
    override fun codeInt(coder: Coder) { coder.code(OP_POWI) }
    override fun codeFloat(coder: Coder) { coder.code(OP_POWF) }
}

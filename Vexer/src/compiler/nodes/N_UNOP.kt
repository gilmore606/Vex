package compiler.nodes

import compiler.Coder
import compiler.CompileException
import compiler.Meaner
import compiler.Opcode.*
import compiler.TokenType
import compiler.ValueType.*

abstract class N_UNOP(val arg: N_EXPRESSION): Node.N_EXPRESSION() {
    override fun kids(): NODES { return super.kids().apply { add(arg) }}

    override fun setType(meaner: Meaner): Boolean {
        val oldtype = this.type
        this.type = arg.type
        return this.type == oldtype
    }
}

class N_INCDEC(val operator: TokenType, arg: N_EXPRESSION,
               val isPost: Boolean, var isStatement: Boolean): N_UNOP(arg) {

    override fun toString() = "INCDEC " + name()
    fun name() = if (operator == TokenType.T_INCREMENT) "increment" else "decrement"

    override fun setType(meaner: Meaner): Boolean {
        val oldtype = this.type
        this.type = arg.type
        return this.type == oldtype
    }

    override fun checkTypeSane() {
        if (arg.type != VAL_INT)
            throw CompileException(this, "can only " + name() + " integers")
        if (arg !is N_VARREF)
            throw CompileException(this, "cannot " + name() + " non-variable expression")
    }

    override fun code(coder: Coder) {
        if (isPost && !isStatement) {
            coder.code(OP_VAR, (arg as N_VARREF).varID)
        }
        coder.code(
                if (operator == TokenType.T_INCREMENT) OP_INCVAR
                else OP_DECVAR
            , (arg as N_VARREF).varID
        )
        if (!isPost && !isStatement) {
            coder.code(OP_VAR, (arg as N_VARREF).varID)
        }
    }
}

class N_NEGATE(arg: N_EXPRESSION): N_UNOP(arg) {
    override fun toString() = "NEGATE"

    override fun setType(meaner: Meaner): Boolean {
        val oldtype = this.type
        this.type = arg.type
        return this.type == oldtype
    }

    override fun code(coder: Coder) {
        arg.code(coder)
        when (this.type) {
            VAL_INT -> coder.code(OP_NEGI)
            VAL_FLOAT -> coder.code(OP_NEGF)
            VAL_VECTOR -> coder.code(OP_NEGV)
            else -> throw CompileException(this, "type error: can only negate numeric values")
        }
    }
}

class N_INVERSE(arg: N_EXPRESSION): N_UNOP(arg) {
    override fun toString() = "NOT"

    override fun setType(meaner: Meaner): Boolean {
        if (this.type != VAL_BOOL) {
            this.type = VAL_BOOL
            return false
        }
        return true
    }

    override fun checkTypeSane() {
        if (arg.type != VAL_BOOL)
            throw CompileException(this, "type error: can only NOT booleans")
    }

    override fun code(coder: Coder) {
        arg.code(coder)
        coder.code(OP_NOT)
    }
}

package compiler.nodes

import compiler.Coder
import compiler.CompileException
import compiler.FuncSig
import compiler.Meaner
import compiler.Opcode.*

abstract class N_VALUE: Node.N_EXPRESSION()

abstract class N_LITERAL: N_VALUE() {
    var constID: Int = 0
    override fun code(coder: Coder) {
        coder.code(OP_CONST, constID)
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
class N_COLOR(val c1: Float, val c2: Float, val c3: Float): N_LITERAL() {
    override fun toString() = "COLOR (" + c1 + "," + c2 + "," + c3 + ")"
}

abstract class N_VARREF(): N_VALUE() {
    var varID: Int = -1
    abstract fun codeSet(coder: Coder)
}

class N_VARIABLE(val name: String): N_VARREF() {
    override fun toString() = "VARIABLE (" + name + "):" + varID
    override fun scopeVars(scope: Node, meaner: Meaner) {
        varID = meaner.variableToID(this, name, scope)
    }
    override fun code(coder: Coder) {
        coder.code(OP_VAR, varID)
    }
    override fun codeSet(coder: Coder) {
        coder.code(OP_SETVAR, varID)
    }
}

class N_FUNCALL(val name: String, val args: List<N_EXPRESSION>): N_VALUE() {
    var sig: FuncSig? = null
    override fun toString() = "FUN:" + name + "(" + args.joinToString(",") + ")"
    override fun kids(): NODES = super.kids().apply { args.forEach { add(it) }}
    override fun setType(meaner: Meaner): Boolean {
        val oldtype = this.type
        sig = meaner.getFuncSig(name, args)
        this.type = sig!!.returnType
        this.objtype = sig!!.returnObjType
        return oldtype == this.type
    }
    override fun checkType() {
        sig?.also { sig ->
            args.forEachIndexed { i, arg ->
                if (arg.type != sig.argtypes[i]) throw CompileException("illegal type function arg")
            }
        }
    }
    override fun code(coder: Coder) {
        args.reversed().forEach {
            it.code(coder)
        }
        sig?.also { sig ->
            coder.code(if (sig.sys) OP_SFUN else OP_FUN, sig.funcID)
        } ?: throw CompileException("unknown function signature")
    }
}

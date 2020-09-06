package compiler.nodes

import compiler.Coder
import compiler.Meaner
import compiler.Opcode.*
import compiler.ValueType.*

abstract class N_ASSIGN_BINOP(target: N_VARREF, value: N_EXPRESSION): N_ASSIGN(target, value) {
    override fun toString() = "assign" + mathNode.toString()
    abstract val mathNode: N_MATH_BINOP
    override fun setType(meaner: Meaner): Boolean {
        return mathNode.setType(meaner)
    }
    override fun checkTypeSane() {
        mathNode.checkTypeSane()
    }
    override fun code(coder: Coder) {
        mathNode.code(coder)
        target.codeSet(coder)
    }
}

class N_ASSIGN_ADD(target: N_VARREF, value: N_EXPRESSION): N_ASSIGN_BINOP(target, value) {
    override val mathNode = N_ADD(target, value)
    override fun code(coder: Coder) {
        if (target is N_VARIABLE) {
            if (value is N_INTEGER) {
                if (value.value == 1) {
                    coder.code(OP_INCVAR, target.varID)
                    return
                } else if (value.value == -1) {
                    coder.code(OP_DECVAR, target.varID)
                    return
                } else {
                    value.code(coder)
                    coder.code(OP_ACCVAR, target.varID)
                    return
                }
            } else if (value.type == VAL_INT) {
                value.code(coder)
                coder.code(OP_ACCVAR, target.varID)
                return
            } else if (value.type == VAL_FLOAT) {
                value.code(coder)
                coder.code(OP_ACCVARF, target.varID)
                return
            }
        }
        super.code(coder)
    }
}

class N_ASSIGN_SUB(target: N_VARREF, value: N_EXPRESSION): N_ASSIGN_BINOP(target, value) {
    override val mathNode = N_SUBTRACT(target, value)
    override fun code(coder: Coder) {
        if ((value is N_INTEGER) && (value.value == 1)) {
            coder.code(OP_DECVAR, target.varID)
            return
        }
        super.code(coder)
    }
}
class N_ASSIGN_MULT(target: N_VARREF, value: N_EXPRESSION): N_ASSIGN_BINOP(target, value) {
    override val mathNode = N_MULTIPLY(target, value)
}
class N_ASSIGN_DIV(target: N_VARREF, value: N_EXPRESSION): N_ASSIGN_BINOP(target, value) {
    override val mathNode = N_DIVIDE(target, value)
}
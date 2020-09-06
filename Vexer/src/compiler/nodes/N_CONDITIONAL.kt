package compiler.nodes

import compiler.Coder
import compiler.CompileException
import compiler.Meaner
import compiler.Opcode.*
import compiler.ValueType.*
import compiler.nodes.Node.*
import compiler.nodes.*

class N_CONDITIONAL(val condition: N_EXPRESSION, val trueVal: N_EXPRESSION, val falseVal: N_EXPRESSION): N_EXPRESSION() {
    override fun toString() = "COND: ? (true) : (false)"
    override fun kids(): NODES = super.kids().apply { add(condition); add(trueVal); add(falseVal) }
    override fun setType(meaner: Meaner): Boolean {
        var unchanged = true
        if (this.type == null) {
            trueVal.type?.also { this.type = it; unchanged = false }
            falseVal.type?.also { this.type = it; unchanged = false }
        }
        return unchanged
    }
    override fun checkTypeSane() {
        if (condition.type != VAL_BOOL) throw CompileException(this, "condition expects bool expr")
        if (trueVal.type != falseVal.type) throw CompileException(this, "conditional types must match")
    }
    override fun code(coder: Coder) {
        condition.code(coder)
        coder.code(OP_IF)
        coder.jumpFuture("ifskip"+id)
        trueVal.code(coder)
        coder.code(OP_JUMP)
        coder.jumpFuture("elseskip"+id)
        coder.reachFuture("ifskip"+id)
        falseVal.code(coder)
        coder.reachFuture("elseskip"+id)
    }
}

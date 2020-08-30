package compiler.nodes

import compiler.Coder
import compiler.CompileException
import compiler.Meaner
import compiler.Opcode.*
import compiler.ValueType.*

abstract class N_STATEMENT: Node()

class N_ASSIGN(val target: N_VARREF, val value: N_EXPRESSION): N_STATEMENT() {
    override fun kids(): NODES { return super.kids().apply { add(target); add(value) }}
    override fun setType(meaner: Meaner): Boolean {
        val oldtype = target.type
        if ((target.type == null) && (value.type != null)) meaner.learnVarType(target.varID, value.type!!)
        return oldtype == target.type
    }
    override fun checkType() {
        if (target.type != value.type) throw CompileException("type error: mismatched types in assignment")
    }
    override fun code(coder: Coder) {
        value.code(coder)
        target.codeSet(coder)
    }
}

class N_REPEAT(val count: N_EXPRESSION, val code: N_CODEBLOCK): N_STATEMENT() {
    var loopVarID = -1
    override fun kids(): NODES { return super.kids().apply { add(count); add(code) }}
    override fun scopeVars(scope: Node, meaner: Meaner) {
        super.scopeVars(scope, meaner)
        loopVarID = meaner.variableToID(this, "_loop"+id, scope)
    }
    override fun checkType() {
        if (count.type != VAL_INT) throw CompileException("type error: repeat statement expects int count")
    }
    override fun code(coder: Coder) {
        count.code(coder)
        coder.code(OP_SETVAR, loopVarID)
        coder.code(OP_JUMPZ, loopVarID)  // skip block if count is already 0
        coder.jumpFuture("loopskip"+id)
        val repeatJumpID = coder.addJumpPoint()
        code.code(coder)
        coder.code(OP_DECVAR, loopVarID)
        coder.code(OP_JUMPNZ, loopVarID, repeatJumpID)
        coder.reachFuture("loopskip"+id)
    }
}

class N_EACH(val iterator: N_VARIABLE, val code: N_CODEBLOCK): N_STATEMENT() {
    override fun kids(): NODES { return super.kids().apply { add(iterator); add(code) }}
}

class N_IF(val condition: N_EXPRESSION, val ifblock: N_CODEBLOCK): N_STATEMENT() {
    override fun kids(): NODES { return super.kids().apply { add(condition); add(ifblock); }}
    override fun checkType() {
        if (condition.type != VAL_BOOL) throw CompileException("type error: if statement expects boolean expr")
    }
    override fun code(coder: Coder) {
        condition.code(coder)
        coder.code(OP_IF)
        coder.jumpFuture("ifskip"+id)
        ifblock.code(coder)
        coder.reachFuture("ifskip"+id)
    }
}

class N_IFELSE(val condition: N_EXPRESSION, val ifblock: N_CODEBLOCK, val elseblock: N_CODEBLOCK): N_STATEMENT() {
    override fun kids(): NODES { return super.kids().apply { add(condition); add(ifblock); add(elseblock) }}
    override fun checkType() {
        if (condition.type != VAL_BOOL) throw CompileException("type error: if statement expects boolean expr")
    }
    override fun code(coder: Coder) {
        condition.code(coder)
        coder.code(OP_IF)
        coder.jumpFuture("ifskip"+id)
        ifblock.code(coder)
        coder.code(OP_JUMP)
        coder.jumpFuture("elseskip"+id)
        coder.reachFuture("ifskip"+id)
        elseblock.code(coder)
        coder.reachFuture("elseskip"+id)
    }
}

class N_FOR(val index: N_VARIABLE, val start: N_EXPRESSION, val end: N_EXPRESSION, val code: N_CODEBLOCK): N_STATEMENT() {
    var endvarID = -1
    override fun kids(): NODES { return super.kids().apply { add(index); add(start); add(end); add(code); }}
    override fun scopeVars(scope: Node, meaner: Meaner) {
        super.scopeVars(scope, meaner)
        endvarID = meaner.variableToID(this, "_forend"+id, scope)
    }
    override fun setType(meaner: Meaner): Boolean {
        val oldtype = index.type
        meaner.learnVarType(index.varID, VAL_INT)
        return oldtype == index.type
    }
    override fun checkType() {
        if (start.type != VAL_INT || end.type != VAL_INT) throw CompileException("type error: for loop can only count int")
    }
    override fun code(coder: Coder) {
        end.code(coder)
        coder.code(OP_SETVAR, endvarID)
        start.code(coder)
        index.codeSet(coder)
        val top = coder.addJumpPoint()
        index.code(coder)
        coder.code(OP_VAR, endvarID)
        coder.code(OP_LEI)
        coder.code(OP_IF)
        coder.jumpFuture("forskip"+id)
        code.code(coder)
        coder.code(OP_INCVAR, index.varID)
        coder.code(OP_JUMP, top)
        coder.reachFuture("forskip"+id)
    }
}

class N_SOUND(val soundid: N_EXPRESSION): N_STATEMENT() {
    override fun kids(): NODES { return super.kids().apply { add(soundid) }}
    override fun checkType() {
        if (soundid.type != VAL_INT) throw CompileException("type error: sound statement expects int id")
    }
    override fun code(coder: Coder) {
        soundid.code(coder)
        coder.code(OP_SONG)
    }
}

class N_WAIT(val time: N_EXPRESSION): N_STATEMENT() {
    override fun kids(): NODES { return super.kids().apply { add(time) }}
    override fun checkType() {
        if (time.type != VAL_INT) throw CompileException("type error: wait statement expects int msec")
    }
    override fun code(coder: Coder) {
        time.code(coder)
        coder.code(OP_WAIT)
    }
}

class N_LOG(val expr: N_EXPRESSION): N_STATEMENT() {
    override fun kids(): NODES { return super.kids().apply { add(expr) }}
    override fun code(coder: Coder) {
        expr.code(coder)
        coder.code(OP_DEBUG)
    }
}
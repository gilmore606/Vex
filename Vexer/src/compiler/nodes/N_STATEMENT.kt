package compiler.nodes

import compiler.Coder
import compiler.CompileException
import compiler.Opcode.*
import compiler.ValueType.*

abstract class N_STATEMENT: Node()

class N_ASSIGN(val target: N_VARREF, val value: N_EXPRESSION): N_STATEMENT() {
    override fun setType() {
        super.setType()
        if (target.type != value.type) throw CompileException("type error: assigned " + value.type.toString() + " to " + target.type.toString() + " var")
    }
    override fun kids(): NODES { return super.kids().apply { add(target); add(value) }}
    override fun code(coder: Coder) {
        value.code(coder)
        target.codeSet(coder)
    }
}

class N_REPEAT(val count: N_EXPRESSION, val code: N_CODEBLOCK): N_STATEMENT() {
    override fun kids(): NODES { return super.kids().apply { add(count); add(code) }}
    override fun setType() {
        super.setType()
        if (count.type != VAL_INT) throw CompileException("type error: repeat statement expects int count")
    }
}

class N_EACH(val iterator: N_VARIABLE, val code: N_CODEBLOCK): N_STATEMENT() {
    override fun kids(): NODES { return super.kids().apply { add(iterator); add(code) }}
}

class N_IFELSE(val condition: N_EXPRESSION, val ifblock: N_CODEBLOCK, val elseblock: N_CODEBLOCK): N_STATEMENT() {
    override fun kids(): NODES { return super.kids().apply { add(condition); add(ifblock); add(elseblock) }}
    override fun setType() {
        super.setType()
        if (condition.type != VAL_BOOL) throw CompileException("type error: if statement expects boolean expr")
    }
}

class N_SOUND(val soundid: N_EXPRESSION): N_STATEMENT() {
    override fun kids(): NODES { return super.kids().apply { add(soundid) }}
    override fun setType() {
        super.setType()
        if (soundid.type != VAL_INT) throw CompileException("type error: sound statement expects int id")
    }
    override fun code(coder: Coder) {
        soundid.code(coder)
        coder.code(OP_SONG)
    }
}

class N_WAIT(val time: N_EXPRESSION): N_STATEMENT() {
    override fun kids(): NODES { return super.kids().apply { add(time) }}
    override fun setType() {
        super.setType();
        if (time.type != VAL_INT) throw CompileException("type error: wait statement expects int msec")
    }
    override fun code(coder: Coder) {
        time.code(coder)
        coder.code(OP_WAIT)
    }
}
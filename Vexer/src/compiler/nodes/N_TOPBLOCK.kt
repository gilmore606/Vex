package compiler.nodes

import compiler.*
import compiler.Opcode.*
import compiler.ValueType.*
import compiler.nodes.*
import compiler.nodes.Node
import compiler.nodes.Node.*
import model.Game

abstract class N_TOPBLOCK(): Node()

class N_TOP_HANDLER(val name: String, val code: N_CODEBLOCK): N_TOPBLOCK() {
    override val localScope = true
    override fun toString() = "HANDLER on " + name + ":"
    override fun kids(): NODES = super.kids().apply { add(code) }
    override fun code(coder: Coder) {
        coder.addEntryPoint(name)
        code.code(coder)
        coder.code(OP_EXIT)
    }
}

class N_TOP_STATE(val assigns: N_CODEBLOCK): N_TOPBLOCK() {
    override fun toString() = "HANDLER state:"
    override fun kids(): NODES = super.kids().apply { add(assigns) }
    override fun code(coder: Coder) {
        coder.addEntryPoint("state")
        assigns.code(coder)
        coder.code(OP_EXIT)
    }
}

class N_TOP_BUTTONS(val handlers: List<N_BUTTON_HANDLER>): N_TOPBLOCK() {
    override val localScope = true
    override fun toString() = "HANDLER on button:"
    override fun kids(): NODES = super.kids().apply { handlers.forEach { add(it) }}
    override fun code(coder: Coder) {
        coder.addEntryPoint("button")
        handlers.forEachIndexed { i, handler ->
            handler.code(i, coder)
        }
        coder.code(OP_EXIT)
    }
}

class N_BUTTON_HANDLER(val button: String, val code: N_CODEBLOCK): Node() {
    var buttonID = -1
    override fun toString() = "BUTTON " + button
    override fun kids(): NODES = super.kids().apply { add(code) }
    override fun resolveResources(config: Game) {
        config.controls.forEachIndexed { i, c ->
            if (c.name == button) buttonID = i
        }
        if (buttonID == -1) throw CompileException("undefined button '" + button + "'")
    }
    fun code(i: Int, coder: Coder) {
        coder.code(OP_BUTTON, buttonID.toUByte())
        coder.code(OP_IF)
        coder.jumpFuture("buttonskip" + i.toString())
        code.code(coder)
        coder.reachFuture("buttonskip" + i.toString())
    }
}

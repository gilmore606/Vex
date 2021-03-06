package compiler.nodes

import compiler.*
import compiler.Opcode.*
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

class N_TOP_USERFUNC(val name: String, val args: List<FuncArg>, val returnType: ValueType, val code: N_CODEBLOCK): N_TOPBLOCK() {
    override val localScope = true
    var funID = -1
    val argVarIDs = ArrayList<Int>()

    init { args.forEach { argVarIDs.add(-1) } }

    fun getFuncSig(): FuncSig {
        return FuncSig(false, name, funID, returnType, null, args.map { it.type })
    }

    override fun toString() = "USERFUNC to " + name + ":"
    override fun kids(): NODES = super.kids().apply { add(code) }

    override fun scopeVars(scope: Node, meaner: Meaner) {
        if (funID != -1) return
        funID = meaner.assignFunctionID(this)
        traverse { if (it is N_RETURN) it.learnFunction(this) }
        args.forEachIndexed { i, arg ->
            argVarIDs[i] = meaner.variableToID(this, arg.name, this)
        }
        super.scopeVars(scope, meaner)
    }

    override fun setType(meaner: Meaner): Boolean {
        var unchanged = true
        args.forEachIndexed { i, arg ->
            if (!meaner.varTypeKnown(argVarIDs[i])) {
                unchanged = false
                meaner.learnVarType(argVarIDs[i], arg.type, null)
            }
        }
        return unchanged
    }

    override fun code(coder: Coder) {
        coder.addEntryPoint(funID.toString())
        argVarIDs.forEach { // load args from stack into vars
            coder.code(OP_SETVAR, it)
        }
        code.code(coder)
        if (coder.lastByte() != OP_RETURN.ordinal.toUByte()) {
            coder.code(OP_RETURN)
        }
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
        if (buttonID == -1) throw CompileException(this, "undefined button '" + button + "'")
    }

    fun code(i: Int, coder: Coder) {
        coder.code(OP_BUTTON, buttonID.toUByte())
        coder.code(OP_IF)
        coder.jumpFuture("buttonskip" + i.toString())
        code.code(coder)
        coder.reachFuture("buttonskip" + i.toString())
    }
}

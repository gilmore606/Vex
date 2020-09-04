package compiler.nodes

import compiler.*
import compiler.Opcode.*
import compiler.ValueType.*

typealias NODES = ArrayList<Node>

open class Node {

    var linePos = 0
    var charPos = 0

    var id = ""
    init {
        id = (Math.random() * 100000.0).toInt().toString()
    }

    fun tag(p: Parser) {
        this.linePos = p.linePos()
        this.charPos = p.charPos()
    }

    fun findNode(test: (Node)->Boolean): Node? {
        if (test(this)) return this
        kids().forEach { kid -> kid.findNode(test)?.also { return it } }
        return null
    }

    open fun print(lvl: Int) {
        println("  ".repeat(lvl) + this.toString())
        kids().forEach { it.print(lvl + 1) }
    }

    open fun code(coder: Coder) { }

    open fun kids(): NODES = ArrayList()

    open fun traverse(worker: (Node)->Unit) {
        kids().forEach { it.traverse(worker) }
        worker(this)
    }

    open fun setType(meaner: Meaner): Boolean { return true }

    open fun checkType() { }

    open val localScope = false
    open fun scopeVars(scope: Node, meaner: Meaner) {
        kids().forEach { it.scopeVars(if (localScope) this else scope, meaner) }
    }



    // CLASSES

    abstract class N_EXPRESSION: Node() {
        var type: ValueType? = null
        var objtype: ObjectType? = null
    }

    abstract class N_BINOP(val arg1: N_EXPRESSION, val arg2: N_EXPRESSION): N_EXPRESSION() {
        override fun kids(): NODES = super.kids().apply { add(arg1); add(arg2) }
    }


    class N_PROGRAM(val blocks: List<N_TOPBLOCK>): Node() {
        override fun kids(): NODES = super.kids().apply { blocks.forEach { add(it) } }
        override fun code(coder: Coder) {
            blocks.forEach { it.code(coder) }
        }
    }

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

    class N_CODEBLOCK(val statements: List<N_STATEMENT>): Node() {
        override fun toString() = "BLOCK:"
        override fun kids(): NODES = super.kids().apply { statements.forEach { add(it) } }
        override fun code(coder: Coder) {
            statements.forEach { it.code(coder) }
        }
    }

}

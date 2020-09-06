package compiler.nodes

import compiler.*
import compiler.Opcode.*
import compiler.ValueType.*
import model.Game

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

    open fun resolveResources(config: Game) { }

    open fun checkTypeResolved() { }

    open fun checkTypeSane() { }

    open val localScope = false
    open fun scopeVars(scope: Node, meaner: Meaner) {
        kids().forEach { it.scopeVars(if (localScope) this else scope, meaner) }
    }



    // CLASSES

    abstract class N_EXPRESSION: Node() {
        var type: ValueType? = null
        var objtype: ObjectType? = null

        override fun checkTypeResolved() {
            type ?: throw CompileException(this, "expression type unresolved")
        }
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

    class N_CODEBLOCK(val statements: List<N_STATEMENT>): Node() {
        override fun toString() = "BLOCK:"
        override fun kids(): NODES = super.kids().apply { statements.forEach { add(it) } }

        override fun code(coder: Coder) {
            statements.forEach { it.code(coder) }
        }
    }

}

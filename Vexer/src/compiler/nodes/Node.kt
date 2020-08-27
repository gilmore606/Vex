package compiler.nodes

import compiler.Coder
import compiler.CompileException
import compiler.Opcode.*
import compiler.Parser
import compiler.ValueType
import compiler.ValueType.*

typealias NODES = ArrayList<Node>

open class Node {

    var linePos = 0
    var charPos = 0

    fun tag(p: Parser) {
        this.linePos = p.linePos()
        this.charPos = p.charPos()
    }
    open fun print(lvl: Int) {
        println("  ".repeat(lvl) + this.toString())
        kids().forEach { it.print(lvl + 1) }
    }
    open fun code(coder: Coder) { }
    open fun kids(): NODES { return ArrayList() }
    open fun traverse(worker: (Node)->Unit) {
        kids().forEach { it.traverse(worker) }
        worker(this)
    }
    open fun setType() {
        kids().forEach { it.setType() }
    }
    open fun scopeVars(scope: Node, coder: Coder) {
        kids().forEach { it.scopeVars(scope, coder) }
    }

    abstract class N_EXPRESSION: Node() {
        var type: ValueType? = null
    }

    abstract class N_BINOP(val arg1: N_EXPRESSION, val arg2: N_EXPRESSION): N_EXPRESSION() {
        override fun kids(): NODES { return super.kids().apply { add(arg1); add(arg2) }}
    }


    class N_PROGRAM(val blocks: List<N_BLOCK>): Node() {
        override fun kids(): NODES { return super.kids().apply { blocks.forEach { add(it) } }}
        override fun code(coder: Coder) {
            blocks.forEach { it.code(coder) }
        }
    }

    abstract class N_BLOCK: Node()

    class N_SETTINGS(val assigns: List<N_ASSIGN>): N_BLOCK() {
        override fun kids(): NODES { return super.kids().apply { assigns.forEach { add(it) } }}
    }

    class N_FUNCTION(val name: String, val code: N_CODEBLOCK): N_BLOCK() {
        override fun kids(): NODES { return super.kids().apply { add(code) }}
        override fun code(coder: Coder) {
            coder.addEntryPoint(name)
            code.code(coder)
            coder.code(OP_EXIT)
        }
    }

    class N_CODEBLOCK(val statements: List<N_STATEMENT>): N_BLOCK() {
        override fun kids(): NODES { return super.kids().apply { statements.forEach { add(it) } }}
        override fun code(coder: Coder) {
            statements.forEach { it.code(coder) }
        }
    }

}

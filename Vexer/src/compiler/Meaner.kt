package compiler

import compiler.nodes.Node
import compiler.nodes.*

enum class ValueType { VAL_NIL, VAL_BOOL, VAL_INT, VAL_FLOAT, VAL_VECTOR, VAL_STRING, VAL_OBJECT }

data class Value(val type: ValueType,
                 val boolean: Boolean, val integer: Int, val fp: Float,
                 val v1: Float, val v2: Float, val string: String, val obj: ValueObj?) {
    override fun toString(): String = when (type) {
            ValueType.VAL_NIL -> "nil"
            ValueType.VAL_BOOL -> if (this.boolean) "true" else "false"
            ValueType.VAL_INT -> this.integer.toString()
            ValueType.VAL_FLOAT -> this.fp.toString()
            ValueType.VAL_VECTOR -> "V(" + this.v1.toString() + "," + this.v2.toString() + ")"
            ValueType.VAL_STRING -> this.string
            ValueType.VAL_OBJECT -> "OBJ:" + this.obj!!.name
        }
}

data class ValueObj(val name: String)

data class Variable(val id: Int, val name: String, val scope: Node, val type: ValueType?, val nodes: ArrayList<N_VARIABLE> = ArrayList())

class Meaner (
        val ast: ArrayList<Node>,
        val fVerbose: Boolean
) {

    val constants = ArrayList<Value>()
    val variables = ArrayList<Variable>()

    // Get ID of named variable in my scope
    fun variableToID(sourceNode: Node, name: String, scope: Node): Int {
        variables.forEach {
            if (it.scope == scope && it.name == name) {
                if (sourceNode is N_VARIABLE) it.nodes.add(sourceNode)
                return it.id
            }
        }
        val v = Variable(variables.size, name, scope, null)
        if (sourceNode is N_VARIABLE) v.nodes.add(sourceNode)
        variables.add(v)
        return v.id
    }

    // Set type of all nodes for a variable
    fun learnVarType(varID: Int, type: ValueType) {
        variables.find { it.id == varID }!!.nodes.forEach {
            it.type = type
        }
    }

    fun mean() {

        // Index all literal constants (and set their type)
        ast[0].traverse { node ->
            if (node is N_LITERAL) {
                var found = -1
                constants.forEachIndexed { i, c ->
                    if (node is N_BOOLEAN && c.type == ValueType.VAL_BOOL && node.value == c.boolean) found = i
                    if (node is N_INTEGER && c.type == ValueType.VAL_INT && node.value == c.integer) found = i
                    if (node is N_FLOAT && c.type == ValueType.VAL_FLOAT && node.value == c.fp) found = i
                    if (node is N_VECTOR && c.type == ValueType.VAL_VECTOR && node.v1 == c.v1 && node.v2 == c.v2) found = i
                }
                if (found == -1) {
                    val type = when {
                        node is N_BOOLEAN -> ValueType.VAL_BOOL
                        node is N_INTEGER -> ValueType.VAL_INT
                        node is N_FLOAT -> ValueType.VAL_FLOAT
                        node is N_VECTOR -> ValueType.VAL_VECTOR
                        else -> ValueType.VAL_NIL
                    }
                    constants.add(Value(type,
                            if (node is N_BOOLEAN) node.value else false,
                            if (node is N_INTEGER) node.value else 0,
                            if (node is N_FLOAT) node.value else 0.0f,
                            if (node is N_VECTOR) node.v1 else 0.0f,
                            if (node is N_VECTOR) node.v2 else 0.0f,
                            if (node is N_STRING) node.value else "",
                            null
                        ))
                    found = constants.lastIndex
                    if (fVerbose) println("  create const " + constants[found].toString())
                }
                node.constID = found
                node.type = constants[found].type
            }
        }

        // Index all variables
        ast[0].scopeVars(ast[0], this)

        // Infer all expression types
        var settled = false
        while (!settled) {
            settled = true
            ast[0].traverse {
                if (!it.setType(this)) settled = false
            }
        }

        // Check types for sanity
        ast[0].traverse { it.checkType() }


        // TODO: Index all function entries

    }

}

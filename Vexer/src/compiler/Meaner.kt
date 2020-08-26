package compiler

import compiler.nodes.Node
import compiler.nodes.*

enum class ValueType { VAL_NIL, VAL_BOOL, VAL_INT, VAL_FLOAT, VAL_VECTOR }

data class Value(val type: ValueType, val boolean: Boolean, val integer: Int, val fp: Float, val v1: Float, val v2: Float)

class Meaner (
        val ast: ArrayList<Node>,
        val fVerbose: Boolean
) {

    val constants = ArrayList<Value>()

    fun mean() {

        // Index all literal constants

        ast[0].flatten().forEach { node ->
            if (fVerbose) println("  " + node.toString())
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
                        if (node is N_VECTOR) node.v2 else 0.0f
                    ))
                    found = constants.lastIndex
                    if (fVerbose) println("  const " + constants[found].toString())
                }
                node.constID = found
                node.type = constants[found].type
            }

        }

        // Index all variables


        // Infer and check all expression types

        //ast[0].setType()

        // TODO: Index all function entries

    }

}

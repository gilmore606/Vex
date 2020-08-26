package compiler

import compiler.Node.*

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
            if (node is LITERAL) {
                var found = -1
                constants.forEachIndexed { i, c ->
                    if (node is BOOLEAN && c.type == ValueType.VAL_BOOL && node.value == c.boolean) found = i
                    if (node is INTEGER && c.type == ValueType.VAL_INT && node.value == c.integer) found = i
                    if (node is FLOAT && c.type == ValueType.VAL_FLOAT && node.value == c.fp) found = i
                }
                if (found == -1) {
                    val type = when {
                        node is BOOLEAN -> ValueType.VAL_BOOL
                        node is INTEGER -> ValueType.VAL_INT
                        node is FLOAT -> ValueType.VAL_FLOAT
                        else -> ValueType.VAL_NIL
                    }
                    constants.add(Value(type,
                        if (node is BOOLEAN) node.value else false,
                        if (node is INTEGER) node.value else 0,
                        if (node is FLOAT) node.value else 0.0f,
                        0.0f,
                        0.0f
                    ))
                    found = constants.lastIndex
                    if (fVerbose) println("  const " + constants[found].toString())
                }
                node.constID = found
            }

        }

    }

}

package compiler

import compiler.nodes.N_VARIABLE
import compiler.nodes.Node

enum class ValueType { VAL_NIL, VAL_BOOL, VAL_INT, VAL_FLOAT, VAL_VECTOR, VAL_COLOR, VAL_STRING, VAL_OBJECT }

enum class ObjectType { OBJ_SPRITE }

data class Value(val type: ValueType,
                 val boolean: Boolean, val integer: Int, val fp: Float,
                 val v1: Float, val v2: Float, val c1: Float, val c2: Float, val c3: Float,
                 val string: String, val obj: ValueObj?) {
    override fun toString(): String = when (type) {
        ValueType.VAL_NIL -> "nil"
        ValueType.VAL_BOOL -> if (this.boolean) "true" else "false"
        ValueType.VAL_INT -> this.integer.toString()
        ValueType.VAL_FLOAT -> this.fp.toString()
        ValueType.VAL_VECTOR -> "V(" + this.v1.toString() + "," + this.v2.toString() + ")"
        ValueType.VAL_COLOR -> "C(" + this.c1.toString() + "," + this.c2.toString() + "," + this.c3.toString() + ")"
        ValueType.VAL_STRING -> this.string
        ValueType.VAL_OBJECT -> "OBJ:" + this.obj!!.name
    }
}

data class ValueObj(val name: String, val type: ObjectType)

data class Variable(val id: Int, val name: String, val scope: Node, val type: ValueType?, val nodes: ArrayList<N_VARIABLE> = ArrayList())

data class FuncSig(val sys: Boolean, val name: String, val funcID: Int,
                   val returnType: ValueType, val returnObjType: ObjectType?, val argtypes: List<ValueType>)

package compiler

import compiler.nodes.Node
import compiler.nodes.*
import compiler.ValueType.*
import compiler.ObjectType.*

typealias FUNCLIST = ArrayList<FuncSig>
typealias TYPELIST = ArrayList<ValueType>

class Meaner (
        val ast: ArrayList<Node>,
        val fVerbose: Boolean
) {

    val constants = ArrayList<Value>()
    val variables = ArrayList<Variable>()

    val systemFuncs = FUNCLIST()
    val systemClasses = HashMap<ObjectType, FUNCLIST>()

    init {

        // Built-in system functions

        systemFuncs.add(FuncSig(true, "V", 0, VAL_VECTOR, null,
                TYPELIST().apply { add(VAL_FLOAT); add(VAL_FLOAT) }))
        systemFuncs.add(FuncSig(true, "C", 1, VAL_COLOR, null,
                TYPELIST().apply { add(VAL_FLOAT); add(VAL_FLOAT); add(VAL_FLOAT) }))
        systemFuncs.add(FuncSig(true, "RAND", 2, VAL_FLOAT, null,
                TYPELIST().apply { add(VAL_FLOAT); add(VAL_FLOAT) }))
        systemFuncs.add(FuncSig(true, "RAND", 3, VAL_INT, null,
                TYPELIST().apply { add(VAL_INT); add(VAL_INT) }))
        systemFuncs.add(FuncSig(true, "RAND", 4, VAL_INT, null,
                TYPELIST().apply { add(VAL_INT) }))
        systemFuncs.add(FuncSig(true, "RAND", 5, VAL_FLOAT, null,
                TYPELIST() ))
        systemFuncs.add(FuncSig(true, "TEXT", 6, VAL_OBJECT, OBJ_SPRITE,
                TYPELIST().apply { add(VAL_STRING) }))

        // Built-in system class methods

        systemClasses[OBJ_SPRITE] = FUNCLIST().apply {
            add(FuncSig(true, "move", 0, VAL_NIL, null,
                TYPELIST().apply { add(VAL_FLOAT); add(VAL_FLOAT) }))
            add(FuncSig(true, "move", 1, VAL_NIL, null,
                TYPELIST().apply { add(VAL_VECTOR) }))
            add(FuncSig(true, "scale", 2, VAL_NIL, null,
                TYPELIST().apply { add(VAL_FLOAT); add(VAL_FLOAT) }))
            add(FuncSig(true, "scale", 3, VAL_NIL, null,
                TYPELIST().apply { add(VAL_FLOAT) }))
            add(FuncSig(true, "hide", 4, VAL_NIL, null,
                TYPELIST()))
            add(FuncSig(true, "show", 5, VAL_NIL, null,
                TYPELIST()))
            add(FuncSig(true, "write", 6, VAL_NIL, null,
                TYPELIST().apply { add(VAL_STRING) }))
        }

    }

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
    fun learnVarType(varID: Int, type: ValueType, objtype: ObjectType?) {
        variables.find { it.id == varID }!!.nodes.forEach {
            it.type = type
            it.objtype = objtype
        }
    }

    // Look up signature of named function
    fun getFuncSig(name: String, args: List<Node.N_EXPRESSION>): FuncSig {
        systemFuncs.forEach { if (it.name == name) {
            var match = true
            it.argtypes.forEachIndexed { i, type ->
                if (i > args.lastIndex) match = false
                else if (type != args[i].type) match = false
            }
            if (match) return it
        } }
        throw CompileException("unknown function call")
    }

    // Look up signature of system class method
    fun getMethodSig(type: ObjectType, name: String, args: List<Node.N_EXPRESSION>): FuncSig {
        systemClasses[type]!!.forEach {
            if (it.name == name && it.returnType == VAL_NIL) {
                var match = true
                it.argtypes.forEachIndexed { i, methArg ->
                    if (i > args.lastIndex) match = false
                    else if (methArg != args[i].type) match = false
                }
                if (match) return it
            }
        }
        throw CompileException("unknown system method call")
    }

    fun mean() {

        // Index all literal constants (and set their type)
        ast[0].traverse { node ->
            if (node is N_LITERAL) {
                var found = -1
                constants.forEachIndexed { i, c ->
                    if (node is N_BOOLEAN && c.type == VAL_BOOL && node.value == c.boolean) found = i
                    if (node is N_INTEGER && c.type == VAL_INT && node.value == c.integer) found = i
                    if (node is N_FLOAT && c.type == VAL_FLOAT && node.value == c.fp) found = i
                    if (node is N_VECTOR && c.type == VAL_VECTOR && node.v1 == c.v1 && node.v2 == c.v2) found = i
                    if (node is N_COLOR && c.type == VAL_COLOR && node.c1 == c.c1 && node.c2 == c.c2 && node.c3 == c.c3) found = i
                    if (node is N_STRING && c.type == VAL_STRING && node.value == c.string) found = i
                }
                if (found == -1) {
                    val type = when {
                        node is N_BOOLEAN -> VAL_BOOL
                        node is N_INTEGER -> VAL_INT
                        node is N_FLOAT -> VAL_FLOAT
                        node is N_VECTOR -> VAL_VECTOR
                        node is N_STRING -> VAL_STRING
                        node is N_COLOR -> VAL_COLOR
                        else -> VAL_NIL
                    }
                    constants.add(Value(type,
                            if (node is N_BOOLEAN) node.value else false,
                            if (node is N_INTEGER) node.value else 0,
                            if (node is N_FLOAT) node.value else 0.0f,
                            if (node is N_VECTOR) node.v1 else 0.0f,
                            if (node is N_VECTOR) node.v2 else 0.0f,
                            if (node is N_COLOR) node.c1 else 0.0f,
                            if (node is N_COLOR) node.c2 else 0.0f,
                            if (node is N_COLOR) node.c3 else 0.0f,
                            if (node is N_STRING) node.value else "",
                            null
                        ))
                    found = constants.lastIndex
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

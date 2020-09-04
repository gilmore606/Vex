package compiler

import compiler.nodes.Node
import compiler.nodes.Node.*
import compiler.nodes.*
import compiler.ValueType.*
import compiler.ObjectType.*

typealias FUNCLIST = ArrayList<FuncSig>
typealias TYPELIST = ArrayList<ValueType>

class Meaner (
        val ast: Node,
        val fVerbose: Boolean
) {

    val constants = ArrayList<Value>()
    val variables = ArrayList<Variable>()
    var globalCount = 0

    // Get ID of named variable in my scope
    fun variableToID(sourceNode: Node, name: String, scope: Node): Int {
        variables.forEachIndexed { i, v ->
            if ((i < globalCount) && (v.name == name)) {  // global
                if (sourceNode is N_VARIABLE) v.nodes.add(sourceNode)
                return v.id
            }
            if (v.scope == scope && v.name == name) {  // local
                if (sourceNode is N_VARIABLE) v.nodes.add(sourceNode)
                return v.id
            }
        }
        val v = Variable(variables.size, name, scope, null, null)
        if (sourceNode is N_VARIABLE) v.nodes.add(sourceNode)
        variables.add(v)
        return v.id
    }

    // Do we already know this var's type?
    fun varTypeKnown(varID: Int): Boolean = variables.find { it.id == varID }?.let { it.type != null } ?: false

    // Set type of all nodes for a variable
    fun learnVarType(varID: Int, type: ValueType, objtype: ObjectType?) {
        variables.find { it.id == varID }!!.also { v ->
            v.type = type
            v.objtype = objtype
            v.nodes.forEach {
                it.type = type
                it.objtype = objtype
            }
        }
    }

    // Look up signature of named function
    fun getFuncSig(name: String, args: List<N_EXPRESSION>): FuncSig {
        Syscalls.funcs.forEach { if (it.name == name) {
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
    fun getMethodSig(type: ObjectType, name: String, args: List<N_EXPRESSION>): FuncSig {
        Syscalls.classes[type]!!.forEach {
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
        findConstants()

        // Index state (global) variables, get count
        // All variables from 0-(globalCount-1) are globals
        globalCount = scopeGlobals()
        println("found " + globalCount + " globals")
        // Index local variables
        ast.scopeVars(ast, this)


        ast.print(0)

        // Set global types globally
        variables.forEachIndexed { i, v ->
            if (i < globalCount) {
                println("     " + v.toString())
                learnVarType(v.id, v.type!!, null)
            }
        }

        // Infer all local expression types
        annealTypes(ast)

        // Check types for sanity
        ast.traverse { it.checkType() }


        // TODO: Index all function entries

    }

    fun annealTypes(top: Node) {
        var settled = false
        while (!settled) {
            settled = true
            top.traverse {
                if (!it.setType(this)) settled = false
            }
        }
    }

    fun scopeGlobals(): Int {
        ast.findNode { it is N_TOP_STATE }?.also { state ->
            state.scopeVars(ast, this)
            annealTypes(state)
        }
        return variables.size
    }

    fun findConstants() {
        ast.traverse { node ->
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
    }
}

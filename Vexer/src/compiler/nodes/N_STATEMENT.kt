package compiler.nodes

import compiler.*
import compiler.Opcode.*
import compiler.ValueType.*
import compiler.nodes.Node.*

abstract class N_STATEMENT: Node()

class N_METHCALL(val obj: N_VARIABLE, val name: String, val args: List<N_EXPRESSION>): N_STATEMENT() {
    var sig: FuncSig? = null
    override fun toString() = "METH:" + obj.name + "." + name + "(" + args.joinToString(",") + ")"
    override fun kids(): NODES = super.kids().apply { add(obj); args.forEach { add(it) }}

    override fun setType(meaner: Meaner): Boolean {
        val oldsig = sig
        if (obj.type == VAL_OBJECT) {
            sig = meaner.getMethodSig(this, obj.objtype!!, name, args)
        }
        return oldsig == sig
    }

    override fun checkTypeSane() {
        if (obj.type != VAL_OBJECT) throw CompileException(this, "can't call method on non-object")
        sig!!.argtypes.forEachIndexed { i, argtype ->
            if (argtype != args[i].type) throw CompileException(this, "wrong arg type in system method call")
        }
    }

    override fun code(coder: Coder) {
        args.forEach { it.code(coder) }
        obj.code(coder)
        coder.code(OP_SMETH, sig!!.funcID)
    }
}

class N_EXPRSTATEMENT(val expr: N_EXPRESSION): N_STATEMENT() {
    override fun kids(): NODES = super.kids().apply { add(expr) }
    override fun code(coder: Coder) {
        expr.code(coder)
    }
}

class N_FUNCSTATEMENT(val name: String, val args: List<N_EXPRESSION>): N_STATEMENT() {
    var funID = -1
    var funSig: FuncSig? = null
    override fun toString() = "FUNC: " + name + "()"
    override fun kids(): NODES = super.kids().apply { args.forEach { add(it) }}

    override fun setType(meaner: Meaner): Boolean {
        var unchanged = true
        if (funID == -1) {
            var argsTyped = true
            args.forEach { if (it.type == null) argsTyped = false }
            if (argsTyped) {
                funSig = meaner.getFuncSig(this, name, args)
                funID = funSig!!.funcID
                unchanged = false
            }
        }
        return unchanged
    }

    override fun checkTypeSane() {
        if (funSig!!.returnType != VAL_NIL)
            throw CompileException(this, "function returns " + funSig!!.returnType + " and can't be a statement")
        args.forEachIndexed { i, a ->
            if (args[i].type != funSig!!.argtypes[i])
                throw CompileException(this, "type error: function takes " + funSig!!.argtypes[i].toString() + " but got " + args[i].type.toString())
        }
    }

    override fun code(coder: Coder) {
        args.forEach {
            it.code(coder)
        }
        coder.code(OP_FUN, funID)
    }
}

open class N_ASSIGN(val target: N_VARREF, val value: N_EXPRESSION): N_STATEMENT() {
    override fun toString() = "ASSIGN " + value.type + " to " + target.type
    override fun kids(): NODES = super.kids().apply { add(target); add(value) }

    override fun setType(meaner: Meaner): Boolean {
        val oldtype = target.type
        if ((target.type == null) && (value.type != null)) meaner.learnVarType(target.varID, value.type!!, value.objtype)
        return oldtype == target.type
    }

    override fun checkTypeSane() {
        if (target.type != value.type)
            throw CompileException(this, "type error: mismatched types in assignment")
    }

    override fun code(coder: Coder) {
        // Optimizations
        if ((target is N_VARIABLE) && (value is N_VARIABLE)) {
            coder.code(OP_V2VAR, value.varID, target.varID)
        } else if ((target is N_VARIABLE) && (value is N_LITERAL)) {
            if (value is N_BOOLEAN) {
                coder.code(if (value.value) OP_VTRUE else OP_VFALSE, target.varID)
                return
            }
            coder.code(OP_C2VAR, value.constID, target.varID)
        } else if ((target is N_VARIABLE) && (value is N_ADD) && (value.arg1 is N_VARIABLE) && (value.arg1.varID == target.varID)) {
            val added = value.arg2
            if ((added is N_INTEGER) && (target.type == VAL_INT)) {
                if (added.value == 1) {
                    coder.code(OP_INCVAR, target.varID)
                } else if (added.value == -1) {
                    coder.code(OP_DECVAR, target.varID)
                } else {
                    added.code(coder)
                    coder.code(OP_ACCVAR, target.varID)
                }
            } else if ((added is N_FLOAT) && (target.type == VAL_FLOAT)) {
                added.code(coder)
                coder.code(OP_ACCVARF, target.varID)
            } else {
                value.code(coder)
                target.codeSet(coder)
            }
        } else if ((target is N_VARIABLE) && (value is N_INVERSE) && (value.arg is N_VARIABLE) && (value.arg.varID == target.varID)) {
            coder.code(OP_VFLIP, target.varID)
        } else {
            value.code(coder)
            target.codeSet(coder)
        }
    }
}

class N_REPEAT(val count: N_EXPRESSION, val code: N_CODEBLOCK): N_STATEMENT() {
    var loopVarID = -1
    override fun kids(): NODES = super.kids().apply { add(count); add(code) }

    override fun scopeVars(scope: Node, meaner: Meaner) {
        super.scopeVars(scope, meaner)
        loopVarID = meaner.variableToID(this, "_loop"+id, scope)
    }

    override fun checkTypeSane() {
        if (count.type != VAL_INT)
            throw CompileException(this, "type error: repeat statement expects int count")
    }

    override fun code(coder: Coder) {
        count.code(coder)
        coder.code(OP_SETVAR, loopVarID)
        coder.code(OP_JUMPZ, loopVarID)  // skip block if count is already 0
        coder.jumpFuture("loopskip"+id)
        val repeatJumpID = coder.addJumpPoint()
        code.code(coder)
        coder.code(OP_DECVAR, loopVarID)
        coder.code(OP_JUMPNZ, loopVarID, repeatJumpID)
        coder.reachFuture("loopskip"+id)
    }
}

class N_EACH(val iterator: N_VARIABLE, val code: N_CODEBLOCK): N_STATEMENT() {
    override fun kids(): NODES = super.kids().apply { add(iterator); add(code) }
}

class N_IF(val condition: N_EXPRESSION, val ifblock: N_CODEBLOCK): N_STATEMENT() {
    override fun kids(): NODES = super.kids().apply { add(condition); add(ifblock); }

    override fun checkTypeSane() {
        if (condition.type != VAL_BOOL)
            throw CompileException(this, "type error: if statement expects boolean expr")
    }

    override fun code(coder: Coder) {
        condition.code(coder)
        coder.code(OP_IF)
        coder.jumpFuture("ifskip"+id)
        ifblock.code(coder)
        coder.reachFuture("ifskip"+id)
    }
}

class N_IFELSE(val condition: N_EXPRESSION, val ifblock: N_CODEBLOCK, val elseblock: N_CODEBLOCK): N_STATEMENT() {
    override fun kids(): NODES = super.kids().apply { add(condition); add(ifblock); add(elseblock) }

    override fun checkTypeSane() {
        if (condition.type != VAL_BOOL)
            throw CompileException(this, "type error: if statement expects boolean expr")
    }

    override fun code(coder: Coder) {
        condition.code(coder)
        coder.code(OP_IF)
        coder.jumpFuture("ifskip"+id)
        ifblock.code(coder)
        coder.code(OP_JUMP)
        coder.jumpFuture("elseskip"+id)
        coder.reachFuture("ifskip"+id)
        elseblock.code(coder)
        coder.reachFuture("elseskip"+id)
    }
}

class N_FOR(val index: N_VARIABLE, val start: N_EXPRESSION, val end: N_EXPRESSION, val code: N_CODEBLOCK): N_STATEMENT() {
    var endvarID = -1
    override fun kids(): NODES = super.kids().apply { add(index); add(start); add(end); add(code); }

    override fun scopeVars(scope: Node, meaner: Meaner) {
        super.scopeVars(scope, meaner)
        endvarID = meaner.variableToID(this, "_forend"+id, scope)
    }

    override fun setType(meaner: Meaner): Boolean {
        val oldtype = index.type
        meaner.learnVarType(index.varID, VAL_INT, null)
        return oldtype == index.type
    }

    override fun checkTypeSane() {
        if (start.type != VAL_INT || end.type != VAL_INT)
            throw CompileException(this, "type error: for loop can only count int")
    }

    override fun code(coder: Coder) {
        end.code(coder)
        coder.code(OP_SETVAR, endvarID)
        start.code(coder)
        index.codeSet(coder)
        val top = coder.addJumpPoint()
        coder.code(OP_VAR, endvarID)
        index.code(coder)
        coder.code(OP_LEI)
        coder.code(OP_IF)
        coder.jumpFuture("forskip"+id)
        code.code(coder)
        coder.code(OP_INCVAR, index.varID)
        coder.code(OP_JUMP, top)
        coder.reachFuture("forskip"+id)
    }
}

class N_WAIT(val time: N_EXPRESSION): N_STATEMENT() {
    override fun kids(): NODES = super.kids().apply { add(time) }

    override fun checkTypeSane() {
        if (time.type != VAL_INT)
            throw CompileException(this, "type error: wait statement expects int msec")
    }

    override fun code(coder: Coder) {
        time.code(coder)
        coder.code(OP_WAIT)
    }
}

class N_EVERY(val time: N_EXPRESSION, val code: N_CODEBLOCK): N_STATEMENT() {
    var elapsedVarID = -1
    var skiptimeVarID = -1
    override fun kids(): NODES = super.kids().apply { add(time); add(code) }
    override fun scopeVars(scope: Node, meaner: Meaner) {
        super.scopeVars(scope, meaner)
        elapsedVarID = meaner.variableToID(this, "_every" + id, scope)
        skiptimeVarID = meaner.variableToID(this, "_skiptime" + id, scope)
    }
    override fun checkTypeSane() {
        if (time.type != VAL_FLOAT) throw CompileException(this, "type error: every statement expects float sec")
    }
    override fun code(coder: Coder) {
        val deltaID = Syscalls.vars.find{it.name=="delta"}!!.id
        coder.code(OP_VAR, deltaID)
        coder.code(OP_ACCVARF, elapsedVarID)
        time.code(coder)
        coder.code(OP_DUBS)
        coder.code(OP_SETVAR, skiptimeVarID)
        coder.code(OP_VAR, elapsedVarID)
        coder.code(OP_GTF)
        coder.code(OP_IF)
        coder.jumpFuture("everyskip"+id)
        coder.code(OP_VAR, skiptimeVarID)
        coder.code(OP_NEGF)
        coder.code(OP_ACCVARF, elapsedVarID)
        code.code(coder)
        coder.reachFuture("everyskip"+id)
    }
}

class N_RETURN(val value: N_EXPRESSION?): N_STATEMENT() {
    lateinit var function: N_TOP_USERFUNC
    override fun kids(): NODES = super.kids().apply { value?.also { add(it) }}

    fun learnFunction(function: N_TOP_USERFUNC) { this.function = function }

    override fun checkTypeSane() {
        when {
            ((value == null) && (function.returnType != VAL_NIL)) ->
                throw CompileException(this, function.returnType.toString() + " function must return a value")
            ((value != null) && (function.returnType == VAL_NIL)) ->
                throw CompileException(this, "can't return a value from a null function")
            ((value != null) && (value.type != function.returnType)) ->
                throw CompileException(this, "can't return " + value.type.toString() + " from " + function.returnType.toString() + " function")
        }
    }

    override fun code(coder: Coder) {
        value?.also { value.code(coder) }
        coder.code(OP_RETURN)
    }
}

class N_LOG(val expr: N_EXPRESSION): N_STATEMENT() {
    override fun kids(): NODES = super.kids().apply { add(expr) }

    override fun code(coder: Coder) {
        expr.code(coder)
        coder.code(OP_DEBUG)
    }
}

package compiler

import compiler.Coder
import compiler.CompileException
import compiler.FuncSig
import compiler.Meaner
import compiler.Opcode.*
import compiler.*
import compiler.nodes.Node
import compiler.ValueType.*

object Syscalls {

    val vars = ArrayList<Variable>().apply {
        add(Variable(0, "delta", Node(), VAL_FLOAT))
    }

    val funcs = FUNCLIST().apply {
        add(FuncSig(true, "V", 0, VAL_VECTOR, null,
                TYPELIST().apply { add(VAL_FLOAT); add(VAL_FLOAT) }))
        add(FuncSig(true, "C", 1, VAL_COLOR, null,
                TYPELIST().apply { add(VAL_FLOAT); add(VAL_FLOAT); add(VAL_FLOAT) }))
        add(FuncSig(true, "RAND", 2, VAL_FLOAT, null,
                TYPELIST().apply { add(VAL_FLOAT); add(VAL_FLOAT) }))
        add(FuncSig(true, "RAND", 3, VAL_FLOAT, null,
                TYPELIST().apply { add(VAL_FLOAT) }))
        add(FuncSig(true, "RAND", 4, VAL_INT, null,
                TYPELIST().apply { add(VAL_INT); add(VAL_INT) }))
        add(FuncSig(true, "RAND", 5, VAL_INT, null,
                TYPELIST().apply { add(VAL_INT) }))
        add(FuncSig(true, "RAND", 6, VAL_FLOAT, null,
                TYPELIST() ))
        add(FuncSig(true, "MIN", 7, VAL_INT, null, 
                TYPELIST().apply { add(VAL_INT); add(VAL_INT) } ))
        add(FuncSig(true, "MAX", 8, VAL_INT, null,
                TYPELIST().apply { add(VAL_INT); add(VAL_INT) } ))
        add(FuncSig(true, "ABS", 9, VAL_INT, null,
                TYPELIST().apply { add(VAL_INT) } ))
        add(FuncSig(true, "SIGN", 10, VAL_INT, null,
                TYPELIST().apply { add(VAL_INT) } ))
        add(FuncSig(true, "MIN", 11, VAL_FLOAT, null,
                TYPELIST().apply { add(VAL_FLOAT); add(VAL_FLOAT) } ))
        add(FuncSig(true, "MAX", 12, VAL_FLOAT, null,
                TYPELIST().apply { add(VAL_FLOAT); add(VAL_FLOAT) } ))
        add(FuncSig(true, "ABS", 13, VAL_FLOAT, null,
                TYPELIST().apply { add(VAL_FLOAT) } ))
        add(FuncSig(true, "SIGN", 14, VAL_INT, null,
                TYPELIST().apply { add(VAL_FLOAT) } ))
        add(FuncSig(true, "SIN", 15, VAL_FLOAT, null,
                TYPELIST().apply { add(VAL_FLOAT) } ))
        add(FuncSig(true, "COS", 16, VAL_FLOAT, null,
                TYPELIST().apply { add(VAL_FLOAT) } ))
        add(FuncSig(true, "CLAMP", 17, VAL_INT, null,
                TYPELIST().apply { add(VAL_INT); add(VAL_INT); add(VAL_INT) } ))
        add(FuncSig(true, "CLAMP", 18, VAL_FLOAT, null,
                TYPELIST().apply { add(VAL_FLOAT); add(VAL_FLOAT); add(VAL_FLOAT) }))
        add(FuncSig(true, "SQRT", 19, VAL_FLOAT, null,
                TYPELIST().apply { add(VAL_FLOAT) }))
        add(FuncSig(true, "CHANCE", 20, VAL_BOOL, null,
                TYPELIST().apply { add(VAL_FLOAT) }))

        add(FuncSig(true, "TEXT", 30, VAL_OBJECT, ObjectType.OBJ_SPRITE,
                TYPELIST().apply { add(VAL_STRING) }))
    }

    val classes = HashMap<ObjectType, FUNCLIST>().also {

        it[ObjectType.OBJ_SPRITE] = FUNCLIST().apply {
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
}

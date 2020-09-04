package compiler

object Syscalls {

    val funcs = FUNCLIST().apply {
        add(FuncSig(true, "V", 0, ValueType.VAL_VECTOR, null,
                TYPELIST().apply { add(ValueType.VAL_FLOAT); add(ValueType.VAL_FLOAT) }))
        add(FuncSig(true, "C", 1, ValueType.VAL_COLOR, null,
                TYPELIST().apply { add(ValueType.VAL_FLOAT); add(ValueType.VAL_FLOAT); add(ValueType.VAL_FLOAT) }))
        add(FuncSig(true, "RAND", 2, ValueType.VAL_FLOAT, null,
                TYPELIST().apply { add(ValueType.VAL_FLOAT); add(ValueType.VAL_FLOAT) }))
        add(FuncSig(true, "RAND", 3, ValueType.VAL_FLOAT, null,
                TYPELIST().apply { add(ValueType.VAL_FLOAT) }))
        add(FuncSig(true, "RAND", 4, ValueType.VAL_INT, null,
                TYPELIST().apply { add(ValueType.VAL_INT); add(ValueType.VAL_INT) }))
        add(FuncSig(true, "RAND", 5, ValueType.VAL_INT, null,
                TYPELIST().apply { add(ValueType.VAL_INT) }))
        add(FuncSig(true, "RAND", 6, ValueType.VAL_FLOAT, null,
                TYPELIST() ))
        add(FuncSig(true, "TEXT", 7, ValueType.VAL_OBJECT, ObjectType.OBJ_SPRITE,
                TYPELIST().apply { add(ValueType.VAL_STRING) }))
    }

    val classes = HashMap<ObjectType, FUNCLIST>().also {

        it[ObjectType.OBJ_SPRITE] = FUNCLIST().apply {
            add(FuncSig(true, "move", 0, ValueType.VAL_NIL, null,
                    TYPELIST().apply { add(ValueType.VAL_FLOAT); add(ValueType.VAL_FLOAT) }))
            add(FuncSig(true, "move", 1, ValueType.VAL_NIL, null,
                    TYPELIST().apply { add(ValueType.VAL_VECTOR) }))
            add(FuncSig(true, "scale", 2, ValueType.VAL_NIL, null,
                    TYPELIST().apply { add(ValueType.VAL_FLOAT); add(ValueType.VAL_FLOAT) }))
            add(FuncSig(true, "scale", 3, ValueType.VAL_NIL, null,
                    TYPELIST().apply { add(ValueType.VAL_FLOAT) }))
            add(FuncSig(true, "hide", 4, ValueType.VAL_NIL, null,
                    TYPELIST()))
            add(FuncSig(true, "show", 5, ValueType.VAL_NIL, null,
                    TYPELIST()))
            add(FuncSig(true, "write", 6, ValueType.VAL_NIL, null,
                    TYPELIST().apply { add(ValueType.VAL_STRING) }))
        }

    }
}

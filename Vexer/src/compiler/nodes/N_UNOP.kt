package compiler.nodes

import compiler.Coder
import compiler.CompileException
import compiler.Opcode
import compiler.ValueType

abstract class N_UNOP(val arg: N_EXPRESSION): Node.N_EXPRESSION() {
    override fun setType(): ValueType {
        this.type = arg.setType()
        return this.type!!
    }
    override fun printMine(lvl: Int) {
        arg.print(lvl)
    }
}
class N_NEGATE(arg: N_EXPRESSION): N_UNOP(arg) {
    override fun setType(): ValueType {
        super.setType()
        this.type = arg.type
        return this.type!!
    }
    override fun code(coder: Coder) {
        arg.code(coder)
        when (this.type) {
            ValueType.VAL_INT -> coder.code(Opcode.OP_NEGI)
            ValueType.VAL_FLOAT -> coder.code(Opcode.OP_NEGF)
            ValueType.VAL_VECTOR -> coder.code(Opcode.OP_NEGV)
            else -> throw CompileException("type error: can only negate numeric values")
        }
    }
}
class N_INVERSE(arg: N_EXPRESSION): N_UNOP(arg) {
    override fun setType(): ValueType {
        super.setType()
        if (arg.type != ValueType.VAL_BOOL) throw CompileException("type error: can only NOT booleans")
        this.type = ValueType.VAL_BOOL
        return ValueType.VAL_BOOL
    }
    override fun code(coder: Coder) {
        arg.code(coder)
        coder.code(Opcode.OP_NOT)
    }
}

package compiler.nodes

import compiler.*
import compiler.Opcode.*
import compiler.ValueType.*
import compiler.nodes.Node.*
import model.Game

data class ParamInfo(val id: Int, val type: ValueType)
typealias PARAMMAP = HashMap<String,ParamInfo>

abstract class N_PARAM_STATEMENT(val params: List<N_PARAMSET>): N_STATEMENT() {
    abstract val paramInfo: PARAMMAP
    abstract val codeID: Int
    abstract fun codeParams(coder: Coder)
    override fun kids(): NODES = super.kids().apply { params.forEach { add(it) }}

    override fun setType(meaner: Meaner): Boolean {
        var unchanged = true
        params.forEachIndexed { i, param ->
            val info = paramInfo[param.target.name] ?: throw CompileException(this, "unknown parameter '" + param.target.name + "'")
            if (info.type != param.target.type) {
                param.target.type = info.type
                unchanged = false
            }
            if (info.id != param.paramID) {
                param.paramID = info.id
                unchanged = false
            }
        }
        return unchanged
    }

    override fun code(coder: Coder) {
        codeParams(coder)
        params.forEach {
            it.code(coder)
        }
        coder.code(OP_STAT, codeID, params.size)
    }
}

class N_PARAMSET(val target: N_PARAM, val value: N_EXPRESSION): N_STATEMENT() {
    override fun toString() = "SET PARAM"
    var paramID: Int = -1
    override fun kids(): NODES { return super.kids().apply { add(target); add(value) }}

    override fun checkTypeSane() {
        if (target.type != value.type)
            throw CompileException(this, "parameter value type mismatch (" + target.type.toString() + ", " + value.type.toString() + ")")
    }

    override fun code(coder: Coder) {
        value.code(coder)
        coder.code(OP_LDI, paramID)
    }
}

class N_PARTICLE(var partid: N_EXPRESSION, params: List<N_PARAMSET>): N_PARAM_STATEMENT(params) {
    override val codeID = 0
    override val paramInfo = PARAMMAP().apply {
        set("p", ParamInfo(0, VAL_VECTOR))
        set("v", ParamInfo(1, VAL_VECTOR))
        set("vv", ParamInfo(2, VAL_VECTOR))
        set("color0", ParamInfo(3, VAL_COLOR))
        set("color1", ParamInfo(4, VAL_COLOR))
        set("scale0", ParamInfo(5, VAL_VECTOR))
        set("scale1", ParamInfo(6, VAL_VECTOR))
        set("rot", ParamInfo(7, VAL_FLOAT))
        set("rotv", ParamInfo(8, VAL_FLOAT))
        set("lifetime", ParamInfo(9, VAL_FLOAT))
    }
    override fun toString() = "PARTICLE"
    override fun kids(): NODES = super.kids().apply { add(partid) }

    override fun resolveResources(config: Game) {
        if (partid is N_STRING) {
            config.particles.forEach {
                if (it.name == (partid as N_STRING).value) {
                    partid = N_INTEGER(it.id)
                    partid.type = VAL_INT
                }
            }
        }
    }

    override fun checkTypeSane() {
        if (partid.type != VAL_INT) throw CompileException(this, "particle id must be int")
    }

    override fun codeParams(coder: Coder) {
        partid.code(coder)
    }
}

class N_SONG(var songid: N_EXPRESSION, params: List<N_PARAMSET>): N_PARAM_STATEMENT(params) {
    override val codeID = 1
    override val paramInfo = PARAMMAP().apply {
        set("vol", ParamInfo(0, VAL_FLOAT))
        set("pan", ParamInfo(1, VAL_FLOAT))
        set("transpose", ParamInfo(2, VAL_INT))
        set("timescale", ParamInfo(3, VAL_FLOAT))
        set("loop", ParamInfo(4, VAL_BOOL))
    }
    override fun toString() = "SONG"
    override fun kids(): NODES = super.kids().apply { add(songid) }

    override fun resolveResources(config: Game) {
        if (songid is N_STRING) {
            config.songs.forEach {
                if (it.name == (songid as N_STRING).value) {
                    this.songid = N_INTEGER(it.id)
                    this.songid.type = VAL_INT
                    return
                }
            }
        }
    }

    override fun checkTypeSane() {
        if (songid.type != VAL_INT) throw CompileException(this, "song id must be int")
    }

    override fun codeParams(coder: Coder) {
        songid.code(coder)
    }
}


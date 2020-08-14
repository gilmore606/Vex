package compiler

import compiler.TokenType.*

class MethodSpec(
        val name: String,
        val args: ArrayList<String>
)

class Peeper(
    var tokens: ArrayList<Token>, val fVerbose: Boolean
) {
    enum class State {
        WATCHING, GOT_TO, GOT_NAME
    }
    val methods = ArrayList<MethodSpec>()
    var functionName = ""
    var functionArgs = ArrayList<String>()

    fun dump() {
        methods.forEach {
            println("  peeper saw: " + it.name + " (" + it.args.joinToString(" ") + ")")
        }
    }

    fun peep() {
        var state = State.WATCHING

        tokens.forEach { t ->
            when (state) {
                State.WATCHING -> {
                    if ((t.type == IDENTIFIER) && (t.string.equals("to"))) {
                        state = State.GOT_TO
                    }
                }
                State.GOT_TO -> {
                    if (t.type != IDENTIFIER) {
                        state = State.WATCHING
                    } else {
                        state = State.GOT_NAME
                        functionName = t.string
                    }
                }
                State.GOT_NAME -> {
                    if (t.type == COLON) {
                        finishFunction()
                    } else if (t.type != IDENTIFIER) {
                        state = State.WATCHING
                    } else {
                        functionArgs.add(t.string)
                    }
                }
            }
        }

        if (fVerbose) dump()
    }

    fun finishFunction() {
        methods.add(MethodSpec(functionName, functionArgs))
    }

}
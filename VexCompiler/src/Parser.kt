import TokenType.*

import java.lang.RuntimeException

class Parser(
    var tokens: ArrayList<Token>
) {
    val ast = ArrayList<Node>()

    // Print the full AST for debugging
    fun dumpTree() {
        println("AST DUMP:")
        ast.forEach { it.print(0) }
    }


    // Token lookahead functions

    fun getToken(): Token {
        val t = tokens.removeAt(0)
        return t
    }
    fun nextToken(skip: Int = 0): Token = if (skip >= tokens.size) Token(EOP) else tokens[skip]
    fun getToken(type: TokenType): Token {
        val t = getToken()
        if (t.type != type) throw RuntimeException()
        return t
    }
    fun requireTokens(vararg types: TokenType): Boolean {
        types.forEachIndexed { i, type ->
            if (nextToken(i).type != type) return false
        }
        return true
    }


    // Parse entry

    fun parse() {
        val blocks = ArrayList<Node.BLOCK>()
        while (tokens.isNotEmpty()) {
            blocks.add(parseTop())
        }
        ast.add(Node.PROGRAM(blocks))
        println("AST generated!")
    }


    // Parsing functions

    fun parseTop(): Node.BLOCK {
        val token = getToken()
        if (token.string.equals("to")) {
            val handler = getToken()
            if (nextToken().type != COLON) throw RuntimeException()
            return Node.CODEBLOCK(handler.string, parseCodeblock(1))
        } else if (nextToken().type == COLON) {
            getToken() // throw away the colon
            return when  {
                token.string.equals("settings") -> Node.SETTINGS(parseSettings())
                token.string.equals("controls") -> Node.CONTROLS(parseControls())
                else -> throw RuntimeException()
            }
        } else throw RuntimeException()
    }

    fun parseControls(): List<Node.CONTROLDEF> {
        val defs = ArrayList<Node.CONTROLDEF>()
        while (nextToken().type == INDENT) {
            getToken() // throw away the indent
            defs.add(parseControlDef())
        }
        return defs
    }
    fun parseControlDef(): Node.CONTROLDEF {
        val name = getToken(IDENTIFIER)
        if (getToken().type != PAREN_OPEN) throw RuntimeException()
        val params = ArrayList<Node.CONTROLPARAM>()
        while (nextToken().type != PAREN_CLOSE) {
            val token = getToken(IDENTIFIER)
            when {
                token.string.equals("switch") -> params.add(Node.CONTROL_SWITCH())
                token.string.equals("button") -> params.add(Node.CONTROL_BUTTON())
                token.string.equals("debounce") -> {
                    val value = getToken(INTEGER)
                    params.add(Node.CONTROL_DEBOUNCE(value.int))
                }
                else -> throw RuntimeException()
            }
        }
        getToken() // throw away the close paren
        return Node.CONTROLDEF(name.string, params)
    }

    fun parseSettings(): List<Node.ASSIGN> {
        return ArrayList()
    }

    fun parseCodeblock(depth: Int): List<Node.STATEMENT> {
        val statements = ArrayList<Node.STATEMENT>()
        while (true) {
            val nextStatement = parseStatement(depth)
            if (nextStatement == null) return statements
            else statements.add(nextStatement)
        }
    }

    fun parseStatement(depth: Int): Node.STATEMENT? {
        repeat (depth) { if (nextToken(it).type != INDENT) return null }
        repeat (depth) { getToken() }
        parseAssign()?.also { return it }
        return null
    }

    fun parseAssign(): Node.ASSIGN? {
        if (!requireTokens(IDENTIFIER, ASSIGN)) return null
        val identifier = getToken()
        getToken() // throw away the operator
        val expression = parseExpression()
        if (expression == null) throw RuntimeException()
        return Node.ASSIGN(Node.VARIABLE(identifier.string), expression)
    }

    fun parseExpression(): Node.EXPRESSION? {

        return null
    }
}

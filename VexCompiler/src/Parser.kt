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
        println("TOKENS REMAINING:")
        tokens.forEach { println(it.toString()) }
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
    fun requireOneOf(vararg types: TokenType): Boolean = nextToken().type in types


    // Parse entry

    fun parse() {
        dumpTree()
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
            if (getToken().type != COLON) throw RuntimeException()
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
        val expression = parseExpression() ?: throw RuntimeException()
        return Node.ASSIGN(Node.VARIABLE(identifier.string), expression)
    }

    fun parseExpression(): Node.EXPRESSION? {
        return parseAndOr()
    }

    fun parseAndOr(): Node.EXPRESSION? {
        var leftExpr = parseEquality() ?: return null
        while (requireOneOf(LOGIC_AND, LOGIC_OR)) {
            val operator = getToken()
            val rightExpr = parseEquality()
            if (rightExpr != null) {
                leftExpr = if (operator.type == LOGIC_AND) Node.LOGIC_AND(leftExpr, rightExpr) else Node.LOGIC_OR(leftExpr, rightExpr)
            }
        }
        return leftExpr
    }

    fun parseEquality(): Node.EXPRESSION? {
        var leftExpr = parseComparison() ?: return null
        while (requireOneOf(EQUALS, NOTEQUALS)) {
            val operator = getToken()
            val rightExpr = parseComparison()
            if (rightExpr != null) {
                leftExpr = if (operator.type == EQUALS) Node.LOGIC_EQUAL(leftExpr, rightExpr) else Node.LOGIC_NOTEQUAL(leftExpr, rightExpr)
            }
        }
        return leftExpr
    }

    fun parseComparison(): Node.EXPRESSION? {
        var leftExpr = parseAddition() ?: return null
        while (requireOneOf(GREATER_THAN, GREATER_EQUAL, LESS_THAN, LESS_EQUAL)) {
            val operator = getToken()
            val rightExpr = parseAddition()
            if (rightExpr != null) {
                leftExpr = when (operator.type) {
                    GREATER_THAN -> Node.LOGIC_GREATER(leftExpr, rightExpr)
                    GREATER_EQUAL -> Node.LOGIC_GREATER_EQUAL(leftExpr, rightExpr)
                    LESS_THAN -> Node.LOGIC_LESS(leftExpr, rightExpr)
                    LESS_EQUAL -> Node.LOGIC_LESS_EQUAL(leftExpr, rightExpr)
                    else -> throw RuntimeException()
                }
            }
        }
        return leftExpr
    }

    fun parseAddition(): Node.EXPRESSION? {
        var leftExpr = parsePower() ?: return null
        while (requireOneOf(ADD, SUBTRACT)) {
            val operator = getToken()
            val rightExpr = parsePower()
            if (rightExpr != null) {
                leftExpr = if (operator.type == ADD) Node.ADD(leftExpr, rightExpr) else Node.SUBTRACT(leftExpr, rightExpr)
            }
        }
        return leftExpr
    }

    fun parsePower(): Node.EXPRESSION? {
        var leftExpr = parseMultiplication() ?: return null
        while (requireTokens(POWER)) {
            getToken()
            val rightExpr = parseMultiplication()
            if (rightExpr != null) {
                leftExpr = Node.POWER(leftExpr, rightExpr)
            }
        }
        return leftExpr
    }

    fun parseMultiplication(): Node.EXPRESSION? {
        var leftExpr = parseUnary() ?: return null
        while (requireOneOf(MULTIPLY, DIVIDE)) {
            val operator = getToken()
            val rightExpr = parseUnary()
            if (rightExpr != null) {
                leftExpr = if (operator.type == MULTIPLY) Node.MULTIPLY(leftExpr, rightExpr) else Node.DIVIDE(leftExpr, rightExpr)
            }
        }
        return leftExpr
    }

    fun parseUnary(): Node.EXPRESSION? {
        if (requireOneOf(BANG, SUBTRACT)) {
            val operator = getToken()
            val rightExpr = parseUnary() ?: throw RuntimeException()
            return if (operator.type == BANG) Node.INVERSE(rightExpr) else Node.NEGATE(rightExpr)
        }
        return parseValue()
    }

    fun parseValue(): Node.EXPRESSION? {
        if (requireTokens(IDENTIFIER)) {
            val ident = getToken()
            if (ident.string.equals("true")) return Node.BOOLEAN(true)
            if (ident.string.equals("false")) return Node.BOOLEAN(false)
            return Node.VARIABLE(ident.string)
        }
        if (requireTokens(STRING)) return Node.STRING(getToken().string)
        if (requireTokens(INTEGER)) return Node.INTEGER(getToken().int)
        if (requireTokens(FLOAT)) return Node.FLOAT(getToken().float)
        if (requireTokens(PAREN_OPEN)) {
            getToken()
            val expr = parseExpression() ?: throw RuntimeException()
            if (getToken().type != PAREN_CLOSE) throw RuntimeException()
            return expr
        }
        return null
    }
}

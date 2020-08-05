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

    fun linePos() = if (tokens.isEmpty()) 0 else tokens[0].linePos
    fun charPos() = if (tokens.isEmpty()) 0 else tokens[0].charPos


    // Token lookahead functions

    fun getToken() = tokens.removeAt(0)
    fun tossNextToken() = getToken()
    fun expectToken(type: TokenType, failMessage: String): Token {
        val t = getToken()
        if (t.type != type) throw ParseException(this, failMessage)
        return t
    }
    fun nextToken(skip: Int = 0): Token = if (skip >= tokens.size) Token(EOP) else tokens[skip]
    fun nextTokenIs(type: TokenType) = nextToken().type == type
    fun nextTokensAre(vararg types: TokenType): Boolean {
        types.forEachIndexed { i, type ->
            if (nextToken(i).type != type) return false
        }
        return true
    }
    fun nextTokenOneOf(vararg types: TokenType): Boolean = nextToken().type in types


    // Parsing functions

    fun parse() {
        val blocks = ArrayList<Node.BLOCK>()
        while (tokens.isNotEmpty()) {
            blocks.add(parseTop())
        }
        ast.add(Node.PROGRAM(blocks))
        println("AST generated!")
    }

    fun parseTop(): Node.BLOCK {
        val token = getToken()
        if (token.string.equals("to")) {
            val handler = getToken()
            if (getToken().type != COLON) throw ParseException(this, "expected : after method declaration")
            return Node.CODEBLOCK(handler.string, parseCodeblock(1))
        } else if (nextToken().type == COLON) {
            tossNextToken()
            return when  {
                token.string.equals("settings") -> Node.SETTINGS(parseSettings())
                token.string.equals("controls") -> Node.CONTROLS(parseControls())
                else -> throw ParseException(this, "unknown top-level block type")
            }
        } else throw ParseException(this, "only top-level block declarations are allowed at top level")
    }

    fun parseControls(): List<Node.CONTROLDEF> {
        val defs = ArrayList<Node.CONTROLDEF>()
        while (nextToken().type == INDENT) {
            tossNextToken()
            defs.add(parseControlDef())
        }
        return defs
    }
    fun parseControlDef(): Node.CONTROLDEF {
        val name = expectToken(IDENTIFIER, "expected control name")
        if (getToken().type != PAREN_OPEN) throw ParseException(this, "expected ( to begin control definition")
        val params = ArrayList<Node.CONTROLPARAM>()
        while (nextToken().type != PAREN_CLOSE) {
            val token = expectToken(IDENTIFIER, "expected control modifier")
            when {
                token.string.equals("switch") -> params.add(Node.CONTROL_SWITCH())
                token.string.equals("button") -> params.add(Node.CONTROL_BUTTON())
                token.string.equals("debounce") -> {
                    val value = expectToken(INTEGER, "expected integer debounce value")
                    params.add(Node.CONTROL_DEBOUNCE(value.int))
                }
                else -> throw ParseException(this, "unknown control modifier")
            }
        }
        tossNextToken()
        return Node.CONTROLDEF(name.string, params)
    }

    fun parseSettings(): List<Node.ASSIGN> {
        return ArrayList()
    }

    fun parseCodeblock(depth: Int): List<Node.STATEMENT> {
        val statements = ArrayList<Node.STATEMENT>()
        while (true) {
            val nextStatement = parseStatement(depth) ?: return statements
            statements.add(nextStatement)
        }
    }

    fun parseStatement(depth: Int): Node.STATEMENT? {
        repeat (depth) { if (nextToken(it).type != INDENT) return null }
        repeat (depth) { tossNextToken() }

        parseAssign()?.also { return it }
        return null
    }

    fun parseAssign(): Node.ASSIGN? {
        if (!nextTokensAre(IDENTIFIER, ASSIGN)) return null
        val identifier = getToken()
        tossNextToken()
        val expression = parseExpression() ?: throw ParseException(this, "expected expression on right side of assignment")
        return Node.ASSIGN(Node.VARIABLE(identifier.string), expression)
    }

    fun parseExpression(): Node.EXPRESSION? {
        return parseAndOr()
    }

    fun parseAndOr(): Node.EXPRESSION? {
        var leftExpr = parseEquality() ?: return null
        while (nextTokenOneOf(LOGIC_AND, LOGIC_OR)) {
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
        while (nextTokenOneOf(EQUALS, NOTEQUALS)) {
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
        while (nextTokenOneOf(GREATER_THAN, GREATER_EQUAL, LESS_THAN, LESS_EQUAL)) {
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
        while (nextTokenOneOf(ADD, SUBTRACT)) {
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
        while (nextTokenIs(POWER)) {
            tossNextToken()
            val rightExpr = parseMultiplication()
            if (rightExpr != null) {
                leftExpr = Node.POWER(leftExpr, rightExpr)
            }
        }
        return leftExpr
    }

    fun parseMultiplication(): Node.EXPRESSION? {
        var leftExpr = parseUnary() ?: return null
        while (nextTokenOneOf(MULTIPLY, DIVIDE)) {
            val operator = getToken()
            val rightExpr = parseUnary()
            if (rightExpr != null) {
                leftExpr = if (operator.type == MULTIPLY) Node.MULTIPLY(leftExpr, rightExpr) else Node.DIVIDE(leftExpr, rightExpr)
            }
        }
        return leftExpr
    }

    fun parseUnary(): Node.EXPRESSION? {
        if (nextTokenOneOf(BANG, SUBTRACT)) {
            val operator = getToken()
            val rightExpr = parseUnary() ?: throw ParseException(this, "expected value after unary operator")
            return if (operator.type == BANG) Node.INVERSE(rightExpr) else Node.NEGATE(rightExpr)
        }
        return parseValue()
    }

    fun parseValue(): Node.EXPRESSION? {
        if (nextTokenIs(IDENTIFIER)) {
            val ident = getToken()
            if (ident.string.equals("true")) return Node.BOOLEAN(true)
            if (ident.string.equals("false")) return Node.BOOLEAN(false)
            if (nextTokenIs(PAREN_OPEN)) {
                tossNextToken()
                return Node.CALL(ident.string, parseArglist())
            }
            return Node.VARIABLE(ident.string)
        }
        if (nextTokenIs(STRING)) return Node.STRING(getToken().string)
        if (nextTokenIs(INTEGER)) return Node.INTEGER(getToken().int)
        if (nextTokenIs(FLOAT)) return Node.FLOAT(getToken().float)
        if (nextTokenIs(PAREN_OPEN)) {
            tossNextToken()
            val expr = parseExpression() ?: throw ParseException(this, "expected expression inside parens")
            if (getToken().type != PAREN_CLOSE) throw ParseException(this, "missing close paren")
            return expr
        }
        return null
    }

    fun parseArglist(): List<Node.EXPRESSION> {
        val args = ArrayList<Node.EXPRESSION>()
        while (!nextTokenIs(PAREN_CLOSE)) {
            parseExpression()?.also { args.add(it) }
            if (nextTokenIs(COMMA)) tossNextToken()
        }
        tossNextToken()
        return args
    }
}

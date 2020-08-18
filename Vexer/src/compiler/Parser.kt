package compiler

import compiler.TokenType.*
import java.lang.RuntimeException

class Parser(
    var tokens: ArrayList<Token>,
    var methodNames: ArrayList<MethodSpec>,
    val fVerbose: Boolean
) {
    val ast = ArrayList<Node>()

    // Print the full AST for debugging
    fun dumpTree() {
        println("AST DUMP:")
        ast.forEach { it.print(0) }
    }

    fun linePos() = if (tokens.isEmpty()) 0 else tokens[0].linePos
    fun charPos() = if (tokens.isEmpty()) 0 else tokens[0].charPos


    // compiler.Token lookahead functions

    fun getToken() = tokens.removeAt(0)
    fun tossNextToken() = getToken()
    fun expectToken(type: TokenType, failMessage: String): Token {
        val t = getToken()
        if (t.type != type) throw ParseException(this, failMessage)
        return t
    }
    fun nextToken(skip: Int = 0): Token = if (skip >= tokens.size) Token(
        EOP
    ) else tokens[skip]
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
    }

    fun parseTop(): Node.BLOCK {
        val token = getToken()
        if (token.string.equals("to")) {
            val handler = getToken()
            if (getToken().type != COLON) throw ParseException(
                this,
                "expected : after method declaration"
            )
            return Node.FUNCTION(handler.string, Node.CODEBLOCK(parseCodeblock(1))).also { it.tag(this) }
        } else if (nextToken().type == COLON) {
            tossNextToken()
            return when  {
                token.string.equals("settings") -> Node.SETTINGS(parseSettings())
                else -> throw ParseException(this, "unknown top-level block type")
            }
        } else throw ParseException(
            this,
            "only top-level block declarations are allowed at top level"
        )
    }

    fun parseSettings(): List<Node.ASSIGN> {
        val assigns = ArrayList<Node.ASSIGN>()
        while (true) {
            val nextAssign = parseStatement(1) ?: return assigns
            if (nextAssign !is Node.ASSIGN) throw ParseException(
                this,
                "only assignments allowed in settings block"
            )
            assigns.add(nextAssign)
        }
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

        parseRepeat(depth)?.also { return it }
        parseEach(depth)?.also { return it }
        parseAssign()?.also { return it }

        return null
    }

    fun parseAssign(): Node.ASSIGN? {
        if (!nextTokenIs(IDENTIFIER)) return null
        val identifier = parseIdentifier() ?: throw ParseException(
            this,
            "not a valid statement"
        )
        if (identifier !is Node.VARREF) throw ParseException(
            this,
            "left side of assignment must be a variable or propref"
        )
        tossNextToken()
        val expression = parseExpression() ?: throw ParseException(
            this,
            "expected expression on right side of assignment"
        )
        return Node.ASSIGN(identifier, expression).also { it.tag(this) }
    }

    fun parseRepeat(depth: Int): Node.REPEAT? {
        if (!(nextTokenIs(IDENTIFIER) && nextToken().string.equals("repeat"))) return null
        tossNextToken()
        val count = parseExpression() ?: throw ParseException(
            this,
            "expected count expression for repeat block"
        )
        expectToken(COLON, "expected colon after repeat count expression")
        val code = parseCodeblock(depth + 1)
        return Node.REPEAT(count, Node.CODEBLOCK(code)).also { it.tag(this) }
    }

    fun parseEach(depth: Int): Node.EACH? {
        if (!(nextTokenIs(IDENTIFIER) && nextToken().string.equals("each"))) return null
        tossNextToken()
        val iterator = parseValue() ?: throw ParseException(
            this,
            "expected iterator name for each loop"
        )
        if (!(iterator is Node.VARIABLE)) throw ParseException(
            this,
            "cannot each-iterate anything but a variable name"
        )
        expectToken(COLON, "expected colon after each iterator name")
        val code = parseCodeblock(depth + 1)
        return Node.EACH(iterator, Node.CODEBLOCK(code)).also { it.tag(this) }
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
                leftExpr = if (operator.type == LOGIC_AND) Node.LOGIC_AND(leftExpr, rightExpr).also { it.tag(this) }
                else Node.LOGIC_OR(leftExpr, rightExpr).also { it.tag(this) }
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
                leftExpr = if (operator.type == EQUALS) Node.LOGIC_EQUAL(leftExpr, rightExpr).also { it.tag(this) }
                else Node.LOGIC_NOTEQUAL(leftExpr, rightExpr).also { it.tag(this) }
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
                    GREATER_THAN -> Node.LOGIC_GREATER(leftExpr, rightExpr).also { it.tag(this) }
                    GREATER_EQUAL -> Node.LOGIC_GREATER_EQUAL(leftExpr, rightExpr).also { it.tag(this) }
                    LESS_THAN -> Node.LOGIC_LESS(leftExpr, rightExpr).also { it.tag(this) }
                    LESS_EQUAL -> Node.LOGIC_LESS_EQUAL(leftExpr, rightExpr).also { it.tag(this) }
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
                leftExpr = if (operator.type == ADD) Node.ADD(leftExpr, rightExpr).also { it.tag(this) }
                else Node.SUBTRACT(leftExpr, rightExpr).also { it.tag(this) }
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
                leftExpr = Node.POWER(leftExpr, rightExpr).also { it.tag(this) }
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
                leftExpr = if (operator.type == MULTIPLY) Node.MULTIPLY(leftExpr, rightExpr).also { it.tag(this) }
                else Node.DIVIDE(leftExpr, rightExpr).also { it.tag(this) }
            }
        }
        return leftExpr
    }

    fun parseUnary(): Node.EXPRESSION? {
        if (nextTokenOneOf(BANG, SUBTRACT)) {
            val operator = getToken()
            val rightExpr = parseUnary() ?: throw ParseException(
                this,
                "expected value after unary operator"
            )
            return if (operator.type == BANG) Node.INVERSE(rightExpr).also { it.tag(this) }
                else Node.NEGATE(rightExpr).also { it.tag(this) }
        }
        return parseValue()
    }

    fun parseValue(): Node.EXPRESSION? {
        if (nextTokenIs(IDENTIFIER)) return parseIdentifier()
        if (nextTokenIs(STRING)) return Node.STRING(getToken().string).also { it.tag(this) }
        if (nextTokenIs(INTEGER)) return Node.INTEGER(getToken().int).also { it.tag(this) }
        if (nextTokenIs(FLOAT)) return Node.FLOAT(getToken().float).also { it.tag(this) }
        if (nextTokenIs(PAREN_OPEN)) {
            tossNextToken()
            val expr = parseExpression() ?: throw ParseException(
                this,
                "expected expression inside parens"
            )
            if (getToken().type != PAREN_CLOSE) throw ParseException(
                this,
                "missing close paren"
            )
            return expr
        }
        return null
    }

    fun parseIdentifier(): Node.VALUE? {
        if (!nextTokenIs(IDENTIFIER)) return null
        val ident = getToken()
        if (ident.string.equals("true")) return Node.BOOLEAN(true).also { it.tag(this) }
        if (ident.string.equals("false")) return Node.BOOLEAN(false).also { it.tag(this) }
        if (nextTokenIs(PAREN_OPEN)) {
            tossNextToken()
            return Node.CALL(ident.string, parseArglist()).also { it.tag(this) }
        }
        if (nextTokenIs(DOTJOIN)) {
            tossNextToken()
            val propname = expectToken(IDENTIFIER, "expected identifier for property reference")
            return Node.PROPREF(ident.string, propname.string).also { it.tag(this) }
        }
        return Node.VARIABLE(ident.string).also { it.tag(this) }
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

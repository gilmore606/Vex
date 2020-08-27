package compiler

import compiler.TokenType.*
import compiler.nodes.Node
import compiler.nodes.Node.*
import compiler.nodes.*
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
        T_EOP
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
        val blocks = ArrayList<N_BLOCK>()
        while (tokens.isNotEmpty()) {
            blocks.add(parseTop())
        }
        ast.add(N_PROGRAM(blocks))
    }

    fun parseTop(): N_BLOCK {
        val token = getToken()
        if (token.string.equals("to")) {
            val handler = getToken()
            if (getToken().type != T_COLON) throw ParseException(
                this,
                "expected colon after method declaration"
            )
            if (fVerbose) println("parsing codeblock " + handler)
            return N_FUNCTION(handler.string, N_CODEBLOCK(parseCodeblock(1))).also { it.tag(this) }
        } else if (nextToken().type == T_COLON) {
            tossNextToken()
            return when  {
                token.string.equals("settings") -> N_SETTINGS(parseSettings())
                else -> throw ParseException(this, "unknown top-level block type")
            }
        } else throw ParseException(
            this,
            "only top-level block declarations are allowed at top level"
        )
    }

    fun parseSettings(): List<N_ASSIGN> {
        val assigns = ArrayList<N_ASSIGN>()
        while (true) {
            val nextAssign = parseStatement(1) ?: return assigns
            if (nextAssign !is N_ASSIGN) throw ParseException(
                this,
                "only assignments allowed in settings block"
            )
            assigns.add(nextAssign)
        }
    }

    fun parseCodeblock(depth: Int): List<N_STATEMENT> {
        val statements = ArrayList<N_STATEMENT>()
        while (true) {
            val nextStatement = parseStatement(depth) ?: return statements
            statements.add(nextStatement)
        }
    }

    fun parseStatement(depth: Int): N_STATEMENT? {
        repeat (depth) { if (nextToken(it).type != T_INDENT) return null }
        repeat (depth) { tossNextToken() }

        parseWait()?.also { return it }
        parseSound()?.also { return it }
        parseRepeat(depth)?.also { return it }
        parseEach(depth)?.also { return it }
        parseAssign()?.also { return it }

        return null
    }

    fun parseWait(): N_WAIT? {
        if (!(nextTokenIs(T_IDENTIFIER) && nextToken().string.equals("wait"))) return null
        tossNextToken()
        val time = parseExpression() ?: throw ParseException(this, "expected wait time expression")
        return N_WAIT(time).also { it.tag(this) }
    }

    fun parseSound(): N_SOUND? {
        if (!(nextTokenIs(T_IDENTIFIER) && nextToken().string.equals("sound"))) return null
        tossNextToken()
        val id = parseExpression() ?: throw ParseException(this, "expected sound id expression")
        return N_SOUND(id).also { it.tag(this) }
    }

    fun parseAssign(): N_ASSIGN? {
        if (!nextTokenIs(T_IDENTIFIER)) return null
        val identifier = parseIdentifier() ?: throw ParseException(this, "not a valid statement")
        if (identifier !is N_VARREF) throw ParseException(this, "left side of assignment must be a variable or propref")
        tossNextToken()
        val expression = parseExpression() ?: throw ParseException(this, "expected expression on right side of assignment")
        return N_ASSIGN(identifier, expression).also { it.tag(this) }
    }

    fun parseRepeat(depth: Int): N_REPEAT? {
        if (!(nextTokenIs(T_IDENTIFIER) && nextToken().string.equals("repeat"))) return null
        tossNextToken()
        val count = parseExpression() ?: throw ParseException(this, "expected count expression for repeat block")
        expectToken(T_COLON, "expected colon after repeat count expression")
        val code = parseCodeblock(depth + 1)
        return N_REPEAT(count, N_CODEBLOCK(code)).also { it.tag(this) }
    }

    fun parseEach(depth: Int): N_EACH? {
        if (!(nextTokenIs(T_IDENTIFIER) && nextToken().string.equals("each"))) return null
        tossNextToken()
        val iterator = parseValue() ?: throw ParseException(
            this,
            "expected iterator name for each loop"
        )
        if (!(iterator is N_VARIABLE)) throw ParseException(
            this,
            "cannot each-iterate anything but a variable name"
        )
        expectToken(T_COLON, "expected colon after each iterator name")
        val code = parseCodeblock(depth + 1)
        return N_EACH(iterator, N_CODEBLOCK(code)).also { it.tag(this) }
    }

    fun parseExpression(): N_EXPRESSION? {
        return parseAndOr()
    }

    fun parseAndOr(): N_EXPRESSION? {
        var leftExpr = parseEquality() ?: return null
        while (nextTokenOneOf(T_LOGIC_AND, T_LOGIC_OR)) {
            val operator = getToken()
            val rightExpr = parseEquality()
            if (rightExpr != null) {
                leftExpr = if (operator.type == T_LOGIC_AND) N_LOGIC_AND(leftExpr, rightExpr).also { it.tag(this) }
                else N_LOGIC_OR(leftExpr, rightExpr).also { it.tag(this) }
            }
        }
        return leftExpr
    }

    fun parseEquality(): N_EXPRESSION? {
        var leftExpr = parseComparison() ?: return null
        while (nextTokenOneOf(T_EQUALS, T_NOTEQUALS)) {
            val operator = getToken()
            val rightExpr = parseComparison()
            if (rightExpr != null) {
                leftExpr = if (operator.type == T_EQUALS) N_EQUAL(leftExpr, rightExpr).also { it.tag(this) }
                else N_NOTEQUAL(leftExpr, rightExpr).also { it.tag(this) }
            }
        }
        return leftExpr
    }

    fun parseComparison(): N_EXPRESSION? {
        var leftExpr = parseAddition() ?: return null
        while (nextTokenOneOf(T_GREATER_THAN, T_GREATER_EQUAL, T_LESS_THAN, T_LESS_EQUAL)) {
            val operator = getToken()
            val rightExpr = parseAddition()
            if (rightExpr != null) {
                leftExpr = when (operator.type) {
                    T_GREATER_THAN -> N_GREATER(leftExpr, rightExpr).also { it.tag(this) }
                    T_GREATER_EQUAL -> N_GREATER_EQUAL(leftExpr, rightExpr).also { it.tag(this) }
                    T_LESS_THAN -> N_LESS(leftExpr, rightExpr).also { it.tag(this) }
                    T_LESS_EQUAL -> N_LESS_EQUAL(leftExpr, rightExpr).also { it.tag(this) }
                    else -> throw RuntimeException()
                }
            }
        }
        return leftExpr
    }

    fun parseAddition(): N_EXPRESSION? {
        var leftExpr = parsePower() ?: return null
        while (nextTokenOneOf(T_ADD, T_SUBTRACT)) {
            val operator = getToken()
            val rightExpr = parsePower()
            if (rightExpr != null) {
                leftExpr = if (operator.type == T_ADD) N_ADD(leftExpr, rightExpr).also { it.tag(this) }
                else N_SUBTRACT(leftExpr, rightExpr).also { it.tag(this) }
            }
        }
        return leftExpr
    }

    fun parsePower(): N_EXPRESSION? {
        var leftExpr = parseMultiplication() ?: return null
        while (nextTokenIs(T_POWER)) {
            tossNextToken()
            val rightExpr = parseMultiplication()
            if (rightExpr != null) {
                leftExpr = N_POWER(leftExpr, rightExpr).also { it.tag(this) }
            }
        }
        return leftExpr
    }

    fun parseMultiplication(): N_EXPRESSION? {
        var leftExpr = parseUnary() ?: return null
        while (nextTokenOneOf(T_MULTIPLY, T_DIVIDE)) {
            val operator = getToken()
            val rightExpr = parseUnary()
            if (rightExpr != null) {
                leftExpr = if (operator.type == T_MULTIPLY) N_MULTIPLY(leftExpr, rightExpr).also { it.tag(this) }
                else N_DIVIDE(leftExpr, rightExpr).also { it.tag(this) }
            }
        }
        return leftExpr
    }

    fun parseUnary(): N_EXPRESSION? {
        if (nextTokenOneOf(T_BANG, T_SUBTRACT)) {
            val operator = getToken()
            val rightExpr = parseUnary() ?: throw ParseException(
                this,
                "expected value after unary operator"
            )
            return if (operator.type == T_BANG) N_INVERSE(rightExpr).also { it.tag(this) }
                else N_NEGATE(rightExpr).also { it.tag(this) }
        }
        return parseValue()
    }

    fun parseValue(): N_EXPRESSION? {
        if (nextTokenIs(T_IDENTIFIER)) return parseIdentifier()
        if (nextTokenIs(T_STRING)) return N_STRING(getToken().string).also { it.tag(this) }
        if (nextTokenIs(T_INTEGER)) return N_INTEGER(getToken().int).also { it.tag(this) }
        if (nextTokenIs(T_FLOAT)) return N_FLOAT(getToken().float).also { it.tag(this) }
        if (nextTokenIs(T_PAREN_OPEN)) {
            tossNextToken()
            val expr = parseExpression() ?: throw ParseException(
                this,
                "expected expression inside parens"
            )
            if (getToken().type != T_PAREN_CLOSE) throw ParseException(
                this,
                "missing close paren"
            )
            return expr
        }
        return null
    }

    fun parseIdentifier(): N_VALUE? {
        if (!nextTokenIs(T_IDENTIFIER)) return null
        val ident = getToken()
        if (ident.string.equals("true")) return N_BOOLEAN(true).also { it.tag(this) }
        if (ident.string.equals("false")) return N_BOOLEAN(false).also { it.tag(this) }
        return N_VARIABLE(ident.string).also { it.tag(this) }
    }

    fun parseArglist(): List<N_EXPRESSION> {
        val args = ArrayList<N_EXPRESSION>()
        while (!nextTokenIs(T_PAREN_CLOSE)) {
            parseExpression()?.also { args.add(it) }
            if (nextTokenIs(T_COMMA)) tossNextToken()
        }
        tossNextToken()
        return args
    }
}

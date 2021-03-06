package compiler

import compiler.TokenType.*
import compiler.nodes.Node
import compiler.nodes.Node.*
import compiler.nodes.*
import compiler.ValueType.*
import java.lang.RuntimeException

class Parser(
    var tokens: ArrayList<Token>,
    var methodNames: ArrayList<MethodSpec>,
    val fVerbose: Boolean
) {
    lateinit var ast: Node

    fun linePos() = if (tokens.isEmpty()) 0 else tokens[0].linePos
    fun charPos() = if (tokens.isEmpty()) 0 else tokens[0].charPos


    // Token lookahead functions

    fun getToken() = if (tokens.isEmpty()) Token(T_EOP) else tokens.removeAt(0)
    fun toss() = getToken()
    fun tossToNextLine() {
        while (nextToken().type == T_INDENT) { toss() }
    }
    fun tossDepth(depth: Int): Boolean {
        repeat (depth) { if (nextToken(it).type != T_INDENT) return false }
        repeat (depth) { toss() }
        return true
    }
    fun expectToken(type: TokenType, failMessage: String): Token {
        val t = getToken()
        if (t.type != type) throw ParseException(this, failMessage)
        return t
    }
    fun nextToken(skip: Int = 0): Token = if (skip >= tokens.size) Token(T_EOP) else tokens[skip]
    fun nextTokenIs(type: TokenType) = nextToken().type == type
    fun nextTokensAre(vararg types: TokenType): Boolean {
        types.forEachIndexed { i, type ->
            if (nextToken(i).type != type) return false
        }
        return true
    }
    fun nextTokenOneOf(vararg types: TokenType): Boolean = nextToken().type in types
    fun nextTokenOneOf(vararg strings: String): Boolean = (nextToken().type == T_IDENTIFIER) && (nextToken().string in strings)
    fun nextIDIs(ident: String): Boolean {
        if (nextToken().type != T_IDENTIFIER) return false
        return nextToken().string == ident
    }
    fun nextIDnextLineIs(ident: String): Boolean {
        var i = 0
        while (nextToken(i).type == T_INDENT) i++
        if (nextToken(i).type != T_IDENTIFIER) return false
        return nextToken(i).string == ident
    }


    // Top level

    fun parse() {
        val blocks = ArrayList<N_TOPBLOCK>()
        while (tokens.isNotEmpty()) {
            var done = true
            parseTop(0)?.also { blocks.add(it) ; done = false }
            while (nextTokenOneOf(T_INDENT, T_COMMENT)) toss()
            if (done && tokens.isNotEmpty()) throw ParseException(this, "code outside top blocks")
        }
        ast = N_PROGRAM(blocks)
    }

    fun parseTop(depth: Int): N_TOPBLOCK? {
        parseOnMethod(depth)?.also { return it }
        parseToMethod(depth)?.also { return it }
        parseState(depth)?.also { return it }
        return null
    }


    fun parseOnMethod(depth: Int): N_TOPBLOCK? {
        if (!(nextIDIs("on"))) return null
        toss()
        val handler = getToken().string
        expectToken(T_COLON, "expected colon after method declaration")
        if (handler == "button") {
            return parseButtonHandlers(depth+1)
        }
        val code = N_CODEBLOCK(parseCodeblock(depth + 1)).also { it.tag(this) }
        return N_TOP_HANDLER(handler, code).also { it.tag(this) }
    }

    fun parseToMethod(depth: Int): N_TOPBLOCK? {
        if (!(nextIDIs("to"))) return null
        toss()
        var name = ""
        var args = ArrayList<FuncArg>()
        if (nextTokenIs(T_IDENTIFIER)) {
            name = getToken().string
        } else if (nextTokenIs(T_IDENTIFUNC)) {
            name = getToken().string
            args = parseArgspec();
        }
        var returnType = VAL_NIL;
        if (nextTokenIs(T_IDENTIFIER) && Syscalls.typeKeywords.contains(nextToken().string)) {
            returnType = Syscalls.typeKeywords[getToken().string]!!
        }
        expectToken(T_COLON, "expected colon after function def")
        val code = parseCodeblock(depth + 1)
        return N_TOP_USERFUNC(name, args, returnType, N_CODEBLOCK(code).also { it.tag(this) }).also { it.tag(this) }
    }

    fun parseButtonHandlers(depth: Int): N_TOPBLOCK? {
        tossDepth(depth) || return null
        val handlers = ArrayList<N_BUTTON_HANDLER>()
        while (nextTokensAre(T_IDENTIFIER, T_COLON)) {
            val button = getToken().string
            toss()
            val code = parseCodeblock(depth + 1)
            handlers.add(N_BUTTON_HANDLER(button, N_CODEBLOCK(code).also { it.tag(this) }).also { it.tag(this) })
            tossDepth(depth) || break
        }
        return N_TOP_BUTTONS(handlers).also { it.tag(this) }
    }

    fun parseState(depth: Int): N_TOPBLOCK? {
        if (!(nextIDIs("state"))) return null
        toss()
        expectToken(T_COLON, "expected colon after state block declaration")
        val assigns = N_CODEBLOCK(parseAssignblock(depth + 1)).also { it.tag(this) }
        return N_TOP_STATE(assigns).also { it.tag(this) }
    }

    fun parseCodeblock(depth: Int): List<N_STATEMENT> {
        val statements = ArrayList<N_STATEMENT>()
        while (true) {
            val nextStatements = parseStatement(depth) ?: return statements
            statements.addAll(nextStatements)
        }
    }

    fun parseAssignblock(depth: Int): List<N_STATEMENT> {
        val statements = ArrayList<N_ASSIGN>()
        while (true) {
            repeat (depth) { if (nextToken(it).type != T_INDENT) return statements }
            repeat (depth) { toss() }
            val nextAssign = parseAssign() ?: return statements
            statements.add(nextAssign)
        }
    }


    // STATEMENTS

    fun parseStatement(depth: Int): List<N_STATEMENT>? {
        tossDepth(depth) || return null
        val l = ArrayList<N_STATEMENT>()
        parseParticle(depth)?.also { l.add(it); return l }
        parseSong(depth)?.also { l.add(it); return l }
        parseFuncStatement(depth)?.also { l.add(it); return l }
        parseMethcalls()?.also { return it }
        parseReturn()?.also { l.add(it); return l }
        parseIf(depth)?.also { l.add(it); return l }
        parseWait()?.also { l.add(it); return l }
        parseLog()?.also { l.add(it); return l }
        parseEvery(depth)?.also { l.add(it); return l }
        parseRepeat(depth)?.also { l.add(it); return l }
        parseFor(depth)?.also { l.add(it); return l }
        parseEach(depth)?.also { l.add(it); return l }
        parseAssign()?.also { l.add(it); return l }
        parseExprStatement(depth)?.also { l.add(it); return l }

        return null
    }

    fun parseParticle(depth: Int): N_PARTICLE? {
        if (!(nextIDIs("spark"))) return null
        toss()
        val partid = parseExpression() ?: throw ParseException(this, "expected particle id expression")
        val params = parseParams(depth + 1)
        return N_PARTICLE(partid, params).also { it.tag(this) }
    }

    fun parseSong(depth: Int): N_SONG? {
        if (!(nextIDIs("song"))) return null
        toss()
        val songid = parseExpression() ?: throw ParseException(this, "expected song id expression")
        val params = parseParams(depth + 1)
        return N_SONG(songid, params).also { it.tag(this) }
    }

    fun parseParams(depth: Int): List<N_PARAMSET> {
        if (!nextTokenIs(T_COLON)) return ArrayList()
        toss()
        val params = parseCodeblock(depth)
        val checked = ArrayList<N_PARAMSET>()
        params.forEach {
            if (it !is N_ASSIGN) throw ParseException(this, "only assignments allowed in param block")
            if (it.target !is N_VARIABLE) throw ParseException(this, "only plain param names allowed in param block")
            val p = N_PARAM(it.target.name).also { it.tag(this) }
            val pset = N_PARAMSET(p, it.value).also { it.tag(this) }
            checked.add(pset)
        }
        return checked
    }

    fun parseExprStatement(depth: Int): N_EXPRSTATEMENT? {
        val expr = parseExpression() ?: return null
        if (expr is N_INCDEC) {
            expr.isStatement = true
            return N_EXPRSTATEMENT(expr).also { it.tag(this) }
        } else throw ParseException(this, "illegal statement expression")
    }

    fun parseFuncStatement(depth: Int): N_FUNCSTATEMENT? {
        if (!nextTokenIs(T_IDENTIFUNC)) return null
        val name = getToken().string
        val args = parseArglist()
        return N_FUNCSTATEMENT(name, args).also { it.tag(this) }
    }

    fun parseMethcalls(): List<N_METHCALL>? {
        if (!nextTokensAre(T_IDENTIFIER, T_DOTJOIN, T_IDENTIFUNC)) return null
        val objname = getToken().string
        val calls = ArrayList<N_METHCALL>()
        while (nextTokenIs(T_DOTJOIN)) {
            toss()
            val meth = getToken().string
            val args = parseArglist()
            calls.add(N_METHCALL(N_VARIABLE(objname).also{ it.tag(this) }, meth, args).also { it.tag(this) })
        }
        return if (calls.isNotEmpty()) calls else null
    }

    fun parseReturn(): N_STATEMENT? {
        if (!nextIDIs("return")) return null
        toss()
        if (nextTokenOneOf(T_INDENT, T_EOP)) return N_RETURN(null)
        return N_RETURN(parseExpression()?.also { it.tag(this) })
    }

    fun parseIf(depth: Int): N_STATEMENT? {
        if (!(nextIDIs("if"))) return null
        toss()
        val test = parseExpression() ?: throw ParseException(this, "expected test expr after if")
        val code = parseStatementOrBlock(depth + 1)
        if (!(nextIDnextLineIs("else"))) return N_IF(test, N_CODEBLOCK(code).also { it.tag(this) }).also { it.tag(this) }
        tossToNextLine()
        toss()
        val elseblock = parseStatementOrBlock(depth + 1)
        return N_IFELSE(test, N_CODEBLOCK(code).also { it.tag(this) }, N_CODEBLOCK(elseblock).also { it.tag(this) }).also { it.tag(this) }
    }

    fun parseStatementOrBlock(depth: Int): List<N_STATEMENT> {
        if (nextTokenIs(T_COLON)) {
            toss()
            return parseCodeblock(depth)
        }
        return parseStatement(0) ?: throw ParseException(this, "expected statement or codeblock")
    }

    fun parseWait(): N_WAIT? {
        if (!(nextIDIs("wait"))) return null
        toss()
        val time = parseExpression() ?: throw ParseException(this, "expected wait time expression")
        return N_WAIT(time).also { it.tag(this) }
    }

    fun parseEvery(depth: Int): N_EVERY? {
        if (!(nextIDIs("every"))) return null
        toss()
        val time = parseExpression() ?: throw ParseException(this, "expected time expression for every block")
        val code = parseStatementOrBlock(depth + 1)
        return N_EVERY(time, N_CODEBLOCK(code).also { it.tag(this) }).also { it.tag(this) }
    }

    fun parseLog(): N_LOG? {
        if (!(nextIDIs("log"))) return null
        toss()
        val expr = parseExpression() ?: throw ParseException(this, "expected expression to log")
        return N_LOG(expr).also { it.tag(this) }
    }

    fun parseRepeat(depth: Int): N_REPEAT? {
        if (!(nextIDIs("repeat"))) return null
        toss()
        val count = parseExpression() ?: throw ParseException(this, "expected count expression for repeat block")
        val code = parseStatementOrBlock(depth + 1)
        return N_REPEAT(count, N_CODEBLOCK(code).also { it.tag(this) }).also { it.tag(this) }
    }

    fun parseFor(depth: Int): N_FOR? {
        if (!(nextIDIs("for"))) return null
        toss()
        val index = parseIdentifier() ?: throw ParseException(this, "expected 'for var...'")
        val start = parseExpression() ?: throw ParseException(this, "expected 'for var startexpr...'")
        if (!(nextIDIs("to"))) throw ParseException(this, "expected 'for var startexpr to...'")
        toss()
        val end = parseExpression() ?: throw ParseException(this, "expected 'for var startexpr to endexpr'")
        expectToken(T_COLON, "expected colon after for statement")
        val code = parseCodeblock(depth + 1)
        return N_FOR(index as N_VARIABLE, start, end, N_CODEBLOCK(code)).also { it.tag(this) }
    }

    fun parseEach(depth: Int): N_EACH? {
        if (!(nextIDIs("each"))) return null
        toss()
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

    fun parseAssign(): N_ASSIGN? {
        if (!nextTokenIs(T_IDENTIFIER)) return null
        val op = nextToken(1).type
        if (!(op==T_ASSIGN || op==T_ADD_ASSIGN || op==T_SUBTRACT_ASSIGN || op==T_MULT_ASSIGN || op==T_DIV_ASSIGN)) return null

        val identifier = parseIdentifier() ?: throw ParseException(this, "not a valid statement")
        if (identifier !is N_VARREF) throw ParseException(this, "left side of assignment must be a variable or propref")
        val oper = getToken()
        val expression = parseExpression() ?: throw ParseException(this, "expected expression on right side of assignment")
        return when (oper.type) {
            T_ASSIGN -> N_ASSIGN(identifier, expression)
            T_ADD_ASSIGN -> N_ASSIGN_ADD(identifier, expression)
            T_SUBTRACT_ASSIGN -> N_ASSIGN_SUB(identifier, expression)
            T_MULT_ASSIGN -> N_ASSIGN_MULT(identifier, expression)
            T_DIV_ASSIGN -> N_ASSIGN_DIV(identifier, expression)
            else -> throw ParseException(this, "illegal statement operator")
        }.also { it.tag(this) }
    }


    // EXPRESSIONS

    fun parseExpression(): N_EXPRESSION? {
        return parseAndOr()
    }

    fun parseAndOr(): N_EXPRESSION? {
        var leftExpr = parseConditional() ?: return null
        while (nextTokenOneOf(T_LOGIC_AND, T_LOGIC_OR) || nextTokenOneOf("and", "or")) {
            var operator = getToken()
            if (operator.type == T_IDENTIFIER) {
                operator = Token(if (operator.string == "and") T_LOGIC_AND else T_LOGIC_OR)
            }
            val rightExpr = parseConditional()
            if (rightExpr != null) {
                leftExpr = if (operator.type == T_LOGIC_AND) N_LOGIC_AND(leftExpr, rightExpr).also { it.tag(this) }
                else N_LOGIC_OR(leftExpr, rightExpr).also { it.tag(this) }
            }
        }
        return leftExpr
    }

    fun parseConditional(): N_EXPRESSION? {
        val cond = parseEquality() ?: return null
        if (nextTokenIs(T_QUESTION)) {
            toss()
            val yesval = parseEquality() ?: throw ParseException(this, "expected expr after conditional")
            if (nextTokenIs(T_COLON)) {
                toss()
                val noval = parseEquality() ?: throw ParseException(this, "expected expr after conditional")
                return N_CONDITIONAL(cond, yesval, noval).also { it.tag(this) }
            }
        }
        return cond
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
        var leftExpr = parseRotation() ?: return null
        while (nextTokenOneOf(T_ADD, T_SUBTRACT)) {
            val operator = getToken()
            val rightExpr = parseRotation()
            if (rightExpr != null) {
                leftExpr = if (operator.type == T_ADD) N_ADD(leftExpr, rightExpr).also { it.tag(this) }
                else N_SUBTRACT(leftExpr, rightExpr).also { it.tag(this) }
            }
        }
        return leftExpr
    }

    fun parseRotation(): N_EXPRESSION? {
        var leftExpr = parsePower() ?: return null
        while (nextTokenIs(T_ATSIGN)) {
            toss()
            val rightExpr = parsePower()
            if (rightExpr != null) {
                leftExpr = N_ROTATE(leftExpr, rightExpr).also { it.tag(this) }
            }
        }
        return leftExpr
    }

    fun parsePower(): N_EXPRESSION? {
        var leftExpr = parseMultiplication() ?: return null
        while (nextTokenIs(T_POWER)) {
            toss()
            val rightExpr = parseMultiplication()
            if (rightExpr != null) {
                leftExpr = N_POWER(leftExpr, rightExpr).also { it.tag(this) }
            }
        }
        return leftExpr
    }

    fun parseMultiplication(): N_EXPRESSION? {
        var leftExpr = parseUnary() ?: return null
        while (nextTokenOneOf(T_MULTIPLY, T_DIVIDE, T_MODULUS)) {
            val operator = getToken()
            val rightExpr = parseUnary()
            if (rightExpr != null) {
                leftExpr = when (operator.type) {
                    T_MULTIPLY -> N_MULTIPLY(leftExpr, rightExpr).also { it.tag(this) }
                    T_MODULUS -> N_MODULO(leftExpr, rightExpr).also { it.tag(this) }
                    else -> N_DIVIDE(leftExpr, rightExpr).also { it.tag(this) }
                }
            }
        }
        return leftExpr
    }

    fun parseUnary(): N_EXPRESSION? {
        if (nextTokenOneOf(T_BANG, T_SUBTRACT)) {
            val operator = getToken()
            val rightExpr = parseExpression() ?: throw ParseException(this, "expected value after unary operator")
            return if (operator.type == T_BANG) N_INVERSE(rightExpr).also { it.tag(this) }
                else N_NEGATE(rightExpr).also { it.tag(this) }
        }
        if (nextTokenOneOf(T_INCREMENT, T_DECREMENT)) {
            val operator = getToken()
            val rightExpr = parseExpression() ?: throw ParseException(this, "expected varref after increment/decrement")
            return N_INCDEC(operator.type, rightExpr, false, false).also { it.tag(this) }
        }
        return parseValue()
    }

    fun parseValue(): N_EXPRESSION? {
        if (nextTokensAre(T_IDENTIFIER, T_INCREMENT) || nextTokensAre(T_IDENTIFIER, T_DECREMENT)) {
            val varname = getToken().string
            val operator = getToken().type
            return N_INCDEC(operator, N_VARIABLE(varname), true, false).also { it.tag(this) }
        }
        if (nextTokenIs(T_IDENTIFIER)) return parseIdentifier()
        if (nextTokenIs(T_IDENTIFUNC)) {
            parseVector()?.also { return it }
            parseColor()?.also { return it }
            parseFuncall()?.also { return it }
        }
        if (nextTokenIs(T_STRING)) return N_STRING(getToken().string).also { it.tag(this) }
        if (nextTokenIs(T_INTEGER)) return N_INTEGER(getToken().int).also { it.tag(this) }
        if (nextTokenIs(T_FLOAT)) return N_FLOAT(getToken().float).also { it.tag(this) }
        if (nextTokenIs(T_STRING)) return N_STRING(getToken().string).also { it.tag(this) }
        if (nextTokenIs(T_PAREN_OPEN)) {
            toss()
            val expr = parseExpression() ?: throw ParseException(this, "expected expression inside parens")
            if (getToken().type != T_PAREN_CLOSE) throw ParseException(this, "missing close paren")
            return expr
        }
        return null
    }

    fun parseVector(): N_VECTOR? {
        if (nextTokensAre(T_IDENTIFUNC, T_FLOAT, T_COMMA, T_FLOAT, T_PAREN_CLOSE)) {
            if (nextToken().string == "V") {
                toss()
                val v1 = getToken()
                toss()
                val v2 = getToken()
                toss()
                return N_VECTOR(v1.float, v2.float).also { it.tag(this) }
            }
        }
        return null
    }

    fun parseColor(): N_COLOR? {
        if (nextTokensAre(T_IDENTIFUNC, T_FLOAT, T_COMMA, T_FLOAT, T_COMMA, T_FLOAT, T_PAREN_CLOSE)) {
            if (nextToken().string == "C") {
                toss()
                val c1 = getToken()
                toss()
                val c2 = getToken()
                toss()
                val c3 = getToken()
                toss()
                return N_COLOR(c1.float, c2.float, c3.float).also { it.tag(this) }
            }
        }
        return null
    }

    fun parseFuncall(): N_FUNCALL? {
        val name = getToken().string
        val args = parseArglist()
        return N_FUNCALL(name, args)
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
            if (nextTokenIs(T_COMMA)) toss()
        }
        toss()
        return args
    }

    fun parseArgspec(): ArrayList<FuncArg> {
        val args = ArrayList<FuncArg>()
        while (!nextTokenIs(T_PAREN_CLOSE)) {
            if (!nextTokensAre(T_IDENTIFIER, T_IDENTIFIER)) throw ParseException(this, "expected name and type of function arg")
            val name = getToken().string
            val type = Syscalls.typeKeywords[getToken().string] ?: throw ParseException(this, "invalid type specifier")
            args.add(FuncArg(name, type))
            if (!nextTokenIs(T_PAREN_CLOSE)) {
                expectToken(T_COMMA, "expected comma separating function args")
            }
        }
        toss()
        return args
    }
}

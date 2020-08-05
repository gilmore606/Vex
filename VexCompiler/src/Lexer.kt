import TokenType.*

class Lexer {

    var inType: TokenType? = null
    var inBuf: String = ""
    var linePos: Int = 0
    var charPos: Int = 0
    var spaceCount: Int = 0

    val outBuffer = ArrayList<Token>()

    fun process(c: Char, isReprocess: Boolean = false) {
        if (!isReprocess) {
            if (c == '\n') { linePos++ ; charPos = 0 }
            else charPos++
        }
        if (inType != null) {
            when (inType) {
                ASSIGN ->
                    if (c == '=') emit(Token(EQUALS)) else finish(Token(ASSIGN), c)
                NOTEQUALS ->
                    if (c == '=') emit(Token(NOTEQUALS)) else finish(Token(BANG), c)
                GREATER_THAN ->
                    if (c == '=') emit(Token(GREATER_EQUAL)) else finish(Token(GREATER_THAN), c)
                LESS_THAN ->
                    if (c == '=') emit(Token(LESS_EQUAL)) else finish(Token(LESS_THAN), c)
                ADD -> when (c) {
                    '=' -> emit(Token(ADD_ASSIGN))
                    '+' -> emit(Token(INCREMENT))
                    else -> finish(Token(ADD), c)
                }
                SUBTRACT -> when (c) {
                    '=' -> emit(Token(SUBTRACT_ASSIGN))
                    '-' -> emit(Token(DECREMENT))
                    else -> finish(Token(SUBTRACT), c)
                }
                DIVIDE ->
                    if (c == '/') inType = COMMENT else finish(Token(DIVIDE), c)
                LOGIC_OR ->
                    if (c == '|') emit(Token(LOGIC_OR)) else throw LexException(this, "expected ||")
                LOGIC_AND ->
                    if (c == '&') emit(Token(LOGIC_AND)) else throw LexException(this, "expected &&")
                STRING ->
                    if (c == '"') emit(Token(STRING)) else inBuf += c
                COMMENT ->
                    if (c == '\n') emit(Token(COMMENT)) else inBuf += c
                INTEGER -> when (c) {
                    '.' -> { inType = FLOAT; inBuf += c }
                    in '0'..'9' -> { inBuf += c }
                    else -> finish(Token(INTEGER, "", inBuf.toInt()), c)
                }
                FLOAT -> when (c) {
                    in '0'..'9' -> { inBuf += c }
                    else -> finish(Token(FLOAT, "", 0, inBuf.toFloat()), c)
                }
                IDENTIFIER ->
                    if (isIdentChar(c)) inBuf += c else finish(Token(IDENTIFIER, inBuf), c)
                INDENT ->
                    if (c == ' ') {
                        if (spaceCount == 3) emit(Token(INDENT)) else spaceCount++
                    } else finish(null, c)
                else -> { }
            }
        } else {
            when (c) {
                '(' -> emit(Token(PAREN_OPEN))
                ')' -> emit(Token(PAREN_CLOSE))
                ':' -> emit(Token(COLON))
                ',' -> emit(Token(COMMA))
                '=' -> begin(ASSIGN)
                '>' -> begin(GREATER_THAN)
                '<' -> begin(LESS_THAN)
                '+' -> begin(ADD)
                '-' -> begin(SUBTRACT)
                '*' -> emit(Token(MULTIPLY))
                '/' -> begin(DIVIDE)
                '.' -> emit(Token(DOTJOIN))
                '!' -> begin(NOTEQUALS)
                '"' -> begin(STRING)
                '^' -> emit(Token(POWER))
                '\t' -> emit(Token(INDENT))
                '|' -> begin(LOGIC_OR)
                '&' -> begin(LOGIC_AND)
                ' ' -> { spaceCount = 1 ; begin(INDENT) }
                in '0'..'9' -> begin(INTEGER, c)
                else -> {
                    if (isIdentChar(c)) begin(IDENTIFIER, c)
                }
            }
        }
    }

    fun isIdentChar(c: Char): Boolean {
        if (c >= 'a' && c <= 'z') return true
        if (c >= 'A' && c <= 'Z') return true
        if (c >= '0' && c <= '9') return true
        if (c == '_') return true
        return false
    }

    fun begin(type: TokenType, c: Char? = null) {
        c?.also { inBuf += c }
        inType = type
    }

    fun emit(token: Token) {
        inBuf = ""
        inType = null
        if (token.type == COMMENT) return
        token.linePos = linePos
        token.charPos = charPos
        outBuffer.add(token)
    }

    fun finish(token: Token?, nextC: Char) {
        token?.also { emit(it) }
        inType = null
        process(nextC, true)
    }
}

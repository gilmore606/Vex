package compiler

import compiler.TokenType.*

class Lexer(val fVerbose: Boolean) {

    var inType: TokenType? = null
    var inBuf: String = ""
    var linePos: Int = 0
    var charPos: Int = 0
    var spaceCount: Int = 0

    var last: Token? = null
    val outBuffer = ArrayList<Token>()

    fun process(c: Char, isReprocess: Boolean = false) {
        if (!isReprocess) {
            if (c == '\n') { linePos++ ; charPos = 0 }
            else charPos++
        }
        if (inType != null) {
            when (inType) {
                T_ASSIGN ->
                    if (c == '=') emit(Token(T_EQUALS)) else finish(Token(T_ASSIGN), c)
                T_NOTEQUALS ->
                    if (c == '=') emit(Token(T_NOTEQUALS)) else finish(Token(T_BANG), c)
                T_GREATER_THAN ->
                    if (c == '=') emit(Token(T_GREATER_EQUAL)) else finish(Token(T_GREATER_THAN), c)
                T_LESS_THAN ->
                    if (c == '=') emit(Token(T_LESS_EQUAL)) else finish(Token(T_LESS_THAN), c)
                T_ADD -> when (c) {
                    '=' -> emit(Token(T_ADD_ASSIGN))
                    '+' -> emit(Token(T_INCREMENT))
                    else -> finish(Token(T_ADD), c)
                }
                T_SUBTRACT -> when (c) {
                    in '0'..'9' -> { if (negateOK()) {
                            inType = T_INTEGER ; inBuf += c
                        } else {
                            finish(Token(T_SUBTRACT), c)
                        }
                    }
                    '.' -> { if (negateOK()) {
                            inType = T_FLOAT ; inBuf += c
                        } else {
                            finish(Token(T_SUBTRACT), c)
                        }
                    }
                    '=' -> emit(Token(T_SUBTRACT_ASSIGN))
                    '-' -> emit(Token(T_DECREMENT))
                    else -> finish(Token(T_SUBTRACT), c)
                }
                T_DIVIDE ->
                    if (c == '/') inType = T_COMMENT else finish(Token(T_DIVIDE), c)
                T_LOGIC_OR ->
                    if (c == '|') emit(Token(T_LOGIC_OR)) else throw LexException(
                        this,
                        "expected ||"
                    )
                T_LOGIC_AND ->
                    if (c == '&') emit(Token(T_LOGIC_AND)) else throw LexException(
                        this,
                        "expected &&"
                    )
                T_STRING ->
                    if (c == '"') emit(Token(T_STRING, inBuf)) else inBuf += c
                T_COMMENT ->
                    if (c == '\n') emit(Token(T_COMMENT)) else inBuf += c
                T_INTEGER -> when (c) {
                    '.' -> { inType = T_FLOAT; inBuf += c }
                    in '0'..'9' -> { inBuf += c }
                    else -> finish(Token(T_INTEGER, "", inBuf.toInt()), c)
                }
                T_FLOAT -> when (c) {
                    in '0'..'9' -> { inBuf += c }
                    else -> finish(Token(T_FLOAT, "", 0, inBuf.toFloat()), c)
                }
                T_IDENTIFIER -> {
                    if (c == '(')  { emit(Token(T_IDENTIFUNC, inBuf)) ; return }
                    if (isIdentChar(c)) inBuf += c else finish(Token(T_IDENTIFIER, inBuf), c)
                }
                T_INDENT ->
                    if (c == ' ') {
                        if (spaceCount == 3) emit(Token(T_INDENT)) else spaceCount++
                    } else finish(null, c)
                else -> { }
            }
        } else {
            when (c) {
                '(' -> emit(Token(T_PAREN_OPEN))
                ')' -> emit(Token(T_PAREN_CLOSE))
                ':' -> emit(Token(T_COLON))
                ',' -> emit(Token(T_COMMA))
                '=' -> begin(T_ASSIGN)
                '>' -> begin(T_GREATER_THAN)
                '<' -> begin(T_LESS_THAN)
                '+' -> begin(T_ADD)
                '-' -> begin(T_SUBTRACT, c)
                '*' -> emit(Token(T_MULTIPLY))
                '/' -> begin(T_DIVIDE)
                '.' -> emit(Token(T_DOTJOIN))
                '!' -> begin(T_NOTEQUALS)
                '"' -> begin(T_STRING)
                '^' -> emit(Token(T_POWER))
                '%' -> emit(Token(T_MODULUS))
                '\t' -> emit(Token(T_INDENT))
                '|' -> begin(T_LOGIC_OR)
                '&' -> begin(T_LOGIC_AND)
                ' ' -> { spaceCount = 1 ; begin(T_INDENT) }
                in '0'..'9' -> begin(T_INTEGER, c)
                else -> {
                    if (isIdentChar(c)) begin(T_IDENTIFIER, c)
                }
            }
        }
    }

    // Is it safe to assume this is a negated number, or is it probably a minus?
    fun negateOK(): Boolean {
        if (last == null) return true
        return !(last!!.type == T_PAREN_CLOSE || last!!.type == T_INTEGER || last!!.type == T_FLOAT
                || last!!.type == T_IDENTIFIER)
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
        if (token.type == T_COMMENT) return
        token.linePos = linePos
        token.charPos = charPos
        outBuffer.add(token)
        last = token
    }

    fun finish(token: Token?, nextC: Char) {
        token?.also { emit(it) }
        inType = null
        process(nextC, true)
    }
}

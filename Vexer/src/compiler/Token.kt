package compiler

enum class TokenType {
    PAREN_OPEN,
    PAREN_CLOSE,
    COLON,
    ASSIGN,             // =
    EQUALS,             // ==
    NOTEQUALS,          // !=
    GREATER_THAN,       // >
    LESS_THAN,          // <
    GREATER_EQUAL,      // >=
    LESS_EQUAL,         // <=
    ADD,                // +
    SUBTRACT,           // -
    ADD_ASSIGN,         // +=
    SUBTRACT_ASSIGN,    // -=
    INCREMENT,                // ++
    DECREMENT,                // --
    MULTIPLY,               // *
    DIVIDE,                // /
    DOTJOIN,            // .
    BANG,               // !
    POWER,              // ^
    LOGIC_OR,           // ||
    LOGIC_AND,          // &&
    COMMA,              // ,
    IDENTIFIER,
    RESERVED,
    INDENT,
    COMMENT,
    STRING,
    INTEGER,
    FLOAT,
    EOP
}

data class Token(
    val type: TokenType,
    val string: String = "",
    val int: Int = 0,
    val float: Float = 0f,
    var linePos: Int = 0,
    var charPos: Int = 0
) {
    override fun toString(): String {
        var s = this.type.toString()
        if (string.isNotBlank()) s += " (" + string + ")"
        if (int != 0) s += " (" + int.toString() + ")"
        if (float != 0f) s += " (" + float.toString() + ")"
        return s
    }
}

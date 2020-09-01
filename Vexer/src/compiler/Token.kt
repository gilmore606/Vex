package compiler

enum class TokenType {
    T_PAREN_OPEN,
    T_PAREN_CLOSE,
    T_COLON,
    T_ASSIGN,             // =
    T_EQUALS,             // ==
    T_NOTEQUALS,          // !=
    T_GREATER_THAN,       // >
    T_LESS_THAN,          // <
    T_GREATER_EQUAL,      // >=
    T_LESS_EQUAL,         // <=
    T_ADD,                // +
    T_SUBTRACT,           // -
    T_ADD_ASSIGN,         // +=
    T_SUBTRACT_ASSIGN,    // -=
    T_MULT_ASSIGN,        // *=
    T_DIV_ASSIGN,         // /=
    T_INCREMENT,                // ++
    T_DECREMENT,                // --
    T_MULTIPLY,               // *
    T_DIVIDE,                // /
    T_DOTJOIN,            // .
    T_BANG,               // !
    T_POWER,              // ^
    T_LOGIC_OR,           // ||
    T_LOGIC_AND,          // &&
    T_COMMA,              // ,
    T_MODULUS,            // %
    T_IDENTIFIER,
    T_IDENTIFUNC,         // ident(
    T_INDENT,
    T_COMMENT,
    T_STRING,
    T_INTEGER,
    T_FLOAT,
    T_EOP
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

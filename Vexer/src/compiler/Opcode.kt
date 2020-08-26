package compiler

enum class Opcode {
    OP_UNDEF,    // Error: VM halt
    OP_NOP,      // No op
    OP_EXIT,     // Exit program
    OP_DEBUG,    // Dump stack to console

    OP_WAIT,     // Suspend execution for stacktop ms

    OP_JUMP,     // Unconditional jump
    OP_IF,       // Jump if stack bool = false

    OP_VAR,      // Get var
    OP_CONST,    // Get const
    OP_RANDF,    // Get random float 0-1
    OP_RANDI,    // Get random int 0-(stacktop-1)
    OP_SETVAR,   // Store stacktop in var
    OP_SETSYS,   // Store stacktop in system setting

    OP_ADDI,     // i + i
    OP_SUBI,     // i - i
    OP_MULTI,	 // i * i
    OP_DIVI,     // i / i
    OP_NEGI,     // -i
    OP_ADDF,     // f + f
    OP_SUBF,     // f - f
    OP_MULTF,    // f * f
    OP_DIVF,     // f / f
    OP_NEGF,     // -f

    OP_I2F,      // convert int to float
    OP_F2I,      // convert float to int
    OP_V2F,      // convert vector to float
    OP_B2I,      // convert boolean to int
    OP_N2I,      // convert nil to int (0)

    OP_NEGV,     // -v
    OP_ADDV,     // v + v
    OP_MULTV,	 // v * v
    OP_DIVV,	 // v / v

    OP_ADDVF,    // v + f
    OP_MULTVF,   // v * f
    OP_DIVVF,    // v / f

    OP_NOT,      // !bool
    OP_OR,       // bool || bool
    OP_AND,      // bool && bool

    OP_GTI,		 // i > i
    OP_GEI,		 // i >= i
    OP_LTI,		 // i < i
    OP_LEI,		 // i <= i
    OP_EQI,		 // i == i
    OP_GTF,		 // f > f
    OP_GEF,		 // f >= f
    OP_LTF,      // f < f
    OP_LEF,      // f <= f
    OP_EQF,      // f == f

    OP_SONG,     // Play song (id=stacktop)

    OP_INPUT,    // Get value of input (id=stacktop)
    OP_BUTTON,   // Get pressed-this-frame status of button (id=stacktop)

    OP_MAX       // for convenience
}

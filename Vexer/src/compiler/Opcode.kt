package compiler

enum class Opcode {
    OP_UNDEF,  // 0: Error: VM halt
    OP_NOP,    // 1: No op
    OP_EXIT,   // Exit program
    OP_DEBUG,  // Dump the stack to the console

    OP_WAIT,   // Suspend execution for stacktop ms

    OP_JUMP,   // Unconditional jump
    OP_IF,     // Jump if stack bool = false

    OP_VAR,     // Get var
    OP_CONST,   // Get const
    OP_RANDF,   // Get random float 0-1
    OP_RANDI,   // Get random int 0-(stacktop-1)
    OP_SETVAR,  // Store stacktop in var

    OP_ADDI,    // int . int
    OP_SUBI,    //
    OP_MULTI,	//
    OP_DIVI,    //
    OP_NEGI,    //
    OP_ADDF,    // float . float
    OP_SUBF,
    OP_MULTF,
    OP_DIVF,
    OP_NEGF,

    OP_I2F,    // convert int to float
    OP_F2I,    // convert float to int
    OP_V2F,    // convert vector to float
    OP_B2I,    // convert bool to int
    OP_N2I,    // convert nil to int (0)

    OP_NEGV,    // Negate vector
    OP_ADDV,   // Add two vectors
    OP_MULTV,
    OP_DIVV,

    OP_ADDVF,   // Add float to vector
    OP_MULTVF,   // Multiply vector by float, return vec
    OP_DIVVF,   // Divide vector by float, return float

    OP_NOT,    // Negate boolean
    OP_OR,      // boolean or
    OP_AND,     // boolean and

    OP_GTI,		// >
    OP_GEI,		// >=
    OP_LTI,		// <
    OP_LEI,		// <=
    OP_EQI,		// ==
    OP_GTF,
    OP_GEF,
    OP_LTF,
    OP_LEF,
    OP_EQF,

    OP_SONG,   // Play song

    OP_INPUT,  // Get value of input
    OP_BUTTON, // Get pressed-this-frame status of button

    OP_MAX
}

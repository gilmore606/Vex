#pragma once

typedef enum {

    OP_UNDEF,    // Error: VM halt
    OP_NOP,      // No op
    OP_EXIT,     // Exit program
    OP_DEBUG,    // Dump stack to console

    OP_WAIT,     // Suspend execution for stacktop ms

    OP_JUMP,     // [2:jump] Unconditional jump
    OP_JUMPZ,    // [2:var,2:jump] Jump if int var = 0
    OP_JUMPNZ,   // [2:var,2:jump] Jump if int var <> 0
    OP_IF,       // [2:jump] Jump if stack bool = false

    OP_VAR,      // [2:var] Get var
    OP_CONST,    // [2:const] Get const
    OP_RANDF,    // Get random float 0-1
    OP_RANDI,    // Get random int 0-(stacktop-1)
    OP_INCVAR,   // [2:var] Increment int var
    OP_DECVAR,   // [2:var] Decrement int var
    OP_SETVAR,   // [2:var] Store stacktop in var
    OP_SETSYS,   // [2:sys] Store stacktop in system setting

    OP_INC,      // i++
    OP_DEC,      // i--
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
    OP_NEGV,     // -v
    OP_ADDV,     // v + v
    OP_MULTV,	 // v * v
    OP_DIVV,	 // v / v

    OP_ADDVF,    // v + f
    OP_MULTVF,   // v * f (dot product, produce float)
    OP_DIVVF,    // v / f (????)

    OP_I2F,      // int to float
    OP_F2I,      // float to int (trunc)
    OP_V2F,      // vector to float (length)
    OP_B2I,      // boolean to int (0, 1)
    OP_N2I,      // nil to int (0)

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
    OP_SPRITE,   // Create sprite
    OP_PARTI,    // Create particle

    OP_INPUT,    // Get value of input (id=stacktop)
    OP_BUTTON,   // Get pressed-this-frame status of button (id=stacktop)

    OP_MAX       // for convenience

} Opcode;

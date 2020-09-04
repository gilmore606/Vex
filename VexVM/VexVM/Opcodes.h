#pragma once

typedef enum {

    OP_UNDEF,    // Error: VM halt
    OP_NOP,      // No op
    OP_EXIT,     // Exit program
    OP_DEBUG,    // Pop stacktop to console
    OP_WAIT,     // Suspend execution for stacktop ms

    OP_JUMP,     // [2:jump] Unconditional jump
    OP_JUMPZ,    // [2:var] [2:jump] Jump if int var = 0
    OP_JUMPNZ,   // [2:var] [2:jump] Jump if int var <> 0
    OP_IF,       // [2:jump] Jump if stack bool = false

    OP_VAR,      // [2:var] Get var
    OP_CONST,    // [2:const] Get const
    OP_INCVAR,   // [2:var] Increment int var
    OP_DECVAR,   // [2:var] Decrement int var
    OP_SETVAR,   // [2:var] Store stacktop in var
    OP_SETSYS,   // [2:sys] Store stacktop in system setting
    OP_LDI,      // [2:int] Push int to the stack as value
    OP_FUN,      // [2:fun] Call function with stack args, push return on stack
    OP_SFUN,     // [2:sysfun] Call system function with stack args, push return on stack
    OP_SMETH,    // [2:sysclassmeth] Call system class method with stack args (this on top), push return on stack
    OP_STAT,     // [2:sid] [2:paramcount] Call system statement <sid> with <paramcount> stack args

    // for compiler optimizations:

    //OP_C2VAR,    // [2:const] [2:var] Copy const to var
    //OP_V2VAR,    // [2:var] [2:var] Copy var to var
    //OP_IFVAR,    // [2:var] Jump if var = false
    //OP_ACCVAR,  // [2:var] i + var -> var

    OP_INPUT,    // Get value of input (id=stacktop)
    OP_BUTTON,   // Get pressed-this-frame status of button (id=stacktop)

    OP_INC,      // i++
    OP_DEC,      // i--
    OP_ADDI,     // i + i
    OP_SUBI,     // i - i
    OP_MULI,	 // i * i
    OP_DIVI,     // i / i
    OP_MODI,     // i % i
    OP_POWI,     // i ^ i
    OP_NEGI,     // -i

    OP_ADDF,     // f + f
    OP_SUBF,     // f - f
    OP_MULF,     // f * f
    OP_DIVF,     // f / f
    OP_MODF,     // f % f
    OP_POWF,     // f ^ f
    OP_NEGF,     // -f

    OP_NEGV,     // -v
    OP_ADDV,     // v + v
    OP_MULV,	 // v * v
    OP_DIVV,	 // v / v
    OP_ADDVF,    // v + f
    OP_MULVF,    // v * f (dot product, produce float)
    OP_DIVVF,    // v / f (????)

    OP_ADDC,     // c + c
    OP_ADDCF,    // c + f
    OP_MULC,     // c * c (average)
    OP_MULCF,    // c * f

    OP_I2F,      // i -> f
    OP_F2I,      // f -> i (trunc)
    OP_V2F,      // v -> f (length)
    OP_C2F,      // c -> f (brightness)
    OP_B2I,      // b -> i (0, 1)
    OP_N2I,      // nil -> i (0)

    OP_I2S,      // i -> str
    OP_F2S,      // f -> str
    OP_CAT,      // str + str
    OP_EQS,      // str == str

    OP_NOT,      // !b
    OP_OR,       // b || b
    OP_AND,      // b && b

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
    OP_EQV,      // v == v
    OP_EQC,      // c == c

    OP_MAX       // for convenience

} Opcode;

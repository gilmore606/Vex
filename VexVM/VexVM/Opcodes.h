#pragma once

typedef enum {

	OP_UNDEF,  // 0: Error: VM halt
	OP_NOP,    // 1: No op
	OP_EXIT,   // Exit program
	OP_DEBUG,  // Dump the stack to the console

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

	OP_VADDV,   // Add two vectors
	OP_VMULTV,
	OP_VADDN,
	OP_VMULTN,
	OP_VROT,   // Rotate vector by float rads

	OP_NOT,    // Negate boolean
	OP_GT,		// >
	OP_GE,		// >=
	OP_LT,		// <
	OP_LE,		// <=
	OP_EQ,		// ==

	OP_SONG,   // Play song

	OP_INPUT,  // Get value of input
	OP_BUTTON, // Get pressed-this-frame status of button

	OP_MAX

} Opcode;

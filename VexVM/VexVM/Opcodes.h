#pragma once

typedef enum {

	OP_UNDEF,  // 0: Error: VM halt
	OP_NOP,    // 1: No op
	OP_EXIT,   // Exit program
	OP_DEBUG,  // Dump the stack to the console

	OP_JUMP,   // Unconditional jump
	OP_IF,     // Jump if stack bool = false
		// For optimizing, we could also have OP_IFGT, IFLT, IFEQ, etc. to skip pushing the bool on
		// the stack in common cases.

	OP_VAR,   // Get var
	OP_CONST,  // Get const
	OP_RANDOM,  // Get random float 0-1
	OP_SETVAR,  // Store stacktop in var

	OP_ADD,    // Add two numbers
	OP_SUB,    // Subtract two numbers
	OP_MULT,	// Multiply two numbers
	OP_DIV,    // Divide two numbers
	OP_NEG,    // Negate number

	OP_NOT,    // Negate boolean
	OP_GT,		// >
	OP_GE,		// >=
	OP_LT,		// <
	OP_LE,		// <=
	OP_EQ,		// ==

	OP_SONG,   // Play song
	OP_INPUT,  // Get value of input

	OP_MAX

} Opcode;

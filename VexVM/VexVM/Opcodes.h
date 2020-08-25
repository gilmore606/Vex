#pragma once

typedef enum {

	OP_UNDEF,  // 0: Error: VM halt
	OP_NOP,    // 1: No op
	OP_EXIT,   // Exit program

	OP_JUMP,   // Unconditional jump
	OP_IF,     // Jump if stack bool = false
		// For optimizing, we could also have OP_IFGT, IFLT, IFEQ, etc. to skip pushing the bool on
		// the stack in common cases.
	OP_ENTER,  // Begin subroutine
	OP_LEAVE,  // End subroutine
	OP_CALL,   // Call subroutine for value

	OP_VAR,   // Push var to stack
	OP_CONST,  // Push const to stack
	OP_SETVAR,  // Pop stack, store in var

	OP_ADD,    // Add two numbers
	OP_SUB,    // Subtract two numbers
	OP_MULT,	// Multiply two numbers
	OP_DIV,    // Divide two numbers
	OP_NEG,    // Negate number

	OP_NOT,    // Negate boolean

	OP_MAX

} Opcode;

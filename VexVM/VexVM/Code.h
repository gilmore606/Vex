#pragma once

#include <vector>
#include "Values.h"
#include "string"

#define SYSVAR_DELTA 0

class ROMReader;

class Code {
public:

	uint8_t* code;
	int codec;

	std::vector<Value> constants;
	std::vector<Value> variables;
	std::vector<uint8_t*> entries;  // pointers into code
	std::vector<std::string> entryLabels;
	std::vector<uint8_t*> functions; // pointers into code
	std::vector<uint8_t*> jumps;  // pointers into code

	uint8_t* entryState;
	uint8_t* entryStart;
	uint8_t* entryUpdate;
	uint8_t* entryButton;

	Code();
	void Read(ROMReader* rom);

private:
	std::vector<int> entriesRel;
	std::vector<int> jumpsRel;

	Value readVal(ROMReader* rom);
};

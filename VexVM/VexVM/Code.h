#pragma once

#include <vector>
#include "Values.h"
class ROMReader;

class Code {
public:

	uint8_t* code;
	int codec;

	std::vector<Value> constants;
	std::vector<Value> variables;
	std::vector<uint8_t*> entries;  // pointers into code
	std::vector<uint8_t*> jumps;  // pointers into code

	void Read(ROMReader* rom);

private:
	std::vector<int> entriesRel;
	std::vector<int> jumpsRel;

	Value readVal(ROMReader* rom);
};

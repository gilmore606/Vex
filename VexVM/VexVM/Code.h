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
	std::vector<uint8_t*> jumps;  // pointers into code

	void Read(ROMReader* rom);

private:
	Value readVal(ROMReader* rom);
};

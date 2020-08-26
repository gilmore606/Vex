#include "Code.h"
#include "ROMReader.h"

Value Code::readVal(ROMReader* rom) {
	Value c;
	switch (rom->next()) {
	case 0:
		c.type = VAL_NIL;
		break;
	case 1:
		c.type = VAL_BOOL;
		c.as.boolean = false;
		break;
	case 2:
		c.type = VAL_BOOL;
		c.as.boolean = true;
		break;
	case 3:
		c.type = VAL_INT;
		c.as.integer = rom->next3Int();
		break;
	}
	return c;
}

void Code::Read(ROMReader* rom) {
	
	// read constants

	if (!rom->expectMarker("CONST")) throw "expected const block";
	int constc = rom->next2Int();
	for (int i = 0; i < constc; i++) {
		constants.push_back(readVal(rom));
	}
	std::cout << "  read " << constc << " constants" << std::endl;

	// read bytecode

	if (!rom->expectMarker("BC")) throw "expected bytecode block";
	codec = rom->next3Int();
	code = new uint8_t[codec];
	for (int i = 0; i < codec; i++) {
		code[i] = rom->next();
	}
	std::cout << "  read " << codec << " bytecode instructions" << std::endl;

	// TODO: build jump table
}

#include "Code.h"
#include "ROMReader.h"

Code::Code() {
	entryStart = nullptr;
	entryUpdate = nullptr;
	entryInput = nullptr;
	codec = 0;
	code = nullptr;
}

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
	case 4:
		c.type = VAL_FLOAT;
		c.as.fp = rom->next4Float();
		break;
	case 5:
		c.type = VAL_VECTOR;
		c.as.vector[0] = rom->next4Float();
		c.as.vector[1] = rom->next4Float();
		break;
	case 6:
		c.type = VAL_COLOR;
		c.as.color[0] = rom->next4Float();
		c.as.color[1] = rom->next4Float();
		c.as.color[2] = rom->next4Float();
		break;
	}
	return c;
}

void Code::Read(ROMReader* rom) {
	
	// read constants

	if (!rom->expectMarker("CONST")) throw "expected const block";
	int constc = rom->next2Int();
	for (int i = 0; i < constc; i++) {
		Value con = readVal(rom);
		constants.push_back(con);
	}
	std::cout << "  " << constc << " constants" << std::endl;

	// read variables

	if (!rom->expectMarker("VARS")) throw "expected vars block";
	int varc = rom->next2Int();
	for (int i = 0; i < varc; i++) {
		variables.push_back(Value{ VAL_NIL, {} });
	}
	std::cout << "  " << varc << " vars" << std::endl;

	// read entry points

	if (!rom->expectMarker("ENTRY")) throw "expected entry point block";
	int entryc = rom->nextInt();
	for (int i = 0; i < entryc; i++) {
		entryLabels.push_back(rom->nextString());
		entriesRel.push_back(rom->next3Int());
	}
	std::cout << "  " << entryc << " entries" << std::endl;

	// read jump points

	if (!rom->expectMarker("JUMP")) throw "expected jump point block";
	int jumpc = rom->next2Int();
	for (int i = 0; i < jumpc; i++) {
		jumpsRel.push_back(rom->next3Int());
	}
	std::cout << "  " << jumpc << " jumps" << std::endl;

	// read bytecode

	if (!rom->expectMarker("BC")) throw "expected bytecode block";
	codec = rom->next3Int();
	code = new uint8_t[codec];
	for (int i = 0; i < codec; i++) {
		code[i] = rom->next();
	}
	std::cout << "  " << codec << " bytecodes" << std::endl;

	// build jump and entry tables

	for (int i = 0; i < entryc; i++) {
		uint8_t* point = code + entriesRel[i];
		entries.push_back(point);
		if (entryLabels[i] == "state") { entryState = point; }
		if (entryLabels[i] == "start") { entryStart = point; }
		if (entryLabels[i] == "update") { entryUpdate = point; }
		if (entryLabels[i] == "input") { entryInput = point; }
	}
	for (int i = 0; i < jumpc; i++) {
		jumps.push_back(code + jumpsRel[i]);
	}

}

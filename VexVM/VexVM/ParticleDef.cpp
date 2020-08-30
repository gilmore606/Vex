#include "ParticleDef.h"
#include "ROMReader.h"
#include <iostream>

ParticleDef::ParticleDef() { }

void ParticleDef::Read(ROMReader* rom) {
	image = rom->next2Int();
	float f1 = rom->next4Float();
	float f2 = rom->next4Float();
	v = VECTOR_VAL(f1, f2);
	std::cout << "v1 was " << v.as.vector[0] << std::endl;
	f1 = rom->next4Float();
	f2 = rom->next4Float();
	vv = VECTOR_VAL(f1, f2);
	f1 = rom->next4Float();
	f2 = rom->next4Float();
	float f3 = rom->next4Float();
	color0 = VECTOR_VAL(f1,f2,f3);
	f1 = rom->next4Float();
	f2 = rom->next4Float();
	f3 = rom->next4Float();
	color1 = VECTOR_VAL(f1,f2,f3);
	f1 = rom->next4Float();
	f2 = rom->next4Float();
	scale0 = VECTOR_VAL(f1,f2);
	f1 = rom->next4Float();
	f2 = rom->next4Float();
	scale1 = VECTOR_VAL(f1,f2);
	rot = rom->next4Float();
	rotv = rom->next4Float();
	lifetime = rom->next4Float();
	std::cout << "rotv was " << rotv << std::endl;
	std::cout << "lifetime was " << lifetime << std::endl;
	std::cout << "cursor at " << rom->cursor << std::endl;
}

#include "ParticleDef.h"
#include "ROMReader.h"
#include <iostream>

ParticleDef::ParticleDef() { }

void ParticleDef::Read(ROMReader* rom) {
	std::cout << "cursor at " << rom->cursor << std::endl;
	image = rom->next2Int();
	std::cout << "image was " << image << std::endl;
	float f1 = rom->next4Float();
	std::cout << "v1 was " << f1 << std::endl;
	float f2 = rom->next4Float();
	v = VECTOR_VAL(f1, f2);
	std::cout << "v1 was " << v.as.vector[0] << std::endl;
	vv = VECTOR_VAL(rom->next4Float(), rom->next4Float());
	color0 = VECTOR_VAL(rom->next4Float(), rom->next4Float(), rom->next4Float());
	color1 = VECTOR_VAL(rom->next4Float(), rom->next4Float(), rom->next4Float());
	scale0 = VECTOR_VAL(rom->next4Float(), rom->next4Float());
	scale1 = VECTOR_VAL(rom->next4Float(), rom->next4Float());
	rot = rom->next4Float();
	rotv = rom->next4Float();
	lifetime = rom->next4Float();
	std::cout << "rotv was " << rotv << std::endl;
	std::cout << "lifetime was " << lifetime << std::endl;
	std::cout << "cursor at " << rom->cursor << std::endl;
	int nextByte = rom->nextInt();
	std::cout << "next byte was " << nextByte << std::endl;
}

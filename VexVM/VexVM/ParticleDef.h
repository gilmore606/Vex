#pragma once
#include "Values.h"
class ROMReader;

class ParticleDef
{
public:
	ParticleDef();

	void Read(ROMReader* rom);

	int id;
	int image;
	Value v;
	Value vv;
	Value color0;
	Value color1;
	Value scale0;
	Value scale1;
	float rot;
	float rotv;
	float lifetime;

private:

};

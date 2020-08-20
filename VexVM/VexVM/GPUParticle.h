#pragma once
#include "GPUSprite.h"

class GPUParticle : public GPUSprite
{
public:
    GPUParticle();

    Vec vv;
    Color color0, color1;
    Vec scale0, scale1;
    float rotv;
    float lifetime;
    float age;
private:
};


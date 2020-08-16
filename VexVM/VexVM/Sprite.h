#pragma once

#include "Line.h"
#include "GPUSprite.h"
#include "GPU.h"
#include <algorithm>
#include <iostream>

class Sprite
{
public:
	Sprite(int image, GPU* gpu);

	void moveTo(float x, float y);
	void rotate(float rads);
	void scale(float xs, float ys);
	void update();

private:
	GPUSprite* gpuSprite;
	int gpuSpriteID;
};

inline void Sprite::moveTo(float newx, float newy) {
	gpuSprite->x = newx;
	gpuSprite->y = newy;
	gpuSprite->dirty = true;
}
inline void Sprite::rotate(float rads) {
	float rot = gpuSprite->rot;
	rot += rads;
	while (rot > 6.283185f) rot -= 6.283185f;
	while (rot < 0.0f) rot += 6.283185f;
	gpuSprite->rot = rot;
	gpuSprite->dirty = true;
}

inline void Sprite::scale(float xs, float ys) {
	gpuSprite->xscale = xs;
	gpuSprite->yscale = ys;
}

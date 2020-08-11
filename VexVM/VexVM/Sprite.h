#pragma once

#include "Line.h"
#include <algorithm>
#include <iostream>

class Sprite
{
public:
	Sprite();

	void add(float x1, float y1, float x2, float y2, float r, float g, float b);
	void moveTo(float x, float y);
	void rotate(float rads);
	void PushData(float dataArray[], int* counter);

	void update();

	Line* data = new Line[64];
	Line* data_out = new Line[64];
	int datac;
	bool dirty = true;
	float x, y;
	float xs = 1.0f;
	float ys = 1.0f;
	float r, g, b;
	float rot;
};

inline void Sprite::moveTo(float newx, float newy) {
	x = newx;
	y = newy;
	dirty = true;
}
inline void Sprite::rotate(float rads) {
	rot += rads;
	while (rot > 6.283185f) rot -= 6.283185f;
	while (rot < 0.0f) rot += 6.283185f;
	dirty = true;
}


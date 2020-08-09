#pragma once

#include "Line.h"

struct Sprite
{
	Sprite();

	void add(float x1, float y1, float x2, float y2, float r, float g, float b);
	void moveTo(float x, float y);
	void pushData(float dataArray[], int* counter);

private:
	void update();

	Line* data = new Line[64];
	Line* data_out = new Line[64];
	int datac;
	bool dirty = true;
	float x, y;
	float xs, ys;
	float r, g, b;
	float rot;
};


#include "Sprite.h"
#include <algorithm>
#include <cmath>
#include <iostream>

Sprite::Sprite() {
	rot = 0.6f;
}

void Sprite::add(float x1, float y1, float x2, float y2, float r, float g, float b) {
	if (datac > 62) throw "Sprite has too many lines!";
	data[datac] = Line(x1, y1, x2, y2, r, g, b);
	datac++;
}

void Sprite::update() {
	float cos = std::cos(rot);
	float sin = std::sin(rot);
	float x1, y1, x2, y2;
	for (int i = 0; i < datac; i++) {
		x1 = xs * data[i].x1;
		y1 = ys * data[i].y1;
		x2 = xs * data[i].x2;
		y2 = ys * data[i].y2;
		data_out[i].x1 = (x1 * cos - y1 * sin) + x;
		data_out[i].y1 = (y1 * cos + x1 * sin) + y;
		data_out[i].x2 = (x2 * cos - y2 * sin) + x;
		data_out[i].y2 = (y2 * cos + x2 * sin) + y;
		data_out[i].r = data[i].r;
		data_out[i].g = data[i].g;
		data_out[i].b = data[i].b;
	}
	dirty = false;
}

void Sprite::PushData(float dataArray[], int* counter) {
	if (dirty) update();
	for (int i = 0; i < datac; i++) {
		data_out[i].PushData(dataArray, counter);
	}
}

#include "GPUSprite.h"
#include <cmath>

GPUSprite::GPUSprite() {
	active = false;
	data = new Line[64];
	data_out = new Line[64];
	reset();
}

void GPUSprite::reset() {
	x = 0.0f;
	y = 0.0f;
	xscale = 1.0f;
	yscale = 1.0f;
	rot = 0.0f;
	dirty = true;
	datac = 0;
}

void GPUSprite::loadImage(Image* image) {
	datac = image->lines.size();
	for (int i = 0; i < datac; i++) {
		data[i].x1 = image->lines[i].x1;
		data[i].y1 = image->lines[i].y1;
		data[i].x2 = image->lines[i].x2;
		data[i].y2 = image->lines[i].y2;
		data[i].r = image->lines[i].r;
		data[i].g = image->lines[i].g;
		data[i].b = image->lines[i].b;
	}
	dirty = true;
}

void GPUSprite::update() {
	float cos = std::cos(rot);
	float sin = std::sin(rot);
	float x1, y1, x2, y2;
	for (int i = 0; i < datac; i++) {
		x1 = xscale * data[i].x1;
		y1 = yscale * data[i].y1;
		x2 = xscale * data[i].x2;
		y2 = yscale * data[i].y2;
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

void GPUSprite::PushData(float dataArray[], int* counter) {
	if (dirty) update();
	for (int i = 0; i < datac; i++) {
		data_out[i].PushData(dataArray, counter);
	}
}

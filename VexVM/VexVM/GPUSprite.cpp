#include "GPUSprite.h"
#include <cmath>
#include <iostream>

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
}

#include "Sprite.h"

Sprite::Sprite() : x(0.0f), y(0.0f), xs(1.0f), ys(1.0f), r(1.0f), g(1.0f), b(1.0f), rot(0.0f) { }

void Sprite::add(float x1, float y1, float x2, float y2, float r, float g, float b) {
	if (datac > 62) return;
	data[datac] = Line(x1, y1, x2, y2, r, g, b);
	datac++;
}

void Sprite::moveTo(float x, float y) {
	this->x = x;
	this->y = y;
	dirty = true;
}

void Sprite::update() {
	for (int i = 0; i < datac; i++) {
		data_out[i].x1 = data[i].x1 + x;
		data_out[i].y1 = data[i].y1 + y;
		data_out[i].x2 = data[i].x2 + x;
		data_out[i].y2 = data[i].y2 + y;
	}
	dirty = false;
}

void Sprite::pushData(float dataArray[], int* counter) {
	if (dirty) update();
	for (int i = 0; i < datac; i++) {
		data_out[i].pushData(dataArray, counter);
	}
}

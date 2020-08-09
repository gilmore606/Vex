#include "Sprite.h"

Sprite::Sprite() : x(0.0f), y(0.0f), xs(1.0f), ys(1.0f), r(1.0f), g(1.0f), b(1.0f), rot(0.0f) { }

void Sprite::add(float x1, float y1, float x2, float y2, float r, float g, float b) {
	if (datac > 62) return;
	data[datac] = Line(x1, y1, x2, y2, r, g, b);
	datac++;
}

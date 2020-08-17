#include "Sprite.h"
#include <algorithm>
#include <cmath>
#include <iostream>

Sprite::Sprite(int image, GPU* gpu) {
	GPUSpriteTicket ticket = gpu->createSprite(image);
	gpuSprite = ticket.gpuSprite;
	gpuSpriteID = ticket.gpuSpriteID;
}

Sprite::Sprite(std::string* text, int font, GPU* gpu, float r, float g, float b) {
	GPUSpriteTicket ticket = gpu->createText(font, text, r, g, b);
	gpuSprite = ticket.gpuSprite;
	gpuSpriteID = ticket.gpuSpriteID;
}

void Sprite::update() {
	gpuSprite->update();
}

float Sprite::x() {
	return gpuSprite->x;
}
float Sprite::y() {
	return gpuSprite->y;
}
float Sprite::rot() {
	return gpuSprite->rot;
}

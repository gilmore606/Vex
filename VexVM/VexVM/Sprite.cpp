#include "Sprite.h"
#include <algorithm>
#include <cmath>
#include <iostream>

Sprite::Sprite(int image, GPU* gpu) {
	GPUSpriteTicket ticket = gpu->createSprite(image);
	gpuSprite = ticket.gpuSprite;
	gpuSpriteID = ticket.gpuSpriteID;
}

Sprite::Sprite(std::string* text, int font, GPU* gpu, float r, float g, float b,
		float charSpacing, float lineSpacing) {

	GPUSpriteTicket ticket = gpu->createText(font, text, r, g, b, charSpacing, lineSpacing);
	gpuSprite = ticket.gpuSprite;
	gpuSpriteID = ticket.gpuSpriteID;
}

void Sprite::setCollision(bool value) {
	gpuSprite->collides = value;
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

short* Sprite::colliders() {
	return gpuSprite->colliders;
}
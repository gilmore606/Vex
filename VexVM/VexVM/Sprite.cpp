#include "Sprite.h"
#include <algorithm>
#include <cmath>
#include <iostream>

Sprite::Sprite() { }

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
void Sprite::setVisible(bool value) {
	gpuSprite->visible = value;
}


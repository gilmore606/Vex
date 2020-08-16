#include "Sprite.h"
#include <algorithm>
#include <cmath>
#include <iostream>

Sprite::Sprite(int image, GPU* gpu) {
	GPUSpriteTicket ticket = gpu->createSprite(image);
	gpuSprite = ticket.gpuSprite;
	gpuSpriteID = ticket.gpuSpriteID;
}

void Sprite::update() {
	gpuSprite->update();
}

#pragma once

#include "Line.h"
#include "Image.h"

// Internal GPU sprite data, for copying into VBO
// Pointed to by Sprite

class GPUSprite {
public:
	bool active;

	GPUSprite();
	void reset();
	void update();
	void loadImage(Image* image);
	void PushData(float dataArray[], int* counter);

	float x, y;
	float xscale, yscale;
	float rot;

	bool dirty;
	Line* data;
	Line* data_out;
	int datac;
};

struct GPUSpriteTicket {
	GPUSprite* gpuSprite;
	int gpuSpriteID;
};

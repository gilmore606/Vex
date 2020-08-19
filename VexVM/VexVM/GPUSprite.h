#pragma once

#include "Line.h"
#include "Image.h"
#include <cmath>
#include <iostream>
#include "Collider.h"
#include "Vector.h"
class GPU;
// Internal GPU sprite data, for copying into VBO
// Pointed to by Sprite

class GPUSprite {
public:
	bool active;
	bool visible;
	
	GPUSprite();
	void reset();
	void allocateLines(int newc);
	void allocatePoints(int newc);
	void writeLine(int linei, float x1, float y1, float x2, float y2, float r, float g, float b);
	void loadImage(Image* image);
	void PushData(float linedata[], int* linec, float pointdata[], int* pointc, float aspect);
	void update();
	void Destroy(int spriteID);

	bool collides;
	Pos p;
	Vec v;
	float xscale, yscale;
	float rot;

	bool dirty;
	Line* data;
	Line* data_out;
	int datac;
	Point* pdata;
	Point* pdata_out;
	int pdatac;

	Collider* colliders;
	int colliderc;
	GPU* gpu;

private:
};

struct GPUSpriteTicket {
	GPUSprite* gpuSprite;
	int gpuSpriteID;
};

inline void GPUSprite::update() {
	float cos = std::cos(rot);
	float sin = std::sin(rot);
	float x1, y1, x2, y2;
	int i = 0;
	while (i < datac) {
		x1 = xscale * data[i].x1;
		y1 = yscale * data[i].y1;
		x2 = xscale * data[i].x2;
		y2 = yscale * data[i].y2;
		data_out[i].x1 = (x1 * cos - y1 * sin) + p.x;
		data_out[i].y1 = (y1 * cos + x1 * sin) + p.y;
		data_out[i].x2 = (x2 * cos - y2 * sin) + p.x;
		data_out[i].y2 = (y2 * cos + x2 * sin) + p.y;
		data_out[i].r = data[i].r;
		data_out[i].g = data[i].g;
		data_out[i].b = data[i].b;
		i++;
	}
	i = 0;
	while (i < pdatac) {
		pdata_out[i].x = xscale * pdata[i].x + p.x;
		pdata_out[i].y = yscale * pdata[i].y + p.y;
		pdata_out[i].r = pdata[i].r;
		pdata_out[i].g = pdata[i].g;
		pdata_out[i].b = pdata[i].b;
		pdata_out[i].size = pdata[i].size;
		i++;
	}
	dirty = false;
}

inline void GPUSprite::PushData(float linedata[], int* linec, float pointdata[], int* pointc, float aspect) {
	if (v.dx != 0.0f || v.dy != 0.0f) update();
	if (dirty) update();
	int i = 0;
	while (i < datac) {
		data_out[i].PushData(linedata, linec, aspect);
		i++;
	}
	i = 0;
	while (i < pdatac) {
		pdata_out[i].PushData(pointdata, pointc, aspect);
		i++;
	}
}

#pragma once

#include "Line.h"
#include "GPUSprite.h"
#include "GPU.h"
#include <algorithm>
#include <iostream>
#include "Vector.h"

class Sprite
{
public:
	Sprite();
	Sprite(int image, GPU* gpu);
	Sprite(std::string* text, int font, GPU* gpu, float r, float g, float b,
		float charSpacing = 0.2f, float lineSpacing = 1.1f);

	float x();
	float y();
	Pos p();
	Vec v();
	float rot();
	Color color();
	float tint();
	bool visible();
	Collider* colliders();
	int id();
	void setCollision(bool value);
	void setVisible(bool value);
	void moveTo(float x, float y);
	void moveTo(Pos p);
	void rotate(float rads);
	void setRotation(float rads);
	void setVector(float dx, float dy);
	void setColor(float r, float g, float b, float tint);
	void setVector(Vec nv);
	void scale(float xs, float ys);
	void Destroy();

private:
	GPUSprite* gpuSprite;
	int gpuSpriteID;
};

inline void Sprite::moveTo(float newx, float newy) {
	gpuSprite->p.x = newx;
	gpuSprite->p.y = newy;
	gpuSprite->dirty = true;
}
inline void Sprite::moveTo(Pos p) {
	gpuSprite->p = p;
	gpuSprite->dirty = true;
}
inline void Sprite::rotate(float rads) {
	float rot = gpuSprite->rot;
	rot += rads;
	while (rot > 6.283185f) rot -= 6.283185f;
	while (rot < 0.0f) rot += 6.283185f;
	gpuSprite->rot = rot;
	gpuSprite->dirty = true;
}
inline void Sprite::setRotation(float rads) {
	gpuSprite->rot = rads;
	gpuSprite->dirty = true;
}
inline void Sprite::setVector(float dx, float dy) {
	gpuSprite->v.dx = dx;
	gpuSprite->v.dy = dy;
}
inline void Sprite::setVector(Vec nv) {
	gpuSprite->v.dx = nv.dx;
	gpuSprite->v.dy = nv.dy;
}
inline void Sprite::scale(float xs, float ys) {
	gpuSprite->scale.dx = xs;
	gpuSprite->scale.dy = ys;
}
inline float Sprite::x() {
	return gpuSprite->p.x;
}
inline float Sprite::y() {
	return gpuSprite->p.y;
}
inline Pos Sprite::p() {
	return gpuSprite->p;
}
inline Vec Sprite::v() {
	return gpuSprite->v;
}
inline float Sprite::rot() {
	return gpuSprite->rot;
}
inline Color Sprite::color() {
	return gpuSprite->color;
}
inline float Sprite::tint() {
	return gpuSprite->tint;
}
inline bool Sprite::visible() {
	return gpuSprite->visible;
}

inline Collider* Sprite::colliders() {
	return gpuSprite->colliders;
}

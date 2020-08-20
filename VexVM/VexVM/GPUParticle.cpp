#include "GPUParticle.h"

GPUParticle::GPUParticle() {
	p.x = 0.0f;
	p.y = 0.0f;
	v.dx = 0.0f;
	v.dy = 0.0f;
	vv.dx = 0.0f;
	vv.dy = 0.0f;
	color0.r = 0.5f;
	color0.g = 0.5f;
	color0.b = 0.5f;
	color1.r = 0.5f;
	color1.g = 0.5f;
	color1.b = 0.5f;
	scale0.dx = 0.1f;
	scale0.dy = 0.1f;
	scale1.dx = 0.1f;
	scale1.dy = 0.1f;
	lifetime = 1.0f;
	age = 0.0f;
	tint = 1.0f;
	rot = 0.0f;
	rotv = 0.0f;
	collides = false;
	active = false;
	visible = true;
	colliderc = 0;
	reset();
}

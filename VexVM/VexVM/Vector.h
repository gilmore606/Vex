#pragma once

#include <cmath>


struct Vec {
	Vec() { }
	Vec(float dx, float dy) { this->dx = dx; this->dy = dy; }
	float dx = 0.0f, dy = 0.0f;
	inline Vec operator + (Vec const& obj) {
		Vec res;
		res.dx = dx + obj.dx;
		res.dy = dy + obj.dy;
		return res;
	}
	inline Vec operator * (float f) {
		Vec res;
		res.dx = dx * f;
		res.dy = dy * f;
		return res;
	}
};

struct Pos {
	Pos() { }
	Pos(float x, float y) { this->x = x; this->y = y; }
	float x = 0.0f, y = 0.0f;
	inline Pos operator + (Vec const& obj) {
		Pos res;
		res.x = x + obj.dx;
		res.y = y + obj.dy;
		return res;
	}
};



inline static Vec rot2vec(float rads) {
	Vec v;
	v.dx = std::cos(rads);
	v.dy = std::sin(rads);
	return v;
}

inline static Pos wrapPos(Pos p) {
	Pos np;
	np.x = p.x > 1.1f ? p.x - 2.2f : (p.x < -1.1f ? p.x + 2.2f : p.x);
	np.y = p.y > 1.1f ? p.y - 2.2f : (p.y < -1.1f ? p.y + 2.2f : p.y);
	return np;
}

inline static bool isOffscreen(Pos p) {
	return (p.x > 1.0f || p.x < -1.0f || p.y > 1.0f || p.x < -1.0f);
}


struct Color {
	Color() { }
	Color(float r, float g, float b) { this->r = r; this->g = g; this->b = b; }
	float r = 1.0f, g = 1.0f, b = 1.0f;
	inline Color operator + (Color const& obj) {
		Color res;
		res.r = r + obj.r;
		res.g = g + obj.g;
		res.b = b + obj.b;
		return res;
	}
	inline Color operator * (float f) {
		Color res;
		res.r = r * f;
		res.g = g * f;
		res.b = b * f;
		return res;
	}
};

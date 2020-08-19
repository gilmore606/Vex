#pragma once

#include <cmath>

struct Pos {
	float x = 0.0f, y = 0.0f;
};

struct Vec {
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



inline static Vec rot2vec(float rads) {
	Vec v;
	v.dx = std::cos(rads);
	v.dy = std::sin(rads);
	return v;
}

inline static Pos wrapPos(Pos p) {
	Pos np;
	np.x = p.x > 1.0f ? p.x - 2.0f : (p.x < -1.0f ? p.x + 2.0f : p.x);
	np.y = p.y > 1.0f ? p.y - 2.0f : (p.y < -1.0f ? p.y + 2.0f : p.y);
	return np;
}

inline static bool isOffscreen(Pos p) {
	return (p.x > 1.0f || p.x < -1.0f || p.y > 1.0f || p.x < -1.0f);
}


struct Color {
	float r = 1.0f, g = 1.0f, b = 1.0f;
};
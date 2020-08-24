#pragma once

#include "CPU.h"
#include "Sprite.h"

struct DemoPoint {
	Point* gpupoint;
	float z;
};
struct DemoRock {
	Sprite* sprite;
	float drot;
};
struct DemoShot {
	Sprite* sprite;
};

class DemoGame : public CPU {
public:

	void Boot();
	void OnUpdate(float deltaTime);
	void OnInput(int input);
	void Stop();

	Voice* engineSound;
	bool thrusting;
	double smokeTime;
	double thrustTime;

	const int DEMO_POINTS = 200;

	DemoPoint* demoPoints = new DemoPoint[DEMO_POINTS];

	std::vector<DemoRock> demoRocks;
	DemoRock demoShip;
	Sprite gridSprite;
	Sprite* flameSprite;

	std::list<DemoShot> demoShots;

private:
	bool spriteIsRock(int id);
	void destroyRockId(int id, Pos pos);
	void fireDemoShot();
	void fireDemoHyperspace();
};
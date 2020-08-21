#include <glad/glad.h>
#include <GLFW\glfw3.h>
#include <iostream>
#include <vector>
#include <cmath>
#include <list>

#include "ROMReader.h"
#include "util.cpp"
#include "Scheduler.h"
#include "CPU.h"
#include "GPU.h"
#include "APU.h"
#include "Input.h"
#include "Sprite.h"
#include "Vector.h"

Scheduler scheduler;
CPU cpu;
GPU gpu;
APU apu;
Input input;



GLFWwindow* window;
int windowWidth = 1300;
int windowHeight = 1300;

constexpr auto DEMO_POINTS = 200;

struct DemoPoint {
	Point* gpupoint;
	float z;
};
DemoPoint* demoPoints = new DemoPoint[DEMO_POINTS];
struct DemoRock {
	Sprite* sprite;
	float drot;
};
std::vector<DemoRock> demoRocks;
DemoRock demoShip;
float thrustTime;
Sprite gridSprite;
Sprite* flameSprite;
struct DemoShot {
	Sprite* sprite;
};
std::list<DemoShot> demoShots;

// Proxy callback for GPU
void onResize(GLFWwindow* window, int w, int h) {
	windowWidth = w;
	windowHeight = h;
	gpu.Resize(w, h);
}

void makeDemoPrims() {
	gridSprite = Sprite(2, &gpu);
	gridSprite.setVisible(false);
	for (int i = 0; i < 24; i++) {
		Sprite* sp = new Sprite(1, &gpu);
		sp->moveTo(randCoord(), randCoord());
		float scale = (randFloat() * 0.05f) + 0.05f;
		sp->scale(scale, scale);
		sp->rotate(randFloat() * 2.7f);
		sp->setColor(randFloat() * 0.5f, randFloat() * 0.5f, randFloat() * 0.5f, 0.3f);
		sp->setCollision(true);
		sp->setVector(randCoord() * 0.2f, randCoord() * 0.2f);
		DemoRock rock;
		rock.sprite = sp;
		rock.drot = randFloat();
		demoRocks.push_back(rock);
	}
	Sprite* shipsp = new Sprite(0, &gpu);
	shipsp->scale(0.4f, 0.4f);
	shipsp->setCollision(true);
	demoShip.sprite = shipsp;
	demoShip.drot = 0.0f;
	flameSprite = new Sprite(3, &gpu);
	flameSprite->scale(0.4f, 0.4f);
	
	apu.Patch(0, Instrument(SINE, SAWTOOTH, 0, 0, 255, 0, 0, 0, 255, 127));
}

void makeDemoStars() {
	for (int i = 0; i < DEMO_POINTS; i++) {
		float x = (randFloat() - 0.5f) * 2.0f;
		float y = (randFloat() - 0.5f) * 2.0f;
		float r = randFloat() * 0.4f;
		float g = randFloat() * 0.7f;
		float b = randFloat() * 1.9f;
		float size = randFloat() * 3.5f + 1.0f;
		demoPoints[i].gpupoint = gpu.addPoint(x, y, r, g, b, size);
		demoPoints[i].z = (randFloat() * 0.4f) + 0.2f;
	}
}

void moveDemoPrims(float delta) {
	std::list<DemoShot>::iterator iter = demoShots.begin();
	std::list<DemoShot>::iterator end = demoShots.end();
	while (iter != end) {
		DemoShot shot = *iter;
		if (!isOffscreen(shot.sprite->p())) {
			++iter;
		} else {
			shot.sprite->Destroy();
			iter = demoShots.erase(iter);
		}
	}
	for (int i = 0; i < demoRocks.size(); i++) {
		Sprite* sp = demoRocks[i].sprite;
		sp->moveTo(wrapPos(sp->p()));
		sp->rotate(demoRocks[i].drot * delta);
	}
	if (input.isPressed(0)) {
		demoShip.sprite->rotate(2.5 * delta);
	} else if (input.isPressed(1)) {
		demoShip.sprite->rotate(-2.5 * delta);
	}
	if (input.isPressed(2)) {
		demoShip.sprite->setVector(demoShip.sprite->v() + rot2vec(demoShip.sprite->rot() + 1.5707) * delta * 0.6f);
		flameSprite->setVisible(true);
		thrustTime += delta;
		if (thrustTime > 0.1f) {
			gpu.spawnParticle(5, demoShip.sprite->p(), rot2vec(demoShip.sprite->rot() - 1.5707) * 0.2f, Vec(0.0f, 0.0f),
				Color(0.1f, 0.1f, 0.1f), Color(0.02f, 0.02f, 0.02f), Vec(0.02f, 0.02f), Vec(0.002f, 0.002f), 
					randFloat() * 2.7f, randFloat() + 3.0f, 1.0f + randFloat());
			thrustTime = 0.0f;
		}
	} else {
		flameSprite->setVisible(false);
		thrustTime = 0.0f;
	}
	Sprite* shipsp = demoShip.sprite;
	shipsp->moveTo(wrapPos(shipsp->p()));
	flameSprite->moveTo(shipsp->x(), shipsp->y());
	flameSprite->setRotation(shipsp->rot());
	if ((shipsp->x() != 0.0f) && (shipsp->y() != 0.0f) && shipsp->colliders()[0].id > 0) {
		for (int i = 0; i < 12; i++) {
			gpu.spawnParticle(6, shipsp->colliders()[0].p, 
				rot2vec(randFloat() * 6.25f) * (randFloat() * 0.4f + 0.2f), 
				Vec(randFloat() - 0.5f, randFloat() - 0.5f),
				Color(1.0f, 1.0f, 1.0f), Color(0.0f, 0.0f, 0.2f), Vec(0.04f, 0.04f), Vec(0.04f, 0.04f),
				randFloat() * 6.25f, 3.5f, 0.5f);
		}
		shipsp->moveTo(0.0f, 0.0f);
		shipsp->setVector(0.0f, 0.0f);
	}
}

void moveDemoStars(float delta) {
	for (int i = 0; i < DEMO_POINTS; i++) {
		demoPoints[i].gpupoint->x = wrapCoord(demoPoints[i].gpupoint->x + demoPoints[i].z * -demoShip.sprite->v().dx * delta * 6.0);
		demoPoints[i].gpupoint->y = wrapCoord(demoPoints[i].gpupoint->y + demoPoints[i].z * -demoShip.sprite->v().dy * delta * 6.0);
	}
}

// Proxy callback for APU
int handleAudio(void* outBuffer, void* inBuffer, unsigned int nFrames,
	double streamTime, RtAudioStreamStatus status, void* userData) {
	return apu.genSamples(outBuffer, inBuffer, nFrames, streamTime, status, userData);
}

// Proxy callback for Input
void handleKey(GLFWwindow* window, int key, int scancode, int action, int mods) {
	input.HandleKey(window, key, scancode, action, mods);
}

void fireDemoShot() {
	if (demoShots.size() > 4) { return; }
	Sprite* sp = new Sprite(4, &gpu);
	sp->moveTo(demoShip.sprite->p());
	sp->setVector(rot2vec((demoShip.sprite->rot() + 1.5707)) * 0.8f);
	DemoShot shot;
	shot.sprite = sp;
	demoShots.push_back(shot);
	apu.voices[0].Trigger(2000.0, 127);
}

void handleButton(int input) {
	if (input == 3) { fireDemoShot(); }

	if (input == 999) { gpu.ToggleFullscreen(); }
	cpu.OnInput(input);

	if (input == 50) { glfwSetWindowShouldClose(window, GLFW_TRUE); }
	if (input == 51) { gpu.toggleLayer(0); }
	if (input == 52) { gpu.toggleLayer(1); }
	if (input == 53) { gpu.toggleLayer(2); }
	if (input == 54) { apu.voices[0].testTone = !apu.voices[0].testTone; }
	if (input == 55) { gridSprite.setVisible(!gridSprite.visible()); }
}
void handleSwitch(int input, bool isDown) { }


int main() {

	// Setup devices
	scheduler = Scheduler();
	cpu = CPU();
	gpu = GPU(windowWidth, windowHeight);
	window = gpu.Setup(onResize);
	apu = APU();
	apu.Setup(handleAudio);
	input = Input();
	input.Setup(window, handleKey, handleButton, handleSwitch);
	input.Add(50, BUTTON, GLFW_KEY_ESCAPE);
	input.Add(51, BUTTON, GLFW_KEY_F1);
	input.Add(52, BUTTON, GLFW_KEY_F2);
	input.Add(53, BUTTON, GLFW_KEY_F3);
	input.Add(54, BUTTON, GLFW_KEY_F4);
	input.Add(55, BUTTON, GLFW_KEY_F5);
	cpu.Connect(&scheduler, &gpu, &apu, &input);

	// Read ROM file
	ROMReader reader = ROMReader("data/demogame.vexo");
	reader.Read(&cpu, &gpu, &apu, &input);

	// Boot
	cpu.Boot();
	scheduler.Start();

	// Make demo shit
	std::cout << "creating demo prims" << std::endl;
	makeDemoPrims();
	makeDemoStars();

	std::string demoText1 = "VEXSYS";
	std::string demoText2 = "v0.1a2   vex-11/780";
	std::string demoText3 = "(c) 1983 Otari Games - All rights reserved\n\nThis is a long piece of text to test the font renderer\nand the sprite/line capacity of the virtual GPU.";
	Sprite textSprite1 = Sprite(&demoText1, 1, &gpu, 1.0f, 0.6f, 0.0f);
	Sprite textSprite2 = Sprite(&demoText2, 1, &gpu, 0.7f, 0.2f, 0.0f);
	Sprite textSprite3 = Sprite(&demoText3, 0, &gpu, 0.3f, 0.02f, 0.0f);
	textSprite1.scale(0.15f, 0.25f);
	textSprite2.scale(0.05f, 0.05f);
	textSprite3.scale(0.025f, 0.035f);
	textSprite1.moveTo(-0.9f, 0.3f);
	textSprite2.moveTo(-0.9f, 0.2f);
	textSprite3.moveTo(-0.9f, 0.1f);

	//apu.PlaySong(0);
	

	// MAIN LOOP

	float currentFrame, lastFrame, deltaTime;
	lastFrame = glfwGetTime();

	while (!glfwWindowShouldClose(window)) {
		currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;

		moveDemoPrims(deltaTime);
		moveDemoStars(deltaTime);
		
		scheduler.OnUpdate(currentFrame);
		cpu.OnUpdate(deltaTime);
		gpu.OnUpdate(deltaTime);
		gpu.Assemble();
		gpu.Render();
		gpu.Collide(deltaTime);
	}

	scheduler.Stop();
	apu.Stop();
	cpu.Stop();
	gpu.Stop();

	return 0;
}

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PWSTR pCmdLine, int nCmdShow) {
	main();
}

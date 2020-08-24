#pragma once
#include "DemoGame.h"
#include "util.cpp"
#include "Vector.h"
#include "Voice.h"
#include "APU.h"


void DemoGame::Boot() {
	std::cout << "DEMO GAME BOOT" << std::endl;
	gridSprite = Sprite(2, gpu);
	gridSprite.setVisible(false);
	for (int i = 0; i < 8; i++) {
		Sprite* sp = new Sprite(1, gpu);
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
	Sprite* shipsp = new Sprite(0, gpu);
	shipsp->scale(0.4f, 0.4f);
	shipsp->setCollision(true);
	demoShip.sprite = shipsp;
	demoShip.drot = 0.0f;
	flameSprite = new Sprite(3, gpu);
	flameSprite->scale(0.4f, 0.4f);
	engineSound = apu->getVoice();
	engineSound->Patch(127, 120, 15, 20, 220, 30, 4, 4, 0, 0);
	engineSound->filter = LOWPASS_12;
	thrusting = false;

	for (int i = 0; i < DEMO_POINTS; i++) {
		float x = (randFloat() - 0.5f) * 2.0f;
		float y = (randFloat() - 0.5f) * 2.0f;
		float r = randFloat() * 0.2f;
		float g = randFloat() * 0.5f;
		float b = randFloat() * 1.3f;
		float size = randFloat() * 3.5f + 1.0f;
		demoPoints[i].gpupoint = gpu->addPoint(x, y, r, g, b, size);
		demoPoints[i].z = (randFloat() * 0.4f) + 0.2f;
	}

	std::string demoText1 = "VEXSYS";
	std::string demoText2 = "v0.1a2   vex-11/780";
	std::string demoText3 = "(c) 1983 Otari Games - All rights reserved\n\nThis is a long piece of text to test the font renderer\nand the sprite/line capacity of the virtual GPU.";
	std::string demoText4 = "score  000040";
	Sprite textSprite1 = Sprite(&demoText1, 1, gpu, 1.0f, 0.6f, 0.0f);
	Sprite textSprite2 = Sprite(&demoText2, 1, gpu, 0.7f, 0.2f, 0.0f);
	Sprite textSprite3 = Sprite(&demoText3, 0, gpu, 0.3f, 0.02f, 0.0f);
	Sprite textSprite4 = Sprite(&demoText4, 0, gpu, 0.7f, 0.7f, 0.7f);
	textSprite1.scale(0.15f, 0.25f);
	textSprite2.scale(0.05f, 0.05f);
	textSprite3.scale(0.025f, 0.035f);
	textSprite1.moveTo(-0.9f, 0.3f);
	textSprite2.moveTo(-0.9f, 0.2f);
	textSprite3.moveTo(-0.9f, 0.1f);
	textSprite4.scale(0.03f, 0.03f);
	textSprite4.moveTo(0.6f, 0.9f);
}


bool DemoGame::spriteIsRock(int id) {
	for (auto const& r : demoRocks) {
		if (r.sprite->id() == id) { return true; }
	}
	return false;
}

void DemoGame::destroyRockId(int id, Pos pos) {
	for (int i = 0; i < 18; i++) {
		gpu->spawnParticle(4, pos,
			rot2vec(randFloat() * 6.25f) * (randFloat() * 0.4f + 0.3f),
			Vec(0.0f, -0.7f),
			Color(0.3f, 0.7f, 1.0f), Color(0.0f, 0.0f, 0.0f), Vec(1.0f, 1.0f), Vec(1.0f, 1.0f),
			0.0f, 0.0f, 0.6f);
	}
}


void DemoGame::fireDemoShot() {
	if (demoShots.size() > 4) { return; }
	apu->PlaySong(3, 0.5f, 0.5f, false);
	Sprite* sp = new Sprite(4, gpu);
	sp->moveTo(demoShip.sprite->p() + rot2vec((demoShip.sprite->rot() + 1.5707)) * 0.025f);
	sp->setVector(rot2vec((demoShip.sprite->rot() + 1.5707)) * 0.8f);
	sp->setCollision(true);
	DemoShot shot;
	shot.sprite = sp;
	demoShots.push_back(shot);
}

void DemoGame::fireDemoHyperspace() {
	apu->PlaySong(5, 0.5f, 0.5f, false);
	Sprite* sp = demoShip.sprite;
	for (int i = 0; i < 6; i++) {
		gpu->spawnParticle(0, sp->p() + sp->v() * 0.15f * i, sp->v() * (0.7f + 0.1f * i), sp->v() * -0.4f,
			Color(0.3f, 1.3f - i * 0.2f, 0.4f), Color(0.0f, 0.0f, 0.0f),
			Vec(0.1f, 0.1f), Vec(2.0f + i * 2.6f, 1.5f + i * 1.8f), sp->rot(), 0.0f, 0.3f + i * 0.1f);
	}
	for (int i = 0; i < 8; i++) {
		gpu->spawnParticle(5, sp->p() + Pos((randFloat() - 0.5f) * 0.1f, (randFloat() - 0.5f) * 0.1f),
			sp->v() * 0.4f, Vec((randFloat() - 0.5f) * 0.3f, (randFloat() - 0.5f) * 0.3f),
			Color(0.1f, 0.3f, 0.1f), Color(0.00f, 0.01f, 0.00f), Vec(0.02f, 0.02f), Vec(0.002f, 0.002f),
			randFloat() * 2.7f, randFloat() + 3.0f, 1.0f + randFloat());
	}
	demoShip.sprite->moveTo(Pos(randFloat() - 0.5f, randFloat() - 0.5f));
	demoShip.sprite->setVector(Vec(0.0f, 0.0f));
	for (int i = 0; i < 8; i++) {
		gpu->spawnParticle(5, sp->p() + Pos((randFloat() - 0.5f) * 0.1f, (randFloat() - 0.5f) * 0.1f),
			sp->v() * 0.4f, Vec((randFloat() - 0.5f) * 0.3f, (randFloat() - 0.5f) * 0.3f),
			Color(0.1f, 0.3f, 0.1f), Color(0.00f, 0.01f, 0.00f), Vec(0.02f, 0.02f), Vec(0.002f, 0.002f),
			randFloat() * 2.7f, randFloat() + 3.0f, 1.0f + randFloat());
	}
}

void DemoGame::OnInput(int input) {
	if (input == 3) { fireDemoShot(); }
	if (input == 5) { fireDemoHyperspace(); }
	if (input == 55) { apu->PlaySong(1, 0.5f, 0.5f, false); }
	if (input == 54) { gridSprite.setVisible(!gridSprite.visible()); }
}

void DemoGame::OnUpdate(float delta) {
	std::list<DemoShot>::iterator iter = demoShots.begin();
	std::list<DemoShot>::iterator end = demoShots.end();
	while (iter != end) {
		DemoShot shot = *iter;
		if (isOffscreen(shot.sprite->p())) {
			shot.sprite->Destroy();
			iter = demoShots.erase(iter);
		}
		else if (spriteIsRock(shot.sprite->colliders()[0].id)) {
			shot.sprite->Destroy();
			iter = demoShots.erase(iter);
			destroyRockId(shot.sprite->colliders()[0].id, shot.sprite->colliders()[0].p);
		}
		else {
			++iter;
		}
	}

	for (int i = 0; i < demoRocks.size(); i++) {
		Sprite* sp = demoRocks[i].sprite;
		sp->moveTo(wrapPos(sp->p()));
		sp->rotate(demoRocks[i].drot * delta);
	}
	if (input->isPressed(0)) {
		demoShip.sprite->rotate(2.5 * delta);
	}
	else if (input->isPressed(1)) {
		demoShip.sprite->rotate(-2.5 * delta);
	}
	if (input->isPressed(2)) {
		demoShip.sprite->setVector(demoShip.sprite->v() + rot2vec(demoShip.sprite->rot() + 1.5707) * delta * 0.6f);
		flameSprite->setVisible(true);
		smokeTime += delta;
		if (smokeTime > 0.1) {
			gpu->spawnParticle(5, demoShip.sprite->p(), rot2vec(demoShip.sprite->rot() - 1.5707) * 0.2f, Vec(0.0f, 0.0f),
				Color(0.1f, 0.1f, 0.1f), Color(0.02f, 0.02f, 0.02f), Vec(0.02f, 0.02f), Vec(0.002f, 0.002f),
				randFloat() * 2.7f, randFloat() + 3.0f, 1.0f + randFloat());
			smokeTime = 0.0;
		}
		if (!thrusting) {
			engineSound->Trigger(200.0, 127);
			thrusting = true;
			thrustTime = 0.0;
		}
		else {
			thrustTime += delta;
			engineSound->setFreq(200.0 + thrustTime * 100.0);
		}
	}
	else {
		flameSprite->setVisible(false);
		smokeTime = 0.0;
		if (thrusting) {
			engineSound->Release();
			thrusting = false;
		}
	}
	Sprite* shipsp = demoShip.sprite;
	shipsp->moveTo(wrapPos(shipsp->p()));
	flameSprite->moveTo(shipsp->x(), shipsp->y());
	flameSprite->setRotation(shipsp->rot());

	// Did we crash?
	if ((shipsp->x() != 0.0f) && (shipsp->y() != 0.0f) && spriteIsRock(shipsp->colliders()[0].id)) {
		apu->PlaySong(2, 1.0f, 0.5f, false);
		for (int i = 0; i < 8; i++) {
			gpu->spawnParticle(6, shipsp->colliders()[0].p,
				rot2vec(randFloat() * 6.25f) * (randFloat() * 0.1f + 0.1f) + shipsp->v() * 0.5f,
				Vec(randFloat() - 0.5f, randFloat() - 0.5f),
				Color(1.0f, 1.0f, 1.0f), Color(0.0f, 0.0f, 0.2f), Vec(0.04f, 0.04f), Vec(0.04f, 0.04f),
				randFloat() * 6.25f, 3.5f, 1.2f);
		}
		for (int i = 0; i < 18; i++) {
			gpu->spawnParticle(4, shipsp->colliders()[0].p,
				rot2vec(randFloat() * 6.25f) * (randFloat() * 0.4f + 0.3f) + shipsp->v() * 0.2f,
				Vec(0.0f, -0.7f),
				Color(0.3f, 0.7f, 1.0f), Color(0.0f, 0.0f, 0.0f), Vec(1.0f, 1.0f), Vec(1.0f, 1.0f),
				0.0f, 0.0f, 0.6f);
		}
		shipsp->moveTo(0.0f, 0.0f);
		shipsp->setVector(0.0f, 0.0f);
		thrusting = false;
	}

	for (int i = 0; i < DEMO_POINTS; i++) {
		demoPoints[i].gpupoint->x = wrapCoord(demoPoints[i].gpupoint->x + demoPoints[i].z * -demoShip.sprite->v().dx * delta * 3.0);
		demoPoints[i].gpupoint->y = wrapCoord(demoPoints[i].gpupoint->y + demoPoints[i].z * -demoShip.sprite->v().dy * delta * 3.0);
	}
}

void DemoGame::Stop() {

}

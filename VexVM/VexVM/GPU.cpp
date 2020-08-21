#include "GPU.h"
#include <glad/glad.h>
#include <GLFW\glfw3.h>
#include <iostream>
#include "Framebuffer.h"
#include "util.cpp"
#include <algorithm>

GPU::GPU() { }

GPU::GPU(int w, int h) {
	this->w = w;
	this->h = h;
	isFullscreen = false;
	points = new Point[settings.MAX_POINTS];
	lines = new Line[settings.MAX_LINES];
	sprites = new GPUSprite[settings.MAX_SPRITES];
	particles = new GPUParticle[settings.MAX_PARTICLES];
	scaledscreen = new float[24];
	colW = w / settings.COLLIDEMAP_SCALE;
	colH = h / settings.COLLIDEMAP_SCALE;
	collideMap = new short*[colH];
	for (int i = 0; i < colH; ++i) {
		collideMap[i] = new short[colW];
	}
	std::cout << "GPU created" << std::endl;
}

void GPU::Resize(int w, int h) {
	std::cout << "window scaled to " << w << "," << h << std::endl;
	this->w = w;
	this->h = h;
	// Calculate the new screen w/h based on aspect ratio
	screenw = h * settings.ASPECT_RATIO;
	screenh = h;
	float scale = std::min((float)w / (float)screenw, (float)h / (float)screenh);
	screenw = scale * screenw;
	screenh = scale * screenh;
	// Resize the screen vertices to match the ratio on the new window size
	xscale = (float)screenw / (float)w;
	yscale = (float)screenh / (float)h;
	for (int i = 0; i < 12; i++) {
		float x = screendata[i];
		float y = screendata[i + 1];
		scaledscreen[i * 2] = screendata[i * 2] * xscale;
		scaledscreen[i * 2 + 1] = screendata[i * 2 + 1] * yscale;
	}
	std::cout << "screen scaled to " << screenw << "," << screenh << std::endl;
	makeBuffers();
}

void GPU::ToggleFullscreen() {
	GLFWmonitor* monitor = glfwGetPrimaryMonitor();
	const GLFWvidmode* mode = glfwGetVideoMode(monitor);
	if (!isFullscreen) {
		savedWidth = w;
		savedHeight = h;
		glfwGetWindowPos(window, &savedX, &savedY);
		glfwSetWindowMonitor(window, monitor, 0, 0, mode->width, mode->height, mode->refreshRate);
		isFullscreen = true;
	} else {
		glfwSetWindowMonitor(window, nullptr, savedX, savedY, savedWidth, savedHeight, 0);
		isFullscreen = false;
	}
	std::cout << "GPU toggle fullscreen" << std::endl;
}

void GPU::loadImage(Image* image) {
	images.push_back(image);
}
void GPU::loadFont(Font* font) {
	fonts.push_back(font);
}

void GPU::spawnParticle(int image, Pos p, Vec v, Vec vv, Color color0, Color color1, 
			Vec scale0, Vec scale1, float rot, float rotv, float lifetime) {
	int id = 0;
	while ((id < settings.MAX_PARTICLES) && particles[id].active) { id++; }
	if (id == settings.MAX_PARTICLES) return;
	if (id >= particlec) particlec = id + 1;
	GPUParticle* particle = &particles[id];
	particle->active = true;
	particle->p = p;
	particle->v = v;
	particle->vv = vv;
	particle->color0 = color0;
	particle->color1 = color1;
	particle->scale0 = scale0;
	particle->scale1 = scale1;
	particle->rot = rot;
	particle->rotv = rotv;
	particle->lifetime = lifetime;
	particle->age = 0.0f;
	particle->color = color0;
	particle->tint = 1.0f;
	particle->scale = scale0;
	particle->loadImage(images.at(image));
}

int GPU::reserveSprite() {
	int id = 0;
	while ((id < settings.MAX_SPRITES) && sprites[id].active) { id++; }
	if (id == settings.MAX_SPRITES) throw "too many sprites";
	if (id >= spritec) spritec = id + 1;
	sprites[id].active = true;
	return id;
}

GPUSpriteTicket GPU::createSprite(int image) {
	int id = reserveSprite();
	GPUSprite* sprite = &sprites[id];
	sprite->gpu = this;
	if (image >= 0) {
		sprite->loadImage(images.at(image));
	}

	GPUSpriteTicket ticket;
	ticket.gpuSprite = sprite;
	ticket.gpuSpriteID = id;
	return ticket;
}

GPUSpriteTicket GPU::createText(int font, std::string* text, Color color, float charSpacing, float lineSpacing) {
	GPUSpriteTicket ticket = createSprite(-1);
	drawText(ticket.gpuSprite, font, text, color, charSpacing, lineSpacing);
	return ticket;
}


void GPU::drawText(GPUSprite* sprite, int font, std::string* text, Color color, float charSpacing, float lineSpacing) {
	sprite->allocateLines(fonts[font]->countLinesInText(text));
	int linec = 0;
	float hoff = 0.0f;
	float voff = 0.0f;
	for (char& c : *text) {
		if (c == ' ') {
			hoff += 0.7f;
		} else if (c == '\n') {
			voff -= lineSpacing;
			hoff = 0.0f;
		} else if (fonts.at(font)->glyphs.count(c) > 0) {
			std::vector<Line>* glyph = fonts.at(font)->glyphs[c];
			float minx = 0.0f;
			float maxx = 0.0f;
			for (int j = 0; j < glyph->size(); j++) {
				Line* l = &glyph->at(j);
				sprite->writeLine(linec, l->x1 + hoff, l->y1 + voff, l->x2 + hoff, l->y2 + voff, color.r, color.g, color.b);
				linec++;
				if (l->x1 < minx) { minx = l->x1; }
				if (l->x1 > maxx) { maxx = l->x1; }
				if (l->x2 < minx) { minx = l->x2; }
				if (l->x2 > maxx) { maxx = l->x2; }
			}
			hoff += (maxx - minx) + charSpacing;
		}
	}
}

void GPU::destroySprite(int id) {
	sprites[id].active = false;
}

Point* GPU::addPoint(float x, float y, float r, float g, float b, float size) {
	if (pointc >= settings.MAX_POINTS) throw "GPU point memory full!";
	points[pointc] = Point(x, y, r, g, b, size);
	pointc++;
	return &points[pointc - 1];
}

Line* GPU::addLine(float x1, float y1, float x2, float y2, float r, float g, float b) {
	if (linec >= settings.MAX_LINES) throw "GPU line memory full!";
	lines[linec] = Line(x1, y1, x2, y2, r, g, b);
	linec++;
	return &lines[linec - 1];
}


// Remove all prims from draw lists
void GPU::Reset() {
	pointc = 0;
	linec = 0;
	spritec = 0;
	particlec = 0;
}

void GPU::makeShaders() {
	std::cout << "Compiling shaders..." << std::endl;
	pointShader = Shader("./data/shaders/drawpoint.vert", "./data/shaders/drawpoint.frag");
	pointShader.Load();
	lineShader = Shader("./data/shaders/drawline.vert", "./data/shaders/drawline.frag");
	lineShader.Load();
	blitShader = Shader("./data/shaders/blit.vert", "./data/shaders/blit.frag");
	blitShader.Load();
	blitShader.SetUniform("screenTexture", 0);
	fadeShader = Shader("./data/shaders/fade.vert", "./data/shaders/fade.frag");
	fadeShader.Load();
	fadeShader.SetUniform("inTexture", 0);
	composeShader = Shader("./data/shaders/compose.vert", "./data/shaders/compose.frag");
	composeShader.Load();
	composeShader.SetUniform("screenTex", 0);
	composeShader.SetUniform("glowTex", 1);
	composeShader.SetUniform("trailTex", 2);
	blurShader = Shader("./data/shaders/blur.vert", "./data/shaders/blur.frag");
	blurShader.Load();
	blurShader.SetUniform("texture", 0);
}

void GPU::makeBuffers() {
	std::cout << "Allocating framebuffers..." << std::endl;
	screenBuffer = Framebuffer(screenw, screenh);
	screenBuffer.Create();
	trailBuffer = Framebuffer(screenw, screenh);
	trailBuffer.Create();
	trailBuffer.Clear(0.0f, 0.0f, 0.0f, 1.0f);
	glowBuffer = Framebuffer(screenw, screenh);
	glowBuffer.Create();
}

void GPU::makeVBs() {
	std::cout << "Allocating vertex buffers..." << std::endl;
	pointsVB = Vertbuffer(GL_POINTS, settings.MAX_POINTS, GL_STREAM_DRAW);
	pointsVB.Attribute(2, GL_FLOAT); // pos
	pointsVB.Attribute(3, GL_FLOAT); // color
	pointsVB.Attribute(1, GL_FLOAT); // size
	pointsVB.Create();
	linesVB = Vertbuffer(GL_LINES, settings.MAX_LINES, GL_STREAM_DRAW);
	linesVB.Attribute(2, GL_FLOAT); // pos
	linesVB.Attribute(3, GL_FLOAT); // color
	linesVB.Create();
	screenVB = Vertbuffer(GL_TRIANGLES, 2, GL_STATIC_DRAW);
	screenVB.Attribute(2, GL_FLOAT); // pos
	screenVB.Attribute(2, GL_FLOAT); // uv
	screenVB.Create();
	bufferVB = Vertbuffer(GL_TRIANGLES, 2, GL_STATIC_DRAW);
	bufferVB.Attribute(2, GL_FLOAT); // pos
	bufferVB.Attribute(2, GL_FLOAT); // uv
	bufferVB.Create();
}

GLFWwindow* GPU::Setup(void (*onResize)(GLFWwindow* window, int neww, int newh)) {
	std::cout << "GPU: GLFW initializing" << std::endl;
	glfwInit();

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_DOUBLEBUFFER, GLFW_TRUE);
	glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);
	glfwWindowHint(GLFW_FOCUS_ON_SHOW, GLFW_TRUE);
	glfwWindowHint(GLFW_SCALE_TO_MONITOR, GLFW_TRUE);
	window = glfwCreateWindow(w, h, "VEX", NULL, NULL);
	if (window == NULL) {
		glfwTerminate();
		std::cout << "GPU: GLFW init failed!" << std::endl;
		throw "GLFW init failed!";
	}
	glfwMakeContextCurrent(window);
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
		std::cout << "GPU: Failed to initialize GLAD" << std::endl;
		throw "GLFW init failed!";
	}
	glfwSetFramebufferSizeCallback(window, onResize);
	std::cout << "GPU: GLFW window initialized" << std::endl;

	glEnable(GL_LINE_SMOOTH);
	glEnable(GL_PROGRAM_POINT_SIZE);
	glEnable(GL_VERTEX_PROGRAM_POINT_SIZE);
	glDepthMask(false);
	glDisable(GL_DEPTH_TEST);

	makeShaders();
	Resize(w, h);  // this also makes the framebuffers
	makeVBs();

	Reset();
	std::cout << "GPU initialized" << std::endl;
	return window;
}

void GPU::Stop() {
	glfwTerminate();
}

void GPU::OnUpdate(float delta) {
	for (int i = 0; i < spritec; i++) {
		GPUSprite* s = &sprites[i];
		if (s->active && (s->v.dx != 0.0f || s->v.dy != 0.0f)) {
			s->p = s->p + s->v * delta;
		}
	}
	for (int i = 0; i < particlec; i++) {
		GPUParticle* p = &particles[i];
		if (p->active) {
			p->v = p->v + p->vv * delta;
			p->p = p->p + p->v * delta;
			p->rot = p->rot + p->rotv * delta;
			float progress = p->age / p->lifetime;
			p->color = (p->color0 * (1.0 - progress)) + (p->color1 * progress);
			p->scale = (p->scale0 * (1.0 - progress)) + (p->scale1 * progress);
			p->age += delta;
			p->dirty = true;
			if (p->age > p->lifetime) {
				p->active = false;
			}
		}
	}
}

// Copy all our abstract prims into vertex buffers, to prep for render
void GPU::Assemble() {
	pointsVB.Reset();
	linesVB.Reset();
	for (int i = 0; i < pointc; i++) {
		points[i].PushData(pointsVB.vertdata, &pointsVB.vertdatac, settings.ASPECT_RATIO); 
	}
	for (int i = 0; i < linec; i++) {
		lines[i].PushData(linesVB.vertdata, &linesVB.vertdatac, settings.ASPECT_RATIO);
	}
	for (int i = 0; i < spritec; i++) {
		if (sprites[i].active && sprites[i].visible) {
			sprites[i].PushData(linesVB.vertdata, &linesVB.vertdatac, pointsVB.vertdata, &pointsVB.vertdatac, settings.ASPECT_RATIO);
		}
	}
	for (int i = 0; i < particlec; i++) {
		if (particles[i].active && particles[i].visible) {
			particles[i].PushData(linesVB.vertdata, &linesVB.vertdatac, pointsVB.vertdata, &pointsVB.vertdatac, settings.ASPECT_RATIO);
		}
	}
	pointsVB.Update();
	linesVB.Update();
	bufferVB.BulkLoad(screendata, 24);
	screenVB.BulkLoad(scaledscreen, 24);
}

void GPU::drawPrims(float pointThick, float lineThick, float pointBright, float lineBright) {
	glEnable(GL_BLEND);
	glBlendFunc(GL_ONE, GL_ONE);
	glLineWidth(lineThick);

	pointShader.Use("brightness", pointBright);
	pointShader.SetUniform("time", (float)glfwGetTime());
	pointShader.SetUniform("spread", settings.POINT_SPREAD);
	pointShader.SetUniform("stability", settings.POINT_STABILITY);
	pointShader.SetUniform("sustain", settings.BEAM_SUSTAIN);
	pointShader.SetUniform("drop", settings.BEAM_DROP);
	pointShader.SetUniform("thickness", pointThick);
	pointsVB.Draw();

	lineShader.Use("brightness", lineBright);
	lineShader.SetUniform("time", (float)glfwGetTime());
	lineShader.SetUniform("spread", settings.LINE_SPREAD);
	lineShader.SetUniform("stability", settings.LINE_STABILITY);
	lineShader.SetUniform("sustain", settings.BEAM_SUSTAIN);
	lineShader.SetUniform("drop", settings.BEAM_DROP);
	linesVB.Draw();

	glDisable(GL_BLEND);
}

void GPU::Render() {
	glViewport(0, 0, screenw, screenh);

	// Draw to trailbuffer, then blur it
	trailBuffer.Target();
	if (settings.DRAW_TRAILS) drawPrims(settings.POINT_THICKNESS, settings.LINE_THICKNESS, settings.POINT_TRAIL_BRIGHTNESS, settings.LINE_TRAIL_BRIGHTNESS);
	blurShader.Use();
	trailBuffer.BindAsTexture(GL_TEXTURE0, blurShader, "texture", 0);
	blurShader.Blur(bufferVB, screenw, screenh, settings.TRAIL_BLUR);

	// Fade trailbuffer
	trailBuffer.Target();
	trailBuffer.BindAsTexture(GL_TEXTURE0, fadeShader, "texture", 0);
	fadeShader.Use("decay", settings.TRAIL_DECAY);
	trailBuffer.Blit(fadeShader, bufferVB);

	// Draw to glowbuffer, then blur it
	glowBuffer.Target();
	glowBuffer.Clear(0.0f, 0.0f, 0.0f, 0.0f);
	if (settings.DRAW_GLOW) drawPrims(settings.POINT_GLOW_THICKNESS, settings.LINE_GLOW_THICKNESS, settings.POINT_GLOW_BRIGHTNESS, settings.LINE_GLOW_BRIGHTNESS);
	glowBuffer.Target();
	blurShader.Use();
	glowBuffer.BindAsTexture(GL_TEXTURE0, blurShader, "texture", 0);
	blurShader.Blur(bufferVB, screenw, screenh, 0.5f);
	blurShader.Blur(bufferVB, screenw, screenh, 1.0f);
	blurShader.Blur(bufferVB, screenw, screenh, 2.0f);

	// Draw to screenbuffer
	screenBuffer.Target();
	screenBuffer.Clear(0.0f, 0.0f, 0.0f, 1.0f);
	if (settings.DRAW_SCREEN) drawPrims(settings.POINT_THICKNESS, settings.LINE_THICKNESS, settings.POINT_BRIGHTNESS, settings.LINE_BRIGHTNESS);

	// Compose to screen
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glClearColor(settings.LETTERBOX_R, settings.LETTERBOX_G, settings.LETTERBOX_B, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);
	glViewport(0, 0, w, h);
	composeShader.Use();
	composeShader.SetUniform("xscale", xscale);
	composeShader.SetUniform("yscale", yscale);
	screenBuffer.BindAsTexture(GL_TEXTURE0, composeShader, "screenTex", 0);
	glowBuffer.BindAsTexture(GL_TEXTURE1, composeShader, "glowTex", 1);
	trailBuffer.BindAsTexture(GL_TEXTURE2, composeShader, "trailTex", 2);
	screenVB.Draw();

	// Finish the frame
	glfwSwapBuffers(window);
	glfwPollEvents();
}

void GPU::toggleLayer(int layer) {
	if (layer == 0) settings.DRAW_SCREEN = (settings.DRAW_SCREEN ? false : true);
	if (layer == 1) settings.DRAW_GLOW = (settings.DRAW_GLOW ? false : true);
	if (layer == 2) settings.DRAW_TRAILS = (settings.DRAW_TRAILS ? false : true);
}

void GPU::Collide(float delta) {
	for (int i = 0; i < colW; i++) {
		for (int j = 0; j < colH; j++) {
			collideMap[i][j] = 0;
		}
	}
	for (int sp = 0; sp < spritec; sp++) {
		if (sprites[sp].collides) {
			for (int i = 0; i < 8; i++) {
				sprites[sp].colliders[i].id = 0;
			}
			sprites[sp].colliderc = 0;
		}
	}
	short x1, y1, x2, y2;
	for (short sp = 0; sp < spritec; sp++) {
		if (sprites[sp].collides) {
			for (short l = 0; l < sprites[sp].datac; l++) {
				x1 = std::min(colW-1, std::max(0, (int)(colW * ((sprites[sp].data_out[l].x1 + 1.0f) * 0.5f))));
				y1 = std::min(colH-1, std::max(0, (int)(colH * ((sprites[sp].data_out[l].y1 + 1.0f) * 0.5f))));
				x2 = std::min(colW-1, std::max(0, (int)(colW * ((sprites[sp].data_out[l].x2 + 1.0f) * 0.5f))));
				y2 = std::min(colH-1, std::max(0, (int)(colH * ((sprites[sp].data_out[l].y2 + 1.0f) * 0.5f))));
				collideLine(sp, x1, y1, x2, y2);
			}
			for (short p = 0; p < sprites[sp].pdatac; p++) {
				// collide points by drawing their vector
				x1 = std::min(colW - 1, std::max(0, (int)(colW * ((sprites[sp].pdata_out[p].x + 1.0f) * 0.5f))));
				y1 = std::min(colH - 1, std::max(0, (int)(colH * ((sprites[sp].pdata_out[p].y + 1.0f) * 0.5f))));
				x2 = std::min(colW - 1, std::max(0, (int)(colW * (((sprites[sp].pdata_out[p].x + sprites[sp].v.dx * delta) + 1.0f) * 0.5f))));
				y2 = std::min(colH - 1, std::max(0, (int)(colH * (((sprites[sp].pdata_out[p].y + sprites[sp].v.dy * delta) + 1.0f) * 0.5f))));
				collideLine(sp, x1, y1, x2, y2);
			}
		}
	}
}

inline void GPU::collideLine(short sp, short x1, short y1, short x2, short y2) {
	short dx = abs(x2 - x1), sx = x1 < x2 ? 1 : -1;
	short dy = abs(y2 - y1), sy = y1 < y2 ? 1 : -1;
	short err = (dx > dy ? dx : -dy) / 2;
	while (x1 != x2 || y1 != y2) {
		if ((collideMap[x1][y1] > 0) && (collideMap[x1][y1] != sp)) {
			// Record collision between sp and osp
			short osp = collideMap[x1][y1];
			Collider* spc = &sprites[sp].colliders[sprites[sp].colliderc];
			if (sprites[sp].colliderc < 8) { sprites[sp].colliderc++; }
			Collider* ospc = &sprites[osp].colliders[sprites[osp].colliderc];
			if (sprites[osp].colliderc < 8) { sprites[osp].colliderc++; }
			spc->id = osp;
			ospc->id = sp;
			spc->p.x = ((x1 / (float)colW) * 2.0f) - 1.0f;
			spc->p.y = ((y1 / (float)colH) * 2.0f) - 1.0f;
			ospc->p = spc->p;
		}
		collideMap[x1][y1] = sp;

		short e2 = err;
		if (e2 > -dx) { err -= dy; x1 += sx; }
		if (e2 < dy) { err += dx; y1 += sy; }
	}
}

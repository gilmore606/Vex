#pragma once
struct Line
{
	Line();
	Line(float x1, float y1, float x2, float y2, float r, float g, float b);

	void pushData(float dataArray[], int* counter);
	void move();

private:
	float x1, y1, x2, y2;
	float r, g, b;
	float xd, yd;
};


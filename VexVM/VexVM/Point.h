#pragma once
struct Point
{
	Point();
	Point(float x, float y, float r, float g, float b);

	void pushData(float dataArray[], int* counter);

private:
	float x, y, r, g, b;
};


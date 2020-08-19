#pragma once


class Line
{
public:
	Line();
	Line(float x1, float y1, float x2, float y2, float r, float g, float b);

	void PushData(float dataArray[], int* counter, float aspect);

	float x1, y1, x2, y2;
	float r, g, b;
};

inline void Line::PushData(float dataArray[], int* counter, float aspect) {
	dataArray[*counter] = x1 / aspect;
	dataArray[*counter + 1] = y1;
	dataArray[*counter + 2] = r;
	dataArray[*counter + 3] = g;
	dataArray[*counter + 4] = b;
	dataArray[*counter + 5] = x2 / aspect;
	dataArray[*counter + 6] = y2;
	dataArray[*counter + 7] = r;
	dataArray[*counter + 8] = g;
	dataArray[*counter + 9] = b;
	*counter += 10;
}

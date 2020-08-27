#pragma once

#include <string>

class Task {
public:

	uint8_t* ip;
	double waketime;

	Task(uint8_t* ip, double waketime);

private:
};

#include "Task.h"

Task::Task(uint8_t* ip, double waketime) {
	this->ip = ip;
	this->waketime = waketime;
}

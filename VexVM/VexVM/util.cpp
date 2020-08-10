#include <iostream>
#include <fstream>
#include <sstream>

static float randFloat() {
	return static_cast <float> (rand()) / static_cast <float> (RAND_MAX);
}

static std::string stringFromFile(const char* path) {
	std::ifstream file(path);
	std::string empty = "";
	if (!file.is_open()) {
		std::cout << "Failed to load text file\n" << std::endl;
		return empty;
	}
	std::ostringstream string_stream;
	string_stream << file.rdbuf();
	return string_stream.str();
}

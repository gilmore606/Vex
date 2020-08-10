#include "APU.h"
#include "RtAudio.h"
#include <iostream>

APU::APU() { }

void APU::Reset() {

}

void APU::Setup() {
	try {
		adac = new RtAudio();
		std::cout << "APU initialized" << std::endl;
	}
	catch (RtAudioError &e) {
		e.printMessage();
	}
}

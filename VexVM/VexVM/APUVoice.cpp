#include "APUVoice.h"

APUVoice::APUVoice() { 
	volume = 1.0f;
	pan = 0.5f;
	enabled = true;
	testTone = false;
	envMain = new ADSR();
}

void APUVoice::Reset() {

}

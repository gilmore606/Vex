#include "DemoSynth.h"
#include "Sprite.h"

void DemoSynth::Boot() {

	std::string text1 = "APU patch test";

	std::string textADSR = "a   d   s   r";

	
	Sprite stext1 = Sprite(&text1, 1, gpu, 1.0f, 0.6f, 0.0f);
	stext1.scale(0.10f, 0.15f);
	stext1.moveTo(-0.9f, 0.72f);

	Sprite stext2 = Sprite(&textADSR, 0, gpu, 0.8f, 0.4f, 0.0f);
	stext2.scale(0.04f, 0.04f);
	stext2.moveTo(-0.4f, 0.0f);

	livesong = new Song(999);
	livesong->volume = 1.0;
	livesong->pan = 0.5;
	voice = apu->getVoice();
	voice->Patch(0.5, 1.0, 1.1, 3.0, 0.0, 3.0, SINE, PULSE, 0.0, 0.0);
	voice->filter = NO_FILTER;
	voice->filterF = 0.9;
	voice->filterQ = 0.0;
	voice->oscMix = 0.3;
	voice->osc2->transpose = 12;
	voice->lfo->osc.waveform = SAWTOOTH;
	voice->lfo->osc.setFreq(6.4);
	voice->lfo->amount = 0.7;
	voice->lfo->target = M_PITCH;
	voice->adsrFilter->set(0.0, 0.4, 0.5, 2.0);
	voice->adsrFilterAmount = 0.0;
	voice->adsrAux->set(1.3, 0.0, 1.0, 3.0);
	voice->adsrAuxAmt = 0.8;
	voice->adsrAuxTarget = M_PITCH;
	voice->echoLevel = 0.0;
	voice->echoTime = 0.8;
	voice->echoRegen = 0.5;

	livesong->addVoice(voice);
}

void DemoSynth::OnUpdate(float delta) {
	livesong->advanceTime(delta);
}

void DemoSynth::OnInput(int input) {

}

void DemoSynth::OnMIDI(Note* note) {
	if (note->type == NOTE_ON && note->data2 == 0 && note->data1 == lastLiveNote) {
		note->type = NOTE_OFF;
	}
	if (note->type == NOTE_ON && note->data2 > 0) {
		lastLiveNote = note->data1;
	}
	if (note->type == CONTROL_CHANGE) {
		ADSR* adsr = livesong->voices[0]->adsrMain;
		if (input->isShift) {
			adsr = livesong->voices[0]->adsrFilter;
		}
		if (note->data1 == 74) {
			adsr->setA((double)note->data2 / 32.0);
		}
		if (note->data1 == 71) {
			adsr->setD((double)note->data2 / 32.0);
		}
		if (note->data1 == 81) {
			adsr->setS((double)note->data2 / 128.0);
		}
		if (note->data1 == 91) {
			adsr->setR((double)note->data2 / 32.0);
		}
		if (note->data1 == 16) {
			livesong->voices[0]->filterF = (double)note->data2 / 128.0;
		}
		if (note->data1 == 80) {
			livesong->voices[0]->filterQ = (double)note->data2 / 128.0;
		}
		if (note->data1 == 19) {
			livesong->voices[0]->osc1->pwidth = (double)note->data2 / 128.0;
		}
		if (note->data1 == 2) {
			livesong->voices[0]->osc2->pwidth = (double)note->data2 / 128.0;
		}
	}

	livesong->playNote(note);
}

void DemoSynth::Stop() {

}

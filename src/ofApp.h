#pragma once

#include "ofMain.h"
#include "ofxDatGui.h"
#include "circularBuffer.h"
#include "serial.h"

#define ARRAY_LENGTH(arr) (sizeof(arr) / sizeof(*arr))

enum class MessageType {
	KEY = 0,
	ENVELOPE,
	SEQUENCER,
	OSCILLATOR,
	LFO,
	TEMPO,
	PAUSE
};

enum KeyMsgType {
	KEY_PRESS = 0,
	KEY_RELEASE
};

enum SequencerMsgType {
	STEP_PRESS = 0,
	RESET,
	STEP_CHANGE
};

enum EnvelopeMsgType {
	ATTACK = 0,
	HOLD,
	DECAY,
	SUSTAIN,
	RELEASE
};

enum OscMsgType {
	WAVEFORM = 0,
	FREQUENCY, 
	AMPLITUDE,
	SWITCH_OSC
};

struct KeyFreq {
	char key;
	float frequency;
};

struct LFO {
	float amplitude;
	float frequency;
	short waveformIndex;
};

struct Oscillator {
	short waveformIndex;
	short envelopeIndex;
	float* ahdsr;
	float amplitude;
};

struct OscUnit {
	float ahdsr[5];
	Oscillator oscillators[2];
	// TODO noise amp..
};

class ofApp : public ofBaseApp {

	public:
		void setup();
		void update();
		void draw();

		void exit();
		void audioIn(ofSoundBuffer& input);
		void audioOut(ofSoundBuffer& output);

		void keyPressed(int key);
		void keyReleased(int key);
		void mousePressed(int x, int y, int button);
		void onMouseScrollEvent(ofMouseEventArgs& mouse);

		void sendKeyOn(int key);
		void sendKeyOff();
		void sendEnvelopeChange(unsigned char type, float value);
		void sendSequencerStepPress(short seqInex, short stepIndex);
		void sendSequencerReset();
		void sendOscChange(short oscUnitIndex, short oscIndex);
		void sendOscWaveformChange(short waveformIndex);
		void sendOscFrequencyChange(float freq);
		void sendOscAmplitudeChange(float val);
		void sendLFOFrequencyChange(float freq);
		void sendLFOWaveformChange(short waveformIndex);
		void sendLFOAmplitudeChange(float val);

		void sendModeChange(bool mode);
		void sendTempoChange(int val);
		void sendStepChange(short val);
	
		vector<ofxDatGuiComponent*> components;
		ofxDatGuiSlider* AHDSRSliders[5];
		ofxDatGuiDropdown* oscWaveformDropdown;
		ofxDatGuiDropdown* lfoWaveformDropdown;
		ofxDatGuiSlider* lfoFreqSlider;
		ofxDatGuiSlider* lfoAmpSlider;

		void onButtonEvent(ofxDatGuiButtonEvent e);
		void onSliderEvent(ofxDatGuiSliderEvent e);
		void LFOSliderFreq(ofxDatGuiSliderEvent e);
		void LFOSliderAmp(ofxDatGuiSliderEvent e);
		void oscToggleEvent(ofxDatGuiMatrixEvent e);
		void onMatrixEvent0(ofxDatGuiMatrixEvent e);
		void onMatrixEvent1(ofxDatGuiMatrixEvent e);
		void onMatrixEvent2(ofxDatGuiMatrixEvent e);
		void oscWaveformDropdownEvent(ofxDatGuiDropdownEvent e);
		void LFOWaveformDropdownEvent(ofxDatGuiDropdownEvent e);

		void updateUIAfterOscSwitch(short selectedOscUnitIndex, short selectedOscIndex);
		void updateEnvelopePoints(int width, int height);

		bool audioOn = false;
		int lastPressedKey;
	
		float updown = 0;
		int sampleRate;
		int bufferSize;
		int numChannels;

		ofSoundStream inStream;
		ofSoundStreamSettings inStreamSettings;
		ofSoundStream outStream;
		ofSoundStreamSettings outStreamSettings;
		CircularBuffer* buffer;

		ofSerial serial;

		// serialBuffer array data layout (bytes-1):
		// 0   - message type (keypress/parameter)
		// 1   - additional info like oscillator id
		// 2-7 - data
		unsigned char serialBuffer[8];
		unsigned char receiveBuffer[8];

		ofPolyline line;

		OscUnit oscUnits[2];
		Oscillator* oscillators[4];
		short selectedOscUnitIndex = 0;
		short selectedOscIndex = 0;
		LFO lfos[2];

		int octave = 3;
		KeyFreq keyFreq[12];

		bool editMode = true;

		short currentStep = 0;
};
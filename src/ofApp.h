#pragma once

#include "ofMain.h"
#include "ofxDatGui.h"
#include "circularBuffer.h"
#include "serial.h"

enum class MessageType {
	KEY = 0,
	ENVELOPE,
	SEQUENCER
};

enum SequencerMsgType {
	STEP_PRESS = 0,
	RESET
};

enum EnvelopeMsgType {
	ATTACK = 0,
	HOLD,
	DECAY,
	SUSTAIN,
	RELEASE
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

		void sendKeyOn(int key);
		void sendKeyOff();
		void sendEnvelopeChange(short type, uint16_t value);
		void sendSequencerStepPress(int stepIndex);
		void sendSequencerReset();
	
		vector<ofxDatGuiComponent*> components;
		void onButtonEvent(ofxDatGuiButtonEvent e);
		void onSliderEvent(ofxDatGuiSliderEvent e);
		void onMatrixEvent(ofxDatGuiMatrixEvent e);

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

		ofPolyline line;
		float ahdsr[5];

		bool steps[8];
};
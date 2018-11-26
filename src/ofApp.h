#pragma once

#include "ofMain.h"
#include "ofxDatGui.h"
#include "circularBuffer.h"

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
		void sendParameterChange(short type, uint16_t value);
	
		vector<ofxDatGuiComponent*> components;
		void onButtonEvent(ofxDatGuiButtonEvent e);
		void onSliderEvent(ofxDatGuiSliderEvent e);

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
};
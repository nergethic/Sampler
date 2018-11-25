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

		void keyPressed(int key);s
		void keyReleased(int key);
	
		vector<ofxDatGuiComponent*> components;
		void onButtonEvent(ofxDatGuiButtonEvent e);

		bool audioOn = true;
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
		unsigned char serialBuffer[8];
};

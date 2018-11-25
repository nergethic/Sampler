#include "ofApp.h"
#include <vector>

ofImage image;

enum MessageIndex {
	MESSAGE_TYPE = 0,
	DATA
};

enum class MessageType {
	KEY = 0,
	PARAMETER
};

//--------------------------------------------------------------
void ofApp::setup() {

	ofSetLogLevel(OF_LOG_VERBOSE);

	audioOn = false;

	// UI
	int x = 20;
	int y = 20;
	int p = 40;
	ofxDatGuiComponent* component;

	component = new ofxDatGuiButton("button");
	component->setPosition(x, y);
	component->onButtonEvent(this, &ofApp::onButtonEvent);
	components.push_back(component);

	//ofSoundDevice::Api:MS_DS;

	sampleRate = 44100;
	bufferSize = 256; //8192;
	numChannels = 2;

	if (audioOn) {
		vector<ofSoundDevice> devices = inStream.getDeviceList(ofSoundDevice::Api::DEFAULT);
		inStreamSettings.setInDevice(devices[2]);

		float* bufferData = (float*)malloc(16384 * sizeof(float));
		buffer = circular_buf_init(bufferData, 16384);

		// IN
		inStreamSettings.setInListener(this);
		inStreamSettings.numInputChannels = 2;
		inStreamSettings.numOutputChannels = 0;
		inStreamSettings.sampleRate = sampleRate;
		inStreamSettings.bufferSize = bufferSize;
		inStream.setup(inStreamSettings);

		// OUT
		outStreamSettings.setOutListener(this);
		outStreamSettings.numInputChannels = 0;
		outStreamSettings.numOutputChannels = 2;
		outStreamSettings.sampleRate = sampleRate;
		outStreamSettings.bufferSize = bufferSize;
		outStream.setup(outStreamSettings);
	}

	vector <ofSerialDeviceInfo> deviceList = serial.getDeviceList();
	if (!serial.setup("COM4", 115200)) {
		ofLogError() << "could not open serial port";
		//OF_EXIT_APP(0);
	}
}

//--------------------------------------------------------------
void ofApp::update() {
	for (int i = 0; i < components.size(); i++) components[i]->update();
}

void ofApp::exit() {
	serialBuffer[MessageIndex::MESSAGE_TYPE] = (char)MessageType::KEY;
	serialBuffer[MessageIndex::DATA] = 255; // NOTE ON
	serial.writeBytes(serialBuffer, 8);

	if (audioOn) {
		inStream.close();
		outStream.close();
		free(buffer->data);
		circular_buf_free(buffer);
	}
}

void ofApp::audioIn(ofSoundBuffer& input) {
	float left, right;
	for (size_t i = 0; i < input.getNumFrames(); ++i) {
		left = input[i*numChannels + 0];
		right = input[i*numChannels + 1];
		circular_buf_put(buffer, left);
		circular_buf_put(buffer, right);
		//circular_buf_put(buffer, input[i*numChannels + 0]);
		//left[i]  = input[i*numChannels + 0];
		//right[i] = input[i*numChannels + 1];
	}
}

void ofApp::audioOut(ofSoundBuffer& output) {

	float inputSample;
	float currentSample;
	float* data = new float;

	for (size_t i = 0; i < output.getNumFrames(); ++i) {

		//inputSample = inputBuffer[i];
		//currentSample = inputSample;

		circular_buf_get(buffer, data);
		output[i * 2 + 0] = *data;

		circular_buf_get(buffer, data);
		output[i * 2 + 1] = *data;
	}
}

//--------------------------------------------------------------

//--------------------------------------------------------------
void ofApp::draw() {
	ofBackground(40);
	for (int i = 0; i < components.size(); i++) components[i]->draw();
	//fbo->begin();
	//ofClear(ui->pColor["bg"]);
	//drawFbo();
	//fbo->end();
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key) {

	if (lastPressedKey == key) return;

	serialBuffer[MessageIndex::MESSAGE_TYPE] = (char)MessageType::KEY;
	serialBuffer[MessageIndex::DATA] = (char)key; // NOTE ON
	serial.writeBytes(serialBuffer, 8);

	//bool byteWasWritten = serial.writeByte((char)key);
	//if (!byteWasWritten) {
	//	printf("byte was not written to serial port");
	//}

	lastPressedKey = key;
}

//--------------------------------------------------------------
void ofApp::keyReleased(int key) {
	if (lastPressedKey == key) {
		lastPressedKey = -1;
	}
	serialBuffer[MessageIndex::MESSAGE_TYPE] = (char)MessageType::KEY;
	serialBuffer[MessageIndex::DATA] = 255; // NOTE OFF
	serial.writeBytes(serialBuffer, 8);
}

enum ParameterType {
	ATTACK = 0,
	HOLD,
	DECAY,
	SUSTAIN,
	RELEASE
};

//--------------------------------------------------------------

void ofApp::onButtonEvent(ofxDatGuiButtonEvent e)
{
	cout << "onButtonEvent: " << e.target->getLabel() << endl;
}
/*
void ofApp::uiEvents(uiEv& e) {
	if (*tab == "synth") {

		serialBuffer[MessageIndex::MESSAGE_TYPE] = (char)MessageType::PARAMETER;
		*((uint16_t*)(serialBuffer + 2)) = (uint16_t)e.f;
		unsigned char val = 255;

		if (e.name == "attack") {
			val = ParameterType::ATTACK;
		}

		if (e.name == "hold") {
			val = ParameterType::HOLD;
		}

		if (e.name == "decay") {
			val = ParameterType::DECAY;
		}

		if (e.name == "sustain") {
			val = ParameterType::SUSTAIN;
		}

		if (e.name == "release") {
			val = ParameterType::RELEASE;
		}

		if (val == 255) return;
		serialBuffer[1] = val;
		serial.writeBytes(serialBuffer, 8);
	}
}
*/
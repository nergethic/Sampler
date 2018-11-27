#include "ofApp.h"
//#include <vector>

void ofApp::updateEnvelopePoints(int width, int height) {
	int x = 0, y = height;
	int offsetX = ofGetWidth() / 2;
	int offsetY = ofGetHeight() / 2;

	int maxWidth = 5 * 11880;
	float ratio = (float)width / (float)maxWidth;
	float ratioY = (float)height / (float)maxWidth;

	line.clear();

	line.addVertex(offsetX + x, offsetY);

	// A
	x = ahdsr[0] * ratio;
	y = height;
	line.addVertex(offsetX + x, offsetY - y);
	// H
	x += ahdsr[1] * ratio;
	line.addVertex(offsetX + x, offsetY - y);
	// D S
	x += ahdsr[2] * ratio;
	y = ahdsr[3]*height;
	line.addVertex(offsetX + x, offsetY - y);
	x += 30;
	line.addVertex(offsetX + x, offsetY - y);
	// R
	x += ahdsr[4] * ratio;
	y = 0;
	line.addVertex(offsetX + x, offsetY - y);
}

//--------------------------------------------------------------
void ofApp::setup() {

	ofSetLogLevel(OF_LOG_VERBOSE);

	keyFreq[0].key = 'a';
	keyFreq[0].frequency = 65.406392;
	// praca domowa dla Filipa :)

	// UI elements
	int x = 20;
	int y = 20;
	int p = 40;
	ofxDatGuiComponent* component;

	ahdsr[0] = 10.5;
	ahdsr[1] = 16.5;
	ahdsr[2] = 300.0;
	ahdsr[3] = 0.5;
	ahdsr[4] = 230.0;

	for (int i = 0; i < 8; ++i) {
		steps[i] = 0;
	}

	component = new ofxDatGuiButton("button");
	component->setPosition(x, y);
	component->onButtonEvent(this, &ofApp::onButtonEvent);
	components.push_back(component);

	y += component->getHeight() + p;
	component = new ofxDatGuiSlider("attack", 0.0, 11880.0, ahdsr[0]);
	component->setPosition(x, y);
	component->onSliderEvent(this, &ofApp::onSliderEvent);
	components.push_back(component);

	y += component->getHeight();
	component = new ofxDatGuiSlider("hold", 0.0, 11880.0, ahdsr[1]);
	component->setPosition(x, y);
	component->onSliderEvent(this, &ofApp::onSliderEvent);
	components.push_back(component);

	y += component->getHeight();
	component = new ofxDatGuiSlider("decay", 0.0, 11880.0, ahdsr[2]);
	component->setPosition(x, y);
	component->onSliderEvent(this, &ofApp::onSliderEvent);
	components.push_back(component);

	y += component->getHeight();
	component = new ofxDatGuiSlider("sustain", 0.0, 1.0, ahdsr[3]);
	component->setPosition(x, y);
	component->onSliderEvent(this, &ofApp::onSliderEvent);
	components.push_back(component);

	y += component->getHeight();
	component = new ofxDatGuiSlider("release", 0.0, 11880.0, ahdsr[4]);
	component->setPosition(x, y);
	component->onSliderEvent(this, &ofApp::onSliderEvent);
	components.push_back(component);

	y += component->getHeight() + p;
	component = new ofxDatGuiMatrix("steps", 8, true);
	component->setWidth(500, 100.0);
	component->setPosition(x, y);
	component->onMatrixEvent(this, &ofApp::onMatrixEvent);
	components.push_back(component);

	// audio buffers
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


	// serial port
	vector <ofSerialDeviceInfo> deviceList = serial.getDeviceList();
	if (!serial.setup("COM5", 115200)) {
		ofLogError() << "could not open serial port";
		//OF_EXIT_APP(0);
	}
}

//--------------------------------------------------------------
void ofApp::update() {
	for (int i = 0; i < components.size(); i++) components[i]->update();
	updateEnvelopePoints(400, 200);
}

void ofApp::exit() {
	sendKeyOff();
	sendSequencerReset();

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
	}
}

void ofApp::audioOut(ofSoundBuffer& output) {

	float* data = new float;

	for (size_t i = 0; i < output.getNumFrames(); ++i) {

		circular_buf_get(buffer, data);
		output[i * 2 + 0] = *data;

		circular_buf_get(buffer, data);
		output[i * 2 + 1] = *data;
	}
}

//--------------------------------------------------------------
void ofApp::draw() {
	ofBackground(40);
	for (int i = 0; i < components.size(); i++) components[i]->draw();

	line.draw();
	//fbo->begin();
	//ofClear(ui->pColor["bg"]);
	//drawFbo();
	//fbo->end();
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key) {


	if (key >= 48 && key < 58) {
	//	steps[key - 48] = !steps[key - 48];
	}
	//else if (lastPressedKey == key) return;
	if (lastPressedKey == key) return;

	float freq;

	for (int i = 0; i < 12; ++i) {
		if (keyFreq[i].key == (char)key) {
			freq = keyFreq[i].frequency * pow(2, octave);
			break;
		}
	}

	if ((char)key == 'x') {
		octave++;
		return;
	}

	sendFrequencyChange(freq);
	sendKeyOn(key);
	lastPressedKey = key;
}

//--------------------------------------------------------------
void ofApp::keyReleased(int key) {
	if (lastPressedKey == key) {
		lastPressedKey = -1;
	}
	sendKeyOff();
}

// ---------- UI EVENTS ----------

void ofApp::onButtonEvent(ofxDatGuiButtonEvent e)
{
	cout << "onButtonEvent: " << e.target->getLabel() << endl;
}

void ofApp::onSliderEvent(ofxDatGuiSliderEvent e)
{
	unsigned char type;
	float val = e.value;

	string name = e.target->getLabel();

	if (name == "attack") {
		type = EnvelopeMsgType::ATTACK;
		ahdsr[0] = val;
	} else if (name == "hold") {
		type = EnvelopeMsgType::HOLD;
		ahdsr[1] = val;
	} else if (name == "decay") {
		type = EnvelopeMsgType::DECAY;
		ahdsr[2] = val;
	} else if (name == "sustain") {
		type = EnvelopeMsgType::SUSTAIN;
		ahdsr[3] = val;
	} else if (name == "release") {
		type = EnvelopeMsgType::RELEASE;
		ahdsr[4] = val;
	}
	else return;

	sendEnvelopeChange(type, e.value);
}

void ofApp::onMatrixEvent(ofxDatGuiMatrixEvent e) {
	sendSequencerStepPress(e.child);
}
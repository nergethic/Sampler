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
	
	keyFreq[1].key = 'w';
	keyFreq[1].frequency = 69.295658;

	keyFreq[2].key = 's';
	keyFreq[2].frequency = 73.416193;

	keyFreq[3].key = 'e';
	keyFreq[3].frequency = 77.781747;

	keyFreq[4].key = 'd';
	keyFreq[4].frequency = 82.406890;

	keyFreq[5].key = 'f';
	keyFreq[5].frequency = 87.307059;

	keyFreq[6].key = 't';
	keyFreq[6].frequency = 92.498607;

	keyFreq[7].key = 'g';
	keyFreq[7].frequency = 97.998860;

	keyFreq[8].key = 'y';
	keyFreq[8].frequency = 103.826175;

	keyFreq[9].key = 'h';
	keyFreq[9].frequency = 110.000001;

	keyFreq[10].key = 'u';
	keyFreq[10].frequency = 116.540942;

	keyFreq[11].key = 'j';
	keyFreq[11].frequency = 123.470827;

	ahdsr[0] = 10.5;
	ahdsr[1] = 16.5;
	ahdsr[2] = 300.0;
	ahdsr[3] = 0.5;
	ahdsr[4] = 230.0;

	for (int i = 0; i < ARRAY_LENGTH(steps); ++i) {
		steps[i] = 0;
	}

	// UI elements
	int x = 20;
	int y = 20;
	int p = 40;
	ofxDatGuiComponent* component;

	//component = new ofxDatGuiButton("button");
	//component->setPosition(x, y);
	//component->onButtonEvent(this, &ofApp::onButtonEvent);
	//components.push_back(component);

	//y += component->getHeight() + p;
	component = new ofxDatGuiSlider("attack", 0.0f, 3000.0f, ahdsr[0]);
	component->setPosition(x, y);
	component->onSliderEvent(this, &ofApp::onSliderEvent);
	components.push_back(component);

	y += component->getHeight();
	component = new ofxDatGuiSlider("hold", 0.0f, 11880.0f, ahdsr[1]);
	component->setPosition(x, y);
	component->onSliderEvent(this, &ofApp::onSliderEvent);
	components.push_back(component);

	y += component->getHeight();
	component = new ofxDatGuiSlider("decay", 0.0f, 11880.0f, ahdsr[2]);
	component->setPosition(x, y);
	component->onSliderEvent(this, &ofApp::onSliderEvent);
	components.push_back(component);

	y += component->getHeight();
	component = new ofxDatGuiSlider("sustain", 0.0f, 1.0f, ahdsr[3]);
	component->setPosition(x, y);
	component->onSliderEvent(this, &ofApp::onSliderEvent);
	components.push_back(component);

	y += component->getHeight();
	component = new ofxDatGuiSlider("release", 0.0f, 11880.0f, ahdsr[4]);
	component->setPosition(x, y);
	component->onSliderEvent(this, &ofApp::onSliderEvent);
	components.push_back(component);

	y += component->getHeight() + p;
	component = new ofxDatGuiMatrix("steps", 16, true);
	component->setWidth(500, 100.0);
	component->setPosition(x, y);
	component->onMatrixEvent(this, &ofApp::onMatrixEvent0);
	components.push_back(component);

	y += component->getHeight() + p;
	component = new ofxDatGuiMatrix("guziczki2", 16, true);
	component->setWidth(500, 100.0);
	component->setPosition(x, y);
	component->onMatrixEvent(this, &ofApp::onMatrixEvent1);
	components.push_back(component);

	x += 300;
	y = 20;

	y += component->getHeight();
	component = new ofxDatGuiSlider("LFO freq", 0.0f, 1000.0f, 5.0f);
	component->setPosition(x, y);
	component->onSliderEvent(this, &ofApp::LFOSliderFreq);
	components.push_back(component);

	y += component->getHeight();
	component = new ofxDatGuiSlider("LFO amp", 0.0f, 0.6f, 0.0f);
	component->setPosition(x, y);
	component->onSliderEvent(this, &ofApp::LFOSliderAmp);
	components.push_back(component);

	y += component->getHeight();
	vector<string> waveformOptions = { "WAVEFORM_SINE", "WAVEFORM_SAWTOOTH", "WAVEFORM_SQUARE", "WAVEFORM_TRIANGLE" };
	component = new ofxDatGuiDropdown("LFO", waveformOptions);
	component->onDropdownEvent(this, &ofApp::LFOWaveformDropdown);
	component->setPosition(x, y);
	components.push_back(component);

	y += component->getHeight();
	component = new ofxDatGuiDropdown("OSC", waveformOptions);
	component->onDropdownEvent(this, &ofApp::oscWaveformDropdown);
	component->setPosition(x, y);
	components.push_back(component);

	y += component->getHeight();
	component = new ofxDatGuiToggle("OSC1");
	component->onToggleEvent(this, &ofApp::oscToggle);
	component->setPosition(x, y);
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
	if (!serial.setup("COM4", 115200)) {
		serial.setup("COM5", 115200);
		//ofLogError() << "could not open serial port";
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

	if (key == lastPressedKey || key == 0x0000001b) return; // ESC

	float freq = 0.0f;

	for (int i = 0; i < ARRAY_LENGTH(keyFreq); ++i) {
		if (keyFreq[i].key == (char)key) {
			freq = keyFreq[i].frequency * pow(2, octave);
			sendOscFrequencyChange(freq);
			break;
		}
	}

	switch ((char)key) {
		case 'x': {
			octave++;
			if (octave > 5) {
				octave = 5;
			}
			return;
		} break;

		case 'z': {
			octave--;
			if (octave < 0) {
				octave = 0;
			}
			return;
		} break;

		case ' ': {
			liveMode = !liveMode;
			sendModeChange(liveMode);
			return;
		} break;

		case 'c': {
			sendTempoChange(0);
			return;
		} break;

		case 'v': {
			sendTempoChange(1);
			return;
		} break;
	}

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

void ofApp::onMatrixEvent0(ofxDatGuiMatrixEvent e) {
	sendSequencerStepPress(0, e.child);
}

void ofApp::onMatrixEvent1(ofxDatGuiMatrixEvent e) {
	sendSequencerStepPress(1, e.child);
}

void ofApp::oscWaveformDropdown(ofxDatGuiDropdownEvent e) {
	sendOscWaveformChange((short)e.child);
}

void ofApp::oscToggle(ofxDatGuiToggleEvent e) {
	selectedOscIndex = (int)e.checked;

	serialBuffer[0] = (char)MessageType::OSCILLATOR;
		(int)e.checked;
	//serial.writeBytes(serialBuffer, 8);
}
#include "ofApp.h"

//bool byteWasWritten = serial.writeByte(x);
//if (!byteWasWritten) {
//	printf("byte was not written to serial port");
//}

void clearSerialBuffer(unsigned char* buffer) {
	for (size_t i = 0; i < 8; ++i) buffer[i] = 0;
}

void ofApp::sendKeyOn(int key) {
	serialBuffer[0] = (char)MessageType::KEY;
	serialBuffer[1] = KeyMsgType::KEY_PRESS;
	serialBuffer[2] = (char)key;
	serial.writeBytes(serialBuffer, 8);
	clearSerialBuffer(serialBuffer);
}

void ofApp::sendKeyOff() {
	serialBuffer[0] = (char)MessageType::KEY;
	serialBuffer[1] = KeyMsgType::KEY_RELEASE;
	serial.writeBytes(serialBuffer, 8);
	clearSerialBuffer(serialBuffer);
}

void ofApp::sendEnvelopeChange(unsigned char type, float value) {
	serialBuffer[0] = (char)MessageType::ENVELOPE;
	serialBuffer[1] = type;
	*((float*)(serialBuffer + 2)) = value;

	serial.writeBytes(serialBuffer, 8);
	clearSerialBuffer(serialBuffer);
}

void ofApp::sendSequencerStepPress(short sequencerIndex, short stepIndex) {
	serialBuffer[0] = (char)MessageType::SEQUENCER;
	serialBuffer[1] = SequencerMsgType::STEP_PRESS;
	serialBuffer[2] = (char)sequencerIndex;
	serialBuffer[3] = (char)stepIndex;

	serial.writeBytes(serialBuffer, 8);
	clearSerialBuffer(serialBuffer);
}

void ofApp::sendSequencerReset() {
	serialBuffer[0] = (char)MessageType::SEQUENCER;
	serialBuffer[1] = SequencerMsgType::RESET;

	serial.writeBytes(serialBuffer, 8);
	clearSerialBuffer(serialBuffer);
}

void ofApp::sendOscChange(short oscUnitIndex, short oscIndex) {
	serialBuffer[0] = (char)MessageType::OSCILLATOR;
	serialBuffer[1] = OscMsgType::SWITCH_OSC;
	serialBuffer[2] = (char)oscUnitIndex;
	serialBuffer[3] = (char)oscIndex;

	serial.writeBytes(serialBuffer, 8);
	clearSerialBuffer(serialBuffer);
}

void ofApp::sendOscFrequencyChange(float freq) {
	serialBuffer[0] = (char)MessageType::OSCILLATOR;
	serialBuffer[1] = OscMsgType::FREQUENCY;
	*((float*)(serialBuffer + 2)) = freq;

	serial.writeBytes(serialBuffer, 8);
	clearSerialBuffer(serialBuffer);
}

void ofApp::sendOscWaveformChange(short waveformIndex) {
	serialBuffer[0] = (char)MessageType::OSCILLATOR;
	serialBuffer[1] = OscMsgType::WAVEFORM;
	serialBuffer[2] = waveformIndex;

	serial.writeBytes(serialBuffer, 8);
	clearSerialBuffer(serialBuffer);
}

void ofApp::sendModeChange(bool mode) {
	serialBuffer[0] = (char)MessageType::PAUSE;
	serialBuffer[1] = (int)mode;

	serial.writeBytes(serialBuffer, 8);
	clearSerialBuffer(serialBuffer);
}

void ofApp::sendTempoChange(int val) {
	serialBuffer[0] = (char)MessageType::TEMPO;
	serialBuffer[1] = val;

	serial.writeBytes(serialBuffer, 8);
	clearSerialBuffer(serialBuffer);
}

void ofApp::sendStepChange(short val) {
	serialBuffer[0] = (char)MessageType::SEQUENCER;
	serialBuffer[1] = SequencerMsgType::STEP_CHANGE;
	serialBuffer[2] = val;

	serial.writeBytes(serialBuffer, 8);
	clearSerialBuffer(serialBuffer);
}

void ofApp::LFOWaveformDropdownEvent(ofxDatGuiDropdownEvent e) {
	serialBuffer[0] = (char)MessageType::LFO;
	serialBuffer[1] = OscMsgType::WAVEFORM;
	serialBuffer[2] = (short)e.child;

	serial.writeBytes(serialBuffer, 8);
}

void ofApp::LFOSliderFreq(ofxDatGuiSliderEvent e) {
	serialBuffer[0] = (char)MessageType::LFO;
	serialBuffer[1] = OscMsgType::FREQUENCY;
	*((float*)(serialBuffer + 2)) = e.value;

	serial.writeBytes(serialBuffer, 8);
}

void ofApp::LFOSliderAmp(ofxDatGuiSliderEvent e) {
	serialBuffer[0] = (char)MessageType::LFO;
	serialBuffer[1] = OscMsgType::AMPLITUDE;
	*((float*)(serialBuffer + 2)) = e.value;

	serial.writeBytes(serialBuffer, 8);
}
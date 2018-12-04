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

void ofApp::sendSequencerStepPress(int stepIndex) {
	serialBuffer[0] = (char)MessageType::SEQUENCER;
	serialBuffer[1] = SequencerMsgType::STEP_PRESS;
	serialBuffer[2] = (char)stepIndex;

	serial.writeBytes(serialBuffer, 8);
	clearSerialBuffer(serialBuffer);
}

void ofApp::sendSequencerReset() {
	serialBuffer[0] = (char)MessageType::SEQUENCER;
	serialBuffer[1] = SequencerMsgType::RESET;

	serial.writeBytes(serialBuffer, 8);
	clearSerialBuffer(serialBuffer);
}

void ofApp::sendOscFrequencyChange(float freq) {
	serialBuffer[0] = (char)MessageType::OSCILLATOR;
	serialBuffer[1] = OscMsgType::FREQUENCY;
	serialBuffer[2] = selectedOscIndex;
	*((float*)(serialBuffer + 3)) = freq;

	serial.writeBytes(serialBuffer, 8);
	clearSerialBuffer(serialBuffer);
}

void ofApp::sendOscWaveformChange(short waveformIndex) {
	serialBuffer[0] = (char)MessageType::OSCILLATOR;
	serialBuffer[1] = OscMsgType::WAVEFORM;
	serialBuffer[2] = selectedOscIndex;
	serialBuffer[3] = waveformIndex;

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
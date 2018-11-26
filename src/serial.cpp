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
	serialBuffer[1] = (char)key; // NOTE ON
	serial.writeBytes(serialBuffer, 8);
	clearSerialBuffer(serialBuffer);
}

void ofApp::sendKeyOff() {
	serialBuffer[0] = (char)MessageType::KEY;
	serialBuffer[1] = 255; // NOTE OFF
	serial.writeBytes(serialBuffer, 8);
	clearSerialBuffer(serialBuffer);
}

void ofApp::sendEnvelopeChange(short type, uint16_t value) {
	serialBuffer[0] = (char)MessageType::ENVELOPE;
	serialBuffer[1] = type;
	*((uint16_t*)(serialBuffer + 2)) = (uint16_t)value;

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
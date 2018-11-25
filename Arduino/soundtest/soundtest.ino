#include <Audio.h>
#include <Wire.h>
#include <SPI.h>
#include <SD.h>
#include <SerialFlash.h>

// GUItool: begin automatically generated code
AudioSynthWaveformSine   sine1;          //xy=479,589
AudioEffectEnvelope      envelope1;      //xy=628,589
AudioOutputUSB           usb1;           //xy=921,586
AudioOutputI2S           i2s1;           //xy=922,627
AudioConnection          patchCord1(sine1, envelope1);
AudioConnection          patchCord2(envelope1, 0, i2s1, 1);
AudioConnection          patchCord3(envelope1, 0, i2s1, 0);
AudioConnection          patchCord4(envelope1, 0, usb1, 1);
AudioConnection          patchCord5(envelope1, 0, usb1, 0);
AudioControlSGTL5000     sgtl5000_1;     //xy=490,502
// GUItool: end automatically generated code

int freq = 100;
char serialBuffer[8];
float stepTime;
float previousTime, currentTime;
bool activeSteps[8];
short currentStep = 0;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  sine1.frequency(freq);
  sine1.amplitude(0.4);
  //sine1.begin();
  //lfo.begin(1,3,WAVEFORM_SINE);

  stepTime = 1000;
  previousTime = 0;

  for (int i=0; i < 8; ++i) {
    activeSteps[i] = 0;
  }
  
  AudioMemory(80);
  delay(500);
}

enum SerialMessageType {
  KEY = 0,
  PARAMETER = 1
};

void handleSerialInput() {
  if (Serial.available() >= 8) {

    Serial.readBytes(serialBuffer, 8);

    switch ((SerialMessageType)serialBuffer[0]) {
      case KEY: {
        char keyCode = serialBuffer[1];
        if (keyCode == 255) {
          envelope1.noteOff();
        } else {
          if (keyCode >= 48 && keyCode < 58) {
              activeSteps[keyCode-48] = !activeSteps[keyCode-48];
          } else {
            freq = 340+ 2*((int)serialBuffer[1]);
            sine1.frequency(freq);
            envelope1.noteOn();
          }
        }
      } break;

      case PARAMETER: {
        short val = *((short*)(serialBuffer+2));
        
        switch (serialBuffer[1]) {
          case 0: {
            envelope1.attack((float)val);
          } break;

          case 1: {
            envelope1.hold((float)val);
          } break;

          case 2: {
            envelope1.decay((float)val);
          } break;

          case 3: {
            envelope1.sustain((float)val);
          } break;

          case 4: {
            envelope1.release((float)val);
          } break;
        }
      } break;
    }
  }
}

void loop() {
  
  //elapsedTime = millis() - currentTime;
  //stepTime -= elapsedTime;

  handleSerialInput();

  currentTime = millis();
  if (currentTime - previousTime >= 200) {
    
    if (activeSteps[currentStep] == true) {
      envelope1.noteOn();
      delay(50);
      envelope1.noteOff();
    }

    currentStep++;
    if (currentStep == 8) {
      currentStep = 0;
    }
    
    previousTime = currentTime;
  }
  
  delay(10);
  //envelope1.noteOn();
}

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
short currentStepIndex = 0;

struct SequencerStep {
  bool on;
};
SequencerStep sequencerSteps[8];

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
    sequencerSteps[i].on = 0;
  }
  
  AudioMemory(80);
  delay(500);
}

enum MessageType {
  KEY = 0,
  ENVELOPE,
  SEQUENCER
};

enum SequencerMsgType {
  STEP_PRESS = 0,
  RESET
};

void handleSerialInput() {
  if (Serial.available() >= 8) {

    Serial.readBytes(serialBuffer, 8);

    switch ((MessageType)serialBuffer[0]) {
      case KEY: {
        
        char keyCode = serialBuffer[1];
        if (keyCode == 255) {
          envelope1.noteOff();
          break;
        }
        
        if (keyCode >= 48 && keyCode < 58) {
            sequencerSteps[keyCode-48].on = !sequencerSteps[keyCode-48].on;
        } else {
          freq = 340+ 2*((int)serialBuffer[1]);
          sine1.frequency(freq);
          envelope1.noteOn();
        }
        
      } break;

      case ENVELOPE: {
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

      case SEQUENCER: {
        if (serialBuffer[1] == STEP_PRESS) {
          int stepIndex = serialBuffer[2];
          sequencerSteps[stepIndex].on = !sequencerSteps[stepIndex].on;
        } else if (serialBuffer[1] == RESET) {
          for (int i=0; i < 8; ++i) {
            sequencerSteps[i].on = 0;
          }
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
    
    if (sequencerSteps[currentStepIndex].on == true) {
      envelope1.noteOn();
      delay(50);
      envelope1.noteOff();
    }

    currentStepIndex++;
    if (currentStepIndex == 8) {
      currentStepIndex = 0;
    }
    
    previousTime = currentTime;
  }
  
  delay(10);
  //envelope1.noteOn();
}

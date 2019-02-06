#include <Audio.h>
#include <Wire.h>
#include <SPI.h>
//#include <SD.h>
#include <SerialFlash.h>

#include "AudioSampleKick.h"

// GUItool: begin automatically generated code
AudioSynthWaveformModulated LFO2;           //xy=1010,351
AudioSynthWaveformModulated LFO1;           //xy=1026,175
AudioSynthNoiseWhite     noise2;         //xy=1145,410
AudioSynthWaveformModulated osc3;           //xy=1149,332
AudioSynthWaveformModulated osc4;           //xy=1153,371
AudioSynthNoiseWhite     noise1;         //xy=1162,234
AudioSynthWaveformModulated osc1;           //xy=1166,156
AudioSynthWaveformModulated osc2;           //xy=1170,195
AudioMixer4              mixer3;         //xy=1306,281
AudioMixer4              mixer2;         //xy=1323,106
AudioEffectEnvelope      envelope2;      //xy=1440,286
AudioEffectEnvelope      envelope1;      //xy=1455,119
AudioPlayMemory          playMem1;       //xy=1562,569
AudioSynthSimpleDrum     drum1;          //xy=1573,493
AudioEffectGranular      granular1;      //xy=1598,126
AudioMixer4              mixer1;         //xy=1765,489
AudioMixer4              mixer4;         //xy=1765,572
AudioFilterStateVariable delayFilter;    //xy=1917,619
AudioMixer4              mainOutMixer;   //xy=1991,489
AudioEffectDelay         delay1;         //xy=2008,831
AudioOutputUSB           usb1;           //xy=2186,490
AudioOutputI2S           i2s2;           //xy=2187,527
AudioConnection          patchCord1(LFO2, 0, osc3, 0);
AudioConnection          patchCord2(LFO2, 0, osc4, 0);
AudioConnection          patchCord3(LFO1, 0, osc1, 0);
AudioConnection          patchCord4(LFO1, 0, osc2, 0);
AudioConnection          patchCord5(noise2, 0, mixer3, 2);
AudioConnection          patchCord6(osc3, 0, mixer3, 0);
AudioConnection          patchCord7(osc4, 0, mixer3, 1);
AudioConnection          patchCord8(noise1, 0, mixer2, 2);
AudioConnection          patchCord9(osc1, 0, mixer2, 0);
AudioConnection          patchCord10(osc2, 0, mixer2, 1);
AudioConnection          patchCord11(mixer3, envelope2);
AudioConnection          patchCord12(mixer2, envelope1);
AudioConnection          patchCord13(envelope2, 0, mixer1, 3);
AudioConnection          patchCord14(envelope1, granular1);
AudioConnection          patchCord15(playMem1, 0, mixer4, 0);
AudioConnection          patchCord16(drum1, 0, mixer1, 2);
AudioConnection          patchCord17(granular1, 0, mixer1, 0);
AudioConnection          patchCord18(mixer1, 0, mainOutMixer, 0);
AudioConnection          patchCord19(mixer4, 0, mainOutMixer, 1);
AudioConnection          patchCord20(delayFilter, 0, mainOutMixer, 3);
AudioConnection          patchCord21(mainOutMixer, delay1);
AudioConnection          patchCord22(mainOutMixer, 0, usb1, 0);
AudioConnection          patchCord23(mainOutMixer, 0, usb1, 1);
AudioConnection          patchCord24(mainOutMixer, 0, i2s2, 0);
AudioConnection          patchCord25(mainOutMixer, 0, i2s2, 1);
AudioConnection          patchCord26(delay1, 0, delayFilter, 0);
AudioControlSGTL5000     sgtl5000_1;     //xy=1055,488
// GUItool: end automatically generated code

#define ARRAY_LENGTH(arr) (sizeof(arr) / sizeof(*arr))

float freq = 500.0;
char serialBuffer[8];
unsigned char sendBuff[8];
float stepTime;
float previousTime, currentTime;
float dt, previousFrameTime;
int stepLengthMs = 300;
short currentStepIndex = 0;
bool liveMode = true;
short selectedOscUnitIndex = 0;
short selectedOscIndex = 0;

int16_t grains[2048];

struct SequencerStep {
  bool on;
  float freq;
};
SequencerStep stepSequencers[3][16];

struct OscUnit {
  AudioSynthWaveformModulated* lfo;
  AudioEffectEnvelope* envelope;
  AudioSynthWaveformModulated* osc[2];
};
OscUnit oscUnits[2];

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);

  for (unsigned int i = 0; i < ARRAY_LENGTH(sendBuff); ++i) {
    sendBuff[i] = 0;  
  }

  oscUnits[0].osc[0] = &osc1;
  oscUnits[0].osc[1] = &osc2;
  oscUnits[0].envelope = &envelope1;
  oscUnits[0].lfo = &LFO1;
  
  oscUnits[1].osc[0] = &osc3;
  oscUnits[1].osc[1] = &osc4;
  oscUnits[1].envelope = &envelope2;
  oscUnits[1].lfo = &LFO2;
  
  osc1.begin(0);
  osc1.frequency(freq);
  osc1.amplitude(0.4);

  osc2.begin(0);
  osc2.frequency(freq+100);
  osc2.amplitude(0.4);

  osc3.begin(0);
  osc3.frequency(freq);
  osc3.amplitude(0.4);

  osc4.begin(0);
  osc4.frequency(freq+400);
  osc4.amplitude(0.4);

  //noise1.amplitude(0.6);

  // TODO play with this value
  envelope1.releaseNoteOn(8);
  envelope2.releaseNoteOn(8);

  LFO1.begin(0);
  LFO1.frequency(0);
  LFO1.amplitude(0.0);

  LFO2.begin(0);
  LFO2.frequency(0);
  LFO2.amplitude(0.0);

  stepTime = 24.7;
  previousTime = 0;

  for (unsigned int i=0; i < ARRAY_LENGTH(stepSequencers); ++i) {
    for (unsigned int j=0; j < ARRAY_LENGTH(stepSequencers[0]); ++j) {
      stepSequencers[i][j].on = false;
      stepSequencers[i][j].freq = 0;
    }
  }

  //SD.begin(BUILTIN_SDCARD);

  granular1.begin(grains, (int16_t)2048);
  granular1.setSpeed(0.5);

  delayFilter.frequency(3000);
  delayFilter.resonance(1);
  //delay1.delay(0,400);
  delay1.delay(0,0);
  mainOutMixer.gain(3, 0.0);
  
  AudioMemory(200);
  delay(500);
}

enum MessageType {
  KEY = 0,
  ENVELOPE,
  SEQUENCER,
  OSCILLATOR, // TODO
  LFO, // TODO
  TEMPO,
  PAUSE
};

enum KeyMsgType {
  KEY_PRESS = 0,
  KEY_RELEASE,
  FREQUENCY_CHANGE
};

enum SequencerMsgType {
  STEP_PRESS = 0,
  RESET,
  STEP_CHANGE
};

enum OscMsgType {
  WAVEFORM = 0,
  FREQUENCY,
  AMPLITUDE,
  SWITCH_OSC
};

void handleSerialInput() {
  if (Serial.available() >= 8) {

    Serial.readBytes(serialBuffer, 8);

    switch ((MessageType)serialBuffer[0]) {
      case KEY: {
        char msgType = serialBuffer[1];

        if (msgType == KEY_RELEASE) {
          oscUnits[selectedOscUnitIndex].envelope->noteOff();
          
        } else if (msgType == KEY_PRESS) {
            
          char keyCode = serialBuffer[2];
          if (keyCode >= 48 && keyCode < 58) {
            //currentStepIndex = keyCode-48;
            //stepSequencers[keyCode-48].on = !stepSequencers[keyCode-48].on;
          } else {
            oscUnits[selectedOscUnitIndex].envelope->noteOn();
            //drum1.frequency(freq/2.0);
            //drum1.length(250);
            //drum1.secondMix(0.5);
            //drum1.pitchMod(1.5);
            //drum1.noteOn();
          }
        }
      } break;

      case ENVELOPE: {
        float val = *((float*)(serialBuffer+2));
        
        switch (serialBuffer[1]) {
          case 0: {
            oscUnits[selectedOscUnitIndex].envelope->attack(val);
          } break;

          case 1: {
            oscUnits[selectedOscUnitIndex].envelope->hold(val);
          } break;

          case 2: {
            oscUnits[selectedOscUnitIndex].envelope->decay(val);
          } break;

          case 3: {
            oscUnits[selectedOscUnitIndex].envelope->sustain(val);
          } break;

          case 4: {
            oscUnits[selectedOscUnitIndex].envelope->release(val);
          } break;
        }
      } break;

      case SEQUENCER: {
        if (serialBuffer[1] == STEP_PRESS) {
          short seqIndex = (char)serialBuffer[2];
          short stepIndex = (char)serialBuffer[3];
          bool newStepState = !stepSequencers[seqIndex][stepIndex].on;
          stepSequencers[seqIndex][stepIndex].on = newStepState;
          if (newStepState == false) {
              //stepSequencers[seqIndex][stepIndex].freq = 0;
          }
        } else if (serialBuffer[1] == RESET) {
          for (unsigned int i=0; i < ARRAY_LENGTH(stepSequencers); ++i) {
            for (unsigned int j=0; j < ARRAY_LENGTH(stepSequencers[0]); ++j) {
              stepSequencers[i][j].on = 0;
              stepSequencers[i][j].freq = 0;
            }
          }
        } else if (serialBuffer[1] == STEP_CHANGE) {
          if (serialBuffer[2] == 0) {
            currentStepIndex--;
          } else {
            currentStepIndex++;
          }
          
          if (currentStepIndex == 16) {
            currentStepIndex = 0;
          } else if (currentStepIndex < 0) {
            currentStepIndex = 15;
          }
        }
      } break;

      case OSCILLATOR: {
        char msgType = serialBuffer[1];
        if (msgType == FREQUENCY) {
            freq = *((float*)(serialBuffer+2));
            oscUnits[selectedOscUnitIndex].osc[0]->frequency(freq);
            oscUnits[selectedOscUnitIndex].osc[1]->frequency(freq);
            stepSequencers[selectedOscUnitIndex][currentStepIndex].freq = freq;
        } else if (msgType == WAVEFORM) {
            oscUnits[selectedOscUnitIndex].osc[selectedOscIndex]->begin(serialBuffer[2]);
        } else if (msgType == SWITCH_OSC) {
            selectedOscUnitIndex = (char)serialBuffer[2];
            selectedOscIndex     = (char)serialBuffer[3];
        }
      } break;

      case LFO: {
        char msgType = serialBuffer[1];
        float val = *((float*)(serialBuffer+2));
        // TODO: crazy option - take serialBuffer[3] as value without casting to float
        if (msgType == FREQUENCY) {
          oscUnits[selectedOscUnitIndex].lfo->frequency(val);
          //granular1.beginPitchShift(val);
        } else if (msgType == WAVEFORM) {
          oscUnits[selectedOscUnitIndex].lfo->begin(serialBuffer[2]);
        } else if (msgType == AMPLITUDE) {
          oscUnits[selectedOscUnitIndex].lfo->amplitude(val);
        }
      } break;

      case PAUSE: {
        liveMode = serialBuffer[1];
      } break;

      case TEMPO: {
        if (serialBuffer[1] == 0) {
          stepTime /= 2;
        } else {
          stepTime *= 2;
        }  
      }
    }
  }
}

void loop() {

  // TODO: how many iterations?
  for (int i = 0; i < 8; ++i) {
    handleSerialInput();
  }

  currentTime = millis();
  //dt = (currentTime - previousFrameTime);// / 1000.0;
  //previousFrameTime = currentTime;
  
  // step sequencer
  if (liveMode == true && currentTime - previousTime >= stepTime) {
    
    currentStepIndex++;
    if (currentStepIndex == ARRAY_LENGTH(stepSequencers[0])) {
      currentStepIndex = 0;
    }
    
    for (unsigned int i=0; i < ARRAY_LENGTH(stepSequencers); ++i) {
      if (stepSequencers[i][currentStepIndex].on == true) {
        if (i <= 1) {
            float freq = stepSequencers[i][currentStepIndex].freq;
            if (freq == 0) {
              oscUnits[i].envelope->noteOff();
            } else {
              oscUnits[i].osc[0]->frequency(stepSequencers[i][currentStepIndex].freq);
              oscUnits[i].osc[1]->frequency(stepSequencers[i][currentStepIndex].freq);
              oscUnits[i].envelope->noteOn();
            }
        } else if (i == 2) { // SAMPLER
            playMem1.play(AudioSampleKick);
        }
      
      } //else if (i <= 1) {
        //oscUnits[i].envelope->noteOff();
      //}
    }

    sendBuff[0] = (char)currentStepIndex;
    Serial.write(sendBuff, 8);
    previousTime = currentTime;
  }
  
  delay(5);
}

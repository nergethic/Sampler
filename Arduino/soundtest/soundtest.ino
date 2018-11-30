#include <Audio.h>
#include <Wire.h>
#include <SPI.h>
#include <SD.h>
#include <SerialFlash.h>

// GUItool: begin automatically generated code
AudioPlaySdWav playSdWav1;
AudioMixer4 mixer1;
AudioSynthWaveformModulated   LFO1;          //xy=201,462
AudioSynthWaveformModulated sine1;   //xy=350,462
AudioEffectEnvelope      envelope1;      //xy=499,462
AudioOutputUSB           usb1;           //xy=792,459
AudioOutputI2S           i2s1;           //xy=793,500
AudioConnection          patchCord1(LFO1, 0, sine1, 0);
AudioConnection          patchCord2(sine1, envelope1);
//AudioConnection          patchCord4(envelope1, 1, mixer1, 1);
AudioConnection          patchCord5(playSdWav1, 0, mixer1, 2);
AudioConnection          patchCord6(playSdWav1, 1, mixer1, 3);
AudioConnection          patchCord7(mixer1, 0, i2s1, 1);
AudioConnection          patchCord8(mixer1, 0, i2s1, 0);
AudioConnection          patchCord9(mixer1, 0, usb1, 1);
AudioConnection          patchCord10(mixer1, 0, usb1, 0);
AudioControlSGTL5000     sgtl5000_1;     //xy=211,327

AudioEffectGranular granular1;
AudioConnection          patchCord3(envelope1, 0, granular1, 0);
AudioConnection          patchCord11(granular1, 0, mixer1, 0);

// GUItool: end automatically generated code

#define ARRAY_LENGTH(arr) (sizeof(arr) / sizeof(*arr))

float freq = 500;
char serialBuffer[8];
float stepTime;
float previousTime, currentTime;
float dt, previousFrameTime;
int stepLengthMs = 100;
short currentStepIndex = 0;

int16_t  grains[2048];

AudioSynthWaveformModulated* oscillators[4];
AudioSynthWaveformModulated* lfo[4];

struct SequencerStep {
  bool on;
  float freq;
};
SequencerStep sequencerSteps[16];

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);

  oscillators[0] = &sine1;
  lfo[0] = &LFO1;

  LFO1.begin(0);
  LFO1.frequency(0);
  LFO1.amplitude(0.0);
  
  sine1.begin(0);
  sine1.frequency(freq);
  sine1.amplitude(0.4);
  //sine1.begin();
  //lfo.begin(1,3,WAVEFORM_SINE);

  stepTime = 24.7;
  previousTime = 0;

  for (int i=0; i < ARRAY_LENGTH(sequencerSteps); ++i) {
    sequencerSteps[i].on = 0;
    sequencerSteps[i].freq = freq;
  }

  SD.begin(BUILTIN_SDCARD);

  granular1.begin(grains, (int16_t)2048);
  granular1.setSpeed(0.5);
  
  AudioMemory(100);
  delay(500);
}

enum MessageType {
  KEY = 0,
  ENVELOPE,
  SEQUENCER,
  OSCILLATOR, // TODO
  LFO, // TODO
  TEMPO, // TODO,
  PAUSE
};

enum KeyMsgType {
  KEY_PRESS = 0,
  KEY_RELEASE,
  FREQUENCY_CHANGE
};

enum SequencerMsgType {
  STEP_PRESS = 0,
  RESET
};

enum OscMsgType {
  WAVEFORM = 0,
  FREQUENCY,
  AMPLITUDE
};

void handleSerialInput(int currentStepIndex) {
  if (Serial.available() >= 8) {

    Serial.readBytes(serialBuffer, 8);

    switch ((MessageType)serialBuffer[0]) {
      case KEY: {
        char msgType = serialBuffer[1];

        if (msgType == KEY_RELEASE) {
          envelope1.noteOff();
          } else if (msgType == KEY_PRESS) {
            
          char keyCode = serialBuffer[2];
          if (keyCode >= 48 && keyCode < 58) {
              //sequencerSteps[keyCode-48].on = !sequencerSteps[keyCode-48].on;
          } else {
            envelope1.noteOn();
          }
        }
      } break;

      case ENVELOPE: {
        float val = *((float*)(serialBuffer+2));
        
        switch (serialBuffer[1]) {
          case 0: {
            envelope1.attack(val);
          } break;

          case 1: {
            envelope1.hold(val);
          } break;

          case 2: {
            envelope1.decay(val);
          } break;

          case 3: {
            envelope1.sustain(val);
          } break;

          case 4: {
            envelope1.release(val);
          } break;
        }
      } break;

      case SEQUENCER: {
        if (serialBuffer[1] == STEP_PRESS) {
          int stepIndex = serialBuffer[2];
          sequencerSteps[stepIndex].on = !sequencerSteps[stepIndex].on;
        } else if (serialBuffer[1] == RESET) {
          for (int i=0; i < ARRAY_LENGTH(sequencerSteps); ++i) {
            sequencerSteps[i].on = 0;
            sequencerSteps[i].freq = 0;
          }
        }
      } break;

      case OSCILLATOR: {
        char msgType = serialBuffer[1];
        short oscIndex = serialBuffer[2];
        if (msgType == FREQUENCY) {
            freq = *((float*)(serialBuffer+3));
            oscillators[oscIndex]->frequency(freq);
            //delay(stepLengthMs);
            //envelope1.noteOff();
            sequencerSteps[currentStepIndex].freq = freq;
        } else if (msgType == WAVEFORM) {
          oscillators[oscIndex]->begin(serialBuffer[3]);
        }
      } break;

      case LFO: {
        char msgType = serialBuffer[1];
        short oscIndex = serialBuffer[2];
        float val = *((float*)(serialBuffer+3));
        // TODO: crazy option - take serialBuffer[3] as value without casting to float
        if (msgType == FREQUENCY) {
            lfo[oscIndex]->frequency(val);
            //granular1.beginPitchShift(val);
        } else if (msgType == WAVEFORM) {
          lfo[oscIndex]->begin(serialBuffer[3]);
        } else if (msgType == AMPLITUDE) {
          lfo[oscIndex]->amplitude(val);
        }
      } break;
    }
  }
}

void loop() {
  
  for (int i = 0; i < 2; ++i) {
    handleSerialInput(currentStepIndex);
  }

  currentTime = millis();
  //dt = (currentTime - previousFrameTime);// / 1000.0;
  //previousFrameTime = currentTime;
  
  // step sequencer
  if (currentTime - previousTime >= stepTime) {
    
    if (sequencerSteps[currentStepIndex].on == true) {
      oscillators[0]->frequency(sequencerSteps[currentStepIndex].freq);
      envelope1.noteOn();
      delay(stepLengthMs); // TODO: make timers
      envelope1.noteOff();

      if (currentStepIndex == 0 || currentStepIndex == 8) playSdWav1.play("KICK.WAV");
    }

    currentStepIndex++;
    if (currentStepIndex == ARRAY_LENGTH(sequencerSteps)) {
      currentStepIndex = 0;
    }
    
    previousTime = currentTime;
  }
  
  delay(5);
}

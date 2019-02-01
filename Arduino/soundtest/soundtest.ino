#include <Audio.h>
#include <Wire.h>
#include <SPI.h>
#include <SD.h>
#include <SerialFlash.h>

// GUItool: begin automatically generated code
AudioSynthWaveformModulated LFO2; //xy=55,446
AudioSynthWaveformModulated LFO1;           //xy=71.99997329711914,270.00000381469727
AudioSynthNoiseWhite     noise2; //xy=190.60003662109375,505.0000009536743
AudioSynthWaveformModulated osc3; //xy=194.00003623962402,427.0000009536743
AudioSynthWaveformModulated osc4; //xy=198.60002899169922,466
AudioSynthNoiseWhite     noise1;         //xy=207.6000099182129,329.0000047683716
AudioSynthWaveformModulated osc1;          //xy=211.00000953674316,251.00000476837158
AudioSynthWaveformModulated osc2;   //xy=215.60000228881836,290.00000381469727
AudioMixer4              mixer3; //xy=351.6000270843506,376.99999618530273
AudioMixer4              mixer2;         //xy=368.6000003814697,201
AudioEffectEnvelope      envelope2; //xy=485.0000534057617,381.0000057220459
AudioEffectEnvelope      envelope1;      //xy=500.00000762939453,214.00000286102295
AudioSynthSimpleDrum     drum1;          //xy=618.6000480651855,588.0000095367432
AudioSynthKarplusStrong  string1;        //xy=619.6000442504883,554.0000076293945
AudioEffectGranular      granular1;      //xy=643.9999828338623,221.0000286102295
AudioMixer4              mixer1;         //xy=810.0000114440918,584.0000076293945
AudioOutputUSB           usb1;           //xy=976.0000648498535,565.0000114440918
AudioOutputI2S           i2s1;           //xy=984.0000648498535,612.0000190734863
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
AudioConnection          patchCord15(drum1, 0, mixer1, 2);
AudioConnection          patchCord16(string1, 0, mixer1, 1);
AudioConnection          patchCord17(granular1, 0, mixer1, 0);
AudioConnection          patchCord18(mixer1, 0, i2s1, 1);
AudioConnection          patchCord19(mixer1, 0, i2s1, 0);
AudioConnection          patchCord20(mixer1, 0, usb1, 1);
AudioConnection          patchCord21(mixer1, 0, usb1, 0);
AudioControlSGTL5000     sgtl5000_1;     //xy=100.00000762939453,583.0000057220459
// GUItool: end automatically generated code




#define ARRAY_LENGTH(arr) (sizeof(arr) / sizeof(*arr))

float freq = 500;
char serialBuffer[8];
float stepTime;
float previousTime, currentTime;
float dt, previousFrameTime;
int stepLengthMs = 100;
short currentStepIndex = 0;
bool liveMode = true;
short currentOscIndex = 0;

int16_t  grains[2048];

AudioSynthWaveformModulated* oscillators[4];
AudioSynthWaveformModulated* lfo[4];

struct SequencerStep {
  bool on;
  float freq;
};
SequencerStep sequencerSteps[2][16];

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);

  oscillators[0] = &osc1;
  oscillators[1] = &osc2;
  lfo[0] = &LFO1;

  LFO1.begin(0);
  LFO1.frequency(0);
  LFO1.amplitude(0.0);
  
  osc1.begin(0);
  osc1.frequency(freq);
  osc1.amplitude(0.4);

  //noise1.amplitude(0.6);

  osc2.begin(0);
  osc2.frequency(freq);
  osc2.amplitude(0.4);

  stepTime = 24.7;
  previousTime = 0;

  for (int i=0; i < ARRAY_LENGTH(sequencerSteps); ++i) {
    for (int j=0; j < ARRAY_LENGTH(sequencerSteps[0]); ++j) {
      sequencerSteps[i][j].on = 0;
      sequencerSteps[i][j].freq = freq;
    }
  }

  //SD.begin(BUILTIN_SDCARD);

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
          int seqIndex = serialBuffer[3];
          sequencerSteps[seqIndex][stepIndex].on = !sequencerSteps[seqIndex][stepIndex].on;
        } else if (serialBuffer[1] == RESET) {
          for (int i=0; i < ARRAY_LENGTH(sequencerSteps); ++i) {
            for (int j=0; j < ARRAY_LENGTH(sequencerSteps[0]); ++j) {
              sequencerSteps[i][j].on = 0;
              sequencerSteps[i][j].freq = 0;
            }
          }
        }
      } break;

      case OSCILLATOR: {
        char msgType = serialBuffer[1];
        currentOscIndex = serialBuffer[2];
        if (msgType == FREQUENCY) {
            freq = *((float*)(serialBuffer+3));
            oscillators[currentOscIndex]->frequency(freq);
            //delay(stepLengthMs);
            //envelope1.noteOff();
            sequencerSteps[currentOscIndex][currentStepIndex].freq = freq;
        } else if (msgType == WAVEFORM) {
          oscillators[currentOscIndex]->begin(serialBuffer[3]);
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
  
  for (int i = 0; i < 2; ++i) {
    handleSerialInput(currentStepIndex);
  }

  currentTime = millis();
  //dt = (currentTime - previousFrameTime);// / 1000.0;
  //previousFrameTime = currentTime;
  
  // step sequencer
  if (liveMode == true && (currentTime - previousTime >= stepTime)) {

    for (int i=0; i < ARRAY_LENGTH(sequencerSteps); ++i) {
      if (sequencerSteps[i][currentStepIndex].on == true) {
        oscillators[i]->frequency(sequencerSteps[i][currentStepIndex].freq);
        envelope1.noteOn();
        //string1.noteOn(freq, 0.5);
        delay(stepLengthMs); // TODO: make timers
        //string1.noteOff(0.5);
        envelope1.noteOff();

        //if (currentStepIndex == 0 || currentStepIndex == 8) playSdWav1.play("KICK.WAV");
      }
    }

    currentStepIndex++;
    if (currentStepIndex == ARRAY_LENGTH(sequencerSteps[0])) {
      currentStepIndex = 0;
    }
    
    previousTime = currentTime;
  }
  
  delay(5);
}

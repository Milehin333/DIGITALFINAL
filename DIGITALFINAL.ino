#include "BetterButton.h"

#include <Audio.h>
#include <Wire.h>
#include <SPI.h>
#include <SD.h>
#include <SerialFlash.h>

#include <synth_simple_drum.h>

#include <Audio.h>
#include <Wire.h>
#include <SPI.h>
#include <SD.h>
#include <SerialFlash.h>

// GUItool: begin automatically generated code
AudioSynthSimpleDrum drum2;  //xy=399,244
AudioSynthSimpleDrum drum3;  //xy=424,310
AudioSynthSimpleDrum drum1;  //xy=431,197
AudioSynthSimpleDrum drum4;  //xy=464,374
AudioMixer4 mixer1;          //xy=737,265
AudioOutputI2S i2s1;         //xy=979,214
AudioConnection patchCord1(drum2, 0, mixer1, 1);
AudioConnection patchCord2(drum3, 0, mixer1, 2);
AudioConnection patchCord3(drum1, 0, mixer1, 0);
AudioConnection patchCord4(drum4, 0, mixer1, 3);
AudioConnection patchCord5(mixer1, 0, i2s1, 0);
AudioConnection patchCord6(mixer1, 0, i2s1, 1);
AudioControlSGTL5000 sgtl5000_1;  //xy=930,518
// GUItool: end automatically generated code

int sequenceLED[8] = {14, 13, 41, 40, 39, 37, 36, 35};
int channelLED[4] = { 0, 1, 2, 3 };

AudioSynthSimpleDrum drums[4] = { drum1, drum2, drum3, drum4 };

// BetterButton button1(4, 0);
// BetterButton button2(5, 1);
// BetterButton button3(6, 2);
// BetterButton button4(9, 3);
// BetterButton button5(10, 4);
// BetterButton button6(11, 5);
// BetterButton button7(12, 6);
// BetterButton button8(24, 7);
// BetterButton button9(25, 8);
// BetterButton button10(26, 9);
// BetterButton button11(27, 10);
// BetterButton button12(28, 11);
// BetterButton button13(29, 12);
// BetterButton button14(30, 13);
// BetterButton button15(31, 14);
// BetterButton button16(32, 15);


BetterButton button1(25, 0);
BetterButton button2(26, 1);
BetterButton button3(27, 2);
BetterButton button4(28, 3);
BetterButton button5(29, 4);
BetterButton button6(30, 5);
BetterButton button7(31, 6);
BetterButton button8(32, 7);


BetterButton channelButton(33, 8);

bool usingMidi = true;

bool buttonOn[4][8] = {  // [channel][sequencenum]
  { LOW, LOW, LOW, LOW, LOW, LOW, LOW, LOW },
  { LOW, LOW, LOW, LOW, LOW, LOW, LOW, LOW },
  { LOW, LOW, LOW, LOW, LOW, LOW, LOW, LOW },
  { LOW, LOW, LOW, LOW, LOW, LOW, LOW, LOW }
};

// bool buttonOn[4][16] = {  // [channel][sequencenum]
//   { LOW, LOW, LOW, LOW, LOW, LOW, LOW, LOW, LOW, LOW, LOW, LOW, LOW, LOW, LOW, LOW },
//   { LOW, LOW, LOW, LOW, LOW, LOW, LOW, LOW, LOW, LOW, LOW, LOW, LOW, LOW, LOW, LOW },
//   { LOW, LOW, LOW, LOW, LOW, LOW, LOW, LOW, LOW, LOW, LOW, LOW, LOW, LOW, LOW, LOW },
//   { LOW, LOW, LOW, LOW, LOW, LOW, LOW, LOW, LOW, LOW, LOW, LOW, LOW, LOW, LOW, LOW }
// };

int pot = A14;

int tempo = 0;

int channelDisplayed = 0;

unsigned long lastStepTime = 0;
int stepLength = 1000;
int currentStep = 0;
int totalSteps = 8;

int midiNotes[4] = { 36, 38, 42, 44 };
int midiSwitch = 34;

void setup() {
  // put your setup code here, to run once:

  Serial.begin(9600);

  // audio library init
  AudioMemory(15);



  AudioNoInterrupts();

  drum1.frequency(60);
  drum1.length(1500);
  drum1.secondMix(0.0);
  drum1.pitchMod(0.55);

  drum2.frequency(60);
  drum2.length(300);
  drum2.secondMix(0.0);
  drum2.pitchMod(1.0);

  drum3.frequency(550);
  drum3.length(400);
  drum3.secondMix(1.0);
  drum3.pitchMod(0.5);

  drum4.frequency(1200);
  drum4.length(150);
  drum4.secondMix(0.0);
  drum4.pitchMod(0.0);

  sgtl5000_1.enable();
  sgtl5000_1.volume(0.5);

  AudioInterrupts();

  for (int i = 0; i < 8; i++) {
    pinMode(sequenceLED[i], OUTPUT);
  }
  for (int i = 0; i < 4; i++) {
    pinMode(channelLED[i], OUTPUT);
  }

  pinMode(pot, INPUT);

  pinMode(midiSwitch, INPUT);

  button1.pressHandler(onPress);
  button2.pressHandler(onPress);
  button3.pressHandler(onPress);
  button4.pressHandler(onPress);
  button5.pressHandler(onPress);
  button6.pressHandler(onPress);
  button7.pressHandler(onPress);
  button8.pressHandler(onPress);
  // button9.pressHandler(onPress);
  // button10.pressHandler(onPress);
  // button11.pressHandler(onPress);
  // button12.pressHandler(onPress);
  // button13.pressHandler(onPress);
  // button14.pressHandler(onPress);
  // button15.pressHandler(onPress);
  // button16.pressHandler(onPress);

  channelButton.pressHandler(channelPress);
}


void loop() {
  // put your main code here, to run repeatedly:

  if (digitalRead(midiSwitch) == HIGH) {
    usingMidi = true;
    Serial.println("midi");
  } else {
    usingMidi = false;
  }

  button1.process();
  button2.process();
  button3.process();
  button4.process();
  button5.process();
  button6.process();
  button7.process();
  button8.process();
  // button9.process();
  // button10.process();
  // button11.process();
  // button12.process();
  // button13.process();
  // button14.process();
  // button15.process();
  // button16.process();

  channelButton.process();

  checkLed();
  sequence();
  checkChannelLeds();
}

void checkLed() {
  for (int i = 0; i < 8; i++) {
    if (i == currentStep) {
      digitalWrite(sequenceLED[i], 255);
    } else if (buttonOn[channelDisplayed][i] == true) {
      digitalWrite(sequenceLED[i], 80);
    } else {
      digitalWrite(sequenceLED[i], 0);
    }
  }
}

void nextStep() {
  currentStep = currentStep + 1;
  if (currentStep >= totalSteps) {
    currentStep = 0;
  }
}

void sequence() {

  tempo = analogRead(pot);
  if (millis() > lastStepTime + tempo) {
    lastStepTime = millis();

    //check to see if midi or audio shield
    if (usingMidi == true) {
      for (int i = 0; i < 4; i++) {
        usbMIDI.sendNoteOff(midiNotes[i], 0, 1);
      }
      nextStep();

      for (int i = 0; i < 4; i++) {
        if (buttonOn[i][currentStep] == HIGH) {
          usbMIDI.sendNoteOn(midiNotes[i], 127, 1);
        }
      }
    } else {
      nextStep();

      if (buttonOn[0][currentStep] == HIGH) {
        drum1.noteOn();
      }
      if (buttonOn[1][currentStep] == HIGH) {
        drum2.noteOn();
      }
      if (buttonOn[2][currentStep] == HIGH) {
        drum3.noteOn();
      }
      if (buttonOn[3][currentStep] == HIGH) {
        drum4.noteOn();
      }
    }
  }
}


void checkChannelLeds() {
  for (int i = 0; i < 4; i++) {
    if (channelDisplayed == i) {
      digitalWrite(channelLED[i], HIGH);
    } else {
      digitalWrite(channelLED[i], LOW);
    }
  }
}


void onPress(int val) {
  Serial.print(val);
  buttonOn[channelDisplayed][val] = !buttonOn[channelDisplayed][val];
  Serial.println(" on");
}

void channelPress(int val) {
  channelDisplayed++;
  if (channelDisplayed > 3) {
    channelDisplayed = 0;
  }
}
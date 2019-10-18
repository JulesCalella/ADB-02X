/* 
 * File:   dsPIC33CH512MP506_Audio.h
 * Author: Jules
 *
 * Created on July 2, 2019, 12:34 AM
 */

#ifndef DSPIC33CH512MP506_AUDIO_H
#define	DSPIC33CH512MP506_AUDIO_H

#define PI 3.1415927
#define SAMPLING_FREQUENCY 11719 //15625 //7813 //46875
#define AMPLITUDE_MULTIPLIER 10

#define NUM_NOTES 5
#define NOTE_ARRAY_SIZE 10
#define ARRAY_SIZE_X 70
#define ARRAY_SIZE_Y 100
#define ARRAY_OUTPUT_BUFFER 10
#define AUDIO_OFFSET 0
#define DURATION_MAX 300000

#define NUM_CTRL 22
#define CTRL_OVERTONE1 0
#define CTRL_OVERTONE2 1
#define CTRL_OVERTONE3 2
#define CTRL_OVERTONE4 3
#define CTRL_OVERTONE5 4
#define CTRL_OVERTONE6 5
#define CTRL_OVERTONE7 6
#define CTRL_OVERTONE8 7
#define CTRL_OVERTONE9 8
#define CTRL_SINE_AMP 9
#define CTRL_SQUARE_AMP 10 
#define CTRL_TRIANGLE_AMP 11
#define CTRL_SAWTOOTH_AMP 12
#define CTRL_BIT_AMP 13
#define CTRL_FN1 14
#define CTRL_FN2 15
#define CTRL_FN3 16
#define CTRL_TEMPO 17
#define CTRL_ATTACK_MULT 18
#define CTRL_ATTACK_DUR 19
#define CTRL_DECAY_DUR 20
#define CTRL_RELEASE_DUR 21

#define C1_BUFF_SIZE 358 //478 //239
#define CSh1_BUFF_SIZE 338 //451 //225
#define D1_BUFF_SIZE 319 //426 //213
#define DSh1_BUFF_SIZE 301 //402 //201
#define E1_BUFF_SIZE 284 //379 //190
#define F1_BUFF_SIZE 268 //358 //179
#define FSh1_BUFF_SIZE 253 //338 //169
#define G1_BUFF_SIZE 239 //319 //159
#define GSh1_BUFF_SIZE 226 //301 //151
#define A1_BUFF_SIZE 213 //284 //142
#define ASh1_BUFF_SIZE 201 //268 //134
#define B1_BUFF_SIZE 190 //253 //127

#define C 0x00
#define Csh 0x01
#define D 0x02
#define Dsh 0x03
#define E 0x04
#define F 0x05
#define Fsh 0x06
#define G 0x07
#define Gsh 0x08
#define A 0x09
#define Ash 0x0A
#define B 0x0B

typedef struct{
    int noteOn;         // Determine  if the note is on or off
    int measureEnd;     // What measure the note ends
    int locationEnd;    // Where in the measure the note ends (measured 64th notes)
    int triplet;        // True (1), False (0)
    int pitch;          // Multiple of the base waveform array
    int dynamic;        // Sustain volume
    int *waveformArray; // Pointer to the waveform array
    int noteElement;    // Current element in the waveform
    int noteElementMax; // Total number of elements in the waveform array
    
    int volumeCurrent;
    int stateElement;  // Counts how long the note has been in the current state
    int currentState;   // Determine the current state (0: Attack, 1: Decay, 2: Sustain, 3:Release)
    double attackMultiplier;   // Attack maximum volume
    double *attackArray;
    int attackElements;
    double *decayArray;
    int decayElements;
    double *releaseArray;
    int releaseElements;
}noteStruct;

typedef struct{
    int measure;
    int location64;
    int location64Max;
    int location64Triplet;
    int location64TripletMax;
    int tempo;
    int interruptCount; // Couter for the LEDs' PWM
    int loading;
    int loadingCount;
    int loadingDisplay;
    int duration;   // Counter for the notes' states
    int durationMax;
}timingStruct;

void generateAllWaveforms();

void generateWaveform(int *audioArray, int *controlArray, int buffSize);

void generateSineWave(int *audioArray, int amplitude, int numBytes);

void generateSquareWave(int *audioArray, int amplitude, int numBytes);

void generateTriangleWave(int *audioArray, int amplitude, int numBytes);

void generateSawWave(int *audioArray, int amplitude, int numBytes);

void generateBitWave(int *audioArray, int amplitude, int numBytes);

void clearAudioArray(int *audioArray, int numBytes);

void offsetAudioArray(int offset, int *audioArray, int numBytes);

void updateStateArrays();

void defaultWaveformInit();

void playNoteA(int *value, int currentElement);

void readControlArray(int *toReadArray);

void writeControlArray(int *toWriteArray);

void notesInit();

void updateOutputBuffer();

void updateOutputBufferOriginal(int *newOutput);

void readScoreArray();

void updateNote(noteStruct *note, int isTriplet);

void writeSong();

void setScoreArray(int x, int y);

void rewind();

void rewindToBeginning();

void fastForward();

//void updateTimer(timingStruct);
void linkTimer(timingStruct *timer);

void loading(int isLoading);

void updateTempo(int newTempo);

int exponentOfTwo(int exp);

#endif	/* DSPIC33CH512MP506_AUDIO_H */


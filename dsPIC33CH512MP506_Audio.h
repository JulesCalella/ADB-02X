/* 
 * File:   dsPIC33CH512MP506_Audio.h
 * Author: Jules
 *
 * Created on July 2, 2019, 12:34 AM
 */

#ifndef DSPIC33CH512MP506_AUDIO_H
#define	DSPIC33CH512MP506_AUDIO_H

#define PI 3.1415927
#define SAMPLING_FREQUENCY 46875
#define AMPLITUDE_MULTIPLIER 100

#define NUM_NOTES 10
#define NOTE_ARRAY_SIZE 10
#define ARRAY_SIZE_X 70
#define ARRAY_SIZE_Y 100
#define ARRAY_OUTPUT_BUFFER 100

#define C1_BUFF_SIZE 717
#define CSh1_BUFF_SIZE 676
#define D1_BUFF_SIZE 638
#define DSh1_BUFF_SIZE 603
#define E1_BUFF_SIZE 569
#define F1_BUFF_SIZE 537
#define FSh1_BUFF_SIZE 507
#define G1_BUFF_SIZE 478
#define GSh1_BUFF_SIZE 452
#define A1_BUFF_SIZE 426
#define ASh1_BUFF_SIZE 402
#define B1_BUFF_SIZE 380

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
    int noteOn;
    int measureEnd;
    int locationEnd;
    int triplet;    // True (1), False (0)
    int pitch;
    int dynamic;
    int *noteArray;
    int noteElement;
    int noteElementMax;
    int attackOn;
    int attackElement;
    int releaseOn;
    int releaseElement;
}noteStruct;

typedef struct{
    int measure;
    int location64;
    int location64Max;
    int location64Triplet;
    int location64TripletMax;
    int tempo;
    int interruptCount;
}timingStruct;

void generateAllWaveforms();

void generateWaveform(int *audioArray, int *controlArray, int buffSize);

void generateSineWave(int *audioArray, int amplitude, int numBytes);

void generateSquareWave(int *audioArray, int amplitude, int numBytes);

void generateTriangleWave(int *audioArray, int amplitude, int numBytes);

void generateSawWave(int *audioArray, int amplitude, int numBytes);

void clearAudioArray(int *audioArray, int numBytes);

void offsetAudioArray(int offset, int *audioArray, int numBytes);

void defaultWaveformInit();

void playNoteA(int *value, int currentElement);

void readControlArray(int *toReadArray);

void writeControlArray(int *toWriteArray);

void notesInit();

void updateOutputBuffer();

void readScoreArray();

int updateNote();

void writeSong();

void setScoreArray(int x, int y);

void updateTimer(timingStruct);

#endif	/* DSPIC33CH512MP506_AUDIO_H */


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

#endif	/* DSPIC33CH512MP506_AUDIO_H */


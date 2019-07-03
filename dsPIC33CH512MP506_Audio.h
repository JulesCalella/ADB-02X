/* 
 * File:   dsPIC33CH512MP506_Audio.h
 * Author: Jules
 *
 * Created on July 2, 2019, 12:34 AM
 */

#ifndef DSPIC33CH512MP506_AUDIO_H
#define	DSPIC33CH512MP506_AUDIO_H

#define PI 3.1415927
#define SAMPLING_FREQUENCY 20000
#define AMPLITUDE_MULTIPLIER 100

#define C1_BUFF_SIZE 612
#define CSh1_BUFF_SIZE 577
#define D1_BUFF_SIZE 545
#define DSh1_BUFF_SIZE 514
#define E1_BUFF_SIZE 485
#define F1_BUFF_SIZE 458
#define FSh1_BUFF_SIZE 432
#define G1_BUFF_SIZE 408
#define GSh1_BUFF_SIZE 385
#define A1_BUFF_SIZE 364
#define ASh1_BUFF_SIZE 343
#define B1_BUFF_SIZE 324

int c1Waveform[C1_BUFF_SIZE];
int cSharp1Waveform[CSh1_BUFF_SIZE];
int d1Waveform[D1_BUFF_SIZE];
int dSharp1Waveform[DSh1_BUFF_SIZE];
int e1Waveform[E1_BUFF_SIZE];
int f1Waveform[F1_BUFF_SIZE];
int fSharp1Waveform[FSh1_BUFF_SIZE];
int g1Waveform[G1_BUFF_SIZE];
int gSharp1Waveform[GSh1_BUFF_SIZE];
int a1Waveform[A1_BUFF_SIZE];
int aSharp1Waveform[ASh1_BUFF_SIZE];
int b1Waveform[B1_BUFF_SIZE];
int controlArray[13];


void generateAllWaveforms();

void generateWaveform(int *audioArray, int *controlArray, int buffSize);

void generateSineWave(int *audioArray, int *controlArray, int numBytes);

void generateSquareWave(int *audioArray, int *controlArray, int numBytes);

void generateTriangleWave(int *audioArray, int *controlArray, int numBytes);

void generateSawWave(int *audioArray, int *controlArray, int numBytes);

void clearAudioArray(int *audioArray, int numBytes);

void offsetAudioArray(int offset, int *audioArray, int numBytes);


#endif	/* DSPIC33CH512MP506_AUDIO_H */


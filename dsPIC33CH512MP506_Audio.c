#include <xc.h>
#include <math.h>
#include "dsPIC33CH512MP506_Audio.h"

/* controlArray Values:
 * 0 - Overtone 1 Amplitude
 * 1 - Overtone 2 Amplitude
 * 2 - Overtone 3 Amplitude
 * 3 - Overtone 4 Amplitude
 * 4 - Overtone 5 Amplitude
 * 5 - Overtone 6 Amplitude
 * 6 - Overtone 7 Amplitude
 * 7 - Overtone 8 Amplitude
 * 8 - Overtone 9 Amplitude
 * 9 - Sine Wave Amplitude
 * 10 - Square Wave Amplitude
 * 11 - Triangle Wave Amplitude
 * 12 - Sawtooth Wave Amplitude
 * 13 - Attack Value
 * 14 - Release Value
 * 15 - Function 1
 * 16 - Function 2
 * 17 - Function 3
 * */

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
int controlArray[18];

/*
 * 
 */
void generateAllWaveforms()
{
    generateWaveform(c1Waveform, controlArray, C1_BUFF_SIZE);
    generateWaveform(cSharp1Waveform, controlArray, CSh1_BUFF_SIZE);
    generateWaveform(d1Waveform, controlArray, D1_BUFF_SIZE);
    generateWaveform(dSharp1Waveform, controlArray, DSh1_BUFF_SIZE);
    generateWaveform(e1Waveform, controlArray, E1_BUFF_SIZE);
    generateWaveform(f1Waveform, controlArray, F1_BUFF_SIZE);
    generateWaveform(fSharp1Waveform, controlArray, FSh1_BUFF_SIZE);
    generateWaveform(g1Waveform, controlArray, G1_BUFF_SIZE);
    generateWaveform(gSharp1Waveform, controlArray, GSh1_BUFF_SIZE);
    generateWaveform(a1Waveform, controlArray, A1_BUFF_SIZE);
    generateWaveform(aSharp1Waveform, controlArray, ASh1_BUFF_SIZE);
    generateWaveform(b1Waveform, controlArray, B1_BUFF_SIZE);
}

/*
 * 
 */
void generateWaveform(int *audioArray, int *controlArray, int buffSize)
{
    clearAudioArray(audioArray, buffSize);
    generateSineWave(audioArray, controlArray[9], buffSize);
    //generateSquareWave(audioArray, controlArray[10], buffSize);
    //generateTriangleWave(audioArray, controlArray[11], buffSize);
    //generateSawWave(audioArray, controlArray[12], buffSize);
    
    offsetAudioArray(0x1FFF, audioArray, buffSize);
}

/*
 * 
 */
void generateSineWave(int *audioArray, int amplitude, int numBytes)
{
    int i, overtoneNum;
    float overtoneAmplitude;
    
    for(i=0; i<numBytes; i++){
        audioArray[i] += (AMPLITUDE_MULTIPLIER* (amplitude * sin(2.0 * PI * (i/(numBytes * 1.0)))));
    }
    
    // Overtones
    for(overtoneNum=0; overtoneNum<=8; overtoneNum++){
        if(controlArray[overtoneNum] != 0){
            overtoneAmplitude = controlArray[overtoneNum]/amplitude;
            for(i=0; i<numBytes; i++){
                audioArray[i] += (AMPLITUDE_MULTIPLIER* (overtoneAmplitude * sin(2.0 * PI * (i/(numBytes * 1.0)))));
            }
        }
    }
}

/*
 * 
 */
void generateSquareWave(int *audioArray, int amplitude, int numBytes)
{
    int i, overtoneNum;
    float overtoneAmplitude;
    int numBytesDivTwo = numBytes/2;
    
    for(i=0; i<numBytes; i++){
        if(i<numBytesDivTwo){
            audioArray[i] += (AMPLITUDE_MULTIPLIER * amplitude);
        } else {
            audioArray[i] += (AMPLITUDE_MULTIPLIER * amplitude * -1);
        }
    }
    
    // Overtones
    for(overtoneNum=0; overtoneNum<=8; overtoneNum++){
        if(controlArray[overtoneNum] != 0){
            overtoneAmplitude = controlArray[overtoneNum]/amplitude;
            
            for(i=0; i<numBytes; i++){
                if(i<numBytesDivTwo){
                    audioArray[i] += (AMPLITUDE_MULTIPLIER * overtoneAmplitude);
                } else {
                    audioArray[i] += (AMPLITUDE_MULTIPLIER * overtoneAmplitude * -1);
                }
            }
        }
    }
}

/*
 * 
 */
void generateTriangleWave(int *audioArray, int amplitude, int numBytes)
{
    int i, overtoneNum;
    float overtoneAmplitude;
    int firstLoopMax = numBytes/4;
    int secondLoopMax = (3*numBytes)/4;
    int bytesDivTwo = numBytes/2;
    
    for(i=0; i<firstLoopMax; i++){
        audioArray[i] += AMPLITUDE_MULTIPLIER * amplitude * (i/(firstLoopMax * 1.0));
    }
    
    for( ; i<secondLoopMax; i++){
        audioArray[i] += AMPLITUDE_MULTIPLIER * amplitude * ((i-firstLoopMax)/(-bytesDivTwo * 1.0)) + 1;
    }
    
    for( ; i<numBytes; i++){
        audioArray[i]  += AMPLITUDE_MULTIPLIER * amplitude * ((i-firstLoopMax)/(firstLoopMax * 1.0)) - 1;
    }
    
    // Overtones
    for(overtoneNum=0; overtoneNum<=8; overtoneNum++){
        if(controlArray[overtoneNum] != 0){
            overtoneAmplitude = controlArray[overtoneNum]/amplitude;
            for(i=0; i<numBytes; i++){
                for(i=0; i<firstLoopMax; i++){
                    audioArray[i] += AMPLITUDE_MULTIPLIER * overtoneAmplitude * (i/(firstLoopMax * 1.0));
                }

                for( ; i<secondLoopMax; i++){
                    audioArray[i] += AMPLITUDE_MULTIPLIER * overtoneAmplitude * ((i-firstLoopMax)/(-bytesDivTwo * 1.0)) + 1;
                }

                for( ; i<numBytes; i++){
                    audioArray[i]  += AMPLITUDE_MULTIPLIER * overtoneAmplitude * ((i-firstLoopMax)/(firstLoopMax * 1.0)) - 1;
                }
            }
        }
    }
}

/*
 * 
 */
void generateSawWave(int *audioArray, int amplitude, int numBytes)
{
    int i, overtoneNum;
    float overtoneAmplitude;
    
    for(i=0; i<numBytes; i++){
        audioArray[i] += AMPLITUDE_MULTIPLIER * amplitude * (((2.0 * i)/numBytes) - 1);
    }
    
    // Overtones
    for(overtoneNum=0; overtoneNum<=8; overtoneNum++){
        if(controlArray[overtoneNum] != 0){
            overtoneAmplitude = controlArray[overtoneNum]/amplitude;
            for(i=0; i<numBytes; i++){
                for(i=0; i<numBytes; i++){
                    audioArray[i] += AMPLITUDE_MULTIPLIER * overtoneAmplitude * (((2.0 * i)/numBytes) - 1);
                }                
            }
        }
    }
}

/*
 * 
 */
void clearAudioArray(int *audioArray, int numBytes)
{
    int i;
    
    for(i=0; i<numBytes; i++){
        audioArray[i] = 0;
    }
}

/*
 * 
 */
void offsetAudioArray(int offset, int *audioArray, int numBytes)
{
    int i;
    
    for(i=0; i<numBytes; i++){
        audioArray[i] += offset;
    }
}

void defaultWaveformInit()
{
    controlArray[0] = 52;
    controlArray[1] = 52;
    controlArray[2] = 52;
    controlArray[3] = 52;
    controlArray[4] = 52;
    controlArray[5] = 52;
    controlArray[6] = 52;
    controlArray[7] = 52;
    controlArray[8] = 52;
    controlArray[9] = 52;
    controlArray[10] = 52;
    controlArray[11] = 52;
    controlArray[12] = 52;
    controlArray[13] = 52;
    controlArray[14] = 52;
    controlArray[15] = 52;
    controlArray[16] = 52;
    controlArray[17] = 52;
}

void playNoteA(int *value, int currentElement)
{
    *value = a1Waveform[currentElement];
}

void readControlArray(int *toReadArray)
{
    int i;
    for(i=0; i<18; i++){
        toReadArray[i] = controlArray[i];
    }
}

void writeControlArray(int *toWriteArray)
{
    int i;
    for(i=0; i<18; i++){
        controlArray[i] = toWriteArray[i];
    }
}
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
 * */

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
    generateSquareWave(audioArray, controlArray[10], buffSize);
    generateTriangleWave(audioArray, controlArray[11], buffSize);
    generateSawWave(audioArray, controlArray[12], buffSize);
    
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
    
    for(i=0; i<numBytes; i++){
        if(i<(numBytes/2)){
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
                for(i=0; i<numBytes; i++){
                    if(i<(numBytes/2)){
                        audioArray[i] += (AMPLITUDE_MULTIPLIER * overtoneAmplitude);
                    } else {
                        audioArray[i] += (AMPLITUDE_MULTIPLIER * overtoneAmplitude * -1);
                    }
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
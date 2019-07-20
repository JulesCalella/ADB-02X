#include <xc.h>
#include <math.h>
#include "dsPIC33CH512MP506_Audio.h"
#include "dsPIC33CH512MP506_Modules.h"
#include "dsPIC33CH512MP506_Pins.h"

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

int scoreArray1[ARRAY_SIZE_X][ARRAY_SIZE_Y];
int currentArrayX;
int currentArrayY;
int currentArray;
volatile int outputBuffer[ARRAY_OUTPUT_BUFFER];

volatile timingStruct timing;

noteStruct notes[NUM_NOTES];

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
    if(controlArray[9] != 0) generateSineWave(audioArray, controlArray[9], buffSize);
    if(controlArray[10] != 0) generateSquareWave(audioArray, controlArray[10], buffSize);
    if(controlArray[11] != 0) generateTriangleWave(audioArray, controlArray[11], buffSize);
    if(controlArray[12] != 0) generateSawWave(audioArray, controlArray[12], buffSize);
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
void defaultWaveformInit()
{
    controlArray[0] = 100;
    controlArray[1] = 0;
    controlArray[2] = 0;
    controlArray[3] = 0;
    controlArray[4] = 0;
    controlArray[5] = 0;
    controlArray[6] = 0;
    controlArray[7] = 0;
    controlArray[8] = 0;
    controlArray[9] = 100;
    controlArray[10] = 0;
    controlArray[11] = 0;
    controlArray[12] = 0;
    controlArray[13] = 50;
    controlArray[14] = 50;
    controlArray[15] = 0;
    controlArray[16] = 0;
    controlArray[17] = 0;
}

/*
 * 
 */
void readControlArray(int *toReadArray)
{
    int i;
    for(i=0; i<18; i++){
        toReadArray[i] = controlArray[i];
    }
}

/*
 * 
 */
void writeControlArray(int *toWriteArray)
{
    int i;
    for(i=0; i<18; i++){
        controlArray[i] = toWriteArray[i];
    }
}

/*
 * 
 */
void notesInit(int *newOutput)
{
    int currentNote;
    
    for(currentNote=0; currentNote<NUM_NOTES; currentNote++){
        notes[currentNote].noteOn = 0;
        notes[currentNote].noteElement = 0;
        notes[currentNote].attackElement = 0;
        notes[currentNote].releaseElement = 0;
    }
    
    currentArrayX = 0;
    currentArrayY = 0;
    currentArray = 1;
    *newOutput = 1;
}

/*
 * 
 */
void updateOutputBuffer(int newOutput)
{
    int output, element, noteNum;
    
    if(newOutput == 1){
        newOutput = 0;
        output = 0;
        
        for(noteNum=0; noteNum<NUM_NOTES; noteNum++){
            if(notes[noteNum].noteOn == 1){
                // Write to output
                output += notes[noteNum].noteArray[element];
                element = notes[noteNum].noteElement + notes[noteNum].pitch;
                if(element >= notes[noteNum].noteElementMax){
                    notes[noteNum].noteElement = element - notes[noteNum].noteElementMax;
                }
                notes[noteNum].noteElement = element;
                
                // Check if note is over
                if(notes[noteNum].measureEnd == timing.measure){
                    if((notes[noteNum].locationEnd == timing.location64)&&(notes[noteNum].triplet == 1)){
                        notes[noteNum].noteOn = 0;
                    } else if((notes[noteNum].locationEnd == timing.location64Triplet)&&(notes[noteNum].triplet == 0)){
                        notes[noteNum].noteOn = 0;
                    }
                }
            }
        }
     
        output += 0x1FFF; // Offset
        
        spi1Write(output);
    }
}

/*
 * 
 */
void readScoreArray()
{
    int scoreMeasure, location, check;
    int ret = 1;
    
    scoreMeasure = (scoreArray1[currentArrayX+2][currentArrayY] << 8) | scoreArray1[currentArrayX+3][currentArrayY];
    location = scoreArray1[currentArrayX+4][currentArrayY];
    
    if(scoreMeasure == timing.measure){
        if((location & 0x3F) == timing.location64){
            for(check=0; check<NUM_NOTES; check++){
                if(notes[check].noteOn == 0){
                    ret = updateNote(notes[check], 0);
                    break;
                }
            }   
        } else if(location == (timing.location64Triplet | 0x40)){
            for(check=0; check<NUM_NOTES; check++){
                if(notes[check].noteOn == 0){
                    ret = updateNote(notes[check], 1);
                    break;
                }
            }
        }
        
        if(ret == 0){
            currentArrayX += NOTE_ARRAY_SIZE;
            if(currentArrayX >= ARRAY_SIZE_X){
                currentArrayX = 0;
                currentArrayY++;
                if(currentArrayY >= ARRAY_SIZE_Y) currentArrayY = 0;
            }
        }
    }
}

/*
 * 
 */
// Returns 1 if no notes are available
int updateNote(noteStruct note, int isTriplet)
{
    int noteValue;
    
    if(note.noteOn == 0){
        note.noteOn = 1;
        note.noteElement = 0;
        noteValue = scoreArray1[currentArrayX][currentArrayY];
        note.pitch = scoreArray1[currentArrayX+1][currentArrayY];
        note.measureEnd = ((scoreArray1[currentArrayX+5][currentArrayY] << 8) | scoreArray1[currentArrayX+6][currentArrayY]);
        note.locationEnd = scoreArray1[currentArrayX+7][currentArrayY];
        if(isTriplet){ 
            note.triplet = 1;
        } else {
            note.triplet = 0;
        }
        
        switch(noteValue){
            case C:
                note.noteArray = c1Waveform;
                note.noteElementMax = C1_BUFF_SIZE;
                break;
                
            case Csh:
                note.noteArray = cSharp1Waveform;
                note.noteElementMax = CSh1_BUFF_SIZE;
                break;
                
            case D:
                note.noteArray = d1Waveform;
                note.noteElementMax = D1_BUFF_SIZE;
                break;
                
            case Dsh:
                note.noteArray = dSharp1Waveform;
                note.noteElementMax = DSh1_BUFF_SIZE;
                break;
                
            case E:
                note.noteArray = e1Waveform;
                note.noteElementMax = E1_BUFF_SIZE;
                break;
                
            case F:
                note.noteArray = f1Waveform;
                note.noteElementMax = F1_BUFF_SIZE;
                break;
                
            case Fsh:
                note.noteArray = fSharp1Waveform;
                note.noteElementMax = FSh1_BUFF_SIZE;
                break;
                
            case G:
                note.noteArray = g1Waveform;
                note.noteElementMax = G1_BUFF_SIZE;
                break;
                
            case Gsh:
                note.noteArray = gSharp1Waveform;
                note.noteElementMax = GSh1_BUFF_SIZE;
                break;
                
            case A:
                note.noteArray = a1Waveform;
                note.noteElementMax = A1_BUFF_SIZE;
                break;
                
            case Ash:
                note.noteArray = aSharp1Waveform;
                note.noteElementMax = ASh1_BUFF_SIZE;
                break;
                
            case B:
                note.noteArray = b1Waveform;
                note.noteElementMax = B1_BUFF_SIZE;
                break;
        }
        
        return 0;
    }
    
    return 1;
}

void setScoreArray(int x, int y)
{
    currentArrayX = x;
    currentArrayY = y;
    timing.measure = 0;
    timing.location64 = 0;
    timing.location64Triplet = 0;
}

void updateTimer(timingStruct timer)
{
    timing.location64 = timer.location64;
    timing.measure = timer.measure;
}

void writeSong(){
    scoreArray1[0][0] = C;
    scoreArray1[1][0] = 3;
    scoreArray1[2][0] = 0;
    scoreArray1[3][0] = 1;
    scoreArray1[4][0] = 0;
    scoreArray1[5][0] = 0;
    scoreArray1[6][0] = 1;
    scoreArray1[7][7] = 16;
    
    scoreArray1[0][0] = C;
    scoreArray1[1][0] = 3;
    scoreArray1[2][0] = 0;
    scoreArray1[3][0] = 1;
    scoreArray1[4][0] = 16;
    scoreArray1[5][0] = 0;
    scoreArray1[6][0] = 1;
    scoreArray1[7][7] = 32;
    
    scoreArray1[0][0] = G;
    scoreArray1[1][0] = 3;
    scoreArray1[2][0] = 0;
    scoreArray1[3][0] = 1;
    scoreArray1[4][0] = 32;
    scoreArray1[5][0] = 0;
    scoreArray1[6][0] = 1;
    scoreArray1[7][7] = 48;
    
    scoreArray1[0][0] = G;
    scoreArray1[1][0] = 3;
    scoreArray1[2][0] = 0;
    scoreArray1[3][0] = 1;
    scoreArray1[4][0] = 48;
    scoreArray1[5][0] = 0;
    scoreArray1[6][0] = 2;
    scoreArray1[7][7] = 0;
    
    scoreArray1[0][0] = A;
    scoreArray1[1][0] = 3;
    scoreArray1[2][0] = 0;
    scoreArray1[3][0] = 2;
    scoreArray1[4][0] = 0;
    scoreArray1[5][0] = 0;
    scoreArray1[6][0] = 2;
    scoreArray1[7][7] = 16;
    
    scoreArray1[0][0] = A;
    scoreArray1[1][0] = 3;
    scoreArray1[2][0] = 0;
    scoreArray1[3][0] = 2;
    scoreArray1[4][0] = 16;
    scoreArray1[5][0] = 0;
    scoreArray1[6][0] = 2;
    scoreArray1[7][7] = 32;
    
    scoreArray1[0][0] = G;
    scoreArray1[1][0] = 3;
    scoreArray1[2][0] = 0;
    scoreArray1[3][0] = 2;
    scoreArray1[4][0] = 32;
    scoreArray1[5][0] = 0;
    scoreArray1[6][0] = 3;
    scoreArray1[7][7] = 0;
    
    scoreArray1[0][0] = F;
    scoreArray1[1][0] = 3;
    scoreArray1[2][0] = 0;
    scoreArray1[3][0] = 3;
    scoreArray1[4][0] = 0;
    scoreArray1[5][0] = 0;
    scoreArray1[6][0] = 3;
    scoreArray1[7][7] = 16;
    
    scoreArray1[0][0] = F;
    scoreArray1[1][0] = 3;
    scoreArray1[2][0] = 0;
    scoreArray1[3][0] = 3;
    scoreArray1[4][0] = 16;
    scoreArray1[5][0] = 0;
    scoreArray1[6][0] = 3;
    scoreArray1[7][7] = 32;
    
    scoreArray1[0][0] = E;
    scoreArray1[1][0] = 3;
    scoreArray1[2][0] = 0;
    scoreArray1[3][0] = 3;
    scoreArray1[4][0] = 32;
    scoreArray1[5][0] = 0;
    scoreArray1[6][0] = 3;
    scoreArray1[7][7] = 48;
    
    scoreArray1[0][0] = E;
    scoreArray1[1][0] = 3;
    scoreArray1[2][0] = 0;
    scoreArray1[3][0] = 3;
    scoreArray1[4][0] = 48;
    scoreArray1[5][0] = 0;
    scoreArray1[6][0] = 4;
    scoreArray1[7][7] = 0;
    
    scoreArray1[0][0] = D;
    scoreArray1[1][0] = 3;
    scoreArray1[2][0] = 0;
    scoreArray1[3][0] = 4;
    scoreArray1[4][0] = 0;
    scoreArray1[5][0] = 0;
    scoreArray1[6][0] = 4;
    scoreArray1[7][7] = 16;
    
    scoreArray1[0][0] = D;
    scoreArray1[1][0] = 3;
    scoreArray1[2][0] = 0;
    scoreArray1[3][0] = 4;
    scoreArray1[4][0] = 16;
    scoreArray1[5][0] = 0;
    scoreArray1[6][0] = 4;
    scoreArray1[7][7] = 32;
    
    scoreArray1[0][0] = C;
    scoreArray1[1][0] = 3;
    scoreArray1[2][0] = 0;
    scoreArray1[3][0] = 4;
    scoreArray1[4][0] = 32;
    scoreArray1[5][0] = 0;
    scoreArray1[6][0] = 5;
    scoreArray1[7][7] = 0;
    
    
}
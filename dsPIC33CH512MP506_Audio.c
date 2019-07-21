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
int emptyArray[1];
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
    emptyArray[0] = 0;
}

/*
 * 
 */
void generateWaveform(int *audioArray, int *controlArray, int buffSize)
{
    clearAudioArray(audioArray, buffSize);
    if(controlArray[9] != 0)  generateSineWave(audioArray, controlArray[9], buffSize);
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
    controlArray[0] = 10;
    controlArray[1] = 0;
    controlArray[2] = 0;
    controlArray[3] = 0;
    controlArray[4] = 0;
    controlArray[5] = 0;
    controlArray[6] = 0;
    controlArray[7] = 0;
    controlArray[8] = 0;
    controlArray[9] = 100;
    controlArray[10] = 50;
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
        notes[currentNote].noteElementMax = 0;
        notes[currentNote].measureEnd = 0;
        notes[currentNote].locationEnd = 0;
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
void updateOutputBuffer(int *newOutput)
{
    int noteNum, noteEnded;
    
    if(*newOutput != 0){
        *newOutput = 0;
        int output = 0;
        
        for(noteNum=0; noteNum<NUM_NOTES; noteNum++){
            if(notes[noteNum].noteOn == 1){
                noteEnded = 0;
                // Check if note is over
                if(notes[noteNum].measureEnd <= timing.measure){
                    if((notes[noteNum].locationEnd <= timing.location64) && (notes[noteNum].triplet == 0)){
                        notes[noteNum].noteOn = 0;
                        noteEnded = 1;
                    } else if((notes[noteNum].locationEnd <= timing.location64Triplet)&&(notes[noteNum].triplet == 1)){
                        notes[noteNum].noteOn = 0;
                        noteEnded = 1;
                    }
                }
                
                if(noteEnded == 0){
                    // Write to output
                    output += notes[noteNum].noteArray[notes[noteNum].noteElement];
                    notes[noteNum].noteElement += exponentOfTwo(notes[noteNum].pitch);
                    if(notes[noteNum].noteElement >= notes[noteNum].noteElementMax){
                        notes[noteNum].noteElement -= notes[noteNum].noteElementMax;
                    }
                }
            }
        }
     
        output += AUDIO_OFFSET; // Offset
        
        spi1Write(output);
    }
}

/*
 * 
 */
void readScoreArray()
{
    int scoreMeasure, location, check;
    int updateScoreArray;
    
    do{
        updateScoreArray = 0;
        scoreMeasure = (scoreArray1[currentArrayX+2][currentArrayY] << 8) | scoreArray1[currentArrayX+3][currentArrayY];
        location = scoreArray1[currentArrayX+4][currentArrayY];

        if(scoreMeasure <= timing.measure){
            if((location) <= timing.location64){
                for(check=0; check<NUM_NOTES; check++){
                    if(notes[check].noteOn == 0){
                        updateNote(&notes[check], 0);
                        updateScoreArray = 1;
                        break;
                    }
                }   
            } 
//            else if(location <= (timing.location64Triplet | 0x80)){
//                for(check=0; check<NUM_NOTES; check++){
//                    if(notes[check].noteOn == 0){
//                        updateNote(&notes[check], 1);
//                        updateScoreArray = 1;
//                        break;
//                    }
//                }
//            }

            if(updateScoreArray == 1){
                currentArrayX += NOTE_ARRAY_SIZE;
                if(currentArrayX >= ARRAY_SIZE_X){
                    currentArrayX = 0;
                    currentArrayY++;
                    if(currentArrayY >= ARRAY_SIZE_Y){ 
                        currentArrayY = 0;
                        // End Song
                    }
                }
            }
        }
    } while(updateScoreArray == 1);
}

/*
 * 
 */
void updateNote(noteStruct *note, int isTriplet)
{
    int noteValue;
    
    note->noteOn = 1;
    note->noteElement = 0;
    noteValue = scoreArray1[currentArrayX][currentArrayY];
    note->pitch = scoreArray1[currentArrayX+1][currentArrayY];
    note->measureEnd = ((scoreArray1[currentArrayX+5][currentArrayY] << 8) | scoreArray1[currentArrayX+6][currentArrayY]);
    note->locationEnd = scoreArray1[currentArrayX+7][currentArrayY];
    if(isTriplet){ 
        note->triplet = 1;
    } else {
        note->triplet = 0;
    }

    switch(noteValue){
        case C:
            note->noteArray = c1Waveform;
            note->noteElementMax = C1_BUFF_SIZE;
            break;

        case Csh:
            note->noteArray = cSharp1Waveform;
            note->noteElementMax = CSh1_BUFF_SIZE;
            break;

        case D:
            note->noteArray = d1Waveform;
            note->noteElementMax = D1_BUFF_SIZE;
            break;

        case Dsh:
            note->noteArray = dSharp1Waveform;
            note->noteElementMax = DSh1_BUFF_SIZE;
            break;

        case E:
            note->noteArray = e1Waveform;
            note->noteElementMax = E1_BUFF_SIZE;
            break;

        case F:
            note->noteArray = f1Waveform;
            note->noteElementMax = F1_BUFF_SIZE;
            break;

        case Fsh:
            note->noteArray = fSharp1Waveform;
            note->noteElementMax = FSh1_BUFF_SIZE;
            break;

        case G:
            note->noteArray = g1Waveform;
            note->noteElementMax = G1_BUFF_SIZE;
            break;

        case Gsh:
            note->noteArray = gSharp1Waveform;
            note->noteElementMax = GSh1_BUFF_SIZE;
            break;

        case A:
            note->noteArray = a1Waveform;
            note->noteElementMax = A1_BUFF_SIZE;
            break;

        case Ash:
            note->noteArray = aSharp1Waveform;
            note->noteElementMax = ASh1_BUFF_SIZE;
            break;

        case B:
            note->noteArray = b1Waveform;
            note->noteElementMax = B1_BUFF_SIZE;
            break;
            
        default:
            note->noteArray = emptyArray;
            note->noteElementMax = 1;
            // Pauses Song and resets to beginning
    }
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
    scoreArray1[0][0] = C;  // Note
    scoreArray1[1][0] = 3;  // Pitch
    scoreArray1[2][0] = 0;  // Measure Start High
    scoreArray1[3][0] = 1;  // Measure Start Low
    scoreArray1[4][0] = 0;  // Location Start
    scoreArray1[5][0] = 0;  // Measure End High
    scoreArray1[6][0] = 1;  // Measure End Low
    scoreArray1[7][0] = 16; // Location End
    
    scoreArray1[10][0] = C;
    scoreArray1[11][0] = 3;
    scoreArray1[12][0] = 0;
    scoreArray1[13][0] = 1;
    scoreArray1[14][0] = 16;
    scoreArray1[15][0] = 0;
    scoreArray1[16][0] = 1;
    scoreArray1[17][0] = 32;
    
    scoreArray1[20][0] = G;
    scoreArray1[21][0] = 3;
    scoreArray1[22][0] = 0;
    scoreArray1[23][0] = 1;
    scoreArray1[24][0] = 32;
    scoreArray1[25][0] = 0;
    scoreArray1[26][0] = 1;
    scoreArray1[27][0] = 48;
    
    scoreArray1[30][0] = G;
    scoreArray1[31][0] = 3;
    scoreArray1[32][0] = 0;
    scoreArray1[33][0] = 1;
    scoreArray1[34][0] = 48;
    scoreArray1[35][0] = 0;
    scoreArray1[36][0] = 2;
    scoreArray1[37][0] = 0;
    
    scoreArray1[40][0] = A;
    scoreArray1[41][0] = 3;
    scoreArray1[42][0] = 0;
    scoreArray1[43][0] = 2;
    scoreArray1[44][0] = 0;
    scoreArray1[45][0] = 0;
    scoreArray1[46][0] = 2;
    scoreArray1[47][0] = 16;
    
    scoreArray1[50][0] = A;
    scoreArray1[51][0] = 3;
    scoreArray1[52][0] = 0;
    scoreArray1[53][0] = 2;
    scoreArray1[54][0] = 16;
    scoreArray1[55][0] = 0;
    scoreArray1[56][0] = 2;
    scoreArray1[57][0] = 32;
    
    scoreArray1[60][0] = G;
    scoreArray1[61][0] = 3;
    scoreArray1[62][0] = 0;
    scoreArray1[63][0] = 2;
    scoreArray1[64][0] = 32;
    scoreArray1[65][0] = 0;
    scoreArray1[66][0] = 3;
    scoreArray1[67][0] = 0;
    
    scoreArray1[0][1] = F;
    scoreArray1[1][1] = 3;
    scoreArray1[2][1] = 0;
    scoreArray1[3][1] = 3;
    scoreArray1[4][1] = 0;
    scoreArray1[5][1] = 0;
    scoreArray1[6][1] = 3;
    scoreArray1[7][1] = 16;
    
    scoreArray1[10][1] = F;
    scoreArray1[11][1] = 3;
    scoreArray1[12][1] = 0;
    scoreArray1[13][1] = 3;
    scoreArray1[14][1] = 16;
    scoreArray1[15][1] = 0;
    scoreArray1[16][1] = 3;
    scoreArray1[17][1] = 32;
    
    scoreArray1[20][1] = E;
    scoreArray1[21][1] = 3;
    scoreArray1[22][1] = 0;
    scoreArray1[23][1] = 3;
    scoreArray1[24][1] = 32;
    scoreArray1[25][1] = 0;
    scoreArray1[26][1] = 3;
    scoreArray1[27][1] = 48;
    
    scoreArray1[30][1] = E;
    scoreArray1[31][1] = 3;
    scoreArray1[32][1] = 0;
    scoreArray1[33][1] = 3;
    scoreArray1[34][1] = 48;
    scoreArray1[35][1] = 0;
    scoreArray1[36][1] = 4;
    scoreArray1[37][1] = 0;
    
    scoreArray1[40][1] = D;
    scoreArray1[41][1] = 3;
    scoreArray1[42][1] = 0;
    scoreArray1[43][1] = 4;
    scoreArray1[44][1] = 0;
    scoreArray1[45][1] = 0;
    scoreArray1[46][1] = 4;
    scoreArray1[47][1] = 16;
    
    scoreArray1[50][1] = D;
    scoreArray1[51][1] = 3;
    scoreArray1[52][1] = 0;
    scoreArray1[53][1] = 4;
    scoreArray1[54][1] = 16;
    scoreArray1[55][1] = 0;
    scoreArray1[56][1] = 4;
    scoreArray1[57][1] = 32;
    
    scoreArray1[60][1] = C;
    scoreArray1[61][1] = 3;
    scoreArray1[62][1] = 0;
    scoreArray1[63][1] = 4;
    scoreArray1[64][1] = 32;
    scoreArray1[65][1] = 0;
    scoreArray1[66][1] = 5;
    scoreArray1[67][1] = 0;
    
    scoreArray1[0][2] = G;
    scoreArray1[1][2] = 3;
    scoreArray1[2][2] = 0;
    scoreArray1[3][2] = 5;
    scoreArray1[4][2] = 0;
    scoreArray1[5][2] = 0;
    scoreArray1[6][2] = 5;
    scoreArray1[7][2] = 16;
    
    scoreArray1[10][2] = G;
    scoreArray1[11][2] = 3;
    scoreArray1[12][2] = 0;
    scoreArray1[13][2] = 5;
    scoreArray1[14][2] = 16;
    scoreArray1[15][2] = 0;
    scoreArray1[16][2] = 5;
    scoreArray1[17][2] = 32;
    
    scoreArray1[20][2] = F;
    scoreArray1[21][2] = 3;
    scoreArray1[22][2] = 0;
    scoreArray1[23][2] = 5;
    scoreArray1[24][2] = 32;
    scoreArray1[25][2] = 0;
    scoreArray1[26][2] = 5;
    scoreArray1[27][2] = 48;
    
    scoreArray1[30][2] = F;
    scoreArray1[31][2] = 3;
    scoreArray1[32][2] = 0;
    scoreArray1[33][2] = 5;
    scoreArray1[34][2] = 48;
    scoreArray1[35][2] = 0;
    scoreArray1[36][2] = 6;
    scoreArray1[37][2] = 0;
    
    scoreArray1[40][2] = E;
    scoreArray1[41][2] = 3;
    scoreArray1[42][2] = 0;
    scoreArray1[43][2] = 6;
    scoreArray1[44][2] = 0;
    scoreArray1[45][2] = 0;
    scoreArray1[46][2] = 6;
    scoreArray1[47][2] = 16;
    
    scoreArray1[50][2] = E;
    scoreArray1[51][2] = 3;
    scoreArray1[52][2] = 0;
    scoreArray1[53][2] = 6;
    scoreArray1[54][2] = 16;
    scoreArray1[55][2] = 0;
    scoreArray1[56][2] = 6;
    scoreArray1[57][2] = 32;
    
    scoreArray1[60][2] = D;
    scoreArray1[61][2] = 3;
    scoreArray1[62][2] = 0;
    scoreArray1[63][2] = 6;
    scoreArray1[64][2] = 32;
    scoreArray1[65][2] = 0;
    scoreArray1[66][2] = 7;
    scoreArray1[67][2] = 0;
    
    scoreArray1[0][3] = G;
    scoreArray1[1][3] = 3;
    scoreArray1[2][3] = 0;
    scoreArray1[3][3] = 7;
    scoreArray1[4][3] = 0;
    scoreArray1[5][3] = 0;
    scoreArray1[6][3] = 7;
    scoreArray1[7][3] = 16;
    
    scoreArray1[10][3] = G;
    scoreArray1[11][3] = 3;
    scoreArray1[12][3] = 0;
    scoreArray1[13][3] = 7;
    scoreArray1[14][3] = 16;
    scoreArray1[15][3] = 0;
    scoreArray1[16][3] = 7;
    scoreArray1[17][3] = 32;
    
    scoreArray1[20][3] = F;
    scoreArray1[21][3] = 3;
    scoreArray1[22][3] = 0;
    scoreArray1[23][3] = 7;
    scoreArray1[24][3] = 32;
    scoreArray1[25][3] = 0;
    scoreArray1[26][3] = 7;
    scoreArray1[27][3] = 48;
    
    scoreArray1[30][3] = F;
    scoreArray1[31][3] = 3;
    scoreArray1[32][3] = 0;
    scoreArray1[33][3] = 7;
    scoreArray1[34][3] = 48;
    scoreArray1[35][3] = 0;
    scoreArray1[36][3] = 8;
    scoreArray1[37][3] = 0;
    
    scoreArray1[40][3] = E;
    scoreArray1[41][3] = 3;
    scoreArray1[42][3] = 0;
    scoreArray1[43][3] = 8;
    scoreArray1[44][3] = 0;
    scoreArray1[45][3] = 0;
    scoreArray1[46][3] = 8;
    scoreArray1[47][3] = 16;
    
    scoreArray1[50][3] = E;
    scoreArray1[51][3] = 3;
    scoreArray1[52][3] = 0;
    scoreArray1[53][3] = 8;
    scoreArray1[54][3] = 16;
    scoreArray1[55][3] = 0;
    scoreArray1[56][3] = 8;
    scoreArray1[57][3] = 32;
    
    scoreArray1[60][3] = D;
    scoreArray1[61][3] = 3;
    scoreArray1[62][3] = 0;
    scoreArray1[63][3] = 8;
    scoreArray1[64][3] = 32;
    scoreArray1[65][3] = 0;
    scoreArray1[66][3] = 9;
    scoreArray1[67][3] = 0;
    
    scoreArray1[0][4] = C;  // Note
    scoreArray1[1][4] = 3;  // Pitch
    scoreArray1[2][4] = 0;  // Measure Start High
    scoreArray1[3][4] = 9;  // Measure Start Low
    scoreArray1[4][4] = 0;  // Location Start
    scoreArray1[5][4] = 0;  // Measure End High
    scoreArray1[6][4] = 9;  // Measure End Low
    scoreArray1[7][4] = 16; // Location End
    
    scoreArray1[10][4] = C;
    scoreArray1[11][4] = 3;
    scoreArray1[12][4] = 0;
    scoreArray1[13][4] = 9;
    scoreArray1[14][4] = 16;
    scoreArray1[15][4] = 0;
    scoreArray1[16][4] = 9;
    scoreArray1[17][4] = 32;
    
    scoreArray1[20][4] = G;
    scoreArray1[21][4] = 3;
    scoreArray1[22][4] = 0;
    scoreArray1[23][4] = 1;
    scoreArray1[24][4] = 32;
    scoreArray1[25][4] = 0;
    scoreArray1[26][4] = 1;
    scoreArray1[27][4] = 48;
    
    scoreArray1[30][4] = G;
    scoreArray1[31][4] = 3;
    scoreArray1[32][4] = 0;
    scoreArray1[33][4] = 9;
    scoreArray1[34][4] = 48;
    scoreArray1[35][4] = 0;
    scoreArray1[36][4] = 10;
    scoreArray1[37][4] = 0;
    
    scoreArray1[40][4] = A;
    scoreArray1[41][4] = 3;
    scoreArray1[42][4] = 0;
    scoreArray1[43][4] = 10;
    scoreArray1[44][4] = 0;
    scoreArray1[45][4] = 0;
    scoreArray1[46][4] = 10;
    scoreArray1[47][4] = 16;
    
    scoreArray1[50][4] = A;
    scoreArray1[51][4] = 3;
    scoreArray1[52][4] = 0;
    scoreArray1[53][4] = 10;
    scoreArray1[54][4] = 16;
    scoreArray1[55][4] = 0;
    scoreArray1[56][4] = 10;
    scoreArray1[57][4] = 32;
    
    scoreArray1[60][4] = G;
    scoreArray1[61][4] = 3;
    scoreArray1[62][4] = 0;
    scoreArray1[63][4] = 10;
    scoreArray1[64][4] = 32;
    scoreArray1[65][4] = 0;
    scoreArray1[66][4] = 11;
    scoreArray1[67][4] = 0;
    
    scoreArray1[0][5] = F;
    scoreArray1[1][5] = 3;
    scoreArray1[2][5] = 0;
    scoreArray1[3][5] = 11;
    scoreArray1[4][5] = 0;
    scoreArray1[5][5] = 0;
    scoreArray1[6][5] = 11;
    scoreArray1[7][5] = 16;
    
    scoreArray1[10][5] = F;
    scoreArray1[11][5] = 3;
    scoreArray1[12][5] = 0;
    scoreArray1[13][5] = 11;
    scoreArray1[14][5] = 16;
    scoreArray1[15][5] = 0;
    scoreArray1[16][5] = 11;
    scoreArray1[17][5] = 32;
    
    scoreArray1[20][5] = E;
    scoreArray1[21][5] = 3;
    scoreArray1[22][5] = 0;
    scoreArray1[23][5] = 11;
    scoreArray1[24][5] = 32;
    scoreArray1[25][5] = 0;
    scoreArray1[26][5] = 11;
    scoreArray1[27][5] = 48;
    
    scoreArray1[30][5] = E;
    scoreArray1[31][5] = 3;
    scoreArray1[32][5] = 0;
    scoreArray1[33][5] = 11;
    scoreArray1[34][5] = 48;
    scoreArray1[35][5] = 0;
    scoreArray1[36][5] = 12;
    scoreArray1[37][5] = 0;
    
    scoreArray1[40][5] = D;
    scoreArray1[41][5] = 3;
    scoreArray1[42][5] = 0;
    scoreArray1[43][5] = 12;
    scoreArray1[44][5] = 0;
    scoreArray1[45][5] = 0;
    scoreArray1[46][5] = 12;
    scoreArray1[47][5] = 16;
    
    scoreArray1[50][5] = G;
    scoreArray1[51][5] = 3;
    scoreArray1[52][5] = 0;
    scoreArray1[53][5] = 12;
    scoreArray1[54][5] = 16;
    scoreArray1[55][5] = 0;
    scoreArray1[56][5] = 12;
    scoreArray1[57][5] = 32;
    
    scoreArray1[60][5] = B;
    scoreArray1[61][5] = 3;
    scoreArray1[62][5] = 0;
    scoreArray1[63][5] = 12;
    scoreArray1[64][5] = 16;
    scoreArray1[65][5] = 0;
    scoreArray1[66][5] = 12;
    scoreArray1[67][5] = 32;
    
    scoreArray1[0][6] = C;
    scoreArray1[1][6] = 4;
    scoreArray1[2][6] = 0;
    scoreArray1[3][6] = 12;
    scoreArray1[4][6] = 32;
    scoreArray1[5][6] = 0;
    scoreArray1[6][6] = 13;
    scoreArray1[7][6] = 0;
    
    scoreArray1[10][6] = C;
    scoreArray1[11][6] = 3;
    scoreArray1[12][6] = 0;
    scoreArray1[13][6] = 12;
    scoreArray1[14][6] = 32;
    scoreArray1[15][6] = 0;
    scoreArray1[16][6] = 13;
    scoreArray1[17][6] = 0;
    
    scoreArray1[20][6] = E;
    scoreArray1[21][6] = 3;
    scoreArray1[22][6] = 0;
    scoreArray1[23][6] = 12;
    scoreArray1[24][6] = 32;
    scoreArray1[25][6] = 0;
    scoreArray1[26][6] = 13;
    scoreArray1[27][6] = 0;
    
    scoreArray1[30][6] = G;
    scoreArray1[31][6] = 3;
    scoreArray1[32][6] = 0;
    scoreArray1[33][6] = 12;
    scoreArray1[34][6] = 32;
    scoreArray1[35][6] = 0;
    scoreArray1[36][6] = 13;
    scoreArray1[37][6] = 0;
   
    
    int j, k;
    
    for(j=40; j<ARRAY_SIZE_X; j++){
        for(k=6; k<ARRAY_SIZE_Y; k++){
            scoreArray1[j][k] = 0x7FFF;
        }
    }
    
}

int exponentOfTwo(int exp)
{
    int i;
    int result = 1;
    
    for(i=0; i<exp; i++){
        result *= 2;
    }
    
    return result;
}
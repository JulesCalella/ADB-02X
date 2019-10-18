#include <xc.h>
#include <math.h>
#include "dsPIC33CH512MP506_Audio.h"
#include "dsPIC33CH512MP506_Modules.h"
#include "dsPIC33CH512MP506_Pins.h"

#define STATE_ARRAY_LENGTH 600

enum noteStates {ATTACK_STATE = 0, DECAY_STATE, SUSTAIN_STATE, RELEASE_STATE};

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
int controlArray[NUM_CTRL];
double attackArray[STATE_ARRAY_LENGTH];
double decayArray[STATE_ARRAY_LENGTH];
double releaseArray[STATE_ARRAY_LENGTH];

int scoreArray1[ARRAY_SIZE_X][ARRAY_SIZE_Y];
int currentArrayX;
int currentArrayY;
int currentArray;
volatile int outputBuffer[ARRAY_OUTPUT_BUFFER];

timingStruct *timerAudio;

noteStruct notes[NUM_NOTES];

void loading(int isLoading)
{
    if(isLoading){
        timerAudio->loading = 1;
    } else {
        timerAudio->loading = 0;
    }

    timerAudio->loadingCount = 0;
    timerAudio->loadingDisplay = 0;
    updateNumberDisplay();
}

/*
 * 
 */
void generateAllWaveforms()
{
    loading(1);
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
    updateStateArrays();
    emptyArray[0] = 0;
    loading(0);
}

void updateStateArrays()
{
    int currentNote, stateElem;
    
    // A value of 0-100 results in a multiplier or 1-5
    float atkMult = 1.0 + (controlArray[CTRL_ATTACK_MULT] / 25.0);
    double decayMult = 1.0 - atkMult;
    
    int atkElem = (int)((SAMPLING_FREQUENCY / 1000.0) * controlArray[CTRL_ATTACK_DUR]);
    if(atkElem >= STATE_ARRAY_LENGTH) atkElem = STATE_ARRAY_LENGTH;
    
    int decElem = (int)((SAMPLING_FREQUENCY / 1000.0) * controlArray[CTRL_DECAY_DUR]);
    if(decElem >= STATE_ARRAY_LENGTH) decElem = STATE_ARRAY_LENGTH;
    
    int relElem = (int)((SAMPLING_FREQUENCY / 1000.0) * controlArray[CTRL_RELEASE_DUR]);
    if(relElem >= STATE_ARRAY_LENGTH) relElem = STATE_ARRAY_LENGTH;
    
    double atkElemF = atkElem * 1.0;
    double decElemF = decElem * 1.0;
    double relElemF = relElem * 1.0;
    
    // Generate Attack
    for(stateElem = 0; stateElem < atkElem; stateElem++){
        attackArray[stateElem] = (atkMult * stateElem) / atkElemF;
    }

    // Generate Decay
    for(stateElem = 0; stateElem < decElem; stateElem++){
        decayArray[stateElem] = ((decayMult * stateElem) / decElemF) + atkMult;
    }
    
    // Generate Release
    for(stateElem = 0; stateElem < relElem; stateElem++){
        releaseArray[stateElem] = ((-1 * stateElem) / relElemF) + 1;
    }
    
    // Reset state element and state to account for shortened duration (bounds check)
    for(currentNote = 0; currentNote < NUM_NOTES; currentNote++){
        notes[currentNote].stateElement = 0;
        notes[currentNote].currentState = ATTACK_STATE;
        notes[currentNote].attackElements = atkElem;
        notes[currentNote].decayElements = decElem;
        notes[currentNote].releaseElements = relElem;
    }
}

/*
 * 
 */
void generateWaveform(int *audioArray, int *controlArray, int buffSize)
{
    clearAudioArray(audioArray, buffSize);
    if(controlArray[CTRL_SINE_AMP] != 0)  generateSineWave(audioArray, controlArray[CTRL_SINE_AMP], buffSize);
    if(controlArray[CTRL_SQUARE_AMP] != 0) generateSquareWave(audioArray, controlArray[CTRL_SQUARE_AMP], buffSize);
    if(controlArray[CTRL_TRIANGLE_AMP] != 0) generateTriangleWave(audioArray, controlArray[CTRL_TRIANGLE_AMP], buffSize);
    if(controlArray[CTRL_SAWTOOTH_AMP] != 0) generateSawWave(audioArray, controlArray[CTRL_SAWTOOTH_AMP], buffSize);
    if(controlArray[CTRL_BIT_AMP] != 0) generateBitWave(audioArray, controlArray[CTRL_BIT_AMP], buffSize);
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
    for(overtoneNum=CTRL_OVERTONE1; overtoneNum<=(CTRL_OVERTONE1 + 8); overtoneNum++){
        if(controlArray[overtoneNum] != 0){
            overtoneAmplitude = (controlArray[overtoneNum]/100.0) * amplitude;
            for(i=0; i<numBytes; i++){
                audioArray[i] += (AMPLITUDE_MULTIPLIER * (overtoneAmplitude * sin(2.0 * PI * (overtoneNum+2) * (i/(numBytes * 1.0)))));
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
    int polarityThresh, toggleThreshHigh, toggleThreshLow;
    for(overtoneNum=CTRL_OVERTONE1; overtoneNum<=(CTRL_OVERTONE1 + 8); overtoneNum++){
        if(controlArray[overtoneNum] != 0){
            overtoneAmplitude = (controlArray[overtoneNum] / 100.0) * amplitude;
            toggleThreshHigh = numBytes / (2 * (overtoneNum+2));
            toggleThreshLow = numBytes / (overtoneNum+2);
            polarityThresh = 0;
            
            for(i=0; i<numBytes; i++){
                if(polarityThresh < toggleThreshHigh){
                    audioArray[i] += (AMPLITUDE_MULTIPLIER * overtoneAmplitude);
                } else {
                    audioArray[i] += (AMPLITUDE_MULTIPLIER * overtoneAmplitude * -1);
                }
                polarityThresh++;
                if(polarityThresh >= toggleThreshLow) polarityThresh = 0;
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
    int halfWave = numBytes/2;
    
    for(i=0; i<firstLoopMax; i++){
        audioArray[i] += AMPLITUDE_MULTIPLIER * amplitude * (i/(firstLoopMax * 1.0));
    }
    
    for( ; i<secondLoopMax; i++){
        audioArray[i] += AMPLITUDE_MULTIPLIER * amplitude * ((i-firstLoopMax)/(-halfWave * 1.0)) + 1;
    }
    
    for( ; i<numBytes; i++){
        audioArray[i]  += AMPLITUDE_MULTIPLIER * amplitude * ((i-firstLoopMax)/(firstLoopMax * 1.0)) - 1;
    }
    
    // Overtones
    int counter, thirdLoopMax;
    for(overtoneNum=CTRL_OVERTONE1; overtoneNum<=(CTRL_OVERTONE1 + 8); overtoneNum++){
        if(controlArray[overtoneNum] != 0){
            overtoneAmplitude = (controlArray[overtoneNum] / 100.0) * amplitude;
            firstLoopMax = numBytes / (4 * (overtoneNum + 2));
            secondLoopMax = (3 * numBytes) / (4 * (overtoneNum + 2));
            thirdLoopMax = numBytes / (overtoneNum + 2);
            halfWave = numBytes/(2*(overtoneNum+2));
            counter = 0;
            
            for(i=0; i<numBytes; i++){
                if(counter < firstLoopMax){
                    audioArray[i] += (AMPLITUDE_MULTIPLIER * overtoneAmplitude * (i/(firstLoopMax * 1.0)));
                } else if(counter < secondLoopMax){
                    audioArray[i] += (AMPLITUDE_MULTIPLIER * overtoneAmplitude * ((i-firstLoopMax)/(-halfWave * 1.0)) + 1);
                } else {
                    audioArray[i]  += (AMPLITUDE_MULTIPLIER * overtoneAmplitude * ((i-firstLoopMax)/(firstLoopMax * 1.0)) - 1);
                }
                counter++;
                if(counter>=thirdLoopMax) counter = 0;
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
    int counter, changeThresh;
    for(overtoneNum=CTRL_OVERTONE1; overtoneNum<=(CTRL_OVERTONE1 + 8); overtoneNum++){
        if(controlArray[overtoneNum] != 0){
            overtoneAmplitude = (controlArray[overtoneNum]/100.0) * amplitude;
            changeThresh = numBytes / (overtoneNum + 2);
            counter = 0;
            
            for(i=0; i<numBytes; i++){
                audioArray[i] += AMPLITUDE_MULTIPLIER * overtoneAmplitude * (((2.0 * counter)/changeThresh) - 1);
                counter++;
                if(counter >= changeThresh) counter = 0;
            }
        }
    }
}

/*
 * 
 */
void generateBitWave(int *audioArray, int amplitude, int numBytes)
{
    int i, overtoneNum;
    float overtoneAmplitude;
    
    for(i=0; i<numBytes; i++){
        audioArray[i] += (AMPLITUDE_MULTIPLIER* (amplitude * sin(2.0 * PI * (i/(numBytes * 1.0)))));
        audioArray[i] &= 0xFFFFFFF8; // Removes the bottom 3 bits
    }
    
    // Overtones
    for(overtoneNum=CTRL_OVERTONE1; overtoneNum<=(CTRL_OVERTONE1 + 8); overtoneNum++){
        if(controlArray[overtoneNum] != 0){
            overtoneAmplitude = (controlArray[overtoneNum]/100.0) * amplitude;
            for(i=0; i<numBytes; i++){
                audioArray[i] += (AMPLITUDE_MULTIPLIER * (overtoneAmplitude * sin(2.0 * PI * (overtoneNum+2) * (i/(numBytes * 1.0)))));
                audioArray[i] &= 0xFFFFFFF8; // Removes the bottom 3 bits
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
    controlArray[CTRL_OVERTONE1] = 80;
    controlArray[CTRL_OVERTONE2] = 30;
    controlArray[CTRL_OVERTONE3] = 50;
    controlArray[CTRL_OVERTONE4] = 17;
    controlArray[CTRL_OVERTONE5] = 3;
    controlArray[CTRL_OVERTONE6] = 5;
    controlArray[CTRL_OVERTONE7] = 20;
    controlArray[CTRL_OVERTONE8] = 0;
    controlArray[CTRL_OVERTONE9] = 0;
    controlArray[CTRL_SINE_AMP] = 100;
    controlArray[CTRL_SQUARE_AMP] = 0;
    controlArray[CTRL_TRIANGLE_AMP] = 0;
    controlArray[CTRL_SAWTOOTH_AMP] = 0;
    controlArray[CTRL_BIT_AMP] = 0;
    controlArray[CTRL_FN1] = 0;
    controlArray[CTRL_FN2] = 0;
    controlArray[CTRL_FN3] = 0;
    controlArray[CTRL_TEMPO] = 70;
    controlArray[CTRL_ATTACK_MULT] = 12;
    controlArray[CTRL_ATTACK_DUR] = 10;
    controlArray[CTRL_DECAY_DUR] = 10;
    controlArray[CTRL_RELEASE_DUR] = 100;
}

/*
 * 
 */
void readControlArray(int *toReadArray)
{
    int i;
    for(i=0; i<NUM_FN; i++){
        toReadArray[i] = controlArray[i];
    }
}

/*
 * 
 */
void writeControlArray(int *toWriteArray)
{
    int i;
    for(i=0; i<NUM_FN; i++){
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
        notes[currentNote].attackArray = attackArray;
        notes[currentNote].decayArray = decayArray;
        notes[currentNote].releaseArray = releaseArray;
    }
    
    currentArrayX = 0;
    currentArrayY = 0;
    currentArray = 1;
    *newOutput = 0;
}

/*
 *
 */ 
void updateOutputBuffer(int *newOutput)
{
    if(*newOutput != 1) return;
    *newOutput = 0;
    
    int noteNum;
    int output = 0;
    
    // Read current duration and reset as it is a volatile variable
    //int elapsedTime = timerAudio->duration;
    timerAudio->duration = 0;
        
    for(noteNum=0; noteNum<NUM_NOTES; noteNum++){
        // If note is currently on, check if it should be over
        if(notes[noteNum].noteOn == 1){

            // Check if note is over
            if(notes[noteNum].measureEnd <= timerAudio->measure){
                if((notes[noteNum].locationEnd <= timerAudio->location64) && (notes[noteNum].triplet == 0)){
                    // Non-triplet case
                    // If note is over, set state to Release and reset stateDuration
                    if(notes[noteNum].currentState != RELEASE_STATE){
                        notes[noteNum].currentState = RELEASE_STATE;
                        notes[noteNum].stateElement = 0;
                    }
                } else if((notes[noteNum].locationEnd <= timerAudio->location64Triplet)&&(notes[noteNum].triplet == 1)){
                    // Triplet case
                    // If note is over, set state to Release and reset stateDuration
                    if(notes[noteNum].currentState != RELEASE_STATE){
                        notes[noteNum].currentState = RELEASE_STATE;
                        notes[noteNum].stateElement = 0;
                    }
                }
            }
            
            // Determine multiplier based on state
            // Update state if ready
            switch(notes[noteNum].currentState){
                case ATTACK_STATE: // Attack
                    if(notes[noteNum].stateElement < notes[noteNum].attackElements){
                        output += (int)(attackArray[notes[noteNum].stateElement] * notes[noteNum].waveformArray[notes[noteNum].noteElement]);
                    } else {
                        notes[noteNum].currentState = DECAY_STATE;
                        notes[noteNum].stateElement = 0;
                        output += (int)(decayArray[notes[noteNum].stateElement] * notes[noteNum].waveformArray[notes[noteNum].noteElement]);
                    }
                    break;

                case DECAY_STATE: // Decay
                    if(notes[noteNum].stateElement < notes[noteNum].decayElements){
                        output += (int)(decayArray[notes[noteNum].stateElement] * notes[noteNum].waveformArray[notes[noteNum].noteElement]);
                    } else {
                        notes[noteNum].currentState = SUSTAIN_STATE;
                        notes[noteNum].stateElement = 0;
                        output += notes[noteNum].waveformArray[notes[noteNum].noteElement];
                    }
                    break;

                case SUSTAIN_STATE: // Sustain
                    output += notes[noteNum].waveformArray[notes[noteNum].noteElement];
                    break;

                case RELEASE_STATE: // Release
                    if(notes[noteNum].stateElement < notes[noteNum].attackElements){
                        output += (int)(releaseArray[notes[noteNum].stateElement] * notes[noteNum].waveformArray[notes[noteNum].noteElement]);
                    } else {
                        notes[noteNum].noteOn = 0;
                    }
                    break;
                    
                default:
                    notes[noteNum].currentState = ATTACK_STATE;
                    break;
            }

            // Increment state array element for next output
            notes[noteNum].stateElement++;
            
            // Increase value of element based on pitch
            notes[noteNum].noteElement += exponentOfTwo(notes[noteNum].pitch);
            if(notes[noteNum].noteElement >= notes[noteNum].noteElementMax){
                notes[noteNum].noteElement -= notes[noteNum].noteElementMax;
            }
        }
    }
     
    output += AUDIO_OFFSET; // Offset
    
    // Output Data
    spi1Write(output);
}

void updateOutputBufferOriginal(int *newOutput)
{
    int noteNum, noteEnded;
    
    if(*newOutput != 0){
        *newOutput = 0;
        int output = 0;
        
        for(noteNum=0; noteNum<NUM_NOTES; noteNum++){
            if(notes[noteNum].noteOn == 1){
                noteEnded = 0;
                // Check if note is over
                if(notes[noteNum].measureEnd <= timerAudio->measure){
                    if((notes[noteNum].locationEnd <= timerAudio->location64) && (notes[noteNum].triplet == 0)){
                        notes[noteNum].noteOn = 0;
                        noteEnded = 1;
                    } else if((notes[noteNum].locationEnd <= timerAudio->location64Triplet)&&(notes[noteNum].triplet == 1)){
                        notes[noteNum].noteOn = 0;
                        noteEnded = 1;
                    }
                }
                
                if(noteEnded == 0){
                    // Write to output
                    output += notes[noteNum].waveformArray[notes[noteNum].noteElement];
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

        if(scoreMeasure <= timerAudio->measure){
            if((location) <= timerAudio->location64){
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

            // Update location of next note on score
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
 *      attackVolume
 *       /\   Sustain Volume (dynamic)
 *      /  \___________________________  
 *     /                               \     
 *    /                                 \ 
 *   /                                   \
 * 
 */                          
void updateNote(noteStruct *note, int isTriplet)
{
    int noteValue;
    
    // Activate note
    note->noteOn = 1;
    note->noteElement = 0;
    // Read values from the score array
    noteValue = scoreArray1[currentArrayX][currentArrayY];
    note->pitch = scoreArray1[currentArrayX+1][currentArrayY];
    note->measureEnd = ((scoreArray1[currentArrayX+5][currentArrayY] << 8) | scoreArray1[currentArrayX+6][currentArrayY]);
    note->locationEnd = scoreArray1[currentArrayX+7][currentArrayY];
    note->dynamic = 100;
    // Activate attack state
    note->currentState = ATTACK_STATE;
    note->stateElement = 0;
    
    if(isTriplet){ 
        note->triplet = 1;
    } else {
        note->triplet = 0;
    }

    switch(noteValue){
        case C:
            note->waveformArray = c1Waveform;
            note->noteElementMax = C1_BUFF_SIZE;
            break;

        case Csh:
            note->waveformArray = cSharp1Waveform;
            note->noteElementMax = CSh1_BUFF_SIZE;
            break;

        case D:
            note->waveformArray = d1Waveform;
            note->noteElementMax = D1_BUFF_SIZE;
            break;

        case Dsh:
            note->waveformArray = dSharp1Waveform;
            note->noteElementMax = DSh1_BUFF_SIZE;
            break;

        case E:
            note->waveformArray = e1Waveform;
            note->noteElementMax = E1_BUFF_SIZE;
            break;

        case F:
            note->waveformArray = f1Waveform;
            note->noteElementMax = F1_BUFF_SIZE;
            break;

        case Fsh:
            note->waveformArray = fSharp1Waveform;
            note->noteElementMax = FSh1_BUFF_SIZE;
            break;

        case G:
            note->waveformArray = g1Waveform;
            note->noteElementMax = G1_BUFF_SIZE;
            break;

        case Gsh:
            note->waveformArray = gSharp1Waveform;
            note->noteElementMax = GSh1_BUFF_SIZE;
            break;

        case A:
            note->waveformArray = a1Waveform;
            note->noteElementMax = A1_BUFF_SIZE;
            break;

        case Ash:
            note->waveformArray = aSharp1Waveform;
            note->noteElementMax = ASh1_BUFF_SIZE;
            break;

        case B:
            note->waveformArray = b1Waveform;
            note->noteElementMax = B1_BUFF_SIZE;
            break;
            
        default:
            note->waveformArray = emptyArray;
            note->noteElementMax = 1;
            // TODO: Pauses Song and resets to beginning
    }
}

void setScoreArray(int x, int y)
{
    currentArrayX = x;
    currentArrayY = y;
    timerAudio->measure = 0;
    timerAudio->location64 = 0;
    timerAudio->location64Triplet = 0;
}

void rewind()
{
    int noteNum;
    for(noteNum=0; noteNum<NUM_NOTES; noteNum++){
        notes[noteNum].noteOn = 0;
    }
    
    int currentTimerMeasure = timerAudio->measure--;
    int currentScoreMeasure;
    
    do{
        currentArrayX -= NUM_NOTES;
        if(currentArrayX < 0){
            currentArrayX = ARRAY_SIZE_X - NUM_NOTES;
            currentArrayY--;
            
            if(currentArrayY < 0){
                currentArrayX = 0;
                currentArrayY = 0;
                timerAudio->interruptCount = 0;
                return;
            }
        }
        
        currentScoreMeasure = (scoreArray1[currentArrayX+2][currentArrayY] << 8) | scoreArray1[currentArrayX+3][currentArrayY];
    } while(currentScoreMeasure >= currentTimerMeasure);
    
    timerAudio->measure = currentTimerMeasure;
    timerAudio->location64 = 0;
    timerAudio->interruptCount = 0;
    
    readScoreArray();
}

void rewindToBeginning()
{
    int noteNum;
    for(noteNum=0; noteNum<NUM_NOTES; noteNum++){
        notes[noteNum].noteOn = 0;
    }
    
    currentArrayX = 0;
    currentArrayY = 0;
    
    timerAudio->measure = 0;
    timerAudio->location64 = 0;
    timerAudio->interruptCount = 0;
}

void fastForward()
{
    int noteNum;
    for(noteNum=0; noteNum<NUM_NOTES; noteNum++){
        notes[noteNum].noteOn = 0;
    }
    
    int currentTimerMeasure = timerAudio->measure++;
    int currentScoreMeasure;
    
    do{
        currentArrayX += NUM_NOTES;
        if(currentArrayX >= ARRAY_SIZE_X){
            currentArrayX = 0;
            currentArrayY++;
            
            // TODO: Check if end of song with end of song marker
            if(currentArrayY >= ARRAY_SIZE_Y){
                currentArrayY--;
                return;
            }
        }
        
        currentScoreMeasure = (scoreArray1[currentArrayX+2][currentArrayY] << 8) | scoreArray1[currentArrayX+3][currentArrayY];
    } while(currentScoreMeasure < currentTimerMeasure);
    
    timerAudio->measure = currentTimerMeasure;
    timerAudio->location64 = 0;
    timerAudio->interruptCount = 0;
    
    readScoreArray();
}

void linkTimer(timingStruct *timer)
{
    timerAudio = timer;
    timerAudio->interruptCount = 0;
    timerAudio->location64 = 0;
    timerAudio->location64Triplet = 0;
    timerAudio->measure = 0;
    updateTempo(120);
    timerAudio->loading = 0;
    timerAudio->loadingCount = 0;
    timerAudio->loadingDisplay = 0;
}

void updateTempo(int newTempo)
{
    timerAudio->tempo = newTempo;
    timerAudio->location64Max = (int)((1000.0/newTempo) * 375);
}

void writeSong(){
    
scoreArray1[0][0] = G;
scoreArray1[1][0] = 3;
scoreArray1[2][0] = 0;
scoreArray1[3][0] = 0;
scoreArray1[4][0] = 0;
scoreArray1[5][0] = 0;
scoreArray1[6][0] = 0;
scoreArray1[7][0] = 16;

scoreArray1[10][0] = D;
scoreArray1[11][0] = 4;
scoreArray1[12][0] = 0;
scoreArray1[13][0] = 0;
scoreArray1[14][0] = 16;
scoreArray1[15][0] = 0;
scoreArray1[16][0] = 0;
scoreArray1[17][0] = 32;

scoreArray1[20][0] = Ash;
scoreArray1[21][0] = 3;
scoreArray1[22][0] = 0;
scoreArray1[23][0] = 0;
scoreArray1[24][0] = 32;
scoreArray1[25][0] = 0;
scoreArray1[26][0] = 0;
scoreArray1[27][0] = 56;

scoreArray1[30][0] = A;
scoreArray1[31][0] = 3;
scoreArray1[32][0] = 0;
scoreArray1[33][0] = 0;
scoreArray1[34][0] = 56;
scoreArray1[35][0] = 0;
scoreArray1[36][0] = 1;
scoreArray1[37][0] = 0;

scoreArray1[40][0] = G;
scoreArray1[41][0] = 3;
scoreArray1[42][0] = 0;
scoreArray1[43][0] = 1;
scoreArray1[44][0] = 0;
scoreArray1[45][0] = 0;
scoreArray1[46][0] = 1;
scoreArray1[47][0] = 8;

scoreArray1[50][0] = Ash;
scoreArray1[51][0] = 3;
scoreArray1[52][0] = 0;
scoreArray1[53][0] = 1;
scoreArray1[54][0] = 8;
scoreArray1[55][0] = 0;
scoreArray1[56][0] = 1;
scoreArray1[57][0] = 16;

scoreArray1[60][0] = A;
scoreArray1[61][0] = 3;
scoreArray1[62][0] = 0;
scoreArray1[63][0] = 1;
scoreArray1[64][0] = 16;
scoreArray1[65][0] = 0;
scoreArray1[66][0] = 1;
scoreArray1[67][0] = 24;

scoreArray1[0][1] = G;
scoreArray1[1][1] = 3;
scoreArray1[2][1] = 0;
scoreArray1[3][1] = 1;
scoreArray1[4][1] = 24;
scoreArray1[5][1] = 0;
scoreArray1[6][1] = 1;
scoreArray1[7][1] = 32;

scoreArray1[10][1] = Fsh;
scoreArray1[11][1] = 3;
scoreArray1[12][1] = 0;
scoreArray1[13][1] = 1;
scoreArray1[14][1] = 32;
scoreArray1[15][1] = 0;
scoreArray1[16][1] = 1;
scoreArray1[17][1] = 40;

scoreArray1[20][1] = A;
scoreArray1[21][1] = 3;
scoreArray1[22][1] = 0;
scoreArray1[23][1] = 1;
scoreArray1[24][1] = 40;
scoreArray1[25][1] = 0;
scoreArray1[26][1] = 1;
scoreArray1[27][1] = 48;

scoreArray1[30][1] = D;
scoreArray1[31][1] = 3;
scoreArray1[32][1] = 0;
scoreArray1[33][1] = 1;
scoreArray1[34][1] = 48;
scoreArray1[35][1] = 0;
scoreArray1[36][1] = 2;
scoreArray1[37][1] = 0;

scoreArray1[40][1] = G;
scoreArray1[41][1] = 3;
scoreArray1[42][1] = 0;
scoreArray1[43][1] = 2;
scoreArray1[44][1] = 0;
scoreArray1[45][1] = 0;
scoreArray1[46][1] = 2;
scoreArray1[47][1] = 8;

scoreArray1[50][1] = D;
scoreArray1[51][1] = 3;
scoreArray1[52][1] = 0;
scoreArray1[53][1] = 2;
scoreArray1[54][1] = 8;
scoreArray1[55][1] = 0;
scoreArray1[56][1] = 2;
scoreArray1[57][1] = 16;

scoreArray1[60][1] = A;
scoreArray1[61][1] = 3;
scoreArray1[62][1] = 0;
scoreArray1[63][1] = 2;
scoreArray1[64][1] = 16;
scoreArray1[65][1] = 0;
scoreArray1[66][1] = 2;
scoreArray1[67][1] = 24;

scoreArray1[0][2] = D;
scoreArray1[1][2] = 3;
scoreArray1[2][2] = 0;
scoreArray1[3][2] = 2;
scoreArray1[4][2] = 24;
scoreArray1[5][2] = 0;
scoreArray1[6][2] = 2;
scoreArray1[7][2] = 32;

scoreArray1[10][2] = Ash;
scoreArray1[11][2] = 3;
scoreArray1[12][2] = 0;
scoreArray1[13][2] = 2;
scoreArray1[14][2] = 32;
scoreArray1[15][2] = 0;
scoreArray1[16][2] = 2;
scoreArray1[17][2] = 40;

scoreArray1[20][2] = A;
scoreArray1[21][2] = 3;
scoreArray1[22][2] = 0;
scoreArray1[23][2] = 2;
scoreArray1[24][2] = 40;
scoreArray1[25][2] = 0;
scoreArray1[26][2] = 2;
scoreArray1[27][2] = 44;

scoreArray1[30][2] = G;
scoreArray1[31][2] = 3;
scoreArray1[32][2] = 0;
scoreArray1[33][2] = 2;
scoreArray1[34][2] = 44;
scoreArray1[35][2] = 0;
scoreArray1[36][2] = 2;
scoreArray1[37][2] = 48;

scoreArray1[40][2] = A;
scoreArray1[41][2] = 3;
scoreArray1[42][2] = 0;
scoreArray1[43][2] = 2;
scoreArray1[44][2] = 48;
scoreArray1[45][2] = 0;
scoreArray1[46][2] = 2;
scoreArray1[47][2] = 56;

scoreArray1[50][2] = D;
scoreArray1[51][2] = 3;
scoreArray1[52][2] = 0;
scoreArray1[53][2] = 2;
scoreArray1[54][2] = 56;
scoreArray1[55][2] = 0;
scoreArray1[56][2] = 3;
scoreArray1[57][2] = 0;

scoreArray1[60][2] = G;
scoreArray1[61][2] = 3;
scoreArray1[62][2] = 0;
scoreArray1[63][2] = 3;
scoreArray1[64][2] = 0;
scoreArray1[65][2] = 0;
scoreArray1[66][2] = 3;
scoreArray1[67][2] = 8;

scoreArray1[0][3] = D;
scoreArray1[1][3] = 3;
scoreArray1[2][3] = 0;
scoreArray1[3][3] = 3;
scoreArray1[4][3] = 8;
scoreArray1[5][3] = 0;
scoreArray1[6][3] = 3;
scoreArray1[7][3] = 12;

scoreArray1[10][3] = G;
scoreArray1[11][3] = 3;
scoreArray1[12][3] = 0;
scoreArray1[13][3] = 3;
scoreArray1[14][3] = 12;
scoreArray1[15][3] = 0;
scoreArray1[16][3] = 3;
scoreArray1[17][3] = 16;

scoreArray1[20][3] = A;
scoreArray1[21][3] = 3;
scoreArray1[22][3] = 0;
scoreArray1[23][3] = 3;
scoreArray1[24][3] = 16;
scoreArray1[25][3] = 0;
scoreArray1[26][3] = 3;
scoreArray1[27][3] = 24;

scoreArray1[30][3] = D;
scoreArray1[31][3] = 3;
scoreArray1[32][3] = 0;
scoreArray1[33][3] = 3;
scoreArray1[34][3] = 24;
scoreArray1[35][3] = 0;
scoreArray1[36][3] = 3;
scoreArray1[37][3] = 28;

scoreArray1[40][3] = A;
scoreArray1[41][3] = 3;
scoreArray1[42][3] = 0;
scoreArray1[43][3] = 3;
scoreArray1[44][3] = 28;
scoreArray1[45][3] = 0;
scoreArray1[46][3] = 3;
scoreArray1[47][3] = 32;

scoreArray1[50][3] = Ash;
scoreArray1[51][3] = 3;
scoreArray1[52][3] = 0;
scoreArray1[53][3] = 3;
scoreArray1[54][3] = 32;
scoreArray1[55][3] = 0;
scoreArray1[56][3] = 3;
scoreArray1[57][3] = 40;

scoreArray1[60][3] = A;
scoreArray1[61][3] = 3;
scoreArray1[62][3] = 0;
scoreArray1[63][3] = 3;
scoreArray1[64][3] = 40;
scoreArray1[65][3] = 0;
scoreArray1[66][3] = 3;
scoreArray1[67][3] = 44;

scoreArray1[0][4] = G;
scoreArray1[1][4] = 3;
scoreArray1[2][4] = 0;
scoreArray1[3][4] = 3;
scoreArray1[4][4] = 44;
scoreArray1[5][4] = 0;
scoreArray1[6][4] = 3;
scoreArray1[7][4] = 48;

scoreArray1[10][4] = A;
scoreArray1[11][4] = 3;
scoreArray1[12][4] = 0;
scoreArray1[13][4] = 3;
scoreArray1[14][4] = 48;
scoreArray1[15][4] = 0;
scoreArray1[16][4] = 3;
scoreArray1[17][4] = 52;

scoreArray1[20][4] = D;
scoreArray1[21][4] = 3;
scoreArray1[22][4] = 0;
scoreArray1[23][4] = 3;
scoreArray1[24][4] = 52;
scoreArray1[25][4] = 0;
scoreArray1[26][4] = 3;
scoreArray1[27][4] = 56;

scoreArray1[30][4] = D;
scoreArray1[31][4] = 4;
scoreArray1[32][4] = 0;
scoreArray1[33][4] = 3;
scoreArray1[34][4] = 56;
scoreArray1[35][4] = 0;
scoreArray1[36][4] = 3;
scoreArray1[37][4] = 60;

scoreArray1[40][4] = C;
scoreArray1[41][4] = 4;
scoreArray1[42][4] = 0;
scoreArray1[43][4] = 3;
scoreArray1[44][4] = 60;
scoreArray1[45][4] = 0;
scoreArray1[46][4] = 4;
scoreArray1[47][4] = 0;

scoreArray1[50][4] = Ash;
scoreArray1[51][4] = 3;
scoreArray1[52][4] = 0;
scoreArray1[53][4] = 4;
scoreArray1[54][4] = 0;
scoreArray1[55][4] = 0;
scoreArray1[56][4] = 4;
scoreArray1[57][4] = 4;

scoreArray1[60][4] = A;
scoreArray1[61][4] = 3;
scoreArray1[62][4] = 0;
scoreArray1[63][4] = 4;
scoreArray1[64][4] = 4;
scoreArray1[65][4] = 0;
scoreArray1[66][4] = 4;
scoreArray1[67][4] = 8;

scoreArray1[0][5] = G;
scoreArray1[1][5] = 3;
scoreArray1[2][5] = 0;
scoreArray1[3][5] = 4;
scoreArray1[4][5] = 8;
scoreArray1[5][5] = 0;
scoreArray1[6][5] = 4;
scoreArray1[7][5] = 12;

scoreArray1[10][5] = Ash;
scoreArray1[11][5] = 3;
scoreArray1[12][5] = 0;
scoreArray1[13][5] = 4;
scoreArray1[14][5] = 12;
scoreArray1[15][5] = 0;
scoreArray1[16][5] = 4;
scoreArray1[17][5] = 16;

scoreArray1[20][5] = A;
scoreArray1[21][5] = 3;
scoreArray1[22][5] = 0;
scoreArray1[23][5] = 4;
scoreArray1[24][5] = 16;
scoreArray1[25][5] = 0;
scoreArray1[26][5] = 4;
scoreArray1[27][5] = 20;

scoreArray1[30][5] = G;
scoreArray1[31][5] = 3;
scoreArray1[32][5] = 0;
scoreArray1[33][5] = 4;
scoreArray1[34][5] = 20;
scoreArray1[35][5] = 0;
scoreArray1[36][5] = 4;
scoreArray1[37][5] = 24;

scoreArray1[40][5] = Fsh;
scoreArray1[41][5] = 3;
scoreArray1[42][5] = 0;
scoreArray1[43][5] = 4;
scoreArray1[44][5] = 24;
scoreArray1[45][5] = 0;
scoreArray1[46][5] = 4;
scoreArray1[47][5] = 28;

scoreArray1[50][5] = A;
scoreArray1[51][5] = 3;
scoreArray1[52][5] = 0;
scoreArray1[53][5] = 4;
scoreArray1[54][5] = 28;
scoreArray1[55][5] = 0;
scoreArray1[56][5] = 4;
scoreArray1[57][5] = 32;

scoreArray1[60][5] = G;
scoreArray1[61][5] = 3;
scoreArray1[62][5] = 0;
scoreArray1[63][5] = 4;
scoreArray1[64][5] = 32;
scoreArray1[65][5] = 0;
scoreArray1[66][5] = 4;
scoreArray1[67][5] = 36;

scoreArray1[0][6] = D;
scoreArray1[1][6] = 3;
scoreArray1[2][6] = 0;
scoreArray1[3][6] = 4;
scoreArray1[4][6] = 36;
scoreArray1[5][6] = 0;
scoreArray1[6][6] = 4;
scoreArray1[7][6] = 40;

scoreArray1[10][6] = G;
scoreArray1[11][6] = 3;
scoreArray1[12][6] = 0;
scoreArray1[13][6] = 4;
scoreArray1[14][6] = 40;
scoreArray1[15][6] = 0;
scoreArray1[16][6] = 4;
scoreArray1[17][6] = 44;

scoreArray1[20][6] = A;
scoreArray1[21][6] = 3;
scoreArray1[22][6] = 0;
scoreArray1[23][6] = 4;
scoreArray1[24][6] = 44;
scoreArray1[25][6] = 0;
scoreArray1[26][6] = 4;
scoreArray1[27][6] = 48;

scoreArray1[30][6] = Ash;
scoreArray1[31][6] = 3;
scoreArray1[32][6] = 0;
scoreArray1[33][6] = 4;
scoreArray1[34][6] = 48;
scoreArray1[35][6] = 0;
scoreArray1[36][6] = 4;
scoreArray1[37][6] = 52;

scoreArray1[40][6] = C;
scoreArray1[41][6] = 4;
scoreArray1[42][6] = 0;
scoreArray1[43][6] = 4;
scoreArray1[44][6] = 52;
scoreArray1[45][6] = 0;
scoreArray1[46][6] = 4;
scoreArray1[47][6] = 56;

scoreArray1[50][6] = D;
scoreArray1[51][6] = 4;
scoreArray1[52][6] = 0;
scoreArray1[53][6] = 4;
scoreArray1[54][6] = 56;
scoreArray1[55][6] = 0;
scoreArray1[56][6] = 4;
scoreArray1[57][6] = 60;

scoreArray1[60][6] = E;
scoreArray1[61][6] = 4;
scoreArray1[62][6] = 0;
scoreArray1[63][6] = 4;
scoreArray1[64][6] = 60;
scoreArray1[65][6] = 0;
scoreArray1[66][6] = 5;
scoreArray1[67][6] = 0;

scoreArray1[0][7] = F;
scoreArray1[1][7] = 4;
scoreArray1[2][7] = 0;
scoreArray1[3][7] = 5;
scoreArray1[4][7] = 0;
scoreArray1[5][7] = 0;
scoreArray1[6][7] = 5;
scoreArray1[7][7] = 4;

scoreArray1[10][7] = D;
scoreArray1[11][7] = 3;
scoreArray1[12][7] = 0;
scoreArray1[13][7] = 5;
scoreArray1[14][7] = 0;
scoreArray1[15][7] = 0;
scoreArray1[16][7] = 5;
scoreArray1[17][7] = 16;

scoreArray1[20][7] = E;
scoreArray1[21][7] = 4;
scoreArray1[22][7] = 0;
scoreArray1[23][7] = 5;
scoreArray1[24][7] = 4;
scoreArray1[25][7] = 0;
scoreArray1[26][7] = 5;
scoreArray1[27][7] = 8;

scoreArray1[30][7] = D;
scoreArray1[31][7] = 4;
scoreArray1[32][7] = 0;
scoreArray1[33][7] = 5;
scoreArray1[34][7] = 8;
scoreArray1[35][7] = 0;
scoreArray1[36][7] = 5;
scoreArray1[37][7] = 12;

scoreArray1[40][7] = F;
scoreArray1[41][7] = 4;
scoreArray1[42][7] = 0;
scoreArray1[43][7] = 5;
scoreArray1[44][7] = 12;
scoreArray1[45][7] = 0;
scoreArray1[46][7] = 5;
scoreArray1[47][7] = 16;

scoreArray1[50][7] = E;
scoreArray1[51][7] = 4;
scoreArray1[52][7] = 0;
scoreArray1[53][7] = 5;
scoreArray1[54][7] = 16;
scoreArray1[55][7] = 0;
scoreArray1[56][7] = 5;
scoreArray1[57][7] = 20;

scoreArray1[60][7] = A;
scoreArray1[61][7] = 3;
scoreArray1[62][7] = 0;
scoreArray1[63][7] = 5;
scoreArray1[64][7] = 16;
scoreArray1[65][7] = 0;
scoreArray1[66][7] = 5;
scoreArray1[67][7] = 32;

scoreArray1[0][8] = D;
scoreArray1[1][8] = 4;
scoreArray1[2][8] = 0;
scoreArray1[3][8] = 5;
scoreArray1[4][8] = 20;
scoreArray1[5][8] = 0;
scoreArray1[6][8] = 5;
scoreArray1[7][8] = 24;

scoreArray1[10][8] = Csh;
scoreArray1[11][8] = 4;
scoreArray1[12][8] = 0;
scoreArray1[13][8] = 5;
scoreArray1[14][8] = 24;
scoreArray1[15][8] = 0;
scoreArray1[16][8] = 5;
scoreArray1[17][8] = 28;

scoreArray1[20][8] = E;
scoreArray1[21][8] = 4;
scoreArray1[22][8] = 0;
scoreArray1[23][8] = 5;
scoreArray1[24][8] = 28;
scoreArray1[25][8] = 0;
scoreArray1[26][8] = 5;
scoreArray1[27][8] = 32;

scoreArray1[30][8] = D;
scoreArray1[31][8] = 4;
scoreArray1[32][8] = 0;
scoreArray1[33][8] = 5;
scoreArray1[34][8] = 32;
scoreArray1[35][8] = 0;
scoreArray1[36][8] = 5;
scoreArray1[37][8] = 40;

scoreArray1[40][8] = F;
scoreArray1[41][8] = 3;
scoreArray1[42][8] = 0;
scoreArray1[43][8] = 5;
scoreArray1[44][8] = 32;
scoreArray1[45][8] = 0;
scoreArray1[46][8] = 5;
scoreArray1[47][8] = 56;

scoreArray1[50][8] = A;
scoreArray1[51][8] = 3;
scoreArray1[52][8] = 0;
scoreArray1[53][8] = 5;
scoreArray1[54][8] = 40;
scoreArray1[55][8] = 0;
scoreArray1[56][8] = 5;
scoreArray1[57][8] = 48;

scoreArray1[60][8] = D;
scoreArray1[61][8] = 4;
scoreArray1[62][8] = 0;
scoreArray1[63][8] = 5;
scoreArray1[64][8] = 48;
scoreArray1[65][8] = 0;
scoreArray1[66][8] = 5;
scoreArray1[67][8] = 56;

scoreArray1[0][9] = E;
scoreArray1[1][9] = 4;
scoreArray1[2][9] = 0;
scoreArray1[3][9] = 5;
scoreArray1[4][9] = 56;
scoreArray1[5][9] = 0;
scoreArray1[6][9] = 6;
scoreArray1[7][9] = 0;

scoreArray1[10][9] = E;
scoreArray1[11][9] = 3;
scoreArray1[12][9] = 0;
scoreArray1[13][9] = 5;
scoreArray1[14][9] = 56;
scoreArray1[15][9] = 0;
scoreArray1[16][9] = 6;
scoreArray1[17][9] = 0;

scoreArray1[20][9] = F;
scoreArray1[21][9] = 4;
scoreArray1[22][9] = 0;
scoreArray1[23][9] = 6;
scoreArray1[24][9] = 0;
scoreArray1[25][9] = 0;
scoreArray1[26][9] = 6;
scoreArray1[27][9] = 4;

scoreArray1[30][9] = D;
scoreArray1[31][9] = 3;
scoreArray1[32][9] = 0;
scoreArray1[33][9] = 6;
scoreArray1[34][9] = 0;
scoreArray1[35][9] = 0;
scoreArray1[36][9] = 6;
scoreArray1[37][9] = 8;

scoreArray1[40][9] = G;
scoreArray1[41][9] = 4;
scoreArray1[42][9] = 0;
scoreArray1[43][9] = 6;
scoreArray1[44][9] = 4;
scoreArray1[45][9] = 0;
scoreArray1[46][9] = 6;
scoreArray1[47][9] = 8;

scoreArray1[50][9] = F;
scoreArray1[51][9] = 4;
scoreArray1[52][9] = 0;
scoreArray1[53][9] = 6;
scoreArray1[54][9] = 8;
scoreArray1[55][9] = 0;
scoreArray1[56][9] = 6;
scoreArray1[57][9] = 12;

scoreArray1[60][9] = F;
scoreArray1[61][9] = 3;
scoreArray1[62][9] = 0;
scoreArray1[63][9] = 6;
scoreArray1[64][9] = 8;
scoreArray1[65][9] = 0;
scoreArray1[66][9] = 6;
scoreArray1[67][9] = 16;

scoreArray1[0][10] = G;
scoreArray1[1][10] = 4;
scoreArray1[2][10] = 0;
scoreArray1[3][10] = 6;
scoreArray1[4][10] = 12;
scoreArray1[5][10] = 0;
scoreArray1[6][10] = 6;
scoreArray1[7][10] = 16;

scoreArray1[10][10] = G;
scoreArray1[11][10] = 4;
scoreArray1[12][10] = 0;
scoreArray1[13][10] = 6;
scoreArray1[14][10] = 16;
scoreArray1[15][10] = 0;
scoreArray1[16][10] = 6;
scoreArray1[17][10] = 18;

scoreArray1[20][10] = E;
scoreArray1[21][10] = 3;
scoreArray1[22][10] = 0;
scoreArray1[23][10] = 6;
scoreArray1[24][10] = 16;
scoreArray1[25][10] = 0;
scoreArray1[26][10] = 6;
scoreArray1[27][10] = 24;

scoreArray1[30][10] = A;
scoreArray1[31][10] = 4;
scoreArray1[32][10] = 0;
scoreArray1[33][10] = 6;
scoreArray1[34][10] = 18;
scoreArray1[35][10] = 0;
scoreArray1[36][10] = 6;
scoreArray1[37][10] = 20;

scoreArray1[40][10] = G;
scoreArray1[41][10] = 4;
scoreArray1[42][10] = 0;
scoreArray1[43][10] = 6;
scoreArray1[44][10] = 20;
scoreArray1[45][10] = 0;
scoreArray1[46][10] = 6;
scoreArray1[47][10] = 22;

scoreArray1[50][10] = A;
scoreArray1[51][10] = 4;
scoreArray1[52][10] = 0;
scoreArray1[53][10] = 6;
scoreArray1[54][10] = 22;
scoreArray1[55][10] = 0;
scoreArray1[56][10] = 6;
scoreArray1[57][10] = 24;

scoreArray1[60][10] = G;
scoreArray1[61][10] = 4;
scoreArray1[62][10] = 0;
scoreArray1[63][10] = 6;
scoreArray1[64][10] = 24;
scoreArray1[65][10] = 0;
scoreArray1[66][10] = 6;
scoreArray1[67][10] = 26;

scoreArray1[0][11] = D;
scoreArray1[1][11] = 3;
scoreArray1[2][11] = 0;
scoreArray1[3][11] = 6;
scoreArray1[4][11] = 24;
scoreArray1[5][11] = 0;
scoreArray1[6][11] = 6;
scoreArray1[7][11] = 32;

scoreArray1[10][11] = A;
scoreArray1[11][11] = 4;
scoreArray1[12][11] = 0;
scoreArray1[13][11] = 6;
scoreArray1[14][11] = 26;
scoreArray1[15][11] = 0;
scoreArray1[16][11] = 6;
scoreArray1[17][11] = 28;

scoreArray1[20][11] = F;
scoreArray1[21][11] = 4;
scoreArray1[22][11] = 0;
scoreArray1[23][11] = 6;
scoreArray1[24][11] = 28;
scoreArray1[25][11] = 0;
scoreArray1[26][11] = 6;
scoreArray1[27][11] = 30;

scoreArray1[30][11] = G;
scoreArray1[31][11] = 4;
scoreArray1[32][11] = 0;
scoreArray1[33][11] = 6;
scoreArray1[34][11] = 30;
scoreArray1[35][11] = 0;
scoreArray1[36][11] = 6;
scoreArray1[37][11] = 32;

scoreArray1[40][11] = A;
scoreArray1[41][11] = 4;
scoreArray1[42][11] = 0;
scoreArray1[43][11] = 6;
scoreArray1[44][11] = 32;
scoreArray1[45][11] = 0;
scoreArray1[46][11] = 6;
scoreArray1[47][11] = 36;

scoreArray1[50][11] = Csh;
scoreArray1[51][11] = 3;
scoreArray1[52][11] = 0;
scoreArray1[53][11] = 6;
scoreArray1[54][11] = 32;
scoreArray1[55][11] = 0;
scoreArray1[56][11] = 6;
scoreArray1[57][11] = 40;

scoreArray1[60][11] = G;
scoreArray1[61][11] = 4;
scoreArray1[62][11] = 0;
scoreArray1[63][11] = 6;
scoreArray1[64][11] = 36;
scoreArray1[65][11] = 0;
scoreArray1[66][11] = 6;
scoreArray1[67][11] = 40;

scoreArray1[0][12] = A;
scoreArray1[1][12] = 4;
scoreArray1[2][12] = 0;
scoreArray1[3][12] = 6;
scoreArray1[4][12] = 40;
scoreArray1[5][12] = 0;
scoreArray1[6][12] = 6;
scoreArray1[7][12] = 44;

scoreArray1[10][12] = E;
scoreArray1[11][12] = 3;
scoreArray1[12][12] = 0;
scoreArray1[13][12] = 6;
scoreArray1[14][12] = 40;
scoreArray1[15][12] = 0;
scoreArray1[16][12] = 6;
scoreArray1[17][12] = 48;

scoreArray1[20][12] = Ash;
scoreArray1[21][12] = 4;
scoreArray1[22][12] = 0;
scoreArray1[23][12] = 6;
scoreArray1[24][12] = 44;
scoreArray1[25][12] = 0;
scoreArray1[26][12] = 6;
scoreArray1[27][12] = 48;

scoreArray1[30][12] = A;
scoreArray1[31][12] = 4;
scoreArray1[32][12] = 0;
scoreArray1[33][12] = 6;
scoreArray1[34][12] = 48;
scoreArray1[35][12] = 0;
scoreArray1[36][12] = 6;
scoreArray1[37][12] = 52;

scoreArray1[40][12] = A;
scoreArray1[41][12] = 2;
scoreArray1[42][12] = 0;
scoreArray1[43][12] = 6;
scoreArray1[44][12] = 48;
scoreArray1[45][12] = 0;
scoreArray1[46][12] = 7;
scoreArray1[47][12] = 0;

scoreArray1[50][12] = G;
scoreArray1[51][12] = 4;
scoreArray1[52][12] = 0;
scoreArray1[53][12] = 6;
scoreArray1[54][12] = 52;
scoreArray1[55][12] = 0;
scoreArray1[56][12] = 6;
scoreArray1[57][12] = 56;

scoreArray1[60][12] = F;
scoreArray1[61][12] = 4;
scoreArray1[62][12] = 0;
scoreArray1[63][12] = 6;
scoreArray1[64][12] = 56;
scoreArray1[65][12] = 0;
scoreArray1[66][12] = 6;
scoreArray1[67][12] = 60;

scoreArray1[0][13] = E;
scoreArray1[1][13] = 4;
scoreArray1[2][13] = 0;
scoreArray1[3][13] = 6;
scoreArray1[4][13] = 60;
scoreArray1[5][13] = 0;
scoreArray1[6][13] = 7;
scoreArray1[7][13] = 0;

scoreArray1[10][13] = F;
scoreArray1[11][13] = 4;
scoreArray1[12][13] = 0;
scoreArray1[13][13] = 7;
scoreArray1[14][13] = 0;
scoreArray1[15][13] = 0;
scoreArray1[16][13] = 7;
scoreArray1[17][13] = 4;

scoreArray1[20][13] = D;
scoreArray1[21][13] = 3;
scoreArray1[22][13] = 0;
scoreArray1[23][13] = 7;
scoreArray1[24][13] = 0;
scoreArray1[25][13] = 0;
scoreArray1[26][13] = 7;
scoreArray1[27][13] = 8;

scoreArray1[30][13] = A;
scoreArray1[31][13] = 4;
scoreArray1[32][13] = 0;
scoreArray1[33][13] = 7;
scoreArray1[34][13] = 4;
scoreArray1[35][13] = 0;
scoreArray1[36][13] = 7;
scoreArray1[37][13] = 8;

scoreArray1[40][13] = G;
scoreArray1[41][13] = 4;
scoreArray1[42][13] = 0;
scoreArray1[43][13] = 7;
scoreArray1[44][13] = 8;
scoreArray1[45][13] = 0;
scoreArray1[46][13] = 7;
scoreArray1[47][13] = 12;

scoreArray1[50][13] = A;
scoreArray1[51][13] = 2;
scoreArray1[52][13] = 0;
scoreArray1[53][13] = 7;
scoreArray1[54][13] = 8;
scoreArray1[55][13] = 0;
scoreArray1[56][13] = 7;
scoreArray1[57][13] = 16;

scoreArray1[60][13] = A;
scoreArray1[61][13] = 4;
scoreArray1[62][13] = 0;
scoreArray1[63][13] = 7;
scoreArray1[64][13] = 12;
scoreArray1[65][13] = 0;
scoreArray1[66][13] = 7;
scoreArray1[67][13] = 16;

scoreArray1[0][14] = Csh;
scoreArray1[1][14] = 4;
scoreArray1[2][14] = 0;
scoreArray1[3][14] = 7;
scoreArray1[4][14] = 16;
scoreArray1[5][14] = 0;
scoreArray1[6][14] = 7;
scoreArray1[7][14] = 20;

scoreArray1[10][14] = E;
scoreArray1[11][14] = 3;
scoreArray1[12][14] = 0;
scoreArray1[13][14] = 7;
scoreArray1[14][14] = 16;
scoreArray1[15][14] = 0;
scoreArray1[16][14] = 7;
scoreArray1[17][14] = 24;

scoreArray1[20][14] = A;
scoreArray1[21][14] = 4;
scoreArray1[22][14] = 0;
scoreArray1[23][14] = 7;
scoreArray1[24][14] = 20;
scoreArray1[25][14] = 0;
scoreArray1[26][14] = 7;
scoreArray1[27][14] = 24;

scoreArray1[30][14] = G;
scoreArray1[31][14] = 4;
scoreArray1[32][14] = 0;
scoreArray1[33][14] = 7;
scoreArray1[34][14] = 24;
scoreArray1[35][14] = 0;
scoreArray1[36][14] = 7;
scoreArray1[37][14] = 28;

scoreArray1[40][14] = A;
scoreArray1[41][14] = 2;
scoreArray1[42][14] = 0;
scoreArray1[43][14] = 7;
scoreArray1[44][14] = 24;
scoreArray1[45][14] = 0;
scoreArray1[46][14] = 7;
scoreArray1[47][14] = 32;

scoreArray1[50][14] = A;
scoreArray1[51][14] = 4;
scoreArray1[52][14] = 0;
scoreArray1[53][14] = 7;
scoreArray1[54][14] = 28;
scoreArray1[55][14] = 0;
scoreArray1[56][14] = 7;
scoreArray1[57][14] = 32;

scoreArray1[60][14] = D;
scoreArray1[61][14] = 4;
scoreArray1[62][14] = 0;
scoreArray1[63][14] = 7;
scoreArray1[64][14] = 32;
scoreArray1[65][14] = 0;
scoreArray1[66][14] = 7;
scoreArray1[67][14] = 36;

scoreArray1[0][15] = F;
scoreArray1[1][15] = 3;
scoreArray1[2][15] = 0;
scoreArray1[3][15] = 7;
scoreArray1[4][15] = 32;
scoreArray1[5][15] = 0;
scoreArray1[6][15] = 7;
scoreArray1[7][15] = 40;

scoreArray1[10][15] = A;
scoreArray1[11][15] = 4;
scoreArray1[12][15] = 0;
scoreArray1[13][15] = 7;
scoreArray1[14][15] = 36;
scoreArray1[15][15] = 0;
scoreArray1[16][15] = 7;
scoreArray1[17][15] = 40;

scoreArray1[20][15] = G;
scoreArray1[21][15] = 4;
scoreArray1[22][15] = 0;
scoreArray1[23][15] = 7;
scoreArray1[24][15] = 40;
scoreArray1[25][15] = 0;
scoreArray1[26][15] = 7;
scoreArray1[27][15] = 44;

scoreArray1[30][15] = E;
scoreArray1[31][15] = 3;
scoreArray1[32][15] = 0;
scoreArray1[33][15] = 7;
scoreArray1[34][15] = 40;
scoreArray1[35][15] = 0;
scoreArray1[36][15] = 7;
scoreArray1[37][15] = 44;

scoreArray1[40][15] = A;
scoreArray1[41][15] = 4;
scoreArray1[42][15] = 0;
scoreArray1[43][15] = 7;
scoreArray1[44][15] = 44;
scoreArray1[45][15] = 0;
scoreArray1[46][15] = 7;
scoreArray1[47][15] = 48;

scoreArray1[50][15] = D;
scoreArray1[51][15] = 3;
scoreArray1[52][15] = 0;
scoreArray1[53][15] = 7;
scoreArray1[54][15] = 44;
scoreArray1[55][15] = 0;
scoreArray1[56][15] = 7;
scoreArray1[57][15] = 48;

scoreArray1[60][15] = Csh;
scoreArray1[61][15] = 4;
scoreArray1[62][15] = 0;
scoreArray1[63][15] = 7;
scoreArray1[64][15] = 48;
scoreArray1[65][15] = 0;
scoreArray1[66][15] = 7;
scoreArray1[67][15] = 52;

scoreArray1[0][16] = E;
scoreArray1[1][16] = 3;
scoreArray1[2][16] = 0;
scoreArray1[3][16] = 7;
scoreArray1[4][16] = 48;
scoreArray1[5][16] = 0;
scoreArray1[6][16] = 7;
scoreArray1[7][16] = 56;

scoreArray1[10][16] = A;
scoreArray1[11][16] = 4;
scoreArray1[12][16] = 0;
scoreArray1[13][16] = 7;
scoreArray1[14][16] = 52;
scoreArray1[15][16] = 0;
scoreArray1[16][16] = 7;
scoreArray1[17][16] = 56;

scoreArray1[20][16] = G;
scoreArray1[21][16] = 4;
scoreArray1[22][16] = 0;
scoreArray1[23][16] = 7;
scoreArray1[24][16] = 56;
scoreArray1[25][16] = 0;
scoreArray1[26][16] = 7;
scoreArray1[27][16] = 60;

scoreArray1[30][16] = A;
scoreArray1[31][16] = 2;
scoreArray1[32][16] = 0;
scoreArray1[33][16] = 7;
scoreArray1[34][16] = 56;
scoreArray1[35][16] = 0;
scoreArray1[36][16] = 8;
scoreArray1[37][16] = 0;

scoreArray1[40][16] = A;
scoreArray1[41][16] = 4;
scoreArray1[42][16] = 0;
scoreArray1[43][16] = 7;
scoreArray1[44][16] = 60;
scoreArray1[45][16] = 0;
scoreArray1[46][16] = 8;
scoreArray1[47][16] = 0;

scoreArray1[50][16] = F;
scoreArray1[51][16] = 4;
scoreArray1[52][16] = 0;
scoreArray1[53][16] = 8;
scoreArray1[54][16] = 0;
scoreArray1[55][16] = 0;
scoreArray1[56][16] = 8;
scoreArray1[57][16] = 4;

scoreArray1[60][16] = D;
scoreArray1[61][16] = 3;
scoreArray1[62][16] = 0;
scoreArray1[63][16] = 8;
scoreArray1[64][16] = 0;
scoreArray1[65][16] = 0;
scoreArray1[66][16] = 8;
scoreArray1[67][16] = 8;

scoreArray1[0][17] = D;
scoreArray1[1][17] = 4;
scoreArray1[2][17] = 0;
scoreArray1[3][17] = 8;
scoreArray1[4][17] = 4;
scoreArray1[5][17] = 0;
scoreArray1[6][17] = 8;
scoreArray1[7][17] = 8;

scoreArray1[10][17] = Csh;
scoreArray1[11][17] = 4;
scoreArray1[12][17] = 0;
scoreArray1[13][17] = 8;
scoreArray1[14][17] = 8;
scoreArray1[15][17] = 0;
scoreArray1[16][17] = 8;
scoreArray1[17][17] = 12;

scoreArray1[20][17] = A;
scoreArray1[21][17] = 2;
scoreArray1[22][17] = 0;
scoreArray1[23][17] = 8;
scoreArray1[24][17] = 8;
scoreArray1[25][17] = 0;
scoreArray1[26][17] = 8;
scoreArray1[27][17] = 12;

scoreArray1[30][17] = D;
scoreArray1[31][17] = 4;
scoreArray1[32][17] = 0;
scoreArray1[33][17] = 8;
scoreArray1[34][17] = 12;
scoreArray1[35][17] = 0;
scoreArray1[36][17] = 8;
scoreArray1[37][17] = 16;

scoreArray1[40][17] = D;
scoreArray1[41][17] = 3;
scoreArray1[42][17] = 0;
scoreArray1[43][17] = 8;
scoreArray1[44][17] = 12;
scoreArray1[45][17] = 0;
scoreArray1[46][17] = 8;
scoreArray1[47][17] = 16;

scoreArray1[50][17] = G;
scoreArray1[51][17] = 4;
scoreArray1[52][17] = 0;
scoreArray1[53][17] = 8;
scoreArray1[54][17] = 16;
scoreArray1[55][17] = 0;
scoreArray1[56][17] = 8;
scoreArray1[57][17] = 20;

scoreArray1[60][17] = E;
scoreArray1[61][17] = 3;
scoreArray1[62][17] = 0;
scoreArray1[63][17] = 8;
scoreArray1[64][17] = 16;
scoreArray1[65][17] = 0;
scoreArray1[66][17] = 8;
scoreArray1[67][17] = 24;

scoreArray1[0][18] = D;
scoreArray1[1][18] = 4;
scoreArray1[2][18] = 0;
scoreArray1[3][18] = 8;
scoreArray1[4][18] = 20;
scoreArray1[5][18] = 0;
scoreArray1[6][18] = 8;
scoreArray1[7][18] = 24;

scoreArray1[10][18] = Csh;
scoreArray1[11][18] = 4;
scoreArray1[12][18] = 0;
scoreArray1[13][18] = 8;
scoreArray1[14][18] = 24;
scoreArray1[15][18] = 0;
scoreArray1[16][18] = 8;
scoreArray1[17][18] = 28;

scoreArray1[20][18] = A;
scoreArray1[21][18] = 2;
scoreArray1[22][18] = 0;
scoreArray1[23][18] = 8;
scoreArray1[24][18] = 24;
scoreArray1[25][18] = 0;
scoreArray1[26][18] = 8;
scoreArray1[27][18] = 28;

scoreArray1[30][18] = D;
scoreArray1[31][18] = 4;
scoreArray1[32][18] = 0;
scoreArray1[33][18] = 8;
scoreArray1[34][18] = 28;
scoreArray1[35][18] = 0;
scoreArray1[36][18] = 8;
scoreArray1[37][18] = 32;

scoreArray1[40][18] = E;
scoreArray1[41][18] = 3;
scoreArray1[42][18] = 0;
scoreArray1[43][18] = 8;
scoreArray1[44][18] = 28;
scoreArray1[45][18] = 0;
scoreArray1[46][18] = 8;
scoreArray1[47][18] = 32;

scoreArray1[50][18] = A;
scoreArray1[51][18] = 4;
scoreArray1[52][18] = 0;
scoreArray1[53][18] = 8;
scoreArray1[54][18] = 32;
scoreArray1[55][18] = 0;
scoreArray1[56][18] = 8;
scoreArray1[57][18] = 36;

scoreArray1[60][18] = F;
scoreArray1[61][18] = 3;
scoreArray1[62][18] = 0;
scoreArray1[63][18] = 8;
scoreArray1[64][18] = 32;
scoreArray1[65][18] = 0;
scoreArray1[66][18] = 8;
scoreArray1[67][18] = 40;

scoreArray1[0][19] = D;
scoreArray1[1][19] = 4;
scoreArray1[2][19] = 0;
scoreArray1[3][19] = 8;
scoreArray1[4][19] = 36;
scoreArray1[5][19] = 0;
scoreArray1[6][19] = 8;
scoreArray1[7][19] = 40;

scoreArray1[10][19] = Csh;
scoreArray1[11][19] = 4;
scoreArray1[12][19] = 0;
scoreArray1[13][19] = 8;
scoreArray1[14][19] = 40;
scoreArray1[15][19] = 0;
scoreArray1[16][19] = 8;
scoreArray1[17][19] = 44;

scoreArray1[20][19] = E;
scoreArray1[21][19] = 3;
scoreArray1[22][19] = 0;
scoreArray1[23][19] = 8;
scoreArray1[24][19] = 40;
scoreArray1[25][19] = 0;
scoreArray1[26][19] = 8;
scoreArray1[27][19] = 44;

scoreArray1[30][19] = D;
scoreArray1[31][19] = 4;
scoreArray1[32][19] = 0;
scoreArray1[33][19] = 8;
scoreArray1[34][19] = 44;
scoreArray1[35][19] = 0;
scoreArray1[36][19] = 8;
scoreArray1[37][19] = 48;

scoreArray1[40][19] = D;
scoreArray1[41][19] = 3;
scoreArray1[42][19] = 0;
scoreArray1[43][19] = 8;
scoreArray1[44][19] = 44;
scoreArray1[45][19] = 0;
scoreArray1[46][19] = 8;
scoreArray1[47][19] = 48;

scoreArray1[50][19] = G;
scoreArray1[51][19] = 4;
scoreArray1[52][19] = 0;
scoreArray1[53][19] = 8;
scoreArray1[54][19] = 48;
scoreArray1[55][19] = 0;
scoreArray1[56][19] = 8;
scoreArray1[57][19] = 52;

scoreArray1[60][19] = E;
scoreArray1[61][19] = 3;
scoreArray1[62][19] = 0;
scoreArray1[63][19] = 8;
scoreArray1[64][19] = 48;
scoreArray1[65][19] = 0;
scoreArray1[66][19] = 8;
scoreArray1[67][19] = 52;

scoreArray1[0][20] = D;
scoreArray1[1][20] = 4;
scoreArray1[2][20] = 0;
scoreArray1[3][20] = 8;
scoreArray1[4][20] = 52;
scoreArray1[5][20] = 0;
scoreArray1[6][20] = 8;
scoreArray1[7][20] = 56;

scoreArray1[10][20] = A;
scoreArray1[11][20] = 2;
scoreArray1[12][20] = 0;
scoreArray1[13][20] = 8;
scoreArray1[14][20] = 52;
scoreArray1[15][20] = 0;
scoreArray1[16][20] = 8;
scoreArray1[17][20] = 56;

scoreArray1[20][20] = Csh;
scoreArray1[21][20] = 4;
scoreArray1[22][20] = 0;
scoreArray1[23][20] = 8;
scoreArray1[24][20] = 56;
scoreArray1[25][20] = 0;
scoreArray1[26][20] = 8;
scoreArray1[27][20] = 60;

scoreArray1[30][20] = A;
scoreArray1[31][20] = 3;
scoreArray1[32][20] = 0;
scoreArray1[33][20] = 8;
scoreArray1[34][20] = 56;
scoreArray1[35][20] = 0;
scoreArray1[36][20] = 8;
scoreArray1[37][20] = 60;

scoreArray1[40][20] = D;
scoreArray1[41][20] = 4;
scoreArray1[42][20] = 0;
scoreArray1[43][20] = 8;
scoreArray1[44][20] = 60;
scoreArray1[45][20] = 0;
scoreArray1[46][20] = 9;
scoreArray1[47][20] = 0;

scoreArray1[50][20] = G;
scoreArray1[51][20] = 3;
scoreArray1[52][20] = 0;
scoreArray1[53][20] = 8;
scoreArray1[54][20] = 60;
scoreArray1[55][20] = 0;
scoreArray1[56][20] = 9;
scoreArray1[57][20] = 0;

scoreArray1[60][20] = A;
scoreArray1[61][20] = 3;
scoreArray1[62][20] = 0;
scoreArray1[63][20] = 9;
scoreArray1[64][20] = 0;
scoreArray1[65][20] = 0;
scoreArray1[66][20] = 9;
scoreArray1[67][20] = 8;

scoreArray1[0][21] = F;
scoreArray1[1][21] = 3;
scoreArray1[2][21] = 0;
scoreArray1[3][21] = 9;
scoreArray1[4][21] = 0;
scoreArray1[5][21] = 0;
scoreArray1[6][21] = 9;
scoreArray1[7][21] = 4;

scoreArray1[10][21] = E;
scoreArray1[11][21] = 3;
scoreArray1[12][21] = 0;
scoreArray1[13][21] = 9;
scoreArray1[14][21] = 4;
scoreArray1[15][21] = 0;
scoreArray1[16][21] = 9;
scoreArray1[17][21] = 8;

scoreArray1[20][21] = F;
scoreArray1[21][21] = 4;
scoreArray1[22][21] = 0;
scoreArray1[23][21] = 9;
scoreArray1[24][21] = 8;
scoreArray1[25][21] = 0;
scoreArray1[26][21] = 9;
scoreArray1[27][21] = 16;

scoreArray1[30][21] = D;
scoreArray1[31][21] = 3;
scoreArray1[32][21] = 0;
scoreArray1[33][21] = 9;
scoreArray1[34][21] = 8;
scoreArray1[35][21] = 0;
scoreArray1[36][21] = 9;
scoreArray1[37][21] = 12;

scoreArray1[40][21] = F;
scoreArray1[41][21] = 3;
scoreArray1[42][21] = 0;
scoreArray1[43][21] = 9;
scoreArray1[44][21] = 12;
scoreArray1[45][21] = 0;
scoreArray1[46][21] = 9;
scoreArray1[47][21] = 16;

scoreArray1[50][21] = G;
scoreArray1[51][21] = 3;
scoreArray1[52][21] = 0;
scoreArray1[53][21] = 9;
scoreArray1[54][21] = 16;
scoreArray1[55][21] = 0;
scoreArray1[56][21] = 9;
scoreArray1[57][21] = 24;

scoreArray1[60][21] = E;
scoreArray1[61][21] = 3;
scoreArray1[62][21] = 0;
scoreArray1[63][21] = 9;
scoreArray1[64][21] = 16;
scoreArray1[65][21] = 0;
scoreArray1[66][21] = 9;
scoreArray1[67][21] = 20;

scoreArray1[0][22] = D;
scoreArray1[1][22] = 3;
scoreArray1[2][22] = 0;
scoreArray1[3][22] = 9;
scoreArray1[4][22] = 20;
scoreArray1[5][22] = 0;
scoreArray1[6][22] = 9;
scoreArray1[7][22] = 24;

scoreArray1[10][22] = E;
scoreArray1[11][22] = 4;
scoreArray1[12][22] = 0;
scoreArray1[13][22] = 9;
scoreArray1[14][22] = 24;
scoreArray1[15][22] = 0;
scoreArray1[16][22] = 9;
scoreArray1[17][22] = 32;

scoreArray1[20][22] = Csh;
scoreArray1[21][22] = 3;
scoreArray1[22][22] = 0;
scoreArray1[23][22] = 9;
scoreArray1[24][22] = 24;
scoreArray1[25][22] = 0;
scoreArray1[26][22] = 9;
scoreArray1[27][22] = 28;

scoreArray1[30][22] = E;
scoreArray1[31][22] = 3;
scoreArray1[32][22] = 0;
scoreArray1[33][22] = 9;
scoreArray1[34][22] = 28;
scoreArray1[35][22] = 0;
scoreArray1[36][22] = 9;
scoreArray1[37][22] = 32;

scoreArray1[40][22] = F;
scoreArray1[41][22] = 3;
scoreArray1[42][22] = 0;
scoreArray1[43][22] = 9;
scoreArray1[44][22] = 32;
scoreArray1[45][22] = 0;
scoreArray1[46][22] = 9;
scoreArray1[47][22] = 40;

scoreArray1[50][22] = D;
scoreArray1[51][22] = 3;
scoreArray1[52][22] = 0;
scoreArray1[53][22] = 9;
scoreArray1[54][22] = 32;
scoreArray1[55][22] = 0;
scoreArray1[56][22] = 9;
scoreArray1[57][22] = 36;

scoreArray1[60][22] = A;
scoreArray1[61][22] = 2;
scoreArray1[62][22] = 0;
scoreArray1[63][22] = 9;
scoreArray1[64][22] = 36;
scoreArray1[65][22] = 0;
scoreArray1[66][22] = 9;
scoreArray1[67][22] = 40;

scoreArray1[0][23] = A;
scoreArray1[1][23] = 3;
scoreArray1[2][23] = 0;
scoreArray1[3][23] = 9;
scoreArray1[4][23] = 40;
scoreArray1[5][23] = 0;
scoreArray1[6][23] = 9;
scoreArray1[7][23] = 48;

scoreArray1[10][23] = D;
scoreArray1[11][23] = 3;
scoreArray1[12][23] = 0;
scoreArray1[13][23] = 9;
scoreArray1[14][23] = 40;
scoreArray1[15][23] = 0;
scoreArray1[16][23] = 9;
scoreArray1[17][23] = 44;

scoreArray1[20][23] = E;
scoreArray1[21][23] = 3;
scoreArray1[22][23] = 0;
scoreArray1[23][23] = 9;
scoreArray1[24][23] = 44;
scoreArray1[25][23] = 0;
scoreArray1[26][23] = 9;
scoreArray1[27][23] = 48;

scoreArray1[30][23] = D;
scoreArray1[31][23] = 4;
scoreArray1[32][23] = 0;
scoreArray1[33][23] = 9;
scoreArray1[34][23] = 48;
scoreArray1[35][23] = 0;
scoreArray1[36][23] = 9;
scoreArray1[37][23] = 56;

scoreArray1[40][23] = F;
scoreArray1[41][23] = 3;
scoreArray1[42][23] = 0;
scoreArray1[43][23] = 9;
scoreArray1[44][23] = 48;
scoreArray1[45][23] = 0;
scoreArray1[46][23] = 9;
scoreArray1[47][23] = 52;

scoreArray1[50][23] = G;
scoreArray1[51][23] = 3;
scoreArray1[52][23] = 0;
scoreArray1[53][23] = 9;
scoreArray1[54][23] = 52;
scoreArray1[55][23] = 0;
scoreArray1[56][23] = 9;
scoreArray1[57][23] = 56;

scoreArray1[60][23] = F;
scoreArray1[61][23] = 4;
scoreArray1[62][23] = 0;
scoreArray1[63][23] = 9;
scoreArray1[64][23] = 56;
scoreArray1[65][23] = 0;
scoreArray1[66][23] = 10;
scoreArray1[67][23] = 0;

scoreArray1[0][24] = A;
scoreArray1[1][24] = 3;
scoreArray1[2][24] = 0;
scoreArray1[3][24] = 9;
scoreArray1[4][24] = 56;
scoreArray1[5][24] = 0;
scoreArray1[6][24] = 9;
scoreArray1[7][24] = 60;

scoreArray1[10][24] = B;
scoreArray1[11][24] = 3;
scoreArray1[12][24] = 0;
scoreArray1[13][24] = 9;
scoreArray1[14][24] = 60;
scoreArray1[15][24] = 0;
scoreArray1[16][24] = 10;
scoreArray1[17][24] = 0;

scoreArray1[20][24] = Dsh;
scoreArray1[21][24] = 4;
scoreArray1[22][24] = 0;
scoreArray1[23][24] = 10;
scoreArray1[24][24] = 0;
scoreArray1[25][24] = 0;
scoreArray1[26][24] = 10;
scoreArray1[27][24] = 8;

scoreArray1[30][24] = C;
scoreArray1[31][24] = 4;
scoreArray1[32][24] = 0;
scoreArray1[33][24] = 10;
scoreArray1[34][24] = 0;
scoreArray1[35][24] = 0;
scoreArray1[36][24] = 10;
scoreArray1[37][24] = 4;

scoreArray1[40][24] = Ash;
scoreArray1[41][24] = 3;
scoreArray1[42][24] = 0;
scoreArray1[43][24] = 10;
scoreArray1[44][24] = 4;
scoreArray1[45][24] = 0;
scoreArray1[46][24] = 10;
scoreArray1[47][24] = 8;

scoreArray1[50][24] = A;
scoreArray1[51][24] = 4;
scoreArray1[52][24] = 0;
scoreArray1[53][24] = 10;
scoreArray1[54][24] = 8;
scoreArray1[55][24] = 0;
scoreArray1[56][24] = 10;
scoreArray1[57][24] = 16;

scoreArray1[60][24] = C;
scoreArray1[61][24] = 4;
scoreArray1[62][24] = 0;
scoreArray1[63][24] = 10;
scoreArray1[64][24] = 8;
scoreArray1[65][24] = 0;
scoreArray1[66][24] = 10;
scoreArray1[67][24] = 12;

scoreArray1[0][25] = D;
scoreArray1[1][25] = 4;
scoreArray1[2][25] = 0;
scoreArray1[3][25] = 10;
scoreArray1[4][25] = 12;
scoreArray1[5][25] = 0;
scoreArray1[6][25] = 10;
scoreArray1[7][25] = 16;

scoreArray1[10][25] = C;
scoreArray1[11][25] = 4;
scoreArray1[12][25] = 0;
scoreArray1[13][25] = 10;
scoreArray1[14][25] = 16;
scoreArray1[15][25] = 0;
scoreArray1[16][25] = 10;
scoreArray1[17][25] = 20;

scoreArray1[20][25] = Ash;
scoreArray1[21][25] = 3;
scoreArray1[22][25] = 0;
scoreArray1[23][25] = 10;
scoreArray1[24][25] = 20;
scoreArray1[25][25] = 0;
scoreArray1[26][25] = 10;
scoreArray1[27][25] = 24;

scoreArray1[30][25] = Dsh;
scoreArray1[31][25] = 4;
scoreArray1[32][25] = 0;
scoreArray1[33][25] = 10;
scoreArray1[34][25] = 24;
scoreArray1[35][25] = 0;
scoreArray1[36][25] = 10;
scoreArray1[37][25] = 32;

scoreArray1[40][25] = A;
scoreArray1[41][25] = 3;
scoreArray1[42][25] = 0;
scoreArray1[43][25] = 10;
scoreArray1[44][25] = 24;
scoreArray1[45][25] = 0;
scoreArray1[46][25] = 10;
scoreArray1[47][25] = 28;

scoreArray1[50][25] = C;
scoreArray1[51][25] = 4;
scoreArray1[52][25] = 0;
scoreArray1[53][25] = 10;
scoreArray1[54][25] = 28;
scoreArray1[55][25] = 0;
scoreArray1[56][25] = 10;
scoreArray1[57][25] = 32;

scoreArray1[60][25] = D;
scoreArray1[61][25] = 4;
scoreArray1[62][25] = 0;
scoreArray1[63][25] = 10;
scoreArray1[64][25] = 32;
scoreArray1[65][25] = 0;
scoreArray1[66][25] = 10;
scoreArray1[67][25] = 40;

scoreArray1[0][26] = Ash;
scoreArray1[1][26] = 3;
scoreArray1[2][26] = 0;
scoreArray1[3][26] = 10;
scoreArray1[4][26] = 32;
scoreArray1[5][26] = 0;
scoreArray1[6][26] = 10;
scoreArray1[7][26] = 36;

scoreArray1[10][26] = A;
scoreArray1[11][26] = 3;
scoreArray1[12][26] = 0;
scoreArray1[13][26] = 10;
scoreArray1[14][26] = 36;
scoreArray1[15][26] = 0;
scoreArray1[16][26] = 10;
scoreArray1[17][26] = 40;

scoreArray1[20][26] = G;
scoreArray1[21][26] = 4;
scoreArray1[22][26] = 0;
scoreArray1[23][26] = 10;
scoreArray1[24][26] = 40;
scoreArray1[25][26] = 0;
scoreArray1[26][26] = 10;
scoreArray1[27][26] = 48;

scoreArray1[30][26] = Ash;
scoreArray1[31][26] = 3;
scoreArray1[32][26] = 0;
scoreArray1[33][26] = 10;
scoreArray1[34][26] = 40;
scoreArray1[35][26] = 0;
scoreArray1[36][26] = 10;
scoreArray1[37][26] = 44;

scoreArray1[40][26] = C;
scoreArray1[41][26] = 4;
scoreArray1[42][26] = 0;
scoreArray1[43][26] = 10;
scoreArray1[44][26] = 44;
scoreArray1[45][26] = 0;
scoreArray1[46][26] = 10;
scoreArray1[47][26] = 48;

scoreArray1[50][26] = Ash;
scoreArray1[51][26] = 3;
scoreArray1[52][26] = 0;
scoreArray1[53][26] = 10;
scoreArray1[54][26] = 48;
scoreArray1[55][26] = 0;
scoreArray1[56][26] = 10;
scoreArray1[57][26] = 52;

scoreArray1[60][26] = A;
scoreArray1[61][26] = 3;
scoreArray1[62][26] = 0;
scoreArray1[63][26] = 10;
scoreArray1[64][26] = 52;
scoreArray1[65][26] = 0;
scoreArray1[66][26] = 10;
scoreArray1[67][26] = 56;

scoreArray1[0][27] = D;
scoreArray1[1][27] = 4;
scoreArray1[2][27] = 0;
scoreArray1[3][27] = 10;
scoreArray1[4][27] = 56;
scoreArray1[5][27] = 0;
scoreArray1[6][27] = 11;
scoreArray1[7][27] = 0;

scoreArray1[10][27] = G;
scoreArray1[11][27] = 3;
scoreArray1[12][27] = 0;
scoreArray1[13][27] = 10;
scoreArray1[14][27] = 56;
scoreArray1[15][27] = 0;
scoreArray1[16][27] = 10;
scoreArray1[17][27] = 60;

scoreArray1[20][27] = Ash;
scoreArray1[21][27] = 3;
scoreArray1[22][27] = 0;
scoreArray1[23][27] = 10;
scoreArray1[24][27] = 60;
scoreArray1[25][27] = 0;
scoreArray1[26][27] = 11;
scoreArray1[27][27] = 0;

scoreArray1[30][27] = C;
scoreArray1[31][27] = 4;
scoreArray1[32][27] = 0;
scoreArray1[33][27] = 11;
scoreArray1[34][27] = 0;
scoreArray1[35][27] = 0;
scoreArray1[36][27] = 11;
scoreArray1[37][27] = 4;

scoreArray1[40][27] = A;
scoreArray1[41][27] = 3;
scoreArray1[42][27] = 0;
scoreArray1[43][27] = 11;
scoreArray1[44][27] = 0;
scoreArray1[45][27] = 0;
scoreArray1[46][27] = 11;
scoreArray1[47][27] = 8;

scoreArray1[50][27] = Ash;
scoreArray1[51][27] = 3;
scoreArray1[52][27] = 0;
scoreArray1[53][27] = 11;
scoreArray1[54][27] = 4;
scoreArray1[55][27] = 0;
scoreArray1[56][27] = 11;
scoreArray1[57][27] = 8;

scoreArray1[60][27] = C;
scoreArray1[61][27] = 4;
scoreArray1[62][27] = 0;
scoreArray1[63][27] = 11;
scoreArray1[64][27] = 8;
scoreArray1[65][27] = 0;
scoreArray1[66][27] = 11;
scoreArray1[67][27] = 12;

scoreArray1[0][28] = G;
scoreArray1[1][28] = 3;
scoreArray1[2][28] = 0;
scoreArray1[3][28] = 11;
scoreArray1[4][28] = 8;
scoreArray1[5][28] = 0;
scoreArray1[6][28] = 11;
scoreArray1[7][28] = 16;

scoreArray1[10][28] = D;
scoreArray1[11][28] = 4;
scoreArray1[12][28] = 0;
scoreArray1[13][28] = 11;
scoreArray1[14][28] = 12;
scoreArray1[15][28] = 0;
scoreArray1[16][28] = 11;
scoreArray1[17][28] = 16;

scoreArray1[20][28] = C;
scoreArray1[21][28] = 4;
scoreArray1[22][28] = 0;
scoreArray1[23][28] = 11;
scoreArray1[24][28] = 16;
scoreArray1[25][28] = 0;
scoreArray1[26][28] = 11;
scoreArray1[27][28] = 20;

scoreArray1[30][28] = Fsh;
scoreArray1[31][28] = 3;
scoreArray1[32][28] = 0;
scoreArray1[33][28] = 11;
scoreArray1[34][28] = 16;
scoreArray1[35][28] = 0;
scoreArray1[36][28] = 11;
scoreArray1[37][28] = 24;

scoreArray1[40][28] = A;
scoreArray1[41][28] = 4;
scoreArray1[42][28] = 0;
scoreArray1[43][28] = 11;
scoreArray1[44][28] = 20;
scoreArray1[45][28] = 0;
scoreArray1[46][28] = 11;
scoreArray1[47][28] = 24;

scoreArray1[50][28] = G;
scoreArray1[51][28] = 4;
scoreArray1[52][28] = 0;
scoreArray1[53][28] = 11;
scoreArray1[54][28] = 24;
scoreArray1[55][28] = 0;
scoreArray1[56][28] = 11;
scoreArray1[57][28] = 28;

scoreArray1[60][28] = D;
scoreArray1[61][28] = 3;
scoreArray1[62][28] = 0;
scoreArray1[63][28] = 11;
scoreArray1[64][28] = 24;
scoreArray1[65][28] = 0;
scoreArray1[66][28] = 11;
scoreArray1[67][28] = 32;

scoreArray1[0][29] = A;
scoreArray1[1][29] = 4;
scoreArray1[2][29] = 0;
scoreArray1[3][29] = 11;
scoreArray1[4][29] = 28;
scoreArray1[5][29] = 0;
scoreArray1[6][29] = 11;
scoreArray1[7][29] = 32;

scoreArray1[10][29] = Ash;
scoreArray1[11][29] = 3;
scoreArray1[12][29] = 0;
scoreArray1[13][29] = 11;
scoreArray1[14][29] = 32;
scoreArray1[15][29] = 0;
scoreArray1[16][29] = 11;
scoreArray1[17][29] = 36;

scoreArray1[20][29] = G;
scoreArray1[21][29] = 3;
scoreArray1[22][29] = 0;
scoreArray1[23][29] = 11;
scoreArray1[24][29] = 32;
scoreArray1[25][29] = 0;
scoreArray1[26][29] = 11;
scoreArray1[27][29] = 48;

scoreArray1[30][29] = G;
scoreArray1[31][29] = 2;
scoreArray1[32][29] = 0;
scoreArray1[33][29] = 11;
scoreArray1[34][29] = 32;
scoreArray1[35][29] = 0;
scoreArray1[36][29] = 11;
scoreArray1[37][29] = 48;

scoreArray1[40][29] = G;
scoreArray1[41][29] = 4;
scoreArray1[42][29] = 0;
scoreArray1[43][29] = 11;
scoreArray1[44][29] = 36;
scoreArray1[45][29] = 0;
scoreArray1[46][29] = 11;
scoreArray1[47][29] = 40;

scoreArray1[50][29] = Fsh;
scoreArray1[51][29] = 4;
scoreArray1[52][29] = 0;
scoreArray1[53][29] = 11;
scoreArray1[54][29] = 40;
scoreArray1[55][29] = 0;
scoreArray1[56][29] = 11;
scoreArray1[57][29] = 44;

scoreArray1[60][29] = G;
scoreArray1[61][29] = 4;
scoreArray1[62][29] = 0;
scoreArray1[63][29] = 11;
scoreArray1[64][29] = 44;
scoreArray1[65][29] = 0;
scoreArray1[66][29] = 11;
scoreArray1[67][29] = 48;
    
    int j, k;
    
    for(j=50; j<ARRAY_SIZE_X; j++){
        for(k=29; k<ARRAY_SIZE_Y; k++){
            scoreArray1[j][k] = 0x7FFF;
        }
    }
    
}

int exponentOfTwo(int exponent)
{
    int i;
    int result = 1;
    
    for(i=1; i <= exponent; i++){
        result *= 2;
    }
    
    return result;
}

//scoreArray1[0][0] = C;  // Note
//    scoreArray1[1][0] = 2;  // Pitch
//    scoreArray1[2][0] = 0;  // Measure Start High
//    scoreArray1[3][0] = 1;  // Measure Start Low
//    scoreArray1[4][0] = 0;  // Location Start
//    scoreArray1[5][0] = 0;  // Measure End High
//    scoreArray1[6][0] = 1;  // Measure End Low
//    scoreArray1[7][0] = 16; // Location End
//    
//    scoreArray1[10][0] = C;
//    scoreArray1[11][0] = 2;
//    scoreArray1[12][0] = 0;
//    scoreArray1[13][0] = 1;
//    scoreArray1[14][0] = 16;
//    scoreArray1[15][0] = 0;
//    scoreArray1[16][0] = 1;
//    scoreArray1[17][0] = 32;
//    
//    scoreArray1[20][0] = G;
//    scoreArray1[21][0] = 2;
//    scoreArray1[22][0] = 0;
//    scoreArray1[23][0] = 1;
//    scoreArray1[24][0] = 32;
//    scoreArray1[25][0] = 0;
//    scoreArray1[26][0] = 1;
//    scoreArray1[27][0] = 48;
//    
//    scoreArray1[30][0] = G;
//    scoreArray1[31][0] = 2;
//    scoreArray1[32][0] = 0;
//    scoreArray1[33][0] = 1;
//    scoreArray1[34][0] = 48;
//    scoreArray1[35][0] = 0;
//    scoreArray1[36][0] = 2;
//    scoreArray1[37][0] = 0;
//    
//    scoreArray1[40][0] = A;
//    scoreArray1[41][0] = 2;
//    scoreArray1[42][0] = 0;
//    scoreArray1[43][0] = 2;
//    scoreArray1[44][0] = 0;
//    scoreArray1[45][0] = 0;
//    scoreArray1[46][0] = 2;
//    scoreArray1[47][0] = 16;
//    
//    scoreArray1[50][0] = A;
//    scoreArray1[51][0] = 2;
//    scoreArray1[52][0] = 0;
//    scoreArray1[53][0] = 2;
//    scoreArray1[54][0] = 16;
//    scoreArray1[55][0] = 0;
//    scoreArray1[56][0] = 2;
//    scoreArray1[57][0] = 32;
//    
//    scoreArray1[60][0] = G;
//    scoreArray1[61][0] = 2;
//    scoreArray1[62][0] = 0;
//    scoreArray1[63][0] = 2;
//    scoreArray1[64][0] = 32;
//    scoreArray1[65][0] = 0;
//    scoreArray1[66][0] = 3;
//    scoreArray1[67][0] = 0;
//    
//    scoreArray1[0][1] = F;
//    scoreArray1[1][1] = 2;
//    scoreArray1[2][1] = 0;
//    scoreArray1[3][1] = 3;
//    scoreArray1[4][1] = 0;
//    scoreArray1[5][1] = 0;
//    scoreArray1[6][1] = 3;
//    scoreArray1[7][1] = 16;
//    
//    scoreArray1[10][1] = F;
//    scoreArray1[11][1] = 2;
//    scoreArray1[12][1] = 0;
//    scoreArray1[13][1] = 3;
//    scoreArray1[14][1] = 16;
//    scoreArray1[15][1] = 0;
//    scoreArray1[16][1] = 3;
//    scoreArray1[17][1] = 32;
//    
//    scoreArray1[20][1] = E;
//    scoreArray1[21][1] = 2;
//    scoreArray1[22][1] = 0;
//    scoreArray1[23][1] = 3;
//    scoreArray1[24][1] = 32;
//    scoreArray1[25][1] = 0;
//    scoreArray1[26][1] = 3;
//    scoreArray1[27][1] = 48;
//    
//    scoreArray1[30][1] = E;
//    scoreArray1[31][1] = 2;
//    scoreArray1[32][1] = 0;
//    scoreArray1[33][1] = 3;
//    scoreArray1[34][1] = 48;
//    scoreArray1[35][1] = 0;
//    scoreArray1[36][1] = 4;
//    scoreArray1[37][1] = 0;
//    
//    scoreArray1[40][1] = D;
//    scoreArray1[41][1] = 2;
//    scoreArray1[42][1] = 0;
//    scoreArray1[43][1] = 4;
//    scoreArray1[44][1] = 0;
//    scoreArray1[45][1] = 0;
//    scoreArray1[46][1] = 4;
//    scoreArray1[47][1] = 16;
//    
//    scoreArray1[50][1] = D;
//    scoreArray1[51][1] = 2;
//    scoreArray1[52][1] = 0;
//    scoreArray1[53][1] = 4;
//    scoreArray1[54][1] = 16;
//    scoreArray1[55][1] = 0;
//    scoreArray1[56][1] = 4;
//    scoreArray1[57][1] = 32;
//    
//    scoreArray1[60][1] = C;
//    scoreArray1[61][1] = 2;
//    scoreArray1[62][1] = 0;
//    scoreArray1[63][1] = 4;
//    scoreArray1[64][1] = 32;
//    scoreArray1[65][1] = 0;
//    scoreArray1[66][1] = 5;
//    scoreArray1[67][1] = 0;
//    
//    scoreArray1[0][2] = G;
//    scoreArray1[1][2] = 2;
//    scoreArray1[2][2] = 0;
//    scoreArray1[3][2] = 5;
//    scoreArray1[4][2] = 0;
//    scoreArray1[5][2] = 0;
//    scoreArray1[6][2] = 5;
//    scoreArray1[7][2] = 16;
//    
//    scoreArray1[10][2] = G;
//    scoreArray1[11][2] = 2;
//    scoreArray1[12][2] = 0;
//    scoreArray1[13][2] = 5;
//    scoreArray1[14][2] = 16;
//    scoreArray1[15][2] = 0;
//    scoreArray1[16][2] = 5;
//    scoreArray1[17][2] = 32;
//    
//    scoreArray1[20][2] = F;
//    scoreArray1[21][2] = 2;
//    scoreArray1[22][2] = 0;
//    scoreArray1[23][2] = 5;
//    scoreArray1[24][2] = 32;
//    scoreArray1[25][2] = 0;
//    scoreArray1[26][2] = 5;
//    scoreArray1[27][2] = 48;
//    
//    scoreArray1[30][2] = F;
//    scoreArray1[31][2] = 2;
//    scoreArray1[32][2] = 0;
//    scoreArray1[33][2] = 5;
//    scoreArray1[34][2] = 48;
//    scoreArray1[35][2] = 0;
//    scoreArray1[36][2] = 6;
//    scoreArray1[37][2] = 0;
//    
//    scoreArray1[40][2] = E;
//    scoreArray1[41][2] = 2;
//    scoreArray1[42][2] = 0;
//    scoreArray1[43][2] = 6;
//    scoreArray1[44][2] = 0;
//    scoreArray1[45][2] = 0;
//    scoreArray1[46][2] = 6;
//    scoreArray1[47][2] = 16;
//    
//    scoreArray1[50][2] = E;
//    scoreArray1[51][2] = 2;
//    scoreArray1[52][2] = 0;
//    scoreArray1[53][2] = 6;
//    scoreArray1[54][2] = 16;
//    scoreArray1[55][2] = 0;
//    scoreArray1[56][2] = 6;
//    scoreArray1[57][2] = 32;
//    
//    scoreArray1[60][2] = D;
//    scoreArray1[61][2] = 2;
//    scoreArray1[62][2] = 0;
//    scoreArray1[63][2] = 6;
//    scoreArray1[64][2] = 32;
//    scoreArray1[65][2] = 0;
//    scoreArray1[66][2] = 7;
//    scoreArray1[67][2] = 0;
//    
//    scoreArray1[0][3] = G;
//    scoreArray1[1][3] = 2;
//    scoreArray1[2][3] = 0;
//    scoreArray1[3][3] = 7;
//    scoreArray1[4][3] = 0;
//    scoreArray1[5][3] = 0;
//    scoreArray1[6][3] = 7;
//    scoreArray1[7][3] = 16;
//    
//    scoreArray1[10][3] = G;
//    scoreArray1[11][3] = 2;
//    scoreArray1[12][3] = 0;
//    scoreArray1[13][3] = 7;
//    scoreArray1[14][3] = 16;
//    scoreArray1[15][3] = 0;
//    scoreArray1[16][3] = 7;
//    scoreArray1[17][3] = 32;
//    
//    scoreArray1[20][3] = F;
//    scoreArray1[21][3] = 2;
//    scoreArray1[22][3] = 0;
//    scoreArray1[23][3] = 7;
//    scoreArray1[24][3] = 32;
//    scoreArray1[25][3] = 0;
//    scoreArray1[26][3] = 7;
//    scoreArray1[27][3] = 48;
//    
//    scoreArray1[30][3] = F;
//    scoreArray1[31][3] = 2;
//    scoreArray1[32][3] = 0;
//    scoreArray1[33][3] = 7;
//    scoreArray1[34][3] = 48;
//    scoreArray1[35][3] = 0;
//    scoreArray1[36][3] = 8;
//    scoreArray1[37][3] = 0;
//    
//    scoreArray1[40][3] = E;
//    scoreArray1[41][3] = 2;
//    scoreArray1[42][3] = 0;
//    scoreArray1[43][3] = 8;
//    scoreArray1[44][3] = 0;
//    scoreArray1[45][3] = 0;
//    scoreArray1[46][3] = 8;
//    scoreArray1[47][3] = 16;
//    
//    scoreArray1[50][3] = E;
//    scoreArray1[51][3] = 2;
//    scoreArray1[52][3] = 0;
//    scoreArray1[53][3] = 8;
//    scoreArray1[54][3] = 16;
//    scoreArray1[55][3] = 0;
//    scoreArray1[56][3] = 8;
//    scoreArray1[57][3] = 32;
//    
//    scoreArray1[60][3] = D;
//    scoreArray1[61][3] = 2;
//    scoreArray1[62][3] = 0;
//    scoreArray1[63][3] = 8;
//    scoreArray1[64][3] = 32;
//    scoreArray1[65][3] = 0;
//    scoreArray1[66][3] = 9;
//    scoreArray1[67][3] = 0;
//    
//    scoreArray1[0][4] = C;  // Note
//    scoreArray1[1][4] = 2;  // Pitch
//    scoreArray1[2][4] = 0;  // Measure Start High
//    scoreArray1[3][4] = 9;  // Measure Start Low
//    scoreArray1[4][4] = 0;  // Location Start
//    scoreArray1[5][4] = 0;  // Measure End High
//    scoreArray1[6][4] = 9;  // Measure End Low
//    scoreArray1[7][4] = 16; // Location End
//    
//    scoreArray1[10][4] = C;
//    scoreArray1[11][4] = 2;
//    scoreArray1[12][4] = 0;
//    scoreArray1[13][4] = 9;
//    scoreArray1[14][4] = 16;
//    scoreArray1[15][4] = 0;
//    scoreArray1[16][4] = 9;
//    scoreArray1[17][4] = 32;
//    
//    scoreArray1[20][4] = G;
//    scoreArray1[21][4] = 2;
//    scoreArray1[22][4] = 0;
//    scoreArray1[23][4] = 1;
//    scoreArray1[24][4] = 32;
//    scoreArray1[25][4] = 0;
//    scoreArray1[26][4] = 1;
//    scoreArray1[27][4] = 48;
//    
//    scoreArray1[30][4] = G;
//    scoreArray1[31][4] = 2;
//    scoreArray1[32][4] = 0;
//    scoreArray1[33][4] = 9;
//    scoreArray1[34][4] = 48;
//    scoreArray1[35][4] = 0;
//    scoreArray1[36][4] = 10;
//    scoreArray1[37][4] = 0;
//    
//    scoreArray1[40][4] = A;
//    scoreArray1[41][4] = 2;
//    scoreArray1[42][4] = 0;
//    scoreArray1[43][4] = 10;
//    scoreArray1[44][4] = 0;
//    scoreArray1[45][4] = 0;
//    scoreArray1[46][4] = 10;
//    scoreArray1[47][4] = 16;
//    
//    scoreArray1[50][4] = A;
//    scoreArray1[51][4] = 2;
//    scoreArray1[52][4] = 0;
//    scoreArray1[53][4] = 10;
//    scoreArray1[54][4] = 16;
//    scoreArray1[55][4] = 0;
//    scoreArray1[56][4] = 10;
//    scoreArray1[57][4] = 32;
//    
//    scoreArray1[60][4] = G;
//    scoreArray1[61][4] = 2;
//    scoreArray1[62][4] = 0;
//    scoreArray1[63][4] = 10;
//    scoreArray1[64][4] = 32;
//    scoreArray1[65][4] = 0;
//    scoreArray1[66][4] = 11;
//    scoreArray1[67][4] = 0;
//    
//    scoreArray1[0][5] = F;
//    scoreArray1[1][5] = 2;
//    scoreArray1[2][5] = 0;
//    scoreArray1[3][5] = 11;
//    scoreArray1[4][5] = 0;
//    scoreArray1[5][5] = 0;
//    scoreArray1[6][5] = 11;
//    scoreArray1[7][5] = 16;
//    
//    scoreArray1[10][5] = F;
//    scoreArray1[11][5] = 2;
//    scoreArray1[12][5] = 0;
//    scoreArray1[13][5] = 11;
//    scoreArray1[14][5] = 16;
//    scoreArray1[15][5] = 0;
//    scoreArray1[16][5] = 11;
//    scoreArray1[17][5] = 32;
//    
//    scoreArray1[20][5] = E;
//    scoreArray1[21][5] = 2;
//    scoreArray1[22][5] = 0;
//    scoreArray1[23][5] = 11;
//    scoreArray1[24][5] = 32;
//    scoreArray1[25][5] = 0;
//    scoreArray1[26][5] = 11;
//    scoreArray1[27][5] = 48;
//    
//    scoreArray1[30][5] = E;
//    scoreArray1[31][5] = 2;
//    scoreArray1[32][5] = 0;
//    scoreArray1[33][5] = 11;
//    scoreArray1[34][5] = 48;
//    scoreArray1[35][5] = 0;
//    scoreArray1[36][5] = 12;
//    scoreArray1[37][5] = 0;
//    
//    scoreArray1[40][5] = D;
//    scoreArray1[41][5] = 2;
//    scoreArray1[42][5] = 0;
//    scoreArray1[43][5] = 12;
//    scoreArray1[44][5] = 0;
//    scoreArray1[45][5] = 0;
//    scoreArray1[46][5] = 12;
//    scoreArray1[47][5] = 16;
//    
//    scoreArray1[50][5] = G;
//    scoreArray1[51][5] = 2;
//    scoreArray1[52][5] = 0;
//    scoreArray1[53][5] = 12;
//    scoreArray1[54][5] = 16;
//    scoreArray1[55][5] = 0;
//    scoreArray1[56][5] = 12;
//    scoreArray1[57][5] = 32;
//    
//    scoreArray1[60][5] = B;
//    scoreArray1[61][5] = 2;
//    scoreArray1[62][5] = 0;
//    scoreArray1[63][5] = 12;
//    scoreArray1[64][5] = 16;
//    scoreArray1[65][5] = 0;
//    scoreArray1[66][5] = 12;
//    scoreArray1[67][5] = 32;
//    
//    scoreArray1[0][6] = C;
//    scoreArray1[1][6] = 3;
//    scoreArray1[2][6] = 0;
//    scoreArray1[3][6] = 12;
//    scoreArray1[4][6] = 32;
//    scoreArray1[5][6] = 0;
//    scoreArray1[6][6] = 13;
//    scoreArray1[7][6] = 0;
//    
//    scoreArray1[10][6] = C;
//    scoreArray1[11][6] = 2;
//    scoreArray1[12][6] = 0;
//    scoreArray1[13][6] = 12;
//    scoreArray1[14][6] = 32;
//    scoreArray1[15][6] = 0;
//    scoreArray1[16][6] = 13;
//    scoreArray1[17][6] = 0;
//    
//    scoreArray1[20][6] = E;
//    scoreArray1[21][6] = 2;
//    scoreArray1[22][6] = 0;
//    scoreArray1[23][6] = 12;
//    scoreArray1[24][6] = 32;
//    scoreArray1[25][6] = 0;
//    scoreArray1[26][6] = 13;
//    scoreArray1[27][6] = 0;
//    
//    scoreArray1[30][6] = G;
//    scoreArray1[31][6] = 2;
//    scoreArray1[32][6] = 0;
//    scoreArray1[33][6] = 12;
//    scoreArray1[34][6] = 32;
//    scoreArray1[35][6] = 0;
//    scoreArray1[36][6] = 13;
//    scoreArray1[37][6] = 0;

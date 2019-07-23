#include <xc.h>
#include <p33CH512MP506.h>
#include "dsPIC33CH512MP506_Pins.h"
#include "dsPIC33CH512MP506_Audio.h"

buttonStruct playPauseButton;
buttonStruct rewindButton;
buttonStruct fastForwardButton;
buttonStruct saveButton;
buttonStruct loadButton;
buttonStruct leftButton;
buttonStruct rightButton;
buttonStruct upButton;
buttonStruct downButton;

interfaceStruct *controlInterface;

// ** Global Variables ** //
enum ledNames{sine = 0, sawtooth, instrument, one, four, seven, f1,
              square, attack, memory, two, five, eight, f2,
              triangle, decay, tempo, three, six, nine, f3,
              d22e, d22d, d22c, d22b, d22a, d22f, d22g,
              d23e, d23d, d23c, d23b, d23a, d23f, d23g,
              d24e, d24d, d24c, d24b, d24a, d24f, d24g};
              
int currentLed = 0;
int ledControlEnabled = 1;
int ledControlArray[42];
int currentLedOn = 0;
int numberDisplay;
int interfaceSelection = 0;
int selectionAmplitude;
int selectionChanged = 1;
int audioControlArray[19];

int tempCounter = 0;

// ** Functions ** //
int pinInit()
{
    LATA = 0x0000;
    LATB = 0x0000;
    LATC = 0x0000;
    LATD = 0x0000;
    
    // Configure LED Pins as Outputs
    TRISBbits.TRISB14 = 0;  // LED1
    TRISBbits.TRISB15 = 0;  // LED2
    TRISCbits.TRISC12 = 0;  // LED3
    TRISCbits.TRISC13 = 0;  // LED4
    TRISCbits.TRISC14 = 0;  // LED5
    TRISCbits.TRISC15 = 0;  // LED6
    TRISDbits.TRISD15 = 0;  // LED7
    TRISDbits.TRISD14 = 0;  // CTRL1
    TRISDbits.TRISD13 = 0;  // CTRL2
    TRISCbits.TRISC0 = 0;   // CTRL3
    TRISAbits.TRISA0 = 0;   // CTRL4
    TRISAbits.TRISA1 = 0;   // CTRL5
    TRISAbits.TRISA2 = 0;   // CTRL6
    
    // Configure Button Pins as Inputs
    TRISAbits.TRISA3 = 1;   // BTN1
    TRISAbits.TRISA4 = 1;   // BTN2
    TRISDbits.TRISD12 = 1;  // BTN3
    TRISCbits.TRISC1 = 1;   // BTN4
    TRISCbits.TRISC2 = 1;   // BTN5
    TRISCbits.TRISC6 = 1;   // BTN6
    TRISCbits.TRISC3 = 1;   // BTN7
    TRISDbits.TRISD11 = 1;  // BTN8
    TRISDbits.TRISD10 = 1;  // BTN9
    
    // Disable Analog Pins
    ANSELA = 0x0000;
    ANSELB = 0x0000;
    ANSELC = 0x0000;
    ANSELD = 0x0000;
    
    // Configure AK4386 Pins
    // Set as I2S
    TRISDbits.TRISD4 = 1;   // DIF0 -> UART
    LATDbits.LATD4 = 0;
    TRISCbits.TRISC11 = 1;  // DIF1 -> UART
    LATCbits.LATC11 = 0;
    // Normal Speed
    TRISDbits.TRISD1 = 0;   // DFS0
    LATDbits.LATD1 = 0;
    TRISDbits.TRISD2 = 0;   // DFS1
    LATDbits.LATD2 = 0;
    // Deemphasis Filter (OFF)
    TRISDbits.TRISD3 = 0;
    LATDbits.LATD3 = 0;
    
    TRISDbits.TRISD0 = 0;   // PDN
    LATDbits.LATD0 = 1;
    
    // Configure I2C Pins 
    TRISBbits.TRISB8 = 0;
    TRISBbits.TRISB9 = 0;
    
    // Configure I2S pins
    TRISBbits.TRISB13 = 0;
    TRISBbits.TRISB12 = 0;
    TRISBbits.TRISB11 = 0;
    TRISBbits.TRISB10 = 0;
    //__builtin_write_RPCON(0x55);
    //__builtin_write_RPCON(0xAA);
    NVMKEY = 0x55;
    NVMKEY = 0xAA;
    __builtin_write_RPCON(0x0000);
    // SCK1 = 6; SDO1 = 5; SS1 = 7; REFCLKO = 14;
    RPOR6bits.RP45R = 14;   // MCLK = RP45/RB13
    RPOR6bits.RP44R = 6;    // BLCK = RP44/RB12
    RPOR5bits.RP43R = 5;    // SDTI = RP43/RB11
    RPOR5bits.RP42R = 7;    // LRCK = RP42/RB10
    RPOR4bits.RP40R = 0;    // RB8 tied to default pin
    RPOR4bits.RP41R = 0;    // RB9 tied to default pin
    NVMKEY = 0x55;
    NVMKEY = 0xAA;
    __builtin_write_RPCON(0x0800);
    
    // Configure Reference Clock (24 MHz / 512 = 46,875 Hz)
    // Configure Reference Clock (4 MHz / 512 = 7812.5 Hz)
    REFOCONLbits.ROEN = 0;
    while((REFOCONL & 0x0100) != 0) continue;
    REFOCONL = 0x3800; // 0011 1000 0000 0000
    REFOCONH = 8; // 96MHz / (2*12) = 4MHz    2;   // 96MHz / (2*2) = 24MHz
    while(REFOCONLbits.ROSWEN == 1) continue;
    REFOCONLbits.ROEN = 1;
    
    // Configure Additional Pins
    TRISDbits.TRISD7 = 0;   // WP
    set_wp(1);
    io_ffc1(1);
    io_ffc2(1);
    io_ffc3(1);
    io_ffc4(1);
    io_ffc5(1);
    io_ffc6(1);
    io_ffc7(1);
    io_ffc8(1);
    io_ffc9(1);
    io_ffc10(1);
    io_ffc11(1);
    io_ffc12(1);
    
    // Turn off all LEDs
    ledsOff();
    
    return 0;
}

int ak4386Init()
{
    // Find device
    
    return 0;
}

void linkInterface(interfaceStruct *interface)
{
    controlInterface = interface;
    controlInterface->fastForward = 0;
    controlInterface->play = 0;
    controlInterface->rewind = 0;
    controlInterface->load = 0;
    controlInterface->saving = 0;
}

void readButtons()
{
    int status = BTN1;
    updateButton(&rewindButton, &ctrlRewind, status);
    status = BTN2;
    updateButton(&playPauseButton, &ctrlPlay, status);
    status = BTN3;
    updateButton(&fastForwardButton, &ctrlFastForward, status);
    status = BTN4;
    updateButton(&loadButton, &ctrlLoad, status);
    status = BTN5;
    updateButton(&saveButton, &ctrlSave, status);
    status = BTN6;
    updateButton(&leftButton, &ctrlLeft, status);
    status = BTN7;
    updateButton(&upButton, &ctrlUp, status);
    status = BTN8;
    updateButton(&rightButton, &ctrlRight, status);
    status = BTN9;
    updateButton(&downButton, &ctrlDown, status);
}

void ctrlRewind(int buttonHold)
{
    // Disable other active states in case multiple buttons are pressed
    playPauseButton.isActive = 0;
    fastForwardButton.isActive = 0;
    loadButton.isActive = 0;
    saveButton.isActive = 0;
    leftButton.isActive = 0;
    rightButton.isActive = 0;
    upButton.isActive = 0;
    downButton.isActive = 0;
    
    if(buttonHold == 0){
        setScoreArray(0, 0);
    }
}

void ctrlPlay(int buttonHold)
{
    // Disable other active states in case multiple buttons are pressed
    rewindButton.isActive = 0;
    fastForwardButton.isActive = 0;
    loadButton.isActive = 0;
    saveButton.isActive = 0;
    leftButton.isActive = 0;
    rightButton.isActive = 0;
    upButton.isActive = 0;
    downButton.isActive = 0;
    
    if(controlInterface->play == 1) controlInterface->play = 0;
    else controlInterface->play = 1;
}

void ctrlFastForward(int buttonHold)
{
    // Disable other active states in case multiple buttons are pressed
    rewindButton.isActive = 0;
    playPauseButton.isActive = 0;
    loadButton.isActive = 0;
    saveButton.isActive = 0;
    leftButton.isActive = 0;
    rightButton.isActive = 0;
    upButton.isActive = 0;
    downButton.isActive = 0;
}

void ctrlLoad(int buttonHold)
{
    // Disable other active states in case multiple buttons are pressed
    rewindButton.isActive = 0;
    playPauseButton.isActive = 0;
    fastForwardButton.isActive = 0;
    saveButton.isActive = 0;
    leftButton.isActive = 0;
    rightButton.isActive = 0;
    upButton.isActive = 0;
    downButton.isActive = 0;
    
    // Prevent accidental loading
    if(buttonHold == 0) return;
    
    if(interfaceSelection == memory){
        // Load Song from EEPROM
    } else if(interfaceSelection == instrument){
        // Load Instrument from EEPROM
    }
    
    readControlArray(audioControlArray);
    selectionChanged = 1;
}

void ctrlSave(int buttonHold)
{
    // Disable other active states in case multiple buttons are pressed
    rewindButton.isActive = 0;
    playPauseButton.isActive = 0;
    fastForwardButton.isActive = 0;
    loadButton.isActive = 0;
    leftButton.isActive = 0;
    rightButton.isActive = 0;
    upButton.isActive = 0;
    downButton.isActive = 0;
    
    // Don't save unless the button is held
    if(buttonHold == 0) return;
    
    if(interfaceSelection == memory){
        // Save song to EEPROM
    } else if(interfaceSelection == instrument){
        // Save instrument to EEPROM
    } else {
        int resume = 1;
        if(controlInterface->play == 0) resume = 0;
        controlInterface->play = 0;
        generateAllWaveforms();
        if(resume == 1) controlInterface->play = 1;
    }
    
    writeControlArray(audioControlArray);
    selectionChanged = 1;
}

void ctrlLeft(int buttonHold)
{
    // Disable other active states in case multiple buttons are pressed
    rewindButton.isActive = 0;
    playPauseButton.isActive = 0;
    fastForwardButton.isActive = 0;
    loadButton.isActive = 0;
    saveButton.isActive = 0;
    rightButton.isActive = 0;
    upButton.isActive = 0;
    downButton.isActive = 0;
    
    interfaceSelection--;
    if(interfaceSelection < 0) interfaceSelection = 19;
    
    selectionChanged = 1;
    
    // UPDATE LEDS
}

void ctrlRight(int buttonHold)
{
    // Disable other active states in case multiple buttons are pressed
    rewindButton.isActive = 0;
    playPauseButton.isActive = 0;
    fastForwardButton.isActive = 0;
    loadButton.isActive = 0;
    saveButton.isActive = 0;
    leftButton.isActive = 0;
    upButton.isActive = 0;
    downButton.isActive = 0;
    
    interfaceSelection++;
    if(interfaceSelection > 19) interfaceSelection = 0;
    
    selectionChanged = 1;
    
    // UPDATE LEDS
}

void ctrlUp(int buttonHold)
{
    // Disable other active states in case multiple buttons are pressed
    rewindButton.isActive = 0;
    playPauseButton.isActive = 0;
    fastForwardButton.isActive = 0;
    loadButton.isActive = 0;
    saveButton.isActive = 0;
    leftButton.isActive = 0;
    rightButton.isActive = 0;
    downButton.isActive = 0;
    
    if(buttonHold == 0){ 
        selectionAmplitude = 1;
    } else {
        selectionAmplitude = 10;
    }
    
    selectionChanged = 1;
}

void ctrlDown(int buttonHold)
{
    // Disable other active states in case multiple buttons are pressed
    rewindButton.isActive = 0;
    playPauseButton.isActive = 0;
    fastForwardButton.isActive = 0;
    loadButton.isActive = 0;
    saveButton.isActive = 0;
    leftButton.isActive = 0;
    rightButton.isActive = 0;
    upButton.isActive = 0;
    
    if(buttonHold == 0){
        selectionAmplitude = -1;
    } else {
        selectionAmplitude = -10;
    }
    
    selectionChanged = 1;
}

void updateButton(buttonStruct *button, void (*functionPtr)(int), int status)
{
    if(status == 0){    // Button is pressed
        button->isPressed = 1;
        button->isReleased = 0;
        button->pressDuration++;
        button->releaseDuration = 0;
        
        // First Press
        if((button->pressDuration > BTN_DURATION) && (button->isActive == 0)){
            (*functionPtr)(0);
            button->isActive = 1;
            
            return;
        }
        
        // Continuous Hold
        if((button->pressDuration > BTN_HOLD_DURATION) && (button->isActive == 1)){
            (*functionPtr)(1);
            button->pressDuration -= BTN_HOLD_DURATION / 2;
            return;
        }
    }
    
    if(status == 1){    // Button is released
        button->isPressed = 0;
        button->isReleased = 1;
        button->pressDuration = 0;
        button->releaseDuration++;
        
        if((button->releaseDuration > BTN_DURATION) &&(button->isActive = 1)){
            button->isActive = 0;
        }
    }
}

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
 * 18 - Tempo
 * */
void updateInterface()
{
    
    readButtons();
    
    if(selectionChanged != 0){
        selectionChanged = 0;
        
        switch(interfaceSelection){
            case 0: // Sine Wave
                updateAmplitude(9);
                updateLeds(sine);
                break;

            case 1: // Square Wave
                updateAmplitude(10);
                updateLeds(square);
                break;

            case 2: // Triangle Wave
                updateAmplitude(11);
                updateLeds(triangle);
                break;

            case 3: // Sawtooth Wave
                updateAmplitude(12);
                updateLeds(sawtooth);
                break;

            case 4: // Attack
                updateAmplitude(13);
                updateLeds(attack);
                break;

            case 5: // Decay
                updateAmplitude(14);
                updateLeds(decay);
                break;

            case 6: // Instrument

                updateLeds(instrument);
                break;

            case 7: // Memory

                updateLeds(memory);
                break;
            case 8: // Tempo
                updateAmplitude(18);
                writeControlArray(audioControlArray);
                updateLeds(0xFF);
                updateTempo(audioControlArray[18]);
                break;
                
            case 9: // Overtone 1
                updateAmplitude(0);
                updateLeds(one);
                break;

            case 10: // Overtone 2
                updateAmplitude(1);
                updateLeds(two);
                break;

            case 11: // Overtone 3
                updateAmplitude(2);
                updateLeds(three);
                break;

            case 12: // Overtone 4
                updateAmplitude(3);
                updateLeds(four);
                break;

            case 13: // Overtone 5
                updateAmplitude(4);
                updateLeds(five);
                break;

            case 14: // Overtone 6
                updateAmplitude(5);
                updateLeds(six);
                break;

            case 15: // Overtone 7
                updateAmplitude(6);
                updateLeds(seven);
                break;

            case 16: // Overtone 8
                updateAmplitude(7);
                updateLeds(eight);
                break;

            case 17: // Overtone 9
                updateAmplitude(8);
                updateLeds(nine);
                break;

            case 18: // Function 1
                updateAmplitude(15);
                updateLeds(f1);
                break;

            case 19: // Function 2
                updateAmplitude(16);
                updateLeds(f2);
                break;

            case 20: // Function 3
                updateAmplitude(17);
                updateLeds(f3);
                break;
        }

        updateNumberDisplay();
    }
}

void ledInit()
{
    int i;
    
    readControlArray(audioControlArray);
    
    ledControlArray[0] = 1;
    interfaceSelection = 0;
    
    for(i=1; i<42; i++){
        ledControlArray[i] = 0;
    }
}

void updateAmplitude(int arrayIndex)
{
    int currentOnes;
    
    if(selectionAmplitude == 10) { 
        currentOnes = audioControlArray[arrayIndex] % 10;
        audioControlArray[arrayIndex] += (10-currentOnes);
    } else if(selectionAmplitude == -10) {
        currentOnes = audioControlArray[arrayIndex] % 10;
        if(currentOnes != 0) audioControlArray[arrayIndex] -= currentOnes;
        else audioControlArray[arrayIndex] -= 10;
    } else {
        audioControlArray[arrayIndex] += selectionAmplitude;
    }
    
    
    if(arrayIndex != 18){
        if(audioControlArray[arrayIndex] > 100) audioControlArray[arrayIndex] = 100;
        if(audioControlArray[arrayIndex] < 0) audioControlArray[arrayIndex] = 0;
    } else {
        if(audioControlArray[arrayIndex] > 220) audioControlArray[arrayIndex] = 220;
        if(audioControlArray[arrayIndex] < 40) audioControlArray[arrayIndex] = 40;
    }
    numberDisplay = audioControlArray[arrayIndex];
    selectionAmplitude = 0;
}

void updateLeds(int ledChoice)
{
    if(ledChoice == 0xFF){
        ledControlArray[currentLedOn] = 0;
        return;
    }
    
    ledControlArray[currentLedOn] = 0;
    ledControlArray[ledChoice] = 1;
    currentLedOn = ledChoice;
}

void updateNumberDisplay()
{
    // Update the seven segment displays
    int ones, tens, hundreds;
    ones = numberDisplay % 10;
    tens = (numberDisplay / 10) % 10;
    hundreds = (numberDisplay / 100) % 10;
    
    if(numberDisplay < 10) tens = 10;
    
    switch(ones){
        case 0:
            ledControlArray[d24a] = 1;
            ledControlArray[d24b] = 1;
            ledControlArray[d24c] = 1;
            ledControlArray[d24d] = 1;
            ledControlArray[d24e] = 1;
            ledControlArray[d24f] = 1;
            ledControlArray[d24g] = 0;
            break;
            
        case 1:
            ledControlArray[d24a] = 0;
            ledControlArray[d24b] = 1;
            ledControlArray[d24c] = 1;
            ledControlArray[d24d] = 0;
            ledControlArray[d24e] = 0;
            ledControlArray[d24f] = 0;
            ledControlArray[d24g] = 0;
            break;
            
        case 2:
            ledControlArray[d24a] = 1;
            ledControlArray[d24b] = 1;
            ledControlArray[d24c] = 0;
            ledControlArray[d24d] = 1;
            ledControlArray[d24e] = 1;
            ledControlArray[d24f] = 0;
            ledControlArray[d24g] = 1;
            break;
            
        case 3:
            ledControlArray[d24a] = 1;
            ledControlArray[d24b] = 1;
            ledControlArray[d24c] = 1;
            ledControlArray[d24d] = 1;
            ledControlArray[d24e] = 0;
            ledControlArray[d24f] = 0;
            ledControlArray[d24g] = 1;
            break;
            
        case 4:
            ledControlArray[d24a] = 0;
            ledControlArray[d24b] = 1;
            ledControlArray[d24c] = 1;
            ledControlArray[d24d] = 0;
            ledControlArray[d24e] = 0;
            ledControlArray[d24f] = 1;
            ledControlArray[d24g] = 1;
            break;
            
        case 5:
            ledControlArray[d24a] = 1;
            ledControlArray[d24b] = 0;
            ledControlArray[d24c] = 1;
            ledControlArray[d24d] = 1;
            ledControlArray[d24e] = 0;
            ledControlArray[d24f] = 1;
            ledControlArray[d24g] = 1;
            break;
            
        case 6:
            ledControlArray[d24a] = 1;
            ledControlArray[d24b] = 0;
            ledControlArray[d24c] = 1;
            ledControlArray[d24d] = 1;
            ledControlArray[d24e] = 1;
            ledControlArray[d24f] = 1;
            ledControlArray[d24g] = 1;
            break;
            
        case 7:
            ledControlArray[d24a] = 1;
            ledControlArray[d24b] = 1;
            ledControlArray[d24c] = 1;
            ledControlArray[d24d] = 0;
            ledControlArray[d24e] = 0;
            ledControlArray[d24f] = 0;
            ledControlArray[d24g] = 0;
            break;
            
        case 8:
            ledControlArray[d24a] = 1;
            ledControlArray[d24b] = 1;
            ledControlArray[d24c] = 1;
            ledControlArray[d24d] = 1;
            ledControlArray[d24e] = 1;
            ledControlArray[d24f] = 1;
            ledControlArray[d24g] = 1;
            break;
            
        case 9:
            ledControlArray[d24a] = 1;
            ledControlArray[d24b] = 1;
            ledControlArray[d24c] = 1;
            ledControlArray[d24d] = 1;
            ledControlArray[d24e] = 0;
            ledControlArray[d24f] = 1;
            ledControlArray[d24g] = 1;
            break;
    }
    
    switch(tens){
        case 0:
            ledControlArray[d23a] = 1;
            ledControlArray[d23b] = 1;
            ledControlArray[d23c] = 1;
            ledControlArray[d23d] = 1;
            ledControlArray[d23e] = 1;
            ledControlArray[d23f] = 1;
            ledControlArray[d23g] = 0;
            break;
            
        case 1:
            ledControlArray[d23a] = 0;
            ledControlArray[d23b] = 1;
            ledControlArray[d23c] = 1;
            ledControlArray[d23d] = 0;
            ledControlArray[d23e] = 0;
            ledControlArray[d23f] = 0;
            ledControlArray[d23g] = 0;
            break;
            
        case 2:
            ledControlArray[d23a] = 1;
            ledControlArray[d23b] = 1;
            ledControlArray[d23c] = 0;
            ledControlArray[d23d] = 1;
            ledControlArray[d23e] = 1;
            ledControlArray[d23f] = 0;
            ledControlArray[d23g] = 1;
            break;
            
        case 3:
            ledControlArray[d23a] = 1;
            ledControlArray[d23b] = 1;
            ledControlArray[d23c] = 1;
            ledControlArray[d23d] = 1;
            ledControlArray[d23e] = 0;
            ledControlArray[d23f] = 0;
            ledControlArray[d23g] = 1;
            break;
            
        case 4:
            ledControlArray[d23a] = 0;
            ledControlArray[d23b] = 1;
            ledControlArray[d23c] = 1;
            ledControlArray[d23d] = 0;
            ledControlArray[d23e] = 0;
            ledControlArray[d23f] = 1;
            ledControlArray[d23g] = 1;
            break;
            
        case 5:
            ledControlArray[d23a] = 1;
            ledControlArray[d23b] = 0;
            ledControlArray[d23c] = 1;
            ledControlArray[d23d] = 1;
            ledControlArray[d23e] = 0;
            ledControlArray[d23f] = 1;
            ledControlArray[d23g] = 1;
            break;
            
        case 6:
            ledControlArray[d23a] = 1;
            ledControlArray[d23b] = 0;
            ledControlArray[d23c] = 1;
            ledControlArray[d23d] = 1;
            ledControlArray[d23e] = 1;
            ledControlArray[d23f] = 1;
            ledControlArray[d23g] = 1;
            break;
            
        case 7:
            ledControlArray[d23a] = 1;
            ledControlArray[d23b] = 1;
            ledControlArray[d23c] = 1;
            ledControlArray[d23d] = 0;
            ledControlArray[d23e] = 0;
            ledControlArray[d23f] = 0;
            ledControlArray[d23g] = 0;
            break;
            
        case 8:
            ledControlArray[d23a] = 1;
            ledControlArray[d23b] = 1;
            ledControlArray[d23c] = 1;
            ledControlArray[d23d] = 1;
            ledControlArray[d23e] = 1;
            ledControlArray[d23f] = 1;
            ledControlArray[d23g] = 1;
            break;
            
        case 9:
            ledControlArray[d23a] = 1;
            ledControlArray[d23b] = 1;
            ledControlArray[d23c] = 1;
            ledControlArray[d23d] = 1;
            ledControlArray[d23e] = 0;
            ledControlArray[d23f] = 1;
            ledControlArray[d23g] = 1;
            break;
            
        // Special case when value less than 10 to hide '0'
        case 10:
            ledControlArray[d23a] = 0;
            ledControlArray[d23b] = 0;
            ledControlArray[d23c] = 0;
            ledControlArray[d23d] = 0;
            ledControlArray[d23e] = 0;
            ledControlArray[d23f] = 0;
            ledControlArray[d23g] = 0;
            break;
    }
    
    switch(hundreds){
        case 0:
            ledControlArray[d22a] = 0; //1;
            ledControlArray[d22b] = 0; //1;
            ledControlArray[d22c] = 0; //1;
            ledControlArray[d22d] = 0; //1;
            ledControlArray[d22e] = 0; //1;
            ledControlArray[d22f] = 0; //1;
            ledControlArray[d22g] = 0;
            break;
            
        case 1:
            ledControlArray[d22a] = 0;
            ledControlArray[d22b] = 1;
            ledControlArray[d22c] = 1;
            ledControlArray[d22d] = 0;
            ledControlArray[d22e] = 0;
            ledControlArray[d22f] = 0;
            ledControlArray[d22g] = 0;
            break;
            
        case 2:
            ledControlArray[d22a] = 1;
            ledControlArray[d22b] = 1;
            ledControlArray[d22c] = 0;
            ledControlArray[d22d] = 1;
            ledControlArray[d22e] = 1;
            ledControlArray[d22f] = 0;
            ledControlArray[d22g] = 1;
            break;
            
        case 3:
            ledControlArray[d22a] = 1;
            ledControlArray[d22b] = 1;
            ledControlArray[d22c] = 1;
            ledControlArray[d22d] = 1;
            ledControlArray[d22e] = 0;
            ledControlArray[d22f] = 0;
            ledControlArray[d22g] = 1;
            break;
            
        case 4:
            ledControlArray[d22a] = 0;
            ledControlArray[d22b] = 1;
            ledControlArray[d22c] = 1;
            ledControlArray[d22d] = 0;
            ledControlArray[d22e] = 0;
            ledControlArray[d22f] = 1;
            ledControlArray[d22g] = 1;
            break;
            
        case 5:
            ledControlArray[d22a] = 1;
            ledControlArray[d22b] = 0;
            ledControlArray[d22c] = 1;
            ledControlArray[d22d] = 1;
            ledControlArray[d22e] = 0;
            ledControlArray[d22f] = 1;
            ledControlArray[d22g] = 1;
            break;
            
        case 6:
            ledControlArray[d22a] = 1;
            ledControlArray[d22b] = 0;
            ledControlArray[d22c] = 1;
            ledControlArray[d22d] = 1;
            ledControlArray[d22e] = 1;
            ledControlArray[d22f] = 1;
            ledControlArray[d22g] = 1;
            break;
            
        case 7:
            ledControlArray[d22a] = 1;
            ledControlArray[d22b] = 1;
            ledControlArray[d22c] = 1;
            ledControlArray[d22d] = 0;
            ledControlArray[d22e] = 0;
            ledControlArray[d22f] = 0;
            ledControlArray[d22g] = 0;
            break;
            
        case 8:
            ledControlArray[d22a] = 1;
            ledControlArray[d22b] = 1;
            ledControlArray[d22c] = 1;
            ledControlArray[d22d] = 1;
            ledControlArray[d22e] = 1;
            ledControlArray[d22f] = 1;
            ledControlArray[d22g] = 1;
            break;
            
        case 9:
            ledControlArray[d22a] = 1;
            ledControlArray[d22b] = 1;
            ledControlArray[d22c] = 1;
            ledControlArray[d22d] = 1;
            ledControlArray[d22e] = 0;
            ledControlArray[d22f] = 1;
            ledControlArray[d22g] = 1;
            break;
    }
}

void ledsOff()
{
    LED1 = 0; 
    LED2 = 0; 
    LED3 = 0;
    LED4 = 0; 
    LED5 = 0; 
    LED6 = 0;
    LED7 = 0;
    
    CTRL1 = 0; 
    CTRL2 = 0; 
    CTRL3 = 0;
    CTRL4 = 0; 
    CTRL5 = 0; 
    CTRL6 = 0;
}

void ledControlDisable()
{
    ledControlEnabled = 0;
}

void ledControlEnable()
{
    ledControlEnabled = 1;
}

void singleLedControl(int currentLed)
{
    // Start by turning off all LEDs
    ledsOff();
    
    Nop();
    Nop();
    Nop();
    
    switch(currentLed){
    // CTRL 1
        case 0: // D1
            LED1 = 1;
            CTRL1 = 1;
            break;
            
        case 1: // D4
            LED2 = 1;
            CTRL1 = 1;
            break;
            
        case 2: // D7
            LED3 = 1;
            CTRL1 = 1;
            break;
            
        case 3: // D10
            LED4 = 1;
            CTRL1 = 1;
            break;
            
        case 4: // D13
            LED5 = 1;
            CTRL1 = 1;
            break;
            
        case 5: // D16
            LED6 = 1;
            CTRL1 = 1;
            break;
            
        case 6: // D19
            LED7 = 1;
            CTRL1 = 1;
            break;
            
    // CTRL 2
        case 7: // D2
            LED1 = 1;
            CTRL2 = 1;
            break;
            
        case 8: // D5
            LED2 = 1;
            CTRL2 = 1;
            break;
            
        case 9: // D8
            LED3 = 1;
            CTRL2 = 1;
            break;
            
        case 10: // D11
            LED4 = 1;
            CTRL2 = 1;
            break;
            
        case 11: // D14
            LED5 = 1;
            CTRL2 = 1;
            break;
            
        case 12: // D17
            LED6 = 1;
            CTRL2 = 1;
            break;
            
        case 13: // D20
            LED7 = 1;
            CTRL2 = 1;
            break;
            
    // CTRL 3
        case 14: // D3
            LED1 = 1;
            CTRL3 = 1;
            break;
            
        case 15: // D6
            LED2 = 1;
            CTRL3 = 1;
            break;
            
        case 16: // D9
            LED3 = 1;
            CTRL3 = 1;
            break;
            
        case 17: // D12
            LED4 = 1;
            CTRL3 = 1;
            break;
            
        case 18: // D15
            LED5 = 1;
            CTRL3 = 1;
            break;
            
        case 19: // D18
            LED6 = 1;
            CTRL3 = 1;
            break;
            
        case 20: // D21
            LED7 = 1;
            CTRL3 = 1;
            break;
            
    // CTRL 4
        case 21: // D22 E
            LED1 = 1;
            CTRL4 = 1;
            break;
            
        case 22: // D22 D
            LED2 = 1;
            CTRL4 = 1;
            break;
            
        case 23: // D22 C
            LED3 = 1;
            CTRL4 = 1;
            break;
            
        case 24: // D22 B
            LED4 = 1;
            CTRL4 = 1;
            break;
            
        case 25: // D22 A
            LED5 = 1;
            CTRL4 = 1;
            break;
            
        case 26: // D22 F
            LED6 = 1;
            CTRL4 = 1;
            break;
            
        case 27: // D22 G
            LED7 = 1;
            CTRL4 = 1;
            break;
          
    // CTRL 5
        case 28: // D24 E
            LED1 = 1;
            CTRL5 = 1;
            break;
            
        case 29: // D24 D
            LED2 = 1;
            CTRL5 = 1;
            break;
            
        case 30: // D24 C
            LED3 = 1;
            CTRL5 = 1;
            break;
            
        case 31: // D24 B
            LED4 = 1;
            CTRL5 = 1;
            break;
            
        case 32: // D24 A
            LED5 = 1;
            CTRL5 = 1;
            break;
            
        case 33: // D24 F
            LED6 = 1;
            CTRL5 = 1;
            break;
            
        case 34: // D24 G
            LED7 = 1;
            CTRL5 = 1;
            break;
       
    // CTRL 6
        case 35: // D24 E
            LED1 = 1;
            CTRL6 = 1;
            break;
            
        case 36: // D24 D
            LED2 = 1;
            CTRL6 = 1;
            break;
            
        case 37: // D24 C
            LED3 = 1;
            CTRL6 = 1;
            break;
            
        case 38: // D24 B
            LED4 = 1;
            CTRL6 = 1;
            break;
            
        case 39: // D24 A
            LED5 = 1;
            CTRL6 = 1;
            break;
            
        case 40: // D24 F
            LED6 = 1;
            CTRL6 = 1;
            break;
            
        case 41: // D24 G
            LED7 = 1;
            CTRL6 = 1;
            break;     
    }
}

// TEMPORARY FUNCTION FOR TESTING LEDS
void currentLedIncrement()
{
    tempCounter++;
    
    if(tempCounter > 10){
        ledControl();
        currentLed++;
        if(currentLed >= 42) currentLed = 0;
        tempCounter = 0;
    }
    
}

void ledControl()
{
    //if(ledControlEnabled == 0) return;
    
    switch(currentLed){
    // ** CTRL1 ** //
        // LED D1 -|>-
        case 0:
            CTRL6 = 0;
            LED7 = 0;
            CTRL1 = 1;
            
            if(ledControlArray[0] == 1){
                LED1 = 1;
            }
            
            break;
        
        // LED D4 -|>-
        case 1:
            LED1 = 0;
            
            if(ledControlArray[1] == 1){
                LED2 = 1;
            }
            
            break;
            
        // LED D7 -|>-
        case 2:
            LED2 = 0;
            
            if(ledControlArray[2] == 1){
                LED3 = 1;
            }
            
            break;
         
        // LED D10 -|>-
        case 3:
            LED3 = 0;
            
            if(ledControlArray[3] == 1){
                LED4 = 1;
            }
            
            break;
        
        // LED D13 -|>-
        case 4:
            LED4 = 0;
            
            if(ledControlArray[4] == 1){
                LED5 = 1;
            }
            
            break;
        
        // LED D16 -|>-
        case 5:
            LED5 = 0;
            
            if(ledControlArray[5] == 1){
                LED6 = 1;
            }
            
            break;
            
        // LED D19 -|>-
        case 6:
            LED6 = 0;
            
            if(ledControlArray[6] == 1){
                LED7 = 1;
            }
            
            break;
        
        
    // ** CTRL2 ** //
        // LED D2 -|>-
        case 7:
            CTRL1 = 0;
            LED7 = 0;
            CTRL2 = 1;
            
            if(ledControlArray[7] == 1){
                LED1 = 1;
            }
            
            break;
            
        // LED D5 -|>-
        case 8:
            LED1 = 0;
            
            if(ledControlArray[8] == 1){
                LED2 = 1;
            }
            
            break;
            
        // LED D8 -|>-
        case 9:
            LED2 = 0;
            
            if(ledControlArray[9] == 1){
                LED3 = 1;
            }
            
            break;
            
        // LED D11 -|>-
        case 10:
            LED3 = 0;
            
            if(ledControlArray[10] == 1){
                LED4 = 1;
            }
            
            break;
            
        // LED D14 -|>-
        case 11:
            LED4 = 0;
            
            if(ledControlArray[11] == 1){
                LED5 = 1;
            }
            
            break;
            
        // LED D17 -|>-
        case 12:
            LED5 = 0;
            
            if(ledControlArray[12] == 1){
                LED6 = 1;
            }
            
            break;
            
        // LED D20 -|>-
        case 13:
            LED6 = 0;
            
            if(ledControlArray[13] == 1){
                LED7 = 1;
            }
            
            break;
            
    // ** CTRL3 ** //
        // LED D3 -|>-
        case 14:
            CTRL2 = 0;
            LED7 = 0;
            CTRL3 = 1;
            
            if(ledControlArray[14] == 1){
                LED1 = 1;
            }
            
            break;
            
        // LED D6 -|>-
        case 15:
            LED1 = 0;
            
            if(ledControlArray[15] == 1){
                LED2 = 1;
            }
            
            break;
            
        // LED D9 -|>-
        case 16:
            LED2 = 0;
            
            if(ledControlArray[16] == 1){
                LED3 = 1;
            }
            
            break;
            
        // LED D12 -|>-
        case 17:
            LED3 = 0;
            
            if(ledControlArray[17] == 1){
                LED4 = 1;
            }
            
            break;
            
        // LED D15 -|>-
        case 18:
            LED4 = 0;
            
            if(ledControlArray[18] == 1){
                LED5 = 1;
            }
            
            break;
            
        // LED D18 -|>-
        case 19:
            LED5 = 0;
            
            if(ledControlArray[19] == 1){
                LED6 = 1;
            }
            
            break;
            
        // LED D21 -|>-
        case 20:
            LED6 = 0;
            
            if(ledControlArray[20] == 1){
                LED7 = 1;
            }
            
            break;
            
    // ** CTRL4 ** //
        // LED D22 -|>- E
        case 21:
            CTRL3 = 0;
            LED7 = 0;
            CTRL4 = 1;
            
            if(ledControlArray[21] == 1){
                LED1 = 1;
            }
            
            break;
            
        // LED D22 -|>- D
        case 22:
            LED1 = 0;
            
            if(ledControlArray[22] == 1){
                LED2 = 1;
            }
            
            break;
            
        // LED D22 -|>- C
        case 23:
            LED2 = 0;
            
            if(ledControlArray[23] == 1){
                LED3 = 1;
            }
            
            break;
            
        // LED D22 -|>- B
        case 24:
            LED3 = 0;
            
            if(ledControlArray[24] == 1){
                LED4 = 1;
            }
            
            break;
            
        // LED D22 -|>- A
        case 25:
            LED4 = 0;
            
            if(ledControlArray[25] == 1){
                LED5 = 1;
            }
            
            break;
            
        // LED D22 -|>- F
        case 26:
            LED5 = 0;
            
            if(ledControlArray[26] == 1){
                LED6 = 1;
            }
            
            break;
            
        // LED D22 -|>- G
        case 27:
            LED6 = 0;
            
            if(ledControlArray[27] == 1){
                LED7 = 1;
            }
            
            break;
            
    // ** CTRL5 ** //
        // LED D23 -|>- E
        case 28:
            CTRL4 = 0;
            LED7 = 0;
            CTRL5 = 1;
            
            if(ledControlArray[28] == 1){
                LED1 = 1;
            }
            
            break;
            
        // LED D23 -|>- D
        case 29:
            LED1 = 0;
            
            if(ledControlArray[29] == 1){
                LED2 = 1;
            }
            
            break;
            
        // LED D23 -|>- C
        case 30:
            LED2 = 0;
            
            if(ledControlArray[30] == 1){
                LED3 = 1;
            }
            
            break;
            
        // LED D23 -|>- B
        case 31:
            LED3 = 0;
            
            if(ledControlArray[31] == 1){
                LED4 = 1;
            }
            
            break;
            
        // LED D23 -|>- A
        case 32:
            LED4 = 0;
            
            if(ledControlArray[32] == 1){
                LED5 = 1;
            }
            
            break;
            
        // LED D23 -|>- F
        case 33:
            LED5 = 0;
            
            if(ledControlArray[33] == 1){
                LED6 = 1;
            }
            
            break;
            
        // LED D23 -|>- G
        case 34:
            LED6 = 0;
            
            if(ledControlArray[34] == 1){
                LED7 = 1;
            }
            
            break;
            
    // ** CTRL6 ** //
        // LED D24 -|>- E
        case 35:
            CTRL5 = 0;
            LED7 = 0;
            CTRL6 = 1;
            
            if(ledControlArray[35] == 1){
                LED1 = 1;
            }
            
            break;
            
        // LED D24 -|>- D
        case 36:
            LED1 = 0;
            
            if(ledControlArray[36] == 1){
                LED2 = 1;
            }
            
            break;
            
        // LED D24 -|>- C
        case 37:
            LED2 = 0;
            
            if(ledControlArray[37] == 1){
                LED3 = 1;
            }
            
            break;
            
        // LED D24 -|>- B
        case 38:
            LED3 = 0;
            
            if(ledControlArray[38] == 1){
                LED4 = 1;
            }
            
            break;
            
        // LED D24 -|>- A
        case 39:
            LED4 = 0;
            
            if(ledControlArray[39] == 1){
                LED5 = 1;
            }
            
            break;
            
        // LED D24 -|>- F
        case 40:
            LED5 = 0;
            
            if(ledControlArray[40] == 1){
                LED6 = 1;
            }
            
            break;
            
        // LED D24 -|>- G
        case 41:
            LED6 = 0;
            
            if(ledControlArray[41] == 1){
                LED7 = 1;
            }
            
            break;
    }
}

void ledDisplaySequence()
{
    int delay = 5;
    
    ledControlDisable();
    
    singleLedControl(tempo); ledTimeDelay(delay);
    singleLedControl(memory); ledTimeDelay(delay);
    singleLedControl(instrument); ledTimeDelay(delay);
    singleLedControl(decay); ledTimeDelay(delay);
    singleLedControl(attack); ledTimeDelay(delay);
    singleLedControl(sawtooth); ledTimeDelay(delay);
    singleLedControl(triangle); ledTimeDelay(delay);
    singleLedControl(square); ledTimeDelay(delay);
    singleLedControl(sine); ledTimeDelay(delay);
    singleLedControl(one); ledTimeDelay(delay);
    singleLedControl(two); ledTimeDelay(delay);
    singleLedControl(three); ledTimeDelay(delay);
    singleLedControl(four); ledTimeDelay(delay);
    singleLedControl(five); ledTimeDelay(delay);
    singleLedControl(six); ledTimeDelay(delay);
    singleLedControl(seven); ledTimeDelay(delay);
    singleLedControl(eight); ledTimeDelay(delay);
    singleLedControl(nine); ledTimeDelay(delay);
    singleLedControl(f1); ledTimeDelay(delay);
    singleLedControl(f2); ledTimeDelay(delay);
    singleLedControl(f3); ledTimeDelay(delay);
    singleLedControl(d22d); ledTimeDelay(delay);
    singleLedControl(d22c); ledTimeDelay(delay);
    singleLedControl(d22g); ledTimeDelay(delay);
    singleLedControl(d22f); ledTimeDelay(delay);
    singleLedControl(d22a); ledTimeDelay(delay);
    singleLedControl(d23a); ledTimeDelay(delay);
    singleLedControl(d23b); ledTimeDelay(delay);
    singleLedControl(d23g); ledTimeDelay(delay);
    singleLedControl(d23e); ledTimeDelay(delay);
    singleLedControl(d23d); ledTimeDelay(delay);
    singleLedControl(d24d); ledTimeDelay(delay);
    singleLedControl(d24c); ledTimeDelay(delay);
    singleLedControl(d24g); ledTimeDelay(delay);
    singleLedControl(d24f); ledTimeDelay(delay);
    singleLedControl(d24a); ledTimeDelay(delay);
    singleLedControl(d24b); ledTimeDelay(delay);
    singleLedControl(d24c); ledTimeDelay(delay);
    singleLedControl(d24d); ledTimeDelay(delay);
    singleLedControl(d23d); ledTimeDelay(delay);
    singleLedControl(d22d); ledTimeDelay(delay);
    singleLedControl(f3); ledTimeDelay(delay);
    singleLedControl(f2); ledTimeDelay(delay);
    singleLedControl(f1); ledTimeDelay(delay);
    singleLedControl(nine); ledTimeDelay(delay);
    singleLedControl(tempo); ledTimeDelay(delay);
    ledsOff();ledTimeDelay(delay);
    
    ledControlEnable();
}

void ledTimeDelay(int maxDelay)
{
    int delay;
    int count;
    
    for(delay=0; delay<maxDelay; delay++){
        for(count=0; count<0x7FFF; count++){
            Nop();
        }
    }
}

// FFC1 Functions
void io_ffc1(int selection)
{
    if(selection == 0) TRISCbits.TRISC10 = 0;
    else TRISCbits.TRISC10 = 1;
}

void set_ffc1(int selection)
{
    FFC1_O = selection;
}

int read_ffc1()
{
   return FFC1_I; 
}

// FFC2 Functions
void io_ffc2(int selection)
{
    if(selection == 0) TRISCbits.TRISC5 = 0;
    else TRISCbits.TRISC5 = 1;
}

void set_ffc2(int selection)
{
    FFC2_O = selection;
}

int read_ffc2()
{
   return FFC2_I; 
}

// FFC3 Functions
void io_ffc3(int selection)
{
    if(selection == 0) TRISCbits.TRISC4 = 0;
    else TRISCbits.TRISC4 = 1;
}

void set_ffc3(int selection)
{
    FFC3_O = selection;
}

int read_ffc3()
{
   return FFC3_I; 
}

// FFC4 Functions
void io_ffc4(int selection)
{
    if(selection == 0) TRISBbits.TRISB7 = 0;
    else TRISBbits.TRISB7 = 1;
}

void set_ffc4(int selection)
{
    FFC4_O = selection;
}

int read_ffc4()
{
   return FFC4_I; 
}

// FFC5 Functions
void io_ffc5(int selection)
{
    if(selection == 0) TRISDbits.TRISD5 = 0;
    else TRISDbits.TRISD5 = 1;
}

void set_ffc5(int selection)
{
    FFC5_O = selection;
}

int read_ffc5()
{
   return FFC5_I; 
}

// FFC6 Functions
void io_ffc6(int selection)
{
    if(selection == 0) TRISDbits.TRISD6 = 0;
    else TRISDbits.TRISD6 = 1;
}

void set_ffc6(int selection)
{
    FFC6_O = selection;
}

int read_ffc6()
{
   return FFC6_I; 
}

// FFC7 Functions
void io_ffc7(int selection)
{
    if(selection == 0) TRISDbits.TRISD8 = 0;
    else TRISDbits.TRISD8 = 1;
}

void set_ffc7(int selection)
{
    FFC7_O = selection;
}

int read_ffc7()
{
   return FFC7_I; 
}

// FFC8 Functions
void io_ffc8(int selection)
{
    if(selection == 0) TRISDbits.TRISD9 = 0;
    else TRISDbits.TRISD9 = 1;
}

void set_ffc8(int selection)
{
    FFC8_O = selection;
}

int read_ffc8()
{
   return FFC8_I; 
}

// FFC9 Functions
void io_ffc9(int selection)
{
    if(selection == 0) TRISCbits.TRISC9 = 0;
    else TRISCbits.TRISC9 = 1;
}

void set_ffc9(int selection)
{
    FFC9_O = selection;
}

int read_ffc9()
{
   return FFC9_I; 
}

// FFC10 Functions
void io_ffc10(int selection)
{
    if(selection == 0) TRISCbits.TRISC9 = 0;
    else TRISCbits.TRISC9 = 1;
}

void set_ffc10(int selection)
{
    FFC10_O = selection;
}

int read_ffc10()
{
   return FFC10_I; 
}

// FFC11 Functions
void io_ffc11(int selection)
{
    if(selection == 0) TRISBbits.TRISB2 = 0;
    else TRISBbits.TRISB2 = 1;
}

void set_ffc11(int selection)
{
    FFC11_O = selection;
}

int read_ffc11()
{
   return FFC11_I; 
}

// FFC12 Functions
void io_ffc12(int selection)
{
    if(selection == 0) TRISCbits.TRISC7 = 0;
    else TRISCbits.TRISC7 = 1;
}

void set_ffc12(int selection)
{
    FFC12_O = selection;
}

int read_ffc12()
{
   return FFC12_I; 
}

void set_wp(int selection)
{
    WP = selection;
}

void ledDeviceFail()
{
    singleLedControl(f1); ledTimeDelay(1000);
    singleLedControl(f3); ledTimeDelay(1000);
}

void tempoLedToggle()
{
    if(ledControlArray[tempo] == 1) ledControlArray[tempo] = 0;
    else ledControlArray[tempo] = 1;
}

void tempoLedOn()
{
    ledControlArray[tempo] = 1;
}

void tempoLedOff()
{
    ledControlArray[tempo] = 0;
}
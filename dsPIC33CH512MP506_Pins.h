/* 
 * File:   dsPIC33CH512506_Pins.h
 * Author: Jules
 *
 * Created on June 26, 2019, 11:32 AM
 */

#ifndef DSPIC33CH512MP506_PINS_H
#define	DSPIC33CH512MP506_PINS_H

#include "dsPIC33CH512MP506_Audio.h"

#define LED1 LATBbits.LATB14
#define LED2 LATBbits.LATB15
#define LED3 LATCbits.LATC12
#define LED4 LATCbits.LATC13
#define LED5 LATCbits.LATC14
#define LED6 LATCbits.LATC15
#define LED7 LATDbits.LATD15

#define CTRL1 LATDbits.LATD14
#define CTRL2 LATDbits.LATD13
#define CTRL3 LATCbits.LATC0
#define CTRL4 LATAbits.LATA0
#define CTRL5 LATAbits.LATA1
#define CTRL6 LATAbits.LATA2

#define BTN1 PORTAbits.RA3
#define BTN2 PORTAbits.RA4
#define BTN3 PORTDbits.RD12
#define BTN4 PORTCbits.RC1
#define BTN5 PORTCbits.RC2
#define BTN6 PORTCbits.RC6
#define BTN7 PORTCbits.RC3
#define BTN8 PORTDbits.RD11
#define BTN9 PORTDbits.RD10

#define WP LATDbits.LATD7

#define NUM_FN 22
#define FN_SINE 0
#define FN_SAWTOOTH 1
#define FN_INSTRUMENT 2
#define FN_ONE 3
#define FN_FOUR 4
#define FN_SEVEN 5
#define FN_F1 6
#define FN_SQUARE 7
#define FN_ATTACK 8
#define FN_MEMORY 9
#define FN_TWO 10
#define FN_FIVE 11
#define FN_EIGHT 12
#define FN_F2 13
#define FN_TRIANGLE 14
#define FN_DECAY 15
#define FN_TEMPO 16
#define FN_THREE 17
#define FN_SIX 18
#define FN_NINE 19
#define FN_F3 20

#define D22E 21
#define D22D 22
#define D22C 23
#define D22B 24
#define D22A 25
#define D22F 26
#define D22G 27

#define D23E 28
#define D23D 29
#define D23C 30
#define D23B 31
#define D23A 32
#define D23F 33
#define D23G 34

#define D24E 35
#define D24D 36
#define D24C 37
#define D24B 38
#define D24A 39
#define D24F 40
#define D24G 41

// FFC Output Pins
#define FFC1_O LATCbits.LATC10
#define FFC2_O LATCbits.LATC5
#define FFC3_O LATCbits.LATC4
#define FFC4_O LATBbits.LATB7
#define FFC5_O LATDbits.LATD5
#define FFC6_O LATDbits.LATD6
#define FFC7_O LATDbits.LATD8
#define FFC8_O LATDbits.LATD9
#define FFC9_O LATCbits.LATC9
#define FFC10_O LATCbits.LATC8
#define FFC11_O LATBbits.LATB2
#define FFC12_O LATCbits.LATC7

// FFC Input Pins
#define FFC1_I PORTCbits.RC10
#define FFC2_I PORTCbits.RC5
#define FFC3_I PORTCbits.RC4
#define FFC4_I PORTBbits.RB7
#define FFC5_I PORTDbits.RD5
#define FFC6_I PORTDbits.RD6
#define FFC7_I PORTDbits.RD8
#define FFC8_I PORTDbits.RD9
#define FFC9_I PORTCbits.RC9
#define FFC10_I PORTCbits.RC8
#define FFC11_I PORTBbits.RB2
#define FFC12_I PORTCbits.RC7


// CTRL1  1 4 7 10 13 16 19
// CTRL2  2 5 8 11 14 17 20
// CTRL3  3 6 9 12 15 18 21
// CTRL4  D22E D22D D22C D22B D22A D22F D22G
// CTRL5  D23E D23D D23C D23B D23A D23F D23G
// CTRL6  D24E D24D D24C D24B D24A D24F D24G

extern enum ledNames;
extern int ledControlEnabled;

#define BTN_DURATION 100
#define BTN_HOLD_DURATION 30000

typedef struct {
    int isPressed;
    int isReleased;
    int pressDuration;
    int releaseDuration;
    int isActive;
} buttonStruct;

typedef struct {
    int play;
    int rewind;
    int fastForward;
    int load;
    int saving;
}interfaceStruct;
// =================== //
// FUNCTION PROTOTYPES // 
// =================== //
/* */
int pinInit();

/* [AK4386 INIT] initializes the AK4386 Digital-Analog Converter by turning on
 * the reference clock and making sure it's connected. */
int ak4386Init();

/*[READ BUTTONS] reads the current status of all the buttons and returns an
 * int value indicating which are held or not. The return int is right 
 * justified with the first 9 bits representing the 9 individual buttons.
 * 
 * 0 0 0 0 - 0 0 0 B9 - B8 B7 B6 B5 - B4 B3 B2 B1 */
void readButtons();
void linkInterface(interfaceStruct *interface);
void ctrlRewind(int buttonHold);
void ctrlPlay(int buttonHold);
void ctrlFastForward(int buttonHold);
void ctrlLoad(int buttonHold);
void ctrlSave(int buttonHold);
void ctrlLeft(int buttonHold);
void ctrlRight(int buttonHold);
void ctrlUp(int buttonHold);
void ctrlDown(int buttonHold);
void updateButton(buttonStruct *button, void (*functionPtr)(), int status);
void updateInterface();
void ledInit();
void updateAmplitude(int arrayIndex);
void updateLeds(int ledChoice);
void updateNumberDisplay();
void linkControlTimer(timingStruct *controlTimer);
void ledsOff();

/* [LED CONTROL DISABLE] disables the automatic LED control. Useful if you need
 * to use the 'singleLedControl(~)' or 'ledsOff()' functions. */
void ledControlDisable();

/* [LED CONTROL ENABLE] enables the automatic LED control. */
void ledControlEnable();

/* [SINGLE LED CONTROL] takes one parameter. 'int ledNumber' defines the LED to
 * activate. Whereas 'ledControl(~)' pulse width modulates the LEDs, this 
 * function supplies constant power so it will be considerably brighter. It is 
 * a useful for testing LEDs, just make sure to call 'ledControlDisable()' 
 * before using it. */
void singleLedControl(int ledNumber);

/* [LED CONTROL] takes two parameters. 'int currentLed' defines the next LED in 
 * the sequence to be controlled. 'int &ledIsOn' is the address of the array 
 * that defines whether that LED is ON or OFF. The values for 'currentLed' 
 * correlate with the values in the enum 'ledNames'.
 * 
 * 'currentLed' must increment by 1 as it will only disable the previous LED in 
 * the sequence. To control an individual LED, use the function:
 * 'void singleLedControl' */ 
void ledControl();

void tempoLedToggle();

/* [LED DISPLAY SEQUENCE] displays an entertaining LED show. */
void ledDisplaySequence();

/* [LED TIME DELAY] is a simple delay function using a for loop. */
void ledTimeDelay(int maxDelay);

void io_ffc1(int selection);
void io_ffc2(int selection);
void io_ffc3(int selection);
void io_ffc4(int selection);
void io_ffc5(int selection);
void io_ffc6(int selection);
void io_ffc7(int selection);
void io_ffc8(int selection);
void io_ffc9(int selection);
void io_ffc10(int selection);
void io_ffc11(int selection);
void io_ffc12(int selection);

void set_ffc1(int selection);
void set_ffc2(int selection);
void set_ffc3(int selection);
void set_ffc4(int selection);
void set_ffc5(int selection);
void set_ffc6(int selection);
void set_ffc7(int selection);
void set_ffc8(int selection);
void set_ffc9(int selection);
void set_ffc10(int selection);
void set_ffc11(int selection);
void set_ffc12(int selection);

int read_ffc1();
int read_ffc2();
int read_ffc3();
int read_ffc4();
int read_ffc5();
int read_ffc6();
int read_ffc7();
int read_ffc8();
int read_ffc9();
int read_ffc10();
int read_ffc11();
int read_ffc12();

void set_wp(int selection);

void ledDeviceFail();

void currentLedIncrement();

void tempoLedOn();
void tempoLedOff();

#endif	/* DSPIC33CH512MP506_PINS_H */


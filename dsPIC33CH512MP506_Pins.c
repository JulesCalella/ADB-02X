#include <xc.h>
#include <p33CH512MP506.h>
#include "dsPIC33CH512MP506_Pins.h"


// ** Golbal Variables ** //
enum ledNames{sine = 0, sawtooth, bpm, one, four, seven, f1,
              square, attack, memory, two, five, eight, f2,
              triangle, decay, tempo, three, six, nine, f3,
              d22e, d22d, d22c, d22b, d22a, d22f, d22g,
              d23e, d23d, d23c, d23b, d23a, d23f, d23g,
              d24e, d24d, d24c, d24b, d24a, d24f, d24g,};
              
int ledControlEnabled = 1;


// ** Functions ** //
int pinInit()
{
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
    
    // Configure AK4386 Pins
    // Set as I2S
    TRISDbits.TRISD4 = 0;   // DIF0
    LATDbits.LATD4 = 1;
    TRISCbits.TRISC11 = 0;  // DIF1
    LATCbits.LATC11 = 1;
    // Half Speed
    TRISDbits.TRISD1 = 0;   // DFS0
    LATDbits.LATD1 = 0;
    TRISDbits.TRISD2 = 0;   // DFS1
    LATDbits.LATD2 = 1;
    // Deemphasis Filter (OFF)
    TRISDbits.TRISD3 = 0;
    LATDbits.LATD3 = 0;
    
    TRISDbits.TRISD0 = 0;   // PDN
    LATDbits.LATD0 = 0;
    
    // Configure I2C Pins 
    
    // Configure I2S pins
    TRISBbits.TRISB13 = 0;
    TRISBbits.TRISB12 = 0;
    TRISBbits.TRISB11 = 0;
    TRISBbits.TRISB10 = 0;
    // SCK1 = 6; SDO1 = 5; SS1 = 7; REFCLKO = 14;
    RPOR6bits.RP45R = 14;   // MCLK = RP45/RB13
    RPOR6bits.RP44R = 6;    // BLCK = RP44/RB12
    RPOR5bits.RP43R = 5;    // SDTI = RP43/RB11
    RPOR5bits.RP42R = 7;    // LRCK = RP42/RB10
    
    // Configure Reference Clock (24kHz * 512 = 12.288 MHz)
    REFOCONLbits.ROEN = 0;
    REFOCONL = 0x1000;
    REFOCONH = 0x0003;
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

inline int readButtons()
{
    int buttons = 0;
    
    if(BTN1 == 1) buttons |= 0x0001;
    if(BTN2 == 1) buttons |= 0x0002;
    if(BTN3 == 1) buttons |= 0x0004;
    if(BTN4 == 1) buttons |= 0x0008;
    if(BTN5 == 1) buttons |= 0x0010;
    if(BTN6 == 1) buttons |= 0x0020;
    if(BTN7 == 1) buttons |= 0x0040;
    if(BTN8 == 1) buttons |= 0x0080;
    if(BTN9 == 1) buttons |= 0x0100;
    
    return buttons;
}

void ledsOff()
{
    LED1 = 0; LED2 = 0; LED3 = 0;
    LED4 = 0; LED5 = 0; LED6 = 0;
    LED6 = 0; LED7 = 0;
    
    CTRL1 = 0; CTRL2 = 0; CTRL3 = 0;
    CTRL4 = 0; CTRL5 = 0; CTRL6 = 0;
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
    LED1 = 0; LED2 = 0; LED3 = 0;
    LED4 = 0; LED5 = 0; LED6 = 0;
    LED6 = 0; LED7 = 0;
    
    CTRL1 = 0; CTRL2 = 0; CTRL3 = 0;
    CTRL4 = 0; CTRL5 = 0; CTRL6 = 0;
    
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

void ledControl(int currentLed, int *ledIsOn)
{
    if(ledControlEnabled == 0) return;
    
    switch(currentLed){
    // ** CTRL1 ** //
        // LED D1 -|>-
        case 0:
            CTRL6 = 0;
            LED7 = 0;
            CTRL1 = 1;
            
            if(ledIsOn[0] == 1){
                LED1 = 1;
            }
            
            break;
        
        // LED D4 -|>-
        case 1:
            LED1 = 0;
            
            if(ledIsOn[1] == 1){
                LED2 = 1;
            }
            
            break;
            
        // LED D7 -|>-
        case 2:
            LED2 = 0;
            
            if(ledIsOn[2] == 1){
                LED3 = 1;
            }
            
            break;
         
        // LED D10 -|>-
        case 3:
            LED3 = 0;
            
            if(ledIsOn[3] == 1){
                LED4 = 1;
            }
            
            break;
        
        // LED D13 -|>-
        case 4:
            LED4 = 0;
            
            if(ledIsOn[4] == 1){
                LED5 = 1;
            }
            
            break;
        
        // LED D16 -|>-
        case 5:
            LED5 = 0;
            
            if(ledIsOn[5] == 1){
                LED6 = 1;
            }
            
            break;
            
        // LED D19 -|>-
        case 6:
            LED6 = 0;
            
            if(ledIsOn[6] == 1){
                LED7 = 1;
            }
            
            break;
        
        
    // ** CTRL2 ** //
        // LED D2 -|>-
        case 7:
            CTRL1 = 0;
            LED7 = 0;
            CTRL2 = 1;
            
            if(ledIsOn[7] == 1){
                LED1 = 1;
            }
            
            break;
            
        // LED D5 -|>-
        case 8:
            LED1 = 0;
            
            if(ledIsOn[8] == 1){
                LED2 = 1;
            }
            
            break;
            
        // LED D8 -|>-
        case 9:
            LED2 = 0;
            
            if(ledIsOn[9] == 1){
                LED3 = 1;
            }
            
            break;
            
        // LED D11 -|>-
        case 10:
            LED3 = 0;
            
            if(ledIsOn[10] == 1){
                LED4 = 1;
            }
            
            break;
            
        // LED D14 -|>-
        case 11:
            LED4 = 0;
            
            if(ledIsOn[11] == 1){
                LED5 = 1;
            }
            
            break;
            
        // LED D17 -|>-
        case 12:
            LED5 = 0;
            
            if(ledIsOn[12] == 1){
                LED6 = 1;
            }
            
            break;
            
        // LED D20 -|>-
        case 13:
            LED6 = 0;
            
            if(ledIsOn[13] == 1){
                LED7 = 1;
            }
            
            break;
            
    // ** CTRL3 ** //
        // LED D3 -|>-
        case 14:
            CTRL2 = 0;
            LED7 = 0;
            CTRL3 = 1;
            
            if(ledIsOn[14] == 1){
                LED1 = 1;
            }
            
            break;
            
        // LED D6 -|>-
        case 15:
            LED1 = 0;
            
            if(ledIsOn[15] == 1){
                LED2 = 1;
            }
            
            break;
            
        // LED D9 -|>-
        case 16:
            LED2 = 0;
            
            if(ledIsOn[16] == 1){
                LED3 = 1;
            }
            
            break;
            
        // LED D12 -|>-
        case 17:
            LED3 = 0;
            
            if(ledIsOn[17] == 1){
                LED4 = 1;
            }
            
            break;
            
        // LED D15 -|>-
        case 18:
            LED4 = 0;
            
            if(ledIsOn[18] == 1){
                LED5 = 1;
            }
            
            break;
            
        // LED D18 -|>-
        case 19:
            LED5 = 0;
            
            if(ledIsOn[19] == 1){
                LED6 = 1;
            }
            
            break;
            
        // LED D21 -|>-
        case 20:
            LED6 = 0;
            
            if(ledIsOn[20] == 1){
                LED7 = 1;
            }
            
            break;
            
    // ** CTRL4 ** //
        // LED D22 -|>- E
        case 21:
            CTRL3 = 0;
            LED7 = 0;
            CTRL4 = 1;
            
            if(ledIsOn[21] == 1){
                LED1 = 1;
            }
            
            break;
            
        // LED D22 -|>- D
        case 22:
            LED1 = 0;
            
            if(ledIsOn[22] == 1){
                LED2 = 1;
            }
            
            break;
            
        // LED D22 -|>- C
        case 23:
            LED2 = 0;
            
            if(ledIsOn[23] == 1){
                LED3 = 1;
            }
            
            break;
            
        // LED D22 -|>- B
        case 24:
            LED3 = 0;
            
            if(ledIsOn[24] == 1){
                LED4 = 1;
            }
            
            break;
            
        // LED D22 -|>- A
        case 25:
            LED4 = 0;
            
            if(ledIsOn[25] == 1){
                LED5 = 1;
            }
            
            break;
            
        // LED D22 -|>- F
        case 26:
            LED5 = 0;
            
            if(ledIsOn[26] == 1){
                LED6 = 1;
            }
            
            break;
            
        // LED D22 -|>- G
        case 27:
            LED6 = 0;
            
            if(ledIsOn[27] == 1){
                LED7 = 1;
            }
            
            break;
            
    // ** CTRL5 ** //
        // LED D23 -|>- E
        case 28:
            CTRL4 = 0;
            LED7 = 0;
            CTRL5 = 1;
            
            if(ledIsOn[28] == 1){
                LED1 = 1;
            }
            
            break;
            
        // LED D23 -|>- D
        case 29:
            LED1 = 0;
            
            if(ledIsOn[29] == 1){
                LED2 = 1;
            }
            
            break;
            
        // LED D23 -|>- C
        case 30:
            LED2 = 0;
            
            if(ledIsOn[30] == 1){
                LED3 = 1;
            }
            
            break;
            
        // LED D23 -|>- B
        case 31:
            LED3 = 0;
            
            if(ledIsOn[31] == 1){
                LED4 = 1;
            }
            
            break;
            
        // LED D23 -|>- A
        case 32:
            LED4 = 0;
            
            if(ledIsOn[32] == 1){
                LED5 = 1;
            }
            
            break;
            
        // LED D23 -|>- F
        case 33:
            LED5 = 0;
            
            if(ledIsOn[33] == 1){
                LED6 = 1;
            }
            
            break;
            
        // LED D23 -|>- G
        case 34:
            LED6 = 0;
            
            if(ledIsOn[34] == 1){
                LED7 = 1;
            }
            
            break;
            
    // ** CTRL6 ** //
        // LED D24 -|>- E
        case 35:
            CTRL5 = 0;
            LED7 = 0;
            CTRL6 = 1;
            
            if(ledIsOn[35] == 1){
                LED1 = 1;
            }
            
            break;
            
        // LED D24 -|>- D
        case 36:
            LED1 = 0;
            
            if(ledIsOn[36] == 1){
                LED2 = 1;
            }
            
            break;
            
        // LED D24 -|>- C
        case 37:
            LED2 = 0;
            
            if(ledIsOn[37] == 1){
                LED3 = 1;
            }
            
            break;
            
        // LED D24 -|>- B
        case 38:
            LED3 = 0;
            
            if(ledIsOn[38] == 1){
                LED4 = 1;
            }
            
            break;
            
        // LED D24 -|>- A
        case 39:
            LED4 = 0;
            
            if(ledIsOn[39] == 1){
                LED5 = 1;
            }
            
            break;
            
        // LED D24 -|>- F
        case 40:
            LED5 = 0;
            
            if(ledIsOn[40] == 1){
                LED6 = 1;
            }
            
            break;
            
        // LED D24 -|>- G
        case 41:
            LED6 = 0;
            
            if(ledIsOn[41] == 1){
                LED7 = 1;
            }
            
            break;
    }
}

void ledDisplaySequence()
{
    int delay = 1000;
    
    ledControlDisable();
    
    singleLedControl(one); ledTimeDelay(delay);
    singleLedControl(sine); ledTimeDelay(delay);
    singleLedControl(two); ledTimeDelay(delay);
    singleLedControl(square); ledTimeDelay(delay);
    singleLedControl(three); ledTimeDelay(delay);
    singleLedControl(triangle); ledTimeDelay(delay);
    singleLedControl(four); ledTimeDelay(delay);
    singleLedControl(sawtooth); ledTimeDelay(delay);
    singleLedControl(five); ledTimeDelay(delay);
    singleLedControl(attack); ledTimeDelay(delay);
    singleLedControl(six); ledTimeDelay(delay);
    singleLedControl(decay); ledTimeDelay(delay);
    singleLedControl(seven); ledTimeDelay(delay);
    singleLedControl(bpm); ledTimeDelay(delay);
    singleLedControl(eight); ledTimeDelay(delay);
    singleLedControl(memory); ledTimeDelay(delay);
    singleLedControl(nine); ledTimeDelay(delay);
    singleLedControl(tempo); ledTimeDelay(delay);
    singleLedControl(memory); ledTimeDelay(delay);
    singleLedControl(bpm); ledTimeDelay(delay);
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
    
    for(delay=0; delay<maxDelay; delay++)
    {
        Nop();
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
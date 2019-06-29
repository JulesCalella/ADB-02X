#include <xc.h>
#include "dsPIC33CH512MP506_Modules.h"

int deviceInit()
{
    
    return 0;
}

int oscillatorInit()
{
    // Fpllo = Fplli * M / (N1 * N2 * N3)
    // Set the multipliers here
    CLKDIVbits.PLLPRE = 1;      // N1=1
    PLLFBDbits.PLLFBDIV = 125;  // M=125
    PLLDIVbits.POST1DIV = 5;    // N2=5
    PLLDIVbits.POST2DIV = 1;    // N3=1

    if(OSCCONbits.COSC == 0x3) return 2;
    
    // Check the current oscillator
    if(OSCCONbits.COSC == 0x0){ // If FRC        
        __builtin_write_OSCCONH(0x78);
        __builtin_write_OSCCONH(0x9A);
        __builtin_write_OSCCONH(0x03);
        
        __builtin_write_OSCCONL(0x46);
        __builtin_write_OSCCONL(0x57);
        __builtin_write_OSCCONL(0x01);
        
        while(OSCCONbits.OSWEN != 0){
            if(OSCCONbits.CF == 1){
                OSCCONbits.CF = 0;
                return -1;
            }
        }
        
        while(OSCCONbits.LOCK != 1);
        
    } else { // If something else, switch to FRC first
        __builtin_write_OSCCONH(0x78);
        __builtin_write_OSCCONH(0x9A);
        __builtin_write_OSCCONH(0x00);
        
        __builtin_write_OSCCONL(0x46);
        __builtin_write_OSCCONL(0x57);
        __builtin_write_OSCCONL(0x01);
        
        while(OSCCONbits.OSWEN != 0){
            if(OSCCONbits.CF == 1){
                OSCCONbits.CF = 0;
                return -1;
            }
        }
        
        Nop(); Nop(); Nop();
        Nop(); Nop(); Nop();
        
        return 1;
    }
        
    return 0;
}

// ========== SPI 1 ========== //
/* -----------------------------------------------------------------------------
 * SPI 1 INIT - initializes the SPI 1 module to operate as I2S in order to
 * communicate with the digital-to-analog converter. The SCK, SDO, and SS are
 * all controlled by the module. The clock is idle low and transfers data on a
 * change from idle to active (low to high).
 * ---------------------------------------------------------------------------*/
int spi1Init()
{
    SPI1CON1Lbits.SPIEN = 0;
    SPI1CON1L = 0x2A33; // 0010 1010 0011 0011
    SPI1CON1H = 0xF891;    // 1111 1000 1001 0001
    SPI1CON2L = 0x0017;
    SPI1IMSKL = 0x0000;
    
    return 0;
}

/* -----------------------------------------------------------------------------
 * SPI 1 ON - turns on the SPI 1 module.
 * ---------------------------------------------------------------------------*/
void spi1On()
{
    SPI1CON1Lbits.SPIEN = 1;
}

/* -----------------------------------------------------------------------------
 * SPI 1 OFF - turns off the SPI 1 module.
 * ---------------------------------------------------------------------------*/
void spi1Off()
{
    SPI1CON1Lbits.SPIEN = 0;
}

/* -----------------------------------------------------------------------------
 * SPI 1 SET BAUD RATE - takes one argument, the desired baud rate, and 
 * calculates the needed value for SPI1BRGL to the nearest integer. It will
 * return the percent error of the desired baud rate. To read the actual value 
 * of the current baudRate, call 'spi1ReadBaudRate()'.
 * ---------------------------------------------------------------------------*/
float  spi1SetBaudRate(int baudRate)
{
    spi1Off();
    
    // SPI1BRG = [Fpb / (2 * BaudRate)] - 1
    int temp = (( PERIPHERAL_CLOCK / (2.0 * baudRate) ) - 1) + 0.5;
    SPI1BRGL = temp;
    
    float percentError = ((spi1ReadBaudRate() - baudRate) / (baudRate * 1.0));
    
    return percentError;
}

/* -----------------------------------------------------------------------------
 * SPI 1 READ BAUD RATE - reads the current value of the baud rate as a floating
 * point number.
 * ---------------------------------------------------------------------------*/
float spi1ReadBaudRate()
{
    // Baud Rate = Fpb / (2 * (SPI1BRG + 1))
    return (PERIPHERAL_CLOCK / (2.0 * (SPI1BRGL + 1.0)));
}

/* -----------------------------------------------------------------------------
 * SPI 1 WRITE - takes two arguments, a low Byte and a high Byte.
 * ---------------------------------------------------------------------------*/
int spi1Write(int toSendDataHigh, int toSendDataLow)
{
    SPI1BUFL = toSendDataLow;
    SPI1BUFH = toSendDataHigh;
    
    return 0;
}

// ========== I2C 1 ========== //
int i2c1Init()
{
    
    return 0;
}

void i2c1On()
{
    
}

void i2c1Off()
{
    
}

void i2c1SetBaudRate()
{
    
}

int i2c1ReadBaudRate()
{
    
    return 0;
}


// ========== I2C 2 ========== //
int i2c2Init()
{
    
    return 0;
}

void i2c2On()
{
    
}

void i2c2Off()
{
    
}

void i2c2SetBaudRate()
{
    
}

int i2c2ReadBaudRate()
{
    
    return 0;
}

// ========== UART 1 ========== //
int uart1Init()
{
    
    return 0;
}

void uart1On()
{
    
}

void uart1Off()
{
    
}

void uart1SetBaudRate()
{
    
}

int uart1ReadBaudRate()
{
    
    return 0;
}
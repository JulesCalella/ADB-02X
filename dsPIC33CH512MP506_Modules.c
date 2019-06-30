#include <xc.h>
#include "dsPIC33CH512MP506_Modules.h"

/* -----------------------------------------------------------------------------
 * DEVICE INIT - 
 * ---------------------------------------------------------------------------*/
int deviceInit()
{
    
    return 0;
}

/* -----------------------------------------------------------------------------
 * OSCILLATOR INIT - 
 * ---------------------------------------------------------------------------*/
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
 * SPI 1 SET BAUD RATE - takes one argument, the desired baud rate in kiloHertz, 
 * and calculates the needed value for SPI1BRGL to the nearest integer. It will
 * return the percent error of the desired baud rate. To read the actual value 
 * of the current baudRate, call 'spi1ReadBaudRate()'.
 * ---------------------------------------------------------------------------*/
float  spi1SetBaudRate(int baudRate)
{
    spi1Off();
    
    // SPI1BRG = [Fpb / (2 * BaudRate)] - 1
    int temp = (( PERIPHERAL_CLOCK / (2000.0 * baudRate) ) - 1) + 0.5;
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
    return (PERIPHERAL_CLOCK / (2.0 * ((SPI1BRGL * 1000.0) + 1.0)));
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
/* -----------------------------------------------------------------------------
 * I2C 1 INIT - 
 * ---------------------------------------------------------------------------*/
int i2c1Init()
{
    I2C1CONLbits.I2CEN = 0;
    I2C1CONL = 0x0000;  // 0000 0000 0000 0000
    I2C1CONH = 0x0008;  // 0000 0000 0000 1000
    
    return 0;
}

/* -----------------------------------------------------------------------------
 * I2C 1 ON - 
 * ---------------------------------------------------------------------------*/
void i2c1On()
{
    I2C1CONLbits.I2CEN = 1;
}

/* -----------------------------------------------------------------------------
 * I2C 1 OFF - 
 * ---------------------------------------------------------------------------*/
void i2c1Off()
{
    I2C1CONLbits.I2CEN = 0;
}

/* -----------------------------------------------------------------------------
 * I2C 1 SET BAUD RATE - 
 * ---------------------------------------------------------------------------*/
float i2c1SetBaudRate(int baudRate)
{
    // I2C1BRG = [ (1/(Fscl - Delay) * Fp ] - 2 must be higher than 3
    int temp = ((((1.0/baudRate) * (1/1000.0)) - 0.00000015) * PERIPHERAL_CLOCK) - 1.5;
    if(temp <4) temp = 0x4;
    I2C1BRG = temp;
    
    return (i2c1ReadBaudRate() - baudRate) / (baudRate * 1.0);
}

/* -----------------------------------------------------------------------------
 * I2C 1 READ BAUD RATE - 
 * ---------------------------------------------------------------------------*/
float i2c1ReadBaudRate()
{
    float baudRate = PERIPHERAL_CLOCK / (I2C1BRG + 2.0 + (PERIPHERAL_CLOCK * 0.00000015));
    
    return baudRate;
}

/* -----------------------------------------------------------------------------
 * I2C 1 WRITE - 
 * ---------------------------------------------------------------------------*/
int i2c1Write(int devAddress, int regAddress, int toSendData, int numBytes)
{
    // Check if Busy
    if(I2C1CONLbits.PEN != 0){
        I2C1CONLbits.PEN = 1;
        return 1;
    }
    
    int timeoutCounter= 0;
    int timeoutMax = 0x3FFF;
    
    // Start
    I2C1CONLbits.SEN = 1;
    
    // Device Address with Write
    int transferData = devAddress & 0xFE;
    I2C1TRN = transferData;
    while(I2C1STATbits.TRSTAT == 1){
        timeoutCounter++;
        if(timeoutCounter >= timeoutMax) return -1;
    }
    
    // Register Address
    I2C1TRN = regAddress;
    while(I2C1STATbits.TRSTAT == 1){
        timeoutCounter++;
        if(timeoutCounter >= timeoutMax) return -1;
    }
    
    // Data Bytes
    if(numBytes == 1){
        I2C1TRN = toSendData;
        while(I2C1STATbits.TRSTAT == 1){
            timeoutCounter++;
            if(timeoutCounter >= timeoutMax) return -1;
        }
    } else if(numBytes == 2){
        I2C1TRN = toSendData >> 8;
        while(I2C1STATbits.TRSTAT == 1){
            timeoutCounter++;
            if(timeoutCounter >= timeoutMax) return -1;
        }
        
        I2C1TRN = toSendData & 0x00FF;
        while(I2C1STATbits.TRSTAT == 1){
            timeoutCounter++;
            if(timeoutCounter >= timeoutMax) return -1;
        }
    } else {
        // Error defining parameters
        I2C1CONLbits.PEN = 1;
        return -2;
    }
    
    // Stop
    I2C1CONLbits.PEN = 1;
    
    return 0;
}

/* -----------------------------------------------------------------------------
 * I2C 1 READ - 
 * ---------------------------------------------------------------------------*/
int i2c1Read(int devAddress, int regAddress, int *readData, int numBytes)
{
    // Check if Busy
    if(I2C1CONLbits.PEN != 0){
        I2C1CONLbits.PEN = 1;
        return 1;
    }
    
    int timeoutCounter= 0;
    int timeoutMax = 0x3FFF;
    int numReads;
    I2C1CONLbits.ACKDT = 0;
    
    // Start
    I2C1CONLbits.SEN = 1;
    
    // Device Address with Write
    int transferData = devAddress & 0xFE;
    I2C1TRN = transferData;
    while(I2C1STATbits.TRSTAT == 1){
        timeoutCounter++;
        if(timeoutCounter >= timeoutMax) return -1;
    }
    
    // Register Address
    I2C1TRN = regAddress;
    while(I2C1STATbits.TRSTAT == 1){
        timeoutCounter++;
        if(timeoutCounter >= timeoutMax) return -1;
    }
    
    // Restart
    I2C1CONLbits.RSEN = 1;
    
    // Device Address with Read
    transferData = devAddress | 0x01;
    I2C1TRN = transferData;
    while(I2C1STATbits.TRSTAT == 1){
        timeoutCounter++;
        if(timeoutCounter >= timeoutMax) return -1;
    }
    
    // Data Bytes
     for(numReads=1; numReads<=numBytes; numReads++){
        if(numReads == numBytes){
            // On the final Byte; Send a NACK
            I2C1CONLbits.ACKDT = 1;
        }
        // Wait for data to be received
        while(I2C1STATbits.RBF == 0){
            timeoutCounter++;
            if(timeoutCounter >= timeoutMax) return -1;
        }
        
        readData[numReads-1] = I2C1RCV;
        I2C1CONLbits.ACKEN = 1;
        // Wait for ACK/NACK to send
        while(I2C1CONLbits.ACKEN == 1);
    }
    
    // Stop
    I2C1CONLbits.PEN = 1;
    
    return 0;
}


// ========== I2C 2 ========== //
/* -----------------------------------------------------------------------------
 * I2C 2 INIT - 
 * ---------------------------------------------------------------------------*/
int i2c2Init()
{
    
    return 0;
}

/* -----------------------------------------------------------------------------
 * I2C 2 ON - 
 * ---------------------------------------------------------------------------*/
void i2c2On()
{
    
}

/* -----------------------------------------------------------------------------
 * I2C 2 OFF - 
 * ---------------------------------------------------------------------------*/
void i2c2Off()
{
    
}

/* -----------------------------------------------------------------------------
 * I2C 2 SET BAUD RATE - 
 * ---------------------------------------------------------------------------*/
float i2c2SetBaudRate()
{
    return 0.0;
}

/* -----------------------------------------------------------------------------
 * I2C 2 READ BAUD RATE - 
 * ---------------------------------------------------------------------------*/
float i2c2ReadBaudRate()
{
    
    return 0.0;
}

/* -----------------------------------------------------------------------------
 * I2C 2 WRITE - 
 * ---------------------------------------------------------------------------*/
int i2c2Write(int devAddress, int regAddress, int toSendData, int numBytes)
{
    
    return 0;
}

/* -----------------------------------------------------------------------------
 * I2C 2 READ - 
 * ---------------------------------------------------------------------------*/
int i2c2Read(int devAddress, int regAddress, int *readData, int numBytes)
{
    
    return 0;
}

// ========== UART 1 ========== //
/* -----------------------------------------------------------------------------
 * UART 1 INIT - 
 * ---------------------------------------------------------------------------*/
int uart1Init()
{
    
    return 0;
}

/* -----------------------------------------------------------------------------
 * UART 1 ON - 
 * ---------------------------------------------------------------------------*/
void uart1On()
{
    
}

/* -----------------------------------------------------------------------------
 * UART 1 OFF - 
 * ---------------------------------------------------------------------------*/
void uart1Off()
{
    
}

/* -----------------------------------------------------------------------------
 * UART 1 SET BAUD RATE - 
 * ---------------------------------------------------------------------------*/
float uart1SetBaudRate()
{
    
    return 0.0;
}

/* -----------------------------------------------------------------------------
 * UART 1 READ BAUD RATE - 
 * ---------------------------------------------------------------------------*/
float uart1ReadBaudRate()
{
    
    return 0.0;
}
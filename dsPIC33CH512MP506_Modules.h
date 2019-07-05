/* 
 * File:   dsPIC33CH512MP506_Oscillator.h
 * Author: Jules
 *
 * Created on June 26, 2019, 11:24 AM
 */

#ifndef DSPIC33CH512MP506_MODULES_H
#define	DSPIC33CH512MP506_MODULES_H

#define FRC 0
#define HS 1
#define XT 2
#define EC 3

#define SYSTEM_CLOCK 96000000
#define PERIPHERAL_CLOCK 48000000

// Function Prototypes 
int deviceInit();

int oscillatorInit();
int spi1Init();
void spi1On();
void spi1Off();
float spi1SetBaudRate(int baudRate);
float spi1ReadBaudRate();
int spi1Write(int toSendData);

int i2c1Init();
void i2c1On();
void i2c1Off();
float i2c1SetBaudRate();
float i2c1ReadBaudRate();
int i2c1Write(int devAddress, int regAddress, int toSendData, int numBytes);
int i2c1Read(int devAddress, int regAddress, int *readData, int numBytes);

int i2c2Init();
void i2c2On();
void i2c2Off();
float i2c2SetBaudRate();
float i2c2ReadBaudRate();
int i2c2Write(int devAddress, int regAddress, int toSendData, int numBytes);
int i2c2Read(int devAddress, int regAddress, int *readData, int numBytes);

int uart1Init();
void uart1On();
void uart1Off();
float uart1SetBaudRate();
float uart1ReadBaudRate();
float uart1SetBaudRate();
float uart1ReadBaudRate();

void dacInit();

int eepromWrite(int devAddress, int regAddressH, int regAddressL, int *toSendData, int startElement, int numBytes);
int eepromRead(int devAddress, int regAddressH, int regAddressL, int *toReadData, int startElement, int numBytes);

#endif	/* DSPIC33CH512MP506_MODULES_H */


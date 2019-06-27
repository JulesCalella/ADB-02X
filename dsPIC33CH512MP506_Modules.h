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

// Function Prototypes 
int deviceInit();

int oscillatorInit();

int spi1Init();

int i2c1Init();

int i2c2Init();

int uart1Init();

#endif	/* DSPIC33CH512MP506_MODULES_H */


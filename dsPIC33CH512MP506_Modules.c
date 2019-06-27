#include <xc.h>
#include "dsPIC33CH512MP506_Modules.h"

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
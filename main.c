
// DSPIC33CH512MP506 Configuration Bit Settings

// 'C' source line config statements

// FSEC
#pragma config BWRP = ON                // Boot Segment Write-Protect bit (Boot Segment is write protected)
#pragma config BSS = DISABLED           // Boot Segment Code-Protect Level bits (No Protection (other than BWRP))
#pragma config BSEN = OFF               // Boot Segment Control bit (No Boot Segment)
#pragma config GWRP = OFF               // General Segment Write-Protect bit (General Segment may be written)
#pragma config GSS = DISABLED           // General Segment Code-Protect Level bits (No Protection (other than GWRP))
#pragma config CWRP = OFF               // Configuration Segment Write-Protect bit (Configuration Segment may be written)
#pragma config CSS = DISABLED           // Configuration Segment Code-Protect Level bits (No Protection (other than CWRP))
#pragma config AIVTDIS = OFF            // Alternate Interrupt Vector Table bit (Disabled AIVT)

// FBSLIM
#pragma config BSLIM = 0x1FFF           // Boot Segment Flash Page Address Limit bits (Enter Hexadecimal value)

// FSIGN

// FOSCSEL
#pragma config FNOSC = PRI              // Oscillator Source Selection (Primary Oscillator (XT, HS, EC))
#pragma config IESO = ON                // Two-speed Oscillator Start-up Enable bit (Start up device with FRC, then switch to user-selected oscillator source)

// FOSC
#pragma config POSCMD = EC              // Primary Oscillator Mode Select bits (EC (External Clock) Mode)
#pragma config OSCIOFNC = ON            // OSC2 Pin Function bit (OSC2 is general purpose digital I/O pin)
#pragma config FCKSM = CSECME           // Clock Switching Mode bits (Both Clock switching and Fail-safe Clock Monitor are enabled)
#pragma config PLLKEN = PLLKEN_ON       // PLL Lock Status Control (PLL lock signal will be used to disable PLL clock output if lock is lost)
#pragma config XTCFG = G3               // XT Config (24-32 MHz crystals)
#pragma config XTBST = DISABLE          // XT Boost (Default kick-start)

// FWDT
#pragma config RWDTPS = PS2147483648    // Run Mode Watchdog Timer Post Scaler select bits (1:2147483648)
#pragma config RCLKSEL = LPRC           // Watchdog Timer Clock Select bits (Always use LPRC)
#pragma config WINDIS = OFF             // Watchdog Timer Window Enable bit (Watchdog Timer operates in Window mode)
#pragma config WDTWIN = WIN25           // Watchdog Timer Window Select bits (WDT Window is 25% of WDT period)
#pragma config SWDTPS = PS2147483648    // Sleep Mode Watchdog Timer Post Scaler select bits (1:2147483648)
#pragma config FWDTEN = ON_SW           // Watchdog Timer Enable bit (WDT controlled via SW, use WDTCON.ON bit)

// FPOR
#pragma config BISTDIS = DISABLED       // Memory BIST Feature Disable (mBIST on reset feature disabled)

// FICD
#pragma config ICS = PGD2               // ICD Communication Channel Select bits (Communicate on PGC2 and PGD2)
#pragma config JTAGEN = OFF             // JTAG Enable bit (JTAG is disabled)
#pragma config NOBTSWP = OFF            // BOOTSWP instruction disable bit (BOOTSWP instruction is disabled)

// FDMTIVTL
#pragma config DMTIVTL = 0xFFFF         // Dead Man Timer Interval low word (Enter Hexadecimal value)

// FDMTIVTH
#pragma config DMTIVTH = 0xFFFF         // Dead Man Timer Interval high word (Enter Hexadecimal value)

// FDMTCNTL
#pragma config DMTCNTL = 0xFFFF         // Lower 16 bits of 32 bit DMT instruction count time-out value (0-0xFFFF) (Enter Hexadecimal value)

// FDMTCNTH
#pragma config DMTCNTH = 0xFFFF         // Upper 16 bits of 32 bit DMT instruction count time-out value (0-0xFFFF) (Enter Hexadecimal value)

// FDMT
#pragma config DMTDIS = OFF             // Dead Man Timer Disable bit (Dead Man Timer is Disabled and can be enabled by software)

// FDEVOPT
#pragma config ALTI2C1 = OFF            // Alternate I2C1 Pin bit (I2C1 mapped to SDA1/SCL1 pins)
#pragma config ALTI2C2 = OFF            // Alternate I2C2 Pin bit (I2C2 mapped to SDA2/SCL2 pins)
#pragma config SMBEN = SMBUS            // SM Bus Enable (SMBus input threshold is enabled)
#pragma config SPI2PIN = PPS            // SPI2 Pin Select bit (SPI2 uses I/O remap (PPS) pins)

// FALTREG
#pragma config CTXT1 = OFF              // Specifies Interrupt Priority Level (IPL) Associated to Alternate Working Register 1 bits (Not Assigned)
#pragma config CTXT2 = OFF              // Specifies Interrupt Priority Level (IPL) Associated to Alternate Working Register 2 bits (Not Assigned)
#pragma config CTXT3 = OFF              // Specifies Interrupt Priority Level (IPL) Associated to Alternate Working Register 3 bits (Not Assigned)
#pragma config CTXT4 = OFF              // Specifies Interrupt Priority Level (IPL) Associated to Alternate Working Register 4 bits (Not Assigned)

// FMBXM
#pragma config MBXM0 = S2M              // Mailbox 0 data direction (Mailbox register configured for Master data read (Slave to Master data transfer))
#pragma config MBXM1 = S2M              // Mailbox 1 data direction (Mailbox register configured for Master data read (Slave to Master data transfer))
#pragma config MBXM2 = S2M              // Mailbox 2 data direction (Mailbox register configured for Master data read (Slave to Master data transfer))
#pragma config MBXM3 = S2M              // Mailbox 3 data direction (Mailbox register configured for Master data read (Slave to Master data transfer))
#pragma config MBXM4 = S2M              // Mailbox 4 data direction (Mailbox register configured for Master data read (Slave to Master data transfer))
#pragma config MBXM5 = S2M              // Mailbox 5 data direction (Mailbox register configured for Master data read (Slave to Master data transfer))
#pragma config MBXM6 = S2M              // Mailbox 6 data direction (Mailbox register configured for Master data read (Slave to Master data transfer))
#pragma config MBXM7 = S2M              // Mailbox 7 data direction (Mailbox register configured for Master data read (Slave to Master data transfer))
#pragma config MBXM8 = S2M              // Mailbox 8 data direction (Mailbox register configured for Master data read (Slave to Master data transfer))
#pragma config MBXM9 = S2M              // Mailbox 9 data direction (Mailbox register configured for Master data read (Slave to Master data transfer))
#pragma config MBXM10 = S2M             // Mailbox 10 data direction (Mailbox register configured for Master data read (Slave to Master data transfer))
#pragma config MBXM11 = S2M             // Mailbox 11 data direction (Mailbox register configured for Master data read (Slave to Master data transfer))
#pragma config MBXM12 = S2M             // Mailbox 12 data direction (Mailbox register configured for Master data read (Slave to Master data transfer))
#pragma config MBXM13 = S2M             // Mailbox 13 data direction (Mailbox register configured for Master data read (Slave to Master data transfer))
#pragma config MBXM14 = S2M             // Mailbox 14 data direction (Mailbox register configured for Master data read (Slave to Master data transfer))
#pragma config MBXM15 = S2M             // Mailbox 15 data direction (Mailbox register configured for Master data read (Slave to Master data transfer))

// FMBXHS1
#pragma config MBXHSA = MBX15           // Mailbox handshake protocol block A register assignment (MSIxMBXD15 assigned to mailbox handshake protocol block A)
#pragma config MBXHSB = MBX15           // Mailbox handshake protocol block B register assignment (MSIxMBXD15 assigned to mailbox handshake protocol block B)
#pragma config MBXHSC = MBX15           // Mailbox handshake protocol block C register assignment (MSIxMBXD15 assigned to mailbox handshake protocol block C)
#pragma config MBXHSD = MBX15           // Mailbox handshake protocol block D register assignment (MSIxMBXD15 assigned to mailbox handshake protocol block D)

// FMBXHS2
#pragma config MBXHSE = MBX15           // Mailbox handshake protocol block E register assignment (MSIxMBXD15 assigned to mailbox handshake protocol block E)
#pragma config MBXHSF = MBX15           // Mailbox handshake protocol block F register assignment (MSIxMBXD15 assigned to mailbox handshake protocol block F)
#pragma config MBXHSG = MBX15           // Mailbox handshake protocol block G register assignment (MSIxMBXD15 assigned to mailbox handshake protocol block G)
#pragma config MBXHSH = MBX15           // Mailbox handshake protocol block H register assignment (MSIxMBXD15 assigned to mailbox handshake protocol block H)

// FMBXHSEN
#pragma config HSAEN = OFF              // Mailbox A data flow control protocol block enable (Mailbox data flow control handshake protocol block disabled.)
#pragma config HSBEN = OFF              // Mailbox B data flow control protocol block enable (Mailbox data flow control handshake protocol block disabled.)
#pragma config HSCEN = OFF              // Mailbox C data flow control protocol block enable (Mailbox data flow control handshake protocol block disabled.)
#pragma config HSDEN = OFF              // Mailbox D data flow control protocol block enable (Mailbox data flow control handshake protocol block disabled.)
#pragma config HSEEN = OFF              // Mailbox E data flow control protocol block enable (Mailbox data flow control handshake protocol block disabled.)
#pragma config HSFEN = OFF              // Mailbox F data flow control protocol block enable (Mailbox data flow control handshake protocol block disabled.)
#pragma config HSGEN = OFF              // Mailbox G data flow control protocol block enable (Mailbox data flow control handshake protocol block disabled.)
#pragma config HSHEN = OFF              // Mailbox H data flow control protocol block enable (Mailbox data flow control handshake protocol block disabled.)

// FCFGPRA0
#pragma config CPRA0 = MSTR             // Pin RA0 Ownership Bits (Master core owns pin.)
#pragma config CPRA1 = MSTR             // Pin RA1 Ownership Bits (Master core owns pin.)
#pragma config CPRA2 = MSTR             // Pin RA2 Ownership Bits (Master core owns pin.)
#pragma config CPRA3 = MSTR             // Pin RA3 Ownership Bits (Master core owns pin.)
#pragma config CPRA4 = MSTR             // Pin RA4 Ownership Bits (Master core owns pin.)

// FCFGPRB0
#pragma config CPRB0 = MSTR             // Pin RB0 Ownership Bits (Master core owns pin.)
#pragma config CPRB1 = MSTR             // Pin RB1 Ownership Bits (Master core owns pin.)
#pragma config CPRB2 = MSTR             // Pin RB2 Ownership Bits (Master core owns pin.)
#pragma config CPRB3 = MSTR             // Pin RB3 Ownership Bits (Master core owns pin.)
#pragma config CPRB4 = MSTR             // Pin RB4 Ownership Bits (Master core owns pin.)
#pragma config CPRB5 = MSTR             // Pin RB5 Ownership Bits (Master core owns pin.)
#pragma config CPRB6 = MSTR             // Pin RB6 Ownership Bits (Master core owns pin.)
#pragma config CPRB7 = MSTR             // Pin RB7 Ownership Bits (Master core owns pin.)
#pragma config CPRB8 = MSTR             // Pin RB8 Ownership Bits (Master core owns pin.)
#pragma config CPRB9 = MSTR             // Pin RB9 Ownership Bits (Master core owns pin.)
#pragma config CPRB10 = MSTR            // Pin RB10 Ownership Bits (Master core owns pin.)
#pragma config CPRB11 = MSTR            // Pin RB11 Ownership Bits (Master core owns pin.)
#pragma config CPRB12 = MSTR            // Pin RB12 Ownership Bits (Master core owns pin.)
#pragma config CPRB13 = MSTR            // Pin RB13 Ownership Bits (Master core owns pin.)
#pragma config CPRB14 = MSTR            // Pin RB14 Ownership Bits (Master core owns pin.)
#pragma config CPRB15 = MSTR            // Pin RB15 Ownership Bits (Master core owns pin.)

// FCFGPRC0
#pragma config CPRC0 = MSTR             // Pin RC0 Ownership Bits (Master core owns pin.)
#pragma config CPRC1 = MSTR             // Pin RC1 Ownership Bits (Master core owns pin.)
#pragma config CPRC2 = MSTR             // Pin RC2 Ownership Bits (Master core owns pin.)
#pragma config CPRC3 = MSTR             // Pin RC3 Ownership Bits (Master core owns pin.)
#pragma config CPRC4 = MSTR             // Pin RC4 Ownership Bits (Master core owns pin.)
#pragma config CPRC5 = MSTR             // Pin RC5 Ownership Bits (Master core owns pin.)
#pragma config CPRC6 = MSTR             // Pin RC6 Ownership Bits (Master core owns pin.)
#pragma config CPRC7 = MSTR             // Pin RC7 Ownership Bits (Master core owns pin.)
#pragma config CPRC8 = MSTR             // Pin RC8 Ownership Bits (Master core owns pin.)
#pragma config CPRC9 = MSTR             // Pin RC9 Ownership Bits (Master core owns pin.)
#pragma config CPRC10 = MSTR            // Pin RC10 Ownership Bits (Master core owns pin.)
#pragma config CPRC11 = MSTR            // Pin RC11 Ownership Bits (Master core owns pin.)
#pragma config CPRC12 = MSTR            // Pin RC12 Ownership Bits (Master core owns pin.)
#pragma config CPRC13 = MSTR            // Pin RC13 Ownership Bits (Master core owns pin.)
#pragma config CPRC14 = MSTR            // Pin RC14 Ownership Bits (Master core owns pin.)
#pragma config CPRC15 = MSTR            // Pin RC15 Ownership Bits (Master core owns pin.)

// FCFGPRD0
#pragma config CPRD0 = MSTR             // Pin RD0 Ownership Bits (Master core owns pin.)
#pragma config CPRD1 = MSTR             // Pin RD1 Ownership Bits (Master core owns pin.)
#pragma config CPRD2 = MSTR             // Pin RD2 Ownership Bits (Master core owns pin.)
#pragma config CPRD3 = MSTR             // Pin RD3 Ownership Bits (Master core owns pin.)
#pragma config CPRD4 = MSTR             // Pin RD4 Ownership Bits (Master core owns pin.)
#pragma config CPRD5 = MSTR             // Pin RD5 Ownership Bits (Master core owns pin.)
#pragma config CPRD6 = MSTR             // Pin RD6 Ownership Bits (Master core owns pin.)
#pragma config CPRD7 = MSTR             // Pin RD7 Ownership Bits (Master core owns pin.)
#pragma config CPRD8 = MSTR             // Pin RD8 Ownership Bits (Master core owns pin.)
#pragma config CPRD9 = MSTR             // Pin RD9 Ownership Bits (Master core owns pin.)
#pragma config CPRD10 = MSTR            // Pin RD10 Ownership Bits (Master core owns pin.)
#pragma config CPRD11 = MSTR            // Pin RD11 Ownership Bits (Master core owns pin.)
#pragma config CPRD12 = MSTR            // Pin RD12 Ownership Bits (Master core owns pin.)
#pragma config CPRD13 = MSTR            // Pin RD13 Ownership Bits (Master core owns pin.)
#pragma config CPRD14 = MSTR            // Pin RD14 Ownership Bits (Master core owns pin.)
#pragma config CPRD15 = MSTR            // Pin RD15 Ownership Bits (Master core owns pin.)

// FCFGPRE0
#pragma config CPRE0 = MSTR             // Pin RE0 Ownership Bits (Master core owns pin.)
#pragma config CPRE1 = MSTR             // Pin RE1 Ownership Bits (Master core owns pin.)
#pragma config CPRE2 = MSTR             // Pin RE2 Ownership Bits (Master core owns pin.)
#pragma config CPRE3 = MSTR             // Pin RE3 Ownership Bits (Master core owns pin.)
#pragma config CPRE4 = MSTR             // Pin RE4 Ownership Bits (Master core owns pin.)
#pragma config CPRE5 = MSTR             // Pin RE5 Ownership Bits (Master core owns pin.)
#pragma config CPRE6 = MSTR             // Pin RE6 Ownership Bits (Master core owns pin.)
#pragma config CPRE7 = MSTR             // Pin RE7 Ownership Bits (Master core owns pin.)
#pragma config CPRE8 = MSTR             // Pin RE8 Ownership Bits (Master core owns pin.)
#pragma config CPRE9 = MSTR             // Pin RE9 Ownership Bits (Master core owns pin.)
#pragma config CPRE10 = MSTR            // Pin RE10 Ownership Bits (Master core owns pin.)
#pragma config CPRE11 = MSTR            // Pin RE11 Ownership Bits (Master core owns pin.)
#pragma config CPRE12 = MSTR            // Pin RE12 Ownership Bits (Master core owns pin.)
#pragma config CPRE13 = MSTR            // Pin RE13 Ownership Bits (Master core owns pin.)
#pragma config CPRE14 = MSTR            // Pin RE14 Ownership Bits (Master core owns pin.)
#pragma config CPRE15 = MSTR            // Pin RE15 Ownership Bits (Master core owns pin.)

// FS1OSCSEL
#pragma config S1FNOSC = PRI            // Oscillator Source Selection (Primary Oscillator (XT, HS, EC))
#pragma config S1IESO = ON              // Two-speed Oscillator Start-up Enable bit (Start up device with FRC, then switch to user-selected oscillator source)

// FS1OSC
#pragma config S1OSCIOFNC = ON          // Slave OSC2 Pin Function bit (OSC2 is general purpose digital I/O pin)
#pragma config S1FCKSM = CSECME         // Clock Switching Mode bits (Both Clock switching and Fail-safe Clock Monitor are enabled)
#pragma config S1PLLKEN = S1PLLKEN_ON   // S1PLLKEN (S1PLLKEN_ON)

// FS1WDT
#pragma config S1RWDTPS = PS1048576     // Run Mode Watchdog Timer Post Scaler select bits (1:1048576)
#pragma config S1RCLKSEL = LPRC         // Watchdog Timer Clock Select bits (Always use LPRC)
#pragma config S1WINDIS = ON            // Watchdog Timer Window Enable bit (Watchdog Timer operates in Non-Window mode)
#pragma config S1WDTWIN = WIN25         // Watchdog Timer Window Select bits (WDT Window is 25% of WDT period)
#pragma config S1SWDTPS = PS1048576     // Sleep Mode Watchdog Timer Post Scaler select bits (1:1048576)
#pragma config S1FWDTEN = ON_SW         // Watchdog Timer Enable bit (WDT controlled via WDTCON.ON bit)

// FS1POR
#pragma config S1BISTDIS = DISABLED     // Slave BIST on reset disable bit (Slave BIST on reset feature disabled)

// FS1ICD
#pragma config S1ICS = PGD1             // ICD Communication Channel Select bits (Communicate on PGC1 and PGD1)
#pragma config S1ISOLAT = ON            // Isolate the Slave core subsystem from the master subsystem during Debug (The slave can operate (in debug mode) even if the SLVEN bit in the MSI is zero.)
#pragma config S1NOBTSWP = OFF          // BOOTSWP Instruction Enable/Disable bit (BOOTSWP instruction is disabled)

// FS1DEVOPT
#pragma config S1ALTI2C1 = OFF          // Alternate I2C1 Pin bit (I2C1 mapped to SDA1/SCL1 pins)
#pragma config S1SPI1PIN = PPS          // S1 SPI1 Pin Select bit (Slave SPI1 uses I/O remap (PPS) pins)
#pragma config S1SSRE = ON              // Slave Slave Reset Enable (Slave generated resets will reset the Slave Enable Bit in the MSI module)
#pragma config S1MSRE = ON              // Master Slave Reset Enable (The master software oriented RESET events (RESET Op-Code, Watchdog timeout, TRAP reset, illegalInstruction) will also cause the slave subsystem to reset.)

// FS1ALTREG
#pragma config S1CTXT1 = OFF            // Specifies Interrupt Priority Level (IPL) Associated to Alternate Working Register 1 bits (Not Assigned)
#pragma config S1CTXT2 = OFF            // Specifies Interrupt Priority Level (IPL) Associated to Alternate Working Register 2 bits (Not Assigned)
#pragma config S1CTXT3 = OFF            // Specifies Interrupt Priority Level (IPL) Associated to Alternate Working Register 3 bits (Not Assigned)
#pragma config S1CTXT4 = OFF            // Specifies Interrupt Priority Level (IPL) Associated to Alternate Working Register 4 bits (Not Assigned)

// FS1DMTIVTL
#pragma config S1DMTIVTL = 0xFFFF       // Slave Dead Man Timer Interval low word (Enter Hexadecimal value)

// FS1DMTIVTH
#pragma config S1DMTIVTH = 0xFFFF       // Slave Dead Man Timer Interval high word (Enter Hexadecimal value)

// FS1DMTCNTL
#pragma config S1DMTCNTL = 0xFFFF       // Slave DMT instruction count time-out value low word (Enter Hexadecimal value)

// FS1DMTCNTH
#pragma config S1DMTCNTH = 0xFFFF       // Slave DMT instruction count time-out value high word (Enter Hexadecimal value)

// FS1DMT
#pragma config S1DMTDIS = OFF           // Slave Dead Man Timer Disable bit (Slave Dead Man Timer is Disabled and can be enabled by software)

// FBTSEQ
#pragma config BSEQ = 0xFFF             // Relative value defining which partition will be active after devie Reset; the partition containing a lower boot number will be active. (Enter Hexadecimal value)
#pragma config IBSEQ = 0xFFF            // The one's complement of BSEQ; must be calculated by the user and written during device programming. (Enter Hexadecimal value)

// #pragma config statements should precede project file includes.
// Use project enums instead of #define for ON and OFF.

#include <xc.h>
#include "dsPIC33CH512MP506_Modules.h"
#include "dsPIC33CH512MP506_Pins.h"
#include "dsPIC33CH512MP506_Audio.h"

volatile int newOutput;
volatile int timerCalled;
timingStruct timer;
interfaceStruct interface;

void __attribute__((__interrupt__, no_auto_psv)) _T1Interrupt(void)
{
    currentLedIncrement();
    
    if(interface.play != 0) timerCalled++;
    
    IFS0bits.T1IF = 0;
}

void __attribute__((__interrupt__, no_auto_psv)) _SPI1TXInterrupt(void)
{
    newOutput = 1;
    
    IFS0bits.SPI1TXIF = 0;
}

void __attribute__((__interrupt__, no_auto_psv)) _U1RXInterrupt(void)
{
    // IFS0bits.U1RXIF
    // IEC0bits.U1RXIE
}

void __attribute__((__interrupt__, no_auto_psv)) _U1TXInterrupt(void)
{
    // IFS0bits.U1TXIF
    // IEC0bits.U1TXIE
}

void updateTimer()
{
    timer.interruptCount += timerCalled;
    timerCalled = 0;
    
    if(timer.interruptCount >= timer.location64Max){
        timer.interruptCount = 0;
        timer.location64++;

        if(timer.location64 >= 64){
            timer.measure++;
            timer.location64 = 0;
        }

        if(timer.location64 == 0){
            tempoLedOn();
        } else if((timer.location64 % 16) == 0){
            tempoLedOn();
        } else if(((timer.location64 % 16) - 2) == 0){
            tempoLedOff();
        }
    }
}

int main(void)
{
    int ret;
    
    // Oscillator Setup
    ret = oscillatorInit();
    if(ret == 1){ // Device was set to FRC.
        ret = oscillatorInit();
    }
    else if(ret == -1){
        while(1){ // Clock Fail
               ledDeviceFail();
        }
    }
    
    // Startup Delay for Oscillator Stabilizing
    for(ret=0; ret<10000; ret++) continue;
    
    // Pin Setup
    pinInit();
    
    // I2C Setup
    i2c1Init();
    i2c1SetBaudRate(100);
    i2c2Init();
    i2c2SetBaudRate(100);
    
    // I2S/SPI Setup
    spi1Init();
    
    // AK4386 Setup
    dacInit();
    
    // Timer Setup   
    
    //ledDisplaySequence();
    ledsOff();
    
    defaultWaveformInit();
    generateAllWaveforms();
    
    writeSong();
    
    newOutput = 1;
    timerCalled = 0;
    notesInit(&newOutput);
    
    linkTimer(&timer);
    linkInterface(&interface);
    ledInit();
    updateOutputBuffer(&newOutput);
    
    // Activate Interrupts just before main loop
    timer1Init();
    spi1On();
    i2c1On();
    i2c2On();
    uart1On(); 
    
    //int send = 0;
    
    while(1)
    {
        updateTimer();
        readScoreArray();
        if(interface.play != 0) updateOutputBuffer(&newOutput);
        updateInterface();
    }
    
    return 0;
}


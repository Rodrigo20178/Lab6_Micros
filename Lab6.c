/*
 * File:   Lab6.c
 * Author: Rodrigo García
 *
 * Created on 24 de marzo de 2023, 03:56 PM
 */

#pragma config FOSC = INTRC_CLKOUT// Oscillator Selection bits (INTOSC oscillator: CLKOUT function on RA6/OSC2/CLKOUT pin, I/O function on RA7/OSC1/CLKIN)
#pragma config WDTE = OFF        // Watchdog Timer Enable bit (WDT enabled)
#pragma config PWRTE = ON      // Power-up Timer Enable bit (PWRT disabled)
#pragma config MCLRE = OFF       // RE3/MCLR pin function select bit (RE3/MCLR pin function is MCLR)
#pragma config CP = OFF         // Code Protection bit (Program memory code protection is disabled)
#pragma config CPD = OFF        // Data Code Protection bit (Data memory code protection is disabled)
#pragma config BOREN = OFF      // Brown Out Reset Selection bits (BOR disabled)
#pragma config IESO = OFF       // Internal External Switchover bit (Internal/External Switchover mode is disabled)
#pragma config FCMEN = OFF       // Fail-Safe Clock Monitor Enabled bit (Fail-Safe Clock Monitor is enabled)
#pragma config LVP = OFF        // Low Voltage Programming Enable bit (RB3 pin has digital I/O, HV on MCLR must be used for programming)

// CONFIG2
#pragma config BOR4V = BOR40V   // Brown-out Reset Selection bit (Brown-out Reset set to 4.0V)
#pragma config WRT = OFF        // Flash Program Memory Self Write Enable bits (Write protection off)

// #pragma config statements should precede project file includes.
// Use project enums instead of #define for ON and OFF.

#include <xc.h>
#include <stdint.h>
#define  _XTAL_FREQ  4000000


void setup(void);

void __interrupt() isr(void){
    if(PIR1bits.ADIF){
        if (ADCON0bits.CHS == 0)
            PORTC = ADRESH;
        else
            PORTD = ADRESH;
        PIR1bits.ADIF = 0;
    }
}

void main(void) {
    
    setup();
    ADCON0bits.GO = 1;
    while (1){
        if(ADCON0bits.GO == 0){
            if(ADCON0bits.CHS == 1)
                ADCON0bits.CHS = 0;
            else
                ADCON0bits.CHS = 1;
            __delay_us(50);
            ADCON0bits.GO = 1;
        }
    }
    
    return;
}
void setup(void){
    ANSEL = 0b00000011;
    ANSELH = 0;
    TRISC = 0;
    TRISD = 0;
 
    
    PORTA = 0;
    PORTC = 0;
    PORTD = 0;
    
    
    OSCCONbits.IRCF = 0b110;
    OSCCONbits.SCS = 1;
    
   
    
    //Configuración de Interrupción
    
    
   //Configuración ADC
    ADCON1bits.ADFM = 0;
    ADCON1bits.VCFG0 = 0;
    ADCON1bits.VCFG1 = 0;
    
    ADCON0bits.ADCS = 0b10;
    ADCON0bits.CHS = 0;
    ADCON0bits.ADON = 1;
    __delay_us(50);
    
    PIR1bits.ADIF = 0;
    PIE1bits.ADIE = 1;
    INTCONbits.PEIE = 1;
    INTCONbits.GIE = 1;
    
    ADIF = 0;
}
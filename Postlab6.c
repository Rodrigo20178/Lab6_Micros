/*
 * File:   Postlab6.c
 * Author: Rodrigo García
 *
 * Created on 29 de marzo de 2023, 10:47 AM
 */
// CONFIG1
#pragma config FOSC = INTRC_CLKOUT// Oscillator Selection bits (INTOSC oscillator: CLKOUT function on RA6/OSC2/CLKOUT pin, I/O function on RA7/OSC1/CLKIN)
#pragma config WDTE = OFF       // Watchdog Timer Enable bit (WDT disabled and can be enabled by SWDTEN bit of the WDTCON register)
#pragma config PWRTE = ON      // Power-up Timer Enable bit (PWRT disabled)
#pragma config MCLRE = OFF      // RE3/MCLR pin function select bit (RE3/MCLR pin function is digital input, MCLR internally tied to VDD)
#pragma config CP = OFF         // Code Protection bit (Program memory code protection is disabled)
#pragma config CPD = OFF        // Data Code Protection bit (Data memory code protection is disabled)
#pragma config BOREN = OFF      // Brown Out Reset Selection bits (BOR disabled)
#pragma config IESO = OFF       // Internal External Switchover bit (Internal/External Switchover mode is disabled)
#pragma config FCMEN = OFF      // Fail-Safe Clock Monitor Enabled bit (Fail-Safe Clock Monitor is disabled)
#pragma config LVP = OFF        // Low Voltage Programming Enable bit (RB3 pin has digital I/O, HV on MCLR must be used for programming)

// CONFIG2
#pragma config BOR4V = BOR40V   // Brown-out Reset Selection bit (Brown-out Reset set to 4.0V)
#pragma config WRT = OFF        // Flash Program Memory Self Write Enable bits (Write protection off)

// #pragma config statements should precede project file includes.
// Use project enums instead of #define for ON and OFF.

#include <xc.h>
#include <stdint.h>
#define  _XTAL_FREQ  4000000
//Variables
int  Disp_valor;
int  Voltaje;

uint8_t Entero;
uint8_t Decimal_1;
uint8_t Decimal_2;
uint8_t Residuo;

uint8_t Flag = 0;
uint8_t Display [3];
//Tabla para decimales
uint8_t Tabla_1 [16] = {0b00111111,       // 0
                        0b00000110,       // 1
                        0b01011011,       // 2
                        0b01001111,       // 3
                        0b01100110,       // 4
                        0b01101101,       // 5
                        0b01111101,       // 6
                        0b00000111,       // 7
                        0b01111111,       // 8
                        0b01101111};      // 9
//Tabla para entero
uint8_t Tabla_2 [16] = {0b10111111,       // 0
                        0b10000110,       // 1
                        0b11011011,       // 2
                        0b11001111,       // 3
                        0b11100110,       // 4
                        0b11101101,       // 5
                        0b11111101,       // 6
                        0b10000111,       // 7
                        0b11111111,       // 8
                        0b11101111};      // 9
          

void setup(void);
void displays(int num);

//Configuración de interrupciones
void __interrupt() isr (void){
    if(PIR1bits.ADIF){
        if (ADCON0bits.CHS == 0)
            Disp_valor = ADRESH;
        else
            PORTC = ADRESH;
        PIR1bits.ADIF = 0;
    }
    if(INTCONbits.T0IF){
        PORTB = 0;
        if (Flag == 0){
            PORTD = Display [2];
            PORTB = 1;
            Flag = 1;
        }
        else if (Flag == 1){
            PORTD = Display [1];
            PORTB = 2;
            Flag = 2;
        }
        else if (Flag == 2){
            PORTD = Display [0];
            PORTB = 4;
            Flag = 0;
        }
        TMR0 = 10;
        INTCONbits.T0IF = 0;
    }
    return;
}
// MAIN
void main(void) {
    setup();
    ADCON0bits.GO = 1;
    while(1){
        Voltaje = (int)(Disp_valor*((float)5/255)*(100));
        displays(Voltaje);
        
        Display [0]= Tabla_1[Decimal_2];
        Display [1]= Tabla_1[Decimal_1];
        Display [2]= Tabla_2[Entero];
        
        if(ADCON0bits.GO == 0){
            if(ADCON0bits.CHS == 1)
                ADCON0bits.CHS = 0;
            else
                ADCON0bits.CHS = 1;
            __delay_us(1000);
            ADCON0bits.GO = 1;
        }
    }
    return;
}
//Configuración de entradas y salidas
void setup(void){
    ANSEL = 0b00000011;
    ANSELH = 0;
    TRISB = 0;
    TRISC = 0;
    TRISD = 0;
 
    
    PORTA = 0;
    PORTB = 0;
    PORTC = 0;
    PORTD = 0;
    
    
    OSCCONbits.IRCF = 0b110;
    OSCCONbits.SCS = 1;
    
   
    
    //Configuración de oscilador
    OSCCONbits.IRCF = 0b0110; //oscilador a 4MHz
    OSCCONbits.SCS = 1;
  
    
   //Configuración ADC
    ADCON1bits.ADFM = 0;
    ADCON1bits.VCFG0 = 0;
    ADCON1bits.VCFG1 = 0;
    
    ADCON0bits.ADCS = 0b10; 
    ADCON0bits.CHS = 0;
    ADCON0bits.ADON = 1;
    __delay_us(50);
    
    //Valor del prescaler
    OPTION_REGbits.T0CS = 0;
    OPTION_REGbits.PSA = 0;
    OPTION_REGbits.PS = 0b001; //Prescaler 1:4
    
    TMR0 = 10;
    //Configuración de Interrupción
    INTCONbits.PEIE = 1;
    INTCONbits.GIE = 1;
    
    PIE1bits.ADIE = 1;
    INTCONbits.TMR0IE = 1;
    
    PIR1bits.ADIF = 0;
    INTCONbits.T0IF = 0;
    
    ADIF = 0;
}
//Configuración para valor de los displays
void displays (int num){
    Entero = (uint8_t)(num/100);
    Residuo = num%100;
    
    Decimal_1 = Residuo/10;
    Residuo = Residuo%10;
    
    Decimal_2 = Residuo;
    
    return;
}
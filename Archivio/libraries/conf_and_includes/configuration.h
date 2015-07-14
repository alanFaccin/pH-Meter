/*
 * File:   configuration.h
 * Author: davidebortolami
 *
 * Created on 17 febbraio 2015, 16.45
 */


/*
 * FIXED PIN ARE PRESENT ON PORTC
 *      RC0
 *      RC1 - pwm 2
 *      RC2 - pwm 1
 *      RC3 - i2c SCL
 *      RC4 - i2c SDA
 *      RC5
 *      RC6 - uart TX
 *      RC7 - uart RX
 */



#ifndef CONFIGURATION_H
#define	CONFIGURATION_H

    //Boot screen at power on
    #define BOOT_SCREEN true
    #define FORCE_MAIN_MODE false
    #define BATTERY_VOLTAGE     1.45

    //default value of pwm, reduce to save battery
    #define Backlight_default_pwm 700
    #define Backlight_timer 100 //time after shutdown, in work cycles of the main routine

    #define probe_voltage_overload 0.500 //+- maximun probe voltage in volt float



    #ifdef Board_EasyPicV7

        #pragma config FOSC = HS // Oscillator Selection bits (HS oscillator)
        #define _XTAL_FREQ 8000000


        #ifndef __LCD_SEVERI_PINOUT__
        #define __LCD_SEVERI_PINOUT__
            #define RS RB4
            #define EN RB5
            #define D4 RB0
            #define D5 RB1
            #define D6 RB2
            #define D7 RB3
        #endif

        //buttons definition
        #define Button_1 PORTCbits.RC0
        #define Button_2 PORTCbits.RC1
        #define Button_3 PORTCbits.RC5
        #define Button_4 PORTCbits.RC6

    #endif


    #ifdef Board_SeveriDemoLCD

        #pragma config FOSC = XT // Oscillator Selection bits (HS oscillator)
        #define _XTAL_FREQ 4000000

        #ifndef __LCD_SEVERI_PINOUT__
        #define __LCD_SEVERI_PINOUT__
            #define RS RB5
            #define EN RB4
            #define D4 RB0
            #define D5 RB1
            #define D6 RB2
            #define D7 RB3
        #endif

    #endif

    #ifdef Board_PCBFinal

        #pragma config FOSC = HS // Oscillator Selection bits (HS oscillator)
        #define _XTAL_FREQ 16000000

        #ifndef __LCD_SEVERI_PINOUT__
        #define __LCD_SEVERI_PINOUT__
            #define RS RB0
            #define EN RB5
            #define D4 RB4
            #define D5 RB3
            #define D6 RB2
            #define D7 RB1
        #endif

        //buttons definition
        //put a ! befor bit if active low
        #define Button_1 !PORTAbits.RA3
        #define Button_2 !PORTAbits.RA2
        #define Button_3 !PORTAbits.RA1
        #define Button_4 !PORTAbits.RA0

    #endif


    //Basic pic configuration
    //#pragma config FOSC = HS // Oscillator Selection bits (HS oscillator)
    #pragma config WDTE = OFF // Watchdog Timer Enable bit (WDT enabled)
    #pragma config PWRTE = OFF // Power-up Timer Enable bit (PWRT disabled)
    #pragma config BOREN = OFF // Brown-out Reset Enable bit (BOR enabled)
    #pragma config LVP = OFF // Low-Voltage (Single-Supply) In-Circuit Serial Programming Enable bit (RB3 is digital I/O, HV on MCLR must be used for programming)
    #pragma config CPD = OFF // Data EEPROM Memory Code Protection bit (Data EEPROM code protection off)
    #pragma config WRT = OFF // Flash Program Memory Write Enable bits (Write protection off; all program memory may be written to by EECON control)
    #pragma config CP = OFF // Flash Program Memory Code Protection bit (Code protection off)


#endif	/* CONFIGURATION_H */



/*
PH METER BY DAVIDE BORTOLAMI
ALL RIGHT RESERVED (C) 2015 Davide Bortolami
 *
 * Made for pic16f876A
 * I.T.I. "F. Severi" Padua
 *
 */

//define Board_EasyPicV7
//#define Board_SeveriDemoLCD
#define Board_PCBFinal


//pic configuration - must be the first include
#include "libraries/conf_and_includes/configuration.h"
//general c includes
#include "libraries/conf_and_includes/includes.h"


//board specific library and main include files
#include "libraries/board-specific/easypic-v7-pic16.h"
//lcd library
#include "libraries/lcd/LCD.h"
//physical constant library
#include "libraries/physics/constants.h"
//ads-1015 library with mask for config bytes
#include "libraries/ads1015/ads1015-config.h"
//pwm library
#include "libraries/pwm/pwm.h"
//hardware i2c library
#include "libraries/i2c/i2cb.h"
//routine for ads1015 acquisition
#include "libraries/ads1015/ads1015_routines.h"
//temperature conversion library
#include "libraries/physics/temperature_conv.h"

//change backlight everytime the subroutine is called
void Backlight_Setup(char reset)
{

    static unsigned int backlight;

    for(char i=0; i<250; i++)
    {
        backlight ++;
        if(backlight < 1023)
            PWM1_Duty(backlight);
    }

    if (backlight >= 1022)
        backlight = 0;

    
    if(reset)
        backlight = 0;

    PWM1_Duty(backlight);
    __delay_ms(100);

}

//setup subroutine, to be called once
void setup(void) {

    //tris configuration
    TRISA = 0b11111111;
    TRISB = 0b11000000;
    TRISC = 0b11100001;

    //port initialization
    PORTA = 0x00;
    PORTB = 0x00;
    PORTC = 0x00;

    ADCON1 = 0b00000110;
    
    //libraries initialization
    LCD_Init();
    PWM_Init(5000);
    I2CInit();

    //reset lcd to initial position and clear
    LCD_Clear();
    LCD_Set_Cursor(1,1);

    //create a custom delta (uppercase greek letter) simbol
    //const declaration allow RAM to be preserved
    const char delta_array[8] = {0x4,0xe,0xa,0x1b,0x11,0x11,0x1f,0x0};  
    LCD_Custom_Char(delta_array, 0x00);

    PWM1_Start();
    PWM1_Duty(0);
    //boot screen - can be activated/deactivated in configuration.h
    if(BOOT_SCREEN)
    {
        //boot text
        LCD_Set_Cursor(1, 1);
        LCD_Write_String("    Ph-Meter   ");
        LCD_Set_Cursor(2, 1);
        LCD_Write_String("By ddavidebor.it");

        //smooth backlight power-up
        for(int  i=0; i!=1022; i+=2)
        {
            __delay_ms(1);
            PWM1_Duty(i);
        }


        //two backlight flash
        PWM1_Stop();
        PORTCbits.RC2 = 1;
        __delay_ms(120);
        PORTCbits.RC2 = 0;
        __delay_ms(120);
        PORTCbits.RC2 = 1;
        __delay_ms(120);
        PORTCbits.RC2 = 0;
        __delay_ms(120);
        PORTCbits.RC2 = 1;
        
        __delay_ms(700);
        LCD_Clear();

        LCD_Set_Cursor(1,1);
        }

    Backlight_Setup(true);

    
    PWM1_Start();
    PWM1_Duty(Backlight_default_pwm);

    

}

//Calculate ph based on voltage (V) and temperature (°K)
float PhCalc(float Voltage, float Temperature) {

    //Voltage imput must be absolute, with zero on 0, positive or negative
    float Ph = 0;
    Ph = 7 + (Voltage * CONST_FARADAY) / (CONST_MOLAR_GAS * Temperature * CONST_TENTH_LINEAR_LOG);
    return Ph;

}

//Return temperature in celsius degrees
float GetTemperature_celsius(void)
{
    return ( 100 * VoltageReadSingleEnded(2, RANGE_1_024V));
}

//Get battery voltage in percentage
unsigned char GetBattery_percent()
{
    //return (Rbottom/Rtop)*100*Vout/VBattMax
    return (11*100*VoltageReadSingleEnded(3, RANGE_1_024V))/BATTERY_VOLTAGE;
}

//Print a float on the lcd in the form of 12.34
//This part of code should be deprecated after testing is concluded
void print_float(float value){

    unsigned int integer_part;
    integer_part = trunc(value);
  
    unsigned int floating_part;
    floating_part = ((value - integer_part)*100);
    if (value > 0)
        LCD_Write_Char(' ');
    else
    {
        integer_part = -integer_part;
        floating_part = -floating_part;
        LCD_Write_Char('-');
    }
    LCD_printf("%2d.%02d", integer_part, floating_part);
}

//Check if the probe voltage is within reasonable parameters
void error_diagnostics_ph_probe_overload(float voltage)
{
    static float minimun = -probe_voltage_overload;
    static float maximun =  probe_voltage_overload;

    //if voltage is ok return
    if ((voltage < maximun) && (voltage > minimun))
        return;

    LCD_Clear();

    LCD_Set_Cursor(1, 1);
    LCD_printf("    Ph probe    ");

    LCD_Set_Cursor(2, 1);
    LCD_printf("    overload   ");

    for(char i=0; i!=5; i++)
    {
        __delay_ms(350);
        LCD_Power(false);
        __delay_ms(350);
        LCD_Power(true);
    }

    LCD_Power(true);
    LCD_Clear();
    
}

//Check if the temperature read is within parameters
void error_diagnostics_temperature_overload(float temperature)
{
    static float maximun = 100.0;
    //if temperature is ok return
    if (temperature < maximun)
        return;

    LCD_Clear();

    LCD_Set_Cursor(1, 1);
    LCD_printf("   Temp probe   ");

    LCD_Set_Cursor(2, 1);
    LCD_printf("    overload   ");

    for(char i=0; i!=5; i++)
    {
        __delay_ms(350);
        LCD_Power(false);
        __delay_ms(350);
        LCD_Power(true);
    }

    LCD_Power(true);
    LCD_Clear();


}


//Print the main mode, featuring ph, temperature and battery
void print_main(float ph_print, float temperature_print, char battery_print) {

        //LCD_Clear();
        int integer_part;
        int floating_part;
        char tempchar;

        //printf first row
        LCD_Set_Cursor(1, 1);
        integer_part = trunc(ph_print); //trunc() truncate the number and return only the integer part without aproximation
        floating_part = ((ph_print - integer_part)*100);

        LCD_printf("Ph %2d.%02d || BATT", integer_part, floating_part);
        

        //print second row
        LCD_Set_Cursor(2, 1);
        integer_part = trunc(temperature_print);
        floating_part = ((temperature_print - integer_part)*10);

        LCD_printf("%cC %2d.%01d  || %3d%%", 0xDF, integer_part, floating_part, battery_print);

}

//Print delta mode, featuring delta, initial ph, temperature and battery
void print_delta(float ph_zero, float ph_delta, float temperature_print, char battery_print )
{

        int integer_part;
        int floating_part;

        //printf first row
        LCD_Set_Cursor(1, 1);
        integer_part = trunc(ph_delta); //trunc() truncate the number and return only the integer part without aproximation
        floating_part = ((ph_delta - integer_part)*100);
        char tempchar;
        if (ph_delta > 0)
            tempchar = ' ';
        else
        {
            integer_part = -integer_part;
            floating_part = -floating_part;
            tempchar='-';
        }
        LCD_Write_Char(0x00); //write delta
        LCD_printf("Ph %c%2d.%02d |BATT", tempchar, integer_part, floating_part);

        //print second row
        LCD_Set_Cursor(2, 1);
        integer_part = trunc(ph_zero);
        floating_part = ((ph_zero - integer_part)*100);
        LCD_printf("Ph0  %2d.%02d |%3d%%",  integer_part, floating_part, battery_print);

}


//One univoque char for every mode
#define MODE_MAIN 0x01 //main mode
#define MODE_DELTA 0x02 //delta mode
#define MODE_DEBUG_VOLTAGE 0x03 //voltage from adc, only for debug


void MODE_Main( long unsigned int cycle_counter)
{
     float voltage;
     float temperature;
     float ph;
     char battery;

     static float ph_array[6]; //array used for mean
     const char measurement_cycles = 5;  //number of measurement per LCD writes

     //clear lcd upon mode activation
     if (cycle_counter == 0)
         LCD_Clear();

     temperature = GetTemperature_celsius();
     voltage = GetProbe_voltage();
     battery = GetBattery_percent();

     //check for errors and overloads
     error_diagnostics_ph_probe_overload(voltage);
     error_diagnostics_temperature_overload(temperature);


     //calculate ph
     ph = PhCalc(voltage, CelsiusToKelvin(temperature));

     //print every tot cycles
     ph_array[cycle_counter % measurement_cycles] = ph;
     if((cycle_counter % measurement_cycles )==0)
     {
         ph = 0;
         for(char i=0; i!=measurement_cycles; i++)
             ph+=ph_array[i];
         ph=ph/measurement_cycles;
         print_main(ph, temperature, battery);
     }

        
}

//Mode for delta measurement
void MODE_Delta(long unsigned int cycle_counter)
{
     static float ph_zero;
     float voltage;
     float temperature;
     char battery;
     float ph_delta;

     static float ph_array[6]; //array used for mean
     const char measurement_cycles = 5;  //number of measurement per LCD writes


     temperature = GetTemperature_celsius();
     voltage = GetProbe_voltage();
     battery = GetBattery_percent();

     //check for errors and overloads
     error_diagnostics_ph_probe_overload(voltage);
     error_diagnostics_temperature_overload(temperature);

     //if button has just been pressed
     if(cycle_counter==0)
     {
         LCD_Clear();
         //acquire ph-zero
         ph_zero = PhCalc(voltage, CelsiusToKelvin(temperature));
     }

     ph_delta = PhCalc(voltage, CelsiusToKelvin(temperature)) - ph_zero;


     ph_array[cycle_counter % measurement_cycles] = ph_delta;
     if((cycle_counter % measurement_cycles )==0)
     {
         ph_delta = 0;
         for(char i=0; i!=measurement_cycles; i++)
             ph_delta+=ph_array[i];
         ph_delta=ph_delta/measurement_cycles;
     print_delta(ph_zero,ph_delta,CelsiusToKelvin(temperature),battery );

     }


}

//Show some useful voltage read from the ph probe sensor
void MODE_Debug_Voltage(long unsigned int cycle_counter)
{
     if(cycle_counter==0)
     LCD_Clear();

     //ra2 as voltage, divided by five
     //assuming temperature = 25°c

     float voltage[4];
     voltage[0] = VoltageReadSingleEnded(0, RANGE_4_096V);
     voltage[1] = VoltageReadSingleEnded(1, RANGE_4_096V);
     voltage[2] = VoltageReadSingleEnded(2, RANGE_4_096V);
     voltage[3] = VoltageReadSingleEnded(3, RANGE_4_096V);

     LCD_Set_Cursor(1,1);
     LCD_Write_Char('S');
     LCD_Write_Char(' ');
     print_float(voltage[0]);
     LCD_Write_Char('R');
     LCD_Write_Char(' ');
     print_float(voltage[1]);
     LCD_Set_Cursor(2,1);
     LCD_printf("Diff:    ");
     print_float(GetProbe_voltage());
     LCD_Write_Char('V');

}


//MAIN ROUTINE
int main(int argc, char** argv) {

     //run once setup subroutine
     setup();
    

     unsigned char work_mode = MODE_MAIN;
     long unsigned int cycle_counter = 0;

     //main while loop
     while(1)
     {
        
         cycle_counter++; //update number of cycles

         char update_backlight;
         update_backlight = 0;

         //if buttons are present and activated via configuration.h
         if(!FORCE_MAIN_MODE)
         {
             //BUTTON SCAN
             if (Button_3) //go to delta mode, if pushed again reset delta mode
             {
                 work_mode = MODE_DELTA;
                 cycle_counter = 0;
             }
             if ((Button_2) && (work_mode != MODE_DEBUG_VOLTAGE) ) //if button is pressed and we are in another mode
             {
                 work_mode = MODE_DEBUG_VOLTAGE;
                 cycle_counter = 0;
             }
             if((Button_4) && (work_mode != MODE_MAIN)) //if button is pressed and we are in another mode
             {
                 work_mode = MODE_MAIN;
                 cycle_counter = 0;
             }
             if(Button_1)
             {
                 update_backlight = true;
             }
         }


         //wait until buttons are released
         while(Button_1 || Button_2 || Button_3 || Button_4)
             __delay_ms(50);


         if(update_backlight)
             Backlight_Setup(false);

         //main switch cycle
         switch(work_mode)
         {

             //Print a simulated voltage based on adc RA2 and a simulated kelvin temperature of
             case MODE_DEBUG_VOLTAGE:
                 MODE_Debug_Voltage(cycle_counter);
                 break;

             case MODE_DELTA:
                MODE_Delta(cycle_counter);
                break;

             case MODE_MAIN:
                 MODE_Main(cycle_counter);
                 break;

             default:
             {
                work_mode = MODE_MAIN;
                cycle_counter = 0;
             }break;
         }
     }

    return (EXIT_SUCCESS);
}


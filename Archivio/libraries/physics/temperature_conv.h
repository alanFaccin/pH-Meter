//Temperature conversion library
//Made by Davide Bortolami

//convert from celsius degrees to kelvin degrees
float CelsiusToKelvin(float Celsius);

//convert from kelvin degrees to celsius degrees
float KelvinToCelsius(float Kelvin);

//convert from Kelvin degrees to Farenheit degrees
float KelvinToFarenheit(float Kelvin);

//convert from Farenheit degrees to Kelvin degrees
float FarenheitToKelvin(float Farenheit);


#ifndef TEMPERATURE_CONV_H
#define	TEMPERATURE_CONV_H

//convert from celsius degrees to kelvin degrees
float CelsiusToKelvin(float Celsius) {
    return Celsius + 273.15 ;
}

//convert from kelvin degrees to celsius degrees
float KelvinToCelsius(float Kelvin) {
    return Kelvin - 273.15;
}

//convert from Kelvin degrees to Farenheit degrees
float KelvinToFarenheit(float Kelvin){

    return ((Kelvin - 273.15)*1.8 + 32.0);
}

//convert from Farenheit degrees to Kelvin degrees
float FarenheitToKelvin(float Farenheit){
    return ((Farenheit + 459.67) * 0.55555);
}


#endif	/* TEMPERATURE_CONV_H */


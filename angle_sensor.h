/***
 * Capibarib E-racing
 * Federal University of Pernambuco (UFPE)
 * Group Area: Powertrain
 
 * This file is part of the car's VCU (VEHICLE CONTROL UNIT)
 * it contains the APPS plausibilty check, BSE plausibility check
 * as well the methods to read and map all the angle related sensors
 ***/

#ifndef _ANGLE_SENSOR_H_
#define _ANGLE_SENSOR_H_

#include "mbed.h"
#include <cstdint>
#include <time.h>

/*================================== SENSORS PARAMETERS ==================================*/
//General Parameters 
#define VREF_ADC        3.3     // ADC Reference (in volts), it scales the 16bit in that range
#define INPUT_MIN       4500    // Minimum 16bit Input the sensor should read
#define INPUT_MAX       48000   // Maximum 16bit Input the sensor should read


//Steering Wheel Parameters
#define Vol_min         0.425   //Steering Wheel Sensors Minimum input voltage
#define Vol_max         2.825   //Steering Wheel Sensors Sensors Maximum input voltage
#define Vol_ang_min     -120    //Minimum value for the Steering Wheel angle (Degrees)
#define Vol_ang_max     120     //Maximum value for the Steering Wheel angle (Degrees)

//BSE (Break System Enconder) Parameters
#define BSE_min         0.3     //BSE Sensors Minimum input voltage
#define BSE_max         3.0     //BSE Sensors Minimum input voltage
#define BSE_ang_min     0.0       //Minimum value for the break Pedal angle (Degrees)
#define BSE_ang_max     120.0     //Maximum value for the break Pedal angle (Degrees)

//APPS (Accelerator Pedal Position Sensor) Parameters
#define APPS1_MIN       0.425   //APPS1 Minimum input voltage
#define APPS1_MAX       3.021   //APPS1 Maximum input voltage
#define APPS2_MIN       0.4     //APPS2 Minimum input voltage
#define APPS2_MAX       2.9     //APPS2 Maximum input voltage
#define APPS_ang_min    0       //Minimum value for the Accelerator Pedal angle (Degrees)
#define APPS_ang_max    120     //Maximum value for the Accelerator Pedal angle (Degrees)

#define APPS_MIN_READ           10          // min 16 bit[0 - 65353] value of  
#define APPS_MAX_READ           4500        // Max Velocity of the motor [RPM] 

#define STEERING_MIN_READ       10          // min 16 bit[0 - 65353] value of  
#define STEERING_MAX_READ       500        // Max Velocity of the motor [RPM] 


//Car's Physical parameters (For speed control)
#define RADIUS_WHEEL    0.27    //Wheel's radius in Meters
#define N_DISC_WHOLES   3       //Number of wholes in the break's disc


//Ultility Functions
double millis();
double Differential();
float RPM_to_W(uint16_t RPM_inv);
void Calibrate_ADC();

struct APPS_struct{
    float s1; //sensor 1 angle value
    float s2; //sensor 2 angle value
};

struct Wref_struct{
    uint16_t W1; //Win
    uint16_t W2; //Wout
};


/*================================== CLASSES ==================================*/
//class for Acelleration Pedal, break Pedal, and Steering wheel
class angle_sensor{

    //Atributes
    private:
    float Angle;        //Angle's value in Degree
    float Volt_min;     //Sensor's Minimum voltage measurement
    float Volt_max;     //Sensor's Maximum voltage measurement
    float Angle_min;    //Sensor's Minimum angle
    float Angle_max;    //Sensor's Maximum angle
    AnalogIn ADC_Pin;   //Input Pin in the MicroController

    //Methods
    public:
    float read_angle();
    uint16_t read_scaled_u16();
    //Maps the Input voltage (16bit) into the Angle
    float map_ADC(long Variable, float in_min, float in_max, float out_min, float out_max);
    //testes if ADC value (16bit) is within bounds of sensor
    bool Input_Error_Check();
    //prints raw voltage
    void Voltage_print();

    //Constructors:
    angle_sensor(PinName adc_Pin, float _volt_min,float _volt_max, float _angle_min,float _angle_max);

};

//Sensors
class BSE_Sensor: public angle_sensor{
    //bool BSE_Error_check();
    
    //Constructors:
    public:
    BSE_Sensor(PinName adc_Pin);
};

class APP_Sensors{
    //Tests if theres more than 10% discrepancy between APPS1 and APPS2
    //If there is a discrapancy, and it maintains for more then 10ms, Error=1 and shuts down the motor
    private:
    AnalogOut APPS_out;

    public:
    angle_sensor APPS1;
    angle_sensor APPS2;

    private:
    float APPS1_Angle{0};           //angle value [degrees]
    float APPS2_Angle{0};           //angle value [degrees]
    //Error related Attributes
    bool AppsError_flag{0};         //just a flag, within the 100ms rule
    double Error_Start_Time{0};     //Time stamp when error started
    bool ERROR_State{0};            //if there is actual error, 1= ERROR, 0= all good :) 

    public:
    bool APPS_Error_check(); //1= ERROR, 0= all good :) 
    APPS_struct read_APPS();
    float read_S1();
    float read_S2();

    public:
    APP_Sensors(PinName _apps1_pin, PinName _apps2_pin, PinName _apps_out_pin);

};

class Steering_Wheel_Sensor: public angle_sensor{

    public:
    Steering_Wheel_Sensor(PinName adc_Pin);
};


/*======================================== Constructors ========================================*/
//Angle Sensor
inline angle_sensor::angle_sensor(PinName adc_Pin, float _volt_min,float _volt_max, float _angle_min,float _angle_max)
:ADC_Pin{adc_Pin,VREF_ADC}, Volt_min{_volt_min},Volt_max{_volt_max}, Angle_min{_angle_min}, Angle_max{_angle_max}{
    //ADC_Pin.set_reference_voltage(3.31);
};

//APP Sensors
inline APP_Sensors::APP_Sensors(PinName _apps1_pin, PinName _apps2_pin, PinName _apps_out_pin)
    :APPS1(_apps1_pin,APPS1_MIN,APPS1_MAX,APPS_ang_min,APPS_ang_max), //APPS 1
     APPS2(_apps2_pin,APPS2_MIN,APPS2_MAX,APPS_ang_min,APPS_ang_max), // APPS 2
     APPS_out{_apps_out_pin}{} //APPS out


//BSE Sensor
inline BSE_Sensor::BSE_Sensor(PinName adc_Pin)
    :angle_sensor{adc_Pin, BSE_min, BSE_max, BSE_ang_min, BSE_ang_max}{}

//Steering Wheel Sensor
inline Steering_Wheel_Sensor::Steering_Wheel_Sensor(PinName adc_Pin)
    :angle_sensor{adc_Pin, Vol_min, Vol_max, Vol_ang_min, Vol_ang_max}{}




/*======================================== Methods ========================================*/
//Maps the ADC 16bit voltage read into the angle's range 
inline float angle_sensor::map_ADC (long Variable, float in_min, float in_max, float out_min, float out_max) {
    //Maps the sensors input voltage from [0 - 3.3] to [0 - 16bit]
    in_min= (in_min/3.3)*65535;
    in_max= (in_max/3.3)*65535;

    float Mapped_Variable = (Variable - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
    return Mapped_Variable;
}


//Reads the ADC pin and returns the angle value in degrees 
inline float angle_sensor:: read_angle(){
    Angle=map_ADC( ADC_Pin.read_u16(), Volt_min, Volt_max, Angle_min, Angle_max);
    return Angle;    
}

inline uint16_t angle_sensor:: read_scaled_u16(){
    uint16_t u1=map_ADC( ADC_Pin.read_u16(), Volt_min, Volt_max, 0, 65535);
    return Angle;    
}

inline void angle_sensor:: Voltage_print(){

    uint16_t Voltage_16bit=ADC_Pin.read_u16();
    ADC_Pin.set_reference_voltage(3.3);
    printf("\n[VCU] ADC: Voltage_Read[16bit]: %d , Voltage[V]: %.2f V  \n",Voltage_16bit, ADC_Pin.read_voltage() );    
}



/*======================================== APPS ========================================*/
inline float APP_Sensors:: read_S1(){
    return APPS1.read_angle();    
}

inline float APP_Sensors:: read_S2(){
    return APPS2.read_angle();    
}

inline APPS_struct APP_Sensors:: read_APPS(){
    APPS_struct Apps_values;
    
    float Angle_S1=APPS1.read_angle();
    float Angle_S2=APPS2.read_angle();
    uint16_t max_Apps = uint16_t( max(Angle_S1,Angle_S2) );

    //? REALLY not sure about that one:
    if (APPS_Error_check() == true){
        APPS_out.write_u16(0);
    } 
    else {
        APPS_out.write_u16(max_Apps);
    }

    Apps_values.s1=Angle_S1;
    Apps_values.s2=Angle_S2;
    
    return Apps_values;
}


// Checks if there's a discrepancy bigger than 10%, for longer than 100 ms
inline bool APP_Sensors::APPS_Error_check(){
    APPS1_Angle=APPS1.read_angle();
    APPS2_Angle=APPS2.read_angle();

    if ( abs(APPS1_Angle - APPS2_Angle) > (0.1 * max(APPS1_Angle, APPS2_Angle)) ){
        if(AppsError_flag == 0) { //Starts Counting
            Error_Start_Time = millis(); 
            AppsError_flag = 1;
        }

        if(millis() - Error_Start_Time > 100) { //if the error continues after 100ms
            ERROR_State= 1;
        } else{
            ERROR_State= 0;
        }

    } else { //if error ceased
        AppsError_flag = 0;
        ERROR_State= 0;
        }

    return ERROR_State;
}

inline bool angle_sensor::Input_Error_Check(){
    float Angle_Value=read_angle();
    float prevTime{0};
    bool isError{0};

    if(Angle_Value < INPUT_MIN or Angle_Value > INPUT_MAX){
        if(millis() - prevTime >= 100){
            prevTime =  millis();
            isError=1;
        } 
    }
    else{
        prevTime = millis();
        isError=0;
    }

    return isError;
}

/*======================================== Auxiliar functions ========================================*/
//time passed (in ms) since the program first started
inline double millis(){
    using namespace std::chrono;
    auto now_us = time_point_cast<microseconds>(Kernel::Clock::now()); //time of referece= program's begin
    long micros = now_us.time_since_epoch().count(); //time (in us) since the reference 
    return micros / 1000.0; //turns time passed from us to ms
}

inline float map (long Variable, float in_min, float in_max, float out_min, float out_max) {
    float Mapped_Variable = (Variable - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
    return Mapped_Variable;
}


inline void Calibrate_ADC(){

}



#endif
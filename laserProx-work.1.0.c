/* This example shows how to get single-shot range
 measurements from the VL53L0X. The sensor can optionally be
 configured with different ranging profiles, as described in
 the VL53L0X API user manual, to get better performance for
 a certain application. This code is based on the four
 "SingleRanging" examples in the VL53L0X API.

 The range readings are in units of mm. */
#include "VL53L0X/pic16f886.h"
//#include <Wire.h>
#include "VL53L0X/VL53L0X.h"
#include <math.h>

#define millis()  (msTimer)
int32 msTimer=0;


#INT_TIMER1
void timer1_isr() {
   set_timer1(65286); // keep period at 1 ms (at 8 MHz)
   msTimer++;
}


void pic_setup() {
   setup_timer_1(T1_INTERNAL | T1_DIV_BY_8);
   set_timer1(65286); // keep period at 1 ms (at 8 MHz)
   enable_interrupts(INT_TIMER1);
   enable_interrupts(GLOBAL);

}



// VL53L0X sensor;


// Uncomment this line to use long range mode. This
// increases the sensitivity of the sensor and extends its
// potential range, but increases the likelihood of getting
// an inaccurate reading because of reflections from objects
// other than the intended target. It works best in dark
// conditions.

#define LONG_RANGE


// Uncomment ONE of these two lines to get
// - higher speed at the cost of lower accuracy OR
// - higher accuracy at the cost of lower speed

//#define HIGH_SPEED
#define HIGH_ACCURACY


void setup()
{

   printf("Starting\r\n");
   
   pic_setup();

  init();
  setTimeout(200);

#if defined LONG_RANGE
  // lower the return signal rate limit (default is 0.25 MCPS)
  setSignalRateLimit(0.1);
  // increase laser pulse periods (defaults are 14 and 10 PCLKs)
  setVcselPulsePeriod(VcselPeriodPreRange, 18);
  setVcselPulsePeriod(VcselPeriodFinalRange, 14);
#endif

#if defined HIGH_SPEED
  // reduce timing budget to 20 ms (default is about 33 ms)
  setMeasurementTimingBudget(20000);
#elif defined HIGH_ACCURACY
  // increase timing budget to 200 ms
  setMeasurementTimingBudget(200000);
#endif
}

void loop()
{
  printf("%lu\r\n", readRangeSingleMillimeters());
  if (timeoutOccurred()) { printf(" TIMEOUT\r\n"); }

}

////////////////////////////////////////////////////////////
//define LCD Display 
////////////////////////////////////////////////////////////
#define SLAVE_ADDRESS  0xB0
#define DISPLAY_ADDRESS 0xB4
#define REGISTER_SIZE  8   
#define DISPLAY_CMD_SEND_VALUE 2
#define DISPLAY_CMD_SEND_LONG_TEXT 5
#define DISPLAY_CMD_CLS 6
#define DISPLAY_CMD_SETPOS 8
////////////////////////////////////////////////////////////
//define LCD Display 
////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////
//LCD Display Funtion 
///////////////////////////////////////////////////////////////////////////
int16 rotaryR;
float degree,distance,tall,rad;

void displayLongText(char* text) {

   int i ;
   i2c_start();
   i2c_write(DISPLAY_ADDRESS);
   i2c_write(DISPLAY_CMD_SEND_LONG_TEXT);

   for(i=0;text[i]!='\0';i++)
   {
      i2c_write(text[i]);

   }
   i2c_write('\0');
   i2c_stop();

//delay_ms(100);

}




void clearDisplay(){

   i2c_start();
   i2c_write(DISPLAY_ADDRESS);
   i2c_write(DISPLAY_CMD_CLS);
   i2c_stop();
}

void setDisplayPos(int pos){
   i2c_start();
   i2c_write(DISPLAY_ADDRESS);
   i2c_write(DISPLAY_CMD_SETPOS);
   i2c_write(pos);
   i2c_write('\0');
   i2c_stop();
}
///////////////////////////////////////////////////////////////////////////
//LCD Display Funtion 
///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////
//READ DISTANCE ULTRASONIC
///////////////////////////////////////////////////////////////////////////


void main() {
   setup();
   
   
   
    setup_adc_ports(sAN0); // setup PIN A0 as analog input 
    setup_adc( ADC_CLOCK_INTERNAL ); 

    printf("Sampling:\r\n");
    set_adc_channel( 0 ); // set the ADC chaneel to read 
    delay_ms(50); // wait for the sensor reading to finish
     
    float toFloor ;
    
   while(1) {
       rotaryR = read_adc();
      degree = 90.0*(rotaryR-394) / 137;
      distance = readRangeSingleMillimeters() / 10.0;
      //if (timeoutOccurred()) { printf(" TIMEOUT\r\n"); }

      if(!input(PIN_B7)){
         toFloor = distance;
         
         clearDisplay();
         setDisplayPos(1);
         displayLongText((char*)"Calibrated!");
         delay_ms(100);
      }

      rad = degree * 0.0174533;
      tall = (tan(rad) * distance ) + toFloor;
     // tall = tall +  0;
      
      printf("Sensor= %Lu\tDegree= %.2f\tDist= %.2f cm  tall=%f cm\r\n", rotaryR,degree,distance,tall);
      delay_ms(100);
      clearDisplay();
      setDisplayPos(1);
      char* message;
      displayLongText((char*)"Deg:");
      
      if(degree >= 90) sprintf( message , "----" );
      else sprintf( message , "%.1lf" ,(float) degree);
      
       setDisplayPos(5);
      displayLongText(message);
      
      setDisplayPos(9);
      displayLongText((char*)"Dis:");
      sprintf( message , "%.1lf" ,(float)distance /100.0);
      displayLongText(message);

//!      if(!input(PIN_B7)){
//!         distanceToFloor = distanceFromUltrasonic();
//!      }
      setDisplayPos(17);
      displayLongText((char*)"tall:");
      sprintf( message , "%.1f" ,tall );
      displayLongText(message);


      
      
      
      delay_ms(300);
      
     
      
   }

}


#include "VL53L0X/VL53L0X.c"


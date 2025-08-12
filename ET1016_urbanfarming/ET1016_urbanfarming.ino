// Title: URBAN FARMING
// Name: ShaoMin, Daniel
// Admin no: P2515971, P2515843
// Class: DCEP/FT/1A/09
// Description: Control of water (watering the plants - the blue led light will on when the sprinkler is on, along with the motor sensor that will move in a systematic manner to water the plant after a set about of time using the display), ac (optimal temp for the plants - when the temperature detected exceeds a certain degree, the ac will auto switch on to allow the farm to cool down to the optimal temp for the plant, the red led will on when the temp is too high)

#include <Wire.h>
#include "PCA9685.h"
#include "RichShieldTM1637.h"
#include "RichShieldNTC.h"
#define LED_BLUE 6
#include "RichShieldPassiveBuzzer.h"

#define NTC_PIN A1 //SIG pin of NTC module connect to A1 of IO Shield, that is pin A1 of OPEN-SMART UNO R3
NTC temper(NTC_PIN);  

#define LED_RED 4
#define PassiveBuzzerPin 3
PassiveBuzzer buz(PassiveBuzzerPin);
#define NOTE_M6 880
#define CLK 10//CLK of the TM1637 IC connect to D10 of OPEN-SMART UNO R3
#define DIO 11//DIO of the TM1637 IC connect to D11 of OPEN-SMART UNO R3
TM1637 disp(CLK,DIO);

PCA9685 pwmController(Wire);
/*Create an object, pwmServo1 of class PCA9685_ServoEval to assist with calculating Servo PWM values from angle/speed done internally by the library files*/
PCA9685_ServoEval pwmServo1;

void setup()
{
  disp.init();//The initialization of the display
  delay(1000);//
	pinMode(LED_RED, OUTPUT);
  disp.init();//The initialization of the display

  pinMode(LED_BLUE, OUTPUT);

	pwmController.resetDevices();   // Resets all PCA9685 devices on i2c line
    /* Initializes module using default totem-pole driver mode, and default disabled phase balancer */
  pwmController.init();
    // Set standard servo frequency of 50Hz (or 20ms servo phase length)
  pwmController.setPWMFreqServo();
    /* Bring to starting position as defined by the programmer based on how the servos were mounted and the application */
    /* Channel 0 connected to upper Servo. Angle value Limited to -10 degree (minimum value in this application) due to its movement blocked by the lower bracket/servo (mechanical design) */
  pwmController.setChannelPWM(0, pwmServo1.pwmForAngle(-10));
    /*Channel 1 connected to lower servo. Angle value is set at 0 degree, at the middle of the normal range from -90 to +90 */
  pwmController.setChannelPWM(1, pwmServo1.pwmForAngle(0));
  delay(1000);

}

int i;
void loop()
{
  int temp, oldtemp = 0;
  int t;
  int i,k;
  
  for(t = 15; t >= 0; t--)  
  {
  digitalWrite(LED_BLUE, LOW); //when sprinkler is not on, the led is not on too
  disp.display(t); //diaplay time
  delay(500);
  temp = temper.getTemperature();//get temperature
  if(temp != oldtemp)
  {
    displayTemperature(temp);//display temp, measure the temp
    oldtemp = temp;
  }
  
  delay(500);
  }
  
  digitalWrite(LED_BLUE, HIGH); //when sprinkler is on, the led is on
  for(k=0;k<3;k++) //Sprinkler spin twice to water the plants
  {
   for (i = 0; i <= 90; i += 5) { //Slow Turn anti-clockwise (from 0 to 90 degree
        pwmController.setChannelPWM(1, pwmServo1.pwmForAngle(i));
        delay(250);  //longer delay for Slow turn movement
    }

  for (i = 90; i >= 0; i -= 5) { //Fast Turn clockwise (from 90 to 0 degree)
        pwmController.setChannelPWM(1, pwmServo1.pwmForAngle(i));
        delay(100); //shorter delay for faster turn movement
    }

  for (i = 0; i >= -90; i -= 5) { //Slow Turn clockwise (from 0 to -90 degree)
        pwmController.setChannelPWM(1, pwmServo1.pwmForAngle(i));
        delay(250); //longer delay for Slow turn movement
    }

  for (i = -90; i <= 0; i += 5) { //Fast Turn anti-clockwise (from -90 to 0 degree)
        pwmController.setChannelPWM(1, pwmServo1.pwmForAngle(i));
        delay(100); //shorter delay for faster turn movement
    }
  } 
 
  digitalWrite(LED_BLUE, LOW);
  delay(100);
}
/************************************************* *********************/
/* Function: Display temperature on 4-digit digital tube */
/* Parameter: -int8_t temperature, temperature range is -40 ~ 125 degrees celsius */
/* Return Value: void */

int flag = 0;
void displayTemperature(int8_t temperature)
{
  int8_t temp[4];


  if(temperature < 0)
	{
		temp[0] = INDEX_NEGATIVE_SIGN;
		temperature = abs(temperature);
	}
	else if(temperature < 100)temp[0] = INDEX_BLANK;
	else temp[0] = temperature/100;
	temperature %= 100;
	temp[1] = temperature / 10;
	temp[2] = temperature % 10;
	temp[3] = 12;	          //index of 'C' for celsius degree symbol.
	disp.display(temp); //display temperature
	if(temperature > 27) //if temperature goes beyond 27 degrees, the aircon will automatically on
	{
		digitalWrite(LED_RED, HIGH); //indicates that the aircon is on
		flag++;
		if(flag == 1) //buzzer plays a sound once to simulate the aircon remote switching on the aircon
			buz.playTone(1000, 100);
	}
	else
	{
		digitalWrite(LED_RED, LOW); //indicates that the aircon is off
		flag = 0;
	}
  delay(1000);
  
  
	return;

}

// testing PWM output of RaspberryPi
// using wiringPi library
#include <wiringPi.h>
#include <stdio.h>

int main (void)
{
	// use library pin1 which is GPIO18 that supports PWM
	const int pwmPin = 1;
	
	// setup pins
	if (wiringPiSetup() == -1)
	{
		printf("Setup failed!");
		return -1;
	}
	
	delay(1000);
	
	pinMode(pwmPin, PWM_OUTPUT);
	
	pwmSetMode(PWM_MODE_MS);
	
	// set frequency to 5Khz which is the standard for this buzzer
	// clock is 19.2 Mhz
    // range of pwmSetClock() is 2 to 4095.
    pwmSetClock (3840);
	
	// active for half a second
	pwmSetRange (2500);
	
	delay(1000);
	
	int i = 0;
	while(1)
	{
		pwmWrite (pwmPin, 1);
		delay(2000);
		pwmWrite (pwmPin, 250);
		delay(2000);
		pwmWrite (pwmPin, 500);
		delay(2000);
		pwmWrite (pwmPin, 1000);
		delay(2000);
	}
}	

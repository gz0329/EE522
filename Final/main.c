#include "colorSensor.h"
#include <stdlib.h>
#include <time.h>

int main (void)
{
  init();
  printf("Initialize the color sensor\n");

  while(1)
  {
		uint16_t r = readRed();
		uint16_t g = readGreen();
		uint16_t b = readBlue();
	
		printf("red: %d green: %d blue: %d\n", r, g, b);
		delay(1000);
  }
}	

// testing SPI output of RaspberryPi
// using loopback method
#include <wiringPi.h>
#include <stdio.h>
#include <wiringPiSPI.h>

// chip selection pin
const int SPI_CHAN = 0;

int main()
{
	int fd, result;
	unsigned char buffer[10];
	
	wiringPiSetup();
	// Configure the interface.
	// CHANNEL insicates chip select,
	// 500000 indicates bus speed.
	fd = wiringPiSPISetup(SPI_CHAN, 500000);
	printf("Init result: %d\n", fd);

	delay(500);

	// try different bytes
	for(int i = 1; i <= 0x7f; i <<= 1)
	{
		// print the byte sent and received
		buffer[0] = i;
		result = wiringPiSPIDataRW(SPI_CHAN, buffer, 1);
		printf( "%d, %d\n", i, buffer[0] );
		delay(100);
	}
}

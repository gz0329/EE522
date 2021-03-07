#include "colorSensor.h"
#include <stdlib.h>
#include <time.h>
#include "stdio.h"
#include "string.h"
#include "MQTTClient.h"
#include <wiringPi.h>

#define ADDRESS     "tcp://localhost:1883"
#define CLIENTID    "ColorSensor"
#define TOPIC       "data/emotion"
#define PAYLOAD     "This is colorSensor"
#define QOS         1
#define TIMEOUT     10000L
volatile MQTTClient_deliveryToken deliveredtoken;

char emotionCurr;
int red = 3;
int green = 0;
int blue = 2;

void delivered(void *context, MQTTClient_deliveryToken dt)
{
	printf("Message with token value %d delivery confirmed\n", dt);
	deliveredtoken = dt;
}

int msgarrvd(void *context, char *topicName, int topicLen, MQTTClient_message *message)
{
	int i;
	char* payloadptr;
	printf("Message arrived\n");
	printf("     topic: %s\n", topicName);
	printf("   message: ");
	payloadptr = message->payload;

	emotionCurr = payloadptr[0];

	for(i=0; i<message->payloadlen; i++)
	{
	    putchar(*payloadptr++);
	}
	putchar('\n');
	MQTTClient_freeMessage(&message);
	MQTTClient_free(topicName);
	return 1;
}

void connlost(void *context, char *cause)
{
	printf("\nConnection lost\n");
	printf("     cause: %s\n", cause);
}

int main (void)
{
	wiringPiSetup();
  	pinMode(green, OUTPUT);
  	pinMode(red, OUTPUT);
  	pinMode(blue, OUTPUT);

  	digitalWrite(red, HIGH);
	digitalWrite(green, HIGH);
	digitalWrite(blue, HIGH);

	MQTTClient client;
	MQTTClient_connectOptions conn_opts = MQTTClient_connectOptions_initializer;
	int rc;
	int ch;
	MQTTClient_create(&client, ADDRESS, CLIENTID,
	    MQTTCLIENT_PERSISTENCE_NONE, NULL);
	conn_opts.keepAliveInterval = 20;
	conn_opts.cleansession = 1;
	MQTTClient_setCallbacks(client, NULL, connlost, msgarrvd, delivered);
	if ((rc = MQTTClient_connect(client, &conn_opts)) != MQTTCLIENT_SUCCESS)
	{
	    printf("Failed to connect, return code %d\n", rc);
	    exit(EXIT_FAILURE);
	}
	printf("Subscribing to topic %s\nfor client %s using QoS%d\n\n"
	       "Press Q<Enter> to quit\n\n", TOPIC, CLIENTID, QOS);
	MQTTClient_subscribe(client, TOPIC, QOS);

	/*while(ch!='Q' && ch != 'q');
	MQTTClient_disconnect(client, 10000);
	MQTTClient_destroy(&client);*/

	// Initialize the color sensor
	init();
	printf("Initialize the color sensor\n");

	while(1)
	{
		uint16_t r = readRed();
		uint16_t g = readGreen();
		uint16_t b = readBlue();

		printf("red: %d green: %d blue: %d\n", r, g, b);
		printf("%s\n", emotionCurr );

		if( r >= g && r >= b )
		{
			digitalWrite(red, LOW);
    		digitalWrite(green, HIGH);
    		digitalWrite(blue, HIGH);
		} else if( b >= r && b >= g )
		{
			digitalWrite(blue, LOW);
    		digitalWrite(green, HIGH);
    		digitalWrite(red, HIGH);
		} else if( g >= r && g >= b )
		{
			digitalWrite(green, LOW);
    		digitalWrite(red, HIGH);
    		digitalWrite(blue, HIGH);
		} else
		{
			digitalWrite(red, HIGH);
    		digitalWrite(green, HIGH);
    		digitalWrite(blue, HIGH);
		}
		delay(500);
	}
}	

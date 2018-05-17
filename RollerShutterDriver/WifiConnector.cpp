#include "WifiConnector.h"
#include <ESP8266WiFi.h>

void WifiConnector::ConnectToWifi()
{
	WiFi.begin(WIFISSID, WIFIPASSWORD); //Connect to your WiFi router

	Serial.println("");

	int connectionSeconds = 0;
	bool isConnected = false;

	while (true)
	{
		if (WiFi.status() != WL_CONNECTED)
		{
			delay(1000);
			Serial.print(".");

			++connectionSeconds;
			if (connectionSeconds >= maxConnectionSeconds)
				break;
		}
		else 
		{
			isConnected = true;
			break;
		}
	}

	if (isConnected) 
	{
		//If connection successful show IP address in serial monitor
		Serial.println("");
		Serial.print("Connected to ");
		Serial.println(WIFISSID);
		Serial.print("IP address: ");
		Serial.println(WiFi.localIP());  //IP address assigned to your ESP
	}
	else 
	{
		Serial.println("");
		Serial.print("Cannot connect to ");
		Serial.println(WIFISSID);
	}
}
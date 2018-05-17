#include "WifiConnector.h"
#include <ESP8266WiFi.h>

void WifiConnector::ConnectToWifi(ConfigManager configManager)
{
	if (configManager.WifiName == "")
	{
		CreateSoftAP();
		return;
	}
	
	const char* wifiname = configManager.WifiName.c_str();
	const char* wifipass = configManager.WifiPass.c_str();

	WiFi.begin(wifiname, wifipass); //Connect to your WiFi router

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
		Serial.println(configManager.WifiName);
		Serial.print("IP address: ");
		Serial.println(WiFi.localIP());  //IP address assigned to your ESP
	}
	else 
	{
		Serial.println("");
		Serial.print("Cannot connect to ");
		Serial.println(configManager.WifiName);

		CreateSoftAP();
	}
}

void WifiConnector::CreateSoftAP()
{
	Serial.println("Creating soft AP");

	bool result = WiFi.softAP("ESP_Same_Pass_As_SSID", "ESP_Same_Pass_As_SSID");
	if (result)
		Serial.println("AP Ready");
	else
		Serial.println("Failed!");
}
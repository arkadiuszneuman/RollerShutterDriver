#include "WifiConnector.h"
#include <ESP8266WiFi.h>

void WifiConnector::Init(Logger &logger)
{
	this->logger = logger;
}

void WifiConnector::ConnectToWifi(ConfigManager &configManager)
{
	if (configManager.WifiName == "")
	{
		CreateSoftAP();
		return;
	}
	
	const char* wifiname = configManager.WifiName.c_str();
	const char* wifipass = configManager.WifiPass.c_str();

	WiFi.begin(wifiname, wifipass); //Connect to your WiFi router

	logger.LogLine();

	int connectionSeconds = 0;
	bool isConnected = false;

	while (true)
	{
		if (WiFi.status() != WL_CONNECTED)
		{
			delay(1000);
			logger.Log(".");

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
		logger.LogLine("");
		logger.Log("Connected to ");
		logger.LogLine(configManager.WifiName);
		logger.Log("IP address: ");
		logger.LogLine(WiFi.localIP().toString());  //IP address assigned to your ESP
	}
	else 
	{
		logger.LogLine("");
		logger.Log("Cannot connect to ");
		logger.LogLine(configManager.WifiName);

		CreateSoftAP();
	}
}

void WifiConnector::CreateSoftAP()
{
	logger.LogLine("Creating soft AP");

	bool result = WiFi.softAP("ESP_Same_Pass_As_SSID", "ESP_Same_Pass_As_SSID");
	if (result)
		logger.LogLine("AP Ready");
	else
		logger.LogLine("Failed!");
}
// WifiConnector.h

#ifndef _WIFICONNECTOR_h
#define _WIFICONNECTOR_h

#include "ConfigManager.h"

#if defined(ARDUINO) && ARDUINO >= 100
	#include "arduino.h"
#else
	#include "WProgram.h"
#endif

class WifiConnector
{
private:
	int maxConnectionSeconds = 10;
	Logger logger;
	void CreateSoftAP();
public:
	void ConnectToWifi(ConfigManager &configManager);
	void Init(Logger &logger);
};

#endif


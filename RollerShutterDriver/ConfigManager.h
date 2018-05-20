// ConfigManager.h

#ifndef _CONFIGMANAGER_h
#define _CONFIGMANAGER_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "arduino.h"
#else
	#include "WProgram.h"
#endif

#include "Logger.h"

class ConfigManager {
private:
	Logger logger;
public:
	void Init(Logger &logger);
	bool SaveConfig();
	bool LoadConfig();

	String WifiName = "";
	String WifiPass = "";
	String InfoUrl = "";
	String Port = "";
	String Uri = "";
};

#endif


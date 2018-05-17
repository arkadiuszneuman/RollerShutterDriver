// ConfigManager.h

#ifndef _CONFIGMANAGER_h
#define _CONFIGMANAGER_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "arduino.h"
#else
	#include "WProgram.h"
#endif

class ConfigManager {
public:
	void Init();
	bool SaveConfig();
	bool LoadConfig();

	String WifiName = "";
	String WifiPass = "";
};

#endif


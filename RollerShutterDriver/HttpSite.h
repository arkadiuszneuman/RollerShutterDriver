// HttpSite.h

#ifndef _HTTPSITE_h
#define _HTTPSITE_h

#include <ESP8266WebServer.h>
#include "ConfigManager.h"

#if defined(ARDUINO) && ARDUINO >= 100
	#include "arduino.h"
#else
	#include "WProgram.h"
#endif

class HttpSite 
{
private:
	ESP8266WebServer* server; //Server on port 80
	ConfigManager configManager;
	String status = "Idle";
	void(*receivedLevelFunc)(int);
	void Index();
	void Config();
	void ConfigPost();
	void ResetSettings();
	void ChangeLevel();
public:
	void Init(ConfigManager configManager, void(*receivedLevelFunc)(int));
	void Update();
	void SetStatus(String status);
	void SendInformationAboutLevel(int level);
};

#endif


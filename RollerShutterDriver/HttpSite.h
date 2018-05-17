// HttpSite.h

#ifndef _HTTPSITE_h
#define _HTTPSITE_h

#include <ESP8266WebServer.h>

#if defined(ARDUINO) && ARDUINO >= 100
	#include "arduino.h"
#else
	#include "WProgram.h"
#endif

class HttpSite 
{
private:
	ESP8266WebServer* server; //Server on port 80
	void HandleRoot();
public:
	void Init();
	void Update();
};

#endif


#ifndef _OTADRIVER_h
#define _OTADRIVER_h

#if defined(ARDUINO) && ARDUINO >= 100
#include "arduino.h"
#else
#include "WProgram.h"
#endif

#include "Logger.h"

class OtaDriver
{
private:
	Logger logger;
public:
	void Init(Logger &logger);
	void Update();
};

#endif


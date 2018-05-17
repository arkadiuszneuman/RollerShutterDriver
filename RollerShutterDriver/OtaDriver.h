#ifndef _OTADRIVER_h
#define _OTADRIVER_h

#if defined(ARDUINO) && ARDUINO >= 100
#include "arduino.h"
#else
#include "WProgram.h"
#endif

class OtaDriver
{
public:
	void Init();
	void Update();
};

#endif


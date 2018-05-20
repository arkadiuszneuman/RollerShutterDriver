// Logger.h

#ifndef _LOGGER_h
#define _LOGGER_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "arduino.h"
#else
	#include "WProgram.h"
#endif

#define logHistoryCount 30

class Logger
{
private:
	String logHistory[logHistoryCount];
	int logLine = 0;

public:
	void Init();
	void Log(String text);
	void LogLine(String text);
	String* GetLogHistory();
};

#endif

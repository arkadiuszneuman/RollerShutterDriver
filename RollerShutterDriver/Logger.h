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
	//String* logHistory;
	int logLine = 0;

public:
	void Init();
	void Log(String text);
	void LogLine(String text = "");
	void LogLine(int text);
	String* GetLogHistory();
};

#endif


#include "Logger.h"

void Logger::Init()
{
	Serial.begin(115200);

	for (int i = 0; i < logHistoryCount; i++)
		logHistory[i] = "";

	LogLine("Log initialized");
}

void Logger::Log(String text)
{
	Serial.print(text);
}

void Logger::LogLine(String text)
{
	Serial.println(text);

	if (logLine > 0)
	{
		for (int i = logLine; i > 0; --i)
		{
			logHistory[logLine] = logHistory[logLine - 1];
		}
	}

	logHistory[logLine] = text;

	if (logLine < logHistoryCount)
		++logLine;
}

String* Logger::GetLogHistory()
{
	return logHistory;
}


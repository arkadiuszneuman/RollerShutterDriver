#include "Logger.h"

void Logger::Init()
{
	Serial.begin(115200);

	//logHistory = new String[logHistoryCount];

	//for (int i = 0; i < logHistoryCount; i++)
	//	logHistory[i] = "";

	//LogLine("Log initialized");
}

void Logger::Log(String text)
{
	Serial.print(text);
	/*logHistory[0] += text;*/
}

void Logger::LogLine(String text)
{
	Serial.println(text);
	/*logHistory[0] += text;

	if (logLine > 0)
	{
		for (int i = logLine - 1; i > 0; --i)
		{
			logHistory[i] = logHistory[i - 1];
		}
	}

	logHistory[0] = "";

	if (logLine < logHistoryCount)
		++logLine;*/
}

void Logger::LogLine(int text)
{
	char number[12];
	sprintf(number, "%d", text);
	LogLine(number);
}

void Logger::Log(int text)
{
	char number[12];
	sprintf(number, "%d", text);
	Log(number);
}

String* Logger::GetLogHistory()
{
	//return logHistory;
}


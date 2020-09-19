#include <ArduinoJson.hpp>
#include <ArduinoJson.h>

#include "ConfigManager.h"
#include "OtaDriver.h"
#include "ButtonStateChecker.h"
#include "WifiConnector.h"
#include "HttpSite.h"
#include "Logger.h"

#define FULL_ROLLER_MOVE_TIME 43000
#define BUTTON_DOWN_PIN D7 //D1
#define BUTTON_UP_PIN D1 //D2
#define RELAY_DOWN_PIN 14
#define RELAY_UP_PIN 12

//#undef max
//#define max(a,b) ((a)>(b)?(a):(b))
//#undef min
//#define min(a,b) ((a)>(b)?(b):(a))

ButtonStateChecker* buttonDown;
ButtonStateChecker* buttonUp;

OtaDriver otaDriver;

bool isRollerMovingUp = false;
bool isRollerMovingDown = false;

unsigned long rollerMillis = 0;
unsigned long rollerMillisFromTop = 0;
unsigned long rollerFinalMillis = 0;

bool isMovedFromButton;
bool fullRollerMove = false;
int lastLogMillis = 0;

Logger logger;
ConfigManager configManager;
WifiConnector wifiConnector;
HttpSite httpSite;

void setup()
{
	pinMode(RELAY_UP_PIN, OUTPUT);
	pinMode(RELAY_DOWN_PIN, OUTPUT);

	digitalWrite(RELAY_UP_PIN, HIGH);
	digitalWrite(RELAY_DOWN_PIN, HIGH);

	pinMode(BUTTON_DOWN_PIN, INPUT);
	pinMode(BUTTON_UP_PIN, INPUT);

	logger.Init();

	buttonDown = new ButtonStateChecker(BUTTON_DOWN_PIN, logger);
	buttonUp = new ButtonStateChecker(BUTTON_UP_PIN, logger);
	

	configManager.Init(logger);
	configManager.LoadConfig();

	wifiConnector.Init(logger);
	wifiConnector.ConnectToWifi(configManager);
	httpSite.Init(configManager, logger, receive);

	otaDriver.Init(logger);
}

void receive(int requestedLevel)
{
	StopMovingRoller();

	requestedLevel = min(requestedLevel, 100);
	requestedLevel = max(requestedLevel, 0);

	logger.Log("Changing level to ");
	logger.LogLine(requestedLevel);

	long millisToSet = requestedLevel * 0.01 * FULL_ROLLER_MOVE_TIME;

	logger.Log("millisToSet ");
	logger.LogLine(millisToSet);

	isMovedFromButton = false;
	fullRollerMove = false;
	MoveRoller(millisToSet);
}

void loop()
{
	httpSite.Update();
	otaDriver.Update();

	int buttonUpState = buttonUp->CheckButton();
	int buttonDownState = buttonDown->CheckButton();

	if (buttonUpState > 0)
	{
		buttonDown->ResetState();
		logger.Log("buttonUpState ");
		logger.LogLine(buttonUpState);

		if (isRollerMoving())
		{
			StopMovingRoller();
		}
		else
		{
			if (buttonUpState == CLICK)
			{
				fullRollerMove = false;
			}
			else if (buttonUpState == LONG_HOLD)
			{
				fullRollerMove = true;
			}

			logger.Log("fullRollerMove ");
			logger.LogLine(fullRollerMove);

			isMovedFromButton = true;
			MoveRoller(0);
		}
	}
	else if (buttonDownState > 0)
	{
		buttonUp->ResetState();
		logger.Log("buttonDownState ");
		logger.LogLine(buttonDownState);

		if (isRollerMoving())
		{
			StopMovingRoller();
		}
		else
		{
			if (buttonDownState == CLICK)
			{
				fullRollerMove = false;
			}
			else if (buttonDownState == LONG_HOLD)
			{
				fullRollerMove = true;
			}

			logger.Log("fullRollerMove ");
			logger.LogLine(fullRollerMove);

			isMovedFromButton = true;
			MoveRoller(FULL_ROLLER_MOVE_TIME);
		}
	}

	if (isTimePassed())
	{
		StopMovingRoller();
	}

	if (isRollerMoving())
	{
		if ((millis() - lastLogMillis) > 100)
		{
			logger.Log(".");
			lastLogMillis = millis();
		}
	}
}

bool isRollerMoving()
{
	return isRollerMovingUp || isRollerMovingDown;
}

bool isTimePassed()
{
	if (isRollerMoving())
	{
		unsigned long passedTime = SubstractWithoutOverflow(millis(), rollerMillis);

		if (fullRollerMove)
		{
			/*logger.Log("is passed by full roller move ");
			logger.LogLine(passedTime > FULL_ROLLER_MOVE_TIME);*/

			return passedTime > FULL_ROLLER_MOVE_TIME;
		}

		//max top or max bottom should keep relay for 5 seconds more
		if (rollerFinalMillis == 0 || rollerFinalMillis == FULL_ROLLER_MOVE_TIME)
		{
			if (isRollerMovingUp)
			{
				return SubstractWithoutOverflow(passedTime, 5000) > SubstractWithoutOverflow(rollerMillisFromTop, rollerFinalMillis);
			}
			else if (isRollerMovingDown)
			{
				return SubstractWithoutOverflow(passedTime, 5000) > SubstractWithoutOverflow(rollerFinalMillis, rollerMillisFromTop);
			}
		}

		if (isRollerMovingUp)
		{
			return passedTime > SubstractWithoutOverflow(rollerMillisFromTop, rollerFinalMillis);
		}
		else if (isRollerMovingDown)
		{
			return passedTime > SubstractWithoutOverflow(rollerFinalMillis, rollerMillisFromTop);
		}
	}

	return false;
}

void MoveRoller(long millisToSet)
{
	if (millisToSet < FULL_ROLLER_MOVE_TIME)
		rollerFinalMillis = millisToSet;
	else
		rollerFinalMillis = FULL_ROLLER_MOVE_TIME;

	if (rollerFinalMillis < 0)
		rollerFinalMillis = 0;

	if (rollerFinalMillis != rollerMillisFromTop || fullRollerMove)
	{
		logger.Log("Millis to set ");
		logger.LogLine(rollerFinalMillis);

		logger.Log("Millis from top ");
		logger.LogLine(rollerMillisFromTop);

		rollerMillis = millis();

		if (rollerFinalMillis > rollerMillisFromTop || (fullRollerMove && rollerFinalMillis == FULL_ROLLER_MOVE_TIME))
		{
			MoveRollerDown();
		}
		else
		{
			MoveRollerUp();
		}
	}
}

void MoveRollerUp()
{
	if (rollerMillisFromTop > 0 || fullRollerMove)
	{
		digitalWrite(RELAY_UP_PIN, LOW);
		isRollerMovingUp = true;
		isRollerMovingDown = false;
	}
}

void MoveRollerDown()
{
	if (rollerMillisFromTop < FULL_ROLLER_MOVE_TIME || fullRollerMove)
	{
		digitalWrite(RELAY_DOWN_PIN, LOW);
		isRollerMovingUp = false;
		isRollerMovingDown = true;
	}
}

void StopMovingRoller()
{
	unsigned long passedTime = SubstractWithoutOverflow(millis(), rollerMillis);

	digitalWrite(RELAY_UP_PIN, HIGH);
	digitalWrite(RELAY_DOWN_PIN, HIGH);

	logger.Log("passedTime ");
	logger.LogLine(passedTime);
	logger.Log("isRollerMovingUp ");
	logger.LogLine(isRollerMovingUp);

	if (isRollerMovingUp)
	{
		rollerMillisFromTop = SubstractWithoutOverflow(rollerMillisFromTop, passedTime);
	}
	else if (isRollerMovingDown)
	{
		rollerMillisFromTop += passedTime;
	}

	logger.Log("before Setting rollerMillisFromTop ");
	logger.LogLine(rollerMillisFromTop);

	if (rollerMillisFromTop > FULL_ROLLER_MOVE_TIME)
		rollerMillisFromTop = FULL_ROLLER_MOVE_TIME;

	if (rollerMillisFromTop < 0)
		rollerMillisFromTop = 0;

	logger.Log("Setting rollerMillisFromTop ");
	logger.LogLine(rollerMillisFromTop);

	isRollerMovingUp = false;
	isRollerMovingDown = false;

	int currentLevel = rollerMillisFromTop * 100 / FULL_ROLLER_MOVE_TIME * 1.0;
	
	logger.Log("Current level ");
	logger.LogLine(currentLevel);

	SendMessage(currentLevel);

	logger.LogLine(" ");
	logger.LogLine(" ");
}

unsigned long SubstractWithoutOverflow(unsigned long firstNumber, unsigned long secondNumber)
{
	if (firstNumber < secondNumber)
	{
		return 0;
	}

	return firstNumber - secondNumber;
}

void SendMessage(int currentLevel)
{
	if (isMovedFromButton)
	{
		httpSite.SendInformationAboutLevel(currentLevel);
	}
}


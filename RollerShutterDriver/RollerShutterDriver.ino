//#include <ESP8266WebServer.h>
//#include <WiFiUdp.h>
//#include <WiFiServer.h>
//#include <WiFiClientSecure.h>
//#include <WiFiClient.h>
//#include <ESP8266WiFiType.h>
//#include <ESP8266WiFiSTA.h>
//#include <ESP8266WiFiScan.h>
//#include <ESP8266WiFiMulti.h>
//#include <ESP8266WiFiGeneric.h>
//#include <ESP8266WiFiAP.h>
//#include <ESP8266WiFi.h>
//#include <ESP8266mDNS.h>
//#include <ArduinoOTA.h>

#include "OtaDriver.h"
#include "ButtonStateChecker.h"
#include "WifiConnector.h"
#include "HttpSite.h"

#define FULL_ROLLER_MOVE_TIME 43000
#define BUTTON_DOWN_PIN 2
#define BUTTON_UP_PIN 3
#define RELAY_DOWN_PIN 5
#define RELAY_UP_PIN 6

#undef max
#define max(a,b) ((a)>(b)?(a):(b))
#undef min
#define min(a,b) ((a)>(b)?(b):(a))

ButtonStateChecker buttonDown(BUTTON_DOWN_PIN);
ButtonStateChecker buttonUp(BUTTON_UP_PIN);

OtaDriver otaDriver;

bool isRollerMovingUp = false;
bool isRollerMovingDown = false;

unsigned long rollerMillis = 0;
unsigned long rollerMillisFromTop = 0;
unsigned long rollerFinalMillis = 0;

bool isMovedFromButton;
bool fullRollerMove = false;

WifiConnector wifiConnector;
HttpSite httpSite;

void setup()
{
	Serial.begin(9600);
	wifiConnector.ConnectToWifi();
	httpSite.Init();

	/*otaDriver.Init();

	pinMode(RELAY_UP_PIN, OUTPUT);
	pinMode(RELAY_DOWN_PIN, OUTPUT);

	StopMovingRoller();*/

	

	// Wait for connection
	


	
}

//void receive(const MyMessage &message)
//{
//	if (message.type == V_LIGHT || message.type == V_DIMMER)
//	{
//		StopMovingRoller();
//
//		int requestedLevel = atoi(message.data);
//
//		requestedLevel *= (message.type == V_LIGHT ? 100 : 1);
//
//		requestedLevel = min(requestedLevel, 100);
//		requestedLevel = max(requestedLevel, 0);
//
//		Serial.print("Changing level to ");
//		Serial.println(requestedLevel);
//
//		long millisToSet = requestedLevel * 0.01 * FULL_ROLLER_MOVE_TIME;
//
//		Serial.print("millisToSet ");
//		Serial.println(millisToSet);
//
//		isMovedFromButton = false;
//		fullRollerMove = false;
//		MoveRoller(millisToSet);
//	}
//}

void loop()
{
	httpSite.Update();

	//otaDriver.Update();

	/*int buttonUpState = buttonUp.CheckButton();
	if (buttonUpState > 0)
	{
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

			Serial.print("fullRollerMove ");
			Serial.println(fullRollerMove);

			isMovedFromButton = true;
			MoveRoller(0);
		}
	}

	int buttonDownState = buttonDown.CheckButton();
	if (buttonDownState > 0)
	{
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

			Serial.print("fullRollerMove ");
			Serial.println(fullRollerMove);

			isMovedFromButton = true;
			MoveRoller(FULL_ROLLER_MOVE_TIME);
		}
	}

	if (isTimePassed())
	{
		StopMovingRoller();
	}*/
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
			Serial.print("is passed by full roller move ");
			Serial.println(passedTime > FULL_ROLLER_MOVE_TIME);

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
	rollerFinalMillis = max(min(millisToSet, FULL_ROLLER_MOVE_TIME), 0);

	if (rollerFinalMillis != rollerMillisFromTop || fullRollerMove)
	{
		Serial.print("Millis to set ");
		Serial.println(rollerFinalMillis);

		Serial.print("Millis from top ");
		Serial.println(rollerMillisFromTop);

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

	Serial.print("passedTime ");
	Serial.println(passedTime);
	Serial.print("isRollerMovingUp ");
	Serial.println(isRollerMovingUp);

	if (isRollerMovingUp)
	{
		rollerMillisFromTop = SubstractWithoutOverflow(rollerMillisFromTop, passedTime);
	}
	else if (isRollerMovingDown)
	{
		rollerMillisFromTop += passedTime;
	}

	Serial.print("before Setting rollerMillisFromTop ");
	Serial.println(rollerMillisFromTop);
	rollerMillisFromTop = max(min(rollerMillisFromTop, FULL_ROLLER_MOVE_TIME), 0);
	Serial.print("Setting rollerMillisFromTop ");
	Serial.println(rollerMillisFromTop);

	isRollerMovingUp = false;
	isRollerMovingDown = false;

	int currentLevel = rollerMillisFromTop * 100 / FULL_ROLLER_MOVE_TIME * 1.0;

	Serial.println(currentLevel);

	SendMessage(currentLevel);
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
		//send(dimmerMessage.set(currentLevel));
	}
}


#define MY_DEBUG
#define MY_RADIO_NRF24
#define MY_REPEATER_FEATURE

#include <MyConfig.h>
#include <MySensors.h>
#include "ButtonStateChecker.h"

ButtonStateChecker buttonDown(2);
ButtonStateChecker buttonUp(3);

const int upRelayPin = 5;
const int downRelayPin = 6;

bool isRollerMovingUp = false;
bool isRollerMovingDown = false;

const int fullRollerMoveTime = 5000;
unsigned long rollerMillis = 0;
unsigned int rollerMillisFromTop = 0;

void setup() 
{
	Serial.begin(9600);

	pinMode(upRelayPin, OUTPUT);
	pinMode(downRelayPin, OUTPUT);

	digitalWrite(upRelayPin, HIGH);
	digitalWrite(downRelayPin, HIGH);

	StopMovingRelays();
}

void loop()
{
	//TODO long hold or double click should move without watching time to end
	if (buttonUp.CheckButton() == 2) 
	{
		Serial.write("Up button");
		if (isRelayMoving())
		{
			StopMovingRelays();
		}
		else
		{
			MoveRelayUp();
		}
	}

	if (buttonDown.CheckButton() == 2)
	{
		Serial.write("Down button");
		if (isRelayMoving())
		{
			StopMovingRelays();
		}
		else
		{
			MoveRelayDown();
		}
	}

	if (isTimePassed())
	{
		bool movedUp = isRollerMovingUp;
		bool movedDown = isRollerMovingDown;

		StopMovingRelays();

		if (movedUp)
			rollerMillisFromTop = 0;
		else if (movedDown)
			rollerMillisFromTop = fullRollerMoveTime;
	}
}

bool isRelayMoving()
{
	return isRollerMovingUp || isRollerMovingDown;
}

bool isTimePassed() 
{
	if (isRelayMoving())
	{
		//TODO millis overflow
		unsigned long passedTime = millis() - rollerMillis;

		if (isRollerMovingUp)
		{
			return passedTime > rollerMillisFromTop;
		}
		else if (isRollerMovingDown)
		{
			return passedTime > fullRollerMoveTime - rollerMillisFromTop;
		}
	}

	return false;
}

void MoveRelayUp() 
{
	if (rollerMillisFromTop > 0)
	{
		digitalWrite(upRelayPin, LOW);
		isRollerMovingUp = true;
		isRollerMovingDown = false;
		rollerMillis = millis();
	}
}

void MoveRelayDown()
{
	if (rollerMillisFromTop < fullRollerMoveTime)
	{
		digitalWrite(downRelayPin, LOW);
		isRollerMovingUp = false;
		isRollerMovingDown = true;
		rollerMillis = millis();
	}
}

void StopMovingRelays() 
{
	unsigned int passedTime = millis() - rollerMillis;

	digitalWrite(upRelayPin, HIGH);
	digitalWrite(downRelayPin, HIGH);

	if (isRollerMovingUp)
	{
		rollerMillisFromTop -= passedTime;
	}
	else if (isRollerMovingDown)
	{
		rollerMillisFromTop += passedTime;
	}

	isRollerMovingUp = false;
	isRollerMovingDown = false;
}


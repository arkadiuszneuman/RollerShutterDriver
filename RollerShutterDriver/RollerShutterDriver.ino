
#include "ButtonStateChecker.h"

ButtonStateChecker buttonDown(2);
ButtonStateChecker buttonUp(3);

const int upRelayPin = 5;
const int downRelayPin = 6;

bool isRollerMovingUp = false;
bool isRollerMovingDown = false;

unsigned long rollerMillis = 0;

void setup() 
{
	Serial.begin(9600);

	pinMode(upRelayPin, OUTPUT);
	pinMode(downRelayPin, OUTPUT);

	digitalWrite(upRelayPin, HIGH);
	digitalWrite(downRelayPin, HIGH);

	MoveRelayUp();
}

void loop()
{
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
			
		}
	}

	if (isTimePassed())
	{
		StopMovingRelays();
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

		if (passedTime > 5000)
		{
			return true;
		}

	}

	return false;
}

void MoveRelayUp() 
{
	digitalWrite(upRelayPin, LOW);
	isRollerMovingUp = true;
	isRollerMovingDown = false;
	rollerMillis = millis();
}

void StopMovingRelays() 
{
	digitalWrite(upRelayPin, HIGH);
	digitalWrite(downRelayPin, HIGH);
	isRollerMovingUp = false;
	isRollerMovingDown = false;
}


#define MY_DEBUG
#define MY_RADIO_NRF24
#define MY_REPEATER_FEATURE

#include <MySensors.h>
#include "ButtonStateChecker.h"

#define NODE_ID 1
#define BUTTON_DOWN_PIN 2
#define BUTTON_UP_PIN 3
#define RELAY_DOWN_PIN 5
#define RELAY_UP_PIN 6

ButtonStateChecker buttonDown(BUTTON_DOWN_PIN);
ButtonStateChecker buttonUp(BUTTON_UP_PIN);

bool isRollerMovingUp = false;
bool isRollerMovingDown = false;

const int fullRollerMoveTime = 5000;
unsigned long rollerMillis = 0;
unsigned int rollerMillisFromTop = 0;

void setup()
{
	Serial.begin(115200);
	Serial.println("setup");

	pinMode(RELAY_UP_PIN, OUTPUT);
	pinMode(RELAY_DOWN_PIN, OUTPUT);

	StopMovingRelays();
}

void presentation()
{
	Serial.println("presentation");
	sendSketchInfo("WindowCover", "1.0");
	present(NODE_ID, S_DIMMER);
}

void receive(const MyMessage &message)
{
	Serial.println(message.type);
	Serial.println(message.bValue);

	int requestedLevel = atoi(message.data);
	Serial.println(message.data);
	Serial.println(requestedLevel);

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
		digitalWrite(RELAY_UP_PIN, LOW);
		isRollerMovingUp = true;
		isRollerMovingDown = false;
		rollerMillis = millis();
	}
}

void MoveRelayDown()
{
	if (rollerMillisFromTop < fullRollerMoveTime)
	{
		digitalWrite(RELAY_DOWN_PIN, LOW);
		isRollerMovingUp = false;
		isRollerMovingDown = true;
		rollerMillis = millis();
	}
}

void StopMovingRelays()
{
	unsigned int passedTime = millis() - rollerMillis;

	digitalWrite(RELAY_UP_PIN, HIGH);
	digitalWrite(RELAY_DOWN_PIN, HIGH);

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


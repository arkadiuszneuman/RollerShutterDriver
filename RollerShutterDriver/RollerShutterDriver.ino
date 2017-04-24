#define MY_DEBUG
#define MY_RADIO_NRF24
#define MY_REPEATER_FEATURE

#include <MySensors.h>
#include "ButtonStateChecker.h"

#define FULL_ROLLER_MOVE_TIME 10000
#define BUTTON_DOWN_PIN 2
#define BUTTON_UP_PIN 3
#define RELAY_DOWN_PIN 5
#define RELAY_UP_PIN 6
#define NODE_ID 1

ButtonStateChecker buttonDown(BUTTON_DOWN_PIN);
ButtonStateChecker buttonUp(BUTTON_UP_PIN);

bool isRollerMovingUp = false;
bool isRollerMovingDown = false;

unsigned long rollerMillis = 0;
unsigned int rollerMillisFromTop = 0;
unsigned int rollerFinalMillis = 0;

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
	if (message.type == V_LIGHT || message.type == V_DIMMER) {

		int requestedLevel = atoi(message.data);

		requestedLevel *= (message.type == V_LIGHT ? 100 : 1);

		requestedLevel = min(requestedLevel, 100);
		requestedLevel = max(requestedLevel, 0);

		Serial.print("Changing level to ");
		Serial.println(requestedLevel);

		int millisToSet = requestedLevel * 0.01 * FULL_ROLLER_MOVE_TIME;

		MoveRoller(millisToSet);
	}
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
			MoveRoller(0);
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
			MoveRoller(FULL_ROLLER_MOVE_TIME);
		}
	}

	if (isTimePassed())
	{
		StopMovingRelays();

		if (rollerFinalMillis == 0)
			rollerMillisFromTop = rollerFinalMillis;
		else if (rollerFinalMillis == FULL_ROLLER_MOVE_TIME)
			rollerMillisFromTop = FULL_ROLLER_MOVE_TIME;
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
			return passedTime > rollerMillisFromTop - rollerFinalMillis;
		}
		else if (isRollerMovingDown)
		{
			return passedTime > rollerFinalMillis - rollerMillisFromTop;
		}
	}

	return false;
}

void MoveRoller(int millisToSet)
{
	rollerFinalMillis = max(min(millisToSet, FULL_ROLLER_MOVE_TIME), 0);
	if (rollerFinalMillis > rollerMillisFromTop)
	{
		MoveRelayDown();
	}
	else
	{
		MoveRelayUp();
	}
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
	if (rollerMillisFromTop < FULL_ROLLER_MOVE_TIME)
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


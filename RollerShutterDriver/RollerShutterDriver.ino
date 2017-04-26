//#define MY_DEBUG
#define MY_RADIO_NRF24
#define MY_REPEATER_FEATURE

#include <MySensors.h>
#include "ButtonStateChecker.h"

#define FULL_ROLLER_MOVE_TIME 5000
#define BUTTON_DOWN_PIN 2
#define BUTTON_UP_PIN 3
#define RELAY_DOWN_PIN 5
#define RELAY_UP_PIN 6
#define NODE_ID 0

ButtonStateChecker buttonDown(BUTTON_DOWN_PIN);
ButtonStateChecker buttonUp(BUTTON_UP_PIN);

bool isRollerMovingUp = false;
bool isRollerMovingDown = false;

unsigned long rollerMillis = 0;
int rollerMillisFromTop = 0;
int rollerFinalMillis = 0;

bool isMovedFromButton;

MyMessage dimmerMessage(NODE_ID, V_DIMMER);

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
	if (message.type == V_LIGHT || message.type == V_DIMMER)
	{
		StopMovingRelays();

		int requestedLevel = atoi(message.data);

		requestedLevel *= (message.type == V_LIGHT ? 100 : 1);

		requestedLevel = min(requestedLevel, 100);
		requestedLevel = max(requestedLevel, 0);

		Serial.print("Changing level to ");
		Serial.println(requestedLevel);

		int millisToSet = requestedLevel * 0.01 * FULL_ROLLER_MOVE_TIME;

		isMovedFromButton = false;
		MoveRoller(millisToSet);
	}
}

void loop()
{
	//TODO long hold or double click should move without watching time to end
	if (buttonUp.CheckButton() == CLICK)
	{
		if (isRelayMoving())
		{
			StopMovingRelays();
		}
		else
		{
			isMovedFromButton = true;
			MoveRoller(0);
		}
	}

	if (buttonDown.CheckButton() == CLICK)
	{
		if (isRelayMoving())
		{
			StopMovingRelays();
		}
		else
		{
			isMovedFromButton = true;
			MoveRoller(FULL_ROLLER_MOVE_TIME);
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
	if (rollerFinalMillis != rollerMillisFromTop)
	{
		Serial.print("Millis to set ");
		Serial.println(rollerFinalMillis);

		Serial.print("Millis from top ");
		Serial.println(rollerMillisFromTop);

		if (rollerFinalMillis > rollerMillisFromTop)
		{
			MoveRelayDown();
		}
		else
		{
			MoveRelayUp();
		}
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

	Serial.print("passedTime ");
	Serial.println(passedTime);
	Serial.print("isRollerMovingUp ");
	Serial.println(isRollerMovingUp);

	if (isRollerMovingUp)
	{
		rollerMillisFromTop -= passedTime;
	}
	else if (isRollerMovingDown)
	{
		rollerMillisFromTop += passedTime;
	}

	Serial.print("before Setting rollerMillisFromTop ");
	Serial.println(rollerMillisFromTop);
	rollerMillisFromTop = max(min(rollerMillisFromTop, FULL_ROLLER_MOVE_TIME), 1);
	Serial.print("Setting rollerMillisFromTop ");
	Serial.println(rollerMillisFromTop);

	isRollerMovingUp = false;
	isRollerMovingDown = false;

	int currentLevel = rollerMillisFromTop * 100 / FULL_ROLLER_MOVE_TIME * 1.0;

	Serial.println(currentLevel);

	SendMessage(currentLevel);
}

void SendMessage(int currentLevel)
{
	if (isMovedFromButton)
	{
		send(dimmerMessage.set(currentLevel));
	}
}


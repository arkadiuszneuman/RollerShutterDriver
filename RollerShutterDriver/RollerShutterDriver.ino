//#define MY_DEBUG
#define MY_RADIO_NRF24
#define MY_REPEATER_FEATURE

#include <MySensors.h>
#include "ButtonStateChecker.h"

#define FULL_ROLLER_MOVE_TIME 45000
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
unsigned long rollerMillisFromTop = 0;
unsigned long rollerFinalMillis = 0;

bool isMovedFromButton;
bool fullRollerMove = false;

MyMessage dimmerMessage(NODE_ID, V_DIMMER);

void setup()
{
	Serial.begin(115200);
	Serial.println("setup");

	pinMode(RELAY_UP_PIN, OUTPUT);
	pinMode(RELAY_DOWN_PIN, OUTPUT);

	StopMovingRoller();
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
		StopMovingRoller();

		int requestedLevel = atoi(message.data);

		requestedLevel *= (message.type == V_LIGHT ? 100 : 1);

		requestedLevel = min(requestedLevel, 100);
		requestedLevel = max(requestedLevel, 0);

		Serial.print("Changing level to ");
		Serial.println(requestedLevel);

		long millisToSet = requestedLevel * 0.01 * FULL_ROLLER_MOVE_TIME;

		Serial.print("millisToSet ");
		Serial.println(millisToSet);

		isMovedFromButton = false;
		fullRollerMove = false;
		MoveRoller(millisToSet);
	}
}

void loop()
{
	int buttonUpState = buttonUp.CheckButton();
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
			Serial.print("is passed by full roller move ");
			Serial.println(passedTime > FULL_ROLLER_MOVE_TIME);

			return passedTime > FULL_ROLLER_MOVE_TIME;
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

		if (rollerFinalMillis > rollerMillisFromTop || (fullRollerMove && rollerFinalMillis == 100))
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
		send(dimmerMessage.set(currentLevel));
	}
}


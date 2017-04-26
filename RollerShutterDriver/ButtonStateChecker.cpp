// 
// 
// 

#include "ButtonStateChecker.h"

ButtonStateChecker::ButtonStateChecker(int buttonPin)
{
	this->buttonPin = buttonPin;
	pinMode(buttonPin, INPUT);
	upTime = millis();
	downTime = millis();
}

int ButtonStateChecker::CheckButton()
{
	int event = NOTHING;
	int buttonVal = digitalRead(buttonPin);

	// Button pressed down
	if (buttonVal == HIGH && buttonLast == LOW && (millis() - upTime) > debounce)
	{
		downTime = millis();
		ignoreUp = false;
		waitForUp = false;
		singleOK = true;
		holdEventPast = false;
		longHoldEventPast = false;
		if ((millis() - upTime) < DCgap && DConUp == false && DCwaiting == true)  DConUp = true;
		else  DConUp = false;
		DCwaiting = false;

		Serial.println("Down");
	}
	// Button released
	else if (buttonVal == LOW && buttonLast == HIGH && (millis() - downTime) > debounce)
	{
		Serial.println("Up");
		if (!ignoreUp)
		{
			upTime = millis();
			if (DConUp == true) DCwaiting = true;
			else
			{
				event = CLICK;
				DConUp = false;
				DCwaiting = false;
				singleOK = false;
			}
		}
	}
	// Test for normal click event: DCgap expired
	if (buttonVal == LOW && (millis() - upTime) >= DCgap && DCwaiting == true && DConUp == false && singleOK == true && event != 2)
	{
		event = DOUBLE_CLICK;
		DCwaiting = false;
	}
	// Test for hold
	if (buttonVal == HIGH && (millis() - downTime) >= holdTime) 
	{
		// Trigger "normal" hold
		if (!holdEventPast)
		{
			event = LONG_HOLD;
			waitForUp = true;
			ignoreUp = true;
			DConUp = false;
			DCwaiting = false;
			downTime = millis();
			holdEventPast = true;
		}
		// Trigger "long" hold
		if ((millis() - downTime) >= longHoldTime)
		{
			if (!longHoldEventPast)
			{
				event = VERY_LONG_HOLD;
				longHoldEventPast = true;
			}
		}
	}
	buttonLast = buttonVal;
	return event;
}
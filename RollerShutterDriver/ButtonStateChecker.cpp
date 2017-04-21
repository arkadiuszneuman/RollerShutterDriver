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
	int event = 0;
	buttonVal = digitalRead(buttonPin);
	// Button pressed down
	if (buttonVal == LOW && buttonLast == HIGH && (millis() - upTime) > debounce)
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
	}
	// Button released
	else if (buttonVal == HIGH && buttonLast == LOW && (millis() - downTime) > debounce)
	{
		if (!ignoreUp)
		{
			upTime = millis();
			if (DConUp == true) DCwaiting = true;
			else
			{
				event = 2;
				DConUp = false;
				DCwaiting = false;
				singleOK = false;
			}
		}
	}
	// Test for normal click event: DCgap expired
	if (buttonVal == HIGH && (millis() - upTime) >= DCgap && DCwaiting == true && DConUp == false && singleOK == true && event != 2)
	{
		event = 1;
		DCwaiting = false;
	}
	// Test for hold
	if (buttonVal == LOW && (millis() - downTime) >= holdTime) {
		// Trigger "normal" hold
		if (!holdEventPast)
		{
			event = 3;
			waitForUp = true;
			ignoreUp = true;
			DConUp = false;
			DCwaiting = false;
			//downTime = millis();
			holdEventPast = true;
		}
		// Trigger "long" hold
		if ((millis() - downTime) >= longHoldTime)
		{
			if (!longHoldEventPast)
			{
				event = 4;
				longHoldEventPast = true;
			}
		}
	}
	buttonLast = buttonVal;
	return event;
}
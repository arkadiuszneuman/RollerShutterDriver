#include "ButtonStateChecker.h"

ButtonStateChecker::ButtonStateChecker(int buttonPin, Logger &logger)
{
	this->logger = logger;
	this->buttonPin = buttonPin;
	pinMode(buttonPin, INPUT);
	upTime = millis();
	downTime = millis();
}

void ButtonStateChecker::ResetState()
{
	isUpPushed = false;
	isDownPushed = false;
	buttonLast = LOW;  // buffered value of the button's previous state
	DCwaiting = false;  // whether we're waiting for a double click (down)
	DConUp = false;     // whether to register a double click on next release, or whether to wait and click
	singleOK = true;    // whether it's OK to do a single click
	downTime = -1;         // time the button was pressed down
	upTime = -1;           // time the button was released
	ignoreUp = false;   // whether to ignore the button release because the click+hold was triggered
	waitForUp = false;        // when held, whether to wait for the up event
	holdEventPast = false;    // whether or not the hold event happened already
	longHoldEventPast = false;// whether or not the long hold event happened already

	logger.Log("Reset state button ");
	logger.LogLine(buttonPin);
}

int ButtonStateChecker::CheckButton()
{
	int event = NOTHING;
	int buttonVal = digitalRead(buttonPin);

	// Button pressed down
	if (buttonVal == HIGH && buttonLast == LOW && (millis() - upTime) > debounce)
	{
		isUpPushed = false;
		if (!isDownPushed)
		{
			downTime = millis();
			ignoreUp = false;
			waitForUp = false;
			singleOK = true;
			holdEventPast = false;
			longHoldEventPast = false;
			//if ((millis() - upTime) < DCgap && DConUp == false && DCwaiting == true)  DConUp = true;
			//else  DConUp = false;
			DCwaiting = false;

			isDownPushed = true;

			logger.Log("Down ");
			logger.LogLine(buttonPin);
		}
	}
	// Button released
	else if (buttonVal == LOW && buttonLast == HIGH && (millis() - downTime) > debounce)
	{
		isDownPushed = false;
		if (!ignoreUp && !isUpPushed)
		{
			logger.Log("Up ");
			logger.LogLine(buttonPin);

			upTime = millis();
			ignoreUp = true;
			isUpPushed = true;
			//if (DConUp == true) DCwaiting = true;
			//else
			{
				event = CLICK;
				DConUp = false;
				DCwaiting = false;
				singleOK = false;
			}
		}
	}
	// Test for normal click event: DCgap expired
	//if (buttonVal == LOW && (millis() - upTime) >= DCgap && DCwaiting == true && DConUp == false && singleOK == true && event != 2)
	//{
	//	event = DOUBLE_CLICK;
	//	DCwaiting = false;
	//}
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
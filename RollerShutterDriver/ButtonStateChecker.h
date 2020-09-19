// ButtonStateChecker.h

#ifndef _BUTTONSTATECHECKER_h
#define _BUTTONSTATECHECKER_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "arduino.h"
#else
	#include "WProgram.h"
#endif

#include "Logger.h"

#define NOTHING 0
#define DOUBLE_CLICK 1
#define CLICK 2
#define LONG_HOLD 3
#define VERY_LONG_HOLD 4

class ButtonStateChecker
{
private:
	int buttonPin;

	// Button timing variables
	int debounce = 10;          // ms debounce period to prevent flickering when pressing or releasing the button
	int DCgap = -1;            // max ms between clicks for a double click event
	int holdTime = 5000;        // ms hold period: how long to wait for press+hold event
	int longHoldTime = -1;    // ms long hold period: how long to wait for press+hold event

	// Button variables
	bool buttonLast = LOW;  // buffered value of the button's previous state
	bool DCwaiting = false;  // whether we're waiting for a double click (down)
	bool DConUp = false;     // whether to register a double click on next release, or whether to wait and click
	bool singleOK = true;    // whether it's OK to do a single click
	long downTime = -1;         // time the button was pressed down
	long upTime = -1;           // time the button was released
	bool ignoreUp = false;   // whether to ignore the button release because the click+hold was triggered
	bool waitForUp = false;        // when held, whether to wait for the up event
	bool holdEventPast = false;    // whether or not the hold event happened already
	bool longHoldEventPast = false;// whether or not the long hold event happened already

	bool isDownPushed = false;
	bool isUpPushed = false;
	Logger logger;
public:
	ButtonStateChecker(int buttonPin, Logger &logger);
	int CheckButton();
	void ResetState();
};

#endif


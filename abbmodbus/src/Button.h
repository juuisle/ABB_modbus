/*
 * Button.h
 *
 *  Created on: 6 Mar 2018
 *      Author: Ville
 */

#ifndef BUTTON_H_
#define BUTTON_H_

#include "DigitalIoPin.h"

class Button {
private:
	DigitalIoPin* pin;
	bool down;
	uint32_t lastTicks;
public:
	Button(DigitalIoPin* pin) :
		pin(pin),
		down(false),
		lastTicks(0)
	{}

	virtual ~Button() {}

	/* Is the button currently pressed. */
	bool get()
	{
		return pin->read();
	}

	/* Is the button currently pressed. Return true each rate ticks */
	bool getRepeat(uint32_t ticks, uint32_t rate = 10)
	{
		if (get() && ticks - lastTicks >= rate)
		{
			lastTicks = ticks;
			return true;
		}
		return false;
	}

	/* Has the button been pressed down since the last call to getDown/Up() */
	bool getDown()
	{
		bool d = down;
		down = get();
		return !d && down;
	}

	/* Has the button been released since the last call to getDown/Up() */
	bool getUp()
	{
		bool d = down;
		down = get();
		return d && !down;
	}
};

#endif /* BUTTON_H_ */

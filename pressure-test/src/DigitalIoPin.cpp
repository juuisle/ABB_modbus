/*
 * DigitalIoPin.cpp
 *
 *  Created on: 22 Jan 2018
 *      Author: Ville
 */

#include "DigitalIoPin.h"
#include "board.h"

DigitalIoPin::DigitalIoPin(int port, int pin, bool input, bool pullup, bool invert) :
		port(port), pin(pin), invert(invert && !input)
{
	uint32_t mod = IOCON_MODE_INACT | IOCON_DIGMODE_EN;

	if (input)
	{
		if (pullup)
			mod |= IOCON_MODE_PULLUP;
		if (invert)
			mod |= IOCON_INV_EN;
	}

	Chip_IOCON_PinMuxSet(LPC_IOCON, port, pin, mod);

	if (input)
	{
		Chip_GPIO_SetPinDIRInput(LPC_GPIO, port, pin);
	}
	else
	{
		Chip_GPIO_SetPinDIROutput(LPC_GPIO, port, pin);
		write(false);
	}
}

DigitalIoPin::~DigitalIoPin()
{
}

bool DigitalIoPin::read()
{
	bool value = Chip_GPIO_GetPinState(LPC_GPIO, port, pin);
	if (invert)
		value = !value;
	return value;
}

void DigitalIoPin::write(bool value)
{
	if (invert)
		value = !value;
	Chip_GPIO_SetPinState(LPC_GPIO, port, pin, value);
}


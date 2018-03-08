/*
 * DigitalIoPin.h
 *
 *  Created on: 22 Jan 2018
 *      Author: Ville
 */

#ifndef DIGITALIOPIN_H_
#define DIGITALIOPIN_H_

#include <stdint.h>

class DigitalIoPin {
public:
	DigitalIoPin(int port, int pin, bool input = true, bool pullup = true, bool invert = false);
	virtual ~DigitalIoPin();
	bool read();
	void write(bool value);
private:
	int port, pin;
	bool invert;
};

#endif /* DIGITALIOPIN_H_ */

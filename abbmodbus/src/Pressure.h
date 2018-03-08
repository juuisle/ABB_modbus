#ifndef PRESSURE_H_
#define PRESSURE_H_

#include "I2C.h"

class Pressure {
public:
	Pressure();
	bool getPressureDiff(int16_t &out);
private:
	I2C i2c;
};

#endif

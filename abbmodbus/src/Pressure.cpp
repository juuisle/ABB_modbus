#include "Pressure.h"

Pressure::Pressure() : i2c(0, 10000) {
//	i2c = I2C(0, 100000);
}

bool Pressure::getPressureDiff(int16_t &out) {
	uint8_t pressureData[3];
	uint8_t readPressureCmd = 0xF1;

	if (i2c.transaction(0x40, &readPressureCmd, 1, pressureData, 3)) {
		out = ((pressureData[0] << 8) | pressureData[1]);
		out = out / 240;
		return 1;
	}
	else {
		return 0;
	}
}



#include "Fan.h"

void Sleep(int ms);

Fan::Fan() : node(2) {
	node.begin(9600); // set transmission rate - other parameters are set inside the object and can't be changed here
	node.writeSingleRegister(0, 0x0406); // prepare for starting
	node.writeSingleRegister(0, 0x047F); // set drive to start mode
}

bool Fan::setFrequency(uint16_t freq) {
	uint8_t result;
	int ctr;
	bool atSetpoint;

	node.writeSingleRegister(1, freq); // set motor frequency

	// wait until we reach set point or timeout occurs
	ctr = 0;
	atSetpoint = false;
	do {
		// read status word
		result = node.readHoldingRegisters(3, 1);
		// check if we are at setpoint
		if (result == node.ku8MBSuccess) {
			if (node.getResponseBuffer(0) & 0x0100)
				atSetpoint = true;
		}
		ctr++;
	} while (ctr < 20 && !atSetpoint);

	return atSetpoint;
}

bool Fan::getFrequency(uint16_t &out) {
	uint8_t result;

	// slave: read (2) 16-bit registers starting at register 102 to RX buffer
	int j = 0;
	do {
		result = node.readHoldingRegisters(102, 2);
		j++;
	} while (j < 3 && result != node.ku8MBSuccess);
	// note: sometimes we don't succeed on first read so we try up to threee times
	// if read is successful print frequency and current (scaled values)
	if (result == node.ku8MBSuccess) {

		out = node.getResponseBuffer(0);
		return 1;
	} else {
		return 0;
	}
}

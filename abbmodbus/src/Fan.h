#ifndef FAN_H_
#define FAN_H_

#include "ModbusMaster.h"

class Fan {
public:
	Fan();
	~Fan();
	bool setFrequency(uint16_t freq);
	bool getFrequency(uint16_t &out);
private:
	ModbusMaster node;

};
#endif

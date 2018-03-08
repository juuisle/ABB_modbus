/*
 * BarGraph.h
 *
 *  Created on: 1.2.2016
 *      Author: krl
 */

#ifndef BARGRAPH_H_
#define BARGRAPH_H_

#include "LiquidCrystal.h"

class BarGraph {
public:
	BarGraph(LiquidCrystal& lcd_, int length_ = 5, bool vertical_ = false);
	virtual ~BarGraph();
	void draw(int value);

protected:
	LiquidCrystal& lcd;

private:
	const static uint8_t horizontalMap[8][8];
	const static uint8_t verticalMap[8][8];
	int length;
	bool vertical;
	int size;
};

#endif /* BARGRAPH_H_ */

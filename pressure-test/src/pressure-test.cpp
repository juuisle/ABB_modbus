/*
===============================================================================
 Name        : main.c
 Author      : $(author)
 Version     :
 Copyright   : $(copyright)
 Description : main definition
===============================================================================
*/


#include "board.h"

#include <cstdio>
#include "DigitalIoPin.h"
#include "I2C.h"

/*****************************************************************************
 * Private types/enumerations/variables
 ****************************************************************************/

#define TICKRATE_HZ (1000)	/* 1000 ticks per second */
static volatile uint32_t ticks;
static volatile uint32_t counter;

extern "C" {

void SysTick_Handler(void)
{
	ticks++;

	if(counter > 0) counter--;
}

} // extern "C"

void Sleep(uint32_t time)
{
	counter = time;
	while(counter > 0) {
		__WFI();
	}
}

void i2cTest() {
	I2C i2c(0, 100000);

	while(1) {
		uint8_t pressureData[3];
		uint8_t readPressureCmd = 0xF1;
		int16_t pressure = 0;

		if (i2c.transaction(0x40, &readPressureCmd, 1, pressureData, 3)) {
			pressure = (pressureData[0] << 8) | pressureData[1];
			printf("Pressure data: ");
			printf("%d, pascal: %d\r\n", pressure, pressure/240);
		}
		else {
			printf("Error reading pressure.\r\n");
		}
		Sleep(1000);
	}
}

int main(void)
{
	SystemCoreClockUpdate();
	Board_Init();

	Board_LED_Set(0, false);

    /* Set up SWO to PIO1_2 */
	Chip_SWM_MovablePortPinAssign(SWM_SWO_O, 1, 2); // Needed for SWO printf

	/* Enable SysTick Timer */
	SysTick_Config(SystemCoreClock / TICKRATE_HZ);

	i2cTest();

	/* Code never reaches here. Only used to satisfy standard main() */
	return 0;
}

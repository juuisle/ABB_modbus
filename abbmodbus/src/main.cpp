/*
===============================================================================
 Name        : main.c
 Author      : $(author)
 Version     :
 Copyright   : $(copyright)
 Description : main definition
===============================================================================
*/

#if defined (__USE_LPCOPEN)
#if defined(NO_BOARD_LIB)
#include "chip.h"
#else
#include "board.h"
#endif
#endif

#include <cr_section_macros.h>

// TODO: insert other include files here
#include <cstring>
#include <cstdio>

#include "Pressure.h"
#include "Fan.h"
#include "DigitalIoPin.h"
#include "LiquidCrystal.h"
#include "Button.h"

static volatile int counter;
static volatile uint32_t systicks;

#ifdef __cplusplus
extern "C" {
#endif
/**
 * @brief	Handle interrupt from SysTick timer
 * @return	Nothing
 */
void SysTick_Handler(void)
{
	systicks++;
	if(counter > 0) counter--;
}
#ifdef __cplusplus
}
#endif

void Sleep(int ms)
{
	counter = ms;
	while(counter > 0) {
		__WFI();
	}
}

/* this function is required by the modbus library */
uint32_t millis() {
	return systicks;
}

template<typename T>
inline T clamp(T a, const T &min, const T &max)
{
	if (a < min)
		a = min;
	if (a > max)
		a = max;
	return a;
}

int main(void) {

#if defined (__USE_LPCOPEN)
    // Read clock settings and update SystemCoreClock variable
    SystemCoreClockUpdate();
#if !defined(NO_BOARD_LIB)
    // Set up and initialize all required blocks and
    // functions related to the board hardware
    Board_Init();
    // Set the LED to the state of "On"
    Board_LED_Set(0, true);
#endif
#endif

    /* Set up SWO to PIO1_2 */
	Chip_SWM_MovablePortPinAssign(SWM_SWO_O, 1, 2); // Needed for SWO printf

	/* Enable and setup SysTick Timer at a periodic rate */
	SysTick_Config(SystemCoreClock / 1000);

	/* Buttons */
	DigitalIoPin sw0(1, 3, true, true, true);
	DigitalIoPin sw1(0, 9, true, true, true);
	DigitalIoPin sw2(0, 10, true, true, true);
	Button b0(&sw0);
	Button b1(&sw1);
	Button b2(&sw2);

	/* LCD */
	Chip_RIT_Init(LPC_RITIMER); // initialize RIT (enable clocking etc.)

	DigitalIoPin a0(0, 8, false, false, false);
	DigitalIoPin a1(1, 6, false, false, false);
	DigitalIoPin a2(1, 8, false, false, false);
	DigitalIoPin a3(0, 5, false, false, false);
	DigitalIoPin a4(0, 6, false, false, false);
	DigitalIoPin a5(0, 7, false, false, false);

	LiquidCrystal lcd(&a0, &a1, &a2, &a3, &a4, &a5);
	// configure display geometry
	lcd.begin(16, 2);

	const int MAX_FREQ = 20000;
	const int MAX_PRESSURE = 120;
    const int EPSILON = 1;
    const int MANUAL = true;
    const int AUTO = false;

	Fan fan;
	Pressure sensor;

	uint16_t freq = 0;
	int16_t pressure;
	int16_t targetPressure = 0;
    char buffer[64];
    int mode = MANUAL;
    uint32_t lastTick = 0;
    uint32_t lastInputTick = 0;
    int16_t diff = 0;
    int newFreq;

	while(1) {
		uint32_t tick = systicks;

		if(b0.getDown())
		{
			mode = !mode;
		}

		switch(mode)
		{
		case MANUAL:

			newFreq = freq;
			if (b1.getRepeat(tick, 10))
				newFreq -= (MAX_FREQ/100);
			if (b2.getRepeat(tick, 10))
				newFreq += (MAX_FREQ/100);
			freq = (uint16_t)clamp(newFreq, 0, MAX_FREQ);

			if (!fan.setFrequency(freq))
			{
				printf("failed to set frequency: %d", freq);
			}

			sensor.getPressureDiff(pressure);
			fan.getFrequency(freq);

			sprintf(buffer,
					"Speed:    %3d%% M\nPressure: %3dPa \n", freq/(MAX_FREQ/100), pressure);

			lcd.print(buffer);

			printf("Manual: Frequency=%d, Pressure=%d\n", freq, pressure);
			break;

		case AUTO:

			if(b1.get() || b2.get()) // set target pressure
			{
				if (targetPressure > 0 && b1.getRepeat(tick, 100)) {
					--targetPressure;
				}
				if (targetPressure < MAX_PRESSURE && b2.getRepeat(tick, 100)) {
					++targetPressure;
				}

				sprintf(buffer, "Set: %3dPa     A\n                ", targetPressure);

				lastInputTick = tick;
			}

			// adjust frequency iteratively to reach target pressure
			{
				fan.getFrequency(freq);
				sensor.getPressureDiff(pressure);

				if (tick >= lastTick + abs(diff) * 80) // give time for pressure reading to update
				{
					lastTick = tick;

					diff = targetPressure - pressure;
					if (diff < -EPSILON || diff > EPSILON)
					{
						newFreq = freq + diff * 40;
						freq = (uint16_t)clamp(newFreq, 0, MAX_FREQ);
					}

					// NOTE: each time frequency is set to a value smaller than 4000,
					// the fan will do a burst and it will take about 3 to 5 seconds to reach the correct speed
					if (freq < 4000)
					{
						diff = 60; // this will make the wait longer
						if (targetPressure < EPSILON)
							freq = 0;
						else if (freq < 1200)
							freq = 1200; // below this the fan will not run
					}
				}

				if (!fan.setFrequency(freq))
				{
					printf("failed to set frequency: %d", freq);
				}

				if (tick > lastInputTick + 1000) // timeout the set screen
				{
					sprintf(buffer,
							"Speed:    %3d%% A\nPressure: %3dPa \n", freq/(MAX_FREQ/100), pressure);
				}
			}

			lcd.print(buffer);

			printf("Auto: Frequency=%d, Pressure=%d\n", freq, pressure);
			break;
		}

	}

    // Force the counter to be placed into memory
    volatile static int i = 0 ;
    // Enter an infinite loop, just incrementing a counter
    while(1) {
        i++ ;
    }
    return 0 ;
}

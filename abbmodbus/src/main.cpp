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

	/* GPIO PIN SETTING */
	DigitalIoPin sw0(0, 29, true, true, true);
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

	Pressure sensor;
	Fan fan;
	uint16_t freq = 2000;
	int16_t pressure;
    char buffer[64];

	while(1) {
		if (freq < 20000 && b2.getRepeat(systicks, 10)) {
			freq += 100;
		}
		if (freq > 0 && b1.getRepeat(systicks, 10)) {
			freq -= 100;
		}

		fan.setFrequency(freq);

		sensor.getPressureDiff(pressure);
		fan.getFrequency(freq);

		sprintf(buffer, "Pressure: %dPa \nFreq: %d%%   \n", pressure, freq/200);
		lcd.print(buffer);
		printf(buffer);
	}

    // Force the counter to be placed into memory
    volatile static int i = 0 ;
    // Enter an infinite loop, just incrementing a counter
    while(1) {
        i++ ;
    }
    return 0 ;
}

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
#include "DigitalIoPin.h"
#include "LiquidCrystal.h"
#include "BarGraph.h"

// TODO: insert other definitions and declarations here
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


// returns the interrupt enable state before entering critical section
bool enter_critical(void)
{
	uint32_t pm = __get_PRIMASK();
	__disable_irq();
	return (pm & 1) == 0;
}

// restore interrupt enable state
void leave_critical(bool enable)
{
	if(enable) __enable_irq();
}

// Example:
// bool irq = enter_critical();
// Change variables that are shared with an ISR
// leave_critical(irq);



void Sleep(uint32_t time)
{
	counter = time;
	while(counter > 0) {
		__WFI();
	}
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

	/* Configure systick timer */
	SysTick_Config(Chip_Clock_GetSysTickClockRate() / TICKRATE_HZ);

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
	// set the cursor to column 0, line 1
	// (note: line 1 is the second row, since counting begins with 0):
	lcd.setCursor(0, 1);
	// Print a message to the LCD.
	//lcd.write('A');
	lcd.print("ABCDEFGH01234567\n,.-?");

	BarGraph bar(lcd, 50);

	/* Buttons */
	DigitalIoPin sw0(0, 29, true, true, true);
	DigitalIoPin sw1(0, 9, true, true, true);
	DigitalIoPin sw2(0, 10, true, true, true);

	uint32_t val0 = 500;
	char str2[80];

	while(1) {

		if (sw0.read())
		{
			// decrement
			if (val0 > 0)
				--val0;
		}
		if (sw1.read())
		{
			// reset
			val0 = 500;
		}
		if (sw2.read())
		{
			// increment
			if (val0 < 1000)
				++val0;
		}

		lcd.setCursor(0, 0);
		sprintf(str2, "%4lu  ", val0);
		lcd.printRow(str2);
//		lcd.setCursor(10, 0);
		bar.draw(val0 * 50 / 1000);

		Sleep(10);
	}

    return 0 ;
}

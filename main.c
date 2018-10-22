/*------------------------------------------------------------------------------/
* File Name    : main.c
*
* Project      : Active RFID Wireless Contact Lens Transceiver
* Organization : California Polytechnic State University
* Hardware     : Atmel ATmega328P
* Description  : This firmware interfaces the ATMega328 to the
*                Sparkfun IOIO V.2 breakout board and detects
*                the frequency of an external clock signal.
*                Frequency detected is transmitted over SPI to
*                the IOIO subsystem.
*
* Hardware     : 1. ATMEL ATmega328P Microcontroller
*                2. Sparkfun IOIO V.2 Breakout Board
*
* Created      : 4/20/2018
* Engineers    : Mark Manuel
*-------------------------------------------------------------------------------*/

#define F_CPU 16000000UL
#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include <stdlib.h>                 // Standard C library
#include <string.h>
#include <math.h>
#include "UART.h"

//function prototype
void initTimer0(void);
void initTimer1(void);
void initTimer2(void);

// global variable declaration
volatile int fs_timer_status = 0;   // Sampling Frequency Status
volatile int FC_L = 0;              // Lower Frequency Counter Value
volatile int FC_H = 0;              // Higher Frequency Counter Value
volatile int FC = 0;                // 16 Bit Frequency Counter Value
volatile int samp_dur_status = 0;   // Sampling Duration Status

int main(void)
{
	double frequency = 0;
	static char freq_print[16];
	initTimer0();
	initTimer1();
	initTimer2();
	usart_init(38400, F_CPU);       //Initialize UART bus at 38.4kbaud
	

	DDRC |= 1<<PC1;                 // Pin B5 output
	DDRC |= 1<<PC0;                 // Pin B0 outputt 
	sei();                          // Enable global interrupts

	while(1)
	{
		/*---------------------- Stop ALL Timers/Counters  ----------------------*/
		TCCR0B = 0x0;    // Stop Sampling Rate Timer
		TCCR1B = 0x0;    // Stop Frequency Counter
		TCCR2B = 0x0;    // Stop Sampling Duration Timer
		/*-----------------------------------------------------------------------*/

		/*--------------------- Reset ALL Timers/Counters -----------------------*/
		TIFR0 = 0x2;     // Reset Sampling Rate Timer
		TIFR1 = 0x2;     // Reset Frequency Counter
		TIFR2 = 0x2;     // Reset Sampling Duration Timer
		/*-----------------------------------------------------------------------*/
		_delay_us(10);
		/*--------------------------- Begin Measurement -------------------------*/

		TCCR0B = 0x03;          // Start Sampling Rate Timer: SysCLK/64
		PORTC |= 1<<PC1;        // Turn on C1: total sampling begins
		TCCR1B = 0x07;          // Start Freq. cnt: External CLK on T0 pin, RET
		TCCR2B = 0x04;          // Start Sampling Duration Timer: SysCLK/64
		PORTC |= 1<<PC0;        // Turn on C0: sampling period begins

		while(!fs_timer_status)        // Wait until 1000uS passes
		{
			while(!samp_dur_status)    // Wait until 500uS passes
			{
			}
			TCCR1B = 0x00;             // Stop frequency counter
			TCCR2B = 0x00;             // Stop measurement frame timer
		}
		TCCR0B = 0;                    // Stop sampling rate timer
		PORTC &= ~(1 << PC1);          // Turn off C1 after sample is taken
		samp_dur_status = 0;           // Unset sampling duration status
		fs_timer_status = 0;           // Unset sampling period status
		
		frequency = (double)FC * 8 / 499;      // Counts * Prescaler / time(uS)
		dtostrf(frequency, 7, 6, freq_print);  // Converts freq into ASCII str.
		_delay_ms(1);
		serial_print_freq(freq_print, FC_L, FC_H); // Print Data to Terminal
	}
	/*---------------------------- End Measurement ------------------------------*/

	return 0;

} // end main

void initTimer0(void)
{
	TCCR0A = 0x02;  // timer overflow mode
	TCCR0B = 0x03;  // timer clk = system clk / 64
	OCR0A  = 249;   // Overflow every 1000 Hz
	TIFR0 = 0x02;   // clear previous timer overflow
	TIMSK0 = 0x02;  // timer overflow interrupt enabled
}
void initTimer1(void)
{
	TCCR1A = 0x00;  // timer overflow mode
	TCCR1B = 0x07;  // Start Freq. cnt: External CLK on T0 pin, RET
	OCR1A  = 0;     // Overflow every 1000 Hz
	TIFR1 = 0x02;   // clear previous timer overflow
	TIMSK1 = 0x00;  // timer overflow interrupt enabled
}
void initTimer2(void)
{
	TCCR2A = 0x02;  // timer overflow mode
	TCCR2B = 0x08;  // timer clk = system clk / 64
	OCR2A  = 124;   // Stop recording after 500us
	TIFR2 = 0x02;   // clear previous timer overflow
	TIMSK2 = 0x02;  // timer overflow interrupt enabled
}

ISR(TIMER0_COMPA_vect)
{
	fs_timer_status = 1;    // Kick out of total sample
}

ISR(TIMER2_COMPA_vect)
{
	samp_dur_status = 1;    // Kick out of sampling period
	FC_H = TCNT1L;
	FC_L = TCNT1H;
	FC = TCNT1;             // Store counts into global var
	TCNT1 = 0x00;
	PORTB &= ~(1 << PB0);   // Turn off B0 when sampling duration closes
}

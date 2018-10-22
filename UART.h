/*------------------------------------------------------------------------------/
* File Name    : UART.h
*
* Project      : Active RFID Wireless Contact Lens Transceiver
* Organization : California Polytechnic State University
* Hardware     : Atmel ATmega328P
* Description  : USART function header for printing to serial terminal
*
* Hardware     : 1. ATMEL ATmega328P Microcontroller
*
* Created      : 4/20/2018
* Engineers    : Mark Manuel
*-------------------------------------------------------------------------------*/

#define F_CPU 16000000UL
#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include <util/twi.h>
#include <stdlib.h>					// Standard C library
#include <string.h>
#include <math.h>

#define BAUD_PRESCALE 103

void usart_init(uint32_t baudin, uint32_t clk_speedin);
void usart_send( uint8_t data );
uint8_t  usart_recv(void);
uint8_t  usart_istheredata(void);
void serial_print(char serial_phrase[]);
void serial_print_num(uint8_t data);
void serial_print_nl(char serial_phrase[]);
void serial_print_freq(char freq_print[], uint8_t FC_L, uint8_t FC_H);




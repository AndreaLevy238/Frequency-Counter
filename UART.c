/*------------------------------------------------------------------------------/
* File Name    : UART.c
*
* Project      : Active RFID Wireless Contact Lens Transceiver
* Organization : California Polytechnic State University
* Hardware     : Atmel ATmega328P
* Description  : USART function lib for printing to serial terminal
*
* Hardware     : 1. ATMEL ATmega328P Microcontroller
*
* Created      : 4/20/2018
* Engineers    : Mark Manuel
*-------------------------------------------------------------------------------*/

#include "UART.h"


void usart_init(uint32_t baudin, uint32_t clk_speedin)
{
	uint32_t ubrr = (clk_speedin/16UL)/baudin-1;
	UBRR0H = (unsigned char)(ubrr>>8);
	UBRR0L = (unsigned char)ubrr;
	/* Enable receiver and transmitter */
	UCSR0B = (1<<RXEN0)|(1<<TXEN0);
	/* Set frame format: 8data, 1stop bit */
	UCSR0C = (1<<USBS0)|(3<<UCSZ00);
	UCSR0A &= ~(1<<U2X0);
}

/*the send function will put 8bits on the trans line. */
void usart_send( uint8_t data )
{
	/* Wait for empty transmit buffer */
	while ( !( UCSR0A & (1<<UDRE0)) );
	/* Put data into buffer, sends the data */
	UDR0 = data;
}

/* the receive data function. Note that this a blocking call
Therefore you may not get control back after this is called 
until a much later time. It may be helpful to use the 
istheredata() function to check before calling this function
	@return 8bit data packet from sender
*/
uint8_t  usart_recv(void)
{
	/* Wait for data to be received */
	while ( !(UCSR0A & (1<<RXC0)) )
	;
	/* Get and return received data from buffer */
	return UDR0;
}

/* function check to see if there is data to be received
	@return true is there is data ready to be read */
uint8_t  usart_istheredata(void)
{
	 return (UCSR0A & (1<<RXC0));
}

void serial_print(uint8_t low, uint8_t high) {
	usart_send(low);
	usart_send(high);
}
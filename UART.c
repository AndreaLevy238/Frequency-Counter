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
until a much later time. It may be helpfull to use the 
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
void serial_print(char serial_phrase[])
{
	int char_cnt = strlen(serial_phrase);
	int i;
	for(i=0; i<char_cnt+1; i++)
	{
		usart_send(serial_phrase[i]);
	}
}
void serial_print_num(uint8_t data)
{
	uint8_t temp0 = 0;
	int temp1 = 0;
	int j=0, k=0;
	//           Begin with:  76543210
	temp0 = data & ~(0xF0); //00003210
	temp1 = data;           //76540000
	temp1 = temp1 >> 4; 	  //00007654
	
	temp0 = (temp0 + 48);   // Convert to ASCII
	temp1 = (temp1 + 48);   // Convert to ASCII
	
	char i[6] = "ABCDEF";   // Decimal to Hex Conversion on lower nibble
	for(j=0; j<7; j++){
		if(temp0 == 58 + j){
			temp0 = i[j];
		}
	}
	
	for(k=0; k<7; k++){ 	// Decimal to Hex Conversion on upper nibble
		if(temp1 == 58 + k){
			temp1 = i[k];
		}
	}
	usart_send(temp1);  	// Send upper character to serial terminal
	usart_send(temp0);  	// Send lower character to serial terminal
	// Result: (upper nibble) (lower nibble) - Hex
}
void serial_print_nl(char serial_phrase[])
{
	int char_cnt = strlen(serial_phrase); // Count number of letters in string
	int i;
	for(i=0; i<char_cnt+1; i++)
	{
		usart_send(serial_phrase[i]);
	}
	usart_send('\n'); // New line
	usart_send('\r'); // Carriage Return
}

void serial_print_freq(char freq_str[], uint8_t FC_L, uint8_t FC_H)
{
		serial_print("Freq. Measured: ");
		serial_print(freq_str);
		serial_print_nl("MHz");
		
		serial_print("TCNT1: ");
		serial_print_num(FC_L);
		serial_print_num(FC_H);  // Send Lower bit of freq cnt
		usart_send('\n'); // New line
		usart_send('\r'); // Carriage Return

		_delay_ms(50);
		usart_send(12);
}
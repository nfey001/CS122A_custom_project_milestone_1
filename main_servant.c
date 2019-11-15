/*
Name & Email: Nathaniel Fey, nfey001@ucr.edu
Partner & Email: NONE
Lab Section: 021
Assignment: Lab 2 USART, part 1 follower code

I acknowledge all content contained herein, excluding template or example code, is my own original work.
*/

#include <avr/io.h>
#include "Timer.h"
#include "usart_ATMega1284.h"


// Follower code

int main(void)
{
	//follow_state = WAIT;
	DDRA = 0xFF; PORTA = 0x00; //
	TimerSet(125);
	TimerOn();
	initUSART(1); // transmit
	
	unsigned char temp1;
	while (1)
	{
		if(USART_HasReceived(1))//check if it has been received from master
		{
			
			temp1 = USART_Receive(1);
			
			PORTA = 0x00;
			if (temp1 == 0xFF)
			{
				
				PORTA = 0x01;
				//PORTB = 0x01;
				
			}
			
			
		}
		while(!TimerFlag)
		;
		TimerFlag = 0;
	}
}



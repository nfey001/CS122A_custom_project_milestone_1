/*
Name & Email: Nathaniel Fey, nfey001@ucr.edu
Lab Section: 021
Assignment: Custom Lab Project

I acknowledge all content contained herein, excluding template or example code, is my own original work.
*/
#ifndef JOYSTICK_H
#define JOYSTICK_H
#include <avr/interrupt.h>
#include <avr/io.h>
#include <stdio.h>
static unsigned char position = 0;
static unsigned char dispPos = 0;
unsigned short tmpA = 0x0000;
#define REF_AVCC (1<<REFS0);
/*------------------------------ JOYSTICK FUNCTIONALITY------------------------*/
void ADC_init()
{
	ADCSRA |= (1 <<ADEN) | (1 << ADSC) | (1 << ADATE);
}


unsigned char getPosition()
{

	if (ADMUX  == 1)
	{
		ADMUX = 0; // switch between ADC1->ADC0 
	}
	else if (ADMUX == 0)
	{
		ADMUX = 1; // switch between ADC0->ADC1
	}
	
	
	tmpA = ADC; // the value ranges between 0 to 1024 (2^10)
	if (tmpA < 100 && ADMUX == 1) // left
	{
		position = 1;
		
	}
	else if (tmpA > 900 && ADMUX == 1) // right
	{
		position = 2;
		
	}
	else if (tmpA > 900 && ADMUX == 0)
	{
		position = 0; // do not set position flag
	}
	else if (tmpA < 100 && ADMUX == 0)
	{
		position = 0; // do not set position flag
	}
	return position;
}

#endif
/*
 * CS122A_custom_project_keypad.c
 *
 * Created: 11/3/2019 2:10:23 PM
 * Author : Nathaniel
 */ 

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <stdio.h>
#include <string.h>
#include "Bit.h"
#include "Keypad.h"
#include "Timer.h"
#include "LCD.h"
#include "Joystick.h"
#include "usart_ATMega1284.h"

#define BUTTON (~PINA & 0x04)

uint8_t userPW[3]; // variable to hold user password
// Shared variables
static unsigned char KeypadInput;

unsigned char password = '3'; // default password
uint8_t defPW[3];
unsigned char passwordFlag = 0; // turns to 1 when password is guessed correctly
unsigned char passwordInput;
unsigned char incorrectFlag = 0; // turns to 1 when incorrect pw
unsigned char LEDFlag = 0;

unsigned char pw1_flag = 0;
unsigned char pw2_flag = 0;
unsigned char pw3_flag = 0;

unsigned char first_pw = 1;
unsigned char second_pw = 2;
unsigned char third_pw = 3;

unsigned char temp = 0x00;
// Function prototyping
void selectionTick();
void setPW();
void getPW();
void combineTick();
void buttonTick();

// SPI Functions
void SPI_MasterInit();
void master_tick();
void SPI_MasterTransmit(unsigned char cData);

//--------User defined FSMs---------------------------------------------------
//Enumeration of states.
enum KeypadInputState { Keypad_Start, Keypad_wait, Keypad_key } key_state = Keypad_Start;
enum passwordState {Password_Start, Password_insert_1, Password_insert_2, Password_insert_3,Password_enter, Password_check, Password_loop} password_state = Password_Start;
enum displayPW {Display_Start, DisplayPW} display_state = Display_Start;
enum selectState {SELECT_START, SELECT_LED, SELECT_SERVO} select_state = SELECT_START;
enum combineState {COMBINE_START, COMBINE_TICKS} combine_state = COMBINE_START;
enum ButtonState {BUTTON_WAIT, BUTTON_PRESSED, BUTTON_HELD, BUTTON_RELEASED} button_state;
enum master_state {WAIT, SEND} m_state;

void servo_PWM_off() // turn off servo motor
{
	TCCR1A = 0x00;
	
}
	
void KeypadTick() {

	
	//State machine transitions
	switch (key_state) {
		case Keypad_Start:
		key_state = Keypad_wait;
		break;

		case Keypad_wait:
		if (KeypadInput == '\0')
		{
			key_state = Keypad_wait;
		}
		else
		key_state = Keypad_key;
		break;
		
		case Keypad_key:
		
		key_state = Keypad_wait;
		
		break;

		
		default:		key_state = Keypad_Start; // default: Initial state
		break;
	}

	//State machine actions
	switch(key_state) {
		case Keypad_Start:
		break;
		
		case Keypad_wait:
		KeypadInput = GetKeypadKey();
		break;
		
		case Keypad_key:
		KeypadInput = GetKeypadKey();
		LCD_init();
		switch(KeypadInput)
		{
			case '0':
			//LCD_init();
			LCD_ClearScreen();
			LCD_DisplayString(1,"0");
			
			break;
			
			case '1':
			//	LCD_init();
			LCD_ClearScreen();
			LCD_DisplayString(1, "1");
			break;
			
			case '2':
			//LCD_init();
			LCD_ClearScreen();
			LCD_DisplayString(1, "2");
			break;
			
			case '3':
			//LCD_init();
			LCD_ClearScreen();
			LCD_DisplayString(1, "3");
			break;
			
			case '4':
			//LCD_init();
			LCD_ClearScreen();
			LCD_DisplayString(1, "4");
			break;
			
			case '5':
			//LCD_init();
			LCD_ClearScreen();
			LCD_DisplayString(1, "5");
			break;
			
			case '6':
			//LCD_init();
			LCD_ClearScreen();
			LCD_DisplayString(1, "6");
			break;
			
			case '7':
			//LCD_init();
			LCD_ClearScreen();
			LCD_DisplayString(1, "7");
			break;
			
			case '8':
			//LCD_init();
			LCD_ClearScreen();
			LCD_DisplayString(1, "8");
			break;
			
			case '9':
			//LCD_init();
			LCD_ClearScreen();
			LCD_DisplayString(1, "9");
			break;
			
			case 'A':
			//LCD_init();
			LCD_ClearScreen();
		//	LCD_DisplayString(1, "A");
			break;
			
			case 'B':
			//LCD_init();
			LCD_ClearScreen();
		//	LCD_DisplayString(1, "B");
			break;
			
			case 'C':
			//LCD_init();
			LCD_ClearScreen();
		//	LCD_DisplayString(1, "C");
			break;
			
			case 'D':
			//LCD_init();
			LCD_ClearScreen();
		//	LCD_DisplayString(1, "D");
			break;
			
			case '#':
			//LCD_init();
			LCD_ClearScreen();
		//	LCD_DisplayString(1, "#");
			break;
			
			case '*':
			//LCD_init();
			LCD_ClearScreen();
		//	LCD_DisplayString(1, "*");
			break;
		}
		break;
		
		default:		break;
	}

	//return state;
}

void passwordTick()
{
	switch(password_state) // transitions
	{
		case Password_Start:
			pw1_flag = 0;
			pw2_flag = 0;
			pw3_flag = 0;
			
			password_state = Password_insert_1;
			break;
		case Password_insert_1:
		if (pw1_flag == 1 )
			{
				password_state = Password_insert_2;
			}
			break;
		case Password_insert_2:
			if (pw2_flag == 1)
			{
				password_state = Password_insert_3;
			}
			break;
		case Password_insert_3:
			if (pw3_flag == 1)
			{
				password_state = Password_enter;
			}
			break;
		case Password_enter:
			KeypadInput = GetKeypadKey();
			if (KeypadInput == '*')
			{
				password_state = Password_check;
			}
			else 
				password_state = Password_enter;
			break;
			case Password_check:
				if (incorrectFlag == 1)
				{
					password_state = Password_Start;
				}
				else
					password_state = Password_loop;
				break;
			case Password_loop:
				password_state = Password_loop;
				break;
	}
	
	switch(password_state) // actions
	{
		case Password_Start:
		
			break;
		case Password_insert_1:
			delay_ms(500);
			KeypadInput = GetKeypadKey();
			if (KeypadInput == '\0')
			{
				if (incorrectFlag == 1)
				{
					LCD_DisplayString(1,"Incorrect PW");
				}
				else
				LCD_DisplayString(1,"Enter guess");
				
			}
			if (KeypadInput != '\0')
			{
				userPW[0] = KeypadInput;
				pw1_flag = 1;
				
			}
		
		/*	if (KeypadInput != first_pw && (KeypadInput != '\0'))
			{
				LCD_DisplayString(1,"Incorrect PW");
			}
			*/
			break;
		case Password_insert_2:
		delay_ms(500);
			incorrectFlag = 0;
			KeypadInput = GetKeypadKey();
			if (KeypadInput == '\0')
			{
				LCD_DisplayString(1,"*");
			}
			if (KeypadInput != '\0')
			{
				userPW[1] = KeypadInput;
				pw2_flag = 1;
				
			}
			
			break;
		case Password_insert_3:
			delay_ms(500);
			KeypadInput = GetKeypadKey();
			if (KeypadInput == '\0')
			{
				
				LCD_DisplayString(1,"**");
				
			}
			if (KeypadInput != '\0')
			{
				userPW[2] = KeypadInput;
				pw3_flag = 1;
				
				//passwordFlag = 1;
			}
			break;
		case Password_enter:
			delay_ms(500);
		// user presses "*" to enter
			LCD_DisplayString(1,"***");
			break;
		case Password_check:
			delay_ms(500);
			for (int i = 1; i <= 3; i++)
			{
				if (userPW[i] == defPW[i])
				{
					passwordFlag = 1;
				}
				else
					incorrectFlag = 1;
			}
			break;
		case Password_loop:
			delay_ms(500);
			KeypadInput = GetKeypadKey();
			if (passwordFlag == 1)
			{
				if (KeypadInput != '\0')
				{
					password_state = Password_loop;
				}
			}
			break;
	}
}

void displayTick()
{
	switch(display_state)
	{
		case Display_Start:
			display_state = DisplayPW;
			break;
		case DisplayPW:
			display_state = DisplayPW;
			break;
	}
	
	switch(display_state)
	{
		case Display_Start:
			break;
		case DisplayPW:
		/*	if (dispPos == 1) // idle, no key presses
			{
				LCD_DisplayString(1,"LED");
			}
			if (dispPos == 2)
			{
				LCD_DisplayString(1,"SERVO");
			}
*/
			break;
	}
}

int main(void)
{
	DDRA = 0x00; PORTA = 0xFF;
	DDRB = 0xF0; PORTB = 0x0F; // half input, half output for keypad input
	DDRC = 0xFF; PORTC = 0x00; // PC7..4 outputs init 0s, PC3..0 inputs init 1s
	DDRD = 0xFF; PORTD = 0x00;
    LCD_init();
	ADC_init();
	button_state = BUTTON_WAIT;
	
	display_state = Display_Start;
	password_state = Password_Start;
	select_state = SELECT_START;
	combine_state = COMBINE_START;
	m_state = WAIT;
	TimerSet(125);
	TimerOn();
	initUSART(0); // using usart0
	setPW();
    while (1) 
    {
	
		combineTick();
		
		/*while (!TimerFlag)
		{
			;
		}
		TimerFlag = 0;
	*/
    }

}

void setPW()
{
	defPW[0] = 1;
	defPW[1] = 0;
	defPW[2] = 4;
	
}

void getPW()
{
	for(int i = 1; i <= 3; i++)
	{
		LCD_Cursor(i);
		LCD_WriteData(userPW[i-1] + '0');
	
	}
}


void selectionTick()
{
	switch(select_state)
	{
		case SELECT_START:
			if (passwordFlag == 1)
			{
				select_state = SELECT_LED;
			}
			else
				select_state = SELECT_START;
			break;
		case SELECT_LED:
			
			if(getPosition() == 1)
			{
				select_state = SELECT_LED;
				position = 0;
				dispPos = 1;
			}
			if(getPosition() == 2)
			{
				select_state = SELECT_SERVO;
				position = 0;
				dispPos = 2;
			}
			break;
		case SELECT_SERVO:
			if (getPosition() == 1)
			{
				select_state = SELECT_LED;
				position = 0;
				dispPos = 1;
			}
			if (getPosition() == 2)
			{
				dispPos = 2;
				select_state = SELECT_SERVO;
				position = 0;

			}
			break;
	}
	
	switch(select_state)
	{
		case SELECT_START:
			
			break;
		case SELECT_LED:
		if (passwordFlag == 1)
		{	
			LCD_ClearScreen();
			for (int i = 1; i <= 3; i++)
			{	
				LCD_Cursor(i);
				LCD_WriteData(userPW[i-1] + '0');
			}
			//LCD_DisplayString(1,"Press for LED");
			if (BUTTON)
			{
				if (LEDFlag == 1)
				{
					LEDFlag = 0;
				}
				else
				LEDFlag = 1;
				
				if(USART_IsSendReady(0))
				{
					USART_Flush(0);
					if (LEDFlag == 1)
					{
						temp = 0xFF;
					}
					if(LEDFlag == 0)
					{
						temp = 0x00;
					}
					
					USART_Send(temp,0); //send to follower
					if (USART_HasTransmitted(0))
					{
						
						LCD_DisplayString(1,"SENT");
					}
				}
			}
		}
			break;
		case SELECT_SERVO:
			if (passwordFlag == 1)
			{
				LCD_ClearScreen();
				LCD_DisplayString(1,"Press for Servo");
			}
			break;
	}
}

void buttonTick()
{
	unsigned char temp = 0x00;
	switch(button_state) // transitions
	{
		case BUTTON_WAIT:
		if (BUTTON)
		{
			button_state = BUTTON_PRESSED;
		}
		
		break;
		case BUTTON_PRESSED:
		button_state = BUTTON_HELD;
		break;
		case BUTTON_HELD:
		if (BUTTON)
		{
			button_state = BUTTON_HELD;
		}
		else
		button_state = BUTTON_RELEASED;
		break;
		case BUTTON_RELEASED:
		button_state = BUTTON_WAIT;
		break;
	}
	
	switch(button_state)
	{
		case BUTTON_WAIT:
		//servo_PWM_off();
		break;
		case BUTTON_PRESSED:
	
		
		break;
		case BUTTON_HELD:
		
		if ( dispPos == 2)
		{
		DDRD = 0xFF;
		TCCR1A |= (1<<COM1A1) | (1<<COM1B1) | (1<<WGM11);
		TCCR1B |= (1<<WGM13) | (1<<WGM12) | (1<<CS11) | (1<<CS10);
		ICR1 = 2499;
		OCR1A = 97;
		//Wait();
		_delay_ms(1500);
		OCR1A = 175;
		_delay_ms(1500);
		//OCR1A=316;
		//OCR1A = 425;
		OCR1A = 300;
		_delay_ms(1500);
		OCR1A = 535;
		
		//Wait();
		servo_PWM_off();
		
		}
		else
		
		break;
		case BUTTON_RELEASED:
		break;
	}
}

void combineTick()
{
	switch (combine_state)
	{
		case COMBINE_START:
			combine_state = COMBINE_TICKS;
			break;
		case COMBINE_TICKS:
			combine_state = COMBINE_TICKS;
			break;
	}
	
	switch(combine_state)
	{
		case COMBINE_START:
			break;
		case COMBINE_TICKS:
			passwordTick();
			selectionTick();
			displayTick();
			buttonTick();
			
			break;
	}
}
/*
enum checkPW_States {CHECK_START, CHECK_PW}check_state = CHECK_START;
void checkPWTick()
{
	switch(check_state) // transitions
	{
		case CHECK_START:
			check_state = CHECK_PW;
			break;
		case CHECK_PW:
			if (passwordFlag == 1)
			{
				// check input to the eeprom 
				
			}
			break;
	}
}


void master_tick()
{
	unsigned char temp = 0x00;
	switch(m_state) // transitions
	{
		case WAIT:
		if (BUTTON)
			{
				m_state = SEND;
			}
		else
			m_state = WAIT;
		break;
		case SEND:
		m_state = WAIT;
		break;
		default:
		m_state = WAIT;
		break;
	}
	
	switch(m_state) // actions
	{
		case WAIT:
		
		break;
		case SEND:
		
			if(USART_IsSendReady(0))
			{
				//USART_Flush(0);
				temp = 0xFF;
				USART_Send(temp,0); //send to follower
			}
		
			if (USART_HasTransmitted(0))
			{
				LCD_DisplayString(1,"SENT");
			}
		break;
		default:
		break;
	}
	
}
*/

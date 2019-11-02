/*	Author: lim001
 *  Partner(s) Name: Festo Bwogi
 *	Lab Section:
 *	Assignment: Lab #  Exercise #
 *	Exercise Description: [optional - include for your own benefit]
 *
 *	I acknowledge all content contained herein, excluding template or example
 *	code, is my own original work.
 */
#include <avr/io.h>
#ifdef _SIMULATE_
#include "simAVRHeader.h"
#endif

enum State{START, OFF, C4, D4, E4} state;

unsigned char button = 0x00;

void set_PWM(double frequency) {
	if (frequency != current_frequency) {
		if (!frequency) { TCCR0B &= 0x08; }
		else { TCCR0B |= 0x03; }	

		if (frequency < 0.954) { OCR0A = 0xFFFF; }

		else { OCR0A = (short)(8000000 / (128 * frequency)) - 1; }
	
		TCNT0 = 0;
		current_frequency = frequency;
	}
}

void PWM_on() {
	TCCR0A = (1 << COM0A0) | (1 << WGM00);
	TCCR0B = (1 << WGM02) | (1 << CS01) | (1 << CS00);
	set_PWM(0);
}

void PWM_off() {
	TCCR0A = 0x00;
	TCCR0B = 0x00;
}

void Tick() {
	
	button = ~PINA & 0x07;
	
	switch(state) { // Transition actions
		case START:
			state = OFF;
			break;
		case OFF:
			if(button == 1) {
				state = C4;
			} else if(button == 2) {
				state = D4;
			} else if(button == 4) {
				state = E4;
			} else {
				state = OFF;
			}
			break;
		case C4:
			if(button == 1) {
				state = C4;
			} else {
				state = OFF;
			}
			break;
		case D4:
			if(button == 2) {
				state = D4;
			} else {
				state = OFF;
			}
			break;
		case E4:
			if(button == 4) {
				state = E4;
			} else {
				state = OFF;
			}
			break;
	}
	
	switch(state) { // State actions
		case OFF:
			set_PWM(0);
			break;
		case C4:
			set_PWM(261.63);
			break;
		case D4:
			set_PWM(293.66);
			break;
		case E4:
			set_PWM(329.63);
			break;
		default:
			break;
	}
}

int main(void)
{
    DDRA = 0x00; PORTA = 0xFF; // Configure A's 8 pins as inputs. Initialize to 1's
	DDRB = 0xFF; PORTB = 0x00; // Configure B's 8 pins as outputs. Initialize to 0's
	
	PWM_on();
	state = START;
    
	while (1) 
    {
		Tick();
    }
}	

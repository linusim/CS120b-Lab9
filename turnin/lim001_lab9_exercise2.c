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

volatile unsigned char TimerFlag = 0;

unsigned long _avr_timer_M = 1;
unsigned long _avr_timer_cntcurr = 0;

void TimerSet(unsigned long M) {
	_avr_timer_M = M;
	_avr_timer_cntcurr = _avr_timer_M;
}

void TimerOn() {
	TCCR1B = 0x0B;
	TIMSK1 	= 0x02;
	TCNT1 = 0;
	_avr_timer_cntcurr = _avr_timer_M;
	SREG |= 0x80;	// 0x80: 1000000
}

void TimerOff() {
	TCCR1B = 0x00;
}

void TimerISR() {
	TimerFlag = 1;
}

void System_On() {
	A0 = ~PINA & 0x01;
	
	switch(power) {
		case START:
		power = OFF;
		break;
		case OFF:
		if(A0) {
			power = ON;
		}
		break;
		case ON:
		if(A0) {
			power = OFF;
		}
		break;
	}
	
	switch(power) {
		case OFF:
		set_PWM(0);
		i = 0x00;
		break;
		case ON:
		set_PWM(note[i]);
		break;
		default:
		break;
	}
}

void Play_Note() {
	A1 = ~PINA & 0x02;
	A2 = ~PINA & 0x04;
	
	switch(play) {
		case PSTART:
		play = WAIT;
		break;
		case WAIT:
		if(A1) {
			play = INC;
			} else if(A2) {
			play = DEC;
			} else {
			play = WAIT;
		}
		break;
		case INC:
		play = WAIT;
		break;
		case DEC:
		play = WAIT;
		break;
	}
	
	switch(play) {
		case WAIT:
		break;
		case INC:
		if(i < 7) {
			i++;
		}
		break;
		case DEC:
		if(i > 0) {
			i--;
		}
		break;
		default:
		break;
	}
}

int main(void)
{
	DDRA = 0x00; PORTA = 0xFF; // Configure A's 8 pins as inputs. Initialize to 1's
	DDRB = 0xFF; PORTB = 0x00; // Configure B's 8 pins as outputs. Initialize to 0's
	
	TimerSet(150);
	TimerOn();
	
	PWM_on();
	
	power = START;
	play = PSTART;
	
	while (1)
	{
		System_On();
		Play_Note();
		
		while(!TimerFlag);
		TimerFlag = 0;
	}
}

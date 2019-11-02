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
#include <avr/interrupt.h>

enum Power{START, PLAY, WAIT, PAUSE} power;
//enum Play{PSTART, WAIT, INC, DEC} play;

unsigned char A0; // ON/OFF button
///unsigned char A1; // Go up button
//unsigned char A2; // Go down button

//261.63, 293.66, 329.63, 349.23, 392.00, 440.00, 493.88, 523.25
const double note[10] = {261.63, 440.00, 440.00, 261.63, 329.63, 392.00, 493.88, 523.25, 293.66, 293.66};

//const double timeHeld[1000];
//const double timeBtwn[500];
unsigned char i = 0x00; // iterator to iterate through note[]
unsigned char j = 0x00;	
unsigned char k = 0x00;

void set_PWM(double frequency) {
	static double current_frequency; 
	if (frequency != current_frequency) {
		if (!frequency) { TCCR0B &= 0x08; }
		else { TCCR0B |= 0x03; }
		if (frequency < 0.954) { OCR0A = 0xFFFF; }

		else if (frequency > 31250) { OCR0A = 0x0000; }
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
	TCCR1B 	= 0x0B;
	OCR1A 	= 125;
	TIMSK1 	= 0x02; 
	TCNT1 = 0;
	_avr_timer_cntcurr = _avr_timer_M;
	SREG |= 0x80;	// 0x80: 1000000
}

void TimerOff() {
	TCCR1B 	= 0x00;
}

void TimerISR() {
	TimerFlag = 1;
}

ISR(TIMER1_COMPA_vect) {
	_avr_timer_cntcurr--; 	
	if (_avr_timer_cntcurr == 0) { 
		TimerISR(); 
		avr_timer_cntcurr = _avr_timer_M;
	}
}

void System_On() {
	A0 = ~PINA & 0x01;
	
	switch(power) {
		case START:
			if(A0 == 1){
				power = PLAY;
			}
			else{
				power = START;
			}
			break;
			
		case PLAY:
			if((i == 10) && (A0 = 1)){
				power = PAUSE;
				set_PWM(0);
				
			}
			else if((i == 10) && (A0 == 0)){
				power = START;
			}
			else if(i < 10){
				power = WAIT;
			}
			break;
			
		case WAIT:
			if(j==200){
				power = PLAY;
			}
			break;
		case PAUSE:
			if(A0 == 0){
				power = START;
			}
		default:
			break;
	}
	
	switch(power) {
		case START:
			set_PWM(0);
			i = 0x00;
			j = 0x00;
			k = 0x00;
			break;
			
		case PLAY:
			set_PWM(note[i]);
			break;
			
		case WAIT:
			while(j!=200){
				++j;
			}
			++i;
		case PAUSE:
			break;
			
		default:
			break;
	}
}

int main(void) {
	DDRA = 0x00; PORTA = 0xFF;
	DDRB = 0xFF; PORTB = 0x00; 

	TimerSet(150);
	TimerOn();
	
	PWM_on();
	
	power = START;
	
	while (1) 
    {
		System_On();
		
		while(!TimerFlag);
		TimerFlag = 0;
	}
}


#include <inttypes.h>
#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include <avr/wdt.h>
#include <avr/sleep.h>
#include <avr/delay.h>
#include "usiTwiSlave.h"

#define USI_SCK PA4
#define USI_MISO PA5
#define LED_RED PB1
#define LED_GREEN PB0

void conductionOn();
void conductionOff();
void stopHoldTimer();
void conduct();
void stopConductionTimer();
void startHoldTimer();
void setupZCInterrupt();

uint16_t conductionDelay = 300;
uint16_t conductionCounter = 0;
uint16_t holdDelay = 75;
uint16_t periodLength = 0;

inline void stopConductionTimer() {
	TIMSK0 = 0;
	TCCR0B = 0;
	conductionCounter = 0;
}

inline void conductionOn() {
	PORTB |= _BV(PB0);
	PORTA |= _BV(PA7);
}

inline void conductionOff() {
	PORTB &= ~_BV(PB0);
	PORTA &= ~_BV(PA7);
}

inline void conduct() {
	conductionOn();
}

inline void startHoldTimer() {
	TIMSK1 |= _BV(OCIE1A);
	OCR1A = holdDelay;
	TCCR1B |= _BV(WGM12);
	TCCR1B |= _BV(CS10);
}

inline void stopHoldTimer() {
	TCCR1B = 0;
}

//Hold timer - keeps conduction pulse length constant
ISR(TIM1_COMPA_vect) {
	conductionOff();
	stopHoldTimer();
}

//Conduction timer - counts time from zero crossing to conduction
ISR(TIM0_OVF_vect) {
	if(conductionCounter++ >= conductionDelay) {
		conduct();
		stopConductionTimer();
		startHoldTimer();
	}
}

void startConductionTimer() {
	TCNT0 = 0;
	TIMSK0 |= _BV(TOIE0);
	TCCR0B |= _BV(CS00);
	conductionCounter = 0;
}

//Zero crossing interrupt
ISR(EXT_INT0_vect) {
	//in case conduction is still on when zc happens, turn it off.
	//this shouldn't normally happen
	conductionOff();
	stopHoldTimer();
	startConductionTimer();
}

inline void setupZCInterrupt() {
	MCUCR |= _BV(ISC01) | _BV(ISC00);
	GIMSK |= _BV(INT0);
}

void measurePeriodLength() {
	PORTB |= _BV(PB1);
	while (0 == (PINB & _BV(PB2)));
	while (PINB & _BV(PB2));
	TCNT1 = 0;
	TCCR1B |= _BV(CS12);
	while (0 == (PINB & _BV(PB2)));
	while (PINB & _BV(PB2));
	TCCR1B = 0;
	periodLength = TCNT1;
	PORTB &= ~_BV(PB1);
}

int main (void) {
	DDRB |= _BV(PB0) | _BV(PB1);
	DDRA |= _BV(PA7);
	PORTA = 0;

	PORTB |= _BV(PB0);
	_delay_ms(100);
	PORTB &= ~_BV(PB0);

	measurePeriodLength();
	setupZCInterrupt();
	sei();

	usiTwiSlaveInit(0x20);

	while(1) {
		if(usiTwiDataInReceiveBuffer()) {
			uint8_t usiRx = usiTwiReceiveByte();
			if(1 == usiRx) {
				conductionDelay = 220;
			} else if (2 == usiRx) {
				conductionDelay = 230;
			} else if (3 == usiRx) {
				conductionDelay = 300;
			} else if (4 == usiRx){
				usiTwiTransmitByte(periodLength >> 8);
				usiTwiTransmitByte(periodLength &0x00FF);
			}
		}
//		_delay_ms(1000);
//		PORTB &= ~_BV(PB1);
//		_delay_ms(1000);
//		PORTB |= _BV(PB1);

//		if(conductionCounter > 10) {
//			conductionOff();
//		}
	}
}

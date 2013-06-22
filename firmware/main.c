#include <inttypes.h>
#include <avr/io.h>
#include <stdlib.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include <avr/wdt.h>
#include <avr/sleep.h>
#include <avr/pgmspace.h>
#include <avr/wdt.h>
#include <avr/eeprom.h>
#include "usiTwiSlave.h"

#define USI_SCK PA4
#define USI_MISO PA5
#define LED_RED PB1
#define LED_GREEN PB0

#define I2C_SET_POWER 1
#define I2C_SET_CONDUCTION_ANGLE 3
#define I2C_READ_PERIOD_LENGTH 4
#define I2C_READ_ZC_PULSE_LENGHT 5
#define I2C_READ_OSCCAL 6
#define I2C_READ_CONDUCTION_ANGLE 7
#define I2C_READ_LAST_PERIOD_LENGTH 8
#define I2C_RESET 9
#define I2C_CALIBRATE 10

#define reset() wdt_enable(WDTO_15MS); while(1) {}

void conductionOn();
void conductionOff();
void setupZCInterrupt();
void setTimerInterruptAt();
void stopTimer();

#define NEVER 0
#define EVENT_TRUE_ZC 1
#define EVENT_CONDUCT 2

volatile uint16_t interruptAt = NEVER;

volatile uint16_t conductionAngle = 9000;
volatile uint16_t maxConductionAngle = 9800;
volatile uint16_t zcPulseLength = 0;
volatile uint16_t periodLength = 0;
volatile uint8_t oscAjdustDelayCounter = 0;
volatile uint16_t newConductionAngle = 9000;
volatile uint16_t zcInterruptTimestamp = 0;
extern const uint16_t conductionAngles[256];

#define EDGE_LEADING 0
#define EDGE_FALLING 1
#define MAX_SANE_ZC_LENGTH_DIFF 5
#define true 1
#define false 0

uint8_t zcInterruptEdge = EDGE_LEADING;
uint8_t powerGood = false;
uint8_t calibrationGood = false;
volatile uint8_t fadeIn = 1;
void fadeInOut() {
	if(fadeIn) {
		newConductionAngle --;
		if(newConductionAngle < 8000) {
			fadeIn = !fadeIn;
		}
	} else {
		newConductionAngle ++;
		if(newConductionAngle > 9999) {
			fadeIn = !fadeIn;
		}
	}
}

inline void conductionOn() {
	PORTB |= _BV(PB0);
	PORTA |= _BV(PA7);
}

inline void conductionOff() {
	PORTB &= ~_BV(PB0);
	PORTA &= ~_BV(PA7);
}

inline void resetTimer() {
	TCNT1 = 0;
}

inline void setTimerInterruptAt(const uint16_t countTo, const uint16_t nextEvent) {
	TIMSK1 |= _BV(OCIE1A) | _BV(TOIE1);
	OCR1A = countTo;
	TCCR1B |= _BV(CS11); //fmcu/8 = 1Mhz == 10000 counts per period
	interruptAt = nextEvent;
}

inline void stopTimer() {
	TCCR1B = 0;
}

inline void continueTimer() {
	TCCR1B |= _BV(CS11); //fmcu/8 = 1Mhz == 10000 counts per period
}

//Timer interrupt - happens at various events in life
ISR(TIM1_COMPA_vect) {
	if(EVENT_TRUE_ZC == interruptAt) {
		stopTimer();
		setTimerInterruptAt(TCNT1 + conductionAngle, EVENT_CONDUCT);
	} else if(EVENT_CONDUCT == interruptAt) {
		conductionOn();
	}
}

ISR(TIM1_OVF_vect) {
	powerGood = false;
	PORTB |= _BV(PB1);
	PORTB &= ~_BV(PB0);
}

inline void saveCalibration(){
	eeprom_write_byte(0x00, OSCCAL);
}

inline void adjustOscillatorSpeed() {
	if (oscAjdustDelayCounter++ > 128) {
		if(periodLength > 10060) {
			OSCCAL--;
		} else if(periodLength < 9940){
			OSCCAL++;
		} else {
			calibrationGood = true;
			saveCalibration();
		}
		oscAjdustDelayCounter = 0;
	}
}

volatile int32_t periodFiltered = 0;
volatile int32_t periodFiltered2 = 0;

inline uint16_t filterPeriodLength(int32_t thisPeriodLength) {
	if(0 != periodFiltered2) {
		periodFiltered = periodFiltered
				+ ((thisPeriodLength - periodFiltered) >> 5);
		periodFiltered2 = periodFiltered2 + ((periodFiltered - periodFiltered2) >> 5);
	} else {
		periodFiltered = thisPeriodLength;
		periodFiltered2 = thisPeriodLength;
	}
	return (uint16_t) periodFiltered2;
}

inline void setupZCInterrupt() {
	MCUCR |= _BV(ISC01) | _BV(ISC00);
	GIMSK |= _BV(INT0);
	zcInterruptEdge = EDGE_LEADING;
}

inline void flipZcInterruptEdge() {
	zcInterruptEdge = !zcInterruptEdge;
	MCUCR = MCUCR ^ 1; //flip last bit, changing the edge for the next intettupt
}

//Zero crossing interrupt: collects statistics, restarts the timer - next stop - true zero crossing interrupt
ISR(EXT_INT0_vect) {
	PORTB |= _BV(PB0);
	stopTimer();

	if(EDGE_LEADING == zcInterruptEdge) {
		//fadeInOut();
		conductionOff();
		stopTimer();
		periodLength = filterPeriodLength(TCNT1);
		zcInterruptTimestamp = TCNT1;
		resetTimer();
		setTimerInterruptAt(zcPulseLength >> 1, EVENT_TRUE_ZC);
		adjustOscillatorSpeed();
		conductionAngle = newConductionAngle;
	} else {
		uint8_t lastZcPulseLength = TCNT1;
		continueTimer();
		int diff = abs(zcPulseLength - lastZcPulseLength);
		if (diff < MAX_SANE_ZC_LENGTH_DIFF || 0 == zcPulseLength) {
			zcPulseLength = lastZcPulseLength;
		}
	}
	PORTB &= ~_BV(PB0);
	PORTB &= ~_BV(PB1);
	flipZcInterruptEdge();
	powerGood=1;
}


int main (void) {
	DDRB |= _BV(PB0) | _BV(PB1);
	PORTB |= _BV(PB2);
	DDRA |= _BV(PA7);
	PORTA = 0;

	OSCCAL = 100;
	//calibrate();

	setupZCInterrupt();
	sei();
	usiTwiSlaveInit(0x20);

	PORTB |= _BV(PB0);
	_delay_ms(100);
	PORTB &= ~_BV(PB0);
	_delay_ms(500);


	while(1) {
		if(usiTwiDataInReceiveBuffer()) {
			uint8_t usiRx = usiTwiReceiveByte();
			if(I2C_SET_POWER == usiRx) {
				usiRx = usiTwiReceiveByte();
				conductionAngle = pgm_read_word(&conductionAngles[usiRx]);
			} else if (2 == usiRx) {
				conductionAngle = 2300;
			} else if (I2C_SET_CONDUCTION_ANGLE == usiRx) {
				usiRx = usiTwiReceiveByte();
				newConductionAngle = ((uint16_t) usiRx) << 8;
				usiRx = usiTwiReceiveByte();
				newConductionAngle |= (((uint16_t) usiRx) & 0x00FF);
			} else if (I2C_READ_PERIOD_LENGTH == usiRx){
				usiTwiTransmitByte(periodLength >> 8);
				usiTwiTransmitByte(periodLength & 0x00FF);
			} else if (I2C_READ_ZC_PULSE_LENGHT == usiRx){
				usiTwiTransmitByte(zcPulseLength >> 8);
				usiTwiTransmitByte(zcPulseLength & 0x00FF);
			} else if (I2C_READ_OSCCAL == usiRx){
				usiTwiTransmitByte(OSCCAL);
			} else if (I2C_READ_CONDUCTION_ANGLE == usiRx){
				usiTwiTransmitByte(conductionAngle >> 8);
				usiTwiTransmitByte(conductionAngle & 0x00FF);
			} else if (I2C_READ_LAST_PERIOD_LENGTH == usiRx){
				usiTwiTransmitByte(zcInterruptTimestamp >> 8);
				usiTwiTransmitByte(zcInterruptTimestamp & 0x00FF);
			} else if (I2C_RESET == usiRx) {
				reset();
			} else {
//				while(usiTwiDataInReceiveBuffer()) {
//					usiTwiReceiveByte();
//				}
			}
//			_delay_ms(100);
//			PORTB |= _BV(PB0);
//			_delay_ms(100);
//			PORTB &= ~_BV(PB0);

		}
	}
}

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

#define GREEN PB0
#define RED PB1

/*
 * Sets the linear output power. 1 byte argument: [0 - 255]
 */
#define I2C_SET_POWER 1
/*
 * Directly sets the conduction angle. 2 bytes argument: [0 - 1000]
 */
#define I2C_SET_CONDUCTION_ANGLE 3
/*
 * Returns 2 bytes of filtered period length, represents the number of
 * internal timer ticks between two zero crossings.
 * Should be around 10000.
 * */
#define I2C_READ_PERIOD_LENGTH 4
/**
 * Returns 2 bytes of zero crossing pulse length.
 */
#define I2C_READ_ZC_PULSE_LENGHT 5
/**
 * Returns the 1 byte value of processor speed calibration register.
 */
#define I2C_READ_OSCCAL 6
/**
 * Returns 2 bytes of currently set conduction angle [0 - 10000]
 */
#define I2C_READ_CONDUCTION_ANGLE 7
/*
 * Returns 2 bytes of last period length, represents the number of
 * internal timer ticks between two zero crossings.
 * Should be around 10000.
 * */
#define I2C_READ_LAST_PERIOD_LENGTH 8
/**
 * Resets the Triac Bloc
 */
#define I2C_RESET 9
/**
 * Starts the calibration routine.
 * Red and green leds blink alternatingly while calibration is in progress.
 * Might take a while.
 */
#define I2C_CALIBRATE 10
#define I2C_ADC 11
/**
 * A routine for testing I2C connection. Both red and green leds will light up for a second.
 */
#define I2C_TEST 12

#define reset() wdt_enable(WDTO_15MS); while(1) {}

inline void conductionOn();
inline void conductionOff();
inline void setupZCInterrupt();
inline void setTimerInterruptAt(const uint16_t countTo, const uint8_t nextEvent);
inline void stopTimer();

#define NEVER 0
#define EVENT_TRUE_ZC 1
#define EVENT_CONDUCT 2

volatile uint8_t interruptAt = NEVER;
volatile uint16_t conductionAngle = 9000;
volatile uint16_t newConductionAngle = 9000;
volatile uint16_t zcPulseLength = 0;
volatile uint16_t periodLength = 0;
volatile uint8_t oscAjdustDelayCounter = 0;
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
volatile uint8_t j = 255;
void fadeInOut() {
	if(fadeIn) {
		j --;
		if(j < 254) {
			fadeIn = !fadeIn;
		}
	} else {
		j ++;
		if(j > 254) {
			fadeIn = !fadeIn;
		}
	}
	newConductionAngle = pgm_read_word(&conductionAngles[j]);
}

inline void conductionOn() {
	PORTA |= _BV(PA7);
}

inline void conductionOff() {
	PORTA &= ~_BV(PA7);
}

inline void resetTimer() {
	TCNT1 = 0;
}

inline void setTimerInterruptAt(const uint16_t countTo, const uint8_t nextEvent) {
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
	} else {// if(EVENT_CONDUCT == interruptAt)
		conductionOn();
	}
}

ISR(TIM1_OVF_vect) {
	conductionOff();
	stopTimer();
	powerGood = false;
}

inline void saveCalibration(){
	eeprom_write_byte(0x00, OSCCAL);
}

inline void adjustOscillatorSpeed() {
	if (oscAjdustDelayCounter++ > 128) {
		if(periodLength > 10060) {
			OSCCAL--;
			calibrationGood = false;
		} else if(periodLength < 9940){
			OSCCAL++;
			calibrationGood = false;
		} else {
			calibrationGood = true;
		}
		oscAjdustDelayCounter = 0;
	}
}

volatile int32_t periodFiltered = 0;
volatile int32_t periodFiltered2 = 0;

inline uint16_t filterPeriodLength(int32_t thisPeriodLength) {
	if(0 != periodFiltered2) {
		periodFiltered = periodFiltered	+ ((thisPeriodLength - periodFiltered) >> 5);
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
	flipZcInterruptEdge();
	powerGood=1;
}

inline void ledOn(uint8_t led){
	PORTB |= _BV(led);
}

inline void ledOff(uint8_t led){
	PORTB &= ~_BV(led);
}

inline void calibrate() {
	uint8_t calibration = eeprom_read_byte(0x00);
	if(calibration != 255) {
		OSCCAL = calibration;
	}

	while(!calibrationGood) {
		ledOn(RED);
		_delay_ms(300);
		ledOff(RED);
		ledOn(GREEN);
		_delay_ms(300);
		ledOff(GREEN);
	}
	eeprom_write_byte(0x00, OSCCAL);
}

int main (void) {
	MCUSR = 0;
	wdt_disable();
	DDRB |= _BV(RED) | _BV(GREEN);
	PORTB |= _BV(PB2);
	DDRA |= _BV(PA7);
	PORTA = 0;

	setupZCInterrupt();
	usiTwiSlaveInit(0x20);
	sei();

//	while(!powerGood) {
//		ledOn(RED);
//		_delay_ms(100);
//		ledOff(RED);
//		_delay_ms(100);
//	}
//
//	calibrate();

	newConductionAngle = pgm_read_word(&conductionAngles[255]);

	DDRA |= _BV(PA0);
	PORTA |= _BV(PA0);

	ADMUX = _BV(MUX1);
	ADCSRA = _BV(ADEN) | _BV(ADPS2);
	ADCSRB = _BV(ADLAR);

	while(1) {
		if(usiTwiDataInReceiveBuffer()) {
			uint8_t usiRx = usiTwiReceiveByte();
			if(I2C_SET_POWER == usiRx) {
				newConductionAngle = pgm_read_word(&conductionAngles[usiTwiReceiveByte()]);
			} else if (I2C_SET_CONDUCTION_ANGLE == usiRx) {
				newConductionAngle = ((uint16_t) usiTwiReceiveByte()) << 8;
				newConductionAngle |= (((uint16_t) usiTwiReceiveByte()) & 0x00FF);
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
			} else if(I2C_CALIBRATE == usiRx) {
				calibrate();
			} else if(I2C_TEST == usiRx) {
				ledOn(RED);
				ledOn(GREEN);
				_delay_ms(2000);
				ledOff(RED);
				ledOff(GREEN);
			}
		}

		if(!powerGood) {
			ledOn(RED);
			_delay_ms(100);
			ledOff(RED);
			_delay_ms(100);
		}
		ledOff(GREEN);
	}
}

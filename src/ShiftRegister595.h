#ifndef SHIFTREGISTER_H
#define SHIFTREGISTER_H
#include <Arduino.h>
#include <pins_arduino.h>
#include <wiring_private.h>
#include "turn_off_pwm.h"

class ShiftRegister595
{
	unsigned char m_dataPort, m_clockPort, m_latchPort;
	volatile unsigned char *m_dataReg, *m_clockReg, *m_latchReg;
	unsigned char m_dataBit, m_clockBit, m_latchBit;
	bool m_msbFirst;
	
	void setPin(volatile unsigned char *reg, unsigned char bit, bool val) {
		unsigned char oldSREG = SREG;
		cli();

		if(val)
			*reg |= bit;
		else
			*reg &= ~bit;

		SREG = oldSREG;
	}

	unsigned char reverse(unsigned char b) {
		b = (b & 0xF0) >> 4 | (b & 0x0F) << 4;
		b = (b & 0xCC) >> 2 | (b & 0x33) << 2;
		b = (b & 0xAA) >> 1 | (b & 0x55) << 1;
		return b;
	}

	void shiftValues() {}

	template <typename... Targs>
	void shiftValues(unsigned char curVal, Targs... vals) {
		//shiftOut(m_dataPin, m_clockPin, m_msbFirst, curVal);

		if( m_msbFirst == MSBFIRST )
			curVal = reverse(curVal);

		unsigned char mask = 1;
		for( unsigned char i = 0; i < 8; i++ ) {
			setPin(m_dataReg, m_dataBit, curVal&mask);
			mask <<= 1;

			setPin(m_clockReg, m_clockBit, HIGH);
			setPin(m_clockReg, m_clockBit, LOW);
		}

		shiftValues(vals...);
	}
public:

	ShiftRegister595(unsigned char dataPin, unsigned char clockPin, unsigned char latchPin, bool msbFirst) {
		pinMode(dataPin, OUTPUT);
		pinMode(clockPin, OUTPUT);
		pinMode(latchPin, OUTPUT);
		
		// Manually setup pins for speeding up the data output.
		m_dataPort = digitalPinToPort(dataPin);
		m_latchPort = digitalPinToPort(latchPin);
		m_clockPort = digitalPinToPort(clockPin);

		if (m_dataPort == NOT_A_PORT) return;
		if (m_clockPort == NOT_A_PORT) return;
		if (m_latchPort == NOT_A_PORT) return;
		
		m_dataBit = digitalPinToBitMask(dataPin);
		m_latchBit = digitalPinToBitMask(latchPin);
		m_clockBit = digitalPinToBitMask(clockPin);
		
		// Manually disable PWM.
		unsigned char timer = digitalPinToTimer(dataPin);
		if (timer != NOT_ON_TIMER) turn_off_pwm(timer);
		
		timer = digitalPinToTimer(clockPin);
		if (timer != NOT_ON_TIMER) turn_off_pwm(timer);
		
		timer = digitalPinToTimer(latchPin);
		if (timer != NOT_ON_TIMER) turn_off_pwm(timer);

		m_dataReg = portOutputRegister(m_dataPort);
		m_latchReg = portOutputRegister(m_latchPort);
		m_clockReg = portOutputRegister(m_clockPort);
	}

	template<typename... Targs>
	void write(Targs... vals) {
		if (m_dataPort == NOT_A_PORT) return;
		if (m_clockPort == NOT_A_PORT) return;
		if (m_latchPort == NOT_A_PORT) return;

		setPin(m_latchReg, m_latchBit, LOW);
		setPin(m_clockReg, m_clockBit, LOW);
		
		shiftValues(vals...);
		
		setPin(m_latchReg, m_latchBit, HIGH);
	}

};

#endif
#ifndef FASTPIN_H
#define FASTPIN_H
#include <Arduino.h>
#include <wiring_private.h>
#include "turn_off_pwm.h"

class FastPin {
	unsigned char m_port, m_bit, m_mode;
	volatile unsigned char *m_outReg, *m_inReg;

public:
	FastPin() {}

	FastPin(unsigned char pin, unsigned char mode) {
		pinMode(pin, mode);
		m_mode = mode;

		m_port = digitalPinToPort(pin);
		if (m_port == NOT_A_PORT)
			return;

		m_bit = digitalPinToBitMask(pin);

		unsigned char timer = digitalPinToTimer(pin);
		if( timer != NOT_ON_TIMER )
			turn_off_pwm(timer);

		if( mode == OUTPUT )
			m_outReg = portOutputRegister(m_port);
		else
			m_inReg = portInputRegister(m_port);
	}

	void setPin(bool val) {
		if (m_port == NOT_A_PORT || m_mode != OUTPUT)
			return;
		
		unsigned char oldSREG = SREG;
		cli();

		if (val)
			*m_outReg |= m_bit;
		else
			*m_outReg &= ~m_bit;

		SREG = oldSREG;
	}

	bool getPin() {
		if (m_port == NOT_A_PORT || m_mode == OUTPUT)
			return LOW;

		return (*m_inReg & m_bit) ? HIGH : LOW;
	}
};

#endif
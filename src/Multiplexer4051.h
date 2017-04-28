#ifndef MULTIPLEXER_H
#define MULTIPLEXER_H
#include <Arduino.h>
#include <wiring_private.h>

class Multiplexer4051
{
	unsigned char m_addr0Port, m_addr1Port, m_addr2Port, m_commonPort;
	unsigned char m_addr0Bit, m_addr1Bit, m_addr2Bit, m_commonBit;
	volatile unsigned char *m_addr0Reg, *m_addr1Reg, *m_addr2Reg, *m_commonOutReg;

	unsigned char m_cmn;
	
	bool m_isOutput, m_isPullUp;

	void setPin(volatile unsigned char *reg, unsigned char bit, bool val) {
		unsigned char oldSREG = SREG;
		cli();

		if (val)
			*reg |= bit;
		else
			*reg &= ~bit;

		SREG = oldSREG;
	}

	bool getPin(volatile unsigned char *reg, unsigned char bit) {
		bool in = (*reg & bit) ? HIGH : LOW;
		Serial.println(in);
		return in;
	}

	void setAddress(unsigned char address) {
		setPin(m_addr2Reg, m_addr2Bit, address & 0x4);
		setPin(m_addr1Reg, m_addr1Bit, address & 0x2);
		setPin(m_addr0Reg, m_addr0Bit, address & 0x1);
	}
public:
	Multiplexer4051(){}
	
	Multiplexer4051(unsigned char addr0, unsigned char addr1, unsigned char addr2, unsigned char cmn, bool isOutput, bool isPullUp)
			: m_isOutput(isOutput),
			  m_isPullUp(isPullUp)
		{

		pinMode(addr0, OUTPUT);
		pinMode(addr1, OUTPUT);
		pinMode(addr2, OUTPUT);

		if (m_isOutput)
			pinMode(cmn, OUTPUT);
		else
			pinMode(cmn, isPullUp ? INPUT_PULLUP : INPUT);

		m_cmn = cmn;

		// Manually setup pins for speeding up the data output.
		m_addr0Port = digitalPinToPort(addr0);
		m_addr1Port = digitalPinToPort(addr1);
		m_addr2Port = digitalPinToPort(addr2);
		m_commonPort = digitalPinToPort(cmn);

		if (m_addr0Port == NOT_A_PORT) return;
		if (m_addr1Port == NOT_A_PORT) return;
		if (m_addr2Port == NOT_A_PORT) return;
		if (m_commonPort == NOT_A_PORT) return;

		m_addr0Bit = digitalPinToBitMask(addr0);
		m_addr1Bit = digitalPinToBitMask(addr1);
		m_addr2Bit = digitalPinToBitMask(addr2);
		m_commonBit = digitalPinToBitMask(cmn);

		// Manually disable PWM.
		unsigned char timer = digitalPinToTimer(addr0);
		if (timer != NOT_ON_TIMER) turn_off_pwm(timer);

		timer = digitalPinToTimer(addr1);
		if (timer != NOT_ON_TIMER) turn_off_pwm(timer);

		timer = digitalPinToTimer(addr2);
		if (timer != NOT_ON_TIMER) turn_off_pwm(timer);

		timer = digitalPinToTimer(cmn);
		if (timer != NOT_ON_TIMER) turn_off_pwm(timer);

		m_addr0Reg = portOutputRegister(m_addr0Port);
		m_addr1Reg = portOutputRegister(m_addr1Port);
		m_addr2Reg = portOutputRegister(m_addr2Port);
		
		if (isOutput)
			m_commonOutReg = portOutputRegister(m_commonPort);
	}

	unsigned char read() {
		if (m_isOutput)	return 0;
		if (m_addr0Port == NOT_A_PORT) return 0;
		if (m_addr1Port == NOT_A_PORT) return 0;
		if (m_addr2Port == NOT_A_PORT) return 0;

		unsigned char input = 0;

		for (unsigned char i = 7; i < 8; --i) {
			setAddress(i);
			input <<= 1;
			input |= digitalRead(m_cmn);
		}

		return m_isPullUp ? ~input : input;
	}

	void write(unsigned char value) {
		if (!m_isOutput) return;
		if (m_addr0Port == NOT_A_PORT) return;
		if (m_addr1Port == NOT_A_PORT) return;
		if (m_addr2Port == NOT_A_PORT) return;

		for (unsigned char i = 0; i < 8; i++) {
			setAddress(i);
			setPin(m_commonOutReg, m_commonBit, value & 1);
			value >>= 1;
		}
	}
};

#endif
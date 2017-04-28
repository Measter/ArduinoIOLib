#ifndef MULTIPLEXED_BTNS_H
#define MULTIPLEXED_BTNS_H

#include <Multiplexer4051.h>
#include <Button.h>
#include "turn_off_pwm.h"

class MultiplexedButtons4051
{
	Button m_btnArr[8];
	unsigned char m_addr0Port, m_addr1Port, m_addr2Port;
	unsigned char m_addr0Bit, m_addr1Bit, m_addr2Bit;
	volatile unsigned char *m_addr0Reg, *m_addr1Reg, *m_addr2Reg;
	bool m_isPullUp;

	void setPin(volatile unsigned char *reg, unsigned char bit, bool val) {
		unsigned char oldSREG = SREG;
		cli();

		if (val)
			*reg |= bit;
		else
			*reg &= ~bit;

		SREG = oldSREG;
	}

	void setAddress(unsigned char address) {
		setPin(m_addr2Reg, m_addr2Bit, address & 0x4);
		setPin(m_addr1Reg, m_addr1Bit, address & 0x2);
		setPin(m_addr0Reg, m_addr0Bit, address & 0x1);
	}

 public:
	MultiplexedButtons4051(){}
 
	MultiplexedButtons4051(unsigned char addr0, unsigned char addr1, unsigned char addr2,
			       unsigned char cmn, bool isPullUp, bool toggleMode[], unsigned char debounce)		  
	{
		m_isPullUp = isPullUp;
		
		pinMode(addr0, OUTPUT);
		pinMode(addr1, OUTPUT);
		pinMode(addr2, OUTPUT);
		pinMode(cmn, m_isPullUp ? INPUT_PULLUP : INPUT);

		// Manually setup pins for speeding up the data output.
		m_addr0Port = digitalPinToPort(addr0);
		m_addr1Port = digitalPinToPort(addr1);
		m_addr2Port = digitalPinToPort(addr2);

		if (m_addr0Port == NOT_A_PORT) return;
		if (m_addr1Port == NOT_A_PORT) return;
		if (m_addr2Port == NOT_A_PORT) return;

		m_addr0Bit = digitalPinToBitMask(addr0);
		m_addr1Bit = digitalPinToBitMask(addr1);
		m_addr2Bit = digitalPinToBitMask(addr2);

		// Manually disable PWM.
		unsigned char timer = digitalPinToTimer(addr0);
		if (timer != NOT_ON_TIMER) turn_off_pwm(timer);

		timer = digitalPinToTimer(addr1);
		if (timer != NOT_ON_TIMER) turn_off_pwm(timer);

		timer = digitalPinToTimer(addr2);
		if (timer != NOT_ON_TIMER) turn_off_pwm(timer);

		m_addr0Reg = portOutputRegister(m_addr0Port);
		m_addr1Reg = portOutputRegister(m_addr1Port);
		m_addr2Reg = portOutputRegister(m_addr2Port);
		
		for(unsigned char i = 0; i < 8; i++) {
			m_btnArr[i] = Button(cmn, debounce, isPullUp, toggleMode[i]);
		}
	}
	
	void updateState() {
		if (m_addr0Port == NOT_A_PORT) return;
		if (m_addr1Port == NOT_A_PORT) return;
		if (m_addr2Port == NOT_A_PORT) return;
		
		for (unsigned char i = 0; i < 8; i++) {
			setAddress(i);
			m_btnArr[i].updateState();
		}
	}
	
	bool isPressed(unsigned char i) const {
		return m_btnArr[i%8].isPressed();
	}

	bool wasPressed(unsigned char i) const {
		return m_btnArr[i%8].wasPressed();
	}
	
	bool wasReleased(unsigned char i) const {
		return m_btnArr[i%8].wasReleased();
	}
};

#endif 

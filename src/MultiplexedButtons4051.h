#ifndef MULTIPLEXED_BTNS_H
#define MULTIPLEXED_BTNS_H

#include <Multiplexer4051.h>
#include <Button.h>

class MultiplexedButtons4051
{
	Button m_btnArr[8];
	unsigned char m_addr0, m_addr1, m_addr2, m_cmn;
	bool m_isPullUp;
	
	void setAddress(unsigned char address) const {
		digitalWrite(m_addr2, address & 0x4);
		digitalWrite(m_addr1, address & 0x2);
		digitalWrite(m_addr0, address & 0x1);
	}

 public:
	MultiplexedButtons4051(){}
 
	MultiplexedButtons4051(unsigned char addr0, unsigned char addr1, unsigned char addr2,
			       unsigned char cmn, bool isPullUp, bool toggleMode[], unsigned char debounce)
				: m_addr0(addr0),
			      m_addr1(addr1),
			      m_addr2(addr2),
				  m_cmn(cmn),
				  m_isPullUp(isPullUp)
	{
		pinMode(m_addr0, OUTPUT);
		pinMode(m_addr1, OUTPUT);
		pinMode(m_addr2, OUTPUT);
		pinMode(cmn, m_isPullUp ? INPUT_PULLUP : INPUT);
		
		for(unsigned char i = 0; i < 8; i++) {
			m_btnArr[i] = Button(cmn, debounce, isPullUp, toggleMode[i]);
		}
	}
	
	void updateState() {
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

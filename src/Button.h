#ifndef BUTTON_H
#define BUTTON_H
#include <Arduino.h>

class Button
{
	const unsigned char m_pin;
	const unsigned char m_debounce;
	unsigned long m_lastPressTime = 0;
	bool m_curState = false;
	bool m_toggleState = false;
	bool m_hadStateChange = false;
	const bool m_isPullUp;
	const bool m_simToggle;
	
public:

	Button(unsigned char pin, unsigned char debounce, bool isPullUp, bool simToggle)
			: m_pin(pin),
			  m_debounce(debounce),
			  m_isPullUp(isPullUp),
			  m_simToggle(simToggle) {
				  
		if (m_isPullUp)
			pinMode(m_pin, INPUT_PULLUP);
		else
			pinMode(m_pin, INPUT);
	}

	void updateState() {
		bool curPressed = digitalRead(m_pin);
		if (m_isPullUp)
			curPressed = !curPressed;

		unsigned long curTime = millis();
		m_hadStateChange = false;

		if ((!curPressed && m_curState) ||
			(curPressed && !m_curState && curTime - m_lastPressTime >= m_debounce)) {
			
			m_curState = curPressed;
			m_lastPressTime = curTime;
			m_hadStateChange = true;
			
			if (m_simToggle && curPressed)
				m_toggleState = !m_toggleState;
		}
		
		
	}

	bool isPressed() const {
		if (m_simToggle)
			return m_toggleState;
		else
			return m_curState;
	}

	bool wasPressed() const {
		return m_hadStateChange && m_curState;
	}
	
	bool wasReleased() const {
		return m_hadStateChange && !m_curState;
	}
};

#endif
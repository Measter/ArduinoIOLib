#ifndef SHIFTREGISTER_H
#define SHIFTREGISTER_H
#include <Arduino.h>

class ShiftRegister595
{
	unsigned char m_dataPin, m_clockPin, m_latchPin;
	bool m_msbFirst;

	void shiftValues() {}

	template <typename... Targs>
	void shiftValues(unsigned char curVal, Targs... vals) {
		shiftOut(m_dataPin, m_clockPin, m_msbFirst, curVal);

		shiftValues(vals...);
	}
public:

	ShiftRegister595(unsigned char dataPin, unsigned char clockPin, unsigned char latchPin, bool msbFirst) 
			: m_dataPin(dataPin),
			  m_clockPin(clockPin),
			  m_latchPin(latchPin),
			  m_msbFirst(msbFirst) {
		pinMode(m_dataPin, OUTPUT);
		pinMode(m_clockPin, OUTPUT);
		pinMode(m_latchPin, OUTPUT);
	}

	template<typename... Targs>
	void write(Targs... vals) const {
		digitalWrite(m_latchPin, LOW);
		digitalWrite(m_clockPin, LOW);
		shiftValues(vals...);
		digitalWrite(m_latchPin, HIGH);
	}

};

#endif
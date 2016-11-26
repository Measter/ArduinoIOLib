#ifndef MULTIPLEXER_H
#define MULTIPLEXER_H
#include <Arduino.h>

template <unsigned char count>
class Multiplexer4051
{
	template<bool B, class T, class F>
	struct conditional { typedef T type; };

	template<class T, class F>
	struct conditional<false, T, F> { typedef F type; };

	using ValueSize = typename conditional<count == 1, unsigned char, typename conditional<count == 2, unsigned int, unsigned long>::type>::type;

	unsigned char m_addr0, m_addr1, m_addr2;
	bool m_isOutput, m_isPullUp;
	unsigned char m_commonPins[count];

	void setAddress(unsigned char address) const {
		digitalWrite(m_addr2, address & 0x4);
		digitalWrite(m_addr1, address & 0x2);
		digitalWrite(m_addr0, address & 0x1);
	}

	void setupPins(unsigned char i) {
		return;
	}
	template <typename... Targs>
	void setupPins(unsigned char i, unsigned char curPin, Targs... pins) {
		if (i == count)
			return;

		m_commonPins[i] = curPin;

		if (m_isOutput)
			pinMode(curPin, OUTPUT);
		else
			pinMode(curPin, m_isPullUp ? INPUT_PULLUP : INPUT);

		setupPins(i + 1, pins...);
	}
public:
	Multiplexer4051(){}
	
	template <typename... Targs>
	Multiplexer4051(unsigned char addr0, unsigned char addr1, unsigned char addr2, bool isOutput, bool isPullUp, Targs...pins)
			: m_addr0(addr0),
			  m_addr1(addr1),
			  m_addr2(addr2),
			  m_isOutput(isOutput),
			  m_isPullUp(isPullUp) {

		static_assert(count > 0 && count <= 4, "Multiplexer: Common pin count must be between 1 and 4");

		pinMode(m_addr0, OUTPUT);
		pinMode(m_addr1, OUTPUT);
		pinMode(m_addr2, OUTPUT);

		setupPins(0, pins...);
	}

	ValueSize read() const {
		if (m_isOutput)
			return 0;

		ValueSize input = 0;

		for (unsigned char mp = 0; mp < count; mp++)
		{
			for (unsigned char i = 7; i < 8; --i) {
				setAddress(i);
				input <<= 1;
				input |= digitalRead(m_commonPins[mp]);
			}
		}

		return m_isPullUp ? ~input : input;
	}

	void write(ValueSize value) const {
		if (!m_isOutput)
			return;
		for (unsigned char i = 0; i < NumBits(); i++) {
			setAddress(i);
			digitalWrite(m_commonPins[0], value & 1);
			value >>= 1;
		}
	}
};

#endif
#include <Arduino.h>
#include "Button.h"
#include "Multiplexer4051.h"

// On these two interfaces, each is read individually.
// The constructor's first three values are the address pins.
// The next value is whether it's input or output.
// The 5th input is whether the input is pull-up or not.
// And the final input is common data pin.
// The integer used in the template is how many multiplexers are connected.
Multiplexer4051<1> right = Multiplexer4051<1>(2, 3, 4, false, true, 6);
Multiplexer4051<1> left = Multiplexer4051<1>(2, 3, 4, false, true, 5);

// This only uses two multiplexers. 
// On this first one, both are combined into a single interface
// for reading as one.
// Because of that, the value passed into the template is 2, and the common
// pins are just listed as the final arguments.
Multiplexer4051<2> both = Multiplexer4051<2>(2, 3, 4, false, true, 5, 6);

// Setting up a button in momentary pullup mode.
Button btn = Button(7, 10, true, false);

void setup()
{
	Serial.begin(9600);
	Serial.println("Beginning Input");
}

void loop()
{
	btn.updateState();

	if (btn.wasPressed()) {
		Serial.print("Left: ");
		Serial.println(left.read(), BIN);
		Serial.print("Right: ");
		Serial.println(right.read(), BIN);
		Serial.print("Both: ");
		Serial.println(both.read(), BIN);
	}

	delay(10);
}
#include <Arduino.h>
#include "MultiplexedButtons4051.h"

MultiplexedButtons4051 btns;

void setup()
{
	bool modes[8] = {false, false, false, false, false, false, false, false};
	btns = MultiplexedButtons4051(2,3,4, 7, true, modes, 10);
	
	Serial.begin(9600);
	Serial.println("Beginning Input");
}

void loop()
{
	btns.updateState();

	for (unsigned char i = 0; i < 8; i++) {
    if (btns.wasPressed(i)){
      Serial.print("Button pressed: ");
      Serial.println(i);
    }
	}

	delay(10);
}
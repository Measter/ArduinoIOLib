#include <Arduino.h>
#include "Button.h"

// Setting up a button in momentary pullup mode, with a 10ms debounce.
Button btnA = Button(7, 10, true, false);
// Setting up a button in toggle pullup mode.
Button btnB = Button(8, 10, true, true);

#define LED_A_PIN 4
#define LED_B_PIN 5
void setup()
{
	Serial.begin(9600);
	
	pinMode(LED_A_PIN, OUTPUT);
	pinMode(LED_B_PIN, OUTPUT);
}

void loop()
{
	btnA.updateState();
	btnB.updateState();
	
	// This LED will only be lit while the button is pressed.
	if (btnA.isPressed()){
		digitalWrite(LED_A_PIN, HIGH);
	} else {
		digitalWrite(LED_A_PIN, LOW);
	}

	// On the other hand, this LED will be toggled on and off.
	if (btnB.isPressed()){
		digitalWrite(LED_B_PIN, HIGH);
	} else {
		digitalWrite(LED_B_PIN, LOW);
	}
	
	Serial.print("BtnA IsPressed: ");
	// IsPressed will return the current state.
	Serial.print(btnA.isPressed());
	Serial.print(" WasPressed: ");
	
	// WasPressed will return whether the button was pressed
	// during this update.
	Serial.print(btnA.wasPressed());
	Serial.print(" WasReleased: ");
	
	// WasReleased will return whether the button was released
	// during this update.
	Serial.println(btnA.wasReleased());
	
	Serial.print("BtnB IsPressed: ");
	Serial.print(btnB.isPressed());
	Serial.print(" WasPressed: ");
	Serial.print(btnB.wasPressed());
	Serial.print(" WasReleased: ");
	Serial.println(btnB.wasReleased());

	delay(10);
}
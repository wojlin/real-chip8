#include "chip8-buzzer.h"

// Static variable to store the buzzer pin number
static uint8_t buzzer_pin;

// Initialize the buzzer with the specified pin
void chip8_buzzer_init(uint8_t b_pin) {
    buzzer_pin = b_pin;

    // Initialize the pin as OUTPUT
    pinMode(buzzer_pin, OUTPUT);
}

// Turn on the buzzer
void chip8_buzzer_on() {
    digitalWrite(buzzer_pin, HIGH);
}

// Turn off the buzzer
void chip8_buzzer_off() {
    digitalWrite(buzzer_pin, LOW);
}

#include "chip8-leds.h"

// Static variables to store pin numbers
static uint8_t power_pin;
static uint8_t signal_pin;

// Initialize the LED control with specified pins for Power and Signal LEDs
void led_control_init(uint8_t p_pin, uint8_t s_pin) {
    power_pin = p_pin;
    signal_pin = s_pin;

    // Initialize the pins as OUTPUT
    pinMode(power_pin, OUTPUT);
    pinMode(signal_pin, OUTPUT);
}

// Turn on the Power LED
void led_control_power_led_on() {
    analogWrite(power_pin, 130);
}

// Turn off the Power LED
void led_control_power_led_off() {
    analogWrite(power_pin, 0);
}

// Turn on the Signal LED
void led_control_signal_led_on() {
    analogWrite(signal_pin, 130);
}

// Turn off the Signal LED
void led_control_signal_led_off() {
    analogWrite(signal_pin, 0);
}

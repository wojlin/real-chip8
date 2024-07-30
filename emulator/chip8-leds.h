#ifndef CHIP8_LEDS_H
#define CHIP8_LEDS_H

#include <Arduino.h>

#ifdef __cplusplus
extern "C"{
#endif

// Documentation for the library
/**
 * @file chip8-leds.h
 * @brief A simple library to control two LEDs: Power LED and Signal LED.
 */

// Function prototypes

/**
 * @brief Initialize the LED control with specified pins for Power and Signal LEDs.
 * 
 * @param power_pin The Arduino pin number connected to the Power LED
 * @param signal_pin The Arduino pin number connected to the Signal LED
 */
void led_control_init(uint8_t power_pin, uint8_t signal_pin);

/**
 * @brief Turn on the Power LED.
 */
void led_control_power_led_on();

/**
 * @brief Turn off the Power LED.
 */
void led_control_power_led_off();

/**
 * @brief Turn on the Signal LED.
 */
void led_control_signal_led_on();

/**
 * @brief Turn off the Signal LED.
 */
void led_control_signal_led_off();

#ifdef __cplusplus
} // extern "C"
#endif

#endif // CHIP8_LEDS_H

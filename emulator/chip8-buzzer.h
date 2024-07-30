#ifndef CHIP8_BUZZER_H
#define CHIP8_BUZZER_H

#include <Arduino.h>


#ifdef __cplusplus
extern "C"{
#endif

// Documentation for the library
/**
 * @file chip8-buzzer.h
 * @brief A simple library to control a buzzer.
 */

// Function prototypes

/**
 * @brief Initialize the buzzer with the specified pin.
 * 
 * @param buzzer_pin The Arduino pin number connected to the buzzer
 */
void chip8_buzzer_init(uint8_t buzzer_pin);

/**
 * @brief Turn on the buzzer.
 */
void chip8_buzzer_on();

/**
 * @brief Turn off the buzzer.
 */
void chip8_buzzer_off();

#ifdef __cplusplus
} // extern "C"
#endif

#endif // CHIP8_BUZZER_H

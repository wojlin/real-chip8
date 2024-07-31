#ifndef CHIP8_KEYPAD_H
#define CHIP8_KEYPAD_H

#include <Arduino.h>

#ifdef __cplusplus
extern "C"{
#endif

/**
 * @brief Initialize the keypad with the given pin configurations.
 * 
 * @param input_pins Array of input pin numbers (columns).
 * @param output_pins Array of output pin numbers (rows).
 * @param num_inputs Number of input pins (columns).
 * @param num_outputs Number of output pins (rows).
 */
void keypad_init(const uint8_t* input_pins, const uint8_t* output_pins, uint8_t num_inputs, uint8_t num_outputs);

/**
 * @brief Wait for a key press and return the key value.
 * 
 * @return int The key value (0-15).
 */
int wait_for_key();

/**
 * @brief Wait for a key press release.
 * 
 * @return int -1
 */
int wait_for_release();

/**
 * @brief Read the currently pressed key without blocking.
 * 
 * @return int The key value (0-15) or -1 if no key is pressed.
 */
int read_key();

/**
 * @brief Convert a hexadecimal value to the corresponding character.
 * 
 * @param hex_value The hexadecimal value (0-15).
 * @return char The corresponding character ('0'-'9', 'A'-'F').
 */
char hex_to_char(int hex_value);

#ifdef __cplusplus
} // extern "C"
#endif

#endif

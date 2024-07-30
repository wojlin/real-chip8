#include "chip8-keypad.h"

// Static variables to hold the pin configurations and keymap
static const uint8_t* input_pins;
static const uint8_t* output_pins;
static uint8_t num_inputs;
static uint8_t num_outputs;
static uint8_t keymap[4][4] = {
    {0x1, 0x2, 0x3, 0xC},
    {0x4, 0x5, 0x6, 0xD},
    {0x7, 0x8, 0x9, 0xE},
    {0xA, 0x0, 0xB, 0xF}
};

void keypad_init(const uint8_t* in_pins, const uint8_t* out_pins, uint8_t num_in, uint8_t num_out) {
    input_pins = in_pins;
    output_pins = out_pins;
    num_inputs = num_in;
    num_outputs = num_out;
    
    // Set output pins as OUTPUT
    for (int i = 0; i < num_outputs; i++) {
        pinMode(output_pins[i], OUTPUT);
    }
    
    // Set input pins as INPUT
    for (int i = 0; i < num_inputs; i++) {
        pinMode(input_pins[i], INPUT);
    }
}

int read_key() {
    // Loop through each output pin (row)
    for (int row = 0; row < num_outputs; row++) {
        // Set the current output pin to HIGH and others to LOW
        for (int i = 0; i < num_outputs; i++) {
            digitalWrite(output_pins[i], (i == row) ? HIGH : LOW);
        }
        
        // Read the input pins (columns)
        for (int col = 0; col < num_inputs; col++) {
            if (digitalRead(input_pins[col]) == HIGH) {
                // Return the detected key as an integer
                return keymap[row][col];
            }
        }
        
        // Short delay to debounce
        delay(10); // Adjust delay as needed
    }
    
    // Return -1 if no key is pressed
    return -1;
}

int wait_for_key() {
    int key = -1;
    // Loop until a key press is detected
    while (key == -1) {
        key = read_key();
    }
    return key;
}

char hex_to_char(int hex_value) {
    // Convert hex value (0-15) to corresponding character
    if (hex_value >= 0 && hex_value <= 9) {
        return '0' + hex_value;
    } else if (hex_value >= 10 && hex_value <= 15) {
        return 'A' + (hex_value - 10);
    }
    // Return '?' if the value is out of range (should not happen)
    return '?';
}

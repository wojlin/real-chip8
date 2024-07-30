#include <Arduino.h>
#include <SPI.h>

#include "chip8-keypad.h"
#include "chip8-leds.h"
#include "chip8-buzzer.h"
#include "chip8-reader.h"

const uint8_t chip_select_reader_pin = 10;
const uint8_t buzzer_pin = A0;
const uint8_t power_led_pin = A1;
const uint8_t signal_led_pin = A2;
const uint8_t input_pins[] = {2, 3, 4, 5}; // Input pins (columns)
const uint8_t output_pins[] = {6, 7, 8, 9}; // Output pins (rows)
const uint8_t num_inputs = sizeof(input_pins) / sizeof(input_pins[0]); // Number of input pins (columns)
const uint8_t num_outputs = sizeof(output_pins) / sizeof(output_pins[0]); // Number of output pins (rows)


void print_file_list(char** file_list, size_t file_count) {
    Serial.println("Files on SD card:");
    for (size_t i = 0; i < file_count; i++) {
        Serial.println(file_list[i]);
        free(file_list[i]);  // Free the memory for each filename
    }
    free(file_list);  // Free the memory for the file list array
}

void setup() {
    // Initialize serial communication for printing
    Serial.begin(9600);

    // Initialize the sd card reader
    chip8_reader_begin(10);

    // Initialize the keypad
    keypad_init(input_pins, output_pins, num_inputs, num_outputs);

    // Initialize the LED control
    led_control_init(power_led_pin, signal_led_pin);

    // Initialize the buzzer
    chip8_buzzer_init(buzzer_pin);

    // Turn on the Power LED
    led_control_power_led_on();
}

void loop() {

    bool sd_plugged = false;
    bool file_selected = false;
    while(!sd_plugged)
    {
      if (chip8_reader_is_card_present()) {
        Serial.println("SD card is present.");
        led_control_signal_led_on();

        size_t file_count;
        char** file_list = chip8_reader_list_files(&file_count);
        if (file_list) 
        {

            print_file_list(file_list, file_count);
            sd_plugged = true;
            break;
        }
        else 
        {
            Serial.println("Failed to list files or no files found.");
        }
        

      }else
      {
          led_control_signal_led_off();
      }
    }

    while(true)
    {
      // Wait for a key press and get the key value
      int key = wait_for_key();
      chip8_buzzer_on();
      delay(50);
      chip8_buzzer_off();
      // Convert the key value to a character
      char key_char = hex_to_char(key);
      // Print the key character to the Serial monitor
      Serial.print("Key ");
      Serial.print(key_char);
      Serial.println(" is pressed.");
    }
    

    

}

#include <Arduino.h>
#include <SPI.h>
#include "chip8-keypad.h"
#include "chip8-leds.h"
#include "chip8-buzzer.h"
#include "chip8-reader.h"
#include "chip8.h"

const uint8_t chip_select_reader_pin = 10;
const uint8_t buzzer_pin = A0;
const uint8_t power_led_pin = A1;
const uint8_t signal_led_pin = A2;
const uint8_t input_pins[] = {2, 3, 4, 5}; // Input pins (columns)
const uint8_t output_pins[] = {6, 7, 8, 9}; // Output pins (rows)
const uint8_t num_inputs = sizeof(input_pins) / sizeof(input_pins[0]); // Number of input pins (columns)
const uint8_t num_outputs = sizeof(output_pins) / sizeof(output_pins[0]); // Number of output pins (rows)

Chip8 chip8;

size_t file_count = 0;
char** file_list = nullptr;

bool sd_plugged = false;
bool file_selected = false;
bool game_finished = false;

int current_file = 0;
int files_in_panel = 4; // Number of filenames that fit into the screen

uint8_t program[PROGRAM_MEMORY_SIZE];
size_t program_size;


int freeMemory() {
  extern int __heap_start, *__brkval;
  int v;
  return (int)&v - (__brkval == 0 ? (int)&__heap_start : (int)__brkval);
}

String get_selection(int current_file, int files_in_panel, size_t file_count, char** file_list) {
  Serial.println("#################################");

  String data = "";
  for (uint8_t i = 0; i < files_in_panel; i++) {
    if (current_file + i < file_count) {
      
      data.concat((i == 0) ? ">" : " ");
      data.concat(file_list[current_file + i]);
      data.concat("\n");
    } else {
      data.concat("\n");
    }
  }
  Serial.print(data);
  Serial.println("#################################");
  return data;
}

void print_file_list(char** file_list, size_t file_count) {
  Serial.println("Files on SD card:");
  for (size_t i = 0; i < file_count; i++) {
    Serial.println(file_list[i]);
  }
}

void setup() {
  // Initialize serial communication for printing
  Serial.begin(9600);

  // Initialize the SD card reader
  chip8_reader_begin(chip_select_reader_pin);

  // Initialize the keypad
  keypad_init(input_pins, output_pins, num_inputs, num_outputs);

  // Initialize the LED control
  led_control_init(power_led_pin, signal_led_pin);

  // Initialize the buzzer
  chip8_buzzer_init(buzzer_pin);

  // Turn on the Power LED
  led_control_power_led_on();

  Serial.println("Initialization successful!");
}

void loop() {
  Serial.println("Waiting for SD card...");

  // phase 1: wait for sd card plug in 
  while (!sd_plugged) {
    if (chip8_reader_is_card_present()) {
      Serial.println("SD card is present.");
      led_control_signal_led_on();

      file_list = chip8_reader_list_files(&file_count);
      if (file_list) {
        print_file_list(file_list, file_count);
        sd_plugged = true;
      } else {
        Serial.println("Failed to list files or no files found.");
      }
    } else {
      led_control_signal_led_off();
    }
  }
  
  // phase 2: select file from menu
  while (!file_selected) {
    String data = get_selection(current_file, files_in_panel, file_count, file_list);
    //Serial.print(data);
    // TODO display the slection menu

    int key = wait_for_key();
    wait_for_release();
    chip8_buzzer_on();
    delay(50);
    chip8_buzzer_off();

    if (key == 2) { // up key
      current_file--;
      if (current_file < 0) {
        current_file = 0;
      }
    } else if (key == 8) { // down key
      current_file++;
      if (current_file >= file_count) {
        current_file = file_count - 1;
      }
    } else if (key == 5) { // select key
      file_selected = true;
    }
  }

  Serial.println("Selected file: " + String(file_list[current_file]));

  Serial.print("Free memory: ");
  Serial.print(freeMemory());
  Serial.println(" bytes");

  // phase 3: load program into chip8 memory
  chip8_init(&chip8);
  if (chip8_reader_read_file(file_list[current_file], program, PROGRAM_MEMORY_SIZE, &program_size)) {
        chip8_load_ram(&chip8, program, program_size);
        Serial.println("Program loaded into RAM successfully.");
    } else {
        Serial.println("Failed to load program.");
    }
  

  // phase 4: game loop
  while(!game_finished)
  {
    if(chip8.sound_timer > 0)
    {
      chip8_buzzer_on();
    }
    else
    {
      chip8_buzzer_off();
    }

    // TODO implement display

    Opcode opcode = chip8_fetch_opcode(&chip8);
    int result = chip8_execute_opcode(&chip8, &opcode);
    if(result)
    {
      game_finished = true;
      break;
    }
  }
  
  //phase 5: cleanup memeory and prepere fot the next loop
  if (file_list) 
  {
      for (size_t i = 0; i < file_count; i++) 
      {
        free(file_list[i]);
      }
      free(file_list);
  }

}

#include <Arduino.h>
#include <SPI.h>
#include "chip8-keypad.h"
#include "chip8-leds.h"
#include "chip8-buzzer.h"
#include "chip8.h"

#include "RGBmatrixPanel.h"
#include "Adafruit_GFX.h"
#include "bit_bmp.h"
#include "fonts.h"

#include <string.h>
#include <stdlib.h>

#include "Fonts/Picopixel.h"

#define CLK 11 
#define OE   9
#define LAT 10
#define A   A0
#define B   A1
#define C   A2
#define D   A3

RGBmatrixPanel matrix(A, B, C, D, CLK, LAT, OE, false, 64);
GFXcanvas1 canvas(64, 32);

//uint16_t high = matrix.ColorHSV(0,0,0,false);
//uint16_t low = matrix.ColorHSV(1100,155,80,false);

uint16_t high = matrix.ColorHSV(0,0,40,false);
uint16_t low = matrix.ColorHSV(0,0,0,false);

const uint8_t chip_select_reader_pin = 53;
const uint8_t buzzer_pin = 32;
const uint8_t power_led_pin = 34;
const uint8_t signal_led_pin = 35;
const uint8_t input_pins[] = {42, 43, 44, 45}; // Input pins (columns)
const uint8_t output_pins[] = {48, 49, 46, 47}; // Output pins (rows)
const uint8_t num_inputs = sizeof(input_pins) / sizeof(input_pins[0]); // Number of input pins (columns)
const uint8_t num_outputs = sizeof(output_pins) / sizeof(output_pins[0]); // Number of output pins (rows)



uint8_t file_count = 0;
const int BUFFER_SIZE = 200; // Size of the buffer to store received data
char received_data[BUFFER_SIZE]; // Buffer to store received data

#define MAX_FILES 20   // Maximum number of files
#define MAX_FILENAME_LENGTH 50  // Maximum length of each filename

char file_array[MAX_FILES][MAX_FILENAME_LENGTH]; // Adjust the size of the array according to the expected number and size of files


bool sd_plugged = false;
bool file_selected = false;
bool game_finished = false;

int current_file = 0;
int files_in_panel = 4; // Number of filenames that fit into the screen
int program_size;

void(* resetFunc) (void) = 0; //declare reset function @ address 0

void screen_setup()
{
  matrix.begin();
}


void screen_clear()
{
  matrix.fillRect(0, 0, matrix.width(), matrix.height(), low);
}


void draw_title()
{
  screen_clear();
  matrix.setFont(NULL);
  screen_clear();
  matrix.setTextSize(2);     
  matrix.setTextWrap(false); 
  matrix.setCursor(4, 10);   
  matrix.setTextColor(high);
  matrix.println("CHIP8");
}

void draw_pixel(uint8_t x, uint8_t y, uint8_t state)
{
  uint16_t color = (state == 1) ? high : low;
	matrix.drawPixel(x, y, color);
}

void draw_text(String data, uint8_t x, uint8_t y, uint8_t size, const GFXfont *f)
{
    matrix.setFont(f);
	  matrix.setTextSize(size);     
  	matrix.setTextWrap(false); 
  	matrix.setCursor(x, y);   
  	matrix.setTextColor(high);
  	matrix.println(data);
}

void draw_chip8_display(Chip8 *chip8)
{
  for (int y = 0; y < DISPLAY_HEIGHT; ++y) 
  {
        for (int x = 0; x < DISPLAY_WIDTH; ++x) 
        {
          uint8_t state = (chip8->display[y] >> (DISPLAY_WIDTH - 1 - x)) & 1;
          //Serial.print(state);
          canvas.drawPixel(x,y,state);
        }
        //Serial.println("");
    }

  matrix.drawBitmap(0, 0, canvas.getBuffer(),
  canvas.width(), canvas.height(), 0xFFFF, 0x0000);
}

int freeMemory() {
  extern int __heap_start, *__brkval;
  int v;
  return (int)&v - (__brkval == 0 ? (int)&__heap_start : (int)__brkval);
}

void draw_selection() {
  Serial.println("#################################");

  screen_clear();

  for (uint8_t i = 0; i < files_in_panel; i++) 
  {
    String line = "";
    if (current_file + i < file_count) {
      line += (i == 0) ? ">" : " ";
      line += String(file_array[current_file + i]);
    }
    Serial.println(line);
    draw_text(line, 2, (i+1)*7,1, &Picopixel);
  }
  Serial.println("#################################");
}

void print_file_list(char** file_list, size_t file_count) {
  Serial.println("Files on SD card:");
  for (size_t i = 0; i < file_count; i++) {
    Serial.println(file_list[i]);
  }
}

void setup() {
  delay(1000);

  // Initialize serial communication for printing
  Serial.begin(9600);
  Serial3.begin(9600);

  // Initialize the keypad
  keypad_init(input_pins, output_pins, num_inputs, num_outputs);

  // Initialize the LED control
  led_control_init(power_led_pin, signal_led_pin);

  // Initialize the buzzer
  chip8_buzzer_init(buzzer_pin);

  // Turn on the Power LED
  led_control_power_led_on();
  
  screen_setup();
  screen_clear();
  draw_title();

  Serial.println("Initialization successful!");
}


void trim(char* str) {
  char* end;

  // Trim leading space
  while (isspace((unsigned char)*str)) str++;

  if (*str == 0) // All spaces?
    return;

  // Trim trailing space
  end = str + strlen(str) - 1;
  while (end > str && isspace((unsigned char)*end)) end--;

  // Null terminate
  *(end + 1) = '\0';
}

// Function to send the "status command" and wait until "plugged" response is received
void handle_status_command() {
  bool status_received = false;
  
  while (!status_received) {
    // Send "status command" to Serial3
    send_command("status");
    
    if (read_serial_response()) {
      trim(received_data);
      if (strcmp(received_data, "plugged") == 0) {
        // Set LED to HIGH state and set status_received to true
        digitalWrite(signal_led_pin, HIGH);
        status_received = true;
      } 
      else if (strcmp(received_data, "unplugged") == 0) {
        // Set LED to LOW state
        digitalWrite(signal_led_pin, LOW);
      }
      Serial.println(received_data);
    }

    delay(1000);
  }
}

// Function to send a command via Serial3
void send_command(const char* command) {
  Serial3.println(command);
}

// Function to read the response from Serial3
bool read_serial_response() {
  static char buffer[BUFFER_SIZE];
  static int buffer_index = 0;
  bool response_complete = false;

  while (Serial3.available()) {
    char incoming_byte = Serial3.read();

    // Check for end of response (newline character)
    if (incoming_byte == '\n') {
      buffer[buffer_index] = '\0'; // Null-terminate the buffer
      // Copy buffer to received_data if it's not empty
      if (buffer_index > 0) {
        strlcpy(received_data, buffer, BUFFER_SIZE);
        buffer_index = 0; // Reset buffer index for next response
        response_complete = true;
      }
      break;
    }

    // Accumulate data into buffer
    if (buffer_index < BUFFER_SIZE - 1) {
      buffer[buffer_index++] = incoming_byte;
    } else {
      // Buffer overflow protection
      buffer_index = 0;
    }
  }

  return response_complete;
}

// Function to handle the "list" command
void handle_list_command() {

  bool list_received = false;
  memset(received_data, 0, BUFFER_SIZE);

  send_command("list");
  while (!list_received) {
    
    delay(10);
    if (read_serial_response()) 
    {
      trim(received_data); // Clean up the received data

      int found = 0;

      for (int i = 0; i < BUFFER_SIZE; i++) {
          if (received_data[i] == '!') {
              found = 1;
              break;
          }
          if (received_data[i] == '\0') {
              break;
          }
      }

      if(found)
      {
        Serial.println(received_data);
        Serial.println("___________________________");
        // Parse the file list
        parse_file_list(received_data);
      
        // Print the file list to Serial Monitor for debugging
        print_file_list();
        return;
      }

      
    
    }
    delay(10);
  }
}

void parse_file_list(const char* list) {
  int char_index = 0;

  // Reset file array and file_count
  for (int i = 0; i < MAX_FILES; i++) {
    file_array[i][0] = '\0'; // Clear all file names
  }
  file_count = 0;

  // Iterate through the list and split filenames based on delimiter '!'
  for (int i = 0; list[i] != '\0'; i++) {
    if (list[i] == '!') {
      if (char_index > 0) {
        file_array[file_count][char_index] = '\0'; // Null-terminate the current filename
        file_count++;

        // Ensure we do not exceed the maximum number of files
        if (file_count >= MAX_FILES) {
          break;
        }

        char_index = 0; // Reset index for the next filename
      }
    } else {
      // Add character to the filename if there's space
      if (char_index < MAX_FILENAME_LENGTH - 1) {
        file_array[file_count][char_index++] = list[i];
      } else {
        // Handle potential filename overflow
        file_array[file_count][MAX_FILENAME_LENGTH - 1] = '\0'; // Null-terminate
        file_count++;

        // Ensure we do not exceed the maximum number of files
        if (file_count >= MAX_FILES) {
          break;
        }

        char_index = 0; // Reset index for the next filename
      }
    }
  }

  // Handle the last filename
  if (char_index > 0 && file_count < MAX_FILES) {
    file_array[file_count][char_index] = '\0';
    file_count++;
  }
}

// Function to print the file list for debugging
void print_file_list() {
  int i = 0;
  while (file_array[i][0] != '\0') {
    Serial.println(file_array[i]);
    i++;
  }
}

int get_file_length(const char* filename) {
  char command[20];
  snprintf(command, sizeof(command), "len!%s", filename);

  send_command(command);
  Serial.print("Requesting length of: ");
  Serial.println(filename);

  // Wait and read the response
  while (!read_serial_response()) {
    delay(100); // Wait until we get the response
  }

  trim(received_data); // Clean up the received data
  int file_length = atoi(received_data); // Convert to integer

  Serial.print("Length of ");
  Serial.print(filename);
  Serial.print(": ");
  Serial.println(file_length);

  return file_length;
}

void fetch_file_into_chip8_ram(const char* filename, Chip8 *chip8) {
  int file_length = get_file_length(filename);
  
  for (int byte_no = 0; byte_no < file_length; byte_no++) {
    char command[30];
    snprintf(command, sizeof(command), "fetch!%s!%d", filename, byte_no);

    send_command(command);
    Serial.print("Fetching byte ");
    Serial.print(byte_no);
    Serial.println("...");

    // Wait and read the response
    while (!read_serial_response()) {
      delay(100); // Wait until we get the response
    }

    trim(received_data); // Clean up the received data
    int number = strtol(received_data, NULL, 16);
    chip8->ram[MEMORY_READ_START + byte_no] = number; // Store byte in CHIP-8 RAM
    
    Serial.print("Fetched byte: ");
    Serial.println(received_data); // Print for debugging
    Serial.println("chip8: " + String(MEMORY_READ_START + byte_no) + " " + chip8->ram[MEMORY_READ_START + byte_no]);
  }

  Serial.println("File loaded into CHIP-8 RAM.");
}

void loop() {
  Chip8 chip8;
  chip8_init(&chip8);
  Serial.println("Waiting for SD card...");

  // phase 1: wait for sd card plug in 
  handle_status_command();
  delay(1500);
  handle_list_command();
  
  Serial.println("Waiting for ROM selection...");

  Serial.println(file_count);
  // phase 2: select file from menu
  while (!file_selected) {
    draw_selection();

    int key = wait_for_key();
    Serial.println(key);
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

  

  String selected_file = String(file_array[current_file]);
  Serial.println("Selected file: " + selected_file);

  screen_clear();
  draw_text("LOADING...", 2, 12, 1, NULL);

  // phase 3: load program into chip8 memory
  
  fetch_file_into_chip8_ram(selected_file.c_str(), &chip8);

  // phase 4: game loop
  Serial.println("launching game!");
  chip8.display_changed = 1;
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

    if(chip8.display_changed)
    {
      //Serial.println("drawing display...");
      draw_chip8_display(&chip8);
      chip8.display_changed = 0;
    }
    

    Opcode opcode = chip8_fetch_opcode(&chip8);
    uint16_t number = opcode.instruction;
    //Serial.println("0x" + String((number >> 12) & 0xF, HEX) + String((number >> 8) & 0xF, HEX) + String((number >> 4) & 0xF, HEX) + String(number & 0xF, HEX));



    int result = chip8_execute_opcode(&chip8, &opcode);
    if(result)
    {
      game_finished = true;
      break;
    }
  }
  Serial.println("game end!");
  delay(1000);
  //phase 5: cleanup memeory and prepere fot the next loop
  resetFunc(); 
}
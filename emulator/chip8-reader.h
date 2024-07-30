#ifndef CHIP8_READER_H
#define CHIP8_READER_H

#include <Arduino.h>
#include <SD.h>

#define MAX_FILES 100
#define MAX_FILENAME_LEN 50

/**
 * Initializes the SD card.
 * @param cs_pin Chip select pin for the SD card.
 * @return True if initialization is successful, otherwise false.
 */
bool chip8_reader_begin(uint8_t cs_pin);

/**
 * Lists all files on the SD card and returns an array of filenames.
 * @param file_count Pointer to a size_t variable to store the number of files found.
 * @return Pointer to an array of filenames. Returns NULL if the directory could not be opened.
 */
char** chip8_reader_list_files(size_t* file_count);

/**
 * Checks if the SD card is present.
 * @return True if the SD card is present, otherwise false.
 */
bool chip8_reader_is_card_present();

/**
 * Reads binary content of a file.
 * @param filename Name of the file to read.
 * @param buffer Pointer to a buffer to store the file content.
 * @param buffer_size Size of the buffer.
 * @param bytes_read Pointer to a size_t variable to store the number of bytes read.
 * @return True if the file was successfully read, otherwise false.
 */
bool chip8_reader_read_file(const char* filename, uint8_t* buffer, size_t buffer_size, size_t* bytes_read);

#endif // CHIP8_READER_H

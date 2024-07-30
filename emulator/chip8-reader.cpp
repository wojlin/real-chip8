#include "chip8-reader.h"

static uint8_t cs_pin;

/**
 * Initializes the SD card.
 * @param cs_pin Chip select pin for the SD card.
 * @return True if initialization is successful, otherwise false.
 */
bool chip8_reader_begin(uint8_t cs_pin) {
    ::cs_pin = cs_pin;
    return SD.begin(cs_pin);
}

/**
 * Lists all files on the SD card and returns an array of filenames.
 * @param file_count Pointer to a size_t variable to store the number of files found.
 * @return Pointer to an array of filenames. Returns NULL if the directory could not be opened.
 */
char** chip8_reader_list_files(size_t* file_count) {
    File root = SD.open("/");
    if (!root) {
        Serial.println("Failed to open directory");
        *file_count = 0;
        return NULL;
    }

    // Allocate memory for the file list
    char** file_list = (char**)malloc(MAX_FILES * sizeof(char*));
    *file_count = 0;

    // Iterate through files and directories
    File file = root.openNextFile();
    while (file && *file_count < MAX_FILES) {
        if (!file.isDirectory()) {
            // Allocate memory for each filename and copy the name
            file_list[*file_count] = (char*)malloc(MAX_FILENAME_LEN);
            strncpy(file_list[*file_count], file.name(), MAX_FILENAME_LEN - 1);
            file_list[*file_count][MAX_FILENAME_LEN - 1] = '\0';  // Ensure null termination
            (*file_count)++;
        }
        file = root.openNextFile();
    }

    return file_list;
}

/**
 * Checks if the SD card is present.
 * @return True if the SD card is present, otherwise false.
 */
bool chip8_reader_is_card_present() {
    return SD.begin(cs_pin);
}

/**
 * Reads binary content of a file.
 * @param filename Name of the file to read.
 * @param buffer Pointer to a buffer to store the file content.
 * @param buffer_size Size of the buffer.
 * @param bytes_read Pointer to a size_t variable to store the number of bytes read.
 * @return True if the file was successfully read, otherwise false.
 */
bool chip8_reader_read_file(const char* filename, uint8_t* buffer, size_t buffer_size, size_t* bytes_read) {
    File file = SD.open(filename, FILE_READ);
    if (!file) {
        Serial.println("Failed to open file");
        *bytes_read = 0;
        return false;
    }

    *bytes_read = file.read(buffer, buffer_size);
    file.close();
    return true;
}
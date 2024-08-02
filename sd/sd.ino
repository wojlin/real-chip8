#include <SPI.h>
#include <SD.h>

const int chipSelect = 10; // Set your SD card chip select pin

bool sdCardInserted = false;

void setup() {
  Serial.begin(9600);
  pinMode(chipSelect, OUTPUT);

  // Initial SD card check
  sdCardInserted = SD.begin(chipSelect);
}

void loop() {
  checkSDCardStatus();  // Continuously check SD card status
  handleSerialCommands(); // Handle serial commands
}

void checkSDCardStatus() {
  // Check if SD card is inserted by attempting to initialize it
  bool currentState = SD.begin(chipSelect);
  
  // Update the state, but do not print status automatically
  sdCardInserted = currentState;
}

void handleSerialCommands() {
  if (Serial.available()) {
    String command = Serial.readStringUntil('\n');
    command.trim();

    if (command.equals("status")) {
      printSDCardStatus();
    } else if (command.equals("list")) {
      listFiles();
    } else if (command.startsWith("fetch!")) {
      fetchByteFromFile(command);
    } else if (command.startsWith("len!")) {
      getFileLength(command);
    }
  }
}

void printSDCardStatus() {
  if (sdCardInserted) {
    Serial.println("plugged");
  } else {
    Serial.println("unplugged");
  }
}

void listFiles() {
  if (!sdCardInserted) {
    Serial.println("unplugged");
    return;
  }

  File root = SD.open("/");
  if (!root) {
    Serial.println("Failed to open directory");
    return;
  }

  root.rewindDirectory();

  String fileList = "";
  File entry;
  while (entry = root.openNextFile()) {
    if (!entry.isDirectory()) {
      fileList += entry.name();
      fileList += "!";
    }
    entry.close();
  }

  if (fileList.length() > 0) {
    fileList.remove(fileList.length() - 1); // Remove the last exclamation mark
  }

  Serial.println(fileList);
  root.close();
}

void fetchByteFromFile(String command) {
  if (!sdCardInserted) {
    Serial.println("unplugged");
    return;
  }

  int firstExclamationIndex = command.indexOf('!');
  int secondExclamationIndex = command.indexOf('!', firstExclamationIndex + 1);

  if (firstExclamationIndex == -1 || secondExclamationIndex == -1) {
    Serial.println("Invalid command format");
    return;
  }

  String filename = command.substring(firstExclamationIndex + 1, secondExclamationIndex);
  String byteNoStr = command.substring(secondExclamationIndex + 1);

  int byteNo = byteNoStr.toInt();

  File file = SD.open(filename);
  if (!file) {
    Serial.println("File not found");
    return;
  }

  if (byteNo >= 0 && byteNo < file.size()) {
    file.seek(byteNo);
    uint8_t byteValue = file.read();
    Serial.print("0x");
    if (byteValue < 0x10) {
      Serial.print("0"); // Add leading zero for values < 0x10
    }
    Serial.println(byteValue, HEX);
  } else {
    Serial.println("Byte out of range");
  }

  file.close();
}

void getFileLength(String command) {
  if (!sdCardInserted) {
    Serial.println("unplugged");
    return;
  }

  int exclamationIndex = command.indexOf('!');

  if (exclamationIndex == -1) {
    Serial.println("Invalid command format");
    return;
  }

  String filename = command.substring(exclamationIndex + 1);

  File file = SD.open(filename);
  if (!file) {
    Serial.println("File not found");
    return;
  }

  Serial.println(file.size());  // Print the file size in bytes
  file.close();
}

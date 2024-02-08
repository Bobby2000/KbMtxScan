# KbMtxScan
KbMtScan is a matrix keyboard scanner for Arduino.

## Description
The library is designed to support handeling of multiple simultaneous key presses. The library is designed to work with Arduinio, in witch  case the applicatiom will need to poll the `scanMatrix()` function in a loop.

If used with the ESP32 platform, the `scanMatrix()` function will be called from a task within the library it self, and the function will not be available to the application.

## Usage
```c++
#include <Arduino.h>
#include <KbMtxScan.hpp>  

KbMtxScan kb = KbMtxScan();

const uint8_t rowCount = 4;                 // Number of keyboard rows
const uint8_t colCount = 3;                 // NUmber of keyboard columns
uint8_t rowPins[rowCount] = {13, 10, 8, 6}; // Keyboard row pins
uint8_t colPins[colCount] = {4, 2, 1};      // Keyboard columns pins
char keymap[rowCount][colCount] =
    {{'1', '2', '3'}, // Chars returned by library when corosponding key is pressed
     {'4', '5', '6'}, 
     {'7', '8', '9'},
     {'*', '0', '#'}};

void press(char key, uint8_t totalKeysPressed) {
  Serial.printf("Pressed: '%c', keys currently pressed: %d\n", key, totalKeysPressed);
}

void release(char key, uint8_t totalKeysPressed) {
  Serial.printf("Released: '%c', keys currently presssed: %d\n", key, totalKeysPressed);
} 

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);

  kb.begin(rowCount, colCount, rowPins, colPins, (char *)keymap);
  kb.setKeyPressedCallback(press);
  kb.setKeyReleasedCallback(release);
}

void loop() {
  // put your main code here, to run repeatedly:

  // kb.scanMatrix(); /* Only need when used with non ESP32 platform */
  delay(10);
}

```

# License
MIT licence

Copyright (c) 2024 - Thomas Houlberg
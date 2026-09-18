/**
 * BMT WHA103 Arduino Example Sketch
 * 
 * This example demonstrates the basic functionality of the BMT WHA103 board
 * using the Arduino-compatible library.
 * 
 * Hardware:
 * - ESP32-C3
 * - SSD1306 0.96" OLED (128x64) on I2C (GPIO 3/4, addr 0x3C)
 * - ES8311 Audio Codec on I2C (GPIO 3/4, addr 0x18)
 * - Boot button on GPIO 9
 * - I2S Audio on GPIO 5,6,7,8,10
 */

#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include "BmtWha103_Arduino.h"

BmtWha103Arduino board;

void setup() {
    // Initialize the board
    if (!board.begin()) {
        Serial.println(F("Board initialization failed!"));
        while (1) delay(1000);
    }
    
    // Set up button callbacks
    board.onButtonClick(onButtonClick);
    board.onButtonLongPress(onButtonLongPress);
    
    // Show initial message on display
    board.clearDisplay();
    board.getDisplay().setCursor(0, 0);
    board.getDisplay().println(F("BMT WHA103"));
    board.getDisplay().println(F("Arduino Ready"));
    board.getDisplay().println(F(""));
    board.getDisplay().println(F("Press button to"));
    board.getDisplay().println(F("toggle status"));
    board.updateDisplay();
    
    // Print board info to serial
    Serial.println(F("\n=== Board Info ==="));
    Serial.println(board.getBoardInfo());
    Serial.println(F("\n=== Device Status ==="));
    Serial.println(board.getDeviceStatus());
}

void loop() {
    // Update button state (handles click and long press detection)
    board.updateButton();
    
    // Update display with current status periodically
    static unsigned long lastDisplayUpdate = 0;
    if (millis() - lastDisplayUpdate > 2000) {
        lastDisplayUpdate = millis();
        updateStatusDisplay();
    }
    
    // Small delay to prevent watchdog issues
    delay(10);
}

void onButtonClick() {
    Serial.println(F("Button clicked!"));
    
    board.clearDisplay();
    board.getDisplay().setCursor(0, 0);
    board.getDisplay().println(F("Button Clicked!"));
    board.getDisplay().println(F(""));
    board.getDisplay().println(F("Status: Active"));
    board.getDisplay().println(F(""));
    board.getDisplay().println(F("Heap: " + String(ESP.getFreeHeap()) + " bytes"));
    board.updateDisplay();
}

void onButtonLongPress() {
    Serial.println(F("Button long pressed!"));
    
    // Toggle power save mode on long press
    board.setPowerSave(!board.isPowerSaveEnabled());
    
    board.clearDisplay();
    board.getDisplay().setCursor(0, 0);
    board.getDisplay().println(F("Long Press!"));
    board.getDisplay().println(F(""));
    board.getDisplay().print(F("Power Save: "));
    board.getDisplay().println(board.isPowerSaveEnabled() ? F("ON") : F("OFF"));
    board.updateDisplay();
}

void updateStatusDisplay() {
    board.clearDisplay();
    board.getDisplay().setCursor(0, 0);
    board.getDisplay().println(F("BMT WHA103 Status"));
    board.getDisplay().println(F("----------------"));
    board.getDisplay().print(F("Heap: "));
    board.getDisplay().print(ESP.getFreeHeap() / 1024);
    board.getDisplay().println(F(" KB"));
    board.getDisplay().print(F("CPU: "));
    board.getDisplay().print(ESP.getCpuFreqMHz());
    board.getDisplay().println(F(" MHz"));
    board.getDisplay().print(F("Btn: "));
    board.getDisplay().println(board.isButtonPressed() ? F("PRESSED") : F("RELEASED"));
    board.getDisplay().print(F("PSave: "));
    board.getDisplay().println(board.isPowerSaveEnabled() ? F("ON") : F("OFF"));
    board.getDisplay().print(F("Vol: "));
    board.getDisplay().print(board.getVolume());
    board.getDisplay().println(F("%"));
    board.updateDisplay();
}
# BMT WHA103 Arduino Library

Arduino-compatible library for the **BMT WHA103** board based on ESP32-C3, featuring:
- 0.96" SSD1306 OLED Display (128x64, I2C)
- ES8311 Audio Codec (I2C + I2S)
- User Button (GPIO 9)
- I2S Audio Interface

## Hardware Pinout

| Function | Pin | Notes |
|----------|-----|-------|
| I2C SDA | GPIO 3 | Shared bus for display & codec |
| I2C SCL | GPIO 4 | Shared bus for display & codec |
| I2S MCLK | GPIO 5 | Master clock |
| I2S BCLK | GPIO 6 | Bit clock |
| I2S DIN | GPIO 7 | Data in (mic) |
| I2S WS | GPIO 8 | Word select |
| Button | GPIO 9 | Active low, pull-up |
| I2S DOUT | GPIO 10 | Data out (speaker) |

## I2C Addresses

| Device | Address |
|--------|---------|
| ES8311 Codec | 0x18 |
| SSD1306 Display | 0x3C |

## Installation

### Option 1: Arduino Library Manager
1. Open Arduino IDE
2. Sketch → Include Library → Manage Libraries
3. Search for "BMT WHA103"
4. Click Install

### Option 2: Manual Installation
1. Download this repository as ZIP
2. Sketch → Include Library → Add .ZIP Library
3. Select the downloaded ZIP

## Quick Start

```cpp
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <BmtWha103_Arduino.h>

BmtWha103Arduino board;

void setup() {
    if (!board.begin()) {
        Serial.println("Board init failed!");
        while (1) delay(1000);
    }
    
    // Set button callbacks
    board.onButtonClick([]() {
        Serial.println("Button clicked!");
    });
    
    board.onButtonLongPress([]() {
        Serial.println("Long press!");
        board.setPowerSave(!board.isPowerSaveEnabled());
    });
    
    // Display test
    board.clearDisplay();
    board.getDisplay().println("Hello BMT WHA103!");
    board.updateDisplay();
}

void loop() {
    board.updateButton();  // Must call regularly
    
    // Your code here
    delay(10);
}
```

## API Reference

### Board Initialization
- `bool begin()` - Initialize all peripherals (I2C, display, button, audio)

### Display Functions
- `Adafruit_SSD1306& getDisplay()` - Get reference to display object
- `void displayOn()` - Turn display on
- `void displayOff()` - Turn display off
- `void clearDisplay()` - Clear display buffer
- `void updateDisplay()` - Push buffer to display

### Button Functions
- `bool isButtonPressed()` - Check current button state
- `void onButtonClick(callback)` - Register click callback
- `void onButtonLongPress(callback)` - Register long press callback (>1s)
- `void updateButton()` - Call regularly in loop() to detect presses

### Audio Functions
- `bool initAudioCodec()` - Initialize ES8311 (placeholder)
- `void setVolume(uint8_t)` - Set volume 0-100%
- `uint8_t getVolume()` - Get current volume

### I2C Functions
- `void scanI2CBus()` - Scan and print all I2C devices
- `bool isDevicePresent(address)` - Check if device responds

### Status Functions
- `String getBoardInfo()` - JSON board metadata
- `String getDeviceStatus()` - JSON runtime status

### Power Management
- `void setPowerSave(bool)` - Enable/disable power save mode
- `bool isPowerSaveEnabled()` - Check power save state

## Examples

### BasicExample
Demonstrates basic board initialization, display usage, and button handling.

### AudioExample
Shows I2S configuration for audio input/output with the ES8311 codec.

## Dependencies

- [Adafruit GFX Library](https://github.com/adafruit/Adafruit-GFX-Library)
- [Adafruit SSD1306](https://github.com/adafruit/Adafruit_SSD1306)
- ESP32 Arduino Core (includes Wire, SPI, I2S)

Install via Library Manager or:
```
pio lib install "Adafruit GFX Library" "Adafruit SSD1306"
```

## PlatformIO Configuration

```ini
[env:bmt-wha103]
platform = espressif32
board = esp32c3-devkitm-1
framework = arduino
lib_deps = 
    adafruit/Adafruit GFX Library
    adafruit/Adafruit SSD1306
    https://github.com/xiaozhi-esp32/BmtWha103_Arduino.git
build_flags = 
    -D BMT_WHA103_ARDUINO
```

## ESP32-C3 Specific Notes

- Flash: 4MB
- PSRAM: None
- CPU: 160 MHz (80 MHz in power save)
- I2S: Uses I2S0 peripheral
- I2C: Uses I2C0 (GPIO 3/4)

## License

MIT License - See LICENSE file for details.

## Credits

Derived from the XiaoZhi ESP32 project ESP-IDF implementation.
Original board: BMT WHA103
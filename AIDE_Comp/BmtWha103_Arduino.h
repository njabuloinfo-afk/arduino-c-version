#ifndef BMT_WHA103_ARDUINO_H
#define BMT_WHA103_ARDUINO_H

#include <Arduino.h>
#include <Wire.h>
#include <SPI.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

/**
 * BMT WHA103 Arduino-Compatible Board Definitions
 * 
 * Hardware pinout derived from the ESP-IDF implementation:
 * - ESP32-C3 MCU
 * - Shared I2C bus (GPIO 3/4) for ES8311 codec and SSD1306 display
 * - I2S audio interface on GPIO 5,6,7,8,10
 * - Boot button on GPIO 9
 * - 0.96" SSD1306 OLED display (128x64)
 */

// I2C Pin Definitions (Shared bus for ES8311 codec and SSD1306 display)
#define BMT_I2C_SDA_PIN          3
#define BMT_I2C_SCL_PIN          4
#define BMT_I2C_PORT             Wire

// ES8311 Audio Codec I2C Address
#define BMT_ES8311_I2C_ADDR      0x18

// SSD1306 OLED Display I2C Address
#define BMT_SSD1306_I2C_ADDR     0x3C

// Display Configuration
#define BMT_DISPLAY_WIDTH        128
#define BMT_DISPLAY_HEIGHT       64
#define BMT_DISPLAY_MIRROR_X     false
#define BMT_DISPLAY_MIRROR_Y     false

// I2S Audio Pin Definitions
#define BMT_I2S_MCLK_PIN         5
#define BMT_I2S_BCLK_PIN         6
#define BMT_I2S_WS_PIN           8
#define BMT_I2S_DIN_PIN          7
#define BMT_I2S_DOUT_PIN         10

// Button Pin Definition
#define BMT_BOOT_BUTTON_PIN      9
#define BMT_BUTTON_ACTIVE_LOW    true  // Button is active low (pressed = LOW)

// LED (Not present on this board)
#define BMT_LED_PIN              -1

// Audio Configuration
#define BMT_AUDIO_SAMPLE_RATE    16000

// Board Identification
#define BMT_BOARD_NAME           "BMT WHA103"
#define BMT_BOARD_VENDOR         "BMT"
#define BMT_BOARD_MODEL          "WHA103"
#define BMT_BOARD_MCU            "ESP32-C3"

// External display instance (defined in BmtWha103_Arduino.cpp)
extern Adafruit_SSD1306 bmt_display;

class BmtWha103Arduino {
public:
    BmtWha103Arduino();
    
    // Initialize all board peripherals
    bool begin();
    
    // Display functions
    Adafruit_SSD1306& getDisplay();
    void displayOn();
    void displayOff();
    void clearDisplay();
    void updateDisplay();
    
    // Button functions
    bool isButtonPressed();
    void onButtonClick(void (*callback)());
    void onButtonLongPress(void (*callback)());
    void updateButton();
    
    // Audio codec functions (placeholder for ES8311)
    bool initAudioCodec();
    void setVolume(uint8_t volume);
    uint8_t getVolume();
    
    // I2C functions
    void scanI2CBus();
    bool isDevicePresent(uint8_t address);
    
    // Status functions
    String getBoardInfo();
    String getDeviceStatus();
    
    // Power management
    void setPowerSave(bool enable);
    bool isPowerSaveEnabled();
    
private:
    // Button state tracking
    bool buttonLastState = HIGH;
    bool buttonCurrentState = HIGH;
    unsigned long buttonPressTime = 0;
    bool buttonLongPressDetected = false;
    void (*buttonClickCallback)() = nullptr;
    void (*buttonLongPressCallback)() = nullptr;
    static const unsigned long LONG_PRESS_TIME = 1000; // 1 second
    
    // Power save state
    bool powerSaveEnabled = false;
    
    // Audio codec volume (0-100)
    uint8_t audioVolume = 50;
};

#endif // BMT_WHA103_ARDUINO_H
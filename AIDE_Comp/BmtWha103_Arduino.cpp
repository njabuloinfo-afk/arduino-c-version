#include "BmtWha103_Arduino.h"

// Global display instance
Adafruit_SSD1306 bmt_display(BMT_DISPLAY_WIDTH, BMT_DISPLAY_HEIGHT, &BMT_I2C_PORT, -1);

BmtWha103Arduino::BmtWha103Arduino() {
    // Constructor - initialization happens in begin()
}

bool BmtWha103Arduino::begin() {
    Serial.begin(115200);
    delay(100);
    
    Serial.println(F("Initializing BMT WHA103 Arduino Board..."));
    
    // Initialize I2C bus
    BMT_I2C_PORT.begin(BMT_I2C_SDA_PIN, BMT_I2C_SCL_PIN);
    BMT_I2C_PORT.setClock(400000); // 400kHz for display
    
    // Scan I2C bus for devices
    scanI2CBus();
    
    // Initialize SSD1306 display
    if (!bmt_display.begin(SSD1306_SWITCHCAPVCC, BMT_SSD1306_I2C_ADDR)) {
        Serial.println(F("SSD1306 allocation failed"));
        return false;
    }
    
    Serial.println(F("SSD1306 display initialized"));
    
    // Configure display
    bmt_display.clearDisplay();
    bmt_display.setTextSize(1);
    bmt_display.setTextColor(SSD1306_WHITE);
    bmt_display.setCursor(0, 0);
    bmt_display.println(F("BMT WHA103 Ready"));
    bmt_display.display();
    
    // Initialize button
    pinMode(BMT_BOOT_BUTTON_PIN, INPUT_PULLUP);
    
    // Initialize audio codec (placeholder)
    initAudioCodec();
    
    Serial.println(F("BMT WHA103 Arduino Board initialized successfully"));
    return true;
}

Adafruit_SSD1306& BmtWha103Arduino::getDisplay() {
    return bmt_display;
}

void BmtWha103Arduino::displayOn() {
    bmt_display.ssd1306_command(SSD1306_DISPLAYON);
}

void BmtWha103Arduino::displayOff() {
    bmt_display.ssd1306_command(SSD1306_DISPLAYOFF);
}

void BmtWha103Arduino::clearDisplay() {
    bmt_display.clearDisplay();
}

void BmtWha103Arduino::updateDisplay() {
    bmt_display.display();
}

bool BmtWha103Arduino::isButtonPressed() {
    return digitalRead(BMT_BOOT_BUTTON_PIN) == (BMT_BUTTON_ACTIVE_LOW ? LOW : HIGH);
}

void BmtWha103Arduino::onButtonClick(void (*callback)()) {
    buttonClickCallback = callback;
}

void BmtWha103Arduino::onButtonLongPress(void (*callback)()) {
    buttonLongPressCallback = callback;
}

void BmtWha103Arduino::updateButton() {
    buttonCurrentState = digitalRead(BMT_BOOT_BUTTON_PIN);
    
    // Button pressed (active low)
    if (buttonCurrentState == LOW && buttonLastState == HIGH) {
        buttonPressTime = millis();
        buttonLongPressDetected = false;
    }
    // Button released
    else if (buttonCurrentState == HIGH && buttonLastState == LOW) {
        unsigned long pressDuration = millis() - buttonPressTime;
        
        if (pressDuration >= LONG_PRESS_TIME && buttonLongPressCallback) {
            buttonLongPressCallback();
        } else if (pressDuration < LONG_PRESS_TIME && buttonClickCallback) {
            buttonClickCallback();
        }
    }
    // Check for long press while held
    else if (buttonCurrentState == LOW && buttonLastState == LOW) {
        unsigned long pressDuration = millis() - buttonPressTime;
        if (pressDuration >= LONG_PRESS_TIME && !buttonLongPressDetected && buttonLongPressCallback) {
            buttonLongPressDetected = true;
            buttonLongPressCallback();
        }
    }
    
    buttonLastState = buttonCurrentState;
}

bool BmtWha103Arduino::initAudioCodec() {
    // Placeholder for ES8311 initialization
    // In a full implementation, this would configure the ES8311 via I2C
    // and set up I2S for audio streaming
    Serial.println(F("Audio codec (ES8311) initialization - placeholder"));
    
    // Check if ES8311 is present on I2C bus
    if (isDevicePresent(BMT_ES8311_I2C_ADDR)) {
        Serial.println(F("ES8311 detected at 0x18"));
        return true;
    } else {
        Serial.println(F("ES8311 not detected"));
        return false;
    }
}

void BmtWha103Arduino::setVolume(uint8_t volume) {
    audioVolume = constrain(volume, 0, 100);
    // In full implementation: send I2C commands to ES8311 to set volume
    Serial.printf("Volume set to %d%%\n", audioVolume);
}

uint8_t BmtWha103Arduino::getVolume() {
    return audioVolume;
}

void BmtWha103Arduino::scanI2CBus() {
    Serial.println(F("Scanning I2C bus..."));
    byte count = 0;
    
    for (byte address = 1; address < 127; address++) {
        BMT_I2C_PORT.beginTransmission(address);
        byte error = BMT_I2C_PORT.endTransmission();
        
        if (error == 0) {
            Serial.printf("I2C device found at 0x%02X\n", address);
            count++;
        }
    }
    
    Serial.printf("Total I2C devices found: %d\n", count);
}

bool BmtWha103Arduino::isDevicePresent(uint8_t address) {
    BMT_I2C_PORT.beginTransmission(address);
    byte error = BMT_I2C_PORT.endTransmission();
    return (error == 0);
}

String BmtWha103Arduino::getBoardInfo() {
    String info = "{";
    info += "\"name\":\"" + String(BMT_BOARD_NAME) + "\",";
    info += "\"vendor\":\"" + String(BMT_BOARD_VENDOR) + "\",";
    info += "\"model\":\"" + String(BMT_BOARD_MODEL) + "\",";
    info += "\"mcu\":\"" + String(BMT_BOARD_MCU) + "\",";
    info += "\"display\":{\"type\":\"SSD1306\",\"width\":" + String(BMT_DISPLAY_WIDTH) + ",\"height\":" + String(BMT_DISPLAY_HEIGHT) + "},";
    info += "\"audio\":{\"codec\":\"ES8311\",\"sample_rate\":" + String(BMT_AUDIO_SAMPLE_RATE) + "},";
    info += "\"button\":1";
    info += "}";
    return info;
}

String BmtWha103Arduino::getDeviceStatus() {
    String status = "{";
    status += "\"board\":\"bmt-wha103\",";
    status += "\"status\":\"operational\",";
    status += "\"display\":true,";
    status += "\"audio_codec\":" + String(isDevicePresent(BMT_ES8311_I2C_ADDR) ? "true" : "false") + ",";
    status += "\"button_state\":" + String(isButtonPressed() ? "pressed" : "released") + ",";
    status += "\"free_heap\":" + String(ESP.getFreeHeap()) + ",";
    status += "\"cpu_freq_mhz\":" + String(ESP.getCpuFreqMHz());
    status += "}";
    return status;
}

void BmtWha103Arduino::setPowerSave(bool enable) {
    powerSaveEnabled = enable;
    if (enable) {
        // Reduce display brightness, lower CPU frequency, etc.
        bmt_display.dim(true);
        setCpuFrequencyMhz(80); // Lower CPU frequency
        Serial.println(F("Power save mode enabled"));
    } else {
        bmt_display.dim(false);
        setCpuFrequencyMhz(160); // Restore full speed
        Serial.println(F("Power save mode disabled"));
    }
}

bool BmtWha103Arduino::isPowerSaveEnabled() {
    return powerSaveEnabled;
}
/**
 * BMT WHA103 Arduino Audio Example
 * 
 * This example demonstrates I2S audio configuration for the ES8311 codec.
 * Note: Full ES8311 driver implementation would require additional I2C register configuration.
 */

#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include "BmtWha103_Arduino.h"

BmtWha103Arduino board;

// I2S configuration for ES8311
#define I2S_SAMPLE_RATE      16000
#define I2S_BITS_PER_SAMPLE  16
#define I2S_CHANNELS         1  // Mono

// Audio buffers
#define AUDIO_BUFFER_SIZE    512
int16_t audioBuffer[AUDIO_BUFFER_SIZE];

void setup() {
    if (!board.begin()) {
        Serial.println(F("Board initialization failed!"));
        while (1) delay(1000);
    }
    
    // Initialize I2S for audio
    initI2S();
    
    board.clearDisplay();
    board.getDisplay().setCursor(0, 0);
    board.getDisplay().println(F("Audio Example"));
    board.getDisplay().println(F("I2S Initialized"));
    board.getDisplay().println(F("Sample Rate: 16kHz"));
    board.getDisplay().println(F(""));
    board.getDisplay().println(F("Speak to test mic"));
    board.updateDisplay();
    
    Serial.println(F("Audio I2S initialized"));
    Serial.printf("Sample Rate: %d Hz\n", I2S_SAMPLE_RATE);
    Serial.printf("Bits: %d\n", I2S_BITS_PER_SAMPLE);
    Serial.printf("Channels: %d\n", I2S_CHANNELS);
}

void loop() {
    board.updateButton();
    
    // Read audio from I2S (microphone)
    size_t bytesRead = 0;
    esp_err_t result = i2s_read(I2S_NUM_0, audioBuffer, sizeof(audioBuffer), &bytesRead, portMAX_DELAY);
    
    if (result == ESP_OK && bytesRead > 0) {
        // Process audio data (e.g., calculate RMS level)
        int samplesRead = bytesRead / sizeof(int16_t);
        int32_t sum = 0;
        for (int i = 0; i < samplesRead; i++) {
            sum += abs(audioBuffer[i]);
        }
        int rms = sum / samplesRead;
        
        // Display audio level on OLED
        static unsigned long lastDisplayUpdate = 0;
        if (millis() - lastDisplayUpdate > 100) {
            lastDisplayUpdate = millis();
            displayAudioLevel(rms);
        }
        
        // Loopback: write same data to speaker (for testing)
        // i2s_write(I2S_NUM_0, audioBuffer, bytesRead, &bytesWritten, portMAX_DELAY);
    }
    
    delay(1);
}

void initI2S() {
    // I2S configuration for ESP32-C3 with ES8311
    i2s_config_t i2sConfig = {
        .mode = (i2s_mode_t)(I2S_MODE_MASTER | I2S_MODE_RX | I2S_MODE_TX),
        .sample_rate = I2S_SAMPLE_RATE,
        .bits_per_sample = I2S_BITS_PER_SAMPLE,
        .channel_format = I2S_CHANNEL_FMT_ONLY_LEFT,  // Mono
        .communication_format = I2S_COMM_FORMAT_STAND_I2S,
        .intr_alloc_flags = ESP_INTR_FLAG_LEVEL1,
        .dma_buf_count = 4,
        .dma_buf_len = 256,
        .use_apll = false,
        .tx_desc_auto_clear = true,
        .fixed_mclk = 0
    };
    
    i2s_pin_config_t pinConfig = {
        .mck_io_num = BMT_I2S_MCLK_PIN,
        .bck_io_num = BMT_I2S_BCLK_PIN,
        .ws_io_num = BMT_I2S_WS_PIN,
        .data_out_num = BMT_I2S_DOUT_PIN,
        .data_in_num = BMT_I2S_DIN_PIN
    };
    
    // Install and start I2S driver
    i2s_driver_install(I2S_NUM_0, &i2sConfig, 0, NULL);
    i2s_set_pin(I2S_NUM_0, &pinConfig);
    i2s_zero_dma_buffer(I2S_NUM_0);
}

void displayAudioLevel(int level) {
    board.clearDisplay();
    board.getDisplay().setCursor(0, 0);
    board.getDisplay().println(F("Audio Level Meter"));
    board.getDisplay().println(F("----------------"));
    
    // Draw level bar
    int barWidth = map(level, 0, 32767, 0, 128);
    barWidth = constrain(barWidth, 0, 128);
    
    board.getDisplay().fillRect(0, 30, barWidth, 10, SSD1306_WHITE);
    board.getDisplay().drawRect(0, 30, 128, 10, SSD1306_WHITE);
    
    board.getDisplay().setCursor(0, 45);
    board.getDisplay().print(F("Level: "));
    board.getDisplay().print(level);
    
    board.getDisplay().setCursor(0, 55);
    board.getDisplay().print(F("Vol: "));
    board.getDisplay().print(board.getVolume());
    board.getDisplay().print(F("%"));
    
    board.updateDisplay();
}
/**
 * @version: 1.0.0
 *
 * @author: Thomas Houlberg
 */
#pragma once

#if __has_include(<Arduino.h>)

#include <Arduino.h>

class KbMtxScan
{
private:
    typedef void (*func_p_t)(char, uint8_t);
    func_p_t keyPressedCallback = NULL;
    func_p_t keyReleasedCallback = NULL;
    uint8_t rowCount = 0;
    uint8_t colCount = 0;
    uint8_t *rowPins = NULL;
    uint8_t *colPins = NULL;
    char *keyCharMap = NULL;
    bool *keyState = NULL;

#ifdef ARDUINO_ARCH_ESP32
    TaskHandle_t workerTaskHandle = NULL;
    void scanMatrix();
    static void taskWorker(void *);
#endif

public:
    KbMtxScan();
    ~KbMtxScan();
    void begin(uint8_t rowCount, uint8_t colCount, uint8_t *rowPins, uint8_t *colPins, char *keyCharMap);
    void end();
    void setKeyPressedCallback(func_p_t callback);
    void setKeyReleasedCallback(func_p_t callback);
#ifndef ARDUINO_ARCH_ESP32
    void scanMatrix();
#endif
};

#else
#error "This library requires the Arduino framework."
#endif
#include <KbMtxScan.hpp>

KbMtxScan::KbMtxScan()
{
}

KbMtxScan::~KbMtxScan()
{
    end();
}

/**
 * If used with non ESP32 platform, this function must be called in the loop() function.
 */
void KbMtxScan::scanMatrix()
{
#ifndef ARDUINO_ARCH_ESP32
    if (keyState == NULL)
    {
        Serial.println("ERROR: KbMtxScan is not initialized. Call begin() first.");
        return;
    }
#endif

    uint8_t totalKeysPressed = 0;
    bool currentState[colCount * rowCount] = {false};

    for (uint8_t row = 0; row < rowCount; row++)
    {
        digitalWrite(rowPins[row], LOW);

        for (uint8_t col = 0; col < colCount; col++)
        {
            if (!digitalRead(colPins[col]))
            {
                totalKeysPressed++;
                currentState[row * colCount + col] = true;
            }
        }

        digitalWrite(rowPins[row], HIGH);
        delay(1); // Allow time for row pin to go high impedance.
    }

    for (uint8_t keyIdx = 0; keyIdx < colCount * rowCount; keyIdx++)
    {
        if (currentState[keyIdx] != keyState[keyIdx])
        {
            if (currentState[keyIdx])
            {
                keyState[keyIdx] = true;
                if (keyPressedCallback != NULL)
                {
                    keyPressedCallback(keyCharMap[keyIdx], totalKeysPressed);
                }
            }
            else
            {
                keyState[keyIdx] = false;
                if (keyReleasedCallback != NULL)
                {
                    keyReleasedCallback(keyCharMap[keyIdx], totalKeysPressed);
                }
            }
        }
    }
}

/**
 * Initialize the keyboard matrix scanner.
 *
 * @param rowCount Number of rows in the keyboard matrix.
 * @param colCount Number of columns in the keyboard matrix.
 * @param rowPins Array of row pin GPIO numbers.
 * @param colPins Array of column pin GPIO numbers.
 * @param keyCharMap Array of key character maps.
 */
void KbMtxScan::begin(uint8_t rowCount, uint8_t colCount, uint8_t *rowPins, uint8_t *colPins, char *keyCharMap)
{
    if (keyState != NULL)
    {
        Serial.println("ERROR: Keyboard matrix scanner already initialized");
        return;
    }

    this->rowCount = rowCount;
    this->colCount = colCount;
    this->rowPins = rowPins;
    this->colPins = colPins;
    this->keyCharMap = keyCharMap;

    for (uint8_t row = 0; row < rowCount; row++)
    {
        pinMode(rowPins[row], OUTPUT_OPEN_DRAIN);
        digitalWrite(rowPins[row], HIGH);
    }

    for (uint8_t col = 0; col < colCount; col++)
    {
        pinMode(colPins[col], INPUT_PULLUP);
    }

    keyState = (bool *)malloc(sizeof(bool) * colCount * rowCount);
    *keyState = {false};

#ifdef ARDUINO_ARCH_ESP32
    xTaskCreate(
        this->taskWorker,        /* Task function. */
        "Keyboard scanner task", /* String with name of task. */
        8192,                    /* Stack size in bytes. */
        this,                    /* Parameter passed as input of the task */
        2,                       /* Priority of the task. */
        &workerTaskHandle        /* Task handle. */
    );
#endif
}

/**
 * End the keyboard matrix scanner, and free the resources.
 * If used with ESP32, the task worker will be destroyed.
 */
void KbMtxScan::end()
{
#ifdef ARDUINO_ARCH_ESP32
    if (workerTaskHandle != NULL)
    {
        vTaskDelete(workerTaskHandle);
        workerTaskHandle = NULL;
    }
#endif

    free(keyState);
    keyState = NULL;
}

/**
 * Set the callback function to be called when a key is pressed.
 *
 * @param callback Callback function.
 * @param callback Function signature: void callback(char key, uint8_t totalKeysPressed).
 */
void KbMtxScan::setKeyPressedCallback(func_p_t callback)
{
    keyPressedCallback = callback;
}

/**
 * Set the callback function to be called when a key is released.
 *
 * @param callback Callback function.
 * @param callback Function signature: void callback(char key, uint8_t totalKeysPressed).
 */
void KbMtxScan::setKeyReleasedCallback(func_p_t callback)
{
    keyReleasedCallback = callback;
}

#ifdef ARDUINO_ARCH_ESP32
void KbMtxScan::taskWorker(void *arg)
{
    KbMtxScan *_this = static_cast<KbMtxScan *>(arg);

    while (true)
    {
        _this->scanMatrix();

        vTaskDelay(10);
    }
}
#endif
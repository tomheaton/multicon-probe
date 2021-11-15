#include <Arduino.h>
#include <SPI.h>
#include <NRFLite.h>

#define BUTTON_PIN 3
#define LED_PIN 10
#define CE_PIN 5
#define CSN_PIN 6

#define RADIO_ID 0  // 0 = Transmitter, 1 = Receiver

bool buttonStateA = 0;
bool buttonStateB = 0;

struct RadioPacket {
    uint8_t FromRadioId;
    uint32_t OnTimeMillis;
    uint32_t FailedTxCount;
};

NRFLite _radio;
RadioPacket _radioData;

void Zero(void);
void One(void);

/**
 * Setup.
 *
 * @return None.
 */
void setup() {
    Serial.begin(9600);
    Serial.println("starting...");

    pinMode(BUTTON_PIN, INPUT_PULLUP);
    pinMode(LED_PIN, OUTPUT);

    if (!_radio.init(RADIO_ID, CE_PIN, CSN_PIN)) {
        Serial.println("Cannot communicate with radio");
        while (true);
    }
    
    _radioData.FromRadioId = RADIO_ID;
}

/**
 * Loop.
 *
 * @return None.
 */
void loop() {    
    if (RADIO_ID == 0) {
        Serial.println("RADIO Zero");
        Zero();
    } else {
        Serial.println("RADIO One");
        One();
    }   
}

/**
 * Radio Zero .
 *
 * @return None.
 */
void Zero() {
    while (true) {
        _radioData.OnTimeMillis = millis();

        Serial.print("Sending ");
        Serial.print(_radioData.OnTimeMillis);
        Serial.print(" ms");
        
        if (_radio.send(RADIO_ID, &_radioData, sizeof(_radioData))) {
            Serial.println("...Success");
        }
        else {
            Serial.println("...Failed");
            _radioData.FailedTxCount++;
        }

        delay(1000);
    }
}

/**
 * Radio One
 *
 * @return None.
 */
void One() {
    while (true) {
        while (_radio.hasData()) {
            _radio.readData(&_radioData);

            String msg = "Radio ";
            msg += _radioData.FromRadioId;
            msg += ", ";
            msg += _radioData.OnTimeMillis;
            msg += " ms, ";
            msg += _radioData.FailedTxCount;
            msg += " Failed TX";

            Serial.println(msg);
        }
    }
}

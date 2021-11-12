#include <Arduino.h>
#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>

#define BUTTON_PIN 3
#define LED_PIN 10

#define MASTER 1  // 1 = A, 0 = B

RF24 radio(7, 8);  // CE, CSN

const byte addresses[][6] = {"00001", "00002"};

bool buttonStateA = 0;
bool buttonStateB = 0;

void A(void);
void B(void);

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

    radio.begin();

    radio.openWritingPipe(addresses[MASTER ? 1 : 0]);
    radio.openReadingPipe(1, addresses[MASTER ? 0 : 1]);

    radio.setPALevel(RF24_PA_MIN);
}

/**
 * Loop.
 *
 * @return None.
 */
void loop() {    
    if (MASTER) {
        Serial.println("MASTER I (A)");
        A();
    } else {
        Serial.println("MASTER II (B)");
        B();
    }   
}

/**
 * A.
 *
 * @return None.
 */
void A() {
    while (true) {
        delay(5);

        radio.stopListening();

        buttonStateA = digitalRead(BUTTON_PIN);

        radio.write(&buttonStateA, sizeof(buttonStateA));

        delay(5);

        radio.startListening();

        while (!radio.available()) {
            Serial.println("radio no available.");
        };

        radio.read(&buttonStateB, sizeof(buttonStateB));
        Serial.println((String)"other button state: " + buttonStateB);

        digitalWrite(LED_PIN, buttonStateB);
    }
}

/**
 * B.
 *
 * @return None.
 */
void B() {
    while (true) {
        delay(5);

        radio.startListening();

        if (radio.available()) {
            
            Serial.println((String)"other button state: " + buttonStateB);

            radio.read(&buttonStateA, sizeof(buttonStateA));
            Serial.println((String)"other button state: " + buttonStateA);
            
            digitalWrite(LED_PIN, buttonStateA);

            delay(5);

            radio.stopListening();

            buttonStateB = digitalRead(buttonStateB);

            radio.write(&buttonStateB, sizeof(buttonStateB));
        } else {
            Serial.println("radio not available.");
        }
    }
}

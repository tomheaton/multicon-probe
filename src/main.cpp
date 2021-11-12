#include <Arduino.h>
#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>

#define BUTTON_PIN 3
#define LED_PIN 10

#define MASTER 1

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

    radio.openWritingPipe(addresses[MASTER ? 0 : 1]);
    radio.openReadingPipe(1, addresses[MASTER ? 1 : 0]);

    radio.setPALevel(RF24_PA_MIN);
}

/**
 * Loop.
 *
 * @return None.
 */
void loop() {
    //MASTER ? A() : B();
    
    if (MASTER) {
        Serial.println("MASTER I");
        A();
    } else {
        Serial.println("MASTER II");
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

        while (!radio.available());

        radio.read(&buttonStateB, sizeof(buttonStateB));

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

            radio.read(&buttonStateA, sizeof(buttonStateA));
            
            digitalWrite(LED_PIN, buttonStateA);

            delay(5);

            radio.stopListening();

            buttonStateB = digitalRead(buttonStateB);

            radio.write(&buttonStateB, sizeof(buttonStateB));
        }
    }
}

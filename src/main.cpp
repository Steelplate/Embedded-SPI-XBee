#include "XbeeProSX.h"

const int CS_PIN = 17;

XbeeProSX xbee(CS_PIN);

void setup() {

    xbee.begin();

}

void loop() {

    xbee.updateSubscribers();

    // Example: send a message to all subscribers
    xbee.sendToSubscribers("Hello, subscribers!");

    delay(10); // Delay to avoid spamming

}

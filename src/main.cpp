#include "XbeeProSX.h"

const int CS_PIN = 17;

XbeeProSX xbee(CS_PIN);

void setup() {

    xbee.begin();

}

void loop() {

    xbee.updateSubscribers();

    std::string str = "Hello, subscribers!";

    // Example: send a message to all subscribers
    xbee.sendToSubscribers(str.c_str(), str.size());

    delay(10); // Delay to avoid spamming

}

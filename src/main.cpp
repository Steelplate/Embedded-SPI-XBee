#include "XbeeProSX.h"

const int CS_PIN = 17;

XbeeProSX xbee(CS_PIN);

void setup() {

    xbee.begin();

}

void loop() {

    std::string message = "hello world";

    xbee.broadcast(message);

    delay(1000);

}

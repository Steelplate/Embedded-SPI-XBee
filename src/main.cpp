#include "XbeeProSX.h"

const int CS_PIN = 17;

XbeeProSX xbee(CS_PIN);

void setup() {

    xbee.begin();

}

void loop() {

  xbee.updateSubscribers();

  std::string message = "Hello, subscribers!";
  // xbee.sendToSubscribers(message);

  delay(100);

  float sensorReading = 25.7;
  xbee.sendToSubscribers((const float&)sensorReading);

  delay(1000);

}

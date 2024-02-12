#ifndef XbeeProSX_h
#define XbeeProSX_h

#include <Arduino.h>
#include <SPI.h>
#include <vector>
#include <algorithm>

class XbeeProSX {

public:

    XbeeProSX(uint8_t cs_pin);

    void begin();

    bool isDataAvailable();

    template <typename T>
    void send(const std::vector<uint8_t> &address, const T &data);

    template <typename T>
    std::pair<std::vector<uint8_t>, T> receive();

    template <typename T>
    void broadcast(const T &data);

    void updateSubscribers();

    template <typename T>
    void sendToSubscribers(const T &data);

private:

    uint8_t _cs_pin;
    std::vector<std::vector<uint8_t>> subscribers; // List of subscribers

    uint8_t calculateChecksum(const uint8_t *data, size_t length);

};

#endif
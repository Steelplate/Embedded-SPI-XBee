#include "XbeeProSX.h"

XbeeProSX::XbeeProSX(uint8_t cs_pin) : _cs_pin(cs_pin) {}

void XbeeProSX::begin() {

    pinMode(_cs_pin, OUTPUT);
    digitalWrite(_cs_pin, HIGH);

    SPI.begin();
    SPI.beginTransaction(SPISettings(6000000, MSBFIRST, SPI_MODE0));

}

bool XbeeProSX::isDataAvailable() {

    digitalWrite(_cs_pin, LOW);
    bool available = (SPI.transfer(0x00) == 0x7E);
    digitalWrite(_cs_pin, HIGH);

    return available;

}

template <typename T>
void XbeeProSX::send(const std::vector<uint8_t> &address, const T &data) {

    const uint8_t *dataPtr = reinterpret_cast<const uint8_t *>(&data);

    size_t dataLength = sizeof(T);
    size_t contentLength = dataLength + 14; // Length calculation

    uint8_t *packet = new uint8_t[contentLength + 4]; // +4 for start delimiter, length, and checksum
    size_t index = 0;

    packet[index++] = 0x7E; // Start delimiter
    packet[index++] = (contentLength >> 8) & 0xFF; // Length high byte
    packet[index++] = contentLength & 0xFF;       // Length low byte
    packet[index++] = 0x10; // Frame type
    packet[index++] = 0x00; // Frame ID

    for (size_t i = 0; i < address.size(); ++i) {
        packet[index++] = address[i];
    }

    packet[index++] = 0xFF; // Reserved
    packet[index++] = 0xFE; // Reserved
    packet[index++] = 0x00; // Broadcast radius
    packet[index++] = 0x03; // Options byte

    for (size_t i = 0; i < dataLength; ++i) {
        packet[index++] = dataPtr[i];
    }

    packet[index] = calculateChecksum(packet + 3, contentLength);

    digitalWrite(_cs_pin, LOW);

    for (size_t i = 0; i < contentLength + 4; ++i) {
        SPI.transfer(packet[i]);
    }

    digitalWrite(_cs_pin, HIGH);

    delete[] packet;

}

template <typename T>
std::pair<std::vector<uint8_t>, T> XbeeProSX::receive() {

    std::vector<uint8_t> address;

    T data;
    uint8_t *dataPtr = reinterpret_cast<uint8_t *>(&data);

    digitalWrite(_cs_pin, LOW);

    uint16_t length = SPI.transfer(0x00) << 8;
    length |= SPI.transfer(0x00);

    if (SPI.transfer(0x00) != 0x90) {

        digitalWrite(_cs_pin, HIGH);

        return {address, data};

    }

    for (int i = 0; i < 8; ++i) {

        address.push_back(SPI.transfer(0x00));

    }

    SPI.transfer(0x00); // Skip reserved bytes
    SPI.transfer(0x00);

    SPI.transfer(0x00); // Receive Options

    size_t dataLength = length - 12; // Subtract bytes already read

    for (size_t i = 0; i < dataLength; ++i) {

        dataPtr[i] = SPI.transfer(0x00);

    }

    SPI.transfer(0x00); // Skip checksum

    digitalWrite(_cs_pin, HIGH);

    return {address, data};

}

template <typename T>
void XbeeProSX::broadcast(const T &data) {

    std::vector<uint8_t> broadcastAddress = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFF, 0xFF};

    send(broadcastAddress, data);

}

void XbeeProSX::updateSubscribers() {

    if (isDataAvailable()) {

        auto data = receive<std::string>();

        std::vector<uint8_t> address = data.first;
        std::string message = data.second;

        if (!address.empty() && message == "subscribe") {

            if (std::find(subscribers.begin(), subscribers.end(), address) == subscribers.end()) {

                subscribers.push_back(address);

            }

        }

    }

}

template <typename T>
void XbeeProSX::sendToSubscribers(const T &data) {

    for (const auto &subscriberAddress : subscribers) {

        send(subscriberAddress, data);

    }

}

uint8_t XbeeProSX::calculateChecksum(const uint8_t *data, size_t length) {

    uint8_t checksum = 0xFF;

    for (size_t i = 0; i < length; ++i) {

        checksum -= data[i];

    }

    return checksum;
    
}
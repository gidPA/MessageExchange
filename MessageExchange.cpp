#include "MessageExchange.h"

MessageExchange::MessageExchange() : uartDevice(nullptr), uartMonitoringDevice(nullptr) {}

void MessageExchange::setUartDevice(HardwareSerial *device) {
    uartDevice = device;
}

HardwareSerial* MessageExchange::getUartDevice() {
    return uartDevice;
}

void MessageExchange::setUartMonitoringDevice(HardwareSerial *device) {
    uartMonitoringDevice = device;
}

void MessageExchange::setSignature(const char * signature){
    strncpy(objectSignature, signature, 20);
}

void MessageExchange::createNewMessage(messageTopic topic) {
    message[0] = MESSAGE_START_CODE;
    message[1] = (byte)topic;
    message[3] = (byte)LoadingType;
    message[4] = (byte)LoadingSize; 
    message[MESSAGE_SIZE - 1] = MESSAGE_END_CODE;
}

void MessageExchange::createNewMessage(const byte messageSrc[MESSAGE_SIZE]){
    memcpy(message, messageSrc, MESSAGE_SIZE);
}

void MessageExchange::setItemEntryStatus(itemStatus status) {
    message[2] = (byte)status;
}

void MessageExchange::getRawMessage(byte dest[MESSAGE_SIZE]){
    memcpy(dest, message, MESSAGE_SIZE);
}

byte MessageExchange::getItemEntryStatus() {
    return message[2];
}

void MessageExchange::setItemType(itemType type) {
    message[3] = (byte)type;
}

byte MessageExchange::getItemType() {
    return message[3];
}

void MessageExchange::setItemSize(itemSize size) {
    message[4] = (byte)size;
}

byte MessageExchange::getItemSize() {
    return message[4];
}

void MessageExchange::setItemPoint(byte point) {
    message[5] = point;
}

byte MessageExchange::getItemPoint() {
    return message[5];
}

bool MessageExchange::checkMessageEntry(){
    if (uartDevice->available()){
        return true;
    }
    else {
        return false;
    }
}

void MessageExchange::previewMessage() {
    char buffer[50];
    sprintf(buffer, "[%s Preview] %d %d %d %d %d %d %d %d %d %d", objectSignature,
            message[0], message[1], message[2], message[3], message[4],
            message[5], message[6], message[7], message[8], message[9]);

    if (uartMonitoringDevice != nullptr) {
        uartMonitoringDevice->println(buffer);
        uartMonitoringDevice->println(getMessageTopicName((messageTopic)message[1]));
        uartMonitoringDevice->println(getItemStatusName((itemStatus)message[2]));
        uartMonitoringDevice->println(getItemTypeName((itemType)message[3]));
        uartMonitoringDevice->println(getItemSizeName((itemSize)message[4]));
    } else {
        Serial.println("UART monitoring device not set.");
    }
}

void MessageExchange::sendMessage() {
    if (uartDevice != nullptr) {
        uartDevice->write(message, MESSAGE_SIZE);
    } else {
        if (uartMonitoringDevice != nullptr) {
            uartMonitoringDevice->println("UART device not set.");
        } else {
            Serial.println("UART device not set.");
        }
    }
}

byte MessageExchange::handleIncomingMessage() {
    // if (uartDevice != nullptr && uartDevice->available() >= MESSAGE_SIZE) {
    //     uartDevice->readBytes(message, MESSAGE_SIZE);
    //     clearSerialBuffer();
    //     return message[1]; // Return the message topic
    // } else {
    //     if (uartMonitoringDevice != nullptr) {
    //         uartMonitoringDevice->println("UART device not set or not enough data available.");
    //     } else {
    //         Serial.println("UART device not set or not enough data available.");
    //     }

    //     clearSerialBuffer();
    //     return TOPIC_ERR; // Return an error code or handle as needed
    // }

    if (uartDevice == nullptr){
        Serial.println("[" + String(objectSignature) + "] UART Device not set");
        return TOPIC_ERR;
    }

    uint8_t byteData;
    int prev = 0;
    //int counter = 0;

    byte header = uartDevice->peek();

    if (header == MESSAGE_START_CODE)
    {
        for(int counter = 0; counter < MESSAGE_SIZE; counter++){
            message[counter] = uartDevice->read();

            // if (counter == (MESSAGE_SIZE - 1) && message[counter] != MESSAGE_END_CODE){
            //     return STAT_ERR;
            // }
        }
    }
    else
    {
        clearSerialBuffer();
        return TOPIC_ERR;
    }

    // if (prev > 0)
    // {
    //     for (int i = 0; i < 10; i++)
    //     {
    //         uartMonitoringDevice->print(message[i]);
    //         uartMonitoringDevice->print(" ");
    //         if (message[i] == 254)
    //         {
    //             uartMonitoringDevice->println();
    //             break;
    //         }
    //     }
    // }

    return message[1];
}

void MessageExchange::clearSerialBuffer() {
    while (uartDevice->available() > 0) {
        uartDevice->read();
    }
}

const char* MessageExchange::getMessageTopicName(messageTopic topic) {
    switch (topic) {
        case TOPIC_ERR: return "TOPIC_ERR";
        case BEGIN_TRANSACTION: return "BEGIN_TRANSACTION";
        case ITEM_ENTRY: return "ITEM_ENTRY";
        case SET_MEMBER_MODE: return "SET_MEMBER_MODE";
        case SET_EXCHANGE_RATE: return "SET_EXCHANGE_RATE";
        case READY_FOR_TRANSACTION: return "READY_FOR_TRANSACTION";
        case TRANSACTION_COMPLETE: return "TRANSACTION_COMPLETE";
        case BIN_FULL: return "BIN_FULL";
        case COIN_DISPENSER_ALERT: return "COIN_DISPENSER_ALERT";
        default: return "UNKNOWN_TOPIC";
    }
}

const char* MessageExchange::getItemTypeName(itemType type) {
    switch (type) {
        case ITEM_ERR: return "ITEM_ERR";
        case PLASTIC_COLOURED: return "PLASTIC_COLOURED";
        case PLASTIC_TRANSPARENT: return "PLASTIC_TRANSPARENT";
        case METAL: return "METAL";
        default: return "UNKNOWN_ITEM_TYPE";
    }
}

const char* MessageExchange::getItemSizeName(itemSize size) {
    switch (size) {
        case SIZE_ERR: return "SIZE_ERR";
        case SMALL: return "SMALL";
        case MEDIUM: return "MEDIUM";
        case LARGE: return "LARGE";
        default: return "UNKNOWN_SIZE";
    }
}

const char* MessageExchange::getItemStatusName(itemStatus status) {
    switch (status) {
        case STAT_ERR: return "STAT_ERR";
        case DECLINED: return "DECLINED";
        case ACCEPTED: return "ACCEPTED";
        case STAT_PENDING: return "PENDING";
        default: return "UNKNOWN_STATUS";
    }
}

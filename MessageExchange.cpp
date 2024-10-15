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

void MessageExchange::createNewMessage(MessageTopic topic) {
    if (topic != ITEM_ENTRY){
        memset(message, 0, sizeof(message));
    }
    else{
        message[3] = (byte)LoadingType;
        message[4] = (byte)LoadingSize; 
    }

    message[0] = MESSAGE_START_CODE;
    message[1] = (byte)topic;

    message[MESSAGE_SIZE - 1] = MESSAGE_END_CODE;
}

void MessageExchange::createNewMessage(const byte messageSrc[MESSAGE_SIZE]){
    memcpy(message, messageSrc, MESSAGE_SIZE);
}

void MessageExchange::setItemEntryStatus(ItemStatus status) {
    message[2] = (byte)status;
}

void MessageExchange::getRawMessage(byte dest[MESSAGE_SIZE]){
    memcpy(dest, message, MESSAGE_SIZE);
}

byte MessageExchange::getItemEntryStatus() {
    Serial.println("Getting Entry Status...");
    return message[2];
}

void MessageExchange::setItemType(ItemType type) {
    message[3] = (byte)type;
}

byte MessageExchange::getItemType() {
    return message[3];
}

void MessageExchange::setItemSize(ItemSize size) {
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

void MessageExchange::setMemberModeReadiness(MemberModeReadyStatus status){
    message[2] = (byte)status;
}

byte MessageExchange::getMemberModeReadiness(){
    return message[2];
};


void MessageExchange::setMessageDataDirectly(byte dataByte, byte dataValue){
    message[dataByte] = dataValue;
}

byte MessageExchange::getMessageDataDirectly(byte dataByte){
    return message[dataByte];
}

bool MessageExchange::checkMessageEntry(){
    int enteredMessageSize = 0;
    if (uartDevice->available() >= MESSAGE_SIZE){
        for (int i=0; i < enteredMessageSize; i++){
            int buffer = uartDevice->peek();
            if (buffer == MESSAGE_START_CODE){
                return true;
            }else{
                uartDevice->read();
            }
            
            if (i == (enteredMessageSize - 1)){
                return false;
            }
        }
        return false;
    }
    else {
        return false;
    }
}

void MessageExchange::previewMessage() {
    char buffer[100];
    snprintf(buffer, sizeof(buffer),"[%s Preview] %d %d %d %d %d %d %d %d %d %d", objectSignature,
            message[0], message[1], message[2], message[3], message[4],
            message[5], message[6], message[7], message[8], message[9]);
    
    if (uartMonitoringDevice != nullptr) {
        uartMonitoringDevice->println(buffer);
        uartMonitoringDevice->println(getMessageTopicName((MessageTopic)message[1]));
        uartMonitoringDevice->println(getItemStatusName((ItemStatus)message[2]));
        uartMonitoringDevice->println(getItemTypeName((ItemType)message[3]));
        uartMonitoringDevice->println(getItemSizeName((ItemSize)message[4]));
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

byte MessageExchange::sendMessageAndWait(){
    this->sendMessage();
    long timeoutCount = millis();

    while(true){
        if(uartDevice->available() > 0){
            return this->handleIncomingMessage();
        } else if((millis() - timeoutCount) == 10000){
            return 100;
        }
    }
}

byte MessageExchange::handleIncomingMessage() {
    if (uartDevice != nullptr && uartDevice->available() >= MESSAGE_SIZE) {
        this->clearMessageBuffer();
        uartDevice->readBytes(message, MESSAGE_SIZE);
        this->clearSerialBuffer();
        return message[1]; // Return the message topic
    } else {
        if (uartMonitoringDevice != nullptr) {
            uartMonitoringDevice->println("UART device not set or not enough data available.");
        } else {
            Serial.println("UART device not set or not enough data available.");
        }
        return TOPIC_ERR; // Return an error code or handle as needed
    }
}

void MessageExchange::clearSerialBuffer() {
    while (uartDevice->available() > 0) {
        uartDevice->read();
    }
}

void MessageExchange::clearMessageBuffer(){
    memset(message, 0, sizeof(message));
}

const char* MessageExchange::getMessageTopicName(MessageTopic topic) {
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

const char* MessageExchange::getItemTypeName(ItemType type) {
    switch (type) {
        case ITEM_ERR: return "ITEM_ERR";
        case PLASTIC_COLOURED: return "PLASTIC_COLOURED";
        case PLASTIC_TRANSPARENT: return "PLASTIC_TRANSPARENT";
        case METAL: return "METAL";
        case LoadingType: return "LoadingType";
        default: return "UNKNOWN_ITEM_TYPE";
    }
}

const char* MessageExchange::getItemSizeName(ItemSize size) {
    switch (size) {
        case SIZE_ERR: return "SIZE_ERR";
        case SMALL: return "SMALL";
        case MEDIUM: return "MEDIUM";
        case LARGE: return "LARGE";
        case LoadingSize: return "LoadingSize";
        default: return "UNKNOWN_SIZE";
    }
}

const char* MessageExchange::getItemStatusName(ItemStatus status) {
    switch (status) {
        case STAT_ERR: return "STAT_ERR";
        case DECLINED: return "DECLINED";
        case ACCEPTED: return "ACCEPTED";
        case STAT_PENDING: return "PENDING";
        default: return "UNKNOWN_STATUS";
    }
}

const char* MessageExchange::getTranslatedItemTypeName(ItemType type) {
    switch (type) {
        case ITEM_ERR: return "ITEM_ERR";
        case PLASTIC_COLOURED: return "Berwarna";
        case PLASTIC_TRANSPARENT: return "Bening";
        case METAL: return "Logam";
        case LoadingType: return "Loading...";
        default: return "UNKNOWN_ITEM_TYPE";
    }
}

const char* MessageExchange::getTranslatedItemSizeName(ItemSize size) {
    switch (size) {
        case SIZE_ERR: return "SIZE_ERR";
        case SMALL: return "Kecil";
        case MEDIUM: return "Sedang";
        case LARGE: return "Besar";
        case LoadingSize: return "Loading...";
        default: return "UNKNOWN_SIZE";
    }
}
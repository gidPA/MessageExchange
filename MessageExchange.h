#ifndef MESSAGE_EXCHANGE_H
#define MESSAGE_EXCHANGE_H

#include <Arduino.h>

#define MESSAGE_SIZE 10
#define MESSAGE_START_CODE 252
#define MESSAGE_END_CODE 254

enum messageTopic {
    TOPIC_ERR,
    BEGIN_TRANSACTION,
    ITEM_ENTRY,
    SET_MEMBER_MODE,
    SET_EXCHANGE_RATE,
    READY_FOR_TRANSACTION,
    TRANSACTION_COMPLETE,
    BIN_FULL,
    COIN_DISPENSER_ALERT,
    BIN_EMPTY_AND_COIN
};

enum itemType {
    ITEM_ERR,
    PLASTIC_COLOURED,
    PLASTIC_TRANSPARENT,
    METAL,
    LoadingType
};

enum itemSize {
    SIZE_ERR,
    SMALL,
    MEDIUM,
    LARGE,
    LoadingSize
};

enum itemStatus {
    STAT_ERR,
    DECLINED,
    ACCEPTED,
    STAT_PENDING
};

class MessageExchange {
private:
    byte message[MESSAGE_SIZE];
    HardwareSerial *uartDevice;
    HardwareSerial *uartMonitoringDevice;

    char objectSignature[20] = "";

    void clearSerialBuffer();

public:
    MessageExchange();
    void setUartDevice(HardwareSerial *device);
    void setSignature(const char * signature);
    HardwareSerial* getUartDevice();

    void setUartMonitoringDevice(HardwareSerial *device);
    void createNewMessage(messageTopic topic);
    void createNewMessage(const byte messageSrc[MESSAGE_SIZE]);

    void getRawMessage(byte dest[MESSAGE_SIZE]);

    void setItemEntryStatus(itemStatus status);
    byte getItemEntryStatus();

    void setItemType(itemType type);
    byte getItemType();

    void setItemSize(itemSize size);
    byte getItemSize();

    void setItemPoint(byte point);
    byte getItemPoint();

    bool checkMessageEntry();
    void previewMessage();
    void sendMessage();
    byte handleIncomingMessage();

    const char* getMessageTopicName(messageTopic topic);
    const char* getItemTypeName(itemType type);
    const char* getItemSizeName(itemSize size);
    const char* getItemStatusName(itemStatus status);
};

#endif // MESSAGE_EXCHANGE_H

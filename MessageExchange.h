#ifndef MESSAGE_EXCHANGE_H
#define MESSAGE_EXCHANGE_H

#include <Arduino.h>

#define MESSAGE_SIZE 10
#define MESSAGE_START_CODE 252
#define MESSAGE_END_CODE 254

enum MessageTopic {
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

enum ItemType {
    ITEM_ERR,
    PLASTIC_COLOURED,
    PLASTIC_TRANSPARENT,
    METAL,
    LoadingType
};

enum ItemSize {
    SIZE_ERR,
    SMALL,
    MEDIUM,
    LARGE,
    LoadingSize
};

enum ItemStatus {
    STAT_ERR,
    DECLINED,
    ACCEPTED,
    STAT_PENDING
};

enum MemberModeReadyStatus{
    MEMBERMODE_ERR = 0,
    MEMBER_APPROVE = 1,
    MEMBER_REJECT_BIN = 125,
    MEMBER_REJECT_COIN = 126,
    MEMBER_REJECT_BUSY = 127
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
    void createNewMessage(MessageTopic topic);
    void createNewMessage(const byte messageSrc[MESSAGE_SIZE]);

    void getRawMessage(byte dest[MESSAGE_SIZE]);

    void setItemEntryStatus(ItemStatus status);
    byte getItemEntryStatus();

    void setItemType(ItemType type);
    byte getItemType();

    void setItemSize(ItemSize size);
    byte getItemSize();

    void setItemPoint(byte point);
    byte getItemPoint();

    void setMemberModeReadiness(MemberModeReadyStatus status);
    byte getMemberModeReadiness();

    void setMessageDataDirectly(byte dataByte, byte dataValue);
    byte getMessageDataDirectly(byte dataByte);

    bool checkMessageEntry();
    void previewMessage();
    void sendMessage();
    byte sendMessageAndWait();
    byte handleIncomingMessage();

    void clearMessageBuffer();

    const char* getMessageTopicName(MessageTopic topic);
    const char* getItemTypeName(ItemType type);
    const char* getItemSizeName(ItemSize size);
    const char* getItemStatusName(ItemStatus status);

    const char* getTranslatedItemTypeName(ItemType type);
    const char* getTranslatedItemSizeName(ItemSize size);
};

#endif // MESSAGE_EXCHANGE_H

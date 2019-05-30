#pragma once
#include "gtpHeader.h"

struct gtpMessage
{
    gtpHeader gtp_header;
    uint32_t payloadLength;
    uint8_t payload[BUF_SIZE];
};

/* Encode/Decode wrapper functions for GTP message*/
int decodeGtpMessage(uint8_t *buffer, gtpMessage *msgStruct,
                uint32_t bufLen);
int encodeGtpMessage(uint8_t *buffer,uint32_t bufLen,
                gtpMessage *msgStruct,uint32_t *encodedLen);
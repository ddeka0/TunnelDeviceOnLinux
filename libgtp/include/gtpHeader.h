#pragma once
#include <bits/stdc++.h>
#define VERSION_MASK 0xE0
#define PT_MASK (1<<4)
#define GTP_E_MASK (1<<2)
#define GTP_S_MASK 0x02
#define GTP_PN_MASK 0x01
#define BUF_SIZE 3000
#define GTP_SEQ_NUM_PRESENT (1<<0)
#define GTP_NPDU_NUM_PRESENT (1<<1)
#define GTP_NEXT_EXTN_HDR_PRESENT (1<<2)
#define GTP_HDR_SEQ_NUM_LENGTH 2
#define GTP_HDR_NPDU_NUM_LENGTH 1
#define GTP_HDR_NXT_EXTN_HDR_FIELD_LENGTH 1
#define GTP_HDR_MANDATORY_FIELD_LENGTH 8 // min GTP header length is 8 B
#define GTP_HDR_OPTIONAL_FIELD_LENGTH 4


struct gtpHeader {
    
    /* Mandatory fields */
    uint8_t flags;
    uint8_t msgType;
    uint16_t length; // length in octets of payload
    uint32_t teid;

    /* Optional fields */
    //uint32_t presenceMask;
    uint16_t seqNo;
    uint8_t npduNum;
    uint8_t nxtExtnHdrType;
    
    gtpHeader() {
        this->flags = 0;
        this->msgType = 0;
        this->length = 0;
        this->teid = 0;
        //this->presenceMask = 0;
        this->seqNo = 0;
        this->npduNum = 0;
        this->nxtExtnHdrType = 0;
    }
};

/* Encode/Decode wrapper functions for GTP header*/
int decodeGtpHeader(uint8_t *buffer, gtpHeader *msgStruct,
                uint32_t bufLen, uint32_t *decodedLen);
int encodeGtpHeader(uint8_t *buffer, gtpHeader *msgStruct,
                uint32_t bufLen, uint32_t *encodedLen);

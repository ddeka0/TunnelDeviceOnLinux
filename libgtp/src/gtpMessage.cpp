#include<bits/stdc++.h>
#include "gtpMessage.h"
#define SUCCESS 0
#define FAILURE (-1)

int encodeGtpMessage(uint8_t *buffer, uint32_t bufLen, gtpMessage *msgStruct, 
        uint32_t *encodedLen)
{
    unsigned int len = 0;
    uint32_t encLen = 0;

    if(!buffer || !msgStruct || !encodedLen)
    {
        std::cout<<"Invalid input parameters"<<std::endl;
        return FAILURE;
    }
    
    if(len+sizeof(msgStruct->gtp_header) > bufLen)  // TODO correct this check
    {
        std::cout<<"Incomplete buffer length for GTP header"<<std::endl;
        return FAILURE;
    }

    if(encodeGtpHeader(buffer+len,&msgStruct->gtp_header,bufLen-len, &encLen)
            == FAILURE)
    {
        std::cout<<"Header encoding failed"<<std::endl;
        return FAILURE;
    }
    len+=encLen;
    printf("len = %d, encLen = %d\n", len, encLen);
    if(len+msgStruct->payloadLength > bufLen)
    {
        std::cout<<"Incomplete buffer length for GTP payload"<<std::endl;
        return FAILURE;
    }
    memcpy(buffer+len, msgStruct->payload, msgStruct->payloadLength);
    len+=msgStruct->payloadLength;

    *encodedLen = len;
    printf("encodeGtpMessage : encoded length = %d, SUCCESS\n", len);
    return SUCCESS;
}


/*  1.buffer = input buffer
*   2. msgStruct = empty gtp message
*   3. bufLen = length to decode
*/
int decodeGtpMessage(uint8_t *buffer, gtpMessage *msgStruct, uint32_t bufLen)
{
    unsigned int len = 0;
    uint32_t consumedLen = 0;

    if(!buffer || !msgStruct)
    {
        std::cout<<"Invalid input parameters"<<std::endl;
        return FAILURE;
    }
    if(len + sizeof(msgStruct->gtp_header) > bufLen)
    {
        std::cout<<"Incomplete buffer length for GTP header"<<std::endl;
        return FAILURE;        
    }
    if(decodeGtpHeader(buffer + len,&(msgStruct->gtp_header),bufLen - len, 
            &consumedLen) == FAILURE)
    {
        std::cout << "decodeGtpHeader failed" << std::endl;
        return FAILURE;        
    }
    len+=consumedLen;

    uint32_t gtpHdrOptionalFieldLength = 
            consumedLen - GTP_HDR_MANDATORY_FIELD_LENGTH;
    uint32_t gtpPayloadLength = 
            msgStruct->gtp_header.length - gtpHdrOptionalFieldLength;
    if(len + gtpPayloadLength > bufLen)
    {
        std::cout<<"Incomplete buffer length for payload"<<std::endl;
        return FAILURE;        
    }
    memcpy(&msgStruct->payload, buffer+len, gtpPayloadLength);
    msgStruct->payloadLength = gtpPayloadLength;
    std::cout << "GTP payload length = "<<gtpPayloadLength<<std::endl;

    return SUCCESS;
}

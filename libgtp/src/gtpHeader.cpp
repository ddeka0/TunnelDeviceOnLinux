#include<bits/stdc++.h>
#include "gtpHeader.h"
#include <netinet/in.h>
#define SUCCESS 0
#define FAILURE (-1)


int encodeGtpHeader(uint8_t *buffer, gtpHeader *msgStruct, uint32_t bufLen, 
                    uint32_t *encodedLen)
{
    unsigned int len = 0;

    if(!buffer || !msgStruct || !encodedLen)
    {
        std::cout<<"Invalid input parameters"<<std::endl;
        return FAILURE;
    }

    if(len+ sizeof(msgStruct->flags) > bufLen)
    {
        std::cout<<"Incomplete buffer length for flags"<<std::endl;
        return FAILURE;
    }
    memcpy(buffer+len, &msgStruct->flags, sizeof(msgStruct->flags));
    len+=sizeof(msgStruct->flags);

    if(len+ sizeof(msgStruct->msgType) > bufLen)
    {
        std::cout<<"Incomplete buffer length for msgType"<<std::endl;
        return FAILURE;
    }
    memcpy(buffer+len, &msgStruct->msgType, sizeof(msgStruct->msgType));
    len+=sizeof(msgStruct->msgType);

    if(len+ sizeof(msgStruct->length) > bufLen)
    {
        std::cout<<"Incomplete buffer length for length"<<std::endl;
        return FAILURE;
    }

    msgStruct->length = htons(msgStruct->length);
    memcpy(buffer+len, &msgStruct->length, sizeof(msgStruct->length));
    len+=sizeof(msgStruct->length);

    if(len+ sizeof(msgStruct->teid) > bufLen)
    {
        std::cout<<"Incomplete buffer length for teid"<<std::endl;
        return FAILURE;
    }
    msgStruct->teid = htonl(msgStruct->teid);
    memcpy(buffer+len, &msgStruct->teid, sizeof(msgStruct->teid));
    len+=sizeof(msgStruct->teid);

    /* 
    * Encoding optional fields
    * As per spec, even if only 1 optional field is present, we have to add
    * all optioanl fields (remaining optional fields will be set to 0), and
    * header length will increase by 4 Bytes.
    */

    if(msgStruct->flags & (GTP_S_MASK|GTP_PN_MASK|GTP_E_MASK))
    {
        if (msgStruct->flags & GTP_S_MASK) // seq num present
        {
            if(len+ sizeof(msgStruct->seqNo) > bufLen)
            {
                std::cout<<"Incomplete buffer length for seq num"<<std::endl;
                return FAILURE;
            }
            msgStruct->seqNo = htons(msgStruct->seqNo);
        }
        memcpy(buffer+len, &msgStruct->seqNo, sizeof(msgStruct->seqNo));
        len+=sizeof(msgStruct->seqNo);
        

        if (msgStruct->flags & GTP_PN_MASK) // npdu num present
        {
            if(len+ sizeof(msgStruct->npduNum) > bufLen)
            {
                std::cout<<"Incomplete buffer length for npdu num"<<std::endl;
                return FAILURE;
            }
        }
        memcpy(buffer+len, &msgStruct->npduNum, sizeof(msgStruct->npduNum));
        len+=sizeof(msgStruct->npduNum);
        

        if (msgStruct->flags & GTP_E_MASK) // next extn hdr present
        {
            if(len+ sizeof(msgStruct->nxtExtnHdrType) > bufLen)
            {
                std::cout<<"Incomplete buffer length for next extn hdr"<<std::endl;
                return FAILURE;
            }
        }
        memcpy(buffer+len, &msgStruct->nxtExtnHdrType, sizeof(msgStruct->nxtExtnHdrType));
        len+=sizeof(msgStruct->nxtExtnHdrType);        
    }

    *encodedLen = len;
    printf("encodeGtpHeader : encoded length = %d, SUCCESS\n", len);
    return SUCCESS;
}
int decodeGtpHeader(uint8_t *buffer, gtpHeader *msgStruct, uint32_t bufLen, 
    uint32_t *decodedLen)
{
    unsigned int len = 0;

    if(!buffer || !msgStruct)
    {
        std::cout<<"Invalid input parameters"<<std::endl;
        return FAILURE;
    }

    if(len+sizeof(msgStruct->flags) > bufLen)
    {
        std::cout<<"Incomplete buffer length for flags"<<std::endl;
        return FAILURE;
    }
    memcpy(&msgStruct->flags, buffer+len, sizeof(msgStruct->flags));
    len += sizeof(msgStruct->flags);

    if(len+sizeof(msgStruct->msgType) > bufLen)
    {
        std::cout<<"Incomplete buffer length for msgType"<<std::endl;
        return FAILURE;
    }
    memcpy(&msgStruct->msgType, buffer+len, sizeof(msgStruct->msgType));
    len += sizeof(msgStruct->msgType);

    if(len+sizeof(msgStruct->length) > bufLen)
    {
        std::cout<<"Incomplete buffer length for length"<<std::endl;
        return FAILURE;
    }
    memcpy(&msgStruct->length, buffer+len, sizeof(msgStruct->length));
    msgStruct->length = ntohs(msgStruct->length);
    len += sizeof(msgStruct->length);

    if(len+sizeof(msgStruct->teid) > bufLen)
    {
        std::cout<<"Incomplete buffer length for teid"<<std::endl;
        return FAILURE;
    }
    memcpy(&msgStruct->teid, buffer+len, sizeof(msgStruct->teid));
    msgStruct->teid = ntohl(msgStruct->teid);
    len += sizeof(msgStruct->teid);

    if(msgStruct->flags & (GTP_S_MASK|GTP_PN_MASK|GTP_E_MASK))
    {
        if(msgStruct->flags & GTP_S_MASK)
        {
            if(len+sizeof(msgStruct->seqNo) > bufLen)
            {
                std::cout<<"Incomplete buffer length for seqNo"<<std::endl;
                return FAILURE;
            }
            memcpy(&msgStruct->seqNo, buffer+len, sizeof(msgStruct->seqNo));
            msgStruct->seqNo = ntohs(msgStruct->seqNo);            
        }
        len += sizeof(msgStruct->seqNo);

        if(msgStruct->flags & GTP_PN_MASK)
        {
            if(len+sizeof(msgStruct->npduNum) > bufLen)
            {
                std::cout<<"Incomplete buffer length for npduNum"<<std::endl;
                return FAILURE;
            }
            memcpy(&msgStruct->npduNum, buffer+len, sizeof(msgStruct->npduNum));
        }
        len += sizeof(msgStruct->npduNum);        

        if(msgStruct->flags & GTP_E_MASK)
        {
            if(len+sizeof(msgStruct->nxtExtnHdrType) > bufLen)
            {
                std::cout<<"Incomplete buffer length for nxtExtnHdrType"<<std::endl;
                return FAILURE;
            }
            memcpy(&msgStruct->nxtExtnHdrType, buffer+len, sizeof(msgStruct->nxtExtnHdrType));    
        }
        len += sizeof(msgStruct->npduNum);
    }
    *decodedLen = len;
    return SUCCESS;
}




#include "protocol.h"
#include <climits>
#include <cstdlib>
#include <cstring>

PDU *mkPDU(uint uiMsgLen)
{
    uint uiPduLen = sizeof(PDU) + uiMsgLen + 1;
    PDU * pdu = (PDU*)malloc(uiPduLen);
    if(!pdu){
        exit(-1);
    }
    memset(pdu,0,uiPduLen);
    pdu->uiPDULen = uiPduLen;
    pdu->uiMsgLen = uiMsgLen;
    return pdu;
}

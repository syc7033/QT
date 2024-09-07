#ifndef MSGHANDLER_H
#define MSGHANDLER_H

#include "protocol.h"

#include <QString>



class MsgHandler
{
public:
    MsgHandler();
    void registerFuncResp(PDU* pdu);
    void loginFuncResp(PDU* pdu);
    void findUserFuncResp(PDU* pdu);
    void onlineUserFuncResp(PDU* pdu, QString strUserName);
    void addFriendFuncResp(PDU* pdu);
    void addFriendFuncRqs(PDU* pdu);
    void agreeAddFriendFuncRqs(PDU* pdu);
    void refreshFriendFuncResp(PDU* pdu);
    void delFriendFuncResp(PDU* pdu);
    void chatFuncRqs(PDU* pdu);
    void mkDirResp(PDU* pdu);
    void refreshFileResp(PDU* pdu);
    void delDirResp(PDU* pdu);
    void delFileResp(PDU* pdu);
    void renameFileResp(PDU* pdu);
    void moveFileResp(PDU* pdu);
    void uploadFileResp(PDU* pdu);
    void uploadFileDataResp(PDU* pdu);
    void shareFileResp();
    void shareFileReq(PDU* pdu);
    void shareFileAgreeResp(PDU* pdu);
};

#endif // MSGHANDLER_H

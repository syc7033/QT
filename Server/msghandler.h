#ifndef MSGHANDLER_H
#define MSGHANDLER_H

#include "protocol.h"
#include <QFile>
#include <QString>


class MsgHandler
{
public:
    MsgHandler();
    PDU* registerFunc(PDU* pdu);
    PDU* loginFunc(PDU* pdu, QString& strLoginUsername);
    PDU* findUserFunc(PDU* pdu);
    PDU* onlineUserFunc();
    PDU* addFriendFunc(PDU* pdu);
    PDU* agreeAddFriendFunc(PDU* pdu);
    PDU* freshOnlineFriendFunc(PDU* pdu);
    PDU* delOnlineFriendFunc(PDU* pdu);
    PDU* mkDirFunc(PDU* pdu);
    void chatFunc(PDU* pdu);
    PDU* refreshFileFunc(PDU* pdu);
    PDU* delDirFunc(PDU* pdu);
    PDU* delFileFunc(PDU* pdu);
    PDU* renameFileFunc(PDU* pdu);
    PDU* moveFileFunc(PDU* pdu);
    PDU* uploadFileFunc(PDU* pdu);
    PDU* uploadFileDataFunc(PDU* pdu);
    PDU* shareFileFunc(PDU* pdu);
    PDU* shareFileAgreeFunc(PDU* pdu);
private:
    QFile m_fUploadFile;
    bool m_bUpload = false;  // 是否正在上传
    qint64 m_iUploadTotal;
    qint64 m_iUploadReveived;
};

#endif // MSGHANDLER_H

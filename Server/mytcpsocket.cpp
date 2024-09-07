 #include "mytcpsocket.h"
#include "protocol.h"
#include "operatordb.h"
MyTcpSocket::MyTcpSocket()
{
    msgHandler = new MsgHandler();
    connect(this,&MyTcpSocket::readyRead,this,&MyTcpSocket::receiveMsg);
    connect(this,&MyTcpSocket::disconnected,this,&MyTcpSocket::clientOffLine);
}

void MyTcpSocket::receiveMsg()
{
//    PDU* pdu = receivePDU();
//    PDU* resPdu = handlePDU(pdu);
//    respPDU(resPdu);
//    free(pdu);
//    pdu = nullptr;
    qDebug() << "MyTcpSocket::receiveMsg" << "接收消息的长度：" << this->bytesAvailable();
    QByteArray data = this->readAll(); // 读出socket中的全部数据
    buffer.append(data); // 正常来讲如果没有半包问题 buffer中因该是没有数据的，反之说明buffer中存在半包问题

    // 循环处理沾包数据
    while(buffer.size() >= int(sizeof(PDU))){
        // 选判断pdu的合法性读取
        PDU* pdu = (PDU*)buffer.data();
        if(buffer.size() < int(pdu->uiPDULen)){
            // 存在半包问题
            break;
        }
        // 肯定存在一个完整的包
        qDebug() << "receivePDU::" << "uiMsgType " << pdu->uiMsgType;

        PDU* resPdu = handlePDU(pdu);
        respPDU(resPdu);
        buffer.remove(0, pdu->uiPDULen);
    }
}


PDU *MyTcpSocket::handlePDU(PDU *pdu)
{
    qDebug() << "进入了handlePDU方法";
    PDU* resPdu = nullptr;
    //进行消息类型的判定
    switch(pdu->uiMsgType){
    case ENUM_MSG_TYPE_REGIST_REQUST:{
        return msgHandler->registerFunc(pdu);
    }
    case ENUM_MSG_TYPE_LOGIN_REQUST:{
        return msgHandler->loginFunc(pdu, m_strLoginUsername);
    }
    case ENUM_MSG_TYPE_FIND_USER_REQUST:{
        return msgHandler->findUserFunc(pdu);
    }
    case ENUM_MSG_TYPE_ONLINE_USER_REQUST:{
        return msgHandler->onlineUserFunc();
    }
    case ENUM_MSG_TYPE_ADD_FRIEND_REQUST:{
        return msgHandler->addFriendFunc(pdu);
    }
    case ENUM_MSG_TYPE_AGREE_ADD_FRIEND_REQUST:{
        return msgHandler->agreeAddFriendFunc(pdu);
    }
    case ENUM_MSG_TYPE_REFRESH_ONLINE_FRIEND_REQUST:{
        return msgHandler->freshOnlineFriendFunc(pdu);
    }
    case ENUM_MSG_TYPE_DEL_ONLINE_FRIEND_REQUST:{
        return msgHandler->delOnlineFriendFunc(pdu);
    }
    case ENUM_MSG_TYPE_CHAT_REQUST:{
        msgHandler->chatFunc(pdu);
        break;
    }
    case ENUM_MSG_TYPE_MKDIR_REQUST:{
        return msgHandler->mkDirFunc(pdu);
    }
    case ENUM_MSG_TYPE_REFRESH_FILE_REQUST:{
        return msgHandler->refreshFileFunc(pdu);
    }
    case ENUM_MSG_TYPE_DEL_DIR_REQUST:{
        return msgHandler->delDirFunc(pdu);
    }
    case ENUM_MSG_TYPE_DEL_FILE_REQUST:{
        return msgHandler->delFileFunc(pdu);
    }
    case ENUM_MSG_TYPE_RENAME_FILE_REQUST:{
        return msgHandler->renameFileFunc(pdu);
    }
    case ENUM_MSG_TYPE_MOVE_FILE_REQUST:{
        return msgHandler->moveFileFunc(pdu);
    }
    case ENUM_MSG_TYPE_UPLOAD_FILE_REQUST:{
        return msgHandler->uploadFileFunc(pdu);
    }
    case ENUM_MSG_TYPE_UPLOAD_FILE_DATA_REQUST:{
        return msgHandler->uploadFileDataFunc(pdu);
    }
    case ENUM_MSG_TYPE_SHARED_FILE_DATA_REQUST:{
        return msgHandler->shareFileFunc(pdu);
    }
    case ENUM_MSG_TYPE_AGREE_SHARED_FILE_REQUST:{
        return msgHandler->shareFileAgreeFunc(pdu);
    }
        default:
            break;
    }
    return resPdu;
}

void MyTcpSocket::respPDU(PDU *resPdu)
{
    if(resPdu == nullptr){
        return;
    }
    write((char*)resPdu, resPdu->uiPDULen);
    free(resPdu);
    resPdu = nullptr;
}



MyTcpSocket::~MyTcpSocket()
{
    delete msgHandler;
}

void MyTcpSocket::clientOffLine()
{
    qDebug() << "clientOffLine::" << "执行了该函数";
    //客户端下线要干俩件事，数据库online变为0，tcpServer里面的socet列表清除对应socket
    OperatorDB::getInstance().DBHandleOffLine(m_strLoginUsername);
    emit removeOffLineTcpSocket_Signal(this);
}

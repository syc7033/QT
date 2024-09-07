#include "msghandler.h"
#include "operatordb.h"
#include "mytcpserver.h"
#include "server.h"

#include <QDebug>
#include <QDir>
#include <QMessageBox>

MsgHandler::MsgHandler()
{

}

PDU *MsgHandler::registerFunc(PDU *pdu)
{
    char caUserName[32]={'\0'};
    char caPwd[32]={'\0'};
    memcpy(caUserName, pdu->caData, 32);
    memcpy(caPwd, pdu->caData + 32, 32);
    qDebug() << "caUserName " << caUserName;
    qDebug() << "caPwd " << caPwd;

    bool ret = OperatorDB::getInstance().DBHandleRegist(caUserName,caPwd);
    qDebug() << "OperatorDB:: " << ret;
    if(ret){
        QDir dir;
        qDebug() << "MsgHandler::registerFunc::" << "初始化每个用户的根目录为：" << QString("%1/%2").arg(Server::getInstance().getRootPath()).arg(caUserName);
        dir.mkdir(QString("%1/%2").arg(Server::getInstance().getRootPath()).arg(caUserName));
    }
    PDU* resPdu = mkPDU(0);
    resPdu->uiMsgType = ENUM_MSG_TYPE_REGIST_RESPONSE;
    qDebug() << "REGIST_RESPONSE "<< "resPdu->uiMsgType" <<resPdu->uiMsgType;
    qDebug() << "REGIST_RESPONSE "<< "resPdu->uiPduLen" <<resPdu->uiPDULen;
    memcpy(resPdu->caData, &ret, sizeof(bool));
    return resPdu;
}

PDU *MsgHandler::loginFunc(PDU *pdu, QString &strLoginUsername)
{
    char caUserName[32]={'\0'};
    char caPwd[32]={'\0'};
    memcpy(caUserName, pdu->caData, 32);
    memcpy(caPwd, pdu->caData + 32, 32);
    qDebug() << "LOGIN_REQUST " << "caUserName " << caUserName;
    qDebug() << "LOGIN_REQUST " << "caPwd " << caPwd;
    bool ret = OperatorDB::getInstance().DBHandleLogin(caUserName,caPwd);
    if(ret){
        strLoginUsername = caUserName;
    }
    qDebug() << "LOGIN_REQUST  " << "DBHandleLogin:: " << "ret " << ret;

    PDU* resPdu = mkPDU(0);
    resPdu->uiMsgType = ENUM_MSG_TYPE_LOGIN_RESPONSE;
    qDebug() << "LOGIN_REQUST " << "resPdu->uiMsgType" <<resPdu->uiMsgType;
    memcpy(resPdu->caData, &ret, sizeof(bool));
    return resPdu;
}


PDU *MsgHandler::findUserFunc(PDU *pdu)
{
    QString strUserName = pdu->caData;
    qDebug() << "ENUM_MSG_TYPE_FIND_USER_REQUST " <<"UserName " << strUserName;
    int ret = OperatorDB::getInstance().DBHandleFindUser(strUserName);
    qDebug() << "ENUM_MSG_TYPE_FIND_USER_REQUST " <<"ret " << ret;
    PDU* resPdu = mkPDU(0);
    resPdu->uiMsgType = ENUM_MSG_TYPE_FIND_USER_RESPONSE;
    memcpy(resPdu->caData, &ret, sizeof(int));
    memcpy(resPdu->caData + 32, strUserName.toStdString().c_str(), 32);
    return resPdu;
}

PDU *MsgHandler::onlineUserFunc()
{
    QStringList strList = OperatorDB::getInstance().DBHandleOnlieUser();
    uint uiMsgLen = 32 * strList.size();
    PDU* resPdu = mkPDU(uiMsgLen);
    resPdu->uiMsgType = ENUM_MSG_TYPE_ONLINE_USER_RESPONSE;
    qDebug() << "ENUM_MSG_TYPE_ONLINE_USER_RESPONSE::" << "uiMsgType " << resPdu->uiMsgType;
    for(int i = 0; i < strList.size(); i++){
        qDebug() << "ENUM_MSG_TYPE_ONLINE_USER_RESPONSE::" << "UserName " << strList[i];
        memcpy(resPdu->caMsg + i * 32, strList[i].toStdString().c_str(), 32);
    }
    return resPdu;
}

PDU *MsgHandler::addFriendFunc(PDU *pdu)
{
    char caCurName[32]={'\0'};
    char caTrgName[32]={'\0'};
    memcpy(caCurName, pdu->caData, 32);
    memcpy(caTrgName, pdu->caData + 32, 32);
    qDebug() << "addFriendFunc::" << "caCurName " << caCurName;
    qDebug() << "addFriendFunc::" << "caTrgName " << caTrgName;
    int ret = OperatorDB::getInstance().DBHandleAddFriend(caCurName,caTrgName);
    qDebug() << "addFriendFunc::" << "DB::ret " << ret;
    if(ret == 1){//不是好友，需要向目标客户端发送加添好友请求
        MyTcpServer::getInstance().resend(caTrgName, pdu);
    }
    PDU* resPdu = mkPDU(0);
    resPdu->uiMsgType = ENUM_MSG_TYPE_ADD_FRIEND_RESPONSE;
    memcpy(resPdu->caData, &ret, sizeof(int));
    return resPdu;
}

PDU *MsgHandler::agreeAddFriendFunc(PDU *pdu)
{
    char caCurName[32] = {'\0'};
    char caTrgName[32] = {'\0'};
    memcpy(caCurName, pdu->caData, 32);
    memcpy(caTrgName, pdu->caData + 32, 32);
    qDebug() << "agreeAddFriendFunc::" << "caUserName" << caCurName;
    qDebug() << "agreeAddFriendFunc::" << "caFriendName" << caTrgName;
    bool ret = OperatorDB::getInstance().DBHandleAgreeAddFriend(caCurName, caTrgName);
    qDebug() << "agreeAddFriendFunc::" << "ret" << ret;
    PDU* resPdu = mkPDU(0);
    resPdu->uiMsgType = ENUM_MSG_TYPE_AGREE_ADD_FRIEND_RESPONSE;
    memcpy(resPdu->caData, &ret,sizeof(bool));
    MyTcpServer::getInstance().resend(caCurName, resPdu);
    return resPdu;
}

PDU *MsgHandler::freshOnlineFriendFunc(PDU *pdu)
{
    char caUserName[32] = {'\0'};
    memcpy(caUserName, pdu->caData, 32);
    qDebug() << "MsgHandler::freshFriendFunc::" << "caUserName " <<caUserName;
    QStringList strFriendList = OperatorDB::getInstance().DBHandleRefreshOnlineFriend(caUserName);
    for(int i = 0; i < strFriendList.size(); i++){
        qDebug() << "MsgHandler::freshFriendFunc" << "strFriendName" << strFriendList[i];
    }
    uint uiMsgLen = strFriendList.size() * 32;
    PDU* resPdu = mkPDU(uiMsgLen);
    resPdu->uiMsgType = ENUM_MSG_TYPE_REFRESH_ONLINE_FRIEND_RESPONSE;
    for(int i = 0; i < strFriendList.size(); i++){
        memcpy(resPdu->caMsg + i * 32, strFriendList[i].toStdString().c_str(), 32);
    }
    return resPdu;
}

PDU *MsgHandler::delOnlineFriendFunc(PDU *pdu)
{
    char caCurName[32] = {'\0'};
    char caDelName[32] = {'\0'};
    memcpy(caCurName, pdu->caData, 32);
    memcpy(caDelName, pdu->caData + 32, 32);
    qDebug() << "MsgHandler::delOnlineFriendFunc" << "caCurName " << caCurName;
    qDebug() << "MsgHandler::delOnlineFriendFunc" << "caDelName " << caDelName;
    int ret = OperatorDB::getInstance().DBHandleAddFriend(caCurName, caDelName);
    qDebug() << "MsgHandler::delOnlineFriendFunc::DBHandleAddFriend" << "ret" << ret;
    PDU* resPdu = mkPDU(0);
    resPdu->uiMsgType = ENUM_MSG_TYPE_DEL_ONLINE_FRIEND_RESPONSE;
    if(ret == -2){
        qDebug() << "MsgHandler::delOnlineFriendFunc" << "caDelName 是你的好友准备删除";
        int ret1 = OperatorDB::getInstance().BDHandleDelFriend(caCurName, caDelName);
        memcpy(resPdu->caData, &ret1, sizeof(bool));
    }
    return resPdu;

}

void MsgHandler::chatFunc(PDU *pdu)
{
    char caTrgName[32] = {'\0'};
    memcpy(caTrgName, pdu->caData + 32, 32);
    qDebug() << "MsgHandler::chatFunc" << "目标用户是 " << caTrgName;
    MyTcpServer::getInstance().resend(caTrgName, pdu);
}

PDU *MsgHandler::refreshFileFunc(PDU *pdu)
{
    QString strCurPath = pdu->caMsg;
    qDebug() << "MsgHandler::refreshFileFunc" << "当前路径 " << strCurPath;
    QDir dir(strCurPath);
    QFileInfoList fileInfoList =  dir.entryInfoList();
    int iFileCount = fileInfoList.size();
    qDebug() << "MsgHandler::refreshFileFunc" << "文件数量 " << iFileCount;
    PDU* resPdu = mkPDU((iFileCount-2) * sizeof(FileInfo));
    resPdu->uiMsgType = ENUM_MSG_TYPE_REFRESH_FILE_RESPONSE;
    FileInfo* pfileInfo = nullptr;
    QString strFileName;
    int continueCnt = 0;
    for(int i = 0; i < iFileCount; i++){

        strFileName = fileInfoList[i].fileName();
        if(strFileName == "." || strFileName == ".."){
            continueCnt++;
            continue;
        }
        pfileInfo = (FileInfo*)resPdu->caMsg + i - continueCnt;
        qDebug() << "MsgHandler::refreshFileFunc" << "文件名字 " << strFileName;
        memcpy(pfileInfo->caFileName, strFileName.toStdString().c_str(), strFileName.toStdString().size());
        if(fileInfoList[i].isDir()){
            pfileInfo->uiFileType = 0; // 0 目录
        }else{
            pfileInfo->uiFileType = 1; // 1 文件
        }
    }
    return resPdu;
}

PDU *MsgHandler::delDirFunc(PDU *pdu)
{

    QString strDelFilePath = pdu->caMsg;
//    QFileInfo fileInfo(strDelFilePath);
    qDebug() << "MsgHandler::delFileFunc" << "删除文件的路径 " << strDelFilePath;
    QDir dir(strDelFilePath);
    bool ret = dir.removeRecursively();
    PDU* resPdu = mkPDU(0);
    resPdu->uiMsgType = ENUM_MSG_TYPE_DEL_DIR_RESPONSE;
    memcpy(resPdu->caData, &ret, sizeof(bool));
    return resPdu;
}

PDU *MsgHandler::delFileFunc(PDU *pdu)
{
    QString strCurPath = pdu->caMsg;
    char caDelName[32] = {'\0'};
    memcpy(caDelName, pdu->caData, 32);
    qDebug() << "MsgHandler::delFileFunc" << "当前路径为 " << strCurPath;
    QDir dir(strCurPath);
    bool ret = dir.remove(caDelName);
    PDU* resPdu = mkPDU(0);
    resPdu->uiMsgType = ENUM_MSG_TYPE_DEL_FILE_RESPONSE;
    memcpy(resPdu->caData, &ret, sizeof(bool));
    return resPdu;
}

PDU *MsgHandler::renameFileFunc(PDU *pdu)
{
    QString strCurPath = pdu->caMsg;
    char caOldName[32] = {'\0'};
    char caNewName[32] = {'\0'};
    memcpy(caOldName, pdu->caData, 32);
    memcpy(caNewName, pdu->caData + 32, 32);
    qDebug() << "MsgHandler::renameFileFunc" << "strCurPath " << strCurPath;
    qDebug() << "MsgHandler::renameFileFunc" << "caOldName " << caOldName;
    qDebug() << "MsgHandler::renameFileFunc" << "caNewName " << caNewName;
    QDir dir(strCurPath);
    bool ret = dir.rename(caOldName, caNewName);
    PDU* resPdu = mkPDU(0);
    resPdu->uiMsgType = ENUM_MSG_TYPE_RENAME_FILE_RESPONSE;
    memcpy(resPdu->caData, &ret, sizeof(bool));
    return resPdu;
}

PDU *MsgHandler::moveFileFunc(PDU *pdu)
{
    int srcLen = 0;
    int trgLen = 0;
    memcpy(&srcLen, pdu->caData, sizeof(int));
    memcpy(&trgLen, pdu->caData + 32, sizeof(int));
    qDebug() << "MsgHandler::moveFileFunc" << "srcLen " << srcLen;
    qDebug() << "MsgHandler::moveFileFunc" << "trgLen " << trgLen;
    char* pSrcPath = new char[srcLen + 1];
    char* pTrgPath = new char[trgLen + 1];
    memset(pSrcPath, '\0', srcLen + 1);
    memset(pTrgPath, '\0', trgLen + 1);
    memcpy(pSrcPath, pdu->caMsg, srcLen);
    memcpy(pTrgPath, pdu->caMsg + srcLen, trgLen);
    qDebug() << "MsgHandler::moveFileFunc" << "pSrcPath " << pSrcPath;
    qDebug() << "MsgHandler::moveFileFunc" << "pTrgPath " << pTrgPath;

    bool ret = QFile::rename(pSrcPath, pTrgPath);
    PDU* resPdu = mkPDU(0);
    resPdu->uiMsgType = ENUM_MSG_TYPE_MOVE_FILE_RESPONSE;
    memcpy(resPdu->caData, &ret, sizeof(bool));
    return resPdu;
}

PDU *MsgHandler::uploadFileFunc(PDU *pdu)
{
    qDebug() << "MsgHandler::uploadFileFunc" << "start";

    PDU* resPdu = mkPDU(0);
    resPdu->uiMsgType = ENUM_MSG_TYPE_UPLOAD_FILE_RESPONSE;

    int ret = 0;

    if(m_bUpload){  // 有文件正在上传
        qDebug() << "MsgHandler::uploadFileFunc" << "正在有文件上传";
        ret = 1;
        memcpy(pdu->caData, &ret, sizeof(bool));
        return resPdu;
    }

    char caFileName[32] = {'\0'};
    qint64 iFileSize = 0;
    memcpy(caFileName, pdu->caData, 32);
    qDebug() << "MsgHandler::uploadFileFunc" << "caFileName" << caFileName;
    qDebug() << "MsgHandler::uploadFileFunc" << "pdu->caMsg" << pdu->caMsg;

//    memcpy(&iFileSize, pdu->caData + 32, 32);  错误 如果是这个的话 会把后面的name弄没
    memcpy(&iFileSize, pdu->caData + 32, sizeof(qint64));
    qDebug() << "MsgHandler::uploadFileFunc" << "caFileName" << caFileName;

    QString strPath = QString("%1/%2").arg(pdu->caMsg).arg(caFileName);

    qDebug() << "MsgHandler::uploadFileFunc" << "strPath" << strPath;

    m_fUploadFile.setFileName(strPath);
    if(!m_fUploadFile.open(QIODevice::WriteOnly)){
        qDebug() << "MsgHandler::uploadFileFunc" << "打开文件失败";
        ret = -1;
    }else{
        qDebug() << "MsgHandler::uploadFileFunc" << "打开文件成功，可以开始上传";
        m_bUpload = true;
        m_iUploadTotal = iFileSize;
        m_iUploadReveived = 0;
    }

    memcpy(pdu->caData, &ret, sizeof(int));
    return resPdu;
}

PDU *MsgHandler::uploadFileDataFunc(PDU *pdu)
{
    qDebug() << "MsgHandler::uploadFileDataFunc" << "start";
    m_fUploadFile.write(pdu->caMsg, pdu->uiMsgLen);
    m_iUploadReveived += pdu->uiMsgLen;
    if(m_iUploadReveived < m_iUploadTotal){
        return NULL;
    }
    qDebug() << "MsgHandler::uploadFileDataFunc" << "传输完成 准备返回响应";
    m_fUploadFile.close();
    m_bUpload = false;
    PDU* resPdu = mkPDU(0);
    qDebug() << "m_iUploadTotal" << m_iUploadTotal;
    qDebug() << "m_iUploadReveived" << m_iUploadReveived;
    resPdu->uiMsgType = ENUM_MSG_TYPE_UPLOAD_FILE_DATA_RESPONSE;
    bool ret = m_iUploadTotal == m_iUploadReveived;

    qDebug() << "MsgHandler::uploadFileDataFunc" << "ret" << ret;

    memcpy(resPdu->caData, &ret, sizeof(bool));
    return resPdu;
}

PDU *MsgHandler::shareFileFunc(PDU *pdu)
{
    qDebug() << "MsgHandler::shareFileFunc" << "start";
    // 把好友名从msg中扣掉
    int iFriendCount;
    memcpy(&iFriendCount, pdu->caData + 32, sizeof(int));
    qDebug() << "MsgHandler::shareFileFunc" << "iFriendCount" << iFriendCount;

    PDU* resendPdu = mkPDU(pdu->uiMsgLen - iFriendCount * 32);
    resendPdu->uiMsgType = ENUM_MSG_TYPE_SHARED_FILE_DATA_REQUST;

    memcpy(resendPdu->caMsg, pdu->caMsg + iFriendCount * 32, resendPdu->uiMsgLen);
    memcpy(resendPdu->caData, pdu->caData, 32);

    qDebug() << "MsgHandler::shareFileFunc" << "resendPdu分享文件的路径" << resendPdu->caMsg;
    qDebug() << "MsgHandler::shareFileFunc" << "resendPdu当前用户" << resendPdu->caData;

    char caFriendName[32] = {'\0'};
    for (int i = 0; i < iFriendCount; i++) {
        memcpy(caFriendName, pdu->caMsg + i * 32, 32);
        qDebug() << "MsgHandler::shareFileFunc" << "要转发的好友为" << caFriendName;
        MyTcpServer::getInstance().resend(caFriendName, resendPdu);
    }
    free(resendPdu);
    resendPdu = NULL;

    PDU* resPdu = mkPDU(0);
    resPdu->uiMsgType = ENUM_MSG_TYPE_SHARED_FILE_DATA_RESPONSE;
    return resPdu;
}

PDU *MsgHandler::shareFileAgreeFunc(PDU *pdu)
{
    qDebug() << "strShareFilePath" << pdu->caMsg;
    QString strShareFilePath = pdu->caMsg;
    int index = strShareFilePath.lastIndexOf('/');

    QString strShareName = strShareFilePath.right(strShareFilePath.size() - index -1);
    qDebug() << "root路径" << Server::getInstance().getRootPath();
    qDebug() << "用户路径" << pdu->caData;
    qDebug() << "分享文件的名" << strShareName;
    QString strReceivePath = QString("%1/%2/%3").arg(Server::getInstance().getRootPath()).arg(pdu->caData).arg(strShareName);
    qDebug() << "MsgHandler::shareFileAgreeFunc" << "分享路径为" << strShareFilePath;
    qDebug() << "MsgHandler::shareFileAgreeFunc" << "接收路径为" << strReceivePath;
    QFileInfo fileInfo(strShareFilePath);
    bool ret = true;
    if(fileInfo.isFile()){
        ret = QFile::copy(strShareFilePath, strReceivePath);
        qDebug() << "MsgHandler::shareFileAgreeFunc" << "ret" << ret;
    }
    PDU* resPdu = mkPDU(0);
    resPdu->uiMsgType = ENUM_MSG_TYPE_AGREE_SHARED_FILE_RESPONSE;
    memcpy(resPdu->caData, &ret, sizeof(bool));
    return resPdu;

}


PDU *MsgHandler::mkDirFunc(PDU *pdu)
{
    QString strCurPath = pdu->caMsg;
    char caDirName[32] = {'\0'};
    memcpy(caDirName, pdu->caData, 32);
    qDebug() << "MsgHandler::mkDirFunc " << "caCurPath " << strCurPath;
    qDebug() << "MsgHandler::mkDirFunc " << "caDirName " << caDirName;
    QString strNewDirPath = QString("%1/%2").arg(strCurPath).arg(caDirName);
    qDebug() << "MsgHandler::mkDirFunc " << "新文件的路径 " << strNewDirPath;
    QDir dir;
    PDU* resPdu = mkPDU(0);
    resPdu->uiMsgType = ENUM_MSG_TYPE_MKDIR_RESPONSE;
    bool ret = false;
    if(!dir.exists(strCurPath) || dir.exists(strNewDirPath) || !dir.mkdir(strNewDirPath)){
        memcpy(resPdu->caData, &ret, sizeof(bool));
        return resPdu;
    }
    ret = true;
    memcpy(resPdu->caData, &ret, sizeof(bool));
    return resPdu;
}




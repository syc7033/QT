#include "index.h"
#include "msghandler.h"
#include "client.h"
#include <QMessageBox>

MsgHandler::MsgHandler()
{

}

void MsgHandler::registerFuncResp(PDU *pdu)
{
    bool ret = false;
    memcpy(&ret,pdu->caData,sizeof(bool));
    if(ret){
        QMessageBox::information(&Client::getInstance(), "成功", "注册成功");
    }
    else{
        QMessageBox::information(&Client::getInstance(), "失败", "注册失败");
    }
}

void MsgHandler::loginFuncResp(PDU *pdu)
{
    bool ret = false;
    memcpy(&ret,pdu->caData,sizeof(bool));
    if(ret){
        Index::getInstance().show();
        Client::getInstance().hide();
    }
    else{
        QMessageBox::information(&Client::getInstance(),"失败","登录失败");
    }
}

void MsgHandler::findUserFuncResp(PDU *pdu)
{
    char caUserName[32] = {'\0'};
    memcpy(caUserName, pdu->caData + 32, 32);
    qDebug() << "MsgHandler::findUserFuncResp::" << "caUserName" << caUserName;
    int ret;
    memcpy(&ret, pdu->caData ,sizeof(int));
    if(ret == -1){
        QMessageBox::information(&Client::getInstance(),"错误",QString("用户%1查找失败").arg(caUserName));
    }
    else if(ret == 0){
        QMessageBox::information(&Client::getInstance(),"成功",QString("该用户%1不在线").arg(caUserName));
    }else if(ret == 1){
        int ret = QMessageBox::information(&Client::getInstance(), "成功", QString("该用户%1在线").arg(caUserName), "添加好友", "取消");
        if(ret != 0){
            return;
        }
        PDU* pdu = mkPDU(0);
        pdu->uiMsgType = ENUM_MSG_TYPE_ADD_FRIEND_REQUST;
        memcpy(pdu->caData, Client::getInstance().m_strUsername.toStdString().c_str(), 32);
        memcpy(pdu->caData + 32, caUserName, 32);
        Client::getInstance().sendPDU(pdu);
    }
}

void MsgHandler::onlineUserFuncResp(PDU *pdu,QString strLoginName)
{
    QStringList strUnameList;
    uint cnt = pdu->uiMsgLen / 32;
    char caTmp[32] = {'\0'};
    for(uint i = 0; i < cnt; i++){
        memcpy(caTmp, pdu->caMsg + i * 32, 32);
        if(QString(caTmp) == strLoginName){
            continue;
        }
        strUnameList.append(QString(caTmp)  );
    }
    for(int i = 0; i < strUnameList.size(); i++){
        qDebug() << "ENUM_MSG_TYPE_ONLINE_USER_RESPONSE::" << "name" << strUnameList[i];
    }
    Index::getInstance().getFriend()->getOnlineUser()->updateOnlieUserLW(strUnameList);
}

void MsgHandler::addFriendFuncResp(PDU *pdu)
{
    int ret;
    memcpy(&ret, pdu->caData, sizeof(int));
    qDebug() << "addFriendFuncResp::" << ret;
    if(ret == -1){
        QMessageBox::information(&Index::getInstance(),"搜索","添加好友失败");
    }
    else if(ret == 0){
        QMessageBox::information(&Index::getInstance(),"搜索","当前用户不在线");
    }
    else if(ret == -2){
        QMessageBox::information(&Index::getInstance(),"搜索","目标用户已经是你的好友了");
    }
}

void MsgHandler::addFriendFuncRqs(PDU *pdu)
{

    int ret = QMessageBox::question(&Index::getInstance(), "选择", QString("是否同一%1的添加好友请求？").arg(pdu->caData));
    if(ret == QMessageBox::No){
        return;
    }
    //同意添加好友，向服务器发送同意添加好友的请求
    PDU* agreeAddFriendRqsPdu = mkPDU(0);
    agreeAddFriendRqsPdu->uiMsgType = ENUM_MSG_TYPE_AGREE_ADD_FRIEND_REQUST;
    memcpy(agreeAddFriendRqsPdu->caData, pdu->caData, 64);
    qDebug() << "addFriendFuncRqs::" << "caCurName " << agreeAddFriendRqsPdu->caData;
    qDebug() << "addFriendFuncRqs::" << "caTrgName " << agreeAddFriendRqsPdu->caData + 32;
    Client::getInstance().sendPDU(agreeAddFriendRqsPdu);
}

void MsgHandler::agreeAddFriendFuncRqs(PDU *pdu)
{
    bool ret;
    memcpy(&ret, pdu->caData, sizeof(bool));
    qDebug() << "greeAddFriendFuncRqs::" << "ret " << ret;
    if(ret == false){
        QMessageBox::information(&Index::getInstance(), "失败", "数据库问题");
        return;
    }
    QMessageBox::information(&Index::getInstance(), "成功", "添加好友成功");
    Index::getInstance().getFriend()->refreshOnlineFriend();
}

void MsgHandler::refreshFriendFuncResp(PDU *pdu)
{
    QStringList strFriendList;
    int cnt = pdu->uiMsgLen / 32;
    qDebug() << "MsgHandler::refreshFriendFuncResp" << "cnt " << cnt;
    char caTmp[32] = {'\0'};
    for(int i = 0; i < cnt; i++){
        memcpy(caTmp, pdu->caMsg + i * 32, 32);
        qDebug() << "MsgHandler::refreshFriendFuncResp" << "caTmp " << caTmp;
        strFriendList.append(QString(caTmp));
    }
    qDebug() << "MsgHandler::refreshFriendFuncResp" << "strFriendList";
    for(int i = 0; i < cnt; i++){
        qDebug() << "friendName " << strFriendList[i];
    }
    Index::getInstance().getFriend()->updateFriend_LW(strFriendList);
}

void MsgHandler::delFriendFuncResp(PDU *pdu)
{

    if(pdu->caData[0] == '\0'){
        QMessageBox::information(&Index::getInstance(), "错误", "该用户不是你的好友");
        Index::getInstance().getFriend()->refreshOnlineFriend();
        return;
    }
    QMessageBox::information(&Index::getInstance(), "成功", "删除好友成功");
    Index::getInstance().getFriend()->refreshOnlineFriend();
}

void MsgHandler::chatFuncRqs(PDU *pdu)
{
    Chat* chat = Index::getInstance().getFriend()->getChat();
    if(chat->isHidden()){
        chat->setWindowTitle(Client::getInstance().m_strUsername);
        chat->show();
    }
    char caMsg[32] = {'\0'};
    char caTrgName[32] = {'\0'};
    memcpy(caMsg, pdu->caMsg, pdu->uiMsgLen);
    memcpy(caTrgName, pdu->caData, 32);
    qDebug() << "MsgHandler::chatFuncRqs " << "消息内容为 " << QString("%1: %2").arg(caTrgName).arg(caMsg);
    chat->updateChat_TE(QString("%1: %2").arg(caTrgName).arg(caMsg));
    chat->m_strChatName = QString(caTrgName);
}

void MsgHandler::mkDirResp(PDU *pdu)
{
    bool ret;
    memcpy(&ret, pdu->caData, sizeof(bool));
    if(!ret){
        QMessageBox::information(&Index::getInstance(), "创建文件夹", "创建文件夹失败");
        return;
    }
    QMessageBox::information(&Index::getInstance(), "创建文件夹", "创建文件夹成功");
    Index::getInstance().getFile()->refreshFile();
}

void MsgHandler::refreshFileResp(PDU *pdu)
{
    int iFileNum = pdu->uiMsgLen / sizeof(FileInfo);
    qDebug() << "MsgHandler::refreshFileResp" << "当前路径下的文件数量为 " << iFileNum;
    QList<FileInfo*> lsFileInfoList;
    for(int i = 0; i < iFileNum; i++){
        FileInfo* pFileInfo = new FileInfo();
        memcpy(pFileInfo, pdu->caMsg + i * sizeof(FileInfo), sizeof(FileInfo));
        qDebug() << "MsgHandler::refreshFileResp" << "文件名为 " << pFileInfo->caFileName;
        qDebug() << "MsgHandler::refreshFileResp" << "类型 " << pFileInfo->uiFileType;
        lsFileInfoList.append(pFileInfo);
    }
    Index::getInstance().getFile()->updateFile_LW(lsFileInfoList);
}

void MsgHandler::delDirResp(PDU *pdu)
{
    bool ret;
    memcpy(&ret, pdu->caData, sizeof(bool));
    if(!ret){
        QMessageBox::information(&Index::getInstance(), "成功", "删除文件架失败");
        return;
    }
    QMessageBox::information(&Index::getInstance(), "失败", "删除文件架成功");
    Index::getInstance().getFile()->refreshFile();
}

void MsgHandler::delFileResp(PDU *pdu)
{
    bool ret;
    memcpy(&ret, pdu->caData, sizeof(bool));
    if(!ret){
        QMessageBox::information(&Index::getInstance(), "失败 ", "删除文件失败");
        return;
    }
    QMessageBox::information(&Index::getInstance(), "成功", "删除文件成功");
    Index::getInstance().getFile()->refreshFile();
}

void MsgHandler::renameFileResp(PDU *pdu)
{
    bool ret;
    memcpy(&ret, pdu->caData, sizeof(bool));
    if(!ret){
        QMessageBox::information(&Index::getInstance(), "失败", "重命名文件失败");
        return;
    }
    QMessageBox::information(&Index::getInstance(), "成功", "重命名文件成功");
    Index::getInstance().getFile()->refreshFile();
}

void MsgHandler::moveFileResp(PDU *pdu)
{
    qDebug() << "客户端接收到了响应";
    bool ret;
    memcpy(&ret, pdu->caData, sizeof(bool));
    if(!ret){
        QMessageBox::information(&Index::getInstance(), "失败", "移动文件失败");
        return;
    }
    QMessageBox::information(&Index::getInstance(), "成功", "移动文件成功");
    Index::getInstance().getFile()->refreshFile();
}

void MsgHandler::uploadFileResp(PDU *pdu)
{
    qDebug() << "uploadFileResp start";
    int ret;
    memcpy(&ret, pdu->caData, sizeof(ret));
    if(ret != 0){
        QMessageBox::information(&Index::getInstance(), "上传文件", "上传文件失败");
    }
    else{
        Index::getInstance().getFile()->uploadFile();
    }
}

void MsgHandler::uploadFileDataResp(PDU *pdu)
{
    qDebug() << "uploadFileDataResp start";
    bool ret;

    memcpy(&ret, pdu->caData, sizeof(bool));
    qDebug() << "uploadFileDataResp ret" << ret;
    if(!ret){
        QMessageBox::information(&Index::getInstance(), "上传文件", "上传文件失败");
    }else{
        QMessageBox::information(&Index::getInstance(), "上传文件", "上传文件成功");
    }
    Index::getInstance().getFile()->refreshFile();
}

void MsgHandler::shareFileResp()
{
    qDebug() << "MsgHandler::shareFileResp" << "start";
    QMessageBox::information(Index::getInstance().getFile(), "分享文件", "文件已分享");
}

void MsgHandler::shareFileReq(PDU *pdu)
{
    QString strShareFilePath = pdu->caMsg;
    QString strCurName = pdu->caData;
    int index = strShareFilePath.lastIndexOf('/');
    QString strShareFileName = strShareFilePath.right(strShareFilePath.size() - index -1);
    int ret = QMessageBox::question(&Index::getInstance(), "分享文件", QString("%1 分享文件：%2\n是否接收？").arg(strCurName).arg(strShareFileName));
    if (ret == QMessageBox::No){
        return;
    }
    PDU* resPdu = mkPDU(pdu->uiMsgLen);
    resPdu->uiMsgType = ENUM_MSG_TYPE_AGREE_SHARED_FILE_REQUST;
    QString strUserName = Client::getInstance().m_strUsername;
    memcpy(resPdu->caMsg, pdu->caMsg, pdu->uiMsgLen);
    memcpy(resPdu->caData, strUserName.toStdString().c_str(), strUserName.toStdString().size());
    qDebug() << "MsgHandler::shareFileReq" << "resPdu->caData" << resPdu->caData;
    qDebug() << "MsgHandler::shareFileReq" << "resPdu->caMsg" << resPdu->caMsg;
    Client::getInstance().sendPDU(resPdu);
}

void MsgHandler::shareFileAgreeResp(PDU *pdu)
{
    qDebug() << "shareFileAgreeResp start";
    bool ret;

    memcpy(&ret, pdu->caData, sizeof(bool));
    qDebug() << "shareFileAgreeResp ret" << ret;
    if(!ret){
        QMessageBox::information(&Index::getInstance(), "分享文件", "分享文件失败");
    }else{
        QMessageBox::information(&Index::getInstance(), "分享文件", "分享文件成功");
    }
    Index::getInstance().getFile()->refreshFile();
}



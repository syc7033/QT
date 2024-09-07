#include "client.h"
#include "ui_client.h"
#include "index.h"

#include <QDebug>
#include <QFile>
#include <QHostAddress>
#include <QMessageBox>

Client::Client(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Client)
{
    ui->setupUi(this);
    //加载配置文件 获得ip和端口号 在socket连接服务器中使用
    loadConfig();

    //添加网络连接模块,将客户端和服务器通过tcpSocket建立传送数据的连接
    //将socket连接成功后发送的信号与客户端的槽函数建立连接
    connect(&m_tcpSocket,&QTcpSocket::connected,this,&Client::connectSuccessSlot);
    connect(&m_tcpSocket,&QTcpSocket::readyRead,this,&Client::receiveMsg);
    //socket调用connectToHost函数连接服务器，参数是Ip和端口号
    m_tcpSocket.connectToHost(QHostAddress(m_strIp),m_usPort);
    msgHandler = new MsgHandler();

}


void Client::loadConfig()
{
    //创建QFile对象，传打开配置文件的路径
    QFile file(":/client.config");
    if(!file.open(QIODevice::ReadOnly)){
        qDebug()<<"打开配置文件文件失败";
    }
    else{//调用open函数打开文件
        //调用readAll读取文件中的内容
        QByteArray baData = file.readAll();
        //读取的返回值为字节类型强转为QString类型
        QString strData = QString(baData);
        //逐行读取数据，调用split函数
        QStringList strList = strData.split("\r\n");
        //第一行为ip
        m_strIp = strList[0];
        //第二行为端口号
        m_usPort = strList[1].toUShort();
        m_rootPath = strList[2];
        qDebug() << "loadConfig：Ip：" << m_strIp << "Port" << m_usPort;
        qDebug() << "rootPath：" << m_rootPath;
        //关闭文件
        file.close();
    }
}

void Client::connectSuccessSlot()
{
    qDebug()<<"客户端连接服务器成功";
}

void Client::receiveMsg()
{
//    PDU* pdu = receiveRespPDU();
//    handleRespPDU(pdu);
//    free(pdu);
//    pdu = nullptr;
    QByteArray data = m_tcpSocket.readAll();
    qDebug() << "Client::receiveMsg" << "接收消息的长度：" << m_tcpSocket.bytesAvailable();

    buffer.append(data);
    while(buffer.size() >= int(sizeof(PDU))){
        PDU* pdu = (PDU*)buffer.data();
        if(buffer.size() < int(pdu->uiPDULen)){
            break;
        }
        qDebug() << "Client::receiveMsg" << "接收消息类型" << pdu->uiMsgType;
        handleRespPDU(pdu);
        buffer.remove(0, pdu->uiPDULen);
    }
}

//PDU *Client::receiveRespPDU()
//{
//    uint uiPDULen = 0;
//    m_tcpSocket.read((char*)&uiPDULen,sizeof(uint));
//    PDU* pdu = mkPDU(uiPDULen-sizeof(PDU));
//    m_tcpSocket.read((char*)pdu+sizeof(uint),uiPDULen-sizeof(uint));
//    bool ret;
//    memcpy(&ret,pdu->caData,sizeof(bool));
//    return pdu;
//}

void Client::handleRespPDU(PDU* pdu)
{
    switch (pdu->uiMsgType) {
    case ENUM_MSG_TYPE_REGIST_RESPONSE:{
        msgHandler->registerFuncResp(pdu);
        break;
    }
    case ENUM_MSG_TYPE_LOGIN_RESPONSE:{
        msgHandler->loginFuncResp(pdu);
        break;
    }
    case ENUM_MSG_TYPE_FIND_USER_RESPONSE:{
        msgHandler->findUserFuncResp(pdu);
        break;
    }
    case ENUM_MSG_TYPE_ONLINE_USER_RESPONSE:{
        msgHandler->onlineUserFuncResp(pdu, m_strUsername);
        break;
    }
    case ENUM_MSG_TYPE_ADD_FRIEND_RESPONSE:{
        msgHandler->addFriendFuncResp(pdu);
        break;
    }
    case ENUM_MSG_TYPE_ADD_FRIEND_REQUST:{
        msgHandler->addFriendFuncRqs(pdu);
        break;
    }
    case ENUM_MSG_TYPE_AGREE_ADD_FRIEND_RESPONSE:{
        msgHandler->agreeAddFriendFuncRqs(pdu);
        break;
    }
    case ENUM_MSG_TYPE_REFRESH_ONLINE_FRIEND_RESPONSE:{
        msgHandler->refreshFriendFuncResp(pdu);
        break;
    }
    case ENUM_MSG_TYPE_DEL_ONLINE_FRIEND_RESPONSE:{
        msgHandler->delFriendFuncResp(pdu);
        break;
    }
    case ENUM_MSG_TYPE_CHAT_REQUST:{
        msgHandler->chatFuncRqs(pdu);
        break;
    }
    case ENUM_MSG_TYPE_MKDIR_RESPONSE:{
        msgHandler->mkDirResp(pdu);
        break;
    }
    case ENUM_MSG_TYPE_REFRESH_FILE_RESPONSE:{
        msgHandler->refreshFileResp(pdu);
        break;
    }
    case ENUM_MSG_TYPE_DEL_DIR_RESPONSE:{
        msgHandler->delDirResp(pdu);
        break;
    }
    case ENUM_MSG_TYPE_DEL_FILE_RESPONSE:{
        msgHandler->delFileResp(pdu);
        break;
    }
    case ENUM_MSG_TYPE_RENAME_FILE_RESPONSE:{
        msgHandler->renameFileResp(pdu);
        break;
    }
    case ENUM_MSG_TYPE_MOVE_FILE_RESPONSE:{
        msgHandler->moveFileResp(pdu);
        break;
    }
    case ENUM_MSG_TYPE_UPLOAD_FILE_RESPONSE:{
        msgHandler->uploadFileResp(pdu);
        break;
    }
    case ENUM_MSG_TYPE_UPLOAD_FILE_DATA_RESPONSE:{
        msgHandler->uploadFileDataResp(pdu);
        break;
    }
    case ENUM_MSG_TYPE_SHARED_FILE_DATA_RESPONSE:{
        msgHandler->shareFileResp();
        break;
    }
    case ENUM_MSG_TYPE_SHARED_FILE_DATA_REQUST:{
        msgHandler->shareFileReq(pdu);
        break;
    }
    case ENUM_MSG_TYPE_AGREE_SHARED_FILE_RESPONSE:{
        msgHandler->shareFileAgreeResp(pdu);
        break;
    }
    default:
        break;
    }
}

void Client::sendPDU(PDU *pdu)
{
    qDebug() << "Client::sendPDU" << "uiMsgType" << pdu->uiMsgType;
    qDebug() << "Client::sendPDU" << "参数1" << pdu->caData;
    qDebug() << "Client::sendPDU" << "参数2" << pdu->caData + 32;
    qDebug() << "Client::sendPDU" << "caMsg" << pdu->caMsg;
    m_tcpSocket.write((char*)pdu,pdu->uiPDULen);
    free(pdu);
    pdu = nullptr;
}

QString Client::getRootPath()
{
    return m_rootPath;
}

QTcpSocket& Client::getTcpSocket()
{
    return this->m_tcpSocket;
}

Client &Client::getInstance()
{
    //单例类的全局访问结点，可以返回唯一的单例类的实例
    //使用static是因为静态成员变量只能初始化一次，且C++11之后是前程安全的，不用加锁
    static Client instance;
    return instance;
}

Client::~Client()
{
    delete ui;
    delete msgHandler;
}


void Client::on_registBtn_clicked()
{
    //获取用户名密码
    QString strUserName = ui->userNameLE->text();
    QString strPwd = ui->pwdLE->text();
    if(strUserName.isEmpty() || strPwd.isEmpty() || strUserName.toStdString().size()>32 || strPwd.toStdString().size()>32){
        QMessageBox::information(this,"错误","用户名或密码输入错误！");
        return;
    }
    //构造PDU
    PDU* pdu = mkPDU(0);
    pdu->uiMsgType = ENUM_MSG_TYPE_REGIST_REQUST;
    memcpy(pdu->caData,strUserName.toStdString().c_str(),32);
    memcpy(pdu->caData + 32,strPwd.toStdString().c_str(),32);
    sendPDU(pdu);
}

void Client::on_loginBtn_clicked()
{
    //获取用户名密码
    QString strUserName = ui->userNameLE->text();
    QString strPwd = ui->pwdLE->text();
    if(strUserName.isEmpty() || strPwd.isEmpty() || strUserName.toStdString().size()>32 || strPwd.toStdString().size()>32){
        QMessageBox::information(this,"错误","用户名或密码输入错误！");
        return;
    }
    //用成员变量存储用户名，关闭客户端的时候用
    m_strUsername = strUserName;
    //构造PDU
    PDU* pdu = mkPDU(0);
    pdu->uiMsgType = ENUM_MSG_TYPE_LOGIN_REQUST;
    memcpy(pdu->caData,strUserName.toStdString().c_str(), strUserName.toStdString().size());
    memcpy(pdu->caData + 32,strPwd.toStdString().c_str(), strPwd.toStdString().size());
    qDebug() << "on_loginBtn_clicked::" << "uiMsgType " << pdu->uiMsgType;
    qDebug() << "on_loginBtn_clicked::" << "strUserName " << pdu->caData;
    qDebug() << "on_loginBtn_clicked::" << "strPwd " << pdu->caData + 32;
    sendPDU(pdu);
}



#include "mytcpserver.h"
#include "mytcpsocket.h"

MyTcpServer &MyTcpServer::getInstance()
{
    static MyTcpServer instance;
    return instance;
}

void MyTcpServer::resend(char *trgName, PDU *pdu)
{
    if(trgName == NULL || pdu == NULL){
        return;
    }
    foreach(MyTcpSocket* item,tcpSocketList){
        if(item->m_strLoginUsername == trgName){
            qDebug() << "MyTcpServer::resend::" << "trgName " << trgName;
            item->write((char*)pdu,pdu->uiPDULen);
            break;
        }
    }
}

MyTcpServer::MyTcpServer()
{

}

void MyTcpServer::incomingConnection(qintptr handle)
{

    qDebug()<<"客户端连接服务器成功";
    MyTcpSocket * myTcpSocket = new MyTcpSocket;
    myTcpSocket->setSocketDescriptor(handle);
    tcpSocketList.append(myTcpSocket);
    connect(myTcpSocket,&MyTcpSocket::removeOffLineTcpSocket_Signal,this,&MyTcpServer::removeOffLineTcpSocket_slot);
}

void MyTcpServer::removeOffLineTcpSocket_slot(MyTcpSocket *tcpsocket)
{

    tcpSocketList.removeOne(tcpsocket);
//    qDebug() << "removeOffLineTcpSocket_slot:: " << "AC";
//    qDebug() << "removeOffLineTcpSocket_slot::" << "当前socketList数量 " <<tcpSocketList.size();
//    for(int i = 0; i < tcpSocketList.size(); i++){
//        qDebug() << "removeOffLineTcpSocket_slot::" << tcpSocketList[i]->m_strLoginUsername;
//    }
}

#ifndef MYTCPSERVER_H
#define MYTCPSERVER_H

#include <QObject>
#include <QTcpServer>
#include "mytcpsocket.h"

class MyTcpServer:public QTcpServer
{
    Q_OBJECT
public:
    static MyTcpServer& getInstance();
    QList<MyTcpSocket*> tcpSocketList;
    void resend(char* trgName, PDU* pdu);
private:
    MyTcpServer();
    MyTcpServer(const MyTcpServer& other) = delete;
    MyTcpServer& operator=(const MyTcpServer& other) = delete;
public slots:
    //当连接成功的时候，会调用该槽函数
    void incomingConnection(qintptr handle) override;
    void removeOffLineTcpSocket_slot(MyTcpSocket* tcpsocket);
};

#endif // MYTCPSERVER_H

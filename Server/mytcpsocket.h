#ifndef MYTCPSOCKET_H
#define MYTCPSOCKET_H

#include <QObject>
#include <QTcpSocket>
#include "protocol.h"
#include "msghandler.h"

class MyTcpSocket : public QTcpSocket
{
    Q_OBJECT
public:
    MyTcpSocket();
    QString m_strLoginUsername;
//    PDU* receivePDU();
    void respPDU(PDU* respdu);
    PDU* handlePDU(PDU* pdu);
    MsgHandler* msgHandler;
    QByteArray buffer;
    ~MyTcpSocket();  
public slots:
    void receiveMsg();
    void clientOffLine();
signals:
    void removeOffLineTcpSocket_Signal(MyTcpSocket* tcpSocket);
private:
};

#endif // MYTCPSOCKET_H

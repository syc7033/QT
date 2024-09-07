#ifndef CLIENT_H
#define CLIENT_H

#include <QWidget>
#include <QTcpSocket>
#include "protocol.h"
#include "msghandler.h"

QT_BEGIN_NAMESPACE
namespace Ui { class Client; }
QT_END_NAMESPACE

class Client : public QWidget
{
    Q_OBJECT

public:

    ~Client();
    //加载配置文件
    void loadConfig();
    //全局访问结点获取唯一的实例
    static Client& getInstance();
    QString m_strUsername;
    void handleRespPDU(PDU* pdu);
    void sendPDU(PDU* pdu);
    MsgHandler* msgHandler;
    QString getRootPath();
    QByteArray buffer;
public slots:
    void connectSuccessSlot();
    void receiveMsg();
    QTcpSocket& getTcpSocket();
private slots:

    void on_loginBtn_clicked();

    void on_registBtn_clicked();

private:
    Ui::Client *ui;
    //为了实现单例模式，一个单例类只能有一个实例
    //需要构造函数私有化，删除拷贝构造和赋值运算符的重载
    Client(QWidget *parent = nullptr);
    Client(const Client& other) = delete;
    Client& operator=(const Client& other) = delete;

    //如果不在.pro文件中添加网络模块则无法使用该QTcpSocket类
    QTcpSocket m_tcpSocket;
    QString m_strIp;
    quint16 m_usPort;
    QString m_rootPath;
};
#endif // CLIENT_H

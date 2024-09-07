#include "server.h"
#include "ui_server.h"
#include "mytcpserver.h"
#include "mytcpsocket.h"

#include <QFile>
#include <QTcpServer>

Server::Server(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Server)
{
    ui->setupUi(this);
    //加载配置文件
    loadConfig();
    //自定义的TcpServer类用单例模式创建对象然后调用listen监听是否连接成功
    MyTcpServer::getInstance().listen(QHostAddress(m_strIp),m_usPort);
}

Server &Server::getInstance()
{
    static Server instance;
    return instance;
}

Server::~Server()
{
    delete ui;
}

void Server::loadConfig()
{
    //创建QFile对象，传打开配置文件的路径
    QFile file(":/server.config");
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
        qDebug() << "loadConfig：Ip：" << m_strIp << "Port" << m_usPort << "m_rootPath" << m_rootPath;
        //关闭文件
        file.close();
    }
}

QString &Server::getRootPath()
{
    return m_rootPath;
}


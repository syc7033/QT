#ifndef SERVER_H
#define SERVER_H

#include <QWidget>
#include <protocol.h>

QT_BEGIN_NAMESPACE
namespace Ui { class Server; }
QT_END_NAMESPACE

class Server : public QWidget
{
    Q_OBJECT

public:
    static Server& getInstance();
    ~Server();
    //加载配置文件
    void loadConfig();
    QString& getRootPath();
private:
    Server(QWidget *parent = nullptr);
    Ui::Server *ui;
    QString m_strIp;
    quint16 m_usPort;
    QString m_rootPath;
};
#endif // SERVER_H

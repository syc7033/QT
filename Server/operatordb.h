#ifndef OPERATORDB_H
#define OPERATORDB_H

#include <QObject>
#include <QSqlDatabase>

class OperatorDB : public QObject
{
    Q_OBJECT
public:
    QSqlDatabase m_sqlDB;
    static OperatorDB& getInstance();
    void connect();
    ~OperatorDB(){
        m_sqlDB.close();
    }
    bool DBHandleRegist(char* caUserName, char* caPwd);
    bool DBHandleLogin(char* caUserName, char* caPwd);
    void DBHandleOffLine(QString strUserName);
    int DBHandleFindUser(QString strUserName);
    QStringList DBHandleOnlieUser();
    int DBHandleAddFriend(char* caCurName, char* caTrgName);
    bool DBHandleAgreeAddFriend(char* caUserName, char* caFriendName);
    QStringList DBHandleRefreshOnlineFriend(char* caUserName);
    bool BDHandleDelFriend(char* caCurName, char* caDelName);
private:
    explicit OperatorDB(QObject *parent = nullptr);
    OperatorDB(const QObject& other) = delete;
    OperatorDB& operator=(const QObject& other) = delete;
signals:

};

#endif // OPERATORDB_H

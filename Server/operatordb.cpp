#include "operatordb.h"
#include <QSqlQuery>
#include <QDebug>
#include <QMessageBox>
OperatorDB &OperatorDB::getInstance()
{
    static OperatorDB instance;
    return instance;
}

void OperatorDB::connect()
{
    m_sqlDB.setHostName("localhost");
    m_sqlDB.setPort(3306);
    m_sqlDB.setDatabaseName("mydb2309");
    m_sqlDB.setUserName("root");
    m_sqlDB.setPassword("123456");
    if(!m_sqlDB.open()){
        qDebug() << "连接数据库成功";
    }
    else{
        qDebug() << "连接数据库成功";
    }

}

bool OperatorDB::DBHandleRegist(char *caUserName, char *caPwd)
{
    //判空
    if(caUserName == NULL || caPwd == NULL)
        return false;
    //查询用户是否存在
    QString strSql = QString("select * from user_info where name = '%1'").arg(caUserName);
    QSqlQuery query;
    if(!query.exec(strSql) || query.next())
        return false;
    strSql = QString("insert into user_info(name,pwd) value('%1','%2')").arg(caUserName).arg(caPwd);
    return query.exec(strSql);
}

bool OperatorDB::DBHandleLogin(char *caUserName, char *caPwd)
{
    //判空
    if(caUserName == NULL || caPwd == NULL)
        return false;
    QString strSql = QString("select * from user_info where name = '%1' and pwd = '%2' and online = 0").arg(caUserName).arg(caPwd);
    QSqlQuery query;
    //如果执行不成功或者没有结果集就返回false
    if(!query.exec(strSql) || !query.next())
        return false;
    strSql = QString("update user_info set online = 1 where name = '%1';").arg(caUserName);
    return query.exec(strSql);
}

void OperatorDB::DBHandleOffLine(QString strUserName)
{
    if(strUserName.isEmpty()) return;
    QString strSql = QString("update user_info set online = 0 where name = '%1'").arg(strUserName);
    QSqlQuery query;
    bool ret = query.exec(strSql);
    if(ret) qDebug() << "DBHandleOffLine 成功";
    else qDebug() << "DBHandleOffLine 失败";
}

int OperatorDB::DBHandleFindUser(QString strUserName)
{
    if(strUserName.isEmpty()) return -1;//异常结束
    QString strSql = QString("select online from user_info where name = '%1'").arg(strUserName);
    QSqlQuery query;
    if(!query.exec(strSql) || !query.next()) return -1;
    return query.value(0).toInt();
}

QStringList OperatorDB::DBHandleOnlieUser()
{
    QString strSql = QString("select name from user_info where online = 1");
    QSqlQuery query;
    query.exec(strSql);
    QStringList strList;
    strList.clear();
    while(query.next()){
        strList.append(query.value(0).toString());
    }
    return strList;
}

int OperatorDB::DBHandleAddFriend(char *caCurName, char *caTrgName)
{
    if(caCurName == NULL || caTrgName == NULL){
        return -1;//当前用户为空或者目标用户为空
    }
    QString sql = QString(R"(select * from friend
                              where
                                (
                                  user_id = (select id from user_info where name = '%1')
                                  and
                                  friend_id = (select id from user_info where name = '%2')
                                )
                                or
                                (
                                  friend_id = (select id from user_info where name = '%3')
                                  and
                                  user_id = (select id from user_info where name = '%4')
                                );)").arg(caCurName).arg(caTrgName).arg(caCurName).arg(caTrgName);
    qDebug() << "DBHandleOnlieUser::sql " << sql;
    QSqlQuery query2;
    query2.exec(sql);
    if(query2.next()) return -2;//已经是好友不用再发送添加请求了
    sql = QString("select online from user_info where name = '%1'").arg(caTrgName);
    QSqlQuery query;
    query.exec(sql);
    if(!query.next()){
        return -1;
    }
    return query.value(0).toInt();
}

bool OperatorDB::DBHandleAgreeAddFriend(char *caUserName, char *caFriendName)
{
    if(caUserName == NULL || caFriendName == NULL){
        return false;
    }
    QString sql = QString(R"(insert into friend(user_id, friend_id)
                          values((select id from user_info where name = '%1'),
                          (select id from user_info where name = '%2'));)").arg(caUserName).arg(caFriendName);
    qDebug() << "DBHandleAgreeAddFriend::sql " << sql;
    QSqlQuery query;
    if(!query.exec(sql)) return false;
    return true;
}

QStringList OperatorDB::DBHandleRefreshOnlineFriend(char *caUserName)
{
    QStringList strFriendNameList;
    if(caUserName == NULL){
        return strFriendNameList;
    }
    QString sql = QString(R"(select name from user_info
                          where id in (
                          select user_id from friend
                          where friend_id = (select id from user_info where name = '%1')
                          union
                          select friend_id from friend
                          where user_id = (select id from user_info where name = '%2')
                          ) and online = 1;)").arg(caUserName).arg(caUserName);
    QSqlQuery query;
    query.exec(sql);
    while(query.next()){
        strFriendNameList.append(query.value(0).toString());
    }
    for(int i = 0; i < strFriendNameList.size(); i++){
        qDebug() << "OperatorDB::DBHandleRefreshFriend::strFriendName" << strFriendNameList[i];
    }
    return strFriendNameList;
}

bool OperatorDB::BDHandleDelFriend(char *caCurName, char *caDelName)
{
    QString sql = QString(R"(delete from friend
                          where user_id = (select id from user_info where name = '%1')
                          and friend_id = (select id from user_info where name = '%2'))").arg(caCurName).arg(caDelName);
    QSqlQuery query;
    if(!query.exec(sql)) return false;
    return true;
}

OperatorDB::OperatorDB(QObject *parent) : QObject(parent)
{
    //加载数据库驱动，给m_sqlDB赋值
    m_sqlDB = QSqlDatabase::addDatabase("QMYSQL");
}

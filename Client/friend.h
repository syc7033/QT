#ifndef FRIEND_H
#define FRIEND_H

#include <QWidget>
#include <QTcpSocket>
#include "onlineuser.h"
#include "chat.h"
namespace Ui {
class Friend;
}

class Friend : public QWidget
{
    Q_OBJECT

public:
    explicit Friend(QWidget *parent = nullptr);
    ~Friend();
    OnlineUser* getOnlineUser();
    Chat* getChat();
    void updateFriend_LW(QStringList strFriendList);
    void refreshOnlineFriend();
    QListWidget* getOnlieFriendLW();
private slots:
    void on_findUser_PB_clicked();
    void on_OnlieUser_PB_clicked();
    void on_refreshFriend_PB_clicked();
    void on_delFriend_PB_clicked();

    void on_chat_PB_clicked();

private:
    Ui::Friend *ui;
    OnlineUser* m_onlineUser;
    Chat* m_chat;
};

#endif // FRIEND_H

#include "friend.h"
#include "ui_friend.h"
#include "protocol.h"
#include "client.h"

#include <QDebug>
#include <QInputDialog>
#include <QMessageBox>

Friend::Friend(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Friend)
{
    ui->setupUi(this);
    this->m_onlineUser = new OnlineUser();
    this->m_chat = new Chat();
    refreshOnlineFriend();
}

Friend::~Friend()
{
    delete ui;
    delete m_onlineUser;
    delete m_chat;
}

OnlineUser *Friend::getOnlineUser()
{
//    m_onlineUser = new OnlineUser();(bug来源)
    return m_onlineUser;
}

Chat *Friend::getChat()
{
    return m_chat;
}

void Friend::updateFriend_LW(QStringList strFriendList)
{
    ui->listWidget->clear();
    ui->listWidget->addItems(strFriendList);
}

void Friend::refreshOnlineFriend()
{
    PDU* pdu = mkPDU(0);
    pdu->uiMsgType = ENUM_MSG_TYPE_REFRESH_ONLINE_FRIEND_REQUST;
    memcpy(pdu->caData, Client::getInstance().m_strUsername.toStdString().c_str(), 32);
    qDebug() << "Client::refreshFriend " << "userName " << Client::getInstance().m_strUsername;
    Client::getInstance().sendPDU(pdu);
}

QListWidget *Friend::getOnlieFriendLW()
{
    return ui->listWidget;
}

void Friend::on_findUser_PB_clicked()
{
    QString strUserName = QInputDialog::getText(this, "查找", "用户名：");
    qDebug() << "on_findUser_PB_clicked " << "UserName：" << strUserName;
    PDU* pdu = mkPDU(0);
    pdu->uiMsgType = ENUM_MSG_TYPE_FIND_USER_REQUST;
    qDebug() << "on_findUser_PB_clicked " << "uiMsgType" << pdu->uiMsgType;
    memcpy(pdu->caData, strUserName.toStdString().c_str(), 32);
    qDebug() << "on_findUser_PB_clicked " << "caData" << pdu->caData;
    Client::getInstance().sendPDU(pdu);
}


void Friend::on_OnlieUser_PB_clicked()
{
    qDebug() << "on_OnlieUser_PB_clicked::" << "1";
    if(m_onlineUser->isHidden()){
        m_onlineUser->show();
    }
    PDU* pdu = mkPDU(0);
    pdu->uiMsgType = ENUM_MSG_TYPE_ONLINE_USER_REQUST;
    qDebug() << "on_OnlieUser_PB_clicked::" << "uiMsgType" <<pdu->uiMsgType;
    Client::getInstance().sendPDU(pdu);
}

void Friend::on_refreshFriend_PB_clicked()
{
    refreshOnlineFriend();
}

void Friend::on_delFriend_PB_clicked()
{
    QListWidgetItem* item = ui->listWidget->currentItem();
    if(item == NULL){
        QMessageBox::information(this, "错误", "请在列表框中选择你要删除的好友");
        return;
    }

    QString strDelName = item->text();
    QString strCurName = Client::getInstance().m_strUsername;
    qDebug() << "on_delFriend_PB_clicked::" << "strDelName " << strDelName;

    int ret = QMessageBox::question(this, "提示", QString("是否确认删除 %1").arg(strDelName));
    if(ret == QMessageBox::No){
        return;
    }
    PDU* pdu = mkPDU(0);
    pdu->uiMsgType = ENUM_MSG_TYPE_DEL_ONLINE_FRIEND_REQUST;
    memcpy(pdu->caData, strCurName.toStdString().c_str(), 32);
    memcpy(pdu->caData + 32, strDelName.toStdString().c_str(), 32);
    Client::getInstance().sendPDU(pdu);
}

void Friend::on_chat_PB_clicked()
{
    QListWidgetItem* item = ui->listWidget->currentItem();
    if(!item){
        QMessageBox::information(this, "错误", "请在列表框中选择你要删除的好友");
        return;
    }
    if(this->m_chat->isHidden()){
        m_chat->setWindowTitle(Client::getInstance().m_strUsername);
        m_chat->show();
    }
    m_chat->m_strChatName = item->text();
    qDebug() << "Friend::on_chat_PB_clicked::" << "选择的聊天好友是：" << m_chat->m_strChatName;
}

#include "chat.h"
#include "protocol.h"
#include "ui_chat.h"
#include "client.h"
#include <QDebug>

Chat::Chat(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Chat)
{
    ui->setupUi(this);
}

void Chat::updateChat_TE(QString strMsg)
{
    ui->chat_TE->append(strMsg);
}

Chat::~Chat()
{
    delete ui;
}

void Chat::on_send_PB_clicked()
{
    QString strMsg = ui->input_LE->text();
    ui->input_LE->clear();
    qDebug() << "Chat::on_send_PB_clicked" << "strMsg " << strMsg;
    qDebug() << "Chat::on_send_PB_clicked" << "CurName " << Client::getInstance().m_strUsername;
    qDebug() << "Chat::on_send_PB_clicked" << "strMsg " << this->m_strChatName;
    PDU* pdu = mkPDU(strMsg.toStdString().size());
    pdu->uiMsgType = ENUM_MSG_TYPE_CHAT_REQUST;
    memcpy(pdu->caMsg, strMsg.toStdString().c_str(), pdu->uiMsgLen);
    memcpy(pdu->caData, Client::getInstance().m_strUsername.toStdString().c_str(), 32);
    memcpy(pdu->caData + 32, this->m_strChatName.toStdString().c_str(), 32);
    Client::getInstance().sendPDU(pdu);
}

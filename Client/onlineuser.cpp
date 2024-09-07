#include "onlineuser.h"
#include "ui_onlineuser.h"
#include <QDebug>
#include "client.h"

OnlineUser::OnlineUser(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::OnlineUser)
{
    ui->setupUi(this);
}

OnlineUser::~OnlineUser()
{
    delete ui;
}

void OnlineUser::updateOnlieUserLW(QStringList strUNameList)
{
    ui->onlieUser_LW->clear();
    ui->onlieUser_LW->addItems(strUNameList);
}

void OnlineUser::on_onlieUser_LW_itemDoubleClicked(QListWidgetItem *item)
{
    //获取当前用户和目标用户名
    QString strCurName = Client::getInstance().m_strUsername;
    QString strTrgName = item->text();
    qDebug() << "on_onlieUser_LW_itemDoubleClicked::" << "strCurName " <<strCurName;
    qDebug() << "on_onlieUser_LW_itemDoubleClicked::" << "strTrgName " <<strTrgName;
    PDU* pdu = mkPDU(0);
    pdu->uiMsgType = ENUM_MSG_TYPE_ADD_FRIEND_REQUST;
    memcpy(pdu->caData, strCurName.toStdString().c_str(), 32);
    memcpy(pdu->caData + 32, strTrgName.toStdString().c_str(), 32);
    Client::getInstance().sendPDU(pdu);
}


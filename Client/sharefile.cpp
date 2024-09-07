#include "sharefile.h"
#include "ui_sharefile.h"
#include "index.h"
#include "client.h"
ShareFile::ShareFile(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ShareFile)
{
    ui->setupUi(this);
}

ShareFile::~ShareFile()
{
    delete ui;
}

void ShareFile::updateOnlineFriend()
{
    qDebug() << "ShareFile::updateOnlineFriend" << "start";
    ui->onlineUser_LW->clear();
    QListWidget* friend_LW = Index::getInstance().getFriend()->getOnlieFriendLW();
    for (int i = 0; i < friend_LW->count(); i++) {
        QListWidgetItem* pItem = friend_LW->item(i);
        QListWidgetItem* addItem = new QListWidgetItem(*pItem);
        ui->onlineUser_LW->addItem(addItem);
    }
}

void ShareFile::on_allSelect_PB_clicked()
{
    qDebug() << "ShareFile::on_allSelect_PB_clicked" << "start";
    for (int i = 0; i < ui->onlineUser_LW->count(); i++) {
        ui->onlineUser_LW->item(i)->setSelected(true);
    }
}

void ShareFile::on_cancelAllSelect_PB_clicked()
{
    qDebug() << "ShareFile::on_cancelAllSelect_PB_clicked" << "start";
    for (int i = 0; i < ui->onlineUser_LW->count(); i++) {
        ui->onlineUser_LW->item(i)->setSelected(false);
    }
}

void ShareFile::on_ok_PB_clicked()
{
    // 要发送完整路径和转发给哪些好友 这些信息都得放到msg中
    // 为了能取出来 需要把好友数量放到 data中 当前用户也放data中
    QString strCurName = Client::getInstance().m_strUsername;
    QString strCurPath = Index::getInstance().getFile()->m_strCurPath;
    QString strShareName = Index::getInstance().getFile()->m_strShareFileName;
    QString strShareFilePath = strCurPath + "/" + strShareName;

    QList<QListWidgetItem*> pItems = ui->onlineUser_LW->selectedItems();

    int iFriendCount = pItems.size();

    PDU* pdu = mkPDU(iFriendCount * 32 + strShareFilePath.toStdString().size());
    pdu->uiMsgType = ENUM_MSG_TYPE_SHARED_FILE_DATA_REQUST;

    memcpy(pdu->caData, strCurName.toStdString().c_str(), 32);
    qDebug() << "ShareFile::on_ok_PB_clicked" << "当前用户为" << pdu->caData;

    memcpy(pdu->caData + 32, &iFriendCount, sizeof(int));
    qDebug() << "ShareFile::on_ok_PB_clicked" << "好友数量为" << iFriendCount;

    for (int i = 0; i < iFriendCount; i++) {
        memcpy(pdu->caMsg + i * 32, pItems.at(i)->text().toStdString().c_str(), 32);
        qDebug() << "ShareFile::on_ok_PB_clicked" << "分享文件的好友为" << pdu->caMsg + i * 32;
    }

    memcpy(pdu->caMsg + iFriendCount * 32, strShareFilePath.toStdString().c_str(), strShareFilePath.toStdString().size());
    qDebug() << "ShareFile::on_ok_PB_clicked" << "分享文件的完整路径为" << pdu->caMsg + iFriendCount * 32;
    Client::getInstance().sendPDU(pdu);
}

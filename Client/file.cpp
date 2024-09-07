#include "file.h"
#include "ui_file.h"
#include "client.h"
#include "protocol.h"
#include <QFileDialog>
#include <QInputDialog>
#include <QMessageBox>

File::File(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::File)
{
    m_pShareFile = new ShareFile();
    ui->setupUi(this);
    m_strUserPath = QString("%1/%2").arg(Client::getInstance().getRootPath()).arg(Client::getInstance().m_strUsername);
    qDebug() << "File构造函数 " << "用户路径为：" << m_strUserPath;
    m_strCurPath = m_strUserPath;
    qDebug() << "File构造函数 " << "当前路径为：" <<m_strCurPath;
    refreshFile();
}

File::~File()
{
    delete ui;
    delete m_pShareFile;
}

void File::refreshFile()
{
    qDebug() << "File::refreshFile" << "当前路径为:" << m_strCurPath;
    PDU* pdu = mkPDU(m_strCurPath.toStdString().size());
    pdu->uiMsgType = ENUM_MSG_TYPE_REFRESH_FILE_REQUST;
    memcpy(pdu->caMsg, m_strCurPath.toStdString().c_str(), pdu->uiMsgLen);
    Client::getInstance().sendPDU(pdu);
    qDebug() << "发送刷新文件请求成功";
}

void File::updateFile_LW(QList<FileInfo *> lsFileInfoList)
{
    qDebug() << "更新文件方法进入";
    foreach(FileInfo* pFileInfo, m_pFileInfoList){
        qDebug() << "文件名：" << pFileInfo->caFileName;
        delete pFileInfo;
    }
//    m_pFileInfoList.clear();
    m_pFileInfoList = lsFileInfoList;
    ui->listWidget->clear();
    foreach(FileInfo* pFileInfo, lsFileInfoList){
        QListWidgetItem* pItem = new QListWidgetItem;
        pItem->setText(pFileInfo->caFileName);
        qDebug() << "File::updateFile_LW" << "文件名为" << pItem->text();
        if(pFileInfo->uiFileType == 0){
            qDebug("目录图片");
            pItem->setIcon(QIcon(QPixmap(":/dir.png")));
        }else if(pFileInfo->uiFileType == 1){
            qDebug("文件图片");
            pItem->setIcon(QIcon(QPixmap(":/file.png")));
        }
        ui->listWidget->addItem(pItem);
    }
}

void File::uploadFile()
{
    qDebug() << "File::uploadFile start";
    QFile file(m_strUploadFilePath);
    if(!file.open(QIODevice::ReadOnly)){
        QMessageBox::information(this, "打开文件", "打开文件失败");
        return;
    }
    m_bUpload = true;

    PDU* dataPdu = mkPDU(4096);
    dataPdu->uiMsgType = ENUM_MSG_TYPE_UPLOAD_FILE_DATA_REQUST;
    qint64 iReadSize = 0;
    while(true){
        iReadSize = file.read(dataPdu->caMsg, 4096);
        if(iReadSize == 0){
            break;
        }
        if(iReadSize < 0){
            QMessageBox::information(this, "上传文件", "上传文件失败");
            break;
        }
        // msg中的长度不一定每次都是4096 可能比他小 这里做一个优化
        dataPdu->uiPDULen = sizeof(dataPdu) + iReadSize;
        dataPdu->uiMsgLen = iReadSize;
//        Client::getInstance().sendPDU(dataPdu); // 这么发送是错误的 因为我们这个pdu需要一直循环使用 这个函数封装的 会free pdu
        qDebug() << "File::uploadFile" << "uiMsgType" << dataPdu->uiMsgType;
        Client::getInstance().getTcpSocket().write((char*)dataPdu, dataPdu->uiPDULen);
    }
    m_bUpload = false;
    file.close();
    free(dataPdu);
    dataPdu = nullptr;
}

void File::on_mkDir_PB_clicked()
{
    qDebug() << "File::on_mkDir_PB_clicked " << "创建文件夹按钮被点击";
    QString strDirName = QInputDialog::getText(this, "创建文件架", "新建的文件夹名：");
    if(strDirName.isEmpty() || strDirName.toStdString().size() > 32){
        QMessageBox::information(this, "创建文件夹", "文件夹长度不合法");
    }
    qDebug() << "File::on_mkDir_PB_clicked " << "新建文件夹名字：" << strDirName;
    qDebug() << "File::on_mkDir_PB_clicked " << "新建文件夹路径：" << m_strCurPath;
    PDU* pdu = mkPDU(m_strCurPath.toStdString().size());
    pdu->uiMsgType = ENUM_MSG_TYPE_MKDIR_REQUST;
    memcpy(pdu->caMsg, m_strCurPath.toStdString().c_str(), pdu->uiMsgLen);
    memcpy(pdu->caData, strDirName.toStdString().c_str(), 32);
    Client::getInstance().sendPDU(pdu);
}

void File::on_refreshFile_PB_clicked()
{
    qDebug() << "刷新文件按钮被点击";
    refreshFile();
}

//void File::on_delFile__PB_clicked()
//{

//}

void File::on_delDir_PB_clicked()
{
    qDebug() << "File::on_delDir_PB_clicked" << "删除文件夹按钮被点击";
    QListWidgetItem* pitem = ui->listWidget->currentItem();
    if(pitem == NULL){
        QMessageBox::information(this, "错误", "请在列表框中选择你要删除的文件架");
        return;
    }
    QString strFile = pitem->text();
    // 文件类型判断
    foreach(FileInfo* pFileInfo ,m_pFileInfoList){
        if(pFileInfo->caFileName == strFile && pFileInfo->uiFileType != 0){
            QMessageBox::information(this, "错误", "所选项必须是文件夹不能是文件");
            return;
        }
    }

    int ret = QMessageBox::question(this, "提示", QString("是否确认删除 %1").arg(strFile));
    if(ret == QMessageBox::No){
        return;
    }
    QString strDelFilePath = QString("%1/%2").arg(m_strCurPath).arg(strFile);
    qDebug() << "File::on_delFile__PB_clicked" << "要删除的文件架路径是 " << strDelFilePath;
    PDU* pdu = mkPDU(strDelFilePath.toStdString().size());
    pdu->uiMsgType = ENUM_MSG_TYPE_DEL_DIR_REQUST;
    memcpy(pdu->caMsg, strDelFilePath.toStdString().c_str(), pdu->uiMsgLen);
    Client::getInstance().sendPDU(pdu);
}

void File::on_delFile__PB_clicked()
{
    qDebug() << "File::on_delFile__PB_clicked" << "删除文件按钮被点击";
    QListWidgetItem* pitem = ui->listWidget->currentItem();
    if(pitem == NULL){
        QMessageBox::information(this, "错误", "请在列表框中选择你要删除的文件");
        return;
    }
    QString strDelName = pitem->text();
    // 文件类型判断
    foreach(FileInfo* pFileInfo ,m_pFileInfoList){
        if(pFileInfo->caFileName == strDelName && pFileInfo->uiFileType == 0){
            QMessageBox::information(this, "错误", "所选项必须是文件不能是文件架");
            return;
        }
    }

    int ret = QMessageBox::question(this, "提示", QString("是否确认删除 %1").arg(strDelName));
    if(ret == QMessageBox::No){
        return;
    }
    PDU* pdu = mkPDU(m_strCurPath.toStdString().size());
    pdu->uiMsgType = ENUM_MSG_TYPE_DEL_FILE_REQUST;
    memcpy(pdu->caMsg, m_strCurPath.toStdString().c_str(), pdu->uiMsgLen);
    memcpy(pdu->caData, strDelName.toStdString().c_str(), 32);
    Client::getInstance().sendPDU(pdu);
}

void File::on_rename_PB_clicked()
{
    qDebug() << "File::on_rename_PB_clicke" << "重命名文件按钮被点击";
    QListWidgetItem* pitem = ui->listWidget->currentItem();
    QString strOldName = pitem->text();
    QString strNewName = QInputDialog::getText(this, "重命名文件", "重命名文件名字：");
    if(strNewName.isEmpty() || strNewName.toStdString().size() > 32){
        QMessageBox::information(this, "创建文件夹", "文件夹长度不合法");
    }
    PDU* pdu = mkPDU(m_strCurPath.toStdString().size());
    pdu->uiMsgType = ENUM_MSG_TYPE_RENAME_FILE_REQUST;
    memcpy(pdu->caMsg, m_strCurPath.toStdString().c_str(), pdu->uiMsgLen);
    memcpy(pdu->caData, strOldName.toStdString().c_str(), 32);
    memcpy(pdu->caData + 32, strNewName.toStdString().c_str(), 32);
    qDebug() << "File::on_rename_PB_clicke" << "当前路径：" << pdu->caMsg;
    qDebug() << "File::on_rename_PB_clicke" << "旧文件名" << pdu->caData;
    qDebug() << "File::on_rename_PB_clicke" << "新文件名" << pdu->caData + 32;
    Client::getInstance().sendPDU(pdu);

}

void File::on_listWidget_itemDoubleClicked(QListWidgetItem *item)
{
    QString strDirName = item->text();
    foreach(FileInfo* pFileInfo, m_pFileInfoList){
        if(pFileInfo->caFileName == strDirName && pFileInfo->uiFileType != 0){
            QMessageBox::information(this, "进入文件夹", "进入的不是文件夹");
            return;
        }
    }
    m_strCurPath = QString("%1/%2").arg(m_strCurPath).arg(strDirName);
    refreshFile();
}

void File::on_returnUp_PB_clicked()
{
    if(m_strCurPath == m_strUserPath){
        QMessageBox::information(this, "返回上一级", "返回失败：已经是最顶层目录");
        return;
    }
    int index = m_strCurPath.lastIndexOf('/');
    m_strCurPath.remove(index, m_strCurPath.size() - index);
    refreshFile();
}

void File::on_mvFile_PB_clicked()
{
    // 移动按钮需要点击俩次才能完成移动功能，第一次点击是获取src完整路径，第二次
    // 点击是获取trg完整路径，src和trg都要存到msg中，因为他的路径长度可能很大，
    // 那么就产生了一个问题，就是他们在read的时候读多长？
    // 解决这个问题需要把srcLen 和 trgLen 放到pdu的caData中
    if(ui->mvFile_PB->text() == "移动文件"){
        QListWidgetItem* pitem = ui->listWidget->currentItem();
        if(pitem == NULL){
            QMessageBox::information(this, "移动文件", "请选择要移动的文件");
            return;
        }
        QMessageBox::information(this, "移动文件", "请选择要移动的目录");
        m_strSrcName = pitem->text();
        m_strSrcPath = QString("%1/%2").arg(m_strCurPath).arg(m_strSrcName);
        qDebug() << "File::on_mvFile_PB_clicked" << "要移动文件的名字" << m_strSrcName;
        qDebug() << "File::on_mvFile_PB_clicked" << "要移动文件的路径" << m_strSrcPath;
        ui->mvFile_PB->setText("确认/取消");
        return;
    }
    QString strTrgPath;
    QString boxMsg;
    QListWidgetItem* pitem = ui->listWidget->currentItem();
    if(pitem == NULL){
        strTrgPath = QString("%1/%2").arg(m_strCurPath).arg(m_strSrcName);
        boxMsg = "是否移动到当前路径下";
    }
    else{
        QString strDirName = pitem->text();
        foreach(FileInfo* pFileInfo, m_pFileInfoList){
            if(pFileInfo->caFileName == strDirName && pFileInfo->uiFileType != 0){
                QMessageBox::information(this, "进入文件夹", "进入的不是文件夹");
                return;
            }
        }
        strTrgPath = QString("%1/%2/%3").arg(m_strCurPath).arg(strDirName).arg(m_strSrcName);
        boxMsg = QString("是否移动到 %1 目录下").arg(strDirName);
    }
    ui->mvFile_PB->setText("移动文件");
    int ret = QMessageBox::information(this, "移动文件", boxMsg, "确认", "取消");
    if(ret != 0){
        return;
    }
    int srcLen = m_strSrcPath.toStdString().size();
    int trgLen = strTrgPath.toStdString().size();

    PDU* pdu = mkPDU(srcLen + trgLen);
    pdu->uiMsgType = ENUM_MSG_TYPE_MOVE_FILE_REQUST;

    memcpy(pdu->caData, &srcLen, sizeof(int));
    memcpy(pdu->caData + 32, &trgLen, sizeof(int));
    qDebug() << "File::on_mvFile_PB_clicked" << "src长度" << pdu->caData;
    qDebug() << "File::on_mvFile_PB_clicked" << "trg长度" << pdu->caData + 32;
    memcpy(pdu->caMsg, m_strSrcPath.toStdString().c_str(), srcLen);
    memcpy(pdu->caMsg + srcLen, strTrgPath.toStdString().c_str(), trgLen);
    qDebug() << "File::on_mvFile_PB_clicked" << "src路径为" << pdu->caMsg;
    qDebug() << "File::on_mvFile_PB_clicked" << "trg路径为" << pdu->caMsg + srcLen;


    Client::getInstance().sendPDU(pdu);
}

void File::on_uploadFile_PB_clicked()
{
    if(m_bUpload){
        QMessageBox::information(this, "上传文件", "正在有文件上传，请稍等");
        return;
    }

    m_strUploadFilePath.clear();
    m_strUploadFilePath = QFileDialog::getOpenFileName();

    int index = m_strUploadFilePath.lastIndexOf('/');
    QString strUploadFileName = m_strUploadFilePath.right(m_strUploadFilePath.size() - index -1);

    QFile file(m_strUploadFilePath);
    qint64 qiSize = file.size();

    qDebug() << "qiSize" << qiSize;

    PDU* pdu = mkPDU(m_strCurPath.toStdString().size());
    pdu->uiMsgType = ENUM_MSG_TYPE_UPLOAD_FILE_REQUST;

    memcpy(pdu->caData, strUploadFileName.toStdString().c_str(), 32);
    memcpy(pdu->caData + 32, &qiSize, sizeof(qint64));
    memcpy(pdu->caMsg, m_strCurPath.toStdString().c_str(), pdu->uiMsgLen);
    Client::getInstance().sendPDU(pdu);
}

void File::on_downloadFile_PB_clicked()
{

}

void File::on_shareFile_PB_clicked()
{
    qDebug() << "File::on_shareFile_PB_clicked" << "start";
    QListWidgetItem* pItem = ui->listWidget->currentItem();
    if (pItem == NULL) {
        QMessageBox::information(this, "分享文件", "请选择你要分享的文件");
        return;
    }
    m_strShareFileName = pItem->text();
    m_pShareFile->updateOnlineFriend();
    if(m_pShareFile->isHidden()){
        m_pShareFile->show();
    }
    qDebug() << "File::on_shareFile_PB_clicked" << "要分享的文件名为" << m_strShareFileName;

}

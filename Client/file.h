#ifndef FILE_H
#define FILE_H

#include <QListWidget>
#include <QWidget>
#include "protocol.h"
#include "sharefile.h"

namespace Ui {
class File;
}

class File : public QWidget
{
    Q_OBJECT

public:
    explicit File(QWidget *parent = nullptr);
    ~File();
    QString m_strCurPath;
    QString m_strUserPath;
    // 移动文件
    QString m_strSrcName;
    QString m_strSrcPath;
    // 上传文件
    bool m_bUpload = false;
    QString m_strUploadFilePath;
    QList<FileInfo*> m_pFileInfoList;
    void refreshFile();
    void updateFile_LW(QList<FileInfo*> lsFileInfoList);
    void uploadFile();
    ShareFile* m_pShareFile;
    QString m_strShareFileName;
private slots:
    void on_mkDir_PB_clicked();

    void on_refreshFile_PB_clicked();

//    void on_delFile__PB_clicked();

    void on_delDir_PB_clicked();

    void on_delFile__PB_clicked();

    void on_rename_PB_clicked();

    void on_listWidget_itemDoubleClicked(QListWidgetItem *item);

    void on_returnUp_PB_clicked();

    void on_mvFile_PB_clicked();

    void on_uploadFile_PB_clicked();

    void on_downloadFile_PB_clicked();

    void on_shareFile_PB_clicked();

private:
    Ui::File *ui;
};

#endif // FILE_H

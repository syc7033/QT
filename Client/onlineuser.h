#ifndef ONLINEUSER_H
#define ONLINEUSER_H

#include "protocol.h"

#include <QListWidget>
#include <QWidget>

namespace Ui {
class OnlineUser;
}

class OnlineUser : public QWidget
{
    Q_OBJECT

public:
    explicit OnlineUser(QWidget *parent = nullptr);
    ~OnlineUser();
    void updateOnlieUserLW(QStringList strUNameList);
    PDU* registerFuncResp();
private slots:
    void on_onlieUser_LW_itemDoubleClicked(QListWidgetItem *item);

private:
    Ui::OnlineUser *ui;
};

#endif // ONLINEUSER_H

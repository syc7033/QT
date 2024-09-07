#include "index.h"
#include "ui_index.h"
#include "client.h"
Index::Index(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Index)
{
    ui->setupUi(this);
    this->setWindowTitle(Client::getInstance().m_strUsername);
}

Index::~Index()
{
    delete ui;
}

Index &Index::getInstance()
{
    static Index instance;
    return instance;
}

Friend *Index::getFriend()
{
    return ui->friendPage;
}

File *Index::getFile()
{
    return ui->filePage;
}


void Index::on_friend_PB_clicked()
{
    ui->stackedWidget->setCurrentIndex(0);
    //设置样式表
    ui->friend_PB->setStyleSheet("QPushButton{border:none;background:rgb(255,255,255);padding:20px;}");
    ui->file_PB->setStyleSheet("QPushButton{border:none;padding:20px;}");
}

void Index::on_file_PB_clicked()
{
    ui->stackedWidget->setCurrentIndex(1);
    ui->file_PB->setStyleSheet("QPushButton{border:none;background:rgb(255,255,255);padding:20px;}");
    ui->friend_PB->setStyleSheet("QPushButton{border:none;padding:20px;}");
}

#ifndef INDEX_H
#define INDEX_H

#include <QWidget>
#include "friend.h"
#include "file.h"
namespace Ui {
class Index;
}

class Index : public QWidget
{
    Q_OBJECT

public:

    ~Index();
    static Index& getInstance();
    Friend* getFriend();
    File* getFile();
private slots:

    void on_friend_PB_clicked();

    void on_file_PB_clicked();

private:
    Ui::Index *ui;
    explicit Index(QWidget *parent = nullptr);
    Index(const Index& other) = delete;
    Index& operator=(const Index& other) = delete;
};

#endif // INDEX_H

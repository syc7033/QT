#include "server.h"

#include <QApplication>
#include "operatordb.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    OperatorDB::getInstance().connect();
    Server::getInstance();
    return a.exec();
}

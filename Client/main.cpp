#include "client.h"
#include "index.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
//    Client &client = Client::getInstance();
//    client.show();
    Client::getInstance().show();
    return app.exec();
}

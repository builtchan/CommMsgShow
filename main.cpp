#include "commmsgshow.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    CommMsgShow w;
    w.show();

    return a.exec();
}

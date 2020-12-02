#include "comdispatcher.h"

#include <QApplication>
#include <QDebug>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    ComDispatcher w;
    w.show();
    return a.exec();
}

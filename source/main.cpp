#include "comdispatcher.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    ComDispatcher w;
    w.show();
    return a.exec();
}

#include "QStegano.h"
#include <QtWidgets/QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    QStegano w;

    w.show();
    return a.exec();
}

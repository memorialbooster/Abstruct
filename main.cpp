#include "mainwindow.h"
#include <QtGui>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    w.setGeometry(200,100,600,600);
    w.show();

    return a.exec();
}

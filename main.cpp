#include "mainwindow.h"
#include <QtGui>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    w.setGeometry(200, 100, DEFAULT_SCREEN_WIDHT, DEFAULT_SCREEN_HEIGHT);
    w.show();

    return a.exec();
}

#include <QApplication>
#include "mainwindow.h"

int main(int argc, char **argv)
{
    QApplication a(argc, argv);

    MainWindow v;
    v.setWindowTitle("Simple 3D Objects Engine");
    v.show();

    return a.exec();
}

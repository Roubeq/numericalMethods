#include "mainwindow.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    w.setWindowTitle("Задача на собственные значения (Qt)");
    w.resize(900, 700);
    w.show();
    return a.exec();
}

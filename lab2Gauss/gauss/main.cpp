#include "mainwindow.h"
#include <fenv.h>
#include <QApplication>

int main(int argc, char *argv[])
{
    fesetenv(FE_PC53_ENV);
    QApplication a(argc, argv);
    MainWindow w;
    w.show();
    return a.exec();
}

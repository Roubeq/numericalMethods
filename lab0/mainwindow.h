#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <qcustomplot.h>

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private:
    QCustomPlot *plotWidget;
    QDoubleSpinBox *kSpinBox;
    QSpinBox *nSpinBox;
    QTextEdit *outputText;

    QTextEdit *errorText;

    QVector<double> x0Values;
    QVector<QVector<double>> partialSums;
    QVector<double> errorsAtX0;
    QVector<double> XnValues;

    double x0Fix = 0.502655;

    double epsilon = 0.1;

    double taylor(double x, double k,int nMax);
    double factorial(int number);

    void calculate();


};
#endif // MAINWINDOW_H

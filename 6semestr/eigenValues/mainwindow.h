#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QVector>
#include <QTableWidget>
#include <QSpinBox>
#include <QPushButton>
#include <QLabel>
#include <QTextEdit>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGroupBox>

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);

private slots:
    void calculate();

private:
    QVector<double> buildMatrixRow(int i, int n);
    QVector<double> matVec(const QVector<QVector<double>>& A, const QVector<double>& x);
    double norm(const QVector<double>& v);
    double dot(const QVector<double>& a, const QVector<double>& b);
    QVector<double> solveA(const QVector<double>& rhs);
    std::pair<double, QVector<double>> powerMethod(int n);
    std::pair<double, QVector<double>> inversePowerMethod(int n);

    // UI
    QSpinBox* spinN;
    QPushButton* btnCalc;
    QTableWidget* tableMatrix;
    QTextEdit* textResults;
};

#endif // MAINWINDOW_H

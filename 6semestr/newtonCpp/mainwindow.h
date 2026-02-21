#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QVector>
#include <QPair>
#include <QPushButton>
#include <QTableWidget>
#include <QTextEdit>

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow() = default;

private slots:
    void solve();

private:
    QPushButton *solveButton;
    QTableWidget *tableWidget;
    QTextEdit    *solutionTextEdit;

    QString fileName;
    void fillTable(const QVector<QPair<double, double>>& data);
};

#endif // MAINWINDOW_H

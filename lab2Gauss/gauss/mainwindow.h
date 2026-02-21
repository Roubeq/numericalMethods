#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTableWidget>
#include <QComboBox>
#include <QPushButton>
#include <QLabel>
#include "gaussSolver.h"

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void onDimensionChanged(int index);
    void onSolveClicked();
    void onLoadTestClicked();
    void onClearClicked();

private:
    void setupUI();
    void setupMatrixTable(int dimension);
    bool readMatrixFromTable(std::vector<std::vector<double>>& A, std::vector<double>& b);

    QComboBox* dimensionComboBox;
    QTableWidget* matrixTable;
    QTableWidget* vectorTable;
    QTableWidget* resultTable;
    QTableWidget* residualTable;
    QPushButton* solveButton;
    QPushButton* testButton;
    QPushButton* clearButton;
    QLabel* normLabel;
    QLabel* statusLabel;

    GaussSolver solver;
};

#endif

#include "mainwindow.h"
#include "newtonsolver.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPushButton>
#include <QLineEdit>
#include <QTableWidget>
#include <QTextEdit>
#include <QHeaderView>
#include <QFileDialog>
#include <QMessageBox>
#include <QFile>
#include <QTextStream>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    QWidget *central = new QWidget(this);
    QVBoxLayout *mainLayout = new QVBoxLayout(central);

    solveButton = new QPushButton("Решить");
    mainLayout->addWidget(solveButton);

    tableWidget = new QTableWidget();
    tableWidget->setColumnCount(3);
    QStringList headers;
    headers << "Iter" << "d1" << "d2";
    tableWidget->setHorizontalHeaderLabels(headers);
    tableWidget->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    mainLayout->addWidget(tableWidget);

    solutionTextEdit = new QTextEdit();
    solutionTextEdit->setReadOnly(true);
    mainLayout->addWidget(solutionTextEdit);

    setCentralWidget(central);
    fileName = "C:/Users/Artem/Desktop/NumericalMethods/numericalMethods/6semestr/untitled/build/Desktop_Qt_6_9_2_MinGW_64_bit-Debug/debug/input.txt";

    connect(solveButton, &QPushButton::clicked, this, &MainWindow::solve);
}

void MainWindow::solve()
{
    QFile file(fileName);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QMessageBox::critical(this, "Ошибка", "Не удалось открыть файл.");
        return;
    }

    QTextStream in(&file);

    QString line = in.readLine();
    if (line.isNull()) {
        QMessageBox::critical(this, "Ошибка", "Файл пуст.");
        return;
    }
    QStringList parts = line.split(' ', Qt::SkipEmptyParts);
    if (parts.size() < 2) {
        QMessageBox::critical(this, "Ошибка", "Неверный формат файла: ожидалось 2 числа в первой строке.");
        return;
    }
    bool ok;
    double x0[2];
    x0[0] = parts[0].toDouble(&ok);
    x0[1] = parts[1].toDouble(&ok);

    line = in.readLine();
    if (line.isNull()) {
        QMessageBox::critical(this, "Ошибка", "В файле отсутствует вторая строка.");
        return;
    }
    parts = line.split(' ', Qt::SkipEmptyParts);
    if (parts.size() < 3) {
        QMessageBox::critical(this, "Ошибка", "Неверный формат файла: ожидалось 3 числа во второй строке.");
        return;
    }
    double e1 = parts[0].toDouble(&ok);
    double e2 = parts[1].toDouble(&ok);
    int NIT = parts[2].toInt(&ok);
    file.close();

    NewtonSolver solver;
    solver.setInitialGuess(x0);
    solver.setTolerances(e1, e2);
    solver.setMaxIterations(NIT);

    bool success = solver.solve();

    const QVector<QPair<double, double>>& history = solver.getHistory();
    fillTable(history);

    if (success) {
        const double* solution = solver.getSolution();
        solutionTextEdit->setText(
            QString("Решение найдено за %1 итераций:\nX1 = %2\nX2 = %3")
                .arg(history.size())
                .arg(solution[0], 0, 'g', 12)
                .arg(solution[1], 0, 'g', 12)
            );
    } else {
        solutionTextEdit->setText("Решение не найдено (достигнуто максимальное число итераций или ошибка).");
    }
}

void MainWindow::fillTable(const QVector<QPair<double, double>>& data)
{
    tableWidget->setRowCount(data.size());
    for (int i = 0; i < data.size(); ++i) {
        QTableWidgetItem *itemIter = new QTableWidgetItem(QString::number(i));
        tableWidget->setItem(i, 0, itemIter);

        QTableWidgetItem *itemD1 = new QTableWidgetItem(QString::number(data[i].first, 'e', 5));
        tableWidget->setItem(i, 1, itemD1);

        QTableWidgetItem *itemD2 = new QTableWidgetItem(QString::number(data[i].second, 'e', 5));
        tableWidget->setItem(i, 2, itemD2);
    }
}

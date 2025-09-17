#include "mainwindow.h"
#include <QGridLayout>
#include <QDoubleSpinBox>
#include <QPushButton>
#include <QDebug>
#include <cmath>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    setWindowTitle("Lab 0");
    setMinimumSize(1200, 800);

    QWidget *central = new QWidget(this);
    setCentralWidget(central);
    QGridLayout *gridLayout = new QGridLayout(central);

    QWidget *controlPanel = new QWidget();
    controlPanel->setFixedWidth(300);
    QVBoxLayout *panelLayout = new QVBoxLayout(controlPanel);

    kSpinBox = new QDoubleSpinBox();
    kSpinBox->setRange(1.0, 10.0);
    kSpinBox->setValue(1.0);
    kSpinBox->setSingleStep(1.0);

    nSpinBox = new QSpinBox();
    nSpinBox->setRange(0, 10);
    nSpinBox->setValue(0);

    outputText = new QTextEdit();
    outputText->setReadOnly(true);
    outputText->setFixedHeight(400);

    QPushButton *plotButton = new QPushButton("Построить график");
    plotButton->setFixedHeight(40);

    panelLayout->addWidget(new QLabel("Коэффициент k:"));
    panelLayout->addWidget(kSpinBox);
    panelLayout->addWidget(new QLabel("Кол-во членов ряда n:"));
    panelLayout->addWidget(nSpinBox);
    panelLayout->addSpacing(20);
    panelLayout->addWidget(plotButton);
    panelLayout->addWidget(new QLabel("Результаты:"));
    panelLayout->addWidget(outputText);
    panelLayout->addStretch();

    plotWidget = new QCustomPlot();
    plotWidget->xAxis->setLabel("Ось X");
    plotWidget->yAxis->setLabel("Ось Y");
    plotWidget->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom | QCP::iSelectPlottables);
    plotWidget->legend->setVisible(true);

    gridLayout->addWidget(controlPanel, 0, 0, 1, 1);
    gridLayout->addWidget(plotWidget, 0, 1, 1, 3);
    gridLayout->setColumnStretch(1, 1);

    connect(plotButton, &QPushButton::clicked, this, &MainWindow::calculate);
}

MainWindow::~MainWindow()
{
}

double MainWindow::factorial(int n)
{
    if (n < 0) {
        qCritical() << "Factorial error: negative input";
        exit(1);
    }
    double fact = 1.0;
    while (n > 1) {
        fact *= n--;
    }
    return fact;
}

double MainWindow::taylor(double x, double k, int nMax)
{
    double sum = 0.0;
    for (int i = 0; i < nMax; ++i) {
        int power = 2 * i + 1;
        double term = std::pow(-1.0, i) * std::pow(k * x, power) / factorial(power);
        sum += term;
    }
    return sum;
}

void MainWindow::calculate()
{
    outputText->clear();
    plotWidget->clearGraphs();

    double k = kSpinBox->value();
    int nMax = nSpinBox->value();

    const int numPoints = 1000;
    QVector<double> xData(numPoints + 1), exactData(numPoints + 1);
    for (int i = 0; i <= numPoints; ++i) {
        xData[i] = 2 * M_PI * i / numPoints;
        exactData[i] = std::sin(k * xData[i]);
    }

    QCPGraph *exactPlot = plotWidget->addGraph();
    exactPlot->setData(xData, exactData);
    exactPlot->setPen(QPen(Qt::blue, 2));
    exactPlot->setName("sin(kx)");

    for (int n = 1; n <= nMax; ++n) {
        QVector<double> validPointsX, validPointsY, invalidPointsX, invalidPointsY;

        for (int i = 0; i < xData.size(); ++i) {
            double x = xData[i];
            double exact = exactData[i];
            double taylorRes = taylor(x, k, n);
            double error = std::abs(exact - taylorRes);

            if (error <= epsilon) {
                validPointsX.append(x);
                validPointsY.append(taylorRes);
                if (x <= 0.502655 && x >= 0.502654) {
                    outputText->append(QString("n=%1, x=%2, ошибка=%3")
                                           .arg(n)
                                           .arg(x, 0, 'f', 6)
                                           .arg(error, 0, 'f', 10));
                }
            } else {
                invalidPointsX.append(x);
                invalidPointsY.append(taylorRes);
            }
        }

        if (!validPointsX.isEmpty()) {
            outputText->append(QString("Интервал для n=%1: [0, %2]")
                            .arg(n)
                            .arg(validPointsX.last(), 0, 'f', 3));

            QCPGraph *validPlot = plotWidget->addGraph();
            validPlot->setData(validPointsX, validPointsY);
            QColor color = QColor::fromHsv((n * 70) % 360, 180, 200);
            validPlot->setPen(QPen(color, 2));
            validPlot->setName(QString("Taylor n=%1 (err≤%2)").arg(n).arg(epsilon));
        }

        if (!invalidPointsX.isEmpty()) {
            QCPGraph *invalidPlot = plotWidget->addGraph();
            invalidPlot->setData(invalidPointsX, invalidPointsY);
            QColor color = QColor::fromHsv((n * 70) % 360, 180, 200, 100);
            invalidPlot->setPen(QPen(color, 1, Qt::DashLine));
            invalidPlot->removeFromLegend();
        }
    }

    plotWidget->xAxis->setRange(0, 2 * M_PI);
    plotWidget->yAxis->setRange(-1.8, 1.8);
    plotWidget->replot();
}

#include "mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    setWindowTitle("Это график");
    resize(1400, 900);

    QWidget *newWidget = new QWidget(this);
    setCentralWidget(newWidget);
    QVBoxLayout *mainBoxLayout = new QVBoxLayout(newWidget);

    QGroupBox *parameters = new QGroupBox("Параметры");
    parameters->setFixedHeight(80);
    mainBoxLayout->addWidget(parameters);
    QHBoxLayout *controlLayout = new QHBoxLayout();

    controlLayout->addWidget(new QLabel("k: "));
    kInput = new QLineEdit("1.0");
    controlLayout->addWidget(kInput);

    controlLayout->addWidget(new QLabel("N: "));
    nMaxInput = new QSpinBox();
    nMaxInput->setValue(1);
    nMaxInput->setRange(1,10);
    controlLayout->addWidget(nMaxInput);

    QPushButton *calculateButton = new QPushButton("Вычислить");
    controlLayout->addWidget(calculateButton);

    parameters->setLayout(controlLayout);
    mainBoxLayout->addWidget(parameters);

    customPlot = new QCustomPlot();
    mainBoxLayout->addWidget(customPlot);

    customPlot->xAxis->setLabel("x");
    customPlot->yAxis->setLabel("y");
    customPlot->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom);
    customPlot->legend->setVisible(true);

    connect(calculateButton, &QPushButton::clicked, this, &MainWindow::calculate);
}

MainWindow::~MainWindow()
{
    //delete customPlot;
}

double MainWindow::factorial(int number) {
    if (number < 0) {
        exit(1);
    }
    double result = 1;
    for (int n = 1; n <= number; n++) {
        result *= n;
    }
    return result;
}

double MainWindow::taylor(double x, double k, int nMax) {
    double result = 0.0;

    for (int n = 0; n < nMax; n++) {
        int exponent = 2 * n + 1;
        double term = std::pow(-1, n) * std::pow(k * x, exponent) / factorial(exponent);
        result += term;
    }

    return result;
}

void MainWindow::calculate() {
    customPlot->clearGraphs();
    double k = kInput->text().toDouble();
    int nMax = nMaxInput->value();

    QVector<double> xValues;
    QVector<double> exactValues;
    int points = 1000;
    for (int i = 0; i <= points; i++) {
        double x = 2 * M_PI * i / points;
        xValues.append(x);
        exactValues.append(std::sin(k * x));
    }

    QCPGraph *exactGraph = customPlot->addGraph();
    exactGraph->setData(xValues, exactValues);
    exactGraph->setPen(QPen(Qt::red, 2));
    exactGraph->setName("Точная функция: sin(kx)");

    int graphIndex = 1;

    for (int n = 1; n <= nMax; n++) {
        QVector<double> validX;
        QVector<double> validY;

        QVector<double> invalidX;
        QVector<double> invalidY;

        for (int i = 0; i < xValues.size(); i++) {
            double x = xValues[i];
            double exact = exactValues[i];
            double approx = taylor(x, k, n);
            double error = std::abs(exact - approx);

            if (error <= epsilon) {
                validX.append(x);
                validY.append(approx);
            } else {
                invalidX.append(x);
                invalidY.append(approx);
            }
        }

        if (!validX.isEmpty()) {
            QCPGraph *validGraph = customPlot->addGraph();
            validGraph->setData(validX, validY);
            QColor color = QColor::fromHsv((n * 50) % 360, 200, 200);
            validGraph->setPen(QPen(color, 2));
            validGraph->setName(QString("S%1(x) (ошибка≤%2)").arg(n).arg(epsilon));
        }

        if (!invalidX.isEmpty()) {
            QCPGraph *invalidGraph = customPlot->addGraph();
            invalidGraph->setData(invalidX, invalidY);
            QColor color = QColor::fromHsv((n * 50) % 360, 200, 200, 150);
            invalidGraph->setPen(QPen(color, 1, Qt::DotLine));
            invalidGraph->removeFromLegend();
        }
    }

    customPlot->xAxis->setRange(0, 2 * M_PI);
    customPlot->yAxis->setRange(-1.5, 1.5);
    customPlot->replot();
}

#include "MainWindow.h"
#include <QCustomPlot.h>
#include <QFile>
#include <QTextStream>
#include <QDebug>
#include <vector>
#include <cmath>

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent) {
    customPlot = new QCustomPlot(this);
    setCentralWidget(customPlot);
    resize(800, 600);

    QFile file("C:/Users/Artem/Desktop/NumericalMethods/numericalMethods/labaMNK/values.txt");
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qDebug() << "Cannot open file";
        return;
    }
    QTextStream in(&file);
    int n, m;
    in >> n >> m;
    std::vector<double> x(n), y(n);
    for (int i = 0; i < n; ++i) {
        in >> x[i] >> y[i];
    }
    file.close();

    int max_power = 2 * m;
    std::vector<double> powerx(max_power + 1, 0.0);
    for (int i = 0; i < n; ++i) {
        double xp = 1.0;
        for (int k = 0; k <= max_power; ++k) {
            powerx[k] += xp;
            xp *= x[i];
        }
    }

    int size = m + 1;
    std::vector<std::vector<double>> sumX(size, std::vector<double>(size, 0.0));
    for (int l = 0; l < size; ++l) {
        for (int j = 0; j < size; ++j) {
            sumX[l][j] = powerx[l + j];
        }
    }

    std::vector<double> praw(size, 0.0);
    for (int l = 0; l < size; ++l) {
        for (int i = 0; i < n; ++i) {
            praw[l] += y[i] * std::pow(x[i], l);
        }
    }

    std::vector<double> a(size);
    for (int i = 0; i < size; ++i) {
        for (int k = i + 1; k < size; ++k) {
            double factor = sumX[k][i] / sumX[i][i];
            for (int j = i; j < size; ++j) {
                sumX[k][j] -= factor * sumX[i][j];
            }
            praw[k] -= factor * praw[i];
        }
    }
    for (int i = size - 1; i >= 0; --i) {
        a[i] = praw[i];
        for (int j = i + 1; j < size; ++j) {
            a[i] -= sumX[i][j] * a[j];
        }
        a[i] /= sumX[i][i];
    }
    qDebug() << "Коэффициенты a0, a1, a2:" << a[0] << a[1] << a[2];

    double residual_sum = 0.0;
    for (int i = 0; i < n; ++i) {
        double y_hat = 0.0;
        for (int k = 0; k < size; ++k) {
            y_hat += a[k] * std::pow(x[i], k);
        }
        residual_sum += std::pow(y[i] - y_hat, 2);
    }
    double s2 = residual_sum / (n - m - 1);
    double sigma = std::sqrt(s2);
    qDebug() << "s^2:" << s2 << "sigma:" << sigma;

    QVector<double> qx(n), qy(n);
    for (int i = 0; i < n; ++i) {
        qx[i] = x[i];
        qy[i] = y[i];
    }
    customPlot->addGraph();
    customPlot->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom | QCP::iSelectPlottables);
    customPlot->graph(0)->setData(qx, qy);
    customPlot->graph(0)->setScatterStyle(QCPScatterStyle::ssCircle);
    customPlot->graph(0)->setLineStyle(QCPGraph::lsNone);
    customPlot->graph(0)->setPen(QPen(Qt::blue));

    double min_x = *std::min_element(x.begin(), x.end());
    double max_x = *std::max_element(x.begin(), x.end());
    QVector<double> approx_x(100), approx_y(100);
    double step = (max_x - min_x) / 99.0;
    for (int i = 0; i < 100; ++i) {
        double curr_x = min_x + i * step;
        approx_x[i] = curr_x;
        double y_hat = 0.0;
        for (int k = 0; k < size; ++k) {
            y_hat += a[k] * std::pow(curr_x, k);
        }
        approx_y[i] = y_hat;
    }
    customPlot->addGraph();
    customPlot->graph(1)->setData(approx_x, approx_y);
    customPlot->graph(1)->setPen(QPen(Qt::red));

    customPlot->xAxis->setLabel("x");
    customPlot->yAxis->setLabel("y");
    customPlot->xAxis->setRange(min_x - 1, max_x + 1);
    customPlot->yAxis->setRange(*std::min_element(y.begin(), y.end()) - 10, *std::max_element(y.begin(), y.end()) + 10);
    customPlot->replot();
}

MainWindow::~MainWindow() {}

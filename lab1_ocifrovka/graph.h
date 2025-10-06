#ifndef GRAPH_H
#define GRAPH_H

#include <QMainWindow>
#include "qcustomplot.h"

class Graph : public QMainWindow
{
    Q_OBJECT

public:
    Graph(QWidget *parent = nullptr);

    QCustomPlot *plot;
    QCustomPlot *plot2;
    int numberOfDots = 115;
    double stepMillimeters = 0.2;

    QVector<double> xList;
    QVector<double> yList;
private:
    void loadParametersX();

    void loadParametersYFromFile();

    double lagranj(double x);
    double newton(double x);

    void plotLagranj();
    void plotNewton();

    void calculateError();

};
#endif // GRAPH_H

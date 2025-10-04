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
    int numberOfDots = 115;
    double stepMillimeters = 0.2;

    QVector<double> xList;
    QVector<double> yList;
private:
    void loadParametersX();

    void loadParametersYFromFile();

};
#endif // GRAPH_H

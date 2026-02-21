#ifndef GRAPH_H
#define GRAPH_H

#include <QMainWindow>
#include "qcustomplot.h"
#include "qgifimage.h"

class Graph : public QMainWindow
{
    Q_OBJECT

public:
    Graph(QWidget *parent = nullptr);

    QCustomPlot *plot;
    QCustomPlot *plot2;
    QCustomPlot *velocityPlot;
    int numberOfDots = 109;
    double stepMillimeters = 0.2;

    QVector<double> xList;
    QVector<double> yList;

    void calculateSplineForAnimation();
    void plotSplineForAnimation();
    void createGifButton();
    void runMotionSimulation();
    void updateAnimation();
    void createGif(const QString& filename = "animation.gif", int frameCount = 100, int delay = 50);
    void createGifTwoPlots(const QString& filename, int frameCount, int delay);
private:
    void startGifRecording();
    void stopGifRecording();
    void loadParametersX();

    void loadParametersYFromFile();

    double lagranj(double x);
    double newton(double x);

    void plotLagranj();
    void plotNewton();

    void calculateError();

    void calculateCubicSpline();


    // дифференцирование
    void calculateNumericalDerivatives();
    QVector<double> forwardDifference(const QVector<double>& x, const QVector<double>& y);
    QVector<double> backwardDifference(const QVector<double>& x, const QVector<double>& y);
    QVector<double> centralDifference(const QVector<double>& x, const QVector<double>& y);
    void plotDerivativesAndOriginal(const QVector<double>& deriv_forward,
                                    const QVector<double>& deriv_backward,
                                    const QVector<double>& deriv_central);
    void printErrorAnalysis(const QVector<double>& deriv_forward,
                            const QVector<double>& deriv_backward,
                            const QVector<double>& deriv_central);
    QVector<double> calculateSecondDerivative(const QVector<double>& x, const QVector<double>& y);
    QVector<double> calculateThirdDerivative(const QVector<double>& x, const QVector<double>& y);


    // интегрирование
    void calculateIntegrationMethods();
    void plotIntegrationMethods();
    double leftRectangleMethod(const QVector<double>& x, const QVector<double>& y);
    double rightRectangleMethod(const QVector<double>& x, const QVector<double>& y);
    double centralRectangleMethod(const QVector<double>& x, const QVector<double>& y);
    void showLeftRectanglesWindow();
    void showRightRectanglesWindow();
    void showCentralRectanglesWindow();
    double calculateFirstDerivativeMax();
    double calculateSecondDerivativeMax();
    double calculateFourthDerivativeMax();
    void calculateIntegrationErrors(double leftRect, double rightRect, double centralRect);
    double trapezoidalMethod(const QVector<double>& x, const QVector<double>& y);
    void showTrapezoidalWindow();
    double simpsonMethod(const QVector<double>& x, const QVector<double>& y);
    void calculateSimpsonMethod();

    void printIntegrationComparisonTable();

    // проецирование шарика по сплайну
    double initialSpeed;
    double gravity;
    double initialHeight;
    QVector<double> velocities;

    QTimer *animationTimer;
    double animationTime;
    double animationDuration;
    QVector<double> splinePointsX;
    QVector<double> splinePointsY;
    QVector<double> splineArclengths;
    double totalPathLength;
    QCPItemEllipse *movingPoint;
    QCPGraph *velocityGraph;

    QVector<double> timeValues;
    QVector<double> vxValues;
    QVector<double> vyValues;
    void calculateVelocityComponents();
    void plotVelocityComponents();

    void calculateVelocities();
    void calculateArclengthsAndTime();
    void calculateAndPlotVelocity(double currentX, double currentY);

};
#endif // GRAPH_H

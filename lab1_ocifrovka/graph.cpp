#include "graph.h"
#include <QFile>
#include <QVector>

using namespace std;

Graph::Graph(QWidget *parent)
    : QMainWindow(parent)
{
    setWindowTitle("Graph");
    QWidget *centralWidget = new QWidget(this);
    setCentralWidget(centralWidget);

    QHBoxLayout *layout = new QHBoxLayout(centralWidget);

    plot = new QCustomPlot();
    plot->xAxis->setLabel("Ось X");
    plot->yAxis->setLabel("Ось Y");
    plot->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom | QCP::iSelectPlottables);

    plot2 = new QCustomPlot();
    plot2->xAxis->setLabel("Ось X");
    plot2->yAxis->setLabel("Ось Y");
    plot2->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom | QCP::iSelectPlottables);

    layout->addWidget(plot);
    layout->addWidget(plot2);
    loadParametersX();
    loadParametersYFromFile();

    QCPCurve *curve = new QCPCurve(plot->xAxis, plot->yAxis);
    curve->setData(xList, yList);
    curve->setLineStyle(QCPCurve::lsLine);
    QColor color = QColor::fromHsv((1 * 39) % 360, 180, 200);
    curve->setPen(QPen(color, 2));
    curve->setName(QString("Кривая"));
    plot->setAntialiasedElements(QCP::aeAll);
    plot->setNotAntialiasedElement(QCP::aeNone);
    plot->xAxis->setRange(0, 26);
    plot->yAxis->setRange(0, -20);

    QCPCurve *curve2 = new QCPCurve(plot2->xAxis, plot2->yAxis);
    curve2->setData(xList, yList);
    curve2->setLineStyle(QCPCurve::lsLine);
    QColor color2 = QColor::fromHsv((1 * 37) % 360, 180, 200);
    curve2->setPen(QPen(color2, 2));
    curve2->setName(QString("Кривая"));
    plot2->setAntialiasedElements(QCP::aeAll);
    plot2->setNotAntialiasedElement(QCP::aeNone);
    plot2->xAxis->setRange(0, 26);
    plot2->yAxis->setRange(0, -20);

    plot->replot();
    plot2->replot();

    plotLagranj();
    plotNewton();
    calculateError();
};

void Graph::loadParametersX() {
    QString filePath = QApplication::applicationDirPath() + "/xParameters.txt";

    QFile file(filePath);

    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qDebug() << "Ошибка открытия файла:" << file.errorString();
        qDebug() << "Путь:" << filePath;
        return;
    }

    QTextStream in(&file);
    xList.clear();

    while (!in.atEnd()) {
        QString line = in.readLine().trimmed();
        if (line.isEmpty()) continue;

        bool ok;
        double symbolX = line.toDouble(&ok);
        if (ok) {
            xList.push_back(symbolX);
        } else {
            qDebug() << "Ошибка преобразования:" << line;
        }
    }

    file.close();
}

void Graph::loadParametersYFromFile() {
    QString filePath = QApplication::applicationDirPath() + "/parameters.txt";

    QFile file(filePath);

    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qDebug() << "Ошибка открытия файла:" << file.errorString();
        qDebug() << "Путь:" << filePath;
        return;
    }

    QTextStream in(&file);
    yList.clear();

    while (!in.atEnd()) {
        QString line = in.readLine().trimmed();
        if (line.isEmpty()) continue;

        bool ok;
        double symbolY = line.toDouble(&ok);
        if (ok) {
            yList.push_back(symbolY);

        } else {
            qDebug() << "Ошибка преобразования:" << line;
        }
    }

    file.close();
}

double Graph::lagranj(double x) {
    vector<pair<double,double>> listLagranj = {{0.0,0.0},{4.6,-13.75},{6.2,-16.7},{9.2,-17.63},{12.2,-17.65},{15.2,-14.5},{21.2,-1.3},{22.0,0.0}};
    double pn = 0.0;
    for(int i = 0; i < listLagranj.size(); i++) {
        double xi = listLagranj[i].first;
        double yi = listLagranj[i].second;
        double pi = yi;
        for (int j = 0; j < listLagranj.size(); j++) {
            if (i != j) {
                double xj = listLagranj[j].first;
                double yj = listLagranj[j].second;
                pi *= (x - xj)/(xi - xj);
            }
        }
        pn += pi;
    }
    return pn;
}

void Graph::plotLagranj() {
    vector<pair<double,double>> points = {{0.0,0.0},{4.6,-13.75},{6.2,-16.7},{9.2,-17.63},{12.2,-17.65},{15.2,-14.5},{21.2,-1.3},{22.0,0.0}};
    QVector<double> xValues;
    QVector<double> yValues;
    for(int i = 0; i< 1000; i++) {
        double xCoord = (22.0 * i) / 1000;
        double yCoord = lagranj(xCoord);

        xValues.append(xCoord);
        yValues.append(yCoord);
    }

    QCPCurve *lagrangeCurve = new QCPCurve(plot->xAxis, plot->yAxis);
    lagrangeCurve->setData(xValues, yValues);
    lagrangeCurve->setLineStyle(QCPCurve::lsLine);
    lagrangeCurve->setPen(QPen(Qt::red, 2));
    lagrangeCurve->setName("Полином Лагранжа");

    QCPGraph *pointsGraph = plot->addGraph();
    QVector<double> xPoints, yPoints;
    for (const auto& p : points) {
        xPoints.append(p.first);
        yPoints.append(p.second);
    }
    pointsGraph->setData(xPoints, yPoints);
    pointsGraph->setLineStyle(QCPGraph::lsNone);
    pointsGraph->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssCircle, 8));
    pointsGraph->setPen(QPen(Qt::blue));
    pointsGraph->setName("Исходные точки");

    plot->legend->setVisible(true);
    plot->replot();
}

double Graph::newton(double x) {
    vector<pair<double,double>> points = {{0.0,0.0},{4.6,-13.75},{6.2,-16.7},{9.2,-17.63},{12.2,-17.65},{15.2,-14.5},{21.2,-1.3},{22.0,0.0}};

    int n = points.size();
    vector<double> coefficients(n);

    for (int i = 0; i < n; i++) {
        coefficients[i] = points[i].second;
    }

    for (int j = 1; j < n; j++) {
        for (int i = n - 1; i >= j; i--) {
            coefficients[i] = (coefficients[i] - coefficients[i-1]) /
                              (points[i].first - points[i-j].first);
        }
    }

    double result = coefficients[0];
    double product = 1.0;

    for (int i = 1; i < n; i++) {
        product *= (x - points[i-1].first);
        result += coefficients[i] * product;
    }

    return result;
}

void Graph::plotNewton() {
    vector<pair<double,double>> points = {{0.0,0.0},{4.6,-13.75},{6.2,-16.7},{9.2,-17.63},{12.2,-17.65},{15.2,-14.5},{21.2,-1.3},{22.0,0.0}};

    QVector<double> xNewton, yNewton;
    for(int i = 0; i < 1000; i++) {
        double xCoord = (22.0 * i) / 1000;
        double yCoord = newton(xCoord);
        xNewton.append(xCoord);
        yNewton.append(yCoord);
    }

    QCPCurve *newtonCurve = new QCPCurve(plot2->xAxis, plot2->yAxis);
    newtonCurve->setData(xNewton, yNewton);
    newtonCurve->setLineStyle(QCPCurve::lsLine);
    newtonCurve->setPen(QPen(Qt::green, 2));
    newtonCurve->setName("Полином Ньютона");

    QCPGraph *pointsGraph = plot2->addGraph();
    QVector<double> xPoints, yPoints;
    for (const auto& p : points) {
        xPoints.append(p.first);
        yPoints.append(p.second);
    }
    pointsGraph->setData(xPoints, yPoints);
    pointsGraph->setLineStyle(QCPGraph::lsNone);
    pointsGraph->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssCircle, 6));
    pointsGraph->setPen(QPen(Qt::red));
    pointsGraph->setName("Узлы интерполяции");

    plot2->legend->setVisible(true);
    plot2->replot();
}

void Graph::calculateError() {
    vector<pair<double,double>> interpolationPoints = {
        {0.0,0.0}, {4.6,-13.75}, {6.2,-16.7}, {9.2,-17.63},
        {12.2,-17.65}, {15.2,-14.5}, {21.2,-1.3}, {22.0,0.0}
    };

    double maxLagrangeError = 0.0;
    double maxNewtonError = 0.0;
    double sumLagrangeError = 0.0;
    double sumNewtonError = 0.0;
    int count = 0;

    for (int i = 0; i < xList.size(); i++) {
        double x = xList[i];
        double y_original = yList[i];
        double y_lagrange = lagranj(x);
        double y_newton = newton(x);

        double error_lagrange = std::abs(y_lagrange - y_original);
        double error_newton = std::abs(y_newton - y_original);

        maxLagrangeError = std::max(maxLagrangeError, error_lagrange);
        maxNewtonError = std::max(maxNewtonError, error_newton);
        sumLagrangeError += error_lagrange;
        sumNewtonError += error_newton;
        count++;

        qDebug() << "x =" << x << "Оригинал:" << y_original
                 << "Лагранж:" << y_lagrange << "Погрешность:" << error_lagrange
                 << "Ньютон:" << y_newton << "Погрешность:" << error_newton;
    }

    double avgLagrangeError = sumLagrangeError / count;
    double avgNewtonError = sumNewtonError / count;

    qDebug() << "=== РЕЗУЛЬТАТЫ ПОГРЕШНОСТИ ===";
    qDebug() << "Максимальная погрешность Лагранжа:" << maxLagrangeError;
    qDebug() << "Максимальная погрешность Ньютона:" << maxNewtonError;
    qDebug() << "Средняя погрешность Лагранжа:" << avgLagrangeError;
    qDebug() << "Средняя погрешность Ньютона:" << avgNewtonError;
}

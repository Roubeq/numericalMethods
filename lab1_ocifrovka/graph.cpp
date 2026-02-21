#include "graph.h"
#include <QFile>
#include <QVector>

using namespace std;

// Graph::Graph(QWidget *parent)
//     : QMainWindow(parent)
// {
//     setWindowTitle("Graph");
//     QWidget *centralWidget = new QWidget(this);
//     setCentralWidget(centralWidget);

//     QHBoxLayout *layout = new QHBoxLayout(centralWidget);

//     plot = new QCustomPlot();
//     plot->xAxis->setLabel("Ось X");
//     plot->yAxis->setLabel("Ось Y");
//     plot->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom | QCP::iSelectPlottables);

//     // plot2 = new QCustomPlot();
//     // plot2->xAxis->setLabel("Ось X");
//     // plot2->yAxis->setLabel("Ось Y");
//     // plot2->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom | QCP::iSelectPlottables);

//     layout->addWidget(plot);
//     //layout->addWidget(plot2);
//     loadParametersX();
//     loadParametersYFromFile();

//     // QCPCurve *curve = new QCPCurve(plot->xAxis, plot->yAxis);
//     // curve->setData(xList, yList);
//     // curve->setLineStyle(QCPCurve::lsLine);
//     // QColor color = QColor::fromHsv((1 * 39) % 360, 180, 200);
//     // curve->setPen(QPen(color, 2));
//     // curve->setName(QString("Кривая"));
//     // plot->setAntialiasedElements(QCP::aeAll);
//     // plot->setNotAntialiasedElement(QCP::aeNone);
//     // plot->xAxis->setRange(0, 26);
//     // plot->yAxis->setRange(0, -20);

//     // QCPCurve *curve2 = new QCPCurve(plot2->xAxis, plot2->yAxis);
//     // curve2->setData(xList, yList);
//     // curve2->setLineStyle(QCPCurve::lsLine);
//     // QColor color2 = QColor::fromHsv((1 * 37) % 360, 180, 200);
//     // curve2->setPen(QPen(color2, 2));
//     // curve2->setName(QString("Кривая"));
//     // plot2->setAntialiasedElements(QCP::aeAll);
//     // plot2->setNotAntialiasedElement(QCP::aeNone);
//     // plot2->xAxis->setRange(0, 26);
//     // plot2->yAxis->setRange(0, -20);

//     // plot->replot();
//     // plot2->replot();

//     // plotLagranj();
//     // plotNewton();
//     // calculateError();
//     // QCPCurve *graph = new QCPCurve(plot->xAxis,plot->yAxis);
//     // graph->setData(xList,yList);
//     // graph->setLineStyle(QCPCurve::lsLine);
//     // QColor color = QColor::fromHsv(353 % 360,180,200);
//     // graph->setPen(QPen(color,2));
//     // graph->setName(QString("Оцифрованный график"));
//     // plot->xAxis->setRange(-5,30);
//     // plot->yAxis->setRange(-30,5);

//     //calculateCubicSpline();
//     //calculateNumericalDerivatives();
//     //calculateIntegrationMethods();

//     initialSpeed = 5.0;      // Начальная скорость 5 ед/с
//     gravity = 9.81;          // Ускорение свободного падения
//     initialHeight = 0.0;     // Будет установлена позже

// };

Graph::Graph(QWidget *parent)
    : QMainWindow(parent)
{
    setWindowTitle("Graph - Моделирование движения");
    QWidget *centralWidget = new QWidget(this);
    setCentralWidget(centralWidget);

    QScreen *screen = QGuiApplication::primaryScreen();
    QRect screenGeometry = screen->geometry();
    int width = screenGeometry.width() * 0.9;
    int height = screenGeometry.height() * 0.9;
    resize(width, height);
    move(screenGeometry.width() * 0.05, screenGeometry.height() * 0.05);

    QVBoxLayout *mainLayout = new QVBoxLayout(centralWidget);
    mainLayout->setContentsMargins(5, 5, 5, 5);

    QWidget *plotsContainer = new QWidget(this);
    QHBoxLayout *plotsLayout = new QHBoxLayout(plotsContainer);
    plotsLayout->setContentsMargins(0, 0, 0, 0);
    plotsLayout->setSpacing(5);

    plot = new QCustomPlot();
    plot->xAxis->setLabel("Координата X");
    plot->yAxis->setLabel("Координата Y");
    plot->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom | QCP::iSelectPlottables);
    plotsLayout->addWidget(plot, 1);

    velocityPlot = new QCustomPlot();
    velocityPlot->xAxis->setLabel("Время, с");
    velocityPlot->yAxis->setLabel("Скорость, ед/с");
    velocityPlot->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom | QCP::iSelectPlottables);
    plotsLayout->addWidget(velocityPlot, 1);

    mainLayout->addWidget(plotsContainer, 1);

    loadParametersX();
    loadParametersYFromFile();

    initialSpeed = 0.95;
    gravity = 9.81;
    initialHeight = 0.0;

    animationTimer = new QTimer(this);
    animationTime = 0.0;
    animationDuration = 10.0;
    totalPathLength = 0.0;
    movingPoint = nullptr;
    velocityGraph = nullptr;

    connect(animationTimer, &QTimer::timeout, this, &Graph::updateAnimation);

    createGifButton();

    runMotionSimulation();

    plot->xAxis->setRange(-5, 30);
    plot->yAxis->setRange(-30, 5);
    plot->legend->setVisible(true);
    plot->replot();

    animationTimer->start(50);
}

void Graph::createGifButton() {
    QWidget *controlWidget = new QWidget(this);
    QHBoxLayout *controlLayout = new QHBoxLayout(controlWidget);
    controlLayout->setContentsMargins(5, 5, 5, 5);

    QPushButton *gifButton = new QPushButton("📹 Создать GIF анимацию", this);
    gifButton->setMinimumHeight(40);
    gifButton->setStyleSheet("QPushButton {"
                             "background-color: #4CAF50;"
                             "color: white;"
                             "font-weight: bold;"
                             "padding: 8px 16px;"
                             "border-radius: 4px;"
                             "}"
                             "QPushButton:hover {"
                             "background-color: #45a049;"
                             "}");

    controlLayout->addStretch();
    controlLayout->addWidget(gifButton);
    controlLayout->addStretch();

    QVBoxLayout *mainLayout = qobject_cast<QVBoxLayout*>(centralWidget()->layout());
    if (mainLayout) {
        mainLayout->addWidget(controlWidget);
    }

    connect(gifButton, &QPushButton::clicked, this, [=]() {
        bool wasActive = animationTimer->isActive();
        if (wasActive) {
            animationTimer->stop();
        }
        QString fileName = QFileDialog::getSaveFileName(this,
                                                        "Сохранить GIF анимацию",
                                                        QApplication::applicationDirPath() + "/animation.gif",
                                                        "GIF Files (*.gif)");

        if (!fileName.isEmpty()) {
            if (!fileName.endsWith(".gif", Qt::CaseInsensitive)) {
                fileName += ".gif";
            }

            QMessageBox::information(this, "Создание GIF",
                                     "Начинается создание GIF анимации...\nЭто может занять несколько секунд.");

            createGifTwoPlots(fileName, 60, 33); // 60 кадров, ~30 FPS

            QMessageBox::information(this, "Готово",
                                     QString("GIF анимация сохранена в:\n%1\n\n"
                                             "Размер: %2x%3 пикселей\n"
                                             "Длительность: ~2 секунды\n"
                                             "Кадров: 60\n"
                                             "FPS: 30").arg(fileName)
                                         .arg(plot->width() + velocityPlot->width())
                                         .arg(qMax(plot->height(), velocityPlot->height())));
        }
        if (wasActive) {
            animationTimer->start(50);
        }
    });
}

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
    vector<pair<double,double>> listLagranj = {{0.0,0.0},{5.2,-15.5},{6.2,-16.7},{9.2,-17.63},{12.2,-17.65},{15.2,-14.5},{17.2,-8.5},{21.2,-1.3},{22.0,0.0}};
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
    vector<pair<double,double>> points = {{0.0,0.0},{5.2,-15.5},{6.2,-16.7},{9.2,-17.63},{12.2,-17.65},{15.2,-14.5},{17.2,-8.5},{21.2,-1.3},{22.0,0.0}};
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
    vector<pair<double,double>> points = {{0.0,0.0},{5.2,-15.5},{6.2,-16.7},{9.2,-17.63},{12.2,-17.65},{15.2,-14.5},{17.2,-8.5},{21.2,-1.3},{22.0,0.0}};

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
    vector<pair<double,double>> points = {{0.0,0.0},{5.2,-15.5},{6.2,-16.7},{9.2,-17.63},{12.2,-17.65},{15.2,-14.5},{17.2,-8.5},{21.2,-1.3},{22.0,0.0}};

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

void Graph::calculateCubicSpline() {
    vector<pair<double, double>> points = {
        {0.0, 0.0},
        {0.8, 0.01},
        {1.4,-0.1},
        {2.0, -0.3},
        {2.8,-0.7},
        {3.2,-1},
        {3.6, -1.6},
        {5.2, -15.5},
        {5.7,-16.1},
        {6.2,-16.7},
        {6.8, -17.35},
        {7.4,-17.3},
        {8.4, -17.68},
        {10.0, -17.71},
        {11.6, -17.71},
        {13.2, -17.2},
        {14.8, -14.5},
        {15.4,-10.8},
        {15.8,-10},
        {16.2,-9.6},
        {16.4, -9.35},
        {17.2,-8.5},
        {17.8,-7.45},
        {18.0, -6.5},
        {19.6, -2.4},
        {21.2, -1.3},
        {22.0, 0.0}
    };
    int n = points.size();
    if (n < 2) {
        qDebug() << "Недостаточно точек для сплайна";
        return;
    }

    QVector<double> x(n), y(n);
    for (int i = 0; i < n; ++i) {
        x[i] = points[i].first;
        y[i] = points[i].second;
    }

    for (int i = 0; i < n - 1; ++i) {
        if (x[i] >= x[i + 1]) {
            qDebug() << "Ошибка: x не строго возрастают";
            return;
        }
    }

    vector<double> h(n - 1);
    for (int i = 0; i < n - 1; ++i) {
        h[i] = x[i + 1] - x[i];
    }

    vector<double> alpha(n - 1, 0.0);
    for (int i = 1; i < n - 1; ++i) {
        alpha[i] = (3.0 / h[i]) * (y[i + 1] - y[i]) - (3.0 / h[i - 1]) * (y[i] - y[i - 1]);
    }

    vector<double> c(n, 0.0);
    vector<double> l(n, 1.0), mu(n, 0.0), z(n, 0.0);
    for (int i = 1; i < n - 1; ++i) {
        l[i] = 2 * (x[i + 1] - x[i - 1]) - h[i - 1] * mu[i - 1];
        mu[i] = h[i] / l[i];
        z[i] = (alpha[i] - h[i - 1] * z[i - 1]) / l[i];
    }

    for (int i = n - 2; i >= 0; --i) {
        c[i] = z[i] - mu[i] * c[i + 1];
    }

    vector<double> b(n - 1), d(n - 1);
    for (int i = 0; i < n - 1; ++i) {
        b[i] = (y[i + 1] - y[i]) / h[i] - h[i] * (c[i + 1] + 2 * c[i]) / 3.0;
        d[i] = (c[i + 1] - c[i]) / (3.0 * h[i]);
    }

    QVector<double> spline_x, spline_y;
    for (int i = 0; i < n - 1; ++i) {
        int segments = 100;
        double step = h[i] / segments;
        for (int j = 0; j <= segments; ++j) {
            double t = x[i] + j * step;
            double dx = t - x[i];
            double s = y[i] + b[i] * dx + c[i] * dx * dx + d[i] * dx * dx * dx;
            spline_x.append(t);
            spline_y.append(s);
        }
    }

    QCPGraph *cubicGraph = plot->addGraph();
    cubicGraph->setData(spline_x, spline_y);
    cubicGraph->setPen(QPen(Qt::green, 2));
    cubicGraph->setName("Кубический сплайн");
    plot->legend->setVisible(true);
    plot->replot();
}

void Graph::calculateNumericalDerivatives() {
    qDebug() << "=== ЧИСЛЕННОЕ ДИФФЕРЕНЦИРОВАНИЕ ===";

    QVector<double> deriv_forward = forwardDifference(xList, yList);
    QVector<double> deriv_backward = backwardDifference(xList, yList);
    QVector<double> deriv_central = centralDifference(xList, yList);

    printErrorAnalysis(deriv_forward, deriv_backward, deriv_central);

    plotDerivativesAndOriginal(deriv_forward, deriv_backward, deriv_central);
}

QVector<double> Graph::forwardDifference(const QVector<double>& x, const QVector<double>& y) {
    int n = x.size();
    QVector<double> derivative(n, 0.0);

    for (int i = 0; i < n - 1; i++) {
        double h = x[i+1] - x[i];
        if (h > 0) {
            derivative[i] = (y[i+1] - y[i]) / h;
        }
    }

    if (n > 1) {
        double h = x[n-1] - x[n-2];
        derivative[n-1] = (y[n-1] - y[n-2]) / h;
    }

    return derivative;
}

QVector<double> Graph::backwardDifference(const QVector<double>& x, const QVector<double>& y) {
    int n = x.size();
    QVector<double> derivative(n, 0.0);

    if (n > 1) {
        double h = x[1] - x[0];
        derivative[0] = (y[1] - y[0]) / h;
    }

    for (int i = 1; i < n; i++) {
        double h = x[i] - x[i-1];
        if (h > 0) {
            derivative[i] = (y[i] - y[i-1]) / h;
        }
    }

    return derivative;
}

QVector<double> Graph::centralDifference(const QVector<double>& x, const QVector<double>& y) {
    int n = x.size();
    QVector<double> derivative(n, 0.0);

    if (n > 1) {
        double h = x[1] - x[0];
        derivative[0] = (y[1] - y[0]) / h;
    }

    for (int i = 1; i < n - 1; i++) {
        double h_forward = x[i+1] - x[i];
        double h_backward = x[i] - x[i-1];
        double h_avg = (h_forward + h_backward) / 2.0;

        if (h_avg > 0) {
            derivative[i] = (y[i+1] - y[i-1]) / (2.0 * h_avg);
        }
    }

    if (n > 1) {
        double h = x[n-1] - x[n-2];
        derivative[n-1] = (y[n-1] - y[n-2]) / h;
    }

    return derivative;
}

QVector<double> Graph::calculateSecondDerivative(const QVector<double>& x, const QVector<double>& y) {
    int n = x.size();
    QVector<double> second_deriv(n, 0.0);

    for (int i = 1; i < n - 1; i++) {
        double h_forward = x[i+1] - x[i];
        double h_backward = x[i] - x[i-1];
        double h_avg = (h_forward + h_backward) / 2.0;

        if (h_avg > 0) {
            second_deriv[i] = (y[i+1] - 2*y[i] + y[i-1]) / (h_avg * h_avg);
        }
    }

    if (n > 2) {
        double h1 = x[1] - x[0];
        double h2 = x[2] - x[1];
        if (h1 > 0 && h2 > 0) {
            second_deriv[0] = (y[2] - 2*y[1] + y[0]) / (h1 * h2);
        }

        h1 = x[n-1] - x[n-2];
        h2 = x[n-2] - x[n-3];
        if (h1 > 0 && h2 > 0) {
            second_deriv[n-1] = (y[n-1] - 2*y[n-2] + y[n-3]) / (h1 * h2);
        }
    }

    return second_deriv;
}

QVector<double> Graph::calculateThirdDerivative(const QVector<double>& x, const QVector<double>& y) {
    int n = x.size();
    QVector<double> third_deriv(n, 0.0);

    for (int i = 2; i < n - 2; i++) {
        double h = (x[i+1] - x[i-1]) / 2.0;

        if (h > 0) {
            third_deriv[i] = (y[i+2] - 2*y[i+1] + 2*y[i-1] - y[i-2]) / (2 * h * h * h);
        }
    }

    if (n > 4) {
        double h = (x[2] - x[0]) / 2.0;
        if (h > 0) {
            third_deriv[1] = (y[3] - 2*y[2] + 2*y[0] - y[0]) / (2 * h * h * h);
        }

        h = (x[n-1] - x[n-3]) / 2.0;
        if (h > 0) {
            third_deriv[n-2] = (y[n-1] - 2*y[n-1] + 2*y[n-3] - y[n-4]) / (2 * h * h * h);
        }
    }

    return third_deriv;
}

void Graph::printErrorAnalysis(const QVector<double>& deriv_forward,
                               const QVector<double>& deriv_backward,
                               const QVector<double>& deriv_central) {
    int n = deriv_central.size();

    qDebug() << "=== ТОЧНЫЙ АНАЛИЗ ПОГРЕШНОСТЕЙ ===";

    QVector<double> second_deriv = calculateSecondDerivative(xList, yList);
    QVector<double> third_deriv = calculateThirdDerivative(xList, yList);

    double M2 = 0.0, M3 = 0.0;
    int valid_points_M2 = 0, valid_points_M3 = 0;

    for (int i = 0; i < n; i++) {
        if (!qIsNaN(second_deriv[i]) && qIsFinite(second_deriv[i])) {
            M2 = qMax(M2, qAbs(second_deriv[i]));
            valid_points_M2++;
        }
        if (i >= 2 && i < n-2 && !qIsNaN(third_deriv[i]) && qIsFinite(third_deriv[i])) {
            M3 = qMax(M3, qAbs(third_deriv[i]));
            valid_points_M3++;
        }
    }

    qDebug() << "M2 = max|f''(x)| =" << M2 << "(по" << valid_points_M2 << "точкам)";
    qDebug() << "M3 = max|f'''(x)| =" << M3 << "(по" << valid_points_M3 << "точкам)";

    double total_h = 0.0;
    int h_count = 0;
    for (int i = 1; i < n; i++) {
        double h = xList[i] - xList[i-1];
        if (h > 0) {
            total_h += h;
            h_count++;
        }
    }
    double h_avg = (h_count > 0) ? total_h / h_count : 0.0;

    qDebug() << "Средний шаг h =" << h_avg;

    double guaranteed_forward_error = (h_avg / 2.0) * M2;
    double guaranteed_backward_error = (h_avg / 2.0) * M2;
    double guaranteed_central_error = (h_avg * h_avg / 6.0) * M3;

    qDebug() << "\n=== ГАРАНТИРОВАННЫЕ ГРАНИЦЫ ПОГРЕШНОСТЕЙ ===";
    qDebug() << "Метод 'Вперёд': |E| ≤ h/2 * M2 =" << guaranteed_forward_error;
    qDebug() << "Метод 'Назад': |E| ≤ h/2 * M2 =" << guaranteed_backward_error;
    qDebug() << "Метод 'Центральный': |E| ≤ h²/6 * M3 =" << guaranteed_central_error;
}

void Graph::plotDerivativesAndOriginal(const QVector<double>& deriv_forward,
                                       const QVector<double>& deriv_backward,
                                       const QVector<double>& deriv_central) {
    QMainWindow *derivativeWindow = new QMainWindow();
    derivativeWindow->setWindowTitle("Численное дифференцирование");
    derivativeWindow->resize(1200, 600);

    QWidget *centralWidget = new QWidget(derivativeWindow);
    derivativeWindow->setCentralWidget(centralWidget);

    QHBoxLayout *layout = new QHBoxLayout(centralWidget);

    QCustomPlot *leftPlot = new QCustomPlot();
    leftPlot->xAxis->setLabel("Ось X");
    leftPlot->yAxis->setLabel("Производная f'(x)");
    leftPlot->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom | QCP::iSelectPlottables);

    QCustomPlot *rightPlot = new QCustomPlot();
    rightPlot->xAxis->setLabel("Ось X");
    rightPlot->yAxis->setLabel("Ось Y");
    rightPlot->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom | QCP::iSelectPlottables);

    layout->addWidget(leftPlot);
    layout->addWidget(rightPlot);


    QCPGraph *graphForward = leftPlot->addGraph();
    graphForward->setData(xList, deriv_forward);
    graphForward->setPen(QPen(Qt::red, 2));
    graphForward->setName("Вперёд O(h)");
    graphForward->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssCircle, 4));

    QCPGraph *graphBackward = leftPlot->addGraph();
    graphBackward->setData(xList, deriv_backward);
    graphBackward->setPen(QPen(Qt::blue, 2));
    graphBackward->setName("Назад O(h)");
    graphBackward->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssSquare, 4));

    QCPGraph *graphCentral = leftPlot->addGraph();
    graphCentral->setData(xList, deriv_central);
    graphCentral->setPen(QPen(Qt::green, 2));
    graphCentral->setName("Центральная O(h²)");
    graphCentral->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssDiamond, 4));

    leftPlot->rescaleAxes();

    leftPlot->legend->setVisible(true);
    leftPlot->legend->setBrush(QBrush(QColor(255,255,255,230)));

    QCPGraph *originalGraph = rightPlot->addGraph();
    originalGraph->setData(xList, yList);
    originalGraph->setLineStyle(QCPGraph::lsNone);
    originalGraph->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssCircle, 6));
    originalGraph->setPen(QPen(QColor(120, 80, 200), 2));
    originalGraph->setName("Оцифрованный график");

    QCPGraph *smoothGraph = rightPlot->addGraph();
    smoothGraph->setData(xList, yList);
    smoothGraph->setPen(QPen(QColor(120, 80, 200, 100), 1));
    smoothGraph->setName("Сглаженная линия");

    rightPlot->xAxis->setRange(-5, 30);
    rightPlot->yAxis->setRange(-30, 5);

    rightPlot->legend->setVisible(true);
    rightPlot->legend->setBrush(QBrush(QColor(255,255,255,230)));

    leftPlot->replot();
    rightPlot->replot();

    derivativeWindow->show();
}



double Graph::leftRectangleMethod(const QVector<double>& x, const QVector<double>& y) {
    if (x.size() != y.size() || x.size() < 2) return 0.0;

    double integral = 0.0;
    for (int i = 0; i < x.size() - 1; i++) {
        double width = x[i+1] - x[i];
        integral += width * y[i];
    }
    return integral;
}

double Graph::rightRectangleMethod(const QVector<double>& x, const QVector<double>& y) {
    if (x.size() != y.size() || x.size() < 2) return 0.0;

    double integral = 0.0;
    for (int i = 0; i < x.size() - 1; i++) {
        double width = x[i+1] - x[i];
        integral += width * y[i+1];
    }
    return integral;
}

double Graph::centralRectangleMethod(const QVector<double>& x, const QVector<double>& y) {
    if (x.size() != y.size() || x.size() < 3) {
        qDebug() << "Ошибка: недостаточно точек для интегрирования (нужно минимум 3)";
        return 0.0;
    }

    double integral = 0.0;

    for (int i = 0; i < x.size() - 2; i += 2) {
        double x_left = x[i];
        double x_mid = x[i + 1];    // Средняя точка
        double x_right = x[i + 2];  // Правая граница

        double h = x_mid - x_left;
        double double_step = 2.0 * h; // 2h

        double f_mid = y[i + 1];

        integral += f_mid * double_step;
    }

    return integral;
}

void Graph::printIntegrationComparisonTable() {
    qDebug() << "\n" << QString().fill('=', 120);
    qDebug() << "ТАБЛИЦА СРАВНЕНИЯ МЕТОДОВ ЧИСЛЕННОГО ИНТЕГРИРОВАНИЯ";
    qDebug() << QString().fill('=', 120);

    double leftRect = leftRectangleMethod(xList, yList);
    double rightRect = rightRectangleMethod(xList, yList);
    double centralRect = centralRectangleMethod(xList, yList);
    double trapezoidal = trapezoidalMethod(xList, yList);
    double simpson = simpsonMethod(xList, yList);

    double M1 = calculateFirstDerivativeMax();
    double M2 = calculateSecondDerivativeMax();
    double M4 = calculateFourthDerivativeMax();

    double h_lr = 0.0, h_cent = 0.0, h_trap = 0.0, h_simp = 0.0;
    int count_lr = 0, count_cent = 0, count_trap = 0, count_simp = 0;

    for (int i = 0; i < xList.size() - 1; i++) {
        double h = xList[i+1] - xList[i];
        if (h > 0) {
            h_lr += h;
            h_trap += h;
            count_lr++;
            count_trap++;
        }
    }

    for (int i = 0; i < xList.size() - 2; i += 2) {
        double h = xList[i+1] - xList[i];
        if (h > 0) {
            h_cent += 2.0 * h;
            h_simp += h;
            count_cent++;
            count_simp++;
        }
    }

    h_lr = (count_lr > 0) ? h_lr / count_lr : 0.0;
    h_cent = (count_cent > 0) ? h_cent / count_cent : 0.0;
    h_trap = (count_trap > 0) ? h_trap / count_trap : 0.0;
    h_simp = (count_simp > 0) ? h_simp / count_simp : 0.0;

    double error_left = (M1 * h_lr) / 2.0;
    double error_right = (M1 * h_lr ) / 2.0;
    double error_central = (M2 * h_cent * h_cent) / 24.0;
    double error_trapezoidal = (M2 * h_trap * h_trap) / 12.0;
    double error_simpson = (M4 * h_simp * h_simp * h_simp * h_simp) / 180.0;

    qDebug() << QString("│ %1 │ %2 │ %3 │ %4 │ %5 │")
                    .arg("Метод", -25)
                    .arg("Интеграл", -12)
                    .arg("Погрешность", -15)
                    .arg("Формула погрешности", -35)
                    .arg("Порядок", -8);

    qDebug() << QString().fill('-', 120);

    struct MethodData {
        QString name;
        double integral;
        double error;
        QString error_formula;
        QString order;
    };

    QVector<MethodData> methods = {
        {"Левые прямоугольники", leftRect, error_left,
         "|I - In| <= (M1 * h)/2", "O(h)"},

        {"Правые прямоугольники", rightRect, error_right,
         "|I - In| <= (M1 * h)/2", "O(h)"},

        {"Центральные прямоуг.", centralRect, error_central,
         "|I - In| <= (M2 * (2h)^2)/24", "O(h^2)"},

        {"Метод трапеций", trapezoidal, error_trapezoidal,
         "|I - In| <= (M2 * h^2)/12", "O(h^2)"},

        {"Метод Симпсона", simpson, error_simpson,
         "|I - In| <= (M4 * h^4)/180", "O(h^4)"}
    };

    for (const auto& method : methods) {
        qDebug() << QString("│ %1 │ %2 │ %3 │ %4 │ %5 │")
                        .arg(method.name, -25)
                        .arg(QString::number(method.integral, 'f', 6), -12)
                        .arg(QString::number(method.error, 'f', 8), -15)
                        .arg(method.error_formula, -35)
                        .arg(method.order, -8);
    }

    qDebug() << QString().fill('-', 120);
    qDebug() << QString().fill('=', 120);
}

void Graph::calculateIntegrationMethods() {
    qDebug() << "=== ЧИСЛЕННОЕ ИНТЕГРИРОВАНИЕ ===";

    printIntegrationComparisonTable();

    showLeftRectanglesWindow();
    showRightRectanglesWindow();
    showCentralRectanglesWindow();
    showTrapezoidalWindow();
}

void Graph::showCentralRectanglesWindow() {
    QMainWindow *window = new QMainWindow();
    window->setWindowTitle("Метод центральных прямоугольников (шаг = 2)");
    window->resize(800, 600);

    QWidget *centralWidget = new QWidget(window);
    window->setCentralWidget(centralWidget);

    QVBoxLayout *layout = new QVBoxLayout(centralWidget);

    QCustomPlot *plot = new QCustomPlot();
    plot->xAxis->setLabel("Ось X");
    plot->yAxis->setLabel("Ось Y");
    plot->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom);

    layout->addWidget(plot);

    QCPGraph *originalGraph = plot->addGraph();
    originalGraph->setData(xList, yList);
    originalGraph->setPen(QPen(Qt::blue, 3));
    originalGraph->setName("Исходная функция");

    QVector<double> usedPointsX, usedPointsY;
    for (int i = 0; i < xList.size() - 2; i += 2) {
        usedPointsX.append(xList[i + 1]);
        usedPointsY.append(yList[i + 1]);
    }

    QCPGraph *midPointsGraph = plot->addGraph();
    midPointsGraph->setData(usedPointsX, usedPointsY);
    midPointsGraph->setLineStyle(QCPGraph::lsNone);
    midPointsGraph->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssCircle, 10));
    midPointsGraph->setPen(QPen(Qt::red));
    midPointsGraph->setName("Используемые точки (cᵢ)");

    for (int i = 0; i < xList.size() - 2; i += 2) {
        double x_left = xList[i];
        double x_mid = xList[i + 1];
        double x_right = xList[i + 2];
        double y_mid = yList[i + 1];
        double h = xList[i+1] - xList[i];
        double double_step = 2.0 * h;

        QVector<double> rectX, rectY;
        rectX << x_left << x_right << x_right << x_left;
        rectY << 0 << 0 << y_mid << y_mid;

        QCPCurve *rectangle = new QCPCurve(plot->xAxis, plot->yAxis);
        rectangle->setData(rectX, rectY);
        rectangle->setPen(QPen(QColor(100, 100, 255), 2));
        rectangle->setBrush(QBrush(QColor(100, 100, 255, 80)));
    }

    plot->xAxis->setRange(-1, 23);
    plot->yAxis->setRange(-20, 2);
    //plot->legend->setVisible(true);
    plot->replot();

    double double_step_avg = 0.0;
    int count = 0;

    for (int i = 0; i < xList.size() - 2; i += 2) {
        double h = xList[i+1] - xList[i];
        if (h > 0) {
            double_step_avg += 2.0 * h;
            count++;
        }
    }
    double_step_avg = (count > 0) ? double_step_avg / count : 0.0;

    window->show();
}

void Graph::showLeftRectanglesWindow() {
    QMainWindow *window = new QMainWindow();
    window->setWindowTitle("Метод левых прямоугольников");
    window->resize(800, 600);

    QWidget *centralWidget = new QWidget(window);
    window->setCentralWidget(centralWidget);

    QVBoxLayout *layout = new QVBoxLayout(centralWidget);

    QCustomPlot *plot = new QCustomPlot();
    plot->xAxis->setLabel("Ось X");
    plot->yAxis->setLabel("Ось Y");
    plot->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom);

    layout->addWidget(plot);

    QCPGraph *originalGraph = plot->addGraph();
    originalGraph->setData(xList, yList);
    originalGraph->setPen(QPen(Qt::blue, 3));
    originalGraph->setName("Исходная функция");

    for (int i = 0; i < xList.size() - 1; i++) {
        double x1 = xList[i];
        double x2 = xList[i+1];
        double y = yList[i];

        QVector<double> rectX, rectY;
        rectX << x1 << x2 << x2 << x1;
        rectY << 0 << 0 << y << y;

        QCPCurve *rectangle = new QCPCurve(plot->xAxis, plot->yAxis);
        rectangle->setData(rectX, rectY);
        rectangle->setPen(QPen(QColor(255, 100, 100), 2));
        rectangle->setBrush(QBrush(QColor(255, 100, 100, 80)));

        QCPGraph *line = plot->addGraph();
        QVector<double> lineX, lineY;
        lineX << x1 << x1;
        lineY << 0 << y;
        line->setData(lineX, lineY);
        line->setPen(QPen(QColor(200, 50, 50), 2));
    }

    for (int i = 0; i < xList.size() - 1; i++) {
        QCPGraph *topLine = plot->addGraph();
        QVector<double> lineX, lineY;
        lineX << xList[i] << xList[i+1];
        lineY << yList[i] << yList[i];
        topLine->setData(lineX, lineY);
        topLine->setPen(QPen(QColor(200, 50, 50), 2));
    }

    plot->xAxis->setRange(-1, 23);
    plot->yAxis->setRange(-20, 2);
    //plot->legend->setVisible(true);
    plot->replot();

    window->show();
}

void Graph::showRightRectanglesWindow() {
    QMainWindow *window = new QMainWindow();
    window->setWindowTitle("Метод правых прямоугольников");
    window->resize(800, 600);

    QWidget *centralWidget = new QWidget(window);
    window->setCentralWidget(centralWidget);

    QVBoxLayout *layout = new QVBoxLayout(centralWidget);

    QCustomPlot *plot = new QCustomPlot();
    plot->xAxis->setLabel("Ось X");
    plot->yAxis->setLabel("Ось Y");
    plot->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom);

    layout->addWidget(plot);

    QCPGraph *originalGraph = plot->addGraph();
    originalGraph->setData(xList, yList);
    originalGraph->setPen(QPen(Qt::blue, 3));
    originalGraph->setName("Исходная функция");

    for (int i = 0; i < xList.size() - 1; i++) {
        double x1 = xList[i];
        double x2 = xList[i+1];
        double y = yList[i+1];

        QVector<double> rectX, rectY;
        rectX << x1 << x2 << x2 << x1;
        rectY << 0 << 0 << y << y;

        QCPCurve *rectangle = new QCPCurve(plot->xAxis, plot->yAxis);
        rectangle->setData(rectX, rectY);
        rectangle->setPen(QPen(QColor(100, 255, 100), 2));
        rectangle->setBrush(QBrush(QColor(100, 255, 100, 80)));

        QCPGraph *line = plot->addGraph();
        QVector<double> lineX, lineY;
        lineX << x2 << x2;
        lineY << 0 << y;
        line->setData(lineX, lineY);
        line->setPen(QPen(QColor(50, 200, 50), 2));
    }

    for (int i = 0; i < xList.size() - 1; i++) {
        QCPGraph *topLine = plot->addGraph();
        QVector<double> lineX, lineY;
        lineX << xList[i] << xList[i+1];
        lineY << yList[i+1] << yList[i+1];
        topLine->setData(lineX, lineY);
        topLine->setPen(QPen(QColor(50, 200, 50), 2));
    }

    plot->xAxis->setRange(-1, 23);
    plot->yAxis->setRange(-20, 2);
    //plot->legend->setVisible(true);
    plot->replot();

    window->show();
}

double Graph::calculateFirstDerivativeMax() {
    QVector<double> firstDeriv = centralDifference(xList, yList);
    double maxDeriv = 0.0;

    for (int i = 0; i < firstDeriv.size(); i++) {
        if (!qIsNaN(firstDeriv[i]) && qIsFinite(firstDeriv[i])) {
            maxDeriv = qMax(maxDeriv, qAbs(firstDeriv[i]));
        }
    }

    return maxDeriv;
}

double Graph::calculateSecondDerivativeMax() {
    QVector<double> secondDeriv = calculateSecondDerivative(xList, yList);
    double maxDeriv = 0.0;

    for (int i = 0; i < secondDeriv.size(); i++) {
        if (!qIsNaN(secondDeriv[i]) && qIsFinite(secondDeriv[i])) {
            maxDeriv = qMax(maxDeriv, qAbs(secondDeriv[i]));
        }
    }

    return maxDeriv;
}

void Graph::calculateIntegrationErrors(double leftRect, double rightRect, double centralRect) {
    qDebug() << "\n=== ОЦЕНКА ПОГРЕШНОСТЕЙ МЕТОДОВ ИНТЕГРИРОВАНИЯ ===";

    double M1 = calculateFirstDerivativeMax();
    double M2 = calculateSecondDerivativeMax();

    double h = 0.2;

    double error_left = (M1 * h) / 2.0;  // |I - I_n| ≤ (M₁ * h) / 2
    double error_right = (M1 * h) / 2.0; // |I - I_n| ≤ (M₁ * h) / 2
    double error_central = (M2)/24.0 * ((2*h)*(2*h)); // |I - I_n| ≤ (M₂ * (2h)²) / 24
    double trapezoidal = trapezoidalMethod(xList, yList);

    double h_avg_trap = 0.0;
    int count_trap = 0;
    for (int i = 0; i < xList.size() - 1; i++) {
        double h = xList[i+1] - xList[i];
        if (h > 0) {
            h_avg_trap += h;
            count_trap++;
        }
    }
    h_avg_trap = (count_trap > 0) ? h_avg_trap / count_trap : 0.0;

    double error_trapezoidal = (M2 * h_avg_trap * h_avg_trap) / 12.0;

    qDebug() << "\n=== ГАРАНТИРОВАННЫЕ ГРАНИЦЫ ПОГРЕШНОСТЕЙ ===";
    qDebug() << "Метод левых прямоугольников: |E| ≤ (M₁ * h²)/2 =" << error_left;
    qDebug() << "Метод правых прямоугольников: |E| ≤ (M₁ * h²)/2 =" << error_right;
    qDebug() << "Метод центральных прямоугольников: |E| ≤ (M₂ * (2h)²)/24 =" << error_central;
    qDebug() << "Метод трапеций: |E| ≤ (M₂ × h²)/12 =" << error_trapezoidal;
}

double Graph::trapezoidalMethod(const QVector<double>& x, const QVector<double>& y) {
    if (x.size() != y.size() || x.size() < 2) return 0.0;

    double integral = 0.0;
    for (int i = 0; i < x.size() - 1; i++) {
        double width = x[i+1] - x[i];
        double avg_height = (y[i] + y[i+1]) / 2.0;
        integral += width * avg_height;
    }
    return integral;
}

double Graph::simpsonMethod(const QVector<double>& x, const QVector<double>& y) {
    if (x.size() != y.size() || x.size() < 3) {
        qDebug() << "Ошибка: для метода Симпсона нужно минимум 3 точки";
        return 0.0;
    }

    int n = x.size() - 1; // количество интервалов
    if (n % 2 != 0) {
        qDebug() << "Предупреждение: для метода Симпсона желательно четное количество интервалов";
    }

    double integral = 0.0;

    for (int i = 0; i < n - 1; i += 2) {
        double h = x[i+1] - x[i];
        integral += (h / 3.0) * (y[i] + 4*y[i+1] + y[i+2]);
    }

    return integral;
}

double Graph::calculateFourthDerivativeMax() {
    // Аппроксимируем четвертую производную через вторые разности
    QVector<double> fourthDeriv(xList.size(), 0.0);

    for (int i = 2; i < xList.size() - 2; i++) {
        double h = (xList[i+1] - xList[i-1]) / 2.0;
        if (h > 0) {
            // f⁽⁴⁾(x) ≈ [f(x+2h) - 4f(x+h) + 6f(x) - 4f(x-h) + f(x-2h)] / h⁴
            fourthDeriv[i] = (yList[i+2] - 4*yList[i+1] + 6*yList[i] - 4*yList[i-1] + yList[i-2]) / (h * h * h * h);
        }
    }

    double maxDeriv = 0.0;
    for (int i = 0; i < fourthDeriv.size(); i++) {
        if (!qIsNaN(fourthDeriv[i]) && qIsFinite(fourthDeriv[i])) {
            maxDeriv = qMax(maxDeriv, qAbs(fourthDeriv[i]));
        }
    }

    return maxDeriv;
}

void Graph::showTrapezoidalWindow() {
    QMainWindow *window = new QMainWindow();
    window->setWindowTitle("Метод трапеций");
    window->resize(800, 600);

    QWidget *centralWidget = new QWidget(window);
    window->setCentralWidget(centralWidget);

    QVBoxLayout *layout = new QVBoxLayout(centralWidget);

    QCustomPlot *plot = new QCustomPlot();
    plot->xAxis->setLabel("Ось X");
    plot->yAxis->setLabel("Ось Y");
    plot->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom);

    layout->addWidget(plot);

    QCPGraph *originalGraph = plot->addGraph();
    originalGraph->setData(xList, yList);
    originalGraph->setPen(QPen(Qt::blue, 3));
    originalGraph->setName("Исходная функция");

    for (int i = 0; i < xList.size() - 1; i++) {
        double x1 = xList[i];
        double x2 = xList[i+1];
        double y1 = yList[i];
        double y2 = yList[i+1];

        QVector<double> trapX, trapY;
        trapX << x1 << x2 << x2 << x1;
        trapY << 0 << 0 << y2 << y1;

        QCPCurve *trapezoid = new QCPCurve(plot->xAxis, plot->yAxis);
        trapezoid->setData(trapX, trapY);
        trapezoid->setPen(QPen(QColor(255, 165, 0), 2));
        trapezoid->setBrush(QBrush(QColor(255, 165, 0, 80)));

        QCPGraph *slopeLine = plot->addGraph();
        QVector<double> lineX, lineY;
        lineX << x1 << x2;
        lineY << y1 << y2;
        slopeLine->setData(lineX, lineY);
        slopeLine->setPen(QPen(QColor(200, 100, 0), 2));
    }

    plot->xAxis->setRange(-1, 23);
    plot->yAxis->setRange(-20, 2);
    //plot->legend->setVisible(true);
    plot->replot();

    window->show();
}

void Graph::calculateSimpsonMethod() {
    qDebug() << "\n=== МЕТОД СИМПСОНА ===";

    double simpsonIntegral = simpsonMethod(xList, yList);

    qDebug() << "Интеграл по методу Симпсона:" << simpsonIntegral;
    double M4 = calculateFourthDerivativeMax();
    double h_avg = 0.0;
    int count = 0;

    for (int i = 0; i < xList.size() - 1; i += 2) {
        double h = xList[i+1] - xList[i];
        if (h > 0) {
            h_avg += h;
            count++;
        }
    }
    h_avg = (count > 0) ? h_avg / count : 0.0;

    // Погрешность метода Симпсона: |E| ≤ (M₄ * h⁴) / 180
    double error_simpson = (M4 * h_avg * h_avg * h_avg * h_avg) / 180.0;

    qDebug() << "\n=== ПОГРЕШНОСТЬ МЕТОДА СИМПСОНА ===";
    qDebug() << "M4 = max|f(4)(x)| =" << M4;
    qDebug() << "Средний шаг h =" << h_avg;
    qDebug() << "Погрешность:  (M4 * h^4)/180 =" << error_simpson;

     printIntegrationComparisonTable();
}

void Graph::calculateSplineForAnimation() {
    if (xList.size() < 2 || yList.size() < 2 || xList.size() != yList.size()) {
        qDebug() << "Недостаточно точек для построения сплайна";
        return;
    }

    QVector<QPair<double, double>> points;
    for (int i = 0; i < xList.size(); ++i) {
        points.append(qMakePair(xList[i], yList[i]));
    }

    std::sort(points.begin(), points.end(),
              [](const QPair<double, double>& a, const QPair<double, double>& b) {
                  return a.first < b.first;
              });

    QVector<double> uniqueX, uniqueY;
    for (int i = 0; i < points.size(); ++i) {
        if (i == 0 || points[i].first != points[i-1].first) {
            uniqueX.append(points[i].first);
            uniqueY.append(points[i].second);
        }
    }

    int n = uniqueX.size();
    if (n < 2) {
        qDebug() << "Недостаточно уникальных точек для сплайна";
        return;
    }

    initialHeight = uniqueY[0];
    qDebug() << "Начальная высота:" << initialHeight;

    QVector<double> h(n - 1);
    for (int i = 0; i < n - 1; ++i) {
        h[i] = uniqueX[i + 1] - uniqueX[i];
    }

    QVector<double> alpha(n, 0.0);
    for (int i = 1; i < n - 1; ++i) {
        alpha[i] = (3.0 / h[i]) * (uniqueY[i + 1] - uniqueY[i]) -
                   (3.0 / h[i - 1]) * (uniqueY[i] - uniqueY[i - 1]);
    }

    QVector<double> c(n, 0.0);
    QVector<double> l(n, 1.0), mu(n, 0.0), z(n, 0.0);

    for (int i = 1; i < n - 1; ++i) {
        l[i] = 2 * (uniqueX[i + 1] - uniqueX[i - 1]) - h[i - 1] * mu[i - 1];
        mu[i] = h[i] / l[i];
        z[i] = (alpha[i] - h[i - 1] * z[i - 1]) / l[i];
    }

    for (int i = n - 2; i >= 0; --i) {
        c[i] = z[i] - mu[i] * c[i + 1];
    }

    QVector<double> b(n - 1), d(n - 1);
    for (int i = 0; i < n - 1; ++i) {
        b[i] = (uniqueY[i + 1] - uniqueY[i]) / h[i] -
               h[i] * (c[i + 1] + 2 * c[i]) / 3.0;
        d[i] = (c[i + 1] - c[i]) / (3.0 * h[i]);
    }

    // Создаем плотный набор точек сплайна для анимации
    splinePointsX.clear();
    splinePointsY.clear();
    velocities.clear();

    const int SEGMENTS_PER_INTERVAL = 50;
    for (int i = 0; i < n - 1; ++i) {
        for (int j = 0; j <= SEGMENTS_PER_INTERVAL; ++j) {
            double t = uniqueX[i] + (h[i] * j) / SEGMENTS_PER_INTERVAL;
            double dx = t - uniqueX[i];
            double s = uniqueY[i] + b[i] * dx + c[i] * dx * dx + d[i] * dx * dx * dx;
            splinePointsX.append(t);
            splinePointsY.append(s);
        }
    }

    // Рассчитываем скорости в каждой точке по закону сохранения энергии
    calculateVelocities();

    // Рассчитываем длины дуг и время прохождения
    calculateArclengthsAndTime();

    calculateVelocityComponents();
    plotVelocityComponents();

    plotSplineForAnimation();
}

void Graph::calculateVelocities() {
    velocities.clear();

    if (splinePointsY.isEmpty()) return;

    for (int i = 0; i < splinePointsY.size(); ++i) {
        double height = splinePointsY[i];
        double deltaH = initialHeight - height;

        // Формула: v = √(2g(h₀ - h) + v₀²)
        double underSqrt = 2.0 * gravity * deltaH + initialSpeed * initialSpeed;

        if (underSqrt >= 0) {
            double velocity = sqrt(underSqrt);
            velocities.append(velocity);
        } else {
            velocities.append(0.0);
        }

    }
}

void Graph::calculateArclengthsAndTime() {
    splineArclengths.clear();
    totalPathLength = 0.0;

    if (splinePointsX.size() < 2 || velocities.isEmpty()) return;

    splineArclengths.append(0.0);

    QVector<double> segmentLengths;
    for (int i = 1; i < splinePointsX.size(); ++i) {
        double dx = splinePointsX[i] - splinePointsX[i-1];
        double dy = splinePointsY[i] - splinePointsY[i-1];
        double segmentLength = sqrt(dx*dx + dy*dy);
        segmentLengths.append(segmentLength);
        totalPathLength += segmentLength;
        splineArclengths.append(totalPathLength);
    }

    double totalTime = 0.0;
    QVector<double> cumulativeTimes;
    cumulativeTimes.append(0.0);

    for (int i = 0; i < segmentLengths.size(); ++i) {
        double avgVelocity = (velocities[i] + velocities[i+1]) / 2.0;

        if (avgVelocity > 0) {
            double segmentTime = segmentLengths[i] / avgVelocity;
            totalTime += segmentTime;
        } else {
            totalTime = 1e10;
        }

        cumulativeTimes.append(totalTime);
    }

    QVector<double> normalizedTimes;
    if (totalTime > 0) {
        for (int i = 0; i < cumulativeTimes.size(); ++i) {
            normalizedTimes.append(cumulativeTimes[i] / totalTime);
        }
        splineArclengths = normalizedTimes;
    }

    animationDuration = totalTime;
}

void Graph::updateAnimation() {
    if (splinePointsX.isEmpty() || splineArclengths.isEmpty()) return;

    double deltaTime = 0.05 / animationDuration;
    animationTime += deltaTime;

    if (animationTime >= 1.0) {
        animationTime = 0.0;
    }

    int segmentIndex = 0;
    for (int i = 1; i < splineArclengths.size(); ++i) {
        if (animationTime <= splineArclengths[i]) {
            segmentIndex = i - 1;
            break;
        }
    }

    double t = 0.0;
    if (segmentIndex < splineArclengths.size() - 1) {
        double timeStart = splineArclengths[segmentIndex];
        double timeEnd = splineArclengths[segmentIndex + 1];
        double timeInterval = timeEnd - timeStart;

        if (timeInterval > 0) {
            t = (animationTime - timeStart) / timeInterval;
        }
    }

    int idx1 = segmentIndex;
    int idx2 = qMin(segmentIndex + 1, splinePointsX.size() - 1);

    double x = splinePointsX[idx1] + t * (splinePointsX[idx2] - splinePointsX[idx1]);
    double y = splinePointsY[idx1] + t * (splinePointsY[idx2] - splinePointsY[idx1]);

    double currentVelocity = 0.0;
    if (segmentIndex < velocities.size() - 1) {
        currentVelocity = velocities[idx1] + t * (velocities[idx2] - velocities[idx1]);
    } else if (!velocities.isEmpty()) {
        currentVelocity = velocities.last();
    }

    if (movingPoint) {
        double pointSize = 0.3;
        movingPoint->topLeft->setCoords(x - pointSize, y + pointSize);
        movingPoint->bottomRight->setCoords(x + pointSize, y - pointSize);
        movingPoint->setVisible(true);

        QCPItemText *velocityLabel = new QCPItemText(plot);
        velocityLabel->position->setCoords(x, y + 1.0);
        velocityLabel->setText(QString("v = %1").arg(currentVelocity, 0, 'f', 2));
        velocityLabel->setFont(QFont("Arial", 10));
        velocityLabel->setColor(Qt::red);

        static QCPItemText *lastLabel = nullptr;
        if (lastLabel) {
            plot->removeItem(lastLabel);
        }
        lastLabel = velocityLabel;
    }

    static QVector<double> timeHistory;
    static QVector<double> velocityHistory;

    timeHistory.append(animationTime * animationDuration);
    velocityHistory.append(currentVelocity);

    const int MAX_HISTORY = 200;
    if (timeHistory.size() > MAX_HISTORY) {
        timeHistory.removeFirst();
        velocityHistory.removeFirst();
    }

    if (velocityGraph) {
        velocityGraph->setData(timeHistory, velocityHistory);
        velocityPlot->replot();
    }

    plot->replot();
}

void Graph::runMotionSimulation() {
    if (animationTimer->isActive()) {
        animationTimer->stop();
    }

    animationTime = 0.0;

    calculateSplineForAnimation();

    QMessageBox::information(this, "Моделирование движения с гравитацией",
                             "Моделирование движения точки по траектории запущено.\n\n"
                             "Физическая модель:\n"
                             "• Движение под действием силы тяжести\n"
                             "• Нет трения и сопротивления воздуха\n"
                             "• Начальная скорость: " + QString::number(initialSpeed) + " ед/с\n"
                                                                   "• Ускорение свободного падения: " + QString::number(gravity) + " ед/с²\n\n"
                                                              "Скорость рассчитывается по закону сохранения энергии:\n"
                                                              "v = √(2g(h₀ - h) + v₀²)\n\n"
                                                              "Используйте кнопки управления для старта/паузы анимации.");

    animationTimer->start(50);
}

void Graph::plotSplineForAnimation() {
    if (splinePointsX.isEmpty() || splinePointsY.isEmpty()) return;

    QCPGraph *splineGraph = plot->addGraph();
    splineGraph->setData(splinePointsX, splinePointsY);
    splineGraph->setPen(QPen(QColor(150, 50, 200), 3));
    splineGraph->setName("Траектория движения (сплайн)");

    movingPoint = new QCPItemEllipse(plot);
    movingPoint->topLeft->setCoords(-0.2, 0.2);
    movingPoint->bottomRight->setCoords(0.2, -0.2);
    movingPoint->setPen(QPen(Qt::red, 2));
    movingPoint->setBrush(QBrush(QColor(255, 0, 0, 150)));
    movingPoint->setVisible(false);

    velocityGraph = velocityPlot->addGraph();
    velocityGraph->setPen(QPen(Qt::blue, 2));
    velocityGraph->setName("Скорость (v vs t)");

    plot->xAxis->setLabel("Координата X");
    plot->yAxis->setLabel("Координата Y");
    plot->rescaleAxes();

    plot->xAxis->scaleRange(1.1, plot->xAxis->range().center());
    plot->yAxis->scaleRange(1.1, plot->yAxis->range().center());

    plot->legend->setVisible(true);
    plot->legend->setBrush(QBrush(QColor(255,255,255,230)));
    plot->replot();
    velocityPlot->rescaleAxes();
    velocityPlot->legend->setVisible(true);
    velocityPlot->legend->setBrush(QBrush(QColor(255,255,255,230)));
    velocityPlot->replot();
}

void Graph::createGifTwoPlots(const QString& filename, int frameCount, int delay) {
    // Создаем GIF с размером, объединяющим оба графика
    int totalWidth = plot->width() + velocityPlot->width();
    int totalHeight = qMax(plot->height(), velocityPlot->height());
    QGifImage gif(QSize(totalWidth, totalHeight));
    gif.setDefaultDelay(delay);

    // Сохраняем текущее состояние анимации
    double savedAnimationTime = animationTime;
    bool wasActive = animationTimer->isActive();
    if (wasActive) {
        animationTimer->stop();
    }

    // Симулируем анимацию и захватываем кадры
    for (int i = 0; i < frameCount; ++i) {
        // Устанавливаем нормализованное время
        animationTime = static_cast<double>(i) / (frameCount - 1.0);

        // Обновляем графики
        updateAnimation();

        // Создаем изображение с обоими графиками
        QImage frame(totalWidth, totalHeight, QImage::Format_ARGB32);
        frame.fill(Qt::white);

        QPainter painter(&frame);

        // Захватываем первый график (траектория)
        QPixmap pixmap1 = plot->grab();
        painter.drawPixmap(0, 0, pixmap1);

        // Захватываем второй график (скорость)
        QPixmap pixmap2 = velocityPlot->grab();
        painter.drawPixmap(plot->width(), 0, pixmap2);

        // Добавляем подписи
        painter.setPen(Qt::black);
        QFont font = painter.font();
        font.setPointSize(12);
        font.setBold(true);
        painter.setFont(font);

        painter.drawText(20, 30, "Траектория движения");
        painter.drawText(plot->width() + 20, 30, "Скорость vs время");

        painter.end();

        // Добавляем кадр в GIF
        gif.addFrame(frame);

        // Для прогресса
        if (i % 10 == 0) {
            qDebug() << "Захвачен кадр" << i << "из" << frameCount;
        }
        QApplication::processEvents(); // Чтобы не завис UI
    }

    // Сохраняем GIF
    if (!gif.save(filename)) {
        qDebug() << "Ошибка сохранения GIF:" << filename;
        QMessageBox::warning(this, "Ошибка", "Не удалось сохранить GIF файл");
    } else {
        qDebug() << "GIF сохранен:" << filename;
        QMessageBox::information(this, "Готово",
                                 QString("Анимация сохранена в:\n%1\n\n"
                                         "Размер: %2x%3\n"
                                         "Кадров: %4\n"
                                         "FPS: %5")
                                     .arg(filename)
                                     .arg(totalWidth)
                                     .arg(totalHeight)
                                     .arg(frameCount)
                                     .arg(1000.0/delay));
    }

    // Восстанавливаем состояние
    animationTime = savedAnimationTime;
    updateAnimation();
    if (wasActive) {
        animationTimer->start(50);
    }
}

void Graph::calculateVelocityComponents()
{
    timeValues.clear();
    vxValues.clear();
    vyValues.clear();

    if (splinePointsX.size() < 2 || velocities.size() < 2 || splineArclengths.size() != splinePointsX.size())
        return;

    int n = splinePointsX.size();

    for (int i = 0; i < n; ++i)
    {
        double dx_dt = 0.0;
        double dy_dt = 0.0;
        double v_total = velocities[i];

        if (i == 0)
        {
            double dt_forward = splineArclengths[1] - splineArclengths[0];
            if (dt_forward > 0)
            {
                dx_dt = (splinePointsX[1] - splinePointsX[0]) / dt_forward;
                dy_dt = (splinePointsY[1] - splinePointsY[0]) / dt_forward;
            }
        }
        else if (i == n - 1)
        {
            double dt_backward = splineArclengths[n-1] - splineArclengths[n-2];
            if (dt_backward > 0)
            {
                dx_dt = (splinePointsX[n-1] - splinePointsX[n-2]) / dt_backward;
                dy_dt = (splinePointsY[n-1] - splinePointsY[n-2]) / dt_backward;
            }
        }
        else
        {
            double dx_forward = splinePointsX[i + 1] - splinePointsX[i];
            double dx_backward = splinePointsX[i] - splinePointsX[i - 1];
            double dy_forward = splinePointsY[i + 1] - splinePointsY[i];
            double dy_backward = splinePointsY[i] - splinePointsY[i - 1];

            double dt_forward = splineArclengths[i + 1] - splineArclengths[i];
            double dt_backward = splineArclengths[i] - splineArclengths[i - 1];

            double dx_dt_forward = (dt_forward > 0) ? dx_forward / dt_forward : 0.0;
            double dx_dt_backward = (dt_backward > 0) ? dx_backward / dt_backward : 0.0;
            double dy_dt_forward = (dt_forward > 0) ? dy_forward / dt_forward : 0.0;
            double dy_dt_backward = (dt_backward > 0) ? dy_backward / dt_backward : 0.0;

            dx_dt = (dx_dt_forward + dx_dt_backward) / 2.0;
            dy_dt = (dy_dt_forward + dy_dt_backward) / 2.0;
        }
        double norm = sqrt(dx_dt * dx_dt + dy_dt * dy_dt);

        double vx = 0.0;
        double vy = 0.0;
        if (norm > 1e-6)
        {
            vx = v_total * (dx_dt / norm);
            vy = v_total * (dy_dt / norm);
        }

        vxValues.append(vx);
        vyValues.append(vy);

        double t = splineArclengths[i] * animationDuration;
        timeValues.append(t);
    }

    for (int i = 0; i < qMin(10, timeValues.size()); ++i)
    {
        qDebug() << "t =" << timeValues[i]
                 << "vx =" << vxValues[i]
                 << "vy =" << vyValues[i];
    }
}

void Graph::plotVelocityComponents()
{
    velocityPlot->clearGraphs();

    QCPGraph *vxGraph = velocityPlot->addGraph();
    vxGraph->setData(timeValues, vxValues);
    vxGraph->setPen(QPen(QColor(255, 50, 50, 200), 1.5));
    vxGraph->setName("vₓ - горизонтальная");
    vxGraph->setLineStyle(QCPGraph::lsLine);

    vxGraph->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssCircle, QColor(255, 100, 100, 150), QColor(255, 100, 100, 150), 3));

    QCPGraph *vyGraph = velocityPlot->addGraph();
    vyGraph->setData(timeValues, vyValues);
    vyGraph->setPen(QPen(QColor(50, 100, 255, 200), 1.5));
    vyGraph->setName("vᵧ - вертикальная");
    vyGraph->setLineStyle(QCPGraph::lsLine);

    vyGraph->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssCircle, QColor(100, 150, 255, 150), QColor(100, 150, 255, 150), 3));

    QVector<double> vTotalValues;
    for (int i = 0; i < vxValues.size(); ++i) {
        double v_total = sqrt(vxValues[i] * vxValues[i] + vyValues[i] * vyValues[i]);
        vTotalValues.append(v_total);
    }

    QCPGraph *totalGraph = velocityPlot->addGraph();
    totalGraph->setData(timeValues, vTotalValues);
    totalGraph->setPen(QPen(QColor(150, 50, 200, 180), 2));
    totalGraph->setName("|v| - полная скорость");
    totalGraph->setLineStyle(QCPGraph::lsLine);
    totalGraph->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssDisc, QColor(150, 50, 200, 120), QColor(150, 50, 200, 120), 4));

    velocityPlot->xAxis->setLabel("Время, с");
    velocityPlot->yAxis->setLabel("Скорость, ед/с");

    velocityPlot->legend->setVisible(true);
    velocityPlot->legend->setBrush(QBrush(QColor(255, 255, 255, 220)));
    velocityPlot->legend->setBorderPen(QPen(Qt::gray, 1));
    velocityPlot->legend->setFont(QFont("Arial", 9));

    velocityPlot->legend->setFillOrder(QCPLegend::foRowsFirst);
    velocityPlot->axisRect()->insetLayout()->setInsetAlignment(0, Qt::AlignTop | Qt::AlignRight);

    velocityPlot->rescaleAxes();

    velocityPlot->xAxis->scaleRange(1.05, velocityPlot->xAxis->range().center());
    velocityPlot->yAxis->scaleRange(1.1, velocityPlot->yAxis->range().center());

    velocityPlot->xAxis->grid()->setVisible(true);
    velocityPlot->yAxis->grid()->setVisible(true);
    velocityPlot->xAxis->grid()->setPen(QPen(QColor(220, 220, 220), 1, Qt::DotLine));
    velocityPlot->yAxis->grid()->setPen(QPen(QColor(220, 220, 220), 1, Qt::DotLine));
    velocityPlot->xAxis->grid()->setSubGridVisible(true);
    velocityPlot->yAxis->grid()->setSubGridVisible(true);

    velocityPlot->axisRect()->setBackground(QBrush(QColor(245, 245, 245)));

    velocityPlot->xAxis->setLabelFont(QFont("Arial", 10, QFont::Bold));
    velocityPlot->yAxis->setLabelFont(QFont("Arial", 10, QFont::Bold));
    velocityPlot->xAxis->setTickLabelFont(QFont("Arial", 9));
    velocityPlot->yAxis->setTickLabelFont(QFont("Arial", 9));

    velocityPlot->replot();
}

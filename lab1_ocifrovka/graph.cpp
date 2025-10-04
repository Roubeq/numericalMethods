#include "graph.h"
#include <QFile>
#include <QVector>

using namespace std;

Graph::Graph(QWidget *parent)
    : QMainWindow(parent)
{
    setWindowTitle("Graph");
    setMinimumSize(1200, 800);
    QWidget *centralWidget = new QWidget(this);
    setCentralWidget(centralWidget);

    QVBoxLayout *layout = new QVBoxLayout(centralWidget);

    plot = new QCustomPlot();
    plot->xAxis->setLabel("Ось X");
    plot->yAxis->setLabel("Ось Y");
    plot->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom | QCP::iSelectPlottables);

    layout->addWidget(plot);
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

    plot->replot();
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

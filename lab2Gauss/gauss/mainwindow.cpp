#include "mainwindow.h"
#include <QGridLayout>
#include <QGroupBox>
#include <QHeaderView>
#include <QMessageBox>
#include <QDoubleValidator>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    setupUI();
}

MainWindow::~MainWindow() {}

void MainWindow::setupUI() {
    QWidget* centralWidget = new QWidget(this);
    setCentralWidget(centralWidget);

    QGridLayout* mainLayout = new QGridLayout(centralWidget);

    // Выбор размерности
    QGroupBox* dimensionGroup = new QGroupBox("Размерность матрицы");
    QHBoxLayout* dimensionLayout = new QHBoxLayout(dimensionGroup);

    dimensionComboBox = new QComboBox();
    for (int i = 2; i <= 6; i++) {
        dimensionComboBox->addItem(QString("%1x%1").arg(i), i);
    }
    dimensionComboBox->setCurrentIndex(1); // 3x3

    dimensionLayout->addWidget(new QLabel("Размерность:"));
    dimensionLayout->addWidget(dimensionComboBox);
    dimensionLayout->addStretch();

    // Таблицы ввода
    QGroupBox* inputGroup = new QGroupBox("Ввод системы");
    QGridLayout* inputLayout = new QGridLayout(inputGroup);

    matrixTable = new QTableWidget();
    vectorTable = new QTableWidget();
    resultTable = new QTableWidget();
    residualTable = new QTableWidget();

    matrixTable->setEditTriggers(QAbstractItemView::DoubleClicked);
    vectorTable->setEditTriggers(QAbstractItemView::DoubleClicked);
    resultTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
    residualTable->setEditTriggers(QAbstractItemView::NoEditTriggers);

    inputLayout->addWidget(new QLabel("Матрица A:"), 0, 0);
    inputLayout->addWidget(matrixTable, 1, 0);
    inputLayout->addWidget(new QLabel("Вектор b:"), 0, 1);
    inputLayout->addWidget(vectorTable, 1, 1);
    inputLayout->addWidget(new QLabel("Решение x:"), 2, 0);
    inputLayout->addWidget(resultTable, 3, 0);
    inputLayout->addWidget(new QLabel("Невязка:"), 2, 1);
    inputLayout->addWidget(residualTable, 3, 1);

    // Кнопки управления
    QGroupBox* buttonGroup = new QGroupBox("Управление");
    QHBoxLayout* buttonLayout = new QHBoxLayout(buttonGroup);

    solveButton = new QPushButton("Решить систему");
    testButton = new QPushButton("Тест №3");
    clearButton = new QPushButton("Очистить");
    normLabel = new QLabel("Норма невязки: ");
    statusLabel = new QLabel("Готово");

    buttonLayout->addWidget(solveButton);
    buttonLayout->addWidget(testButton);
    buttonLayout->addWidget(clearButton);
    buttonLayout->addWidget(normLabel);
    buttonLayout->addWidget(statusLabel);
    buttonLayout->addStretch();

    // Основная компоновка
    mainLayout->addWidget(dimensionGroup, 0, 0);
    mainLayout->addWidget(inputGroup, 1, 0);
    mainLayout->addWidget(buttonGroup, 2, 0);

    setupMatrixTable(3);

    connect(dimensionComboBox, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &MainWindow::onDimensionChanged);
    connect(solveButton, &QPushButton::clicked, this, &MainWindow::onSolveClicked);
    connect(testButton, &QPushButton::clicked, this, &MainWindow::onLoadTestClicked);
    connect(clearButton, &QPushButton::clicked, this, &MainWindow::onClearClicked);

    setWindowTitle("Метод Гаусса с выбором главного элемента");
    resize(900, 600);
}

void MainWindow::setupMatrixTable(int dimension) {
    matrixTable->setRowCount(dimension);
    matrixTable->setColumnCount(dimension);
    vectorTable->setRowCount(dimension);
    vectorTable->setColumnCount(1);
    resultTable->setRowCount(dimension);
    resultTable->setColumnCount(1);
    residualTable->setRowCount(dimension);
    residualTable->setColumnCount(1);

    QStringList headers;
    for (int i = 0; i < dimension; i++) {
        headers << QString("x%1").arg(i + 1);
    }
    matrixTable->setHorizontalHeaderLabels(headers);
    matrixTable->setVerticalHeaderLabels(headers);

    vectorTable->setHorizontalHeaderLabels(QStringList() << "b");
    vectorTable->setVerticalHeaderLabels(headers);

    resultTable->setHorizontalHeaderLabels(QStringList() << "x");
    resultTable->setVerticalHeaderLabels(headers);

    residualTable->setHorizontalHeaderLabels(QStringList() << "F");
    residualTable->setVerticalHeaderLabels(headers);

    // Заполняем нулями для удобства
    for (int i = 0; i < dimension; i++) {
        for (int j = 0; j < dimension; j++) {
            if (!matrixTable->item(i, j)) {
                QTableWidgetItem* item = new QTableWidgetItem("0");
                matrixTable->setItem(i, j, item);
            }
        }
        if (!vectorTable->item(i, 0)) {
            QTableWidgetItem* item = new QTableWidgetItem("0");
            vectorTable->setItem(i, 0, item);
        }
    }
}

void MainWindow::onDimensionChanged(int index) {
    int size = dimensionComboBox->currentData().toInt();
    setupMatrixTable(size);
    statusLabel->setText("Размерность изменена");
}

bool MainWindow::readMatrixFromTable(std::vector<std::vector<double>>& A, std::vector<double>& b) {
    int dimension = dimensionComboBox->currentData().toInt();
    A.assign(dimension, std::vector<double>(dimension, 0.0));
    b.assign(dimension, 0.0);

    for (int i = 0; i < dimension; i++) {
        for (int j = 0; j < dimension; j++) {
            QTableWidgetItem* item = matrixTable->item(i, j);
            if (!item || item->text().isEmpty()) {
                QMessageBox::warning(this, "Ошибка",
                                     QString("Заполните элемент A[%1][%2]!").arg(i+1).arg(j+1));
                return false;
            }

            bool ok;
            double value = item->text().replace(',', '.').toDouble(&ok);
            if (!ok) {
                QMessageBox::warning(this, "Ошибка",
                                     QString("Некорректное число в A[%1][%2]!").arg(i+1).arg(j+1));
                return false;
            }
            A[i][j] = value;
        }

        QTableWidgetItem* bItem = vectorTable->item(i, 0);
        if (!bItem || bItem->text().isEmpty()) {
            QMessageBox::warning(this, "Ошибка",
                                 QString("Заполните элемент b[%1]!").arg(i+1));
            return false;
        }

        bool ok;
        double bValue = bItem->text().replace(',', '.').toDouble(&ok);
        if (!ok) {
            QMessageBox::warning(this, "Ошибка",
                                 QString("Некорректное число в b[%1]!").arg(i+1));
            return false;
        }
        b[i] = bValue;
    }

    return true;
}

void MainWindow::onSolveClicked() {
    std::vector<std::vector<double>> A;
    std::vector<double> b;

    if (!readMatrixFromTable(A, b)) {
        return;
    }

    std::vector<std::vector<double>> A_original = A;
    std::vector<double> b_original = b;
    std::vector<double> x;
    std::string errorMessage;

    statusLabel->setText("Вычисление...");

    if (solver.solve(A, b, x, errorMessage)) {
        // Вывод решения
        for (int i = 0; i < x.size(); i++) {
            QTableWidgetItem* item = new QTableWidgetItem(QString::number(x[i], 'f', 6));
            resultTable->setItem(i, 0, item);
        }

        // Вычисление невязки
        std::vector<double> residual = solver.calculateResidual(A_original, b_original, x);
        double norm = solver.calculateResidualNorm(residual);

        // Вывод невязки
        for (int i = 0; i < residual.size(); i++) {
            QTableWidgetItem* item = new QTableWidgetItem(QString::number(residual[i], 'e', 2));
            residualTable->setItem(i, 0, item);
        }

        normLabel->setText(QString("Норма невязки: %1").arg(norm, 0, 'e', 2));
        statusLabel->setText("Решение найдено!");

    } else {
        QMessageBox::critical(this, "Ошибка", QString::fromStdString(errorMessage));
        statusLabel->setText("Ошибка решения!");
    }
}

void MainWindow::onLoadTestClicked() {
    // Тест №3 из задания
    int dimension = 3;
    dimensionComboBox->setCurrentIndex(1);

    std::vector<std::vector<double>> testMatrix = {
        {2.30, 5.70, -0.80},
        {3.50, -2.70, 5.30},
        {1.70, 2.30, -1.80}
    };

    std::vector<double> testVector = {-6.49, 19.20, -5.09};

    for (int i = 0; i < dimension; i++) {
        for (int j = 0; j < dimension; j++) {
            QTableWidgetItem* item = new QTableWidgetItem(QString::number(testMatrix[i][j]));
            matrixTable->setItem(i, j, item);
        }
        QTableWidgetItem* vecItem = new QTableWidgetItem(QString::number(testVector[i]));
        vectorTable->setItem(i, 0, vecItem);
    }

    statusLabel->setText("Тест №3 загружен");
}

void MainWindow::onClearClicked() {
    int dimension = dimensionComboBox->currentData().toInt();

    for (int i = 0; i < dimension; i++) {
        for (int j = 0; j < dimension; j++) {
            QTableWidgetItem* item = new QTableWidgetItem("0");
            matrixTable->setItem(i, j, item);
        }
        QTableWidgetItem* vecItem = new QTableWidgetItem("0");
        vectorTable->setItem(i, 0, vecItem);

        resultTable->setItem(i, 0, new QTableWidgetItem(""));
        residualTable->setItem(i, 0, new QTableWidgetItem(""));
    }

    normLabel->setText("Норма невязки: ");
    statusLabel->setText("Таблицы очищены");
}

#include "mainwindow.h"
#include <QMessageBox>
#include <cmath>
#include <iomanip>
#include <sstream>

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent)
{
    auto* central = new QWidget;
    auto* mainLayout = new QVBoxLayout(central);
    auto* top = new QHBoxLayout;
    top->addWidget(new QLabel("Размер матрицы n:"));
    spinN = new QSpinBox;
    spinN->setRange(1, 500);
    spinN->setValue(4);
    top->addWidget(spinN);

    btnCalc = new QPushButton("Вычислить");
    connect(btnCalc, &QPushButton::clicked, this, &MainWindow::calculate);
    top->addWidget(btnCalc);
    top->addStretch();

    mainLayout->addLayout(top);

    auto* groupMatrix = new QGroupBox("Матрица A (трёхдиагональная)");
    auto* layMatrix = new QVBoxLayout;
    tableMatrix = new QTableWidget;
    tableMatrix->setEditTriggers(QTableWidget::NoEditTriggers);
    layMatrix->addWidget(tableMatrix);
    groupMatrix->setLayout(layMatrix);
    mainLayout->addWidget(groupMatrix);

    auto* groupRes = new QGroupBox("Результаты");
    auto* layRes = new QVBoxLayout;
    textResults = new QTextEdit;
    textResults->setReadOnly(true);
    textResults->setFont(QFont("Consolas", 10));
    layRes->addWidget(textResults);
    groupRes->setLayout(layRes);
    mainLayout->addWidget(groupRes);

    setCentralWidget(central);
}

void MainWindow::calculate()
{
    int n = spinN->value();
    if (n < 1) return;

    textResults->clear();
    QVector<QVector<double>> A(n, QVector<double>(n, 0.0));
    for (int i = 0; i < n; ++i) {
        A[i][i] = 2.0;
        if (i > 0) A[i][i-1] = -1.0;
        if (i < n-1) A[i][i+1] = -1.0;
    }

    tableMatrix->setRowCount(n);
    tableMatrix->setColumnCount(n);
    for (int i = 0; i < n; ++i) {
        for (int j = 0; j < n; ++j) {
            tableMatrix->setItem(i, j, new QTableWidgetItem(QString::number(A[i][j], 'f', 1)));
        }
    }

    // степенной метод
    auto [lambdaMax, vecMax] = powerMethod(n);

    // метод обратных итераций
    auto [lambdaMin, vecMin] = inversePowerMethod(n);

    QString exactStr = "\nТочные собственные значения:\n";
    if (n <= 5) {
        for (int k = 1; k <= n; ++k) {
            double mu = 2.0 - 2.0 * std::cos(k * M_PI / (n + 1));
            exactStr += QString("λ%1 = %2\n").arg(k).arg(mu, 0, 'f', 10);
        }
    } else {
        exactStr += "(для n > 5 формула остаётся той же, но не выводим все)\n";
    }

    std::stringstream ss;
    ss << std::fixed << std::setprecision(10);

    textResults->append("=== РЕЗУЛЬТАТЫ ===\n");
    textResults->append(QString("n = %1\n").arg(n));
    textResults->append("──────────────────────\n");
    textResults->append(QString("Степенной метод (наибольшее СЗ):\nλ_max = %1\n").arg(lambdaMax, 0, 'f', 10));
    textResults->append("Вектор:\n");
    for (int i = 0; i < std::min(8, n); ++i) {
        textResults->append(QString("v[%1] = %2\n").arg(i).arg(vecMax[i], 0, 'f', 8));
    }

    textResults->append("\nМетод обратных итераций (наименьшее СЗ):\n");
    textResults->append(QString("λ_min = %1\n").arg(lambdaMin, 0, 'f', 10));
    textResults->append("Вектор:\n");
    for (int i = 0; i < std::min(8, n); ++i) {
        textResults->append(QString("v[%1] = %2\n").arg(i).arg(vecMin[i], 0, 'f', 8));
    }

    textResults->append(exactStr);
}

QVector<double> MainWindow::matVec(const QVector<QVector<double>>& A, const QVector<double>& x)
{
    int n = A.size();
    QVector<double> y(n, 0.0);
    for (int i = 0; i < n; ++i)
        for (int j = 0; j < n; ++j)
            y[i] += A[i][j] * x[j];
    return y;
}

double MainWindow::norm(const QVector<double>& v)
{
    double s = 0.0;
    for (double val : v) s += val * val;
    return std::sqrt(s);
}

double MainWindow::dot(const QVector<double>& a, const QVector<double>& b)
{
    double s = 0.0;
    for (int i = 0; i < a.size(); ++i) s += a[i] * b[i];
    return s;
}

QVector<double> MainWindow::solveA(const QVector<double>& rhs)
{
    int n = rhs.size();
    QVector<double> cp(n > 1 ? n-1 : 0, 0.0);
    QVector<double> dp(n, 0.0);
    QVector<double> x(n);

    dp[0] = rhs[0] / 2.0;
    if (n > 1) cp[0] = -1.0 / 2.0;

    for (int i = 1; i < n; ++i) {
        double denom = 2.0 - (-1.0) * cp[i-1];
        dp[i] = (rhs[i] - (-1.0) * dp[i-1]) / denom;
        if (i < n-1) cp[i] = -1.0 / denom;
    }

    x[n-1] = dp[n-1];
    for (int i = n-2; i >= 0; --i)
        x[i] = dp[i] - cp[i] * x[i+1];

    return x;
}

std::pair<double, QVector<double>> MainWindow::powerMethod(int n)
{
    QVector<double> x(n); // 1 2 3 4 ...
    for(int i = 0; i < n; i++) x[i] = i + 1;

    double nx = norm(x);
    for (auto& v : x) v /= nx;

    double lambda = 0.0;
    QVector<QVector<double>> A(n, QVector<double>(n, 0.0));
    for (int i = 0; i < n; ++i) {
        A[i][i] = 2.0;
        if (i > 0) A[i][i-1] = -1.0;
        if (i < n-1) A[i][i+1] = -1.0;
    }

    textResults->append("Степенной метод — начало\n");
    textResults->append("Итерация | λ (Релея) |");

    for (int iter = 0; iter < 300; ++iter) {
        auto y = matVec(A, x);
        double new_lambda = dot(x, y);
        double ny = norm(y);

        QString line = QString("%1 | %2 |")
                           .arg(iter, 8)
                           .arg(new_lambda, 12, 'f', 8);
        textResults->append(line);

        for (auto& v : y) v /= ny;
        x = y;

        if (iter > 5 && std::abs(new_lambda - lambda) < 1e-10) {
            textResults->append(QString("Сходимость достигнута на итерации %1").arg(iter));
            break;
        }
        lambda = new_lambda;
    }

    textResults->append("Степенной метод — конец\n");
    return {lambda, x};
}

std::pair<double, QVector<double>> MainWindow::inversePowerMethod(int n)
{
    QVector<double> x(n, 1.0);
    double nx = norm(x);
    for (auto& v : x) v /= nx;

    double lambda = 0.0;

    textResults->append("\nОбратные итерации — начало\n");
    textResults->append("Итерация | μ (1/λ)     | λ           |\n");

    for (int iter = 0; iter < 300; ++iter) {
        auto z = solveA(x);
        double mu = dot(x, z);           // ≈ xᵀ A⁻¹ x
        double new_lambda = 1.0 / mu;

        double nz = norm(z);

        QString line = QString("%1 | %2 | %3 |")
                           .arg(iter, 8)
                           .arg(mu, 12, 'f', 8)
                           .arg(new_lambda, 12, 'f', 8);
        textResults->append(line);

        for (auto& v : z) v /= nz;
        x = z;

        if (iter > 5 && std::abs(new_lambda - lambda) < 1e-10) {
            textResults->append(QString("Сходимость достигнута на итерации %1").arg(iter));
            break;
        }
        lambda = new_lambda;


    }

    textResults->append("Обратные итерации — конец\n");
    return {lambda, x};
}

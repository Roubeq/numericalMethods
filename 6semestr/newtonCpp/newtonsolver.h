#ifndef NEWTONSOLVER_H
#define NEWTONSOLVER_H

#include <QVector>
#include <QPair>

class NewtonSolver
{
public:
    NewtonSolver();

    void setInitialGuess(const double x0[2]);
    void setTolerances(double e1, double e2);
    void setMaxIterations(int maxIter);

    bool solve();
    const double* getSolution() const { return x; }
    const QVector<QPair<double, double>>& getHistory() const { return history; }

private:
    static const int n = 2;                  // размерность системы
    double x[n];                             // текущее приближение
    double x_new[n];                         // новое приближение
    double e1, e2;                           // точности
    int maxIter;                             // макс. число итераций
    QVector<QPair<double, double>> history;  // история (d1, d2) на каждой итерации

    // вычисление вектор-функции и якоби
    void computeF(const double x[n], double F[n]);
    void computeJ(const double x[n], double J[n][n]);

    // решение слау
    bool solveLinearSystem(double J[n][n], const double F[n], double dx[n]);

    // вычисление d1 и d2
    double computeD1(const double F[n]);
    double computeD2(const double x_new[n], const double x_old[n]);
};

#endif // NEWTONSOLVER_H

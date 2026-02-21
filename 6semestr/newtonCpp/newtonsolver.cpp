#include "newtonsolver.h"
#include <cmath>
#include <algorithm>

NewtonSolver::NewtonSolver()
    : e1(1e-9), e2(1e-9), maxIter(100)
{
    x[0] = x[1] = 0.0;
}

void NewtonSolver::setInitialGuess(const double x0[2])
{
    x[0] = x0[0];
    x[1] = x0[1];
}

void NewtonSolver::setTolerances(double e1_, double e2_)
{
    e1 = e1_;
    e2 = e2_;
}

void NewtonSolver::setMaxIterations(int maxIter_)
{
    maxIter = maxIter_;
}

void NewtonSolver::computeF(const double x[n], double F[n])
{
    F[0] = x[0] - x[1] - 6*log10(x[0]) - 1;
    F[1] = x[0] - 3*x[1] - 6*log10(x[1]) - 2;
}

void NewtonSolver::computeJ(const double x[n], double J[n][n])
{
    double h = 1e-8;
    double F_plus[n];
    double F_minus[n];
    double x_temp[n];

    for (int j = 0; j < n; ++j) {
        for (int i = 0; i < n; ++i) {
            x_temp[i] = x[i];
        }
        x_temp[j] = x[j] + h;
        computeF(x_temp, F_plus);

        for (int i = 0; i < n; ++i) {
            x_temp[i] = x[i];
        }
        x_temp[j] = x[j] - h;
        computeF(x_temp, F_minus);

        for (int i = 0; i < n; ++i) {
            J[i][j] = (F_plus[i] - F_minus[i]) / (2 * h);
        }
    }
}

bool NewtonSolver::solveLinearSystem(double J[n][n], const double F[n], double dx[n])
{
    double a[n][n+1];
    for (int i = 0; i < n; ++i) {
        for (int j = 0; j < n; ++j)
            a[i][j] = J[i][j];
        a[i][n] = -F[i];
    }

    for (int i = 0; i < n; ++i) {
        int maxRow = i;
        for (int k = i+1; k < n; ++k)
            if (std::fabs(a[k][i]) > std::fabs(a[maxRow][i]))
                maxRow = k;
        std::swap(a[i], a[maxRow]);

        if (std::fabs(a[i][i]) < 1e-15)
            return false;

        double div = a[i][i];
        for (int j = i; j <= n; ++j)
            a[i][j] /= div;

        for (int k = i+1; k < n; ++k) {
            double factor = a[k][i];
            for (int j = i; j <= n; ++j)
                a[k][j] -= factor * a[i][j];
        }
    }

    dx[n-1] = a[n-1][n];
    for (int i = n-2; i >= 0; --i) {
        dx[i] = a[i][n];
        for (int j = i+1; j < n; ++j)
            dx[i] -= a[i][j] * dx[j];
    }
    return true;
}

double NewtonSolver::computeD1(const double F[n])
{
    double max = std::fabs(F[0]);
    for (int i = 1; i < n; ++i)
        if (std::fabs(F[i]) > max) max = std::fabs(F[i]);
    return max;
}

double NewtonSolver::computeD2(const double x_new[n], const double x_old[n])
{
    double max = 0.0;
    for (int i = 0; i < n; ++i) {
        double diff = std::fabs(x_new[i] - x_old[i]);
        if (std::fabs(x_new[i]) >= 1.0)
            diff /= std::fabs(x_new[i]);
        if (diff > max) max = diff;
    }
    return max;
}

bool NewtonSolver::solve()
{
    history.clear();
    int iter = 0;
    double F[n];
    double J[n][n];
    double dx[n];

    while (iter < maxIter) {
        computeF(x, F);
        computeJ(x, J);

        if (!solveLinearSystem(J, F, dx))
            return false;

        for (int i = 0; i < n; ++i)
            x_new[i] = x[i] + dx[i];

        double F_new[n];
        computeF(x_new, F_new);
        double d1_new = computeD1(F_new);
        double d2 = computeD2(x_new, x);

        history.append(qMakePair(d1_new, d2));

        if (d1_new <= e1 && d2 <= e2) {
            for (int i = 0; i < n; ++i)
                x[i] = x_new[i];
            return true;
        }

        for (int i = 0; i < n; ++i)
            x[i] = x_new[i];

        iter++;
    }
    return false;
}

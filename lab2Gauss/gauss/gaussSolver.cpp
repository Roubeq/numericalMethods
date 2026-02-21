#include "gaussSolver.h"
#include <cmath>
#include <algorithm>

GaussSolver::GaussSolver() : sizeMatrix(0) {}

bool GaussSolver::validateInput(const std::vector<std::vector<double>>& A,
                                const std::vector<double>& b,
                                std::string& errorMessage) {
    int n = A.size();

    if (n == 0) {
        errorMessage = "Матрица не может быть пустой!";
        return false;
    }

    if (n > 20) {
        errorMessage = "Слишком большая размерность (максимум 20)!";
        return false;
    }

    for (int i = 0; i < n; i++) {
        if (A[i].size() != n) {
            errorMessage = "Матрица должна быть квадратной!";
            return false;
        }
    }

    if (b.size() != n) {
        errorMessage = "Размер вектора b должен совпадать с размером матрицы!";
        return false;
    }

    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            if (std::isnan(A[i][j]) || std::isinf(A[i][j])) {
                errorMessage = "Матрица содержит некорректные значения!";
                return false;
            }
        }
        if (std::isnan(b[i]) || std::isinf(b[i])) {
            errorMessage = "Вектор b содержит некорректные значения!";
            return false;
        }
    }

    errorMessage = "OK";
    return true;
}

int GaussSolver::findPivot(const std::vector<std::vector<double>> &A,
                           const std::vector<int>& IOR, int k) {
    int n = A.size();
    double maxVal = 0.0;
    int pivotIndex = -1;

    for (int i = k; i < n; i++) {
        int row = IOR[i];
        double currentVal = std::abs(A[row][k]);

        if (currentVal > maxVal) {
            maxVal = currentVal;
            pivotIndex = i;
        }
    }

    return (maxVal < 1e-15) ? -1 : pivotIndex;
}

bool GaussSolver::solve(std::vector<std::vector<double>> A,
                        std::vector<double> b,
                        std::vector<double> &x,
                        std::string& errorMessage) {

    if (!validateInput(A, b, errorMessage)) {
        return false;
    }

    int n = A.size();
    sizeMatrix = n;
    x.resize(n, 0.0);

    std::vector<int> IOR(n);
    for(int i = 0; i < n; i++) {
        IOR[i] = i;
    }

    for (int k = 0; k < n - 1; k++) {
        int p = findPivot(A, IOR, k);
        if (p == -1) {
            errorMessage = "Матрица вырождена!";
            return false;
        }

        if (p != k) {
            std::swap(IOR[k], IOR[p]);
        }

        int M = IOR[k];
        double AMAIN = A[M][k];

        if (std::abs(AMAIN) < 1e-15) {
            errorMessage = "Нулевой ведущий элемент!";
            return false;
        }

        for(int j = k; j < n; j++) {
            A[M][j] /= AMAIN;
        }
        b[M] /= AMAIN;

        for(int i = k + 1; i < n; i++) {
            int l = IOR[i];
            double factor = A[l][k];

            for(int j = k + 1; j < n; j++) {
                A[l][j] -= factor * A[M][j];
            }
            b[l] -= factor * b[M];
        }
    }

    int lastRow = IOR[n-1];
    if (std::abs(A[lastRow][n-1]) < 1e-15) {
        errorMessage = "Система не имеет решения!";
        return false;
    }

    x[n-1] = b[lastRow] / A[lastRow][n-1];

    for (int k = n-2; k >= 0; k--) {
        int row = IOR[k];
        double sum = 0.0;

        for(int j = k + 1; j < n; j++) {
            sum += A[row][j] * x[j];
        }
        x[k] = b[row] - sum;
    }

    errorMessage = "Решение найдено успешно!";
    return true;
}

std::vector<double> GaussSolver::calculateResidual(
    const std::vector<std::vector<double>>& A,
    const std::vector<double>& b,
    const std::vector<double>& x) {

    int n = A.size();
    std::vector<double> residual(n, 0.0);

    for (int i = 0; i < n; i++) {
        double sum = 0.0;
        for (int j = 0; j < n; j++) {
            sum += A[i][j] * x[j];
        }
        residual[i] = sum - b[i];
    }

    return residual;
}

double GaussSolver::calculateResidualNorm(const std::vector<double>& residual) {
    double norm = 0.0;
    for (double val : residual) {
        norm = std::max(norm, std::abs(val));
    }
    return norm;
}

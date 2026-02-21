#ifndef GAUSS_SOLVER_H
#define GAUSS_SOLVER_H

#include <vector>
#include <string>

class GaussSolver
{
public:
    GaussSolver();

    bool solve(std::vector<std::vector<double>> A,
               std::vector<double> b,
               std::vector<double>& x,
               std::string& errorMessage);

    bool validateInput(const std::vector<std::vector<double>>& A,
                       const std::vector<double>& b,
                       std::string& errorMessage);

    std::vector<double> calculateResidual(const std::vector<std::vector<double>>& A,
                                          const std::vector<double>& b,
                                          const std::vector<double>& x);

    double calculateResidualNorm(const std::vector<double>& residual);

    int getMatrixSize() const { return sizeMatrix; }

private:
    int sizeMatrix;

    int findPivot(const std::vector<std::vector<double>>& A,
                  const std::vector<int>& IOR, int k);
};

#endif

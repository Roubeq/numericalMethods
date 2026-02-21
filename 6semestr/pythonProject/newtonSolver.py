import copy
from math import log10, fabs


class NewtonSolver:
    def __init__(self):
        self.n = 2
        self.x = [0.0, 0.0]
        self.e1 = 1e-9
        self.e2 = 1e-9
        self.maxIter = 100
        self.history = []
        self.h = 1e-8

    def setInitialGuess(self, x0):
        self.x[0] = x0[0]
        self.x[1] = x0[1]

    def setTolerances(self, e1_, e2_):
        self.e1 = e1_
        self.e2 = e2_

    def setMaxIterations(self, maxIter_):
        self.maxIter = maxIter_

    def computeF(self, x):
        F = [0.0, 0.0]
        F[0] = x[0] - x[1] - 6 * log10(x[0]) - 1
        F[1] = x[0] - 3 * x[1] - 6 * log10(x[1]) - 2
        return F

    def computeJ(self, x):
        J = [[0.0, 0.0], [0.0, 0.0]]
        h = self.h

        for j in range(self.n):
            x_plus = copy.deepcopy(x)
            x_plus[j] = x[j] + h
            F_plus = self.computeF(x_plus)

            x_minus = copy.deepcopy(x)
            x_minus[j] = x[j] - h
            F_minus = self.computeF(x_minus)

            for i in range(self.n):
                J[i][j] = (F_plus[i] - F_minus[i]) / (2 * h)

        return J

    def solveLinearSystem(self, J, F):
        n = self.n
        a = [[0.0] * (n + 1) for _ in range(n)]
        for i in range(n):
            for j in range(n):
                a[i][j] = J[i][j]
            a[i][n] = -F[i]

        for i in range(n):
            maxRow = i
            for k in range(i + 1, n):
                if fabs(a[k][i]) > fabs(a[maxRow][i]):
                    maxRow = k
            a[i], a[maxRow] = a[maxRow], a[i]

            if fabs(a[i][i]) < 1e-15:
                return False, None

            div = a[i][i]
            for j in range(i, n + 1):
                a[i][j] /= div

            for k in range(i + 1, n):
                factor = a[k][i]
                for j in range(i, n + 1):
                    a[k][j] -= factor * a[i][j]

        dx = [0.0] * n
        dx[n - 1] = a[n - 1][n]
        for i in range(n - 2, -1, -1):
            dx[i] = a[i][n]
            for j in range(i + 1, n):
                dx[i] -= a[i][j] * dx[j]

        return True, dx

    def computeD1(self, F):
        return max(fabs(F[i]) for i in range(self.n))

    def computeD2(self, x_new, x_old):
        max_val = 0.0
        for i in range(self.n):
            diff = fabs(x_new[i] - x_old[i])
            if fabs(x_new[i]) >= 1.0:
                diff /= fabs(x_new[i])
            if diff > max_val:
                max_val = diff
        return max_val

    def getHistory(self):
        return self.history

    def getSolution(self):
        return self.x

    def solve(self):
        self.history = []
        iter_count = 0

        while iter_count < self.maxIter:
            F = self.computeF(self.x)
            J = self.computeJ(self.x)

            success, dx = self.solveLinearSystem(J, F)
            if not success:
                return False

            x_new = [self.x[i] + dx[i] for i in range(self.n)]

            F_new = self.computeF(x_new)
            d1_new = self.computeD1(F_new)
            d2 = self.computeD2(x_new, self.x)

            self.history.append((d1_new, d2))

            if d1_new <= self.e1 and d2 <= self.e2:
                self.x = x_new
                return True

            self.x = x_new
            iter_count += 1

        return False
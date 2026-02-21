package com.example.gauss

import kotlin.math.abs

class GaussSolver {
    private var sizeMatrix: Int = 0

    fun solve(A: List<List<Double>>, b: List<Double>): Result {
        val errorMessage = validateInput(A, b)
        if (errorMessage != "OK") {
            return Result.Failure(errorMessage)
        }

        val n = A.size
        sizeMatrix = n

        val matrix = A.map { it.toMutableList() }.toMutableList()
        val vector = b.toMutableList()
        val x = MutableList(n) { 0.0 }
        val IOR = MutableList(n) { it }

        for (k in 0 until n - 1) {
            val p = findPivot(matrix, IOR, k)
            if (p == -1) {
                return Result.Failure("Матрица вырождена!")
            }

            if (p != k) {
                IOR[k] = IOR[p].also { IOR[p] = IOR[k] }
            }

            val M = IOR[k]
            val AMAIN = matrix[M][k]

            if (abs(AMAIN) < 1e-15) {
                return Result.Failure("Нулевой ведущий элемент!")
            }

            for (j in k until n) {
                matrix[M][j] /= AMAIN
            }
            vector[M] /= AMAIN

            for (i in k + 1 until n) {
                val l = IOR[i]
                val factor = matrix[l][k]

                for (j in k + 1 until n) {
                    matrix[l][j] -= factor * matrix[M][j]
                }
                vector[l] -= factor * vector[M]
            }
        }

        val lastRow = IOR[n - 1]
        if (abs(matrix[lastRow][n - 1]) < 1e-15) {
            return Result.Failure("Система не имеет решения!")
        }

        x[n - 1] = vector[lastRow] / matrix[lastRow][n - 1]

        for (k in n - 2 downTo 0) {
            val row = IOR[k]
            var sum = 0.0

            for (j in k + 1 until n) {
                sum += matrix[row][j] * x[j]
            }
            x[k] = vector[row] - sum
        }

        return Result.Success(x)
    }

    private fun validateInput(A: List<List<Double>>, b: List<Double>): String {
        val n = A.size

        if (n == 0) {
            return "Матрица не может быть пустой!"
        }

        if (n > 20) {
            return "Слишком большая размерность (максимум 20)!"
        }

        for (i in 0 until n) {
            if (A[i].size != n) {
                return "Матрица должна быть квадратной!"
            }
        }

        if (b.size != n) {
            return "Размер вектора b должен совпадать с размером матрицы!"
        }

        for (i in 0 until n) {
            for (j in 0 until n) {
                if (A[i][j].isNaN() || A[i][j].isInfinite()) {
                    return "Матрица содержит некорректные значения!"
                }
            }
            if (b[i].isNaN() || b[i].isInfinite()) {
                return "Вектор b содержит некорректные значения!"
            }
        }

        return "OK"
    }

    private fun findPivot(A: List<List<Double>>, IOR: List<Int>, k: Int): Int {
        val n = A.size
        var maxVal = 0.0
        var pivotIndex = -1

        for (i in k until n) {
            val row = IOR[i]
            val currentVal = abs(A[row][k])

            if (currentVal > maxVal) {
                maxVal = currentVal
                pivotIndex = i
            }
        }

        return if (maxVal < 1e-15) -1 else pivotIndex
    }

    fun calculateResidual(A: List<List<Double>>, b: List<Double>, x: List<Double>): List<Double> {
        val n = A.size
        val residual = MutableList(n) { 0.0 }

        for (i in 0 until n) {
            var sum = 0.0
            for (j in 0 until n) {
                sum += A[i][j] * x[j]
            }
            residual[i] = sum - b[i]
        }

        return residual
    }

    fun calculateResidualNorm(residual: List<Double>): Double {
        return residual.maxOfOrNull { abs(it) } ?: 0.0
    }

    fun getMatrixSize(): Int = sizeMatrix

    sealed class Result {
        data class Success(val solution: List<Double>) : Result()
        data class Failure(val error: String) : Result()
    }
}
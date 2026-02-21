package com.example.newton

import kotlin.math.*

class NewtonSolver {
    private val n = 2
    private var x = doubleArrayOf(0.0, 0.0)
    private var e1 = 1e-9
    private var e2 = 1e-9
    private var maxIter = 100
    private val history = mutableListOf<Pair<Double, Double>>()
    private val h = 1e-8

    fun setInitialGuess(x0: DoubleArray) {
        x = x0.copyOf()
    }

    fun setTolerances(e1_: Double, e2_: Double) {
        e1 = e1_
        e2 = e2_
    }

    fun setMaxIterations(maxIter_: Int) {
        maxIter = maxIter_
    }

    fun computeF(x: DoubleArray): DoubleArray {
        return doubleArrayOf(
            x[0] - x[1] - 6 * log10(x[0]) - 1,
            x[0] - 3 * x[1] - 6 * log10(x[1]) - 2
        )
    }

    fun computeJ(x: DoubleArray): Array<DoubleArray> {
        val J = Array(n) { DoubleArray(n) }

        for (j in 0 until n) {
            val xPlus = x.copyOf()
            xPlus[j] = x[j] + h
            val FPlus = computeF(xPlus)

            val xMinus = x.copyOf()
            xMinus[j] = x[j] - h
            val FMinus = computeF(xMinus)

            for (i in 0 until n) {
                J[i][j] = (FPlus[i] - FMinus[i]) / (2 * h)
            }
        }
        return J
    }

    fun solveLinearSystem(J: Array<DoubleArray>, F: DoubleArray): Pair<Boolean, DoubleArray?> {
        val a = Array(n) { DoubleArray(n + 1) }
        for (i in 0 until n) {
            for (j in 0 until n) {
                a[i][j] = J[i][j]
            }
            a[i][n] = -F[i]
        }

        for (i in 0 until n) {
            var maxRow = i
            for (k in i + 1 until n) {
                if (abs(a[k][i]) > abs(a[maxRow][i])) {
                    maxRow = k
                }
            }
            val temp = a[i]
            a[i] = a[maxRow]
            a[maxRow] = temp

            if (abs(a[i][i]) < 1e-15) {
                return Pair(false, null)
            }

            val div = a[i][i]
            for (j in i..n) {
                a[i][j] /= div
            }

            for (k in i + 1 until n) {
                val factor = a[k][i]
                for (j in i..n) {
                    a[k][j] -= factor * a[i][j]
                }
            }
        }

        val dx = DoubleArray(n)
        dx[n - 1] = a[n - 1][n]
        for (i in n - 2 downTo 0) {
            dx[i] = a[i][n]
            for (j in i + 1 until n) {
                dx[i] -= a[i][j] * dx[j]
            }
        }

        return Pair(true, dx)
    }

    fun computeD1(F: DoubleArray): Double {
        return F.maxOf { abs(it) }
    }

    fun computeD2(xNew: DoubleArray, xOld: DoubleArray): Double {
        var maxVal = 0.0
        for (i in 0 until n) {
            var diff = abs(xNew[i] - xOld[i])
            if (abs(xNew[i]) >= 1.0) {
                diff /= abs(xNew[i])
            }
            if (diff > maxVal) {
                maxVal = diff
            }
        }
        return maxVal
    }

    fun getHistory(): List<Pair<Double, Double>> = history

    fun getSolution(): DoubleArray = x

    fun solve(): Boolean {
        history.clear()
        var iter = 0

        while (iter < maxIter) {
            val F = computeF(x)
            val J = computeJ(x)

            val (success, dx) = solveLinearSystem(J, F)
            if (!success || dx == null) {
                return false
            }

            val xNew = DoubleArray(n) { x[it] + dx[it] }
            val FNew = computeF(xNew)
            val d1New = computeD1(FNew)
            val d2 = computeD2(xNew, x)

            history.add(Pair(d1New, d2))

            if (d1New <= e1 && d2 <= e2) {
                x = xNew
                return true
            }

            x = xNew
            iter++
        }

        return false
    }
}
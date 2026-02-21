package com.example.labamnkotlin

import android.os.Bundle
import androidx.activity.ComponentActivity
import androidx.activity.compose.setContent
import androidx.compose.foundation.layout.*
import androidx.compose.material3.MaterialTheme
import androidx.compose.material3.Surface
import androidx.compose.material3.Text
import androidx.compose.runtime.Composable
import androidx.compose.ui.Modifier
import androidx.compose.ui.text.font.FontWeight
import androidx.compose.ui.unit.dp
import androidx.compose.ui.viewinterop.AndroidView
import com.github.mikephil.charting.charts.LineChart
import com.github.mikephil.charting.data.Entry
import com.github.mikephil.charting.data.LineData
import com.github.mikephil.charting.data.LineDataSet
import java.io.BufferedReader
import java.io.InputStreamReader
import kotlin.math.pow
import kotlin.math.sqrt

class MainActivity : ComponentActivity() {
    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        setContent {
            MaterialTheme {
                Surface(
                    modifier = Modifier.fillMaxSize(),
                    color = MaterialTheme.colorScheme.background
                ) {
                    MNKScreen()
                }
            }
        }
    }

    @Composable
    fun MNKScreen() {
        val inputStream = assets.open("data.txt")
        val reader = BufferedReader(InputStreamReader(inputStream))
        val firstLine = reader.readLine().split(" ")
        val n = firstLine[0].toInt()
        val m = firstLine[1].toInt()
        val x = mutableListOf<Double>()
        val y = mutableListOf<Double>()
        for (i in 0 until n) {
            val line = reader.readLine().split(" ")
            x.add(line[0].toDouble())
            y.add(line[1].toDouble())
        }
        reader.close()

        val maxPower = 2 * m
        val powerx = MutableList(maxPower + 1) { 0.0 }
        for (i in 0 until n) {
            var xp = 1.0
            for (k in 0..maxPower) {
                powerx[k] += xp
                xp *= x[i]
            }
        }

        val size = m + 1
        val sumX = MutableList(size) { MutableList(size) { 0.0 } }
        for (l in 0 until size) {
            for (j in 0 until size) {
                sumX[l][j] = powerx[l + j]
            }
        }

        val praw = MutableList(size) { 0.0 }
        for (l in 0 until size) {
            for (i in 0 until n) {
                praw[l] += y[i] * x[i].pow(l.toDouble())
            }
        }

        val a = gaussianElimination(sumX, praw)

        var residualSum = 0.0
        for (i in 0 until n) {
            var yHat = 0.0
            for (k in 0 until size) {
                yHat += a[k] * x[i].pow(k.toDouble())
            }
            residualSum += (y[i] - yHat).pow(2)
        }
        val s2 = residualSum / (n - m - 1)
        val sigma = sqrt(s2)

        Column(
            modifier = Modifier
                .fillMaxSize()
                .padding(16.dp)
        ) {
            Text(
                text = "Коэффициенты: a0=${String.format("%.6f", a[0])}, a1=${String.format("%.6f", a[1])}, a2=${String.format("%.6f", a[2])}",
                style = MaterialTheme.typography.bodyLarge,
                fontWeight = FontWeight.Bold,
                modifier = Modifier.padding(bottom = 8.dp)
            )
            Text(
                text = "s²=${String.format("%.6f", s2)}, sigma=${String.format("%.6f", sigma)}",
                style = MaterialTheme.typography.bodyLarge,
                modifier = Modifier.padding(bottom = 16.dp)
            )

            AndroidView(
                modifier = Modifier
                    .fillMaxWidth()
                    .height(400.dp),
                factory = { context ->
                    LineChart(context).apply {
                        val dataEntries = mutableListOf<Entry>()
                        for (i in 0 until n) {
                            dataEntries.add(Entry(x[i].toFloat(), y[i].toFloat()))
                        }
                        val dataSet = LineDataSet(dataEntries, "Данные")
                        dataSet.color = android.graphics.Color.BLUE
                        dataSet.setCircleColor(android.graphics.Color.BLUE)
                        dataSet.setDrawValues(false)
                        dataSet.mode = LineDataSet.Mode.LINEAR
                        dataSet.setDrawCircles(true)
                        dataSet.setDrawCircleHole(false)

                        val minX = x.minOrNull() ?: 0.0
                        val maxX = x.maxOrNull() ?: 10.0
                        val approxEntries = mutableListOf<Entry>()
                        val step = (maxX - minX) / 99.0
                        for (i in 0..99) {
                            val currX = minX + i * step
                            var yHat = 0.0
                            for (k in 0 until size) {
                                yHat += a[k] * currX.pow(k.toDouble())
                            }
                            approxEntries.add(Entry(currX.toFloat(), yHat.toFloat()))
                        }
                        val approxSet = LineDataSet(approxEntries, "Аппроксимация")
                        approxSet.color = android.graphics.Color.RED
                        approxSet.setDrawCircles(false)
                        approxSet.setDrawValues(false)

                        data = LineData(dataSet, approxSet)
                        description.text = ""
                        setTouchEnabled(true)
                        isDragEnabled = true
                        setScaleEnabled(true)
                        setPinchZoom(true)

                        xAxis.labelCount = 10
                        xAxis.setDrawGridLines(true)

                    }
                }
            )
        }
    }

    private fun gaussianElimination(A: MutableList<MutableList<Double>>, b: MutableList<Double>): List<Double> {
        val n = A.size
        for (i in 0 until n) {
            for (k in i + 1 until n) {
                val factor = A[k][i] / A[i][i]
                for (j in i until n) {
                    A[k][j] -= factor * A[i][j]
                }
                b[k] -= factor * b[i]
            }
        }
        val x = MutableList(n) { 0.0 }
        for (i in n - 1 downTo 0) {
            x[i] = b[i]
            for (j in i + 1 until n) {
                x[i] -= A[i][j] * x[j]
            }
            x[i] /= A[i][i]
        }
        return x
    }
}
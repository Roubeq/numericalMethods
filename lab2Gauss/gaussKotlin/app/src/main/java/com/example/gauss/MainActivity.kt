package com.example.gauss

import android.os.Bundle
import androidx.activity.ComponentActivity
import androidx.activity.compose.setContent
import androidx.compose.foundation.layout.*
import androidx.compose.foundation.rememberScrollState
import androidx.compose.foundation.text.KeyboardOptions
import androidx.compose.foundation.verticalScroll
import androidx.compose.material3.*
import androidx.compose.runtime.*
import androidx.compose.ui.Alignment
import androidx.compose.ui.Modifier
import androidx.compose.ui.text.font.FontWeight
import androidx.compose.ui.text.input.KeyboardType
import androidx.compose.ui.unit.dp
import androidx.compose.ui.unit.sp

class MainActivity : ComponentActivity() {
    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        setContent {
            GaussSolverApp()
        }
    }
}

@OptIn(ExperimentalMaterial3Api::class)
@Composable
fun GaussSolverApp() {
    var dimension by remember { mutableStateOf(3) }
    var matrixValues by remember { mutableStateOf(createEmptyMatrix(3)) }
    var vectorValues by remember { mutableStateOf(createEmptyVector(3)) }
    var solution by remember { mutableStateOf<List<Double>?>(null) }
    var residuals by remember { mutableStateOf<List<Double>?>(null) }
    var residualNorm by remember { mutableStateOf(0.0) }
    var status by remember { mutableStateOf("Готово") }

    val solver = remember { GaussSolver() }

    Surface(
        modifier = Modifier.fillMaxSize(),
        color = MaterialTheme.colorScheme.background
    ) {
        Column(
            modifier = Modifier
                .padding(16.dp)
                .padding(top = 48.dp)
                .verticalScroll(rememberScrollState()),
            verticalArrangement = Arrangement.spacedBy(16.dp)
        ) {
            Card(
                modifier = Modifier.fillMaxWidth()
            ) {
                Column(
                    modifier = Modifier.padding(16.dp)
                ) {
                    Text(
                        text = "Размерность матрицы",
                        fontWeight = FontWeight.Bold,
                        fontSize = 16.sp
                    )
                    Spacer(modifier = Modifier.height(8.dp))
                    Row(
                        verticalAlignment = Alignment.CenterVertically
                    ) {
                        Text("Размерность:")
                        Spacer(modifier = Modifier.width(8.dp))
                        DropdownMenuButton(
                            dimension = dimension,
                            onDimensionChanged = { newDimension ->
                                dimension = newDimension
                                matrixValues = createEmptyMatrix(newDimension)
                                vectorValues = createEmptyVector(newDimension)
                                solution = null
                                residuals = null
                                status = "Размерность изменена"
                            }
                        )
                    }
                }
            }

            Card(
                modifier = Modifier.fillMaxWidth()
            ) {
                Column(
                    modifier = Modifier.padding(16.dp)
                ) {
                    Text(
                        text = "Ввод системы",
                        fontWeight = FontWeight.Bold,
                        fontSize = 16.sp
                    )
                    Spacer(modifier = Modifier.height(8.dp))

                    Text("Матрица A:", fontWeight = FontWeight.Medium)
                    Spacer(modifier = Modifier.height(4.dp))
                    MatrixInputTable(
                        matrixValues = matrixValues,
                        onValueChanged = { row, col, value ->
                            matrixValues = matrixValues.mapIndexed { r, rowList ->
                                if (r == row) {
                                    rowList.mapIndexed { c, currentValue ->
                                        if (c == col) value else currentValue
                                    }
                                } else rowList
                            }
                        }
                    )

                    Spacer(modifier = Modifier.height(16.dp))

                    Text("Вектор b:", fontWeight = FontWeight.Medium)
                    Spacer(modifier = Modifier.height(4.dp))
                    VectorInputTable(
                        vectorValues = vectorValues,
                        onValueChanged = { index, value ->
                            vectorValues = vectorValues.mapIndexed { i, currentValue ->
                                if (i == index) value else currentValue
                            }
                        }
                    )
                }
            }

            Card(
                modifier = Modifier.fillMaxWidth()
            ) {
                Column(
                    modifier = Modifier.padding(16.dp)
                ) {
                    Text(
                        text = "Управление",
                        fontWeight = FontWeight.Bold,
                        fontSize = 16.sp
                    )
                    Spacer(modifier = Modifier.height(8.dp))

                    Row(
                        modifier = Modifier.fillMaxWidth(),
                        horizontalArrangement = Arrangement.spacedBy(8.dp)
                    ) {
                        Button(
                            onClick = {
                                status = "Вычисление..."
                                val result = solver.solve(matrixValues, vectorValues)
                                when (result) {
                                    is GaussSolver.Result.Success -> {
                                        solution = result.solution
                                        residuals = solver.calculateResidual(matrixValues, vectorValues, result.solution)
                                        residualNorm = solver.calculateResidualNorm(residuals!!)
                                        status = "Решение найдено!"
                                    }
                                    is GaussSolver.Result.Failure -> {
                                        solution = null
                                        residuals = null
                                        status = "Ошибка: ${result.error}"
                                    }
                                }
                            },
                            modifier = Modifier.weight(1f)
                        ) {
                            Text("Решить систему")
                        }

                        Button(
                            onClick = {
                                dimension = 3
                                matrixValues = listOf(
                                    listOf(2.30, 5.70, -0.80),
                                    listOf(3.50, -2.70, 5.30),
                                    listOf(1.70, 2.30, -1.80)
                                )
                                vectorValues = listOf(-6.49, 19.20, -5.09)
                                solution = null
                                residuals = null
                                status = "Тест №3 загружен"
                            },
                            modifier = Modifier.weight(1f)
                        ) {
                            Text("Тест №3")
                        }

                        Button(
                            onClick = {
                                matrixValues = createEmptyMatrix(dimension)
                                vectorValues = createEmptyVector(dimension)
                                solution = null
                                residuals = null
                                residualNorm = 0.0
                                status = "Таблицы очищены"
                            },
                            modifier = Modifier.weight(1f)
                        ) {
                            Text("Очистить")
                        }
                    }

                    Spacer(modifier = Modifier.height(8.dp))
                    Row(
                        modifier = Modifier.fillMaxWidth(),
                        horizontalArrangement = Arrangement.SpaceBetween
                    ) {
                        Text(
                            text = "Норма невязки: ${if (residuals != null) "%.2e".format(residualNorm) else ""}",
                            fontWeight = FontWeight.Medium
                        )
                        Text(
                            text = status,
                            color = when {
                                status.contains("Ошибка") -> MaterialTheme.colorScheme.error
                                status.contains("найдено") -> MaterialTheme.colorScheme.primary
                                else -> MaterialTheme.colorScheme.onSurface
                            }
                        )
                    }
                }
            }

            if (solution != null || residuals != null) {
                Card(
                    modifier = Modifier.fillMaxWidth()
                ) {
                    Column(
                        modifier = Modifier.padding(16.dp)
                    ) {
                        Text(
                            text = "Результаты",
                            fontWeight = FontWeight.Bold,
                            fontSize = 16.sp
                        )
                        Spacer(modifier = Modifier.height(8.dp))

                        Row(
                            modifier = Modifier.fillMaxWidth(),
                            horizontalArrangement = Arrangement.spacedBy(16.dp)
                        ) {
                            Column(modifier = Modifier.weight(1f)) {
                                Text("Решение x:", fontWeight = FontWeight.Medium)
                                Spacer(modifier = Modifier.height(4.dp))
                                SolutionTable(solution ?: emptyList())
                            }

                            Column(modifier = Modifier.weight(1f)) {
                                Text("Невязка:", fontWeight = FontWeight.Medium)
                                Spacer(modifier = Modifier.height(4.dp))
                                ResidualTable(residuals ?: emptyList())
                            }
                        }
                    }
                }
            }
        }
    }
}

@Composable
fun DropdownMenuButton(dimension: Int, onDimensionChanged: (Int) -> Unit) {
    var expanded by remember { mutableStateOf(false) }
    val dimensions = (2..6).toList()

    Box {
        Button(
            onClick = { expanded = true },
            colors = ButtonDefaults.buttonColors(
                containerColor = MaterialTheme.colorScheme.primaryContainer,
                contentColor = MaterialTheme.colorScheme.onPrimaryContainer
            )
        ) {
            Text("${dimension}x$dimension")
        }

        DropdownMenu(
            expanded = expanded,
            onDismissRequest = { expanded = false }
        ) {
            dimensions.forEach { size ->
                DropdownMenuItem(
                    text = { Text("${size}x$size") },
                    onClick = {
                        onDimensionChanged(size)
                        expanded = false
                    }
                )
            }
        }
    }
}

@Composable
fun MatrixInputTable(matrixValues: List<List<Double>>, onValueChanged: (Int, Int, Double) -> Unit) {
    Column {
        matrixValues.forEachIndexed { rowIndex, row ->
            Row(
                horizontalArrangement = Arrangement.spacedBy(4.dp)
            ) {
                row.forEachIndexed { colIndex, value ->
                    MatrixInputCell(
                        value = value,
                        onValueChanged = { newValue ->
                            onValueChanged(rowIndex, colIndex, newValue)
                        },
                        modifier = Modifier.weight(1f)
                    )
                }
            }
            Spacer(modifier = Modifier.height(4.dp))
        }
    }
}

@Composable
fun VectorInputTable(vectorValues: List<Double>, onValueChanged: (Int, Double) -> Unit) {
    Column {
        vectorValues.forEachIndexed { index, value ->
            Row {
                MatrixInputCell(
                    value = value,
                    onValueChanged = { newValue ->
                        onValueChanged(index, newValue)
                    },
                    modifier = Modifier.fillMaxWidth()
                )
            }
            Spacer(modifier = Modifier.height(4.dp))
        }
    }
}

@Composable
fun MatrixInputCell(
    value: Double,
    onValueChanged: (Double) -> Unit,
    modifier: Modifier = Modifier
) {
    var text by remember(value) { mutableStateOf(value.toString()) }
    var isError by remember { mutableStateOf(false) }
    LaunchedEffect(value) {
        text = value.toString()
    }
    OutlinedTextField(
        value = text,
        onValueChange = { newText ->
            text = newText
            val doubleValue = newText.toDoubleOrNull()
            if (doubleValue != null) {
                onValueChanged(doubleValue)
                isError = false
            } else {
                isError = true
            }
        },
        keyboardOptions = KeyboardOptions(keyboardType = KeyboardType.Number),
        modifier = modifier,
        singleLine = true,
        isError = isError,
        placeholder = { Text("0.0") },
    )
}

@Composable
fun SolutionTable(solution: List<Double>) {
    Column {
        solution.forEachIndexed { index, value ->
            Card(
                modifier = Modifier.fillMaxWidth(),
                colors = CardDefaults.cardColors(
                    containerColor = MaterialTheme.colorScheme.surfaceVariant
                )
            ) {
                Row(
                    modifier = Modifier.padding(8.dp),
                    horizontalArrangement = Arrangement.SpaceBetween
                ) {
                    Text("x${index + 1} ")
                    Text("%.6f".format(value))
                }
            }
            Spacer(modifier = Modifier.height(4.dp))
        }
    }
}

@Composable
fun ResidualTable(residuals: List<Double>) {
    Column {
        residuals.forEachIndexed { index, value ->
            Card(
                modifier = Modifier.fillMaxWidth(),
                colors = CardDefaults.cardColors(
                    containerColor = MaterialTheme.colorScheme.surfaceVariant
                )
            ) {
                Row(
                    modifier = Modifier.padding(8.dp),
                    horizontalArrangement = Arrangement.SpaceBetween
                ) {
                    Text("F${index + 1} ")
                    Text("%.2e".format(value))
                }
            }
            Spacer(modifier = Modifier.height(4.dp))
        }
    }
}

private fun createEmptyMatrix(size: Int): List<List<Double>> {
    return List(size) { List(size) { 0.0 } }
}

private fun createEmptyVector(size: Int): List<Double> {
    return List(size) { 0.0 }
}
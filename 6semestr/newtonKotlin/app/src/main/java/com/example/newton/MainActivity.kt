package com.example.newton

import android.os.Bundle
import androidx.activity.ComponentActivity
import androidx.activity.compose.setContent
import androidx.compose.foundation.layout.*
import androidx.compose.foundation.lazy.LazyColumn
import androidx.compose.foundation.lazy.itemsIndexed
import androidx.compose.material3.*
import androidx.compose.runtime.*
import androidx.compose.ui.Alignment
import androidx.compose.ui.Modifier
import androidx.compose.ui.platform.LocalContext
import androidx.compose.ui.unit.dp
import androidx.compose.ui.unit.sp
import com.example.newton.ui.theme.NewtonTheme
import kotlinx.coroutines.Dispatchers
import kotlinx.coroutines.launch
import kotlinx.coroutines.withContext
import java.io.BufferedReader
import java.io.InputStreamReader

class MainActivity : ComponentActivity() {
    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        setContent {
            NewtonTheme {
                Surface(
                    modifier = Modifier.fillMaxSize(),
                    color = MaterialTheme.colorScheme.background
                ) {
                    NewtonSolverApp()
                }
            }
        }
    }
}

@OptIn(ExperimentalMaterial3Api::class)
@Composable
fun NewtonSolverApp() {
    val context = LocalContext.current
    val scope = rememberCoroutineScope()

    var history by remember { mutableStateOf<List<Pair<Double, Double>>>(emptyList()) }
    var solutionText by remember { mutableStateOf("") }
    var showError by remember { mutableStateOf(false) }
    var errorMessage by remember { mutableStateOf("") }
    var isLoading by remember { mutableStateOf(false) }

    Column(
        modifier = Modifier
            .fillMaxSize()
            .padding(16.dp),
        horizontalAlignment = Alignment.CenterHorizontally
    ) {
        Text(
            text = "Метод Ньютона",
            fontSize = 24.sp,
            modifier = Modifier.padding(16.dp)
        )

        Button(
            onClick = {
                scope.launch {
                    isLoading = true
                    try {
                        withContext(Dispatchers.IO) {
                            val inputStream = context.assets.open("input.txt")
                            val reader = BufferedReader(InputStreamReader(inputStream))
                            val lines = reader.readLines()
                            reader.close()

                            if (lines.size < 2) {
                                errorMessage = "Файл должен содержать 2 строки"
                                showError = true
                                return@withContext
                            }

                            val x0 = lines[0].trim().split("\\s+".toRegex()).map { it.toDouble() }
                            val params = lines[1].trim().split("\\s+".toRegex()).map { it.toDouble() }

                            if (x0.size != 2 || params.size != 3) {
                                errorMessage = "Неверный формат файла. Нужно: x1 x2 / e1 e2 maxIter"
                                showError = true
                                return@withContext
                            }

                            val solver = NewtonSolver()
                            solver.setInitialGuess(doubleArrayOf(x0[0], x0[1]))
                            solver.setTolerances(params[0], params[1])
                            solver.setMaxIterations(params[2].toInt())

                            val success = solver.solve()
                            history = solver.getHistory()

                            if (success) {
                                val sol = solver.getSolution()
                                val F = solver.computeF(sol)
                                solutionText = """
                                    Решение:
                                    x₁ = ${sol[0].format(6)}
                                    x₂ = ${sol[1].format(6)}
                                    
                                    F₁ = ${F[0].format(3, true)}
                                    F₂ = ${F[1].format(3, true)}
                                """.trimIndent()
                            } else {
                                solutionText = "Решение не найдено"
                            }
                        }
                    } catch (e: Exception) {
                        errorMessage = "Ошибка: ${e.message}"
                        showError = true
                    } finally {
                        isLoading = false
                    }
                }
            },
            enabled = !isLoading,
            modifier = Modifier.padding(8.dp)
        ) {
            if (isLoading) {
                CircularProgressIndicator(
                    modifier = Modifier.size(24.dp),
                    color = MaterialTheme.colorScheme.onPrimary
                )
            } else {
                Text("Решить")
            }
        }

        if (history.isNotEmpty()) {
            Card(
                modifier = Modifier
                    .fillMaxWidth()
                    .weight(1f)
                    .padding(8.dp)
            ) {
                LazyColumn(
                    modifier = Modifier.fillMaxSize()
                ) {
                    item {
                        Row(
                            modifier = Modifier
                                .fillMaxWidth()
                                .padding(8.dp),
                            horizontalArrangement = Arrangement.SpaceEvenly
                        ) {
                            Text("Итер", modifier = Modifier.weight(1f))
                            Text("d₁", modifier = Modifier.weight(1f))
                            Text("d₂", modifier = Modifier.weight(1f))
                        }
                        HorizontalDivider()
                    }

                    itemsIndexed(history) { index, (d1, d2) ->
                        Row(
                            modifier = Modifier
                                .fillMaxWidth()
                                .padding(8.dp),
                            horizontalArrangement = Arrangement.SpaceEvenly
                        ) {
                            Text(index.toString(), modifier = Modifier.weight(1f))
                            Text(d1.format(3, true), modifier = Modifier.weight(1f))
                            Text(d2.format(3, true), modifier = Modifier.weight(1f))
                        }
                        HorizontalDivider()
                    }
                }
            }
        }

        if (solutionText.isNotEmpty()) {
            Card(
                modifier = Modifier
                    .fillMaxWidth()
                    .padding(8.dp)
            ) {
                Text(
                    text = solutionText,
                    modifier = Modifier.padding(16.dp),
                    fontFamily = MaterialTheme.typography.bodyLarge.fontFamily
                )
            }
        }
    }

    if (showError) {
        AlertDialog(
            onDismissRequest = { showError = false },
            title = { Text("Ошибка") },
            text = { Text(errorMessage) },
            confirmButton = {
                TextButton(onClick = { showError = false }) {
                    Text("OK")
                }
            }
        )
    }
}

fun Double.format(digits: Int, scientific: Boolean = false): String {
    return if (scientific) {
        String.format("%.${digits}e", this)
    } else {
        String.format("%.${digits}f", this)
    }
}
import sys
from PyQt5.QtWidgets import (QApplication, QMainWindow, QWidget, QVBoxLayout,
                             QPushButton, QTableWidget, QTextEdit,
                             QTableWidgetItem, QHeaderView, QMessageBox, QFileDialog)
from PyQt5.QtCore import Qt
from newtonSolver import NewtonSolver


class MainWindow(QMainWindow):
    def __init__(self):
        super().__init__()
        self.setWindowTitle("Метод Ньютона")
        self.setGeometry(100, 100, 600, 500)

        central = QWidget()
        self.setCentralWidget(central)
        layout = QVBoxLayout(central)

        self.btn = QPushButton("Выбрать файл и решить")
        self.btn.clicked.connect(self.solve)
        layout.addWidget(self.btn)

        self.table = QTableWidget()
        self.table.setColumnCount(3)
        self.table.setHorizontalHeaderLabels(["Итерация", "d1", "d2"])
        self.table.horizontalHeader().setSectionResizeMode(QHeaderView.Stretch)
        layout.addWidget(self.table)

        self.result = QTextEdit()
        self.result.setReadOnly(True)
        layout.addWidget(self.result)

    def solve(self):
        fileName, _ = QFileDialog.getOpenFileName(self, "Выберите файл", "", "*.txt")
        if not fileName:
            return

        try:
            with open(fileName, 'r') as f:
                lines = f.readlines()

            if len(lines) < 2:
                QMessageBox.critical(self, "Ошибка", "Файл должен содержать 2 строки")
                return

            x0 = list(map(float, lines[0].strip().split()))
            params = list(map(float, lines[1].strip().split()))

            if len(x0) != 2 or len(params) != 3:
                QMessageBox.critical(self, "Ошибка", "Неверный формат файла")
                return

            solver = NewtonSolver()
            solver.setInitialGuess(x0)
            solver.setTolerances(params[0], params[1])
            solver.setMaxIterations(int(params[2]))

            success = solver.solve()
            history = solver.getHistory()

            self.table.setRowCount(len(history))
            for i, (d1, d2) in enumerate(history):
                self.table.setItem(i, 0, QTableWidgetItem(str(i)))
                self.table.setItem(i, 1, QTableWidgetItem(f"{d1:.3e}"))
                self.table.setItem(i, 2, QTableWidgetItem(f"{d2:.3e}"))

            if success:
                sol = solver.getSolution()
                F = solver.computeF(sol)  # Считаем F1 и F2
                self.result.setText(
                    f"Решение:\n"
                    f"x1 = {sol[0]:.6f}\n"
                    f"x2 = {sol[1]:.6f}\n\n"
                    f"F1 = {F[0]:.3e}\n"
                    f"F2 = {F[1]:.3e}"
                )
            else:
                self.result.setText("Решение не найдено")

        except Exception as e:
            QMessageBox.critical(self, "Ошибка", str(e))


if __name__ == "__main__":
    app = QApplication(sys.argv)
    window = MainWindow()
    window.show()
    sys.exit(app.exec_())
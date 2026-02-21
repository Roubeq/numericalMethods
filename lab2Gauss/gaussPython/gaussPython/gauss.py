import tkinter as tk
from tkinter import ttk, messagebox
import numpy as np
from typing import List, Tuple, Optional

class GaussSolver:
    def __init__(self):
        self.size_matrix = 0

    def validate_input(self, A: List[List[float]], b: List[float]) -> Tuple[bool, str]:
        n = len(A)

        if n == 0:
            return False, "Матрица не может быть пустой!"

        if n > 20:
            return False, "Слишком большая размерность (максимум 20)!"

        for i in range(n):
            if len(A[i]) != n:
                return False, "Матрица должна быть квадратной!"

        if len(b) != n:
            return False, "Размер вектора b должен совпадать с размером матрицы!"

        for i in range(n):
            for j in range(n):
                if np.isnan(A[i][j]) or np.isinf(A[i][j]):
                    return False, "Матрица содержит некорректные значения!"
            if np.isnan(b[i]) or np.isinf(b[i]):
                return False, "Вектор b содержит некорректные значения!"

        return True, "OK"

    def find_pivot(self, A: List[List[float]], IOR: List[int], k: int) -> int:
        n = len(A)
        max_val = 0.0
        pivot_index = -1

        for i in range(k, n):
            row = IOR[i]
            current_val = abs(A[row][k])

            if current_val > max_val:
                max_val = current_val
                pivot_index = i

        return -1 if max_val < 1e-15 else pivot_index

    def solve(self, A: List[List[float]], b: List[float]) -> Tuple[bool, List[float], str]:
        is_valid, error_msg = self.validate_input(A, b)
        if not is_valid:
            return False, [], error_msg

        n = len(A)
        self.size_matrix = n

        # Создаем копии для работы
        matrix = [row[:] for row in A]
        vector = b[:]
        x = [0.0] * n
        IOR = list(range(n))

        for k in range(n - 1):
            p = self.find_pivot(matrix, IOR, k)
            if p == -1:
                return False, [], "Матрица вырождена!"

            if p != k:
                IOR[k], IOR[p] = IOR[p], IOR[k]

            M = IOR[k]
            AMAIN = matrix[M][k]

            if abs(AMAIN) < 1e-15:
                return False, [], "Нулевой ведущий элемент!"

            # Нормализация строки
            for j in range(k, n):
                matrix[M][j] /= AMAIN
            vector[M] /= AMAIN

            # Исключение переменной
            for i in range(k + 1, n):
                l = IOR[i]
                factor = matrix[l][k]

                for j in range(k + 1, n):
                    matrix[l][j] -= factor * matrix[M][j]
                vector[l] -= factor * vector[M]

        last_row = IOR[n - 1]
        if abs(matrix[last_row][n - 1]) < 1e-15:
            return False, [], "Система не имеет решения!"

        # Обратная подстановка
        x[n - 1] = vector[last_row] / matrix[last_row][n - 1]

        for k in range(n - 2, -1, -1):
            row = IOR[k]
            sum_val = 0.0

            for j in range(k + 1, n):
                sum_val += matrix[row][j] * x[j]
            x[k] = vector[row] - sum_val

        return True, x, "Решение найдено успешно!"

    def calculate_residual(self, A: List[List[float]], b: List[float], x: List[float]) -> List[float]:
        n = len(A)
        residual = [0.0] * n

        for i in range(n):
            sum_val = 0.0
            for j in range(n):
                sum_val += A[i][j] * x[j]
            residual[i] = sum_val - b[i]

        return residual

    def calculate_residual_norm(self, residual: List[float]) -> float:
        return max(abs(val) for val in residual) if residual else 0.0


class GaussSolverApp:
    def __init__(self, root):
        self.root = root
        self.root.title("Метод Гаусса с выбором главного элемента")
        self.root.geometry("900x700")

        self.solver = GaussSolver()
        self.dimension = tk.IntVar(value=3)
        self.matrix_entries = []
        self.vector_entries = []
        self.solution_labels = []
        self.residual_labels = []

        self.setup_ui()
        self.create_matrix_input(3)

    def setup_ui(self):
        # Основной фрейм
        main_frame = ttk.Frame(self.root, padding="10")
        main_frame.grid(row=0, column=0, sticky=(tk.W, tk.E, tk.N, tk.S))

        # Настройка весов для растягивания
        self.root.columnconfigure(0, weight=1)
        self.root.rowconfigure(0, weight=1)
        main_frame.columnconfigure(1, weight=1)

        # Заголовок
        title_label = ttk.Label(main_frame,
                                text="Метод Гаусса с выбором главного элемента",
                                font=('Arial', 14, 'bold'))
        title_label.grid(row=0, column=0, columnspan=3, pady=(0, 20))

        # Выбор размерности
        dimension_frame = ttk.LabelFrame(main_frame, text="Размерность матрицы", padding="10")
        dimension_frame.grid(row=1, column=0, columnspan=3, sticky=(tk.W, tk.E), pady=(0, 10))

        ttk.Label(dimension_frame, text="Размерность:").grid(row=0, column=0, padx=(0, 10))

        dimension_combo = ttk.Combobox(dimension_frame,
                                       textvariable=self.dimension,
                                       values=[2, 3, 4, 5, 6],
                                       state="readonly",
                                       width=10)
        dimension_combo.grid(row=0, column=1)
        dimension_combo.bind('<<ComboboxSelected>>', self.on_dimension_changed)

        # Фрейм для ввода данных
        input_frame = ttk.LabelFrame(main_frame, text="Ввод системы", padding="10")
        input_frame.grid(row=2, column=0, columnspan=3, sticky=(tk.W, tk.E, tk.N, tk.S), pady=(0, 10))
        input_frame.columnconfigure(1, weight=1)

        # Матрица A
        ttk.Label(input_frame, text="Матрица A:", font=('Arial', 10, 'bold')).grid(
            row=0, column=0, sticky=tk.W, pady=(0, 5))

        self.matrix_frame = ttk.Frame(input_frame)
        self.matrix_frame.grid(row=1, column=0, columnspan=2, sticky=(tk.W, tk.E), pady=(0, 10))

        # Вектор b
        ttk.Label(input_frame, text="Вектор b:", font=('Arial', 10, 'bold')).grid(
            row=2, column=0, sticky=tk.W, pady=(5, 5))

        self.vector_frame = ttk.Frame(input_frame)
        self.vector_frame.grid(row=3, column=0, columnspan=2, sticky=(tk.W, tk.E), pady=(0, 10))

        # Кнопки управления
        button_frame = ttk.LabelFrame(main_frame, text="Управление", padding="10")
        button_frame.grid(row=3, column=0, columnspan=3, sticky=(tk.W, tk.E), pady=(0, 10))

        ttk.Button(button_frame, text="Решить систему",
                   command=self.solve_system).grid(row=0, column=0, padx=(0, 10))
        ttk.Button(button_frame, text="Тест №3",
                   command=self.load_test).grid(row=0, column=1, padx=(0, 10))
        ttk.Button(button_frame, text="Очистить",
                   command=self.clear_all).grid(row=0, column=2, padx=(0, 10))

        self.norm_label = ttk.Label(button_frame, text="Норма невязки: ")
        self.norm_label.grid(row=0, column=3, padx=(20, 10))

        self.status_label = ttk.Label(button_frame, text="Готово")
        self.status_label.grid(row=0, column=4, padx=(10, 0))

        # Результаты
        results_frame = ttk.LabelFrame(main_frame, text="Результаты", padding="10")
        results_frame.grid(row=4, column=0, columnspan=3, sticky=(tk.W, tk.E, tk.N, tk.S), pady=(0, 10))
        results_frame.columnconfigure(0, weight=1)
        results_frame.columnconfigure(1, weight=1)

        # Решение
        ttk.Label(results_frame, text="Решение x:", font=('Arial', 10, 'bold')).grid(
            row=0, column=0, sticky=tk.W, pady=(0, 5))

        self.solution_frame = ttk.Frame(results_frame)
        self.solution_frame.grid(row=1, column=0, sticky=(tk.W, tk.E, tk.N, tk.S), padx=(0, 10))

        # Невязка
        ttk.Label(results_frame, text="Невязка:", font=('Arial', 10, 'bold')).grid(
            row=0, column=1, sticky=tk.W, pady=(0, 5))

        self.residual_frame = ttk.Frame(results_frame)
        self.residual_frame.grid(row=1, column=1, sticky=(tk.W, tk.E, tk.N, tk.S))

        # Настройка растягивания
        main_frame.rowconfigure(2, weight=1)
        main_frame.rowconfigure(4, weight=1)

    def create_matrix_input(self, dimension):
        # Очищаем предыдущие поля
        for widget in self.matrix_frame.winfo_children():
            widget.destroy()
        for widget in self.vector_frame.winfo_children():
            widget.destroy()

        self.matrix_entries = []
        self.vector_entries = []

        # Создаем матрицу
        for i in range(dimension):
            row_entries = []
            for j in range(dimension):
                entry = ttk.Entry(self.matrix_frame, width=8, justify='center')
                entry.insert(0, "0.0")
                entry.grid(row=i, column=j, padx=2, pady=2)
                row_entries.append(entry)
            self.matrix_entries.append(row_entries)

        # Создаем вектор
        for i in range(dimension):
            entry = ttk.Entry(self.vector_frame, width=8, justify='center')
            entry.insert(0, "0.0")
            entry.grid(row=0, column=i, padx=2, pady=2)
            self.vector_entries.append(entry)

    def on_dimension_changed(self, event):
        new_dimension = self.dimension.get()
        self.create_matrix_input(new_dimension)
        self.clear_results()
        self.status_label.config(text="Размерность изменена")

    def get_input_values(self):
        try:
            dimension = self.dimension.get()
            A = []
            b = []

            # Читаем матрицу
            for i in range(dimension):
                row = []
                for j in range(dimension):
                    value = self.matrix_entries[i][j].get().replace(',', '.')
                    row.append(float(value))
                A.append(row)

            # Читаем вектор
            for i in range(dimension):
                value = self.vector_entries[i].get().replace(',', '.')
                b.append(float(value))

            return True, A, b

        except ValueError as e:
            messagebox.showerror("Ошибка", "Некорректные числовые значения!")
            return False, [], []

    def solve_system(self):
        success, A, b = self.get_input_values()
        if not success:
            return

        self.status_label.config(text="Вычисление...")
        self.root.update()

        success, x, message = self.solver.solve(A, b)

        if success:
            # Вычисляем невязку
            residual = self.solver.calculate_residual(A, b, x)
            norm = self.solver.calculate_residual_norm(residual)

            # Отображаем результаты
            self.display_results(x, residual)
            self.norm_label.config(text=f"Норма невязки: {norm:.2e}")
            self.status_label.config(text="Решение найдено!")
        else:
            messagebox.showerror("Ошибка", message)
            self.status_label.config(text="Ошибка решения!")

    def display_results(self, solution, residuals):
        # Очищаем предыдущие результаты
        for widget in self.solution_frame.winfo_children():
            widget.destroy()
        for widget in self.residual_frame.winfo_children():
            widget.destroy()

        self.solution_labels = []
        self.residual_labels = []

        # Отображаем решение
        for i, value in enumerate(solution):
            frame = ttk.Frame(self.solution_frame)
            frame.grid(row=i, column=0, sticky=(tk.W, tk.E), pady=2)

            ttk.Label(frame, text=f"x{i + 1} =", width=5).grid(row=0, column=0, sticky=tk.W)
            label = ttk.Label(frame, text=f"{value:.6f}", width=12)
            label.grid(row=0, column=1, sticky=tk.W)
            self.solution_labels.append(label)

        # Отображаем невязку
        for i, value in enumerate(residuals):
            frame = ttk.Frame(self.residual_frame)
            frame.grid(row=i, column=0, sticky=(tk.W, tk.E), pady=2)

            ttk.Label(frame, text=f"F{i + 1} =", width=5).grid(row=0, column=0, sticky=tk.W)
            label = ttk.Label(frame, text=f"{value:.2e}", width=12)
            label.grid(row=0, column=1, sticky=tk.W)
            self.residual_labels.append(label)

    def load_test(self):
        # Тест №3
        test_matrix = [
            [2.30, 5.70, -0.80],
            [3.50, -2.70, 5.30],
            [1.70, 2.30, -1.80]
        ]
        test_vector = [-6.49, 19.20, -5.09]

        # Устанавливаем размерность
        self.dimension.set(3)
        self.create_matrix_input(3)

        # Заполняем значения
        for i in range(3):
            for j in range(3):
                self.matrix_entries[i][j].delete(0, tk.END)
                self.matrix_entries[i][j].insert(0, str(test_matrix[i][j]))

        for i in range(3):
            self.vector_entries[i].delete(0, tk.END)
            self.vector_entries[i].insert(0, str(test_vector[i]))

        self.clear_results()
        self.status_label.config(text="Тест №3 загружен")

    def clear_all(self):
        dimension = self.dimension.get()
        for i in range(dimension):
            for j in range(dimension):
                self.matrix_entries[i][j].delete(0, tk.END)
                self.matrix_entries[i][j].insert(0, "0.0")

        for i in range(dimension):
            self.vector_entries[i].delete(0, tk.END)
            self.vector_entries[i].insert(0, "0.0")

        self.clear_results()
        self.status_label.config(text="Таблицы очищены")

    def clear_results(self):
        for widget in self.solution_frame.winfo_children():
            widget.destroy()
        for widget in self.residual_frame.winfo_children():
            widget.destroy()
        self.norm_label.config(text="Норма невязки: ")


def main():
    root = tk.Tk()
    app = GaussSolverApp(root)
    root.mainloop()


if __name__ == "__main__":
    main()
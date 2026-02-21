import numpy as np
import matplotlib.pyplot as plt

def gaussian_elimination(A, b):
    n = len(A)
    for i in range(n):
        for k in range(i + 1, n):
            factor = A[k][i] / A[i][i]
            for j in range(i, n):
                A[k][j] -= factor * A[i][j]
            b[k] -= factor * b[i]
    x = [0.0] * n
    for i in range(n - 1, -1, -1):
        x[i] = b[i]
        for j in range(i + 1, n):
            x[i] -= A[i][j] * x[j]
        x[i] /= A[i][i]
    return x

with open('values.txt', 'r') as file:
    lines = file.readlines()
    n, m = map(int, lines[0].split())
    x = []
    y = []
    for line in lines[1:]:
        xi, yi = map(float, line.split())
        x.append(xi)
        y.append(yi)

max_power = 2 * m
powerx = [0.0] * (max_power + 1)
for i in range(n):
    xp = 1.0
    for k in range(max_power + 1):
        powerx[k] += xp
        xp *= x[i]

size = m + 1
sumX = [[0.0 for _ in range(size)] for _ in range(size)]
for l in range(size):
    for j in range(size):
        sumX[l][j] = powerx[l + j]

praw = [0.0] * size
for l in range(size):
    for i in range(n):
        praw[l] += y[i] * (x[i] ** l)

A_copy = [row[:] for row in sumX]
b_copy = praw[:]
a = gaussian_elimination(A_copy, b_copy)
print("Коэффициенты a0, a1, a2:", a)

residual_sum = 0.0
for i in range(n):
    y_hat = 0.0
    for k in range(size):
        y_hat += a[k] * (x[i] ** k)
    residual_sum += (y[i] - y_hat) ** 2
s2 = residual_sum / (n - m - 1)
sigma = np.sqrt(s2)
print("s^2:", s2, "sigma:", sigma)

plt.figure(figsize=(8, 6))

plt.scatter(x, y, color='blue', label='Данные')

min_x = min(x)
max_x = max(x)
approx_x = np.linspace(min_x, max_x, 100)
approx_y = np.zeros(100)
for i in range(100):
    curr_x = approx_x[i]
    y_hat = 0.0
    for k in range(size):
        y_hat += a[k] * (curr_x ** k)
    approx_y[i] = y_hat
plt.plot(approx_x, approx_y, color='red', label='Аппроксимация')

plt.xlabel('x')
plt.ylabel('y')
plt.legend()
plt.grid(True)
plt.show()
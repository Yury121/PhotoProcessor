import numpy as np
import matplotlib.pyplot as plt

# Генерируем случайный вектор
np.random.seed(0)
data = np.random.rand(256)

# Вычисляем FFT
fft_data = np.fft.fft(data)
#pltdata = np.transpose(np.array([fft_data.real, fft_data.img]))

print("Original data:", data)
print("FFT result:", fft_data)


# Создайте массив данных
#x = np.linspace(0, 2 * np.pi, 100)  # x-coordinates from 0 to 2*pi

# Вычислить y-coordinates (например, sine wave)
#y = np.sin(x)
# Построение графика
plt.plot(fft_data.real, fft_data.imag)

# Добавление заголовка и подписей осей
plt.title('Sine Wave')
plt.xlabel('X-axis')
plt.ylabel('Y-axis')

# Сохранение графика в файл (в формате PNG)
#plt.savefig('sine_wave.png')

# Отображение графика
plt.show()

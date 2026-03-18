import numpy as np
import matplotlib.pyplot as plt
import mylib as m


def test():
    N = 1000
    fs = N
    f1 = 10
    q1 = 2
    q2 = 4
    t = np.linspace(0, 2, N)
    y1 = q1 * np.sin(f1 * np.pi * 2 * t)
    y2 = q2 * np.cos(f1 * np.pi * 2 * t)
    y = y1 + y2
    y_fft = np.fft.fft(y)
    y_angle = m.get_magnitude_phase(y, fs, f_signal=10)

    print(f"fft angle = {y_angle}")
    y_fft = np.abs(y_fft)[: N // 2] / N * 2
    y_freq = np.fft.fftfreq(N, 1 / fs)[: N // 2]
    plt.subplot(211)
    plt.plot(t, y, label="time")
    plt.subplot(212)
    plt.plot(y_freq, y_fft, label="freq")
    print(f"fft max = {max(y_fft)}")
    plt.legend()
    plt.show()


test()

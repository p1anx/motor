import numpy as np
from pandas._config import using_nullable_dtypes
from scipy import signal
import matplotlib.pyplot as plt
import mylib as m


def demo1():
    # 参数设置
    M = 16  # 16-QAM
    fs = 8000  # 采样率
    symbol_rate = 1000  # 符号率
    beta = 0.35  # 滚降因子
    sps = int(fs / symbol_rate)  # 每符号采样点数

    # 生成随机QAM符号
    num_symbols = 50
    symbols = np.random.randint(0, M, num_symbols)
    constellation = np.array(
        [
            -3 - 3j,
            -3 - 1j,
            -3 + 3j,
            -3 + 1j,
            -1 - 3j,
            -1 - 1j,
            -1 + 3j,
            -1 + 1j,
            3 - 3j,
            3 - 1j,
            3 + 3j,
            3 + 1j,
            1 - 3j,
            1 - 1j,
            1 + 3j,
            1 + 1j,
        ]
    ) / np.sqrt(10)
    iq_symbols = constellation[symbols]
    file = "./data0/qam4_1Hz_1s_0Bx.csv"
    iq_symbols = m.read_csv(file)

    # 上采样（插零）
    plt.plot(iq_symbols)
    upsampled = np.zeros(len(iq_symbols) * sps, dtype=complex)
    upsampled[::sps] = iq_symbols
    plt.plot(upsampled)

    # 设计升余弦滤波器
    taps = signal.firwin(101, 1 / sps, window=("kaiser", beta))
    taps /= np.sum(taps)  # 归一化

    # 脉冲整形
    shaped_signal = signal.convolve(upsampled, taps, mode="same")

    # 绘制时域波形
    # plt.figure(figsize=(10, 6))
    # plt.plot(np.real(shaped_signal), label="I路（整形后）")
    # plt.plot(np.imag(shaped_signal), label="Q路（整形后）")
    # plt.title("QAM信号（升余弦脉冲整形）")
    # plt.xlabel("采样点")
    # plt.ylabel("幅度")
    # plt.legend()
    # plt.grid(True)
    # plt.show()
    plt.show()


def demo2():
    # 参数设置
    fs = 1000  # 采样率
    numtaps = 101  # 滤波器长度
    cutoff = 100  # 截止频率(Hz)

    # 设计滤波器
    taps = signal.firwin(numtaps, cutoff, fs=fs)

    # 绘制频率响应
    w, h = signal.freqz(taps, fs=fs)
    plt.plot(w, 20 * np.log10(np.abs(h)))
    plt.title("FIR低通滤波器频率响应")
    plt.xlabel("频率 (Hz)")
    plt.ylabel("增益 (dB)")
    plt.grid(True)
    plt.show()


def test():
    file = "./data1/qam4_1Hz_1s_0/Bx.csv"
    data = m.read_csv(file)
    N = len(data) // 2
    data = data[:N]
    x = np.linspace(0, N, N)
    plt.figure(figsize=(8, 6))
    plt.plot(x, data)
    plt.show()


def test_up_cos_filter():
    N = 100
    fs = 100
    f = 5
    x = np.linspace(0, 1, N)
    y = np.sin(2 * np.pi * f * x)
    upsampled = np.zeros(len(y) * 10)
    upsampled[::10] = y
    plt.figure()
    m.plot_fft_magnitude(y, fs)
    plt.figure()
    m.plot_fft_magnitude(upsampled, fs * 10)

    alpha = 0.35
    taps = signal.firwin(101, [0.1, 0.5], window=("kaiser", alpha))
    shaped_signal = signal.convolve(upsampled, taps, mode="same")
    plt.figure()
    m.plot_fft_magnitude(shaped_signal, 1000)


def plot_qam_constellation(file, fs=284, start=408, step=270, g_n_point=10):
    data = m.read_csv(file)
    # fs = 280
    # start = 430
    # step = 570

    magnitudes = []
    phases = []
    for i in range(g_n_point):
        x = range(start, start + step)
        data_segment = data[start : start + step]

        plt.plot(x, data_segment)
        plt.title("without filtered")
        plt.tight_layout()

        start = start + step
        magnitude, phase = m.get_magnitude_phase(data_segment, fs)
        magnitudes.append(magnitude)
        phases.append(phase)

    plt.figure()
    m.plot_qam_constellation(
        magnitudes, phases, 5, show_decision_regions=True, set_tag=False
    )


def plot_qam_constellation_upsample(file, fs=284, start=408, step=270, g_n_point=10):
    data = m.read_csv(file)
    # fs = 280
    # start = 430
    # step = 570

    magnitudes = []
    phases = []
    for i in range(g_n_point):
        x = range(start, start + step)
        data_segment = data[start : start + step]

        plt.plot(x, data_segment)
        plt.title("without filtered")
        plt.tight_layout()

        start = start + step
        magnitude, phase = m.get_magnitude_phase(data_segment, fs)
        magnitudes.append(magnitude)
        phases.append(phase)

    plt.figure()
    m.plot_qam_constellation(
        magnitudes, phases, 5, show_decision_regions=True, set_tag=False
    )


def plot_qam_constellation_upsample_data(
    data, fs=284, start=408, step=270, g_n_point=10
):
    # data = m.read_csv(file)
    # fs = 280
    # start = 430
    # step = 570

    magnitudes = []
    phases = []
    for i in range(g_n_point):
        x = range(start, start + step)
        data_segment = data[start : start + step]

        plt.plot(x, data_segment)
        plt.title("without filtered")
        plt.tight_layout()

        start = start + step
        magnitude, phase = m.get_magnitude_phase(data_segment, fs)
        magnitudes.append(magnitude)
        phases.append(phase)

    plt.figure()
    m.plot_qam_constellation(
        magnitudes, phases, 5, show_decision_regions=True, set_tag=False
    )


def test_data_cos_filter():
    # file = "./data1/qam4_1Hz_1s_0/Bx.csv"
    file = "./data_after_offset_t/qam4_1Hz_1s_7/Bz.csv"
    data = m.read_csv(file)
    # plot_qam_constellation(file)
    start = 408
    step = 270
    # y = data[start : start + step]
    y = data

    fs = 280
    n_zeros = 10
    upsampled = np.zeros(len(y) * n_zeros)
    upsampled[::n_zeros] = y
    plt.figure()
    m.plot_fft_magnitude(y, fs)
    plt.figure()
    m.plot_fft_magnitude(upsampled, fs * n_zeros)

    alpha = 0.35
    taps = signal.firwin(101, 0.001, window=("kaiser", alpha))
    shaped_signal = signal.convolve(upsampled, taps, mode="same")
    plt.figure()
    m.plot_fft_magnitude(shaped_signal, fs * n_zeros)
    start_shaped = 1200
    step_shaped = 2850
    y = shaped_signal[start_shaped : start_shaped + step_shaped]
    plt.figure()
    # m.plot_fft_magnitude(y, fs * n_zeros)

    plot_qam_constellation_upsample_data(
        shaped_signal, fs, start=start_shaped, step=step_shaped
    )


if __name__ == "__main__":
    # demo1()
    # test()
    # plt.plot(upsampled)
    # plt.plot(x, y)

    test_data_cos_filter()
    plt.show()

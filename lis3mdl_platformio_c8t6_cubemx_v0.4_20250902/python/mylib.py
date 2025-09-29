import numpy as np
import matplotlib.pyplot as plt
import pandas as pd
import cmath
from pandas._libs.tslibs.period import period_ordinal
from matplotlib.ticker import MultipleLocator
from scipy.signal import butter, filtfilt, lfilter, freqz

def plot(data):
    n_data = len(data)
    n = np.linspace(0, n_data, n_data)
    plt.plot(n, data)

def find_fft_max(freq, fft, min_range_freq, max_range_freq):
    data = zip(freq, fft)
    data_list = []
    for x, y in data:
        if min_range_freq < x < max_range_freq:
            data_list.append(y)
    return max(data_list)


def read_csv(file):
    df = pd.read_csv(file)
    data = df.to_numpy()
    data = data[:, 0]
    return data


def to_fft(data, fs):
    data = np.array(data)
    data = data - np.mean(data)
    N = len(data)
    magnitude_complex_fft = np.fft.fft(data)
    freq_fft = np.fft.fftfreq(N, 1 / fs)

    magnitude_complex_fft = (magnitude_complex_fft)[: N // 2] / N * 2
    freq_fft = freq_fft[: N // 2]
    return freq_fft, magnitude_complex_fft


def plot_fft_magnitude(data, fs):
    data = np.array(data)
    data = data - np.mean(data)
    x = np.linspace(0, len(data), len(data))

    freq_fft, complex_fft = to_fft(data, fs)
    amp_fft = abs(complex_fft)
    plt.subplot(211)
    plt.plot(x, data, label="time")
    plt.subplot(212)
    plt.plot(freq_fft, amp_fft, label="fft")
    plt.xlabel("freq(Hz)")
    plt.ylabel("amplitude")
    plt.legend()
    plt.tight_layout()


def get_magnitude_phase(signal, Fs, f_signal=1):
    # 1. 生成示例信号
    # 2. 执行FFT
    # f_signal = 1
    n = len(signal)  # 信号长度
    k = np.arange(n)
    T = n / Fs
    freq = k / T  # 频率向量
    freq = freq[: n // 2]  # 取单侧频谱

    # 执行FFT并归一化
    fft_result = np.fft.fft(signal) / n
    fft_result = fft_result[: n // 2]  # 取正频率部分

    # 3. 计算幅度谱
    magnitude = 2 * np.abs(fft_result)  # 乘以2恢复实际幅度(单侧谱)

    magnitude_max = find_fft_max(freq, magnitude, f_signal - 0.5, f_signal + 2)
    # 4. 计算相位谱（弧度制）
    phase = np.angle(fft_result)  # 使用angle函数直接计算相位

    # ====== 新增：幅度阈值处理 ======
    # 方法1：固定阈值（根据信号幅度调整）
    # threshold = 0.05  # 固定阈值

    # 方法2：动态阈值（推荐）
    threshold_ratio = 0.8  # 最大幅度的百分比
    # threshold = np.max(magnitude) * threshold_ratio
    threshold = magnitude_max * threshold_ratio

    # 创建清理后的相位数组
    clean_phase = np.copy(phase)

    # 应用阈值：低于阈值的相位设为NaN（不显示）
    # for i in range(len(magnitude)):
    #     if magnitude[i] < threshold:
    #         clean_phase[i] = np.nan  # 设为NaN，绘图时自动忽略

    # ====== 可选：相位解卷绕 ======
    # unwrapped_phase = np.unwrap(clean_phase)  # 处理相位跳变
    # 原始时域信号

    # ====== 新增：在信号频率点查看相位值 ======
    # 找到10Hz和20Hz附近的索引
    idx_10hz = np.argmin(np.abs(freq - f_signal))
    # idx_20hz = np.argmin(np.abs(freq - 20))

    print("\n相位值报告:")
    print(
        f"{f_signal}Hz分量: 幅度={magnitude[idx_10hz]:.4f}, 相位={clean_phase[idx_10hz]:.4f} rad (≈{np.degrees(clean_phase[idx_10hz]):.1f}°)"
    )
    # 相位谱（原始和清理后对比）
    return magnitude[idx_10hz], clean_phase[idx_10hz]


def plot_qam_constellation(
    amplitudes, phases, M, title=None, show_decision_regions=False, set_tag=False
):
    """
    根据幅度和相位绘制QAM星座图

    参数:
        amplitudes (list/array): 每个星座点的幅度值
        phases (list/array): 每个星座点的相位值(弧度)
        M (int): QAM调制阶数(4, 16, 64, 256等)
        title (str): 图表标题
        show_decision_regions (bool): 是否显示判决区域
    """
    # 检查输入
    # if len(amplitudes) != M or len(phases) != M:
    #     raise ValueError(f"需要 {M} 个幅度和相位值")

    # 将幅度和相位转换为复数(I/Q)
    constellation = []
    for amp, phase in zip(amplitudes, phases):
        # 使用极坐标转换为复数
        complex_point = cmath.rect(amp, phase)
        constellation.append(complex_point)

    constellation = np.array(constellation)
    # 归一化处理：使星座点平均功率为1
    avg_power = np.mean(np.abs(constellation) ** 2)
    scale_factor = 1.0 / np.sqrt(avg_power)
    constellation = constellation * scale_factor

    # 创建图表
    # plt.figure(figsize=(10, 10))
    ax = plt.gca()

    # 绘制星座点
    plt.scatter(
        np.real(constellation),
        np.imag(constellation),
        s=10,
        c="blue",
        marker="o",
        label="constellation",
    )
    # ax.set_aspect("equal")

    # 标记坐标原点
    plt.scatter(0, 0, s=50, c="red", marker="o", label="origin")

    # 添加幅度和相位标记
    if set_tag == True:
        for i, point in enumerate(constellation):
            plt.annotate(
                f"P{i}\nA={amplitudes[i]:.2f}\nθ={np.degrees(phases[i]):.1f}°",
                (np.real(point), np.imag(point)),
                xytext=(np.real(point) + 0.1, np.imag(point) + 0.1),
                arrowprops=dict(arrowstyle="->", lw=1),
            )

    # 绘制决策区域（如果需要）
    if show_decision_regions and M > 4:
        # 找出星座点的最小和最大坐标值
        min_val = np.min(np.real(constellation)) - 0.5
        max_val = np.max(np.real(constellation)) + 0.5

        # 创建网格
        x = np.linspace(min_val, max_val, 200)
        y = np.linspace(min_val, max_val, 200)
        X, Y = np.meshgrid(x, y)
        grid = X + 1j * Y

        # 为每个网格点分配最近星座点
        decision_map = np.zeros_like(X, dtype=int)
        for i in range(len(x)):
            for j in range(len(y)):
                point = grid[j, i]
                distances = np.abs(constellation - point)
                decision_map[j, i] = np.argmin(distances)

        # 绘制决策区域
        plt.imshow(
            decision_map,
            extent=(min_val, max_val, min_val, max_val),
            origin="lower",
            alpha=0.3,
            cmap="tab20",
        )

    # 设置图表属性
    plt.axhline(0, color="gray", lw=0.5, ls="--")
    plt.axvline(0, color="gray", lw=0.5, ls="--")
    plt.title(title or f"{M}-QAM constellation", fontsize=16)
    plt.xlabel("I", fontsize=14)
    plt.ylabel("Q", fontsize=14)
    plt.grid(True, linestyle="--", alpha=0.7)

    # 设置坐标轴刻度和范围
    max_axis = np.max(np.abs(constellation)) * 1.2
    # plt.axis([-max_axis, max_axis, -max_axis, max_axis])

    # 设置网格线间距
    step = max_axis / 5
    ax.xaxis.set_major_locator(MultipleLocator(step))
    ax.yaxis.set_major_locator(MultipleLocator(step))

    # 添加极坐标网格
    ax.grid(True, which="major", linestyle="-", linewidth=0.5)
    ax.grid(True, which="minor", linestyle=":", linewidth=0.5)

    # 添加图例
    plt.legend()
    plt.tight_layout()
    # plt.show()

    return constellation


def butter_bandpass(lowcut, highcut, fs, order=5):
    """
    Design a Butterworth bandpass filter

    Parameters:
    lowcut (float): Low cutoff frequency (Hz)
    highcut (float): High cutoff frequency (Hz)
    fs (float): Sampling frequency (Hz)
    order (int): Filter order

    Returns:
    b, a (ndarray, ndarray): Numerator and denominator polynomials of the IIR filter
    """
    nyq = 0.5 * fs
    low = lowcut / nyq
    high = highcut / nyq
    b, a = butter(order, [low, high], btype="bandpass")
    return b, a


def bandpass_filter(data, lowcut, highcut, fs, order=5):
    """
    Apply bandpass filter to signal

    Parameters:
    data (array): Input signal
    lowcut (float): Low cutoff frequency (Hz)
    highcut (float): High cutoff frequency (Hz)
    fs (float): Sampling frequency (Hz)
    order (int): Filter order

    Returns:
    y (array): Filtered signal
    """
    b, a = butter_bandpass(lowcut, highcut, fs, order=order)
    # y = lfilter(b, a, data)
    y = filtfilt(b, a, data)
    return y


def butter_lowpass(cutoff, fs, order=5):
    """
    Design a Butterworth lowpass filter

    Parameters:
    cutoff (float): Cutoff frequency (Hz)
    fs (float): Sampling frequency (Hz)
    order (int): Filter order

    Returns:
    b, a (ndarray, ndarray): Numerator and denominator polynomials of the IIR filter
    """
    nyq = 0.5 * fs  # Nyquist frequency
    normal_cutoff = cutoff / nyq
    b, a = butter(order, normal_cutoff, btype="low", analog=False)
    return b, a


def lowpass_filter(data, cutoff, fs, order=5):
    """
    Apply lowpass filter to signal

    Parameters:
    data (array): Input signal
    cutoff (float): Cutoff frequency (Hz)
    fs (float): Sampling frequency (Hz)
    order (int): Filter order

    Returns:
    y (array): Filtered signal
    """
    b, a = butter_lowpass(cutoff, fs, order=order)
    # y = lfilter(b, a, data)
    y = filtfilt(b, a, data)
    return y

def plot_qam_constellation_filtered(xyz, file, fs=284, start=231, step=270, g_n_point=10):

    data = read_csv(file)
    # fs = 280
    # start = 430
    # step = 570

    magnitudes = []
    phases = []
    for i in range(g_n_point):
        x = range(start, start + step)
        # plt.subplot(211)
        data_segment = data[start : start + step]
        # print(f"x = {len(x)}, data = {len(data_segment)}")

        plt.subplot(211)
        plt.plot(x, data_segment)
        plt.title("without filtered")
        # m.butter_bandpass()
        # plt.figure()

        # data_segment = m.bandpass_filter(data_segment, 0.01, 2, fs)
        if 1:
            # data_filtered = m.bandpass_filter(data_segment, 1, 2, fs)
            data_filtered = lowpass_filter(data_segment, 2, fs, order=3)
            data_segment = data_filtered

        # plt.figure()
        # plt.plot(np.arange(len(data_filtered)), data_filtered)
        # m.plot_fft_magnitude(data_filtered, fs)
        plt.subplot(212)
        plt.plot(x, data_segment)
        plt.title("filtered")
        plt.tight_layout()

        start = start + step
        magnitude, phase = get_magnitude_phase(data_segment, fs)
        magnitudes.append(magnitude)
        phases.append(phase)

    # plt.subplot(212)
    # plt.savefig(f"./{file}_time.png", dpi=300)
    plt.savefig(
        f"./{file}_{g_n_point}p_{xyz}_step{step}_time.png",
        dpi=300,
    )
    plt.figure()
    plot_qam_constellation(
        magnitudes, phases, 16, show_decision_regions=True, set_tag=False
    )
def plot_constellation(xyz= "x", g_n_point = 20, start = 0, step = 280, folderName = ""):
    # xyz = "y"
    # g_n_point = 20*5

    # start = 549  # 84
    # step = 283  # 280, 1180
    # folderName = "data/16qam_fc1Hz1Hz_0"
    file = f"./{folderName}/B{xyz}.csv"

    # plot_qam_constellation_test(file, start=start, step=step, g_n_point=g_n_point)
    plot_qam_constellation_filtered(xyz, file, start=start, step=step, g_n_point=g_n_point)

    # plt.savefig(f"{folderName}_{g_n_point}p_{xyz}_step{step}.png", dpi=300)
    plt.savefig(
        f"{folderName}/_{g_n_point}p_{xyz}_step{step}_test_filtered_bandpass.png",
        dpi=300,
    )

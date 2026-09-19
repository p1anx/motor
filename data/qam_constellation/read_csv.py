from math import fma, sqrt
import numpy as np
import pandas as pd
import matplotlib.pyplot as plt
# from pandas.compat import F
# from pandas.core.common import T
from plot_two_csv import plot_two_csv


def _median_filter(y: np.ndarray, window: int = 5) -> np.ndarray:
    """简单中值滤波，去除毛刺。window 需为奇数。"""
    if window < 3 or window % 2 == 0:
        return y.copy()
    N = y.size
    k = window // 2
    padded = np.pad(y, (k, k), mode="edge")
    out = np.empty_like(y)
    for i in range(N):
        out[i] = np.median(padded[i : i + window])
    return out


def _moving_average(y: np.ndarray, window: int = 7) -> np.ndarray:
    """简单滑动平均，进一步平滑波形。"""
    if window < 1:
        return y.copy()
    kernel = np.ones(window, dtype=float) / window
    return np.convolve(y, kernel, mode="same")


def lowpass_signal(y: np.ndarray, window: int = 31) -> np.ndarray:
    """
    简单低通滤波（滑动平均 FIR）。
    window 越大，截止频率越低、平滑越强。
    """
    if window < 1:
        raise ValueError("window 必须为正整数。")
    return _moving_average(np.asarray(y, dtype=float), window=window)


def denoise_signal(one_period: np.ndarray) -> np.ndarray:
    """只做一次中值滤波，主要去掉孤立毛刺，尽量不改变正弦形状。"""
    y = np.asarray(one_period, dtype=float)
    y1 = _median_filter(y, window=5)  # 可按需要调整窗口为 3/5/7
    return y1


def _phases_for_level(
    y: np.ndarray,
    level: float,
    phase_start_deg: float,
    phase_end_deg: float,
    merge_thresh_deg: float = 2.0,
) -> np.ndarray:
    """
    在数据对应的相位区间 [phase_start_deg, phase_end_deg] 内，找到幅度=level 的所有相位点，
    并按阈值合并相近的点。

    merge_thresh_deg: 若两个相位点相差小于该阈值（度），则认为是同一个点并合并（取平均）。
    """
    N = y.size
    if N < 2:
        return np.array([])

    # 数据索引 0..N-1 对应相位 phase_start_deg .. phase_end_deg
    phases = np.linspace(phase_start_deg, phase_end_deg, N, endpoint=True)
    phases_win = phases
    y_win = y

    diff = y_win - level
    sign = np.sign(diff)
    idxs = np.where(sign[:-1] * sign[1:] <= 0)[0]
    if idxs.size == 0:
        return np.array([])

    raw_phis: list[float] = []
    for k in idxs:
        x0, x1 = phases_win[k], phases_win[k + 1]
        y0, y1 = y_win[k], y_win[k + 1]
        if y1 == y0:
            phi = float(x0)
        else:
            phi = float(x0 + (level - y0) * (x1 - x0) / (y1 - y0))
        raw_phis.append(phi)

    if not raw_phis:
        return np.array([])

    # 对所有相位点排序，然后用阈值合并相近的点
    raw_phis = sorted(raw_phis)
    clusters: list[list[float]] = [[raw_phis[0]]]
    for phi in raw_phis[1:]:
        if abs(phi - clusters[-1][-1]) <= merge_thresh_deg:
            clusters[-1].append(phi)
        else:
            clusters.append([phi])

    merged = [float(np.mean(c)) for c in clusters]
    return np.asarray(merged)


def quarter_points(
    one_period: np.ndarray,
    phase_start_deg: float = 360.0 * 0.01,
    phase_end_deg: float = 360.0 * 0.99,
    merge_thresh_deg: float = 2.0,
) -> tuple[np.ndarray, np.ndarray]:
    """
    对一个周期的正弦信号：
    1. 数据按相位线性映射：第 0 个点 = phase_start_deg，第 N-1 个点 = phase_end_deg；
    2. 计算峰峰值的 1/4 和 3/4 幅度，找出这些幅度对应的所有相位点；
    3. 对相近的相位点按阈值合并。返回的相位值均在 [phase_start_deg, phase_end_deg] 内。

    注意：不在此函数内部滤波，外面先调用 denoise_signal 再传入。
    返回:
        phase_deg: 所有合并后的相位点（单位：度，与传入的起始/终点相位一致）
        amps:      对应的幅度值（与 phase_deg 一一对应）
    """
    y = np.asarray(one_period, dtype=float)
    N = y.size
    if N < 2:
        return np.array([]), np.array([])

    # 数据对应的相位：索引 0 → phase_start_deg，索引 N-1 → phase_end_deg
    phases = np.linspace(phase_start_deg, phase_end_deg, N, endpoint=True)
    y_win = y
    y_min, y_max = float(y_win.min()), float(y_win.max())
    A1 = y_min + 0.25 * (y_max - y_min)
    A3 = y_min + 0.75 * (y_max - y_min)

    phase_list: list[float] = []
    amp_list: list[float] = []

    # 1/4 幅度：找到所有相位点，并按阈值合并
    phi1_all = _phases_for_level(
        y,
        level=A1,
        phase_start_deg=phase_start_deg,
        phase_end_deg=phase_end_deg,
        merge_thresh_deg=merge_thresh_deg,
    )
    for phi in phi1_all:
        phase_list.append(float(phi))
        amp_list.append(A1)

    # 3/4 幅度：同样处理
    phi3_all = _phases_for_level(
        y,
        level=A3,
        phase_start_deg=phase_start_deg,
        phase_end_deg=phase_end_deg,
        merge_thresh_deg=merge_thresh_deg,
    )
    for phi in phi3_all:
        phase_list.append(float(phi))
        amp_list.append(A3)

    return np.asarray(phase_list), np.asarray(amp_list)


def main():
    # 1. 用 pandas 读取 CSV 数据
    df = pd.read_csv("data0.csv")

    # 假设 ch1 是你要处理的那个正弦通道
    if "ch1" not in df.columns:
        raise ValueError("data.csv 中未找到列 'ch1'，请确认列名。")

    one_period = df["ch2"].to_numpy()

    # 2. 先滤波，去除毛刺（只做一次）
    filtered = denoise_signal(one_period)
    # filtered = one_period

    # 3. 在峰峰值的 1/4 和 3/4 处求相位点（数据起始相位 3.6°，终点 360*0.99°）
    phase_start_deg = 3.6
    phase_end_deg = 360.0 * 0.99
    phase_deg, amps = quarter_points(
        one_period=filtered,
        phase_start_deg=phase_start_deg,
        phase_end_deg=phase_end_deg,
        merge_thresh_deg=2.0,
    )

    # 4. 打印结果
    for i, (p, a) in enumerate(zip(phase_deg, amps), start=1):
        print(f"Point {i}: phase = {p:.3f} deg, amp = {a:.6f}")

    # 5. 保存结果到 CSV
    out = pd.DataFrame({"phase_deg": phase_deg, "amp": amps})
    out.to_csv("phase_points.csv", index=False)
    print("1/4 和 3/4 等幅度相位点已保存到 phase_points.csv")

    # 6. 画出原始/滤波后正弦和等幅度点（x 轴为角度 phase_start_deg ~ phase_end_deg）
    N = one_period.size
    phases_all = np.linspace(phase_start_deg, phase_end_deg, N, endpoint=True)

    plt.figure()
    plt.plot(phases_all, one_period, label="raw sine")
    plt.plot(phases_all, filtered, label="filtered sine")
    plt.scatter(phase_deg, amps, color="red", s=100, label="1/4 & 3/4 points")
    plt.xlabel("phase (deg)")
    plt.ylabel("amplitude")
    plt.legend()
    plt.grid(True)
    plt.tight_layout()
    plt.show()


def plot_raw(file_path: str, ch: str = "ch2"):
    """只绘制原始正弦波（不滤波、不标记相位点）。"""
    # file_path = "plot_constellation/data_20260511/data_20260511_142033.csv"
    df = pd.read_csv(file_path)
    if ch not in df.columns:
        raise ValueError(f"data.csv 中未找到列 {ch}，请确认列名。")

    one_period = df[ch].to_numpy()
    N = one_period.size
    x = np.arange(N)

    plt.figure()
    plt.plot(x, one_period, label="raw sine")
    plt.xlabel("sample index")
    plt.ylabel("amplitude")
    plt.legend()
    plt.grid(True)
    plt.tight_layout()
    plt.show()


def plot_raw_lowpass(
    file_path: str = "plot_constellation/data_20260511/data_20260511_142033.csv",
    col: str = "ch2",
    window: int = 31,
):
    """绘制原始数据及其低通滤波结果（滑动平均）。"""
    df = pd.read_csv(file_path)
    if col not in df.columns:
        raise ValueError(f"CSV 中未找到列 '{col}'，请确认列名。")

    y = df[col].to_numpy()
    y_lp = lowpass_signal(y, window=window)
    x = np.arange(y.size)

    plt.figure()
    plt.plot(x, y, label="raw", alpha=0.6)
    plt.plot(x, y_lp, label=f"lowpass(ma window={window})", linewidth=2)
    plt.xlabel("sample index (original)")
    plt.ylabel("amplitude")
    plt.legend()
    plt.grid(True)
    plt.tight_layout()
    plt.show()


def _local_maxima_indices(y: np.ndarray) -> np.ndarray:
    """返回局部极大值点索引（简单三点比较）。"""
    y = np.asarray(y, dtype=float)
    if y.size < 3:
        return np.array([], dtype=int)
    return np.where((y[1:-1] > y[:-2]) & (y[1:-1] >= y[2:]))[0] + 1


def plot_maxima_points(
    file_path: str = "plot_constellation/data_20260511/data_20260511_142033.csv",
    col: str = "ch3",
    lowpass_window: int | None = 31,
):
    """
    绘制曲线的极大值点，并将极大值点序列单独画成一条曲线。

    - lowpass_window=None: 不滤波，直接在原始数据上找极大值
    - lowpass_window=整数: 先低通再找极大值（通常更稳）
    """
    df = pd.read_csv(file_path)
    if col not in df.columns:
        raise ValueError(f"CSV 中未找到列 '{col}'，请确认列名。")

    y_raw = df[col].to_numpy()
    y = y_raw if lowpass_window is None else lowpass_signal(y_raw, window=int(lowpass_window))
    x = np.arange(y.size)
    k  = (360*(0.99-0.01)) /  49011# 25790 是采样点数，360是度数，0.99-0.01是相位范围
    x = x * k

    idx = _local_maxima_indices(y)
    x_peak = x[idx]
    y_peak = y[idx]

    fig, (ax0, ax1) = plt.subplots(2, 1, figsize=(10, 6), sharex=False)

    ax0.plot(x, y, label="signal" if lowpass_window is None else f"lowpass(window={lowpass_window})")
    ax0.scatter(x_peak, y_peak, s=15, color="red", label=f"maxima (n={idx.size})")
    ax0.set_xlabel("sample index (original)")
    ax0.set_ylabel("amplitude")
    ax0.set_title("Signal with local maxima points")
    ax0.grid(True)
    ax0.legend()

    ax1.plot(x_peak, y_peak, marker="o", linewidth=1)
    ax1.set_xlabel("sample index (peak positions)")
    ax1.set_ylabel("peak amplitude")
    ax1.set_title("Maxima points curve")
    ax1.grid(True)

    plt.tight_layout()
    plt.show()

    return idx, y_peak

def plot_segmented_qam_standard(
    file_path: str = "plot_constellation/data_20260511/data_20260511_142033.csv",
    segment_len: int = 1024,
    isPlot = True,
    hasFlag = True,
    QAM_type = 64,
    cycle_k = 1.1,
    ch = 'ch2',
) -> np.ndarray:
    """
    将 ch2 数据按固定采样点数分段，在一个图里叠加绘制所有分段的正弦波形，
    并在单独的子图中绘制各分段的峰峰值曲线。

    返回值:
        vpps: shape=(num_segments,) 的数组，存放每段的峰峰值幅度。
    """
    cycle = 500
    # qam_phase_time = [51, 125, 125, 198]
    # qam_phase_time = [125,51, 198, 125]
    qam_phase_time = [125,51,198, 125, 301, 375, 448, 375, 551, 625, 698, 625, 801, 875, 948, 875]

    # ch = "ch2"
    # qam_phase = int(np.array(qam_phase) * (1/2))
    if segment_len <= 0:
        raise ValueError("segment_len 必须为正整数。")

    df = pd.read_csv(file_path)
    if ch not in df.columns:
        raise ValueError("CSV 中未找到列 'ch2'，请确认列名。")

    data_segment = []
    data_segment_index = []
    data = df[ch].to_numpy()
    N = data.size
    num_segments = N // segment_len
    if num_segments == 0:
        raise ValueError(f"数据长度 {N} 小于一个分段长度 {segment_len}。")

    vpps: list[float] = []

    fig, (ax_wave, ax_vpp) = plt.subplots(2, 1, figsize=(8, 6), sharex=False)
    qam_index = []

    for i in range(num_segments):
        if QAM_type == 16:
            qam_num = len(qam_phase_time)
            # qam_phase = [0, 250//2, 500//2, 750//2] 
            # qam_phase = [qam_phase_time[0]//2, qam_phase_time[1]//2, qam_phase_time[2]//2, qam_phase_time[3]//2] 
            qam_phase = [qam_phase_time[i]//2 for i in range(qam_num)]
        elif QAM_type == 64:
            qam_num = 8
            qam_phase = [0,125//2, 250//2,375//2, 500//2, 625//2, 750//2, 875//2] 
        i_index =    i % (qam_num)
        start = i * segment_len
        start = i * segment_len + qam_phase[i_index]
        end = int(start + cycle* cycle_k)
        # end = (i+1)* segment_len
        seg = data[start:end]
        vpp = float(seg.max() - seg.min())
        vpps.append(vpp)

        # 使用原始数据的全局索引作为 x 轴
        x_global = np.arange(start, end)

        # if i_index == qam_num:
        #     pass
        # else:
            # x_global = np.arange(start+qam_phase[ii], end)
            # if len(qam_index) > 0 and qam_index[-1] == start:
            #     pass
            # else:
        qam_index.append(start)
        qam_index.append(end)
        data_segment.append(seg)
        data_segment_index.append(x_global)
        if isPlot:
            print(f'x = {x_global}')
            ax_wave.plot(x_global, seg, alpha=0.6, label=f"seg {i}")

    # 上图：所有分段正弦叠加，x 轴为原始采样点索引
    if isPlot:
        ax_wave.set_xlabel("sample index (original)")
        ax_wave.set_ylabel("amplitude")
        ax_wave.set_title(f"All segments (len={segment_len})")
        ax_wave.grid(True)
        ax_wave.legend(loc="upper right", fontsize=8, ncol=2)

        # 下图：峰峰值曲线
        vpps_arr = np.asarray(vpps, dtype=float)
        seg_idx = np.arange(num_segments)
        ax_vpp.plot(seg_idx, vpps_arr, marker="o")
        ax_vpp.set_xlabel("segment index")
        ax_vpp.set_ylabel("Vpp")
        ax_vpp.set_title("Vpp of each segment")
        ax_vpp.grid(True)

        plt.tight_layout()
        plt.show()

        print("各段峰峰值幅度 (Vpp):")
        for i, v in enumerate(vpps_arr):
            print(f"segment {i}: Vpp = {v:.6f}")

    return data_segment_index, data_segment, qam_index
def plot_segmented_qam(
    file_path: str = "plot_constellation/data_20260511/data_20260511_142033.csv",
    segment_len: int = 1024,
    isPlot = True,
    hasFlag = True,
    QAM_type = 64
) -> np.ndarray:
    """
    将 ch2 数据按固定采样点数分段，在一个图里叠加绘制所有分段的正弦波形，
    并在单独的子图中绘制各分段的峰峰值曲线。

    返回值:
        vpps: shape=(num_segments,) 的数组，存放每段的峰峰值幅度。
    """
    cycle = 500
    qam_phase_time = [51, 125, 125, 198]
    ch = "ch3"
    # qam_phase = int(np.array(qam_phase) * (1/2))
    if segment_len <= 0:
        raise ValueError("segment_len 必须为正整数。")

    df = pd.read_csv(file_path)
    if ch not in df.columns:
        raise ValueError("CSV 中未找到列 'ch2'，请确认列名。")

    data_segment = []
    data_segment_index = []
    data = df[ch].to_numpy()
    N = data.size
    num_segments = N // segment_len
    if num_segments == 0:
        raise ValueError(f"数据长度 {N} 小于一个分段长度 {segment_len}。")

    vpps: list[float] = []

    fig, (ax_wave, ax_vpp) = plt.subplots(2, 1, figsize=(8, 6), sharex=False)
    qam_index = []

    for i in range(num_segments):
        if QAM_type == 16:
            qam_num = 4
            qam_phase = [0, 250//2, 500//2, 750//2] 
            # qam_phase = [qam_phase_time[0]//2, qam_phase_time[1]//2, qam_phase_time[2]//2, qam_phase_time[3]//2] 
        elif QAM_type == 64:
            qam_num = 8
            qam_phase = [0,125//2, 250//2,375//2, 500//2, 625//2, 750//2, 875//2] 
        i_index =    i % (qam_num+1)
        start = i * segment_len
        if i_index < qam_num:
            start = i * segment_len + qam_phase[i_index]
        end = start + cycle
        seg = data[start:end]
        vpp = float(seg.max() - seg.min())
        vpps.append(vpp)

        # 使用原始数据的全局索引作为 x 轴
        x_global = np.arange(start, end)

        if i_index == qam_num:
            pass
        else:
            # x_global = np.arange(start+qam_phase[ii], end)
            # if len(qam_index) > 0 and qam_index[-1] == start:
            #     pass
            # else:
            qam_index.append(start)
            qam_index.append(end)
            data_segment.append(seg)
            data_segment_index.append(x_global)
            print(f'x = {x_global}')
            ax_wave.plot(x_global, seg, alpha=0.6, label=f"seg {i}")

    # 上图：所有分段正弦叠加，x 轴为原始采样点索引
    ax_wave.set_xlabel("sample index (original)")
    ax_wave.set_ylabel("amplitude")
    ax_wave.set_title(f"All segments (len={segment_len})")
    ax_wave.grid(True)
    ax_wave.legend(loc="upper right", fontsize=8, ncol=2)

    # 下图：峰峰值曲线
    vpps_arr = np.asarray(vpps, dtype=float)
    seg_idx = np.arange(num_segments)
    ax_vpp.plot(seg_idx, vpps_arr, marker="o")
    ax_vpp.set_xlabel("segment index")
    ax_vpp.set_ylabel("Vpp")
    ax_vpp.set_title("Vpp of each segment")
    ax_vpp.grid(True)

    plt.tight_layout()
    if isPlot:
        plt.show()

    print("各段峰峰值幅度 (Vpp):")
    for i, v in enumerate(vpps_arr):
        print(f"segment {i}: Vpp = {v:.6f}")

    return data_segment_index, data_segment, qam_index
def plot_segmented_sine_group(
    file_path: str = "plot_constellation/data_20260511/data_20260511_142033.csv",
    segment_len: int = 1024,
    isPlot = True,
    QAM_type = 16

) -> np.ndarray:
    """
    将 ch2 数据按固定采样点数分段，在一个图里叠加绘制所有分段的正弦波形，
    并在单独的子图中绘制各分段的峰峰值曲线。

    返回值:
        vpps: shape=(num_segments,) 的数组，存放每段的峰峰值幅度。
    """
    if segment_len <= 0:
        raise ValueError("segment_len 必须为正整数。")

    df = pd.read_csv(file_path)
    ch = "ch3"
    if ch not in df.columns:
        raise ValueError("CSV 中未找到列 'ch2'，请确认列名。")

    data_segment = []
    data_segment_index = []
    data = df[ch].to_numpy()
    N = data.size
    num_segments = N // segment_len
    if num_segments == 0:
        raise ValueError(f"数据长度 {N} 小于一个分段长度 {segment_len}。")

    vpps: list[float] = []

    fig, (ax_wave, ax_vpp) = plt.subplots(2, 1, figsize=(8, 6), sharex=False)
    qam_index = []

    for i in range(num_segments):
        start = i * segment_len
        end = start + segment_len
        seg = data[start:end]
        vpp = float(seg.max() - seg.min())
        vpps.append(vpp)

        # 使用原始数据的全局索引作为 x 轴
        x_global = np.arange(start, end)
        if QAM_type == 16:
            qam_num = 4
        elif QAM_type == 64:
            qam_num = 8

        if i % (qam_num + 1) == qam_num:
            pass
        else:
            # if len(qam_index) > 0 and qam_index[-1] == start:
            #     pass
            # else:
            qam_index.append(start)
            qam_index.append(end)
            data_segment.append(seg)
            data_segment_index.append(x_global)
            print(f'x = {x_global}')
            ax_wave.plot(x_global, seg, alpha=0.6, label=f"seg {i}")

    # 上图：所有分段正弦叠加，x 轴为原始采样点索引
    ax_wave.set_xlabel("sample index (original)")
    ax_wave.set_ylabel("amplitude")
    ax_wave.set_title(f"All segments (len={segment_len})")
    ax_wave.grid(True)
    ax_wave.legend(loc="upper right", fontsize=8, ncol=2)

    # 下图：峰峰值曲线
    vpps_arr = np.asarray(vpps, dtype=float)
    seg_idx = np.arange(num_segments)
    ax_vpp.plot(seg_idx, vpps_arr, marker="o")
    ax_vpp.set_xlabel("segment index")
    ax_vpp.set_ylabel("Vpp")
    ax_vpp.set_title("Vpp of each segment")
    ax_vpp.grid(True)

    plt.tight_layout()
    if isPlot:
        plt.show()

    print("各段峰峰值幅度 (Vpp):")
    for i, v in enumerate(vpps_arr):
        print(f"segment {i}: Vpp = {v:.6f}")

    return data_segment_index, data_segment, qam_index

def plot_segmented_sine(
    file_path: str = "plot_constellation/data_20260511/data_20260511_142033.csv",
    segment_len: int = 1024,
) -> np.ndarray:
    """
    将 ch2 数据按固定采样点数分段，在一个图里叠加绘制所有分段的正弦波形，
    并在单独的子图中绘制各分段的峰峰值曲线。

    返回值:
        vpps: shape=(num_segments,) 的数组，存放每段的峰峰值幅度。
    """
    if segment_len <= 0:
        raise ValueError("segment_len 必须为正整数。")

    df = pd.read_csv(file_path)
    ch = "ch3"
    if ch not in df.columns:
        raise ValueError("CSV 中未找到列 'ch2'，请确认列名。")

    data = df[ch].to_numpy()
    N = data.size
    num_segments = N // segment_len
    if num_segments == 0:
        raise ValueError(f"数据长度 {N} 小于一个分段长度 {segment_len}。")

    vpps: list[float] = []

    fig, (ax_wave, ax_vpp) = plt.subplots(2, 1, figsize=(8, 6), sharex=False)

    for i in range(num_segments):
        start = i * segment_len
        end = start + segment_len
        seg = data[start:end]
        vpp = float(seg.max() - seg.min())
        vpps.append(vpp)

        # 使用原始数据的全局索引作为 x 轴
        x_global = np.arange(start, end)
        ax_wave.plot(x_global, seg, alpha=0.6, label=f"seg {i}")

    # 上图：所有分段正弦叠加，x 轴为原始采样点索引
    ax_wave.set_xlabel("sample index (original)")
    ax_wave.set_ylabel("amplitude")
    ax_wave.set_title(f"All segments (len={segment_len})")
    ax_wave.grid(True)
    ax_wave.legend(loc="upper right", fontsize=8, ncol=2)

    # 下图：峰峰值曲线
    vpps_arr = np.asarray(vpps, dtype=float)
    seg_idx = np.arange(num_segments)
    ax_vpp.plot(seg_idx, vpps_arr, marker="o")
    ax_vpp.set_xlabel("segment index")
    ax_vpp.set_ylabel("Vpp")
    ax_vpp.set_title("Vpp of each segment")
    ax_vpp.grid(True)

    plt.tight_layout()
    plt.show()

    print("各段峰峰值幅度 (Vpp):")
    for i, v in enumerate(vpps_arr):
        print(f"segment {i}: Vpp = {v:.6f}")
    return vpps_arr


def plot_raw_three(file: str = "data.csv"):
    """绘制三个通道的原始数据（ch1, ch2, ch3），x 轴为采样点索引。"""
    df = pd.read_csv(file)
    for col in ("ch1", "ch2", "ch3"):
        if col not in df.columns:
            raise ValueError(f"data.csv 中未找到列 '{col}'，请确认列名。")

    ch1 = df["ch1"].to_numpy()
    ch2 = df["ch2"].to_numpy()
    ch3 = df["ch3"].to_numpy()
    N = len(ch1)
    x = np.arange(N)

    plt.figure(figsize=(8, 8))

    plt.subplot(3, 1, 1)
    plt.plot(x, ch1, label="ch1")
    plt.ylabel("ch1")
    plt.grid(True)

    plt.subplot(3, 1, 2)
    plt.plot(x, ch2, label="ch2", color="orange")
    plt.ylabel("ch2")
    plt.grid(True)

    plt.subplot(3, 1, 3)
    plt.plot(x, ch3, label="ch3", color="green")
    plt.xlabel("sample index")
    plt.ylabel("ch3")
    plt.grid(True)

    plt.tight_layout()
    plt.savefig(f"{file.split('/')[-1].split('.')[0]}_raw_three.png", dpi=300)
    plt.show()
def plot_raw_and_fft(
    file_path: str = "plot_constellation/data_20260511/data_20260511_142033.csv",
    col: str = "ch3",
    fs: float = 1.0,
    remove_dc: bool = True,
):
    """
    绘制原始时域波形和 FFT 幅度谱。
    参数:
        file_path: CSV 路径
        col:      需要分析的列名
        fs:       采样率(Hz)，默认 1.0；若未知可先用默认值看归一化频率趋势
        remove_dc: 是否先减去均值再做 FFT，默认 True
    """
    if fs <= 0:
        raise ValueError("fs 必须大于 0。")
    df = pd.read_csv(file_path)
    if col not in df.columns:
        raise ValueError(f"CSV 中未找到列 '{col}'，请确认列名。")
    y = df[col].to_numpy(dtype=float)
    n = y.size
    if n < 2:
        raise ValueError("数据点太少，无法进行 FFT 分析。")
    y_fft_in = y - np.mean(y) if remove_dc else y.copy()
    # 单边幅度谱（实数信号）
    yf = np.fft.rfft(y_fft_in)
    freqs = np.fft.rfftfreq(n, d=1.0 / fs)
    amp = np.abs(yf) * 2.0 / n
    amp[0] = amp[0] * 0.5  # 直流分量不需要乘 2
    x_time = np.arange(n)
    fig, (ax0, ax1) = plt.subplots(2, 1, figsize=(10, 6), sharex=False)
    ax0.plot(x_time, y, label=f"raw {col}")
    ax0.set_xlabel("sample index")
    ax0.set_ylabel("amplitude")
    ax0.set_title("Raw signal")
    ax0.grid(True)
    ax0.legend()
    ax1.plot(freqs, amp, label="FFT amplitude")
    ax1.set_xlabel("frequency (Hz)")
    ax1.set_ylabel("amplitude")
    ax1.set_title("Single-sided FFT spectrum")
    ax1.grid(True)
    ax1.legend()
    plt.tight_layout()
    plt.show()
    return freqs, amp


def plot_raw_three_one_figure():
    """将 ch1/ch2/ch3 三个通道绘制在同一个图中。"""
    df = pd.read_csv("data.csv")
    for col in ("ch1", "ch2", "ch3"):
        if col not in df.columns:
            raise ValueError(f"data.csv 中未找到列 '{col}'，请确认列名。")

    ch1 = df["ch1"].to_numpy()
    ch2 = df["ch2"].to_numpy()
    ch3 = df["ch3"].to_numpy()
    N = len(ch1)
    x = np.arange(N)

    plt.figure(figsize=(8, 4))
    plt.plot(x, ch1, label="ch1")
    plt.plot(x, ch2, label="ch2")
    plt.plot(x, ch3, label="ch3")
    plt.xlabel("sample index")
    plt.ylabel("amplitude")
    plt.legend()
    plt.grid(True)
    plt.tight_layout()
    plt.show()


if __name__ == "__main__":
    # data_foler = "data"
    #1.
    # csv_file_sin = "./data/sin_2s2s2s2s2s1p25s2p75s2s_40s.csv"
    # csv_file_static = "./data/static_2s2s2s2s2s1p25s2p75s2s_40s.csv"
    #2.
    tail_name =  "1p5s"
    csv_file_sin = "./data/1p50/sin_2s2s2s2s2s1p5s2p5s2s.csv"
    csv_file_static = "./data/1p50/static_2s2s2s2s2s1p5s2p5s2s.csv"

    # main()                     # 完整分析：滤波 + 找 1/4、3/4 相位点

    # plot_segmented_sine(segment_len=260*2)


    # plot_maxima_envelope()

    # file_path ="plot_constellation/data_20260511/data_20260511_223059.csv"
    # plot_raw_and_fft(file_path=file_path, col  = "ch3", fs = 500, fmax = 10)
    

# if __name__ == "__main__":
     


#     # file_path ="plot_constellation/data_20260511/data_20260511_225247.csv"
#     # file_path = "plot_constellation/data_20260511/data_20260512_150313.csv"

#     # file_path = "plot_constellation/data_20260511/16QAM_fs500.csv"
#     # file_path = "plot_constellation/data_20260518/data_20260518_173513.csv"
#     file_path = "plot_constellation/data_20260518/data_20260519_151740.csv"
#     file_path = "plot_constellation/data_20260518/standard_qam16.csv"

#     # plot_raw(file_path, ch = "ch2")                 # 只画单路原始信号

#     # plot_segmented_sine(segment_len=500*2, file_path=file_path)
#     # index, data, qam_index = plot_segmented_sine_group(segment_len=500*2,isPlot=False, file_path=file_path)


#     # 1. 绘制分段图
#     # index, data, qam_index = plot_segmented_qam(segment_len=500*2,isPlot=True, file_path=file_path)
#     index, data, qam_index = plot_segmented_qam_standard(segment_len=500*2,isPlot=True, file_path=file_path, QAM_type= 16)
#     a = 0.46
#     b = 1.16
#     c = sqrt(a**2 * b ** 2)
#     print(f'c = {c}')
#     # index, data, qam_index = plot_segmented_sine_group(segment_len=500*2,isPlot=True, file_path=file_path)

#     # 64QAM
#     # file_path = "plot_constellation/data_20260511/64QAM_fs500.csv"
#     # file_path = "plot_constellation/data_20260518/data_20260518_173513.csv"
#     # index, data, qam_index = plot_segmented_sine_group(segment_len=500*2,isPlot=True, file_path=file_path, QAM_type=64)
#     # index, data, qam_index = plot_segmented_qam(segment_len=500*2,isPlot=True, file_path=file_path, QAM_type=64)

#     # plot_raw(file_path, ch = "ch2")                 # 只画单路原始信号

#     # plt.figure()
#     # for data_index, data_seg in zip(index, data):
#     #     plt.plot(data_index, data_seg)
#     # # plt.plot(index, data)
#     # print(f'index = {qam_index}')
#     # plt.show()

#     # 2. 绘制幅度分段图
    file_path = "plot_constellation/data_20260518/standard_qam16.csv"
    # plot_maxima_points(file_path=file_path, col = "ch3")

    # plot_raw_three(csv_file_sin)           # 三个通道分三幅图
    plot_raw_three(file_path)           # 三个通道分三幅图
#     # plot_raw_three(csv_file_static)           # 三个通道分三幅图

#     # plot_raw_three_one_figure()  # 三个通道画在同一幅图


#     # plot_two_csv(csv_file_sin, csv_file_static, f"sin_{tail_name}", f"static_{tail_name}")
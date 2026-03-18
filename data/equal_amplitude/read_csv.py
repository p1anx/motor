import numpy as np
import pandas as pd
import matplotlib.pyplot as plt


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


def plot_raw():
    """只绘制原始正弦波（不滤波、不标记相位点）。"""
    df = pd.read_csv("data.csv")
    if "ch1" not in df.columns:
        raise ValueError("data.csv 中未找到列 'ch1'，请确认列名。")

    one_period = df["ch2"].to_numpy()
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


def plot_raw_three():
    """绘制三个通道的原始数据（ch1, ch2, ch3），x 轴为采样点索引。"""
    df = pd.read_csv("data.csv")
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
    plt.show()


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
    main()                     # 完整分析：滤波 + 找 1/4、3/4 相位点
    # plot_raw()                 # 只画单路原始信号
    # plot_raw_three()           # 三个通道分三幅图
    # plot_raw_three_one_figure()  # 三个通道画在同一幅图
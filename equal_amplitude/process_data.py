import numpy as np
import pandas as pd
import matplotlib.pyplot as plt


def plot_multi_series_from_csv(
    csv_path: str,
    x_col: str | None = None,
    y_cols: list[str] | None = None,
    title: str = "Multi-series Plot",
) -> None:
    """
    读取 CSV 中多组数据，并绘制在同一张图中。

    csv_path: CSV 路径
    x_col:    作为 x 轴的列名；为 None 时使用行索引
    y_cols:   需要绘制的 y 列；为 None 时自动选择所有数值列
    """
    df = pd.read_csv(csv_path)
    if df.empty:
        raise ValueError(f"CSV 为空: {csv_path}")

    if x_col is not None and x_col not in df.columns:
        raise ValueError(f"CSV 中未找到 x 轴列: {x_col}")

    numeric_cols = df.select_dtypes(include=[np.number]).columns.tolist()
    if not numeric_cols:
        raise ValueError("CSV 中没有可绘制的数值列。")

    if y_cols is None:
        y_cols = numeric_cols.copy()
        if x_col in y_cols:
            y_cols.remove(x_col)
    else:
        missing = [c for c in y_cols if c not in df.columns]
        if missing:
            raise ValueError(f"CSV 中未找到这些 y 列: {missing}")

    if not y_cols:
        raise ValueError("没有可绘制的 y 列，请检查 y_cols 或 x_col。")

    x = df[x_col].to_numpy() if x_col is not None else np.arange(len(df))

    plt.figure(figsize=(10, 5))
    for col in y_cols:
        y = pd.to_numeric(df[col], errors="coerce").to_numpy()
        plt.plot(x, y, label=col, linewidth=1.2)

    plt.xlabel(x_col if x_col is not None else "index")
    plt.ylabel("value")
    plt.title(title)
    plt.grid(True, alpha=0.3)
    plt.legend()
    plt.tight_layout()
    plt.show()


def moving_average_filter(signal: np.ndarray, window_size: int = 9) -> np.ndarray:
    """
    对一维信号做移动平均滤波（低通平滑）。

    signal:      输入信号
    window_size: 滑动窗口大小，建议奇数（自动修正为 >=3 的奇数）
    """
    y = np.asarray(signal, dtype=float)
    if y.size == 0:
        return y

    # 保证窗口是 >=3 的奇数，减少相位偏移并稳定边界行为
    w = max(3, int(window_size))
    if w % 2 == 0:
        w += 1
    if w > y.size:
        w = y.size if y.size % 2 == 1 else max(3, y.size - 1)

    if w <= 1:
        return y.copy()

    kernel = np.ones(w, dtype=float) / w
    pad = w // 2
    y_pad = np.pad(y, (pad, pad), mode="edge")
    y_filt = np.convolve(y_pad, kernel, mode="valid")
    return y_filt


def equal_amplitude_phase_points(
    one_period: np.ndarray,
    num_levels: int,
    start_phase_deg: float = 360.0 * 0.01,
    end_phase_deg: float = 360.0 * 0.99,
) -> tuple[np.ndarray, np.ndarray]:
    """
    在一个周期正弦数据中，求等幅度的相位点（线性插值）。

    one_period: 一周期采样数据（长度 N，对应 0~360 度等间隔）
    num_levels: 需要的等幅度电平数

    返回:
        phase_deg: 每个电平对应的相位（度）
        amps:      对应的幅度值
    """
    y = np.asarray(one_period, dtype=float)
    N = y.size
    if N < 2 or num_levels <= 0:
        return np.array([]), np.array([])

    # 假设采样均匀覆盖 0~360 度
    phases = np.linspace(0.0, 360.0, N, endpoint=False)

    # 只在 [0.01*360, 0.99*360] 的相位窗口内寻找
    mask = (phases >= start_phase_deg) & (phases <= end_phase_deg)
    if not np.any(mask):
        return np.array([]), np.array([])

    phases_win = phases[mask]
    y_win = y[mask]

    # 在窗口内取等幅度等级（去掉最小/最大端点，避免只出现一个交点）
    y_min, y_max = float(y_win.min()), float(y_win.max())
    target_amps = np.linspace(y_min, y_max, num_levels + 2)[1:-1]

    phase_points: list[float] = []
    amps: list[float] = []

    for A in target_amps:
        diff = y_win - A
        crossings: list[float] = []

        # 找出该幅度在一个周期窗口中的所有交点
        for k in range(diff.size - 1):
            d0, d1 = float(diff[k]), float(diff[k + 1])
            x0, x1 = float(phases_win[k]), float(phases_win[k + 1])
            y0, y1 = float(y_win[k]), float(y_win[k + 1])

            # 样本点刚好命中目标幅度
            if d0 == 0.0:
                crossings.append(x0)

            # 相邻采样跨过目标幅度，做线性插值
            if d0 * d1 < 0.0:
                phi = x0 + (A - y0) * (x1 - x0) / (y1 - y0)
                crossings.append(float(phi))

        # 末点恰好命中时补上
        if diff[-1] == 0.0:
            crossings.append(float(phases_win[-1]))

        if not crossings:
            continue

        # 去重并排序后，取首尾两个交点（对应上升沿/下降沿）
        crossings_arr = np.asarray(crossings, dtype=float)
        crossings_arr = np.unique(np.round(crossings_arr, decimals=9))
        crossings_arr.sort()

        if crossings_arr.size >= 2:
            selected = [float(crossings_arr[0]), float(crossings_arr[-1])]
        else:
            selected = [float(crossings_arr[0])]

        for phi in selected:
            phase_points.append(phi)
            amps.append(float(A))

    return np.asarray(phase_points), np.asarray(amps)


def main():
    # 示例：把 CSV 中多组数据画在同一张图

    # 1. 用 pandas 读取 CSV 数据
    df = pd.read_csv("data1.csv")

    # 假设 ch1 是你要处理的那个正弦通道
    if "ch2" not in df.columns:
        raise ValueError("data.csv 中未找到列 'ch1'，请确认列名。")

    one_period = df["ch2"].to_numpy()

    # 2. 先做滤波（窗口可按噪声强度调整）
    filtered_period = moving_average_filter(one_period, window_size=9)

    # 3. 基于滤波后的数据计算等幅度处的相位点
    num_levels = 4  # 例如需要 16 个等幅度点，可按需修改
    phase_deg, amps = equal_amplitude_phase_points(
        one_period=filtered_period,
        num_levels=num_levels,
        start_phase_deg=360.0 * 0.01,
        end_phase_deg=360.0 * 0.99,
    )

    # 4. 打印结果
    for i, (p, a) in enumerate(zip(phase_deg, amps), start=1):
        print(f"Level {i:02d}: phase = {p:.3f} deg, amp = {a:.6f}")

    # 5. 可选：把结果保存到 CSV
    out = pd.DataFrame({"phase_deg": phase_deg, "amp": amps})
    out.to_csv("phase_points.csv", index=False)
    print("等幅度相位点已保存到 phase_points.csv")

    # 6. 可选：画出原始/滤波后正弦和等幅度点
    N = one_period.size
    phases_all = np.linspace(0.0, 360.0, N, endpoint=False)

    plt.figure()
    plt.plot(phases_all, one_period, label="raw", alpha=0.45)
    plt.plot(phases_all, filtered_period, label="filtered", linewidth=1.5)
    plt.scatter(phase_deg, amps, color="red", label="equal-amplitude points")
    plt.xlabel("phase (deg)")
    plt.ylabel("amplitude")
    plt.legend()
    plt.grid(True)
    plt.tight_layout()
    plt.show()


if __name__ == "__main__":
    csv_file = "equal_8points_sin.csv"
    plot_multi_series_from_csv(csv_file, x_col=None, y_cols=None, title="data1.csv")
    # main()


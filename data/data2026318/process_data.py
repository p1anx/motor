import numpy as np
import pandas as pd
import matplotlib.pyplot as plt


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

    # 在窗口内取等幅度等级
    y_min, y_max = float(y_win.min()), float(y_win.max())
    target_amps = np.linspace(y_min, y_max, num_levels)

    phase_points: list[float] = []
    amps: list[float] = []

    for A in target_amps:
        diff = y_win - A
        sign = np.sign(diff)

        # 寻找一次交叉点（上升/下降均可）
        idx = np.where(sign[:-1] * sign[1:] <= 0)[0]
        if idx.size == 0:
            continue

        k = int(idx[0])
        x0, x1 = phases_win[k], phases_win[k + 1]
        y0, y1 = y_win[k], y_win[k + 1]

        if y1 == y0:
            phi = x0
        else:
            # 线性插值求精确相位
            phi = x0 + (A - y0) * (x1 - x0) / (y1 - y0)

        phase_points.append(float(phi))
        amps.append(float(A))

    return np.asarray(phase_points), np.asarray(amps)


def main():
    # 1. 用 pandas 读取 CSV 数据
    df = pd.read_csv("data.csv")

    # 假设 ch1 是你要处理的那个正弦通道
    if "ch2" not in df.columns:
        raise ValueError("data.csv 中未找到列 'ch1'，请确认列名。")

    one_period = df["ch2"].to_numpy()

    # 2. 计算等幅度处的相位点
    num_levels = 4  # 例如需要 16 个等幅度点，可按需修改
    phase_deg, amps = equal_amplitude_phase_points(
        one_period=one_period,
        num_levels=num_levels,
        start_phase_deg=360.0 * 0.01,
        end_phase_deg=360.0 * 0.99,
    )

    # 3. 打印结果
    for i, (p, a) in enumerate(zip(phase_deg, amps), start=1):
        print(f"Level {i:02d}: phase = {p:.3f} deg, amp = {a:.6f}")

    # 4. 可选：把结果保存到 CSV
    out = pd.DataFrame({"phase_deg": phase_deg, "amp": amps})
    out.to_csv("phase_points.csv", index=False)
    print("等幅度相位点已保存到 phase_points.csv")

    # 5. 可选：画出原始正弦和等幅度点
    N = one_period.size
    phases_all = np.linspace(0.0, 360.0, N, endpoint=False)

    plt.figure()
    plt.plot(phases_all, one_period, label="sine")
    plt.scatter(phase_deg, amps, color="red", label="equal-amplitude points")
    plt.xlabel("phase (deg)")
    plt.ylabel("amplitude")
    plt.legend()
    plt.grid(True)
    plt.tight_layout()
    plt.show()


if __name__ == "__main__":
    main()


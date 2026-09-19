"""
sin(θ) 在 θ∈[0°, 90°] 时，y = sin(θ) ∈ [0, 1]。
对 y 轴幅度做 n 等分，求各等分点对应的角度。
"""

import numpy as np
import matplotlib.pyplot as plt


def sin_amplitude_division_angles(
    n_divisions: int,
    include_endpoints: bool = True,
) -> tuple[np.ndarray, np.ndarray]:
    """
    将 sin 在 0°~90° 范围内 y 轴幅度 [0, 1] 做 n 等分，返回各等分点的幅度与角度。

    等分点幅度: y_k = k/n，k = 0, 1, ..., n（含端点时）
    对应角度:   θ_k = arcsin(y_k)，单位：度

    参数:
        n_divisions: 等分数 n（如 2、4、6 表示 2/4/6 等分）
        include_endpoints: 是否包含 y=0(0°)、y=1(90°) 端点

    返回:
        y_levels: 幅度等分点 (长度 n+1 或 n-1)
        theta_deg: 对应角度（度）
    """
    if n_divisions < 1:
        raise ValueError("n_divisions 必须 >= 1")

    if include_endpoints:
        k = np.arange(n_divisions + 1, dtype=float)
    else:
        k = np.arange(1, n_divisions, dtype=float)

    y_levels = k / n_divisions
    # arcsin 输入需在 [-1, 1]，y∈[0,1] 对应 θ∈[0°, 90°]
    theta_deg = np.degrees(np.arcsin(y_levels))
    return y_levels, theta_deg


def sin_amplitude_ratio_angles(
    amplitude_ratios: list[float] | np.ndarray,
    amplitude_max: float = 1.0,
) -> tuple[np.ndarray, np.ndarray, np.ndarray]:
    """
    按相对最大幅度的比例求 sin 在 0°~90° 上对应的角度（非等分）。

    参数:
        amplitude_ratios: 相对最大幅度的比例，如 0.33、0.75、1.0
        amplitude_max:    最大幅度，0°~90° 内 sin 峰值，默认 1.0

    返回:
        ratios:    输入比例（排序后）
        y_levels:  实际幅度 y = ratio * amplitude_max
        theta_deg: 对应角度 θ = arcsin(y / amplitude_max)，度
    """
    ratios = np.asarray(amplitude_ratios, dtype=float).ravel()
    if ratios.size == 0:
        raise ValueError("amplitude_ratios 不能为空")
    if amplitude_max <= 0:
        raise ValueError("amplitude_max 必须 > 0")
    if np.any(ratios < 0) or np.any(ratios > 1):
        raise ValueError("amplitude_ratios 须在 [0, 1] 内（相对最大幅度）")

    ratios = np.sort(ratios)
    y_levels = ratios * amplitude_max
    # y = sin(θ)，峰值 amplitude_max 时 θ=90°
    sin_arg = np.clip(y_levels / amplitude_max, 0.0, 1.0)
    theta_deg = np.degrees(np.arcsin(sin_arg))
    print(f'{ratios}, {y_levels}, {theta_deg}')
    return ratios, y_levels, theta_deg


def plot_sin_amplitude_ratios(
    amplitude_ratios: list[float] | np.ndarray,
    amplitude_max: float = 1.0,
    show: bool = True,
) -> tuple[np.ndarray, np.ndarray, np.ndarray]:
    """绘制 sin 曲线及按指定幅度比例标定的点。"""
    ratios, y_levels, theta_deg = sin_amplitude_ratio_angles(
        amplitude_ratios, amplitude_max=amplitude_max
    )
    theta_curve = np.linspace(0, 90, 500)
    y_curve = amplitude_max * np.sin(np.radians(theta_curve))

    fig, ax = plt.subplots(figsize=(8, 4))
    ax.plot(theta_curve, y_curve, "b-", label="y = A_max·sin(θ)")
    ax.scatter(theta_deg, y_levels, c="red", s=50, zorder=3, label="ratio points")
    for r, yi, ti in zip(ratios, y_levels, theta_deg):
        ax.annotate(
            f"y={yi:.2f}\nθ={ti:.1f}°",
            (ti, yi),
            textcoords="offset points",
            xytext=(6, 6),
            fontsize=8,
        )
    ax.set_xlabel("θ (deg)")
    ax.set_ylabel("amplitude y")
    ax.set_title("sin: angles at given amplitude ratios")
    ax.set_xlim(0, 90)
    ax.set_ylim(0, amplitude_max * 1.05)
    ax.grid(True, alpha=0.4)
    ax.legend()
    plt.tight_layout()
    if show:
        plt.show()
    return ratios, y_levels, theta_deg


def print_amplitude_divisions(n_list: list[int] | None = None) -> None:
    """打印多种等分数下的幅度与角度对照表。"""
    if n_list is None:
        n_list = [2, 4, 6, 8]

    for n in n_list:
        y, theta = sin_amplitude_division_angles(n, include_endpoints=True)
        print(f"\n--- {n} 等分 (y = k/{n}, θ = arcsin(y)) ---")
        for yi, ti in zip(y, theta):
            print(f"  y = {yi:.6f}  ->  θ = {ti:.6f}°")


def ideal_square_qam_constellation(M: int, normalize_peak: bool = True):
    """
    生成标准方形 M-QAM（M=4,16,64...）的理想星座点，并转为幅度、相位。

    方形 M-QAM：每维 √M 个电平，电平取奇数 ±1, ±3, ... ±(√M-1)，
    共 M 个复数点 z = I + jQ。
    默认按峰值归一化：max(|z|) = 1（16-QAM 最外点幅度为 1）。

    返回:
        z:           复数星座点
        amplitudes:  |z|
        phases_rad:  arg(z)，弧度
        phases_deg:  arg(z)，度
        I, Q:        同相/正交分量
    """
    if M < 4 or int(np.sqrt(M)) ** 2 != M:
        raise ValueError("M 须为完全平方数，如 4, 16, 64")

    n_side = int(np.sqrt(M))
    levels = np.arange(-(n_side - 1), n_side, 2, dtype=float)  # 如 16-QAM: [-3,-1,1,3]
    I, Q = np.meshgrid(levels, levels)
    z = (I + 1j * Q).ravel()

    if normalize_peak:
        peak = float(np.max(np.abs(z)))
        if peak > 0:
            z = z / peak

    amplitudes = np.abs(z)
    phases_rad = np.angle(z)
    phases_deg = np.degrees(phases_rad)
    return z, amplitudes, phases_rad, phases_deg, I.ravel(), Q.ravel()


def ideal_qpsk_constellation(normalize_peak: bool = True):
    """
    4-QAM（QPSK）：4 点等幅度，相位相差 90°。
    常用点: exp(j(π/4 + k·π/2)), k=0,1,2,3
    """
    k = np.arange(4)
    z = np.exp(1j * (np.pi / 4 + k * np.pi / 2))
    if normalize_peak:
        peak = float(np.max(np.abs(z)))
        if peak > 0:
            z = z / peak
    amplitudes = np.abs(z)
    phases_rad = np.angle(z)
    phases_deg = np.degrees(phases_rad)
    return z, amplitudes, phases_rad, phases_deg


def print_qam_constellation(M: int) -> None:
    """打印 M-QAM 各点的 I/Q、幅度、相位。"""
    if M == 4:
        z, amp, phr, phd = ideal_qpsk_constellation()
        title = "4-QAM (QPSK)"
    else:
        z, amp, phr, phd, _, _ = ideal_square_qam_constellation(M)
        title = f"{M}-QAM (方形)"

    print(f"\n=== {title}, {len(z)} 点 ===")
    for idx in range(len(z)):
        print(
            f"  P{idx}: I={np.real(z[idx]):+.4f}, Q={np.imag(z[idx]):+.4f}, "
            f"A={amp[idx]:.4f}, θ={phd[idx]:.2f}°"
        )


def _get_qam_points(M: int):
    if M == 4:
        z, amp, phr, phd = ideal_qpsk_constellation()
        title = "4-QAM (QPSK)"
    else:
        z, amp, phr, phd, _, _ = ideal_square_qam_constellation(M)
        title = f"{M}-QAM (square)"
    return z, amp, phd, title


def plot_sin_amplitude_divisions(
    n_divisions: int,
    show: bool = True,
) -> tuple[np.ndarray, np.ndarray]:
    """绘制 0°~90° 正弦曲线及幅度等分点。"""
    y_levels, theta_deg = sin_amplitude_division_angles(n_divisions, include_endpoints=True)
    theta_curve = np.linspace(0, 90, 500)
    y_curve = np.sin(np.radians(theta_curve))

    fig, ax = plt.subplots(figsize=(8, 4))
    ax.plot(theta_curve, y_curve, "b-", label="y = sin(θ)")
    ax.scatter(theta_deg, y_levels, c="red", s=50, zorder=3, label="division points")
    for yi, ti in zip(y_levels, theta_deg):
        ax.annotate(
            f"y={yi:.2f}\nθ={ti:.1f}°",
            (ti, yi),
            textcoords="offset points",
            xytext=(6, 6),
            fontsize=8,
        )
    ax.set_xlabel("θ (deg)")
    ax.set_ylabel("amplitude y")
    ax.set_title(f"sin amplitude divisions (n={n_divisions})")
    ax.set_xlim(0, 90)
    ax.set_ylim(0, 1.05)
    ax.grid(True, alpha=0.4)
    ax.legend()
    plt.tight_layout()
    if show:
        plt.show()
    return y_levels, theta_deg


def plot_qam_constellation(
    M: int,
    annotate: bool = True,
    show: bool = True,
):
    """
    绘制 QAM 星座图（I-Q），并标注各点幅度与相位。
    """
    z, amp, phd, title = _get_qam_points(M)
    I = np.real(z)
    Q = np.imag(z)

    fig, axes = plt.subplots(1, 2, figsize=(12, 5))

    ax0 = axes[0]
    ax0.axhline(0, color="gray", lw=0.5, ls="--")
    ax0.axvline(0, color="gray", lw=0.5, ls="--")
    ax0.scatter(I, Q, s=60, c="tab:blue", zorder=3)
    ax0.scatter(0, 0, s=40, c="red", marker="x", label="origin")
    if annotate:
        for i in range(len(z)):
            ax0.annotate(
                f"P{i}\nA={amp[i]:.2f}\nθ={phd[i]:.1f}°",
                (I[i], Q[i]),
                xytext=(I[i] + 0.08, Q[i] + 0.08),
                fontsize=7,
                arrowprops=dict(arrowstyle="->", lw=0.6),
            )
    ax0.set_aspect("equal", adjustable="box")
    ax0.set_xlabel("I")
    ax0.set_ylabel("Q")
    ax0.set_title(f"{title} constellation")
    ax0.grid(True, alpha=0.4)
    ax0.legend()

    ax1 = axes[1]
    idx = np.arange(len(z))
    width = 0.35
    ax1.bar(idx - width / 2, amp, width, label="|A|", color="tab:orange")
    ax1_twin = ax1.twinx()
    ax1_twin.plot(idx, phd, "go-", label="θ (deg)", markersize=6)
    ax1.set_xlabel("point index")
    ax1.set_ylabel("amplitude")
    ax1_twin.set_ylabel("phase (deg)")
    ax1.set_title("Amplitude & phase per point")
    ax1.set_xticks(idx)
    ax1.set_xticklabels([f"P{i}" for i in idx], fontsize=8)
    ax1.grid(True, axis="y", alpha=0.4)
    lines1, labels1 = ax1.get_legend_handles_labels()
    lines2, labels2 = ax1_twin.get_legend_handles_labels()
    ax1.legend(lines1 + lines2, labels1 + labels2, loc="upper right")

    plt.tight_layout()
    if show:
        plt.show()
    return z, amp, phd


def plot_qam_compare(M_list: list[int] | None = None, show: bool = True):
    """在一张图里对比多个 M 的星座点（仅 I-Q）。"""
    if M_list is None:
        M_list = [4, 16]

    fig, axes = plt.subplots(1, len(M_list), figsize=(5 * len(M_list), 5))
    if len(M_list) == 1:
        axes = [axes]

    for ax, M in zip(axes, M_list):
        z, amp, phd, title = _get_qam_points(M)
        ax.axhline(0, color="gray", lw=0.5, ls="--")
        ax.axvline(0, color="gray", lw=0.5, ls="--")
        ax.scatter(np.real(z), np.imag(z), s=50, c="tab:blue")
        for i in range(len(z)):
            ax.annotate(f"P{i}", (np.real(z[i]), np.imag(z[i])), fontsize=7, xytext=(4, 4),
                        textcoords="offset points")
        ax.set_aspect("equal", adjustable="box")
        ax.set_xlabel("I")
        ax.set_ylabel("Q")
        ax.set_title(title)
        ax.grid(True, alpha=0.4)

    plt.tight_layout()
    if show:
        plt.show()
def getCycle(cycle = 1000):
    # a, b, c = sin_amplitude_ratio_angles([1, 0.75, 0.33])
    angle_list = [18.4, 45, 45, 71.6, 108.4]
    amp_list =   [0.33, 0.75, 0.75, 1.0, 0.75, 0.33, 0.75, 1.0, 0.75, 0.33,0.75, 1.0, 0.75, 0.33, 0.75, 1.0]
    phase_list = [45,   18.4, 71.6, 45, 108.4, 135,  161.6,135, 198.4,225, 251.6,225, 288.4, 315, 341.6,315]


    all_segment = []
    for angle in phase_list:
        cycle_segment = angle / 360 * cycle

        cycle_segment = int(cycle_segment)
        all_segment.append(cycle_segment)
    print(f'all = {all_segment}')



if __name__ == "__main__":
    plot_sin_amplitude_divisions(4, show=False)
    plot_sin_amplitude_ratios([0.33, 0.75, 1])
    # plot_qam_constellation(4, show=False)

    plot_qam_constellation(16, show=False)

    plt.show()
    # plot_qam_compare([4, 16], show=True)

    getCycle()



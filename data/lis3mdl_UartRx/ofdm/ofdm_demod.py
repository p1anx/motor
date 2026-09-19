"""逐符号 OFDM 解调 demo：两块永磁体作为两个子载波。

物理模型:
    磁体1 以 F1=1 Hz 旋转 -> 子载波1
    磁体2 以 F2=2 Hz 旋转 -> 子载波2
    子载波间隔 dF = F2 - F1 = 1 Hz -> 符号周期 T = 1/dF = 1 s

接收端处理:
    1. 单轴数据按 T=1s(500采样点)切成 OFDM 符号
    2. 每个符号做 FFT
    3. 取 F1、F2 对应频率 bin 的复幅度(幅度+相位)
    4. 输出每个符号、每个子载波的幅度与相位

用法:
    python ofdm_demod.py                     # 默认数据文件
    python ofdm_demod.py path/to/file.csv    # 指定数据文件
"""

import os
import sys

import matplotlib

# 使用支持中文的字体，避免图中中文显示为方框
matplotlib.rcParams["font.sans-serif"] = [
    "Noto Sans CJK JP", "Droid Sans Fallback", "DejaVu Sans"
]
matplotlib.rcParams["axes.unicode_minus"] = False

import matplotlib.pyplot as plt
import numpy as np
import pandas as pd

# ==================== 可调参数 ====================
FS = 500.0   # 采样频率 (Hz)，按需修改
F1 = 1.0     # 子载波1 频率 (磁体1 旋转频率)
F2 = 2.0     # 子载波2 频率 (磁体2 旋转频率)
CHANNEL = "ch1"  # 用于解调的单轴通道 (ch1/ch2/ch3)
PHASE_SUBCARRIER = "1Hz"  # 单独绘制相位-符号图的子载波 ("1Hz"/"2Hz")
DEFAULT_FILE = "data/data_20260708_225003.csv"
# ================================================

DF = F2 - F1                      # 子载波间隔
T_SYMBOL = 1.0 / DF               # 符号周期 (s)
N_PER_SYMBOL = int(round(FS * T_SYMBOL))  # 每符号采样点数


def read_csv(filepath: str) -> pd.DataFrame:
    df = pd.read_csv(filepath, sep=",")
    cols = [c for c in ("ch1", "ch2", "ch3") if c in df.columns]
    return df[cols]


def demodulate_symbol(symbol: np.ndarray, fs: float) -> dict[str, complex]:
    """对单个符号做 FFT，提取 F1/F2 子载波的复幅度。

    返回值 key 为子载波频率(字符串)，value 为复幅度(含幅度与相位)。
    """
    x = symbol - np.mean(symbol)      # 去直流(地磁场 + 磁体静态偏移)
    n = len(x)
    X = np.fft.rfft(x)                # 单边谱，复值
    freqs = np.fft.rfftfreq(n, d=1.0 / fs)

    result = {}
    for fc in (F1, F2):
        k = int(round(fc * n / fs))   # 子载波对应的 FFT bin
        result[f"{fc:g}Hz"] = X[k] * 2.0 / n  # 复幅度(正幅度归一)
    return result


def plot_phase_across_symbols(
    rows: list[dict[str, complex]],
    fc: str,
    save_path: str | None = None,
    show: bool = True,
) -> None:
    """单独绘制同一子载波不同符号的相位变化。

    左图: 相位(deg) 随符号序号变化，逐点标注数值;
    右图: 各符号复幅度在单位圆上的位置(箭头方向=相位)，颜色按符号先后;
    标题: 平均每符号相位漂移量(换算为频偏)。
    """
    n = len(rows)
    idx = np.arange(n)
    z = np.array([row[fc] for row in rows])
    ph = np.angle(z, deg=True)

    # 每符号相位增量(漂移)，归一化到 (-180, 180]
    dphi = np.angle(np.exp(1j * np.diff(np.angle(z))), deg=True)
    mean_dphi = float(np.mean(dphi))
    cfo_hz = mean_dphi / 360.0 / T_SYMBOL

    colors = plt.cm.viridis(np.linspace(0, 1, n))

    fig, axes = plt.subplots(1, 2, figsize=(12, 5))
    fig.suptitle(
        f"子载波 {fc} 不同符号的相位变化"
        f"  (平均每符号漂移 {mean_dphi:+.2f}° ≈ 频偏 {cfo_hz:+.4f} Hz)",
        fontsize=12,
    )

    # (1) 相位 vs 符号
    ax = axes[0]
    ax.plot(idx, ph, "-o", markersize=6, color="tab:blue")
    for i, p in enumerate(ph):
        ax.annotate(
            f"{p:.1f}°", (idx[i], ph[i]),
            textcoords="offset points", xytext=(0, 12),
            ha="center", fontsize=8,
        )
    ax.set_xlabel("Symbol index")
    ax.set_ylabel("Phase (deg)")
    ax.set_xticks(idx)
    ax.grid(True, alpha=0.4)

    # (2) 单位圆上的星座点(相位旋转方向)
    ax = axes[1]
    th = np.linspace(0, 2 * np.pi, 300)
    ax.plot(np.cos(th), np.sin(th), color="gray", linewidth=0.6, alpha=0.6)
    ax.axhline(0, color="gray", linewidth=0.5)
    ax.axvline(0, color="gray", linewidth=0.5)
    for i in range(n):
        ang = np.angle(z[i])
        u = np.array([np.cos(ang), np.sin(ang)])
        ax.arrow(
            0, 0, u[0], u[1], head_width=0.05, head_length=0.08,
            length_includes_head=True, color=colors[i], alpha=0.85,
        )
        ax.annotate(
            str(i), (u[0], u[1]), textcoords="offset points",
            xytext=(6, 6), fontsize=9, color=colors[i],
        )
    ax.set_xlim(-1.35, 1.35)
    ax.set_ylim(-1.35, 1.35)
    ax.set_aspect("equal")
    ax.set_xlabel("I (real)")
    ax.set_ylabel("Q (imag)")
    ax.set_title("相位在单位圆上的旋转")

    plt.tight_layout()
    if save_path is not None:
        plt.savefig(save_path, dpi=150)
        print(f"已保存图片: {save_path}")
    if show:
        plt.show()


def plot_phase_drift_comparison(
    rows: list[dict[str, complex]],
    keys: list[str],
    save_path: str | None = None,
    show: bool = True,
) -> None:
    """同一个图对比两个子载波的相位(漂移)。

    左图: 两子载波原始相位(展开后)随符号变化;
    右图: 相对符号0的相位漂移(两载波都从0开始), 直观对比漂移速率。
    """
    n = len(rows)
    idx = np.arange(n)
    colors = {"1Hz": "tab:blue", "2Hz": "tab:orange"}

    fig, axes = plt.subplots(1, 2, figsize=(12, 5))

    # 左: 原始相位(展开)
    ax = axes[0]
    for k in keys:
        z = np.array([row[k] for row in rows])
        ph = np.unwrap(np.angle(z)) * 180.0 / np.pi
        ax.plot(idx, ph, "-o", markersize=5, label=k, color=colors.get(k))
    ax.set_xlabel("Symbol index")
    ax.set_ylabel("Phase (deg, unwrapped)")
    ax.set_title("各子载波原始相位(展开)")
    ax.set_xticks(idx)
    ax.legend()
    ax.grid(True, alpha=0.4)

    # 右: 相对符号0的漂移
    ax = axes[1]
    print("\n相位漂移率(线性拟合):")
    for k in keys:
        z = np.array([row[k] for row in rows])
        ph = np.unwrap(np.angle(z)) * 180.0 / np.pi
        drift = ph - ph[0]
        slope = float(np.polyfit(idx, drift, 1)[0])  # 度/符号
        cfo_hz = slope / 360.0 / T_SYMBOL
        label = f"{k}  ({slope:+.2f}°/符号 ≈ {cfo_hz:+.4f} Hz)"
        ax.plot(idx, drift, "-o", markersize=5, label=label, color=colors.get(k))
        print(f"  {k}: {slope:+.3f}°/符号 = {cfo_hz:+.5f} Hz")
    ax.set_xlabel("Symbol index")
    ax.set_ylabel("Phase drift (deg)")
    ax.set_title("相对符号0的相位漂移")
    ax.set_xticks(idx)
    ax.axhline(0, color="gray", linewidth=0.5)
    ax.legend()
    ax.grid(True, alpha=0.4)

    fig.suptitle("两个子载波相位漂移对比", fontsize=13)
    plt.tight_layout()
    if save_path is not None:
        plt.savefig(save_path, dpi=150)
        print(f"已保存图片: {save_path}")
    if show:
        plt.show()


def main() -> None:
    script_dir = os.path.dirname(os.path.abspath(__file__))
    filepath = sys.argv[1] if len(sys.argv) > 1 else os.path.join(script_dir, DEFAULT_FILE)

    df = read_csv(filepath)
    x = df[CHANNEL].to_numpy(dtype=float)
    n_total = len(x)

    n_symbols = n_total // N_PER_SYMBOL
    print(f"文件: {filepath}")
    print(f"通道: {CHANNEL} | 采样率={FS} Hz | 每符号={N_PER_SYMBOL}点 "
          f"(T={T_SYMBOL}s) | 符号数={n_symbols}\n")

    t = np.arange(n_total) / FS
    sym_bounds = [i * N_PER_SYMBOL / FS for i in range(n_symbols + 1)]

    # 逐符号解调
    f1_keys = [f"{F1:g}Hz", f"{F2:g}Hz"]
    rows = []
    for s in range(n_symbols):
        seg = x[s * N_PER_SYMBOL:(s + 1) * N_PER_SYMBOL]
        c = demodulate_symbol(seg, FS)
        rows.append({k: c[k] for k in f1_keys})

    # 打印表格
    print(f"{'符号':>4} | {'子载波':>6} | {'幅度':>8} | {'相位(deg)':>10}")
    print("-" * 42)
    for s, row in enumerate(rows):
        for fc in f1_keys:
            v = row[fc]
            amp = abs(v)
            ph = np.angle(v, deg=True)
            print(f"{s:>4} | {fc:>6} | {amp:>8.4f} | {ph:>10.2f}")

    # 绘图
    fig, axes = plt.subplots(2, 1, figsize=(11, 7))

    # (1) 时域波形 + 符号边界
    ax = axes[0]
    ax.plot(t, x, linewidth=0.6)
    for b in sym_bounds:
        ax.axvline(b, color="r", linestyle="--", linewidth=0.8, alpha=0.6)
    ax.set_xlabel("Time (s)")
    ax.set_ylabel(f"{CHANNEL}")
    ax.set_title(f"{CHANNEL} 波形与符号边界 (T={T_SYMBOL}s)")
    ax.grid(True, alpha=0.4)

    # (2) 星座图 (复平面)
    ax = axes[1]
    colors = {"1Hz": "tab:blue", "2Hz": "tab:orange"}
    for fc in f1_keys:
        pts = np.array([row[fc] for row in rows])
        ax.scatter(pts.real, pts.imag, label=fc, s=45, color=colors.get(fc))
        ax.scatter(0, 0, marker="+", color="gray")
    ax.axhline(0, color="gray", linewidth=0.5)
    ax.axvline(0, color="gray", linewidth=0.5)
    ax.set_xlabel("I (real)")
    ax.set_ylabel("Q (imag)")
    ax.set_title("星座图 (各符号复幅度)")
    ax.axis("equal")
    ax.legend()
    ax.grid(True, alpha=0.4)

    plt.tight_layout()
    save_path = filepath.rsplit(".", 1)[0] + "_ofdm_demod.png"
    plt.savefig(save_path, dpi=150)
    print(f"\n已保存图片: {save_path}")

    # 单独绘制同一子载波不同符号的相位变化
    phase_save_path = filepath.rsplit(".", 1)[0] + "_phase_across_symbols.png"
    plot_phase_across_symbols(
        rows, PHASE_SUBCARRIER, save_path=phase_save_path, show=False
    )

    # 同一个图对比两个子载波的相位漂移
    cmp_save_path = filepath.rsplit(".", 1)[0] + "_phase_drift_comparison.png"
    plot_phase_drift_comparison(rows, f1_keys, save_path=cmp_save_path, show=False)

    plt.show()


if __name__ == "__main__":
    main()

"""绘制三通道数据的原始时域图与 FFT 频谱图。

用法:
    python plot_raw_fft.py                     # 使用默认数据文件
    python plot_raw_fft.py path/to/file.csv    # 指定数据文件
"""

import os
import sys
from typing import Optional

import matplotlib.pyplot as plt
import numpy as np
import pandas as pd

# ==================== 可调参数 ====================
FS = 500.0          # 采样频率 (Hz)，按需修改
FFT_FREQ_MAX = 10.0  # FFT 频谱图频率显示上限 (Hz)，按需修改
DEFAULT_FILE = "data/data_20260708_225003.csv"
# ================================================


def read_csv(filepath: str) -> pd.DataFrame:
    """读取 CSV，仅保留 ch1/ch2/ch3 三列。"""
    df = pd.read_csv(filepath, sep=",")
    cols = [c for c in ("ch1", "ch2", "ch3") if c in df.columns]
    return df[cols]


def compute_fft(x: np.ndarray, fs: float) -> tuple[np.ndarray, np.ndarray]:
    """计算单边幅度谱，返回 (freq, magnitude)。"""
    n = len(x)
    # 去均值，避免直流分量淹没频谱
    x = x - np.mean(x)
    # 加汉宁窗，减小频谱泄漏
    window = np.hanning(n)
    spectrum = np.fft.rfft(x * window)
    magnitude = np.abs(spectrum) / (n / 2)
    freq = np.fft.rfftfreq(n, d=1.0 / fs)
    return freq, magnitude


def plot_raw_and_fft(
    df: pd.DataFrame,
    fs: float,
    title: str = "",
    save_path: Optional[str] = None,
    show: bool = True,
) -> None:
    """画出原始时域图（左列）与 FFT 频谱图（右列）。"""
    ch_names = list(df.columns)
    n_channels = len(ch_names)
    t = np.arange(len(df)) / fs

    fig, axes = plt.subplots(n_channels, 2, figsize=(12, 3 * n_channels + 1))
    if n_channels == 1:
        axes = axes.reshape(1, 2)
    for i, ch in enumerate(ch_names):
        data = df[ch].to_numpy(dtype=float)

        # 原始时域图
        ax_t = axes[i, 0]
        ax_t.plot(t, data, linewidth=0.6)
        ax_t.set_ylabel("Value")
        ax_t.grid(True, alpha=0.4)

        # FFT 频谱图
        freq, mag = compute_fft(data, fs)
        ax_f = axes[i, 1]
        ax_f.plot(freq, mag, linewidth=0.6)
        ax_f.set_xlabel("Frequency (Hz)")
        ax_f.set_ylabel("Magnitude")
        ax_f.grid(True, alpha=0.4)
        ax_f.set_xlim(0, FFT_FREQ_MAX)
        ax_f.set_xticks(np.arange(0, FFT_FREQ_MAX + 1, 1))

    axes[-1, 0].set_xlabel("Time (s)")
    plt.tight_layout()

    if save_path is not None:
        plt.savefig(save_path, dpi=150)
        print(f"已保存图片: {save_path}")
    if show:
        plt.show()


if __name__ == "__main__":
    # 默认文件路径相对脚本所在目录解析，避免依赖当前工作目录
    script_dir = os.path.dirname(os.path.abspath(__file__))
    if len(sys.argv) > 1:
        filepath = sys.argv[1]
    else:
        filepath = os.path.join(script_dir, DEFAULT_FILE)

    df = read_csv(filepath)
    print(f"读取: {filepath}  (样本数={len(df)}, 采样率={FS} Hz)")

    save_path = filepath.rsplit(".", 1)[0] + "_raw_fft.png"
    plot_raw_and_fft(df, fs=FS, title=filepath, save_path=save_path, show=True)

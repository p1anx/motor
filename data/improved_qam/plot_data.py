"""绘制 improved_qam 目录下的三通道 CSV 数据。

用法：
    python plot_data.py                       # 绘制默认文件 data_20260709_021512.csv
    python plot_data.py 某文件.csv            # 绘制指定文件

输出：
    <文件名>_time.png           三通道时域波形（堆叠子图）
    <文件名>_constellation.png  通道散点图（ch1-ch2、ch1-ch3，观察 I/Q 星座）
"""
import sys
from pathlib import Path

import numpy as np
import pandas as pd
import matplotlib.pyplot as plt


def load_csv(path: str) -> pd.DataFrame:
    df = pd.read_csv(path)
    for col in ("ch1", "ch2", "ch3"):
        if col not in df.columns:
            raise ValueError(f"CSV 需包含列 {col}，实际列为 {list(df.columns)}")
    return df


def plot_time(df: pd.DataFrame, out_path: str) -> None:
    n = len(df)
    x = np.arange(n)

    fig, axes = plt.subplots(3, 1, figsize=(10, 8), sharex=True)

    for ax, col, color in zip(
        axes,
        ("ch1", "ch2", "ch3"),
        ("#1f77b4", "#d62728", "#2ca02c"),
    ):
        ax.plot(x, df[col], color=color, lw=0.6, label=col)
        ax.set_ylabel(col)
        ax.legend(loc="upper right")
        ax.grid(True, alpha=0.3)

    axes[-1].set_xlabel("sample index")
    fig.suptitle(f"time-domain  {Path(out_path).stem}")
    plt.tight_layout()
    plt.savefig(out_path, dpi=150)
    plt.show()
    # plt.close(fig)
    print(f"saved: {out_path}")


def plot_constellation(df: pd.DataFrame, out_path: str, stride: int = 1) -> None:
    d = df.iloc[::stride]

    fig, axes = plt.subplots(1, 2, figsize=(12, 5))

    axes[0].scatter(d["ch1"], d["ch2"], s=2, c="#1f77b4", alpha=0.6, linewidths=0)
    axes[0].set_xlabel("ch1")
    axes[0].set_ylabel("ch2")
    axes[0].set_title("ch1 vs ch2")
    axes[0].grid(True, alpha=0.3)

    axes[1].scatter(d["ch1"], d["ch3"], s=2, c="#d62728", alpha=0.6, linewidths=0)
    axes[1].set_xlabel("ch1")
    axes[1].set_ylabel("ch3")
    axes[1].set_title("ch1 vs ch3")
    axes[1].grid(True, alpha=0.3)

    fig.suptitle(f"constellation  {Path(out_path).stem}  (stride={stride})")
    plt.tight_layout()
    plt.savefig(out_path, dpi=150)
    plt.close(fig)
    print(f"saved: {out_path}")


def main() -> None:
    path = sys.argv[1] if len(sys.argv) > 1 else "data_20260709_021512.csv"
    stem = Path(path).stem

    df = load_csv(path)
    print(f"loaded: {path}  rows={len(df)}  cols={list(df.columns)}")

    plot_time(df, f"{stem}_time.png")
    # plot_constellation(df, f"{stem}_constellation.png")
    plt.show()


if __name__ == "__main__":
    main()

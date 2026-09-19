import glob
import os
from typing import Optional, Union

import matplotlib.pyplot as plt
import numpy as np
import pandas as pd


def read_data_test(
    folder: str = "data_test",
    filename: Optional[str] = None,
    as_array: bool = True,
) -> Union[pd.DataFrame, dict[str, np.ndarray], dict[str, dict[str, np.ndarray]]]:
    """
    读取 data_test 文件夹中的 CSV 数据文件。

    每个 CSV 文件是逗号分隔的，包含三列 ch1, ch2, ch3。

    Parameters
    ----------
    folder : 数据文件夹路径，默认为 "data_test"
    filename : 指定读取单个文件。若为 None，则读取文件夹中所有 .csv 文件。
    as_array : True 时返回 numpy 数组，False 时返回 pandas DataFrame

    Returns
    -------
    - 读取单个文件时:
        as_array=True  -> dict[str, np.ndarray]  {"ch1": array, "ch2": array, "ch3": array}
        as_array=False -> pd.DataFrame (列: ch1, ch2, ch3)

    - 读取所有文件时:
        as_array=True  -> dict[str, dict[str, np.ndarray]]  {文件名: {"ch1": array, ...}}
        as_array=False -> dict[str, pd.DataFrame]           {文件名: DataFrame}
    """
    if filename is not None:
        csv_path = os.path.join(folder, filename)
        return _read_single_csv(csv_path, as_array)

    # 读取所有 .csv 文件
    csv_files = sorted(glob.glob(os.path.join(folder, "*.csv")))
    if not csv_files:
        print(f"在 {folder} 中未找到 CSV 文件")
        return {}

    result: dict = {}
    for f in csv_files:
        basename = os.path.basename(f)
        result[basename] = _read_single_csv(f, as_array)

    return result


def _read_single_csv(
    filepath: str,
    as_array: bool = True,
) -> Union[pd.DataFrame, dict[str, np.ndarray]]:
    """读取单个 CSV 文件，返回 DataFrame 或 dict of arrays."""
    df = pd.read_csv(filepath, sep=",")
    # 只保留 ch1, ch2, ch3 列
    cols = [c for c in df.columns if c in ("ch1", "ch2", "ch3")]
    df = df[cols]

    if as_array:
        return {
            "ch1": df["ch1"].to_numpy(dtype=float),
            "ch2": df["ch2"].to_numpy(dtype=float),
            "ch3": df["ch3"].to_numpy(dtype=float),
        }
    else:
        return df


def list_csv_files(folder: str = "data_test") -> list[str]:
    """列出 data_test 文件夹中所有 CSV 文件名。"""
    csv_files = sorted(glob.glob(os.path.join(folder, "*.csv")))
    return [os.path.basename(f) for f in csv_files]


# ==================== 绘图函数 ====================

def plot_time_domain(
    ch1: np.ndarray,
    ch2: np.ndarray,
    ch3: np.ndarray,
    fs: float = 1.0,
    title: str = "Time Domain",
    save_path: Optional[str] = None,
    show: bool = True,
) -> None:
    """
    绘制三通道时域波形（三行子图）。

    Parameters
    ----------
    ch1, ch2, ch3 : 三个通道的数据
    fs : 采样频率 (Hz)，用于生成时间轴
    title : 图表总标题
    save_path : 若提供路径，保存图片
    show : 是否显示图像
    """
    n = len(ch1)
    t = np.arange(n) / fs

    fig, axes = plt.subplots(3, 1, figsize=(10, 9), sharex=True)
    fig.suptitle(title, fontsize=14)

    channels = [(ch1, "ch1"), (ch2, "ch2"), (ch3, "ch3")]
    for ax, (data, ch_name) in zip(axes, channels):
        ax.plot(t, data, linewidth=0.6)
        ax.set_ylabel("Value")
        ax.set_title(ch_name)
        ax.grid(True, alpha=0.4)

    axes[-1].set_xlabel("Time (s)")
    plt.tight_layout()

    if save_path is not None:
        plt.savefig(save_path)
    if show:
        plt.show()


if __name__ == "__main__":
    save_dir = "data_test"

    filenames = [
        "data_20260709_021512.csv",
        "data_20260709_014806.csv",
    ]
    fs = 500.0

    # 读取两个文件
    datasets = []
    for f in filenames:
        data = read_data_test(folder=save_dir, filename=f)
        if isinstance(data, pd.DataFrame):
            print(f"错误: {f} 数据格式异常")
            exit(1)
        datasets.append((f, data))

    # 3 行（通道）× 2 列（文件）子图
    ch_names = ["ch1", "ch2", "ch3"]
    fig, axes = plt.subplots(3, 2, figsize=(14, 10), sharex="col", sharey="row")
    fig.suptitle("Time Domain Comparison", fontsize=14)

    for col, (filename, data) in enumerate(datasets):
        label = os.path.splitext(filename)[0]
        n = len(data["ch1"])
        t = np.arange(n) / fs
        for row, ch in enumerate(ch_names):
            ax = axes[row, col]
            ax.plot(t, data[ch], linewidth=0.6)
            ax.grid(True, alpha=0.4)
            if row == 0:
                ax.set_title(label, fontsize=10)
            if col == 0:
                ax.set_ylabel(ch)

    axes[-1, 0].set_xlabel("Time (s)")
    axes[-1, 1].set_xlabel("Time (s)")
    plt.tight_layout()
    plt.show()

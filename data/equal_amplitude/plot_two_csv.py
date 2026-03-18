"""
将 30_20_20_20.csv 与 sin_30_20_20_20.csv 绘制在同一图中，同一通道在同一子图。
"""
import numpy as np
import pandas as pd
import matplotlib.pyplot as plt


# def plot_two_csv(
#     file1: str = "10_10_10_10.csv",
#     file2: str = "sin_10_10_10_10.csv",
#     label1: str = "10_10_10_10",
#     label2: str = "sin_10_10_10_10",
# ) -> None:
def plot_two_csv(
    file1: str = "30_20_20_20.csv",
    file2: str = "sin_30_20_20_20.csv",
    label1: str = "30_20_20_20",
    label2: str = "sin_30_20_20_20",
) -> None:
    df1 = pd.read_csv(file1)
    df2 = pd.read_csv(file2)

    for col in ("ch1", "ch2", "ch3"):
        if col not in df1.columns or col not in df2.columns:
            raise ValueError(f"两个 CSV 均需包含列 {col}")

    n1 = len(df1)
    n2 = len(df2)
    x1 = np.arange(n1)
    x2 = np.arange(n2)

    fig, axes = plt.subplots(3, 1, figsize=(8, 8), sharex=True)

    axes[0].plot(x1, df1["ch1"], label=label1)
    axes[0].plot(x2, df2["ch1"], label=label2)
    axes[0].set_ylabel("ch1")
    axes[0].legend()
    axes[0].grid(True)

    axes[1].plot(x1, df1["ch2"], label=label1)
    axes[1].plot(x2, df2["ch2"], label=label2)
    axes[1].set_ylabel("ch2")
    axes[1].legend()
    axes[1].grid(True)

    axes[2].plot(x1, df1["ch3"], label=label1)
    axes[2].plot(x2, df2["ch3"], label=label2)
    axes[2].set_xlabel("sample index")
    axes[2].set_ylabel("ch3")
    axes[2].legend()
    axes[2].grid(True)

    plt.tight_layout()
    plt.show()


if __name__ == "__main__":
    # file1 = "10_10_10_10.csv"
    # file2 = "sin_10_10_10_10.csv"
    # label1 = "10_10_10_10"
    # label2 = "sin_10_10_10_10"

    file1 = "sin_30_20_20_20.csv"
    file2 = "sin_10_10_10_10.csv"
    label1 = "sin_30_20_20_20"
    label2 = "sin_10_10_10_10"

    plot_two_csv(file1, file2, label1, label2)
    # plot_two_csv()

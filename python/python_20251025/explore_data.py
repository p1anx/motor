#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
数据探索脚本
深入分析CSV数据的特征和模式
"""

import pandas as pd
import numpy as np
import matplotlib.pyplot as plt
import seaborn as sns
from scipy import stats
import os

def explore_csv_data(csv_file, sampling_rate=250):
    """
    探索CSV数据特征
    
    Args:
        csv_file (str): CSV文件路径
        sampling_rate (float): 采样率 (Hz)
    """
    # 检查文件是否存在
    if not os.path.exists(csv_file):
        print(f"文件不存在: {csv_file}")
        return False
    
    try:
        # 读取数据
        print("=== 数据探索分析 ===")
        print(f"正在读取: {csv_file}")
        data = pd.read_csv(csv_file)
        
        # 基本信息
        print(f"\n1. 基本信息:")
        print(f"  数据形状: {data.shape}")
        print(f"  列名: {list(data.columns)}")
        print(f"  数据类型:\n{data.dtypes}")
        
        # 数据预览
        print(f"\n2. 数据预览 (前5行):")
        print(data.head())
        
        # 统计信息
        print(f"\n3. 统计信息:")
        print(data.describe())
        
        # 数据质量检查
        print(f"\n4. 数据质量检查:")
        print(f"  缺失值: {data.isnull().sum().sum()}")
        print(f"  重复行: {data.duplicated().sum()}")
        print(f"  无穷值: {np.isinf(data.select_dtypes(include=[np.number])).sum().sum()}")
        
        # 创建时间轴
        data_length = len(data)
        duration = data_length / sampling_rate
        time_data = np.linspace(0, duration, data_length)
        
        print(f"\n5. 时间信息:")
        print(f"  数据点数: {data_length}")
        print(f"  采样率: {sampling_rate} Hz")
        print(f"  总时长: {duration:.2f} 秒")
        
        # 设置绘图样式
        plt.style.use('default')
        plt.rcParams['font.sans-serif'] = ['SimHei', 'DejaVu Sans']
        plt.rcParams['axes.unicode_minus'] = False
        
        # 创建综合可视化
        fig = plt.figure(figsize=(16, 12))
        
        # 1. 时间序列图
        ax1 = plt.subplot(3, 3, 1)
        columns = list(data.columns)
        colors = ['red', 'green', 'blue']
        
        for col, color in zip(columns, colors):
            plt.plot(time_data, data[col], color=color, linewidth=0.8, alpha=0.8, label=col)
        
        plt.title('时间序列图', fontsize=12, fontweight='bold')
        plt.xlabel('时间 (秒)')
        plt.ylabel('数值')
        plt.legend()
        plt.grid(True, alpha=0.3)
        
        # 2. 数据分布直方图
        ax2 = plt.subplot(3, 3, 2)
        for col, color in zip(columns, colors):
            plt.hist(data[col], bins=50, alpha=0.6, color=color, label=col, density=True)
        
        plt.title('数据分布直方图', fontsize=12, fontweight='bold')
        plt.xlabel('数值')
        plt.ylabel('密度')
        plt.legend()
        plt.grid(True, alpha=0.3)
        
        # 3. 箱线图
        ax3 = plt.subplot(3, 3, 3)
        data.boxplot(ax=ax3)
        plt.title('箱线图', fontsize=12, fontweight='bold')
        plt.ylabel('数值')
        plt.grid(True, alpha=0.3)
        
        # 4. 相关性热力图
        ax4 = plt.subplot(3, 3, 4)
        correlation_matrix = data.corr()
        im = ax4.imshow(correlation_matrix, cmap='coolwarm', aspect='auto', vmin=-1, vmax=1)
        ax4.set_xticks(range(len(columns)))
        ax4.set_yticks(range(len(columns)))
        ax4.set_xticklabels(columns, rotation=45)
        ax4.set_yticklabels(columns)
        plt.title('相关性热力图', fontsize=12, fontweight='bold')
        
        # 添加数值标注
        for i in range(len(columns)):
            for j in range(len(columns)):
                text = ax4.text(j, i, f'{correlation_matrix.iloc[i, j]:.2f}',
                               ha="center", va="center", color="black", fontsize=8)
        
        # 5. 散点图矩阵
        ax5 = plt.subplot(3, 3, 5)
        if len(columns) >= 2:
            plt.scatter(data[columns[0]], data[columns[1]], alpha=0.5, s=1)
            plt.xlabel(columns[0])
            plt.ylabel(columns[1])
            plt.title(f'{columns[0]} vs {columns[1]}', fontsize=12, fontweight='bold')
            plt.grid(True, alpha=0.3)
        
        # 6. 移动平均
        ax6 = plt.subplot(3, 3, 6)
        window_size = min(100, len(data) // 10)  # 移动平均窗口大小
        for col, color in zip(columns, colors):
            moving_avg = data[col].rolling(window=window_size).mean()
            plt.plot(time_data, moving_avg, color=color, linewidth=2, label=f'{col} (MA)')
        
        plt.title(f'移动平均 (窗口={window_size})', fontsize=12, fontweight='bold')
        plt.xlabel('时间 (秒)')
        plt.ylabel('数值')
        plt.legend()
        plt.grid(True, alpha=0.3)
        
        # 7. 频谱分析
        ax7 = plt.subplot(3, 3, 7)
        for col, color in zip(columns, colors):
            # 计算FFT
            fft_data = np.fft.fft(data[col])
            freqs = np.fft.fftfreq(len(data[col]), 1/sampling_rate)
            
            # 只取正频率部分
            positive_freqs = freqs[:len(freqs)//2]
            positive_fft = np.abs(fft_data[:len(fft_data)//2])
            
            plt.plot(positive_freqs, positive_fft, color=color, linewidth=1, alpha=0.8, label=col)
        
        plt.title('频谱分析', fontsize=12, fontweight='bold')
        plt.xlabel('频率 (Hz)')
        plt.ylabel('幅度')
        plt.legend()
        plt.grid(True, alpha=0.3)
        plt.xlim(0, sampling_rate/2)  # 只显示到奈奎斯特频率
        
        # 8. 数据统计信息
        ax8 = plt.subplot(3, 3, 8)
        ax8.axis('off')
        
        stats_text = "数据统计信息:\n\n"
        for col in columns:
            mean_val = data[col].mean()
            std_val = data[col].std()
            min_val = data[col].min()
            max_val = data[col].max()
            stats_text += f"{col}:\n"
            stats_text += f"  均值: {mean_val:.4f}\n"
            stats_text += f"  标准差: {std_val:.4f}\n"
            stats_text += f"  范围: [{min_val:.4f}, {max_val:.4f}]\n\n"
        
        ax8.text(0.05, 0.95, stats_text, transform=ax8.transAxes, 
                fontsize=10, verticalalignment='top', fontfamily='monospace',
                bbox=dict(boxstyle='round', facecolor='lightgray', alpha=0.8))
        
        # 9. 数据趋势
        ax9 = plt.subplot(3, 3, 9)
        for col, color in zip(columns, colors):
            # 计算数据的趋势（使用线性回归）
            slope, intercept, r_value, p_value, std_err = stats.linregress(time_data, data[col])
            trend_line = slope * time_data + intercept
            
            plt.plot(time_data, data[col], color=color, linewidth=0.5, alpha=0.6, label=f'{col} (原始)')
            plt.plot(time_data, trend_line, color=color, linewidth=2, linestyle='--', 
                    label=f'{col} (趋势, R²={r_value**2:.3f})')
        
        plt.title('数据趋势分析', fontsize=12, fontweight='bold')
        plt.xlabel('时间 (秒)')
        plt.ylabel('数值')
        plt.legend(fontsize=8)
        plt.grid(True, alpha=0.3)
        
        # 调整布局
        plt.tight_layout()
        
        # 保存图像
        output_file = 'Bxyz_t_4s_10degrees_exploration.png'
        plt.savefig(output_file, dpi=300, bbox_inches='tight')
        print(f"\n探索分析图像已保存为: {output_file}")
        
        # 显示图像
        plt.show()
        
        # 输出详细分析结果
        print(f"\n6. 详细分析结果:")
        print(f"  数据范围分析:")
        for col in columns:
            min_val = data[col].min()
            max_val = data[col].max()
            range_val = max_val - min_val
            print(f"    {col}: [{min_val:.6f}, {max_val:.6f}], 范围: {range_val:.6f}")
        
        print(f"\n  相关性分析:")
        print(correlation_matrix)
        
        print(f"\n  数据质量评估:")
        for col in columns:
            null_count = data[col].isnull().sum()
            inf_count = np.isinf(data[col]).sum()
            print(f"    {col}: 缺失值={null_count}, 无穷值={inf_count}")
        
        return True
        
    except Exception as e:
        print(f"数据探索失败: {e}")
        import traceback
        traceback.print_exc()
        return False

def main():
    """主函数"""
    # CSV文件路径
    csv_file = "/home/xwj/0-code/0-github/motor/python/data_motor/Bxyz_t_4s_10degrees.csv"
    
    # 探索数据
    explore_csv_data(csv_file, sampling_rate=250)

if __name__ == "__main__":
    main()

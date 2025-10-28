#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
快速数据可视化脚本
读取CSV数据并快速绘制图像
"""

import pandas as pd
import numpy as np
import matplotlib.pyplot as plt
import os

def quick_plot_csv(csv_file, sampling_rate=250):
    """
    快速绘制CSV数据
    
    Args:
        csv_file (str): CSV文件路径
        sampling_rate (float): 采样率 (Hz)
    """
    # 检查文件是否存在
    if not os.path.exists(csv_file):
        print(f"文件不存在: {csv_file}")
        return False
    
    # 创建图片保存文件夹
    current_dir = os.path.dirname(os.path.abspath(__file__))
    images_folder = os.path.join(current_dir, "images")
    if not os.path.exists(images_folder):
        os.makedirs(images_folder)
        print(f"创建图片文件夹: {images_folder}")
    else:
        print(f"图片文件夹已存在: {images_folder}")
    
    try:
        # 读取CSV文件
        print(f"正在读取: {csv_file}")
        data = pd.read_csv(csv_file)
        print(f"数据形状: {data.shape}")
        print(f"列名: {list(data.columns)}")
        
        # 创建时间轴
        data_length = len(data)
        duration = data_length / sampling_rate
        time_data = np.linspace(0, duration, data_length)
        
        print(f"数据点数: {data_length}")
        print(f"采样率: {sampling_rate} Hz")
        print(f"总时长: {duration:.2f} 秒")
        
        # 设置字体
        plt.rcParams['font.family'] = 'DejaVu Sans'
        plt.rcParams['axes.unicode_minus'] = False
        
        # 创建图形
        fig, axes = plt.subplots(2, 2, figsize=(15, 10))
        fig.suptitle('Bxyz_t_4s_10degrees Data Visualization', fontsize=16, fontweight='bold')
        
        # 获取列名和颜色
        columns = list(data.columns)
        colors = ['red', 'green', 'blue']
        
        # 1. 分离绘制每个数据列
        for i, (col, color) in enumerate(zip(columns, colors)):
            if i < 3:  # 只绘制前3列
                ax = axes[0, i] if i < 2 else axes[1, 0]
                
                ax.plot(time_data, data[col], color=color, linewidth=1, alpha=0.8)
                ax.set_title(f'{col} vs Time', fontsize=12, fontweight='bold')
                ax.set_xlabel('Time (seconds)', fontsize=10)
                ax.set_ylabel(f'{col} Value', fontsize=10)
                ax.grid(True, alpha=0.3)
                
                # 添加统计信息
                mean_val = data[col].mean()
                ax.axhline(y=mean_val, color=color, linestyle='--', alpha=0.7)
                ax.text(0.02, 0.98, f'Mean: {mean_val:.3f}', transform=ax.transAxes, 
                       verticalalignment='top', fontsize=9, 
                       bbox=dict(boxstyle='round', facecolor='white', alpha=0.8))
        
        # 2. 组合绘制所有数据列
        ax_combined = axes[1, 1]
        for col, color in zip(columns, colors):
            ax_combined.plot(time_data, data[col], color=color, linewidth=1, 
                           alpha=0.8, label=col)
        
        ax_combined.set_title('Combined Data Visualization', fontsize=12, fontweight='bold')
        ax_combined.set_xlabel('Time (seconds)', fontsize=10)
        ax_combined.set_ylabel('Value', fontsize=10)
        ax_combined.grid(True, alpha=0.3)
        ax_combined.legend(fontsize=10)
        
        # 调整布局
        plt.tight_layout()
        
        # 保存图像
        output_file = os.path.join(images_folder, 'Bxyz_t_4s_10degrees_quick_plot.png')
        plt.savefig(output_file, dpi=300, bbox_inches='tight')
        print(f"图像已保存为: {output_file}")
        
        # 显示图像
        plt.show()
        
        return True
    
    def slice_and_analyze(data, time_data, sampling_rate=250, slice_duration=1.0):
        """数据切片分析"""
        # 创建图片保存文件夹
        current_dir = os.path.dirname(os.path.abspath(__file__))
        images_folder = os.path.join(current_dir, "images")
        if not os.path.exists(images_folder):
            os.makedirs(images_folder)
        
        # 计算切片参数
        slice_points = int(slice_duration * sampling_rate)
        total_slices = len(data) // slice_points
        
        print(f"\n数据切片分析:")
        print(f"  切片时长: {slice_duration} 秒")
        print(f"  每切片点数: {slice_points}")
        print(f"  总切片数: {total_slices}")
        
        columns = list(data.columns)
        colors = ['red', 'green', 'blue']
        
        # 创建图形
        fig, axes = plt.subplots(3, 1, figsize=(12, 10))
        fig.suptitle(f'Data Slice Analysis (Slice Angle: {slice_duration}°)', fontsize=16, fontweight='bold')
        
        # 对每个数据列进行分析
        for col_idx, (col, color) in enumerate(zip(columns, colors)):
            peak_to_peak_values = []
            
            # 计算每个切片的峰峰值
            for i in range(total_slices):
                start_idx = i * slice_points
                end_idx = min((i + 1) * slice_points, len(data))
                slice_data = data[col].iloc[start_idx:end_idx]
                
                if len(slice_data) > 0:
                    # 计算峰峰值
                    peak_to_peak = slice_data.max() - slice_data.min()
                    peak_to_peak_values.append(peak_to_peak)
            
            # 绘制峰峰值随角度变化
            ax = axes[col_idx]
            slice_angles = np.arange(total_slices) * slice_duration
            ax.plot(slice_angles, peak_to_peak_values, color=color, 
                    linewidth=2, marker='o', markersize=4, label=f'{col} Peak-to-Peak')
            ax.set_title(f'{col} Peak-to-Peak Variation', fontsize=12, fontweight='bold')
            ax.set_xlabel('Angle (degrees)', fontsize=10)
            ax.set_ylabel('Peak-to-Peak', fontsize=10)
            ax.grid(True, alpha=0.3)
            ax.legend()
        
        # 调整布局
        plt.tight_layout()
        
        # 保存图像
        output_file = os.path.join(images_folder, f'Bxyz_t_4s_10degrees_slice_analysis_{slice_duration}deg.png')
        plt.savefig(output_file, dpi=300, bbox_inches='tight')
        print(f"切片分析图像已保存为: {output_file}")
        
        # 显示图像
        plt.show()
        
        return True
        
    except Exception as e:
        print(f"绘图失败: {e}")
        return False

def main():
    """主函数"""
    # CSV文件路径
    csv_file = "/home/xwj/0-code/0-github/motor/python/data_motor/Bxyz_t_4s_10degrees.csv"
    
    # 快速绘制
    quick_plot_csv(csv_file, sampling_rate=250)
    
    # 数据切片分析
    print("\n正在进行数据切片分析...")
    try:
        # 重新读取数据进行切片分析
        data = pd.read_csv(csv_file)
        data_length = len(data)
        duration = data_length / 250
        time_data = np.linspace(0, duration, data_length)
        
        # 进行切片分析
        slice_and_analyze(data, time_data, sampling_rate=250, slice_duration=10.0)
        
    except Exception as e:
        print(f"切片分析失败: {e}")

if __name__ == "__main__":
    main()

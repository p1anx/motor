#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
数据处理和可视化工具
读取CSV数据文件并绘制图像
"""

import pandas as pd
import numpy as np
import matplotlib.pyplot as plt
import matplotlib.dates as mdates
from datetime import datetime, timedelta
import os
import sys

class DataProcessor:
    """数据处理类"""
    
    def __init__(self, csv_file_path):
        """
        初始化数据处理器
        
        Args:
            csv_file_path (str): CSV文件路径
        """
        self.csv_file_path = csv_file_path
        self.data = None
        self.time_data = None
        self.sampling_rate = None
        
        # 创建图片保存文件夹
        self.create_images_folder()
    
    def create_images_folder(self):
        """创建图片保存文件夹"""
        # 获取当前脚本所在目录
        current_dir = os.path.dirname(os.path.abspath(__file__))
        # 创建图片文件夹路径
        self.images_folder = os.path.join(current_dir, "images")
        
        # 如果文件夹不存在则创建
        if not os.path.exists(self.images_folder):
            os.makedirs(self.images_folder)
            print(f"创建图片文件夹: {self.images_folder}")
        else:
            print(f"图片文件夹已存在: {self.images_folder}")
        
    def load_data(self):
        """加载CSV数据"""
        try:
            print(f"正在读取数据文件: {self.csv_file_path}")
            
            # 读取CSV文件
            self.data = pd.read_csv(self.csv_file_path)
            
            print(f"数据加载成功!")
            print(f"数据形状: {self.data.shape}")
            print(f"列名: {list(self.data.columns)}")
            print(f"数据类型:\n{self.data.dtypes}")
            
            # 显示前几行数据
            print(f"\n前5行数据:")
            print(self.data.head())
            
            # 显示数据统计信息
            print(f"\n数据统计信息:")
            print(self.data.describe())
            
            return True
            
        except Exception as e:
            print(f"数据加载失败: {e}")
            return False
    
    def create_angle_axis(self, total_degrees=360):
        """
        创建角度轴
        
        Args:
            total_degrees (float): 总角度范围 (度)
        """
        if self.data is None:
            print("请先加载数据!")
            return False
        
        data_length = len(self.data)
        
        # 创建角度轴 (0-360度)
        self.angle_data = np.linspace(0, total_degrees, data_length)
        
        print(f"角度轴创建完成:")
        print(f"  数据点数: {data_length}")
        print(f"  角度范围: 0-{total_degrees} 度")
        print(f"  角度分辨率: {total_degrees/data_length:.4f} 度/点")
        
        return True
    
    def plot_data(self, save_plot=True, show_plot=True):
        """
        绘制数据图像
        
        Args:
            save_plot (bool): 是否保存图像
            show_plot (bool): 是否显示图像
        """
        if self.data is None:
            print("请先加载数据!")
            return False
        
        if self.angle_data is None:
            print("请先创建角度轴!")
            return False
        
        # 设置字体
        plt.rcParams['font.family'] = 'DejaVu Sans'
        plt.rcParams['axes.unicode_minus'] = False
        
        # 创建图形
        fig, axes = plt.subplots(3, 1, figsize=(12, 10))
        fig.suptitle('Bxyz_t_4s_10degrees Data Visualization', fontsize=16, fontweight='bold')
        
        # 获取列名
        columns = list(self.data.columns)
        colors = ['red', 'green', 'blue']
        
        # 绘制每个数据列
        for i, (col, color) in enumerate(zip(columns, colors)):
            ax = axes[i]
            
            # 绘制数据
            ax.plot(self.angle_data, self.data[col], color=color, linewidth=1, alpha=0.8)
            
            # 设置标题和标签
            ax.set_title(f'{col} vs Angle', fontsize=14, fontweight='bold')
            ax.set_xlabel('Angle (degrees)', fontsize=12)
            ax.set_ylabel(f'{col} Value', fontsize=12)
            
            # 设置网格
            ax.grid(True, alpha=0.3)
            
            # 设置坐标轴
            ax.tick_params(axis='both', which='major', labelsize=10)
            
            # 添加统计信息
            mean_val = self.data[col].mean()
            std_val = self.data[col].std()
            ax.axhline(y=mean_val, color=color, linestyle='--', alpha=0.7, 
                      label=f'Mean: {mean_val:.3f}')
            ax.axhline(y=mean_val + std_val, color=color, linestyle=':', alpha=0.7,
                      label=f'Mean+Std: {mean_val + std_val:.3f}')
            ax.axhline(y=mean_val - std_val, color=color, linestyle=':', alpha=0.7,
                      label=f'Mean-Std: {mean_val - std_val:.3f}')
            
            ax.legend(fontsize=9)
        
        # 调整布局
        plt.tight_layout()
        
        # 保存图像
        if save_plot:
            output_file = os.path.join(self.images_folder, 'Bxyz_t_4s_10degrees_plot.png')
            plt.savefig(output_file, dpi=300, bbox_inches='tight')
            print(f"图像已保存为: {output_file}")
        
        # 显示图像
        if show_plot:
            plt.show()
        
        return True
    
    def plot_combined(self, save_plot=True, show_plot=True):
        """
        绘制组合图像
        
        Args:
            save_plot (bool): 是否保存图像
            show_plot (bool): 是否显示图像
        """
        if self.data is None:
            print("请先加载数据!")
            return False
        
        if self.angle_data is None:
            print("请先创建角度轴!")
            return False
        
        # 设置字体
        plt.rcParams['font.family'] = 'DejaVu Sans'
        plt.rcParams['axes.unicode_minus'] = False
        
        # 创建图形
        fig, ax = plt.subplots(1, 1, figsize=(12, 8))
        
        # 获取列名和颜色
        columns = list(self.data.columns)
        colors = ['red', 'green', 'blue']
        
        # 绘制所有数据列
        for col, color in zip(columns, colors):
            ax.plot(self.angle_data, self.data[col], color=color, linewidth=1, 
                   alpha=0.8, label=col)
        
        # 设置标题和标签
        ax.set_title('Bxyz_t_4s_10degrees Combined Data Visualization', fontsize=16, fontweight='bold')
        ax.set_xlabel('Angle (degrees)', fontsize=12)
        ax.set_ylabel('Value', fontsize=12)
        
        # 设置网格和图例
        ax.grid(True, alpha=0.3)
        ax.legend(fontsize=12)
        
        # 设置坐标轴
        ax.tick_params(axis='both', which='major', labelsize=10)
        
        # 调整布局
        plt.tight_layout()
        
        # 保存图像
        if save_plot:
            output_file = os.path.join(self.images_folder, 'Bxyz_t_4s_10degrees_combined_plot.png')
            plt.savefig(output_file, dpi=300, bbox_inches='tight')
            print(f"组合图像已保存为: {output_file}")
        
        # 显示图像
        if show_plot:
            plt.show()
        
        return True
    
    def slice_data_and_analyze(self, slice_angle=10.0, save_plot=True, show_plot=True):
        """
        数据切片分析，计算峰峰值并绘制FFT图
        
        Args:
            slice_angle (float): 切片角度 (度)
            save_plot (bool): 是否保存图像
            show_plot (bool): 是否显示图像
        """
        if self.data is None:
            print("请先加载数据!")
            return False
        
        if self.angle_data is None:
            print("请先创建角度轴!")
            return False
        
        # 设置字体
        plt.rcParams['font.family'] = 'DejaVu Sans'
        plt.rcParams['axes.unicode_minus'] = False
        
        # 计算切片参数
        data_length = len(self.data)
        total_degrees = 360
        slice_points = int(slice_angle * data_length / total_degrees)
        total_slices = data_length // slice_points
        
        print(f"数据切片分析:")
        print(f"  切片角度: {slice_angle} 度")
        print(f"  每切片点数: {slice_points}")
        print(f"  总切片数: {total_slices}")
        
        # 创建图形
        fig, axes = plt.subplots(3, 1, figsize=(12, 10))
        fig.suptitle(f'Data Slice Analysis (Slice Angle: {slice_angle}°)', fontsize=16, fontweight='bold')
        
        columns = list(self.data.columns)
        colors = ['red', 'green', 'blue']
        
        # 存储峰峰值结果
        peak_to_peak_results = {}
        
        # 对每个数据列进行分析
        for col_idx, (col, color) in enumerate(zip(columns, colors)):
            peak_to_peak_results[col] = []
            
            # 计算每个切片的峰峰值
            for i in range(total_slices):
                start_idx = i * slice_points
                end_idx = min((i + 1) * slice_points, len(self.data))
                slice_data = self.data[col].iloc[start_idx:end_idx]
                
                if len(slice_data) > 0:
                    peak_to_peak = slice_data.max() - slice_data.min()
                    peak_to_peak_results[col].append(peak_to_peak)
                else:
                    peak_to_peak_results[col].append(0)
            
            # 绘制峰峰值随角度变化
            ax = axes[col_idx]
            slice_angles = np.arange(total_slices) * slice_angle
            ax.plot(slice_angles, peak_to_peak_results[col], color=color, 
                    linewidth=2, marker='o', markersize=4, label=f'{col} Peak-to-Peak')
            ax.set_title(f'{col} Peak-to-Peak Variation', fontsize=12, fontweight='bold')
            ax.set_xlabel('Angle (degrees)', fontsize=10)
            ax.set_ylabel('Peak-to-Peak', fontsize=10)
            ax.grid(True, alpha=0.3)
            ax.legend()
        
        # 调整布局
        plt.tight_layout()
        
        # 保存图像
        if save_plot:
            output_file = os.path.join(self.images_folder, f'Bxyz_t_4s_10degrees_slice_analysis_{slice_angle}deg.png')
            plt.savefig(output_file, dpi=300, bbox_inches='tight')
            print(f"切片分析图像已保存为: {output_file}")
        
        # 显示图像
        if show_plot:
            plt.show()
        
        # 输出峰峰值统计信息
        print(f"\n峰峰值统计信息:")
        for col in columns:
            if peak_to_peak_results[col]:
                mean_pp = np.mean(peak_to_peak_results[col])
                std_pp = np.std(peak_to_peak_results[col])
                max_pp = np.max(peak_to_peak_results[col])
                min_pp = np.min(peak_to_peak_results[col])
                print(f"  {col}:")
                print(f"    平均峰峰值: {mean_pp:.6f}")
                print(f"    标准差: {std_pp:.6f}")
                print(f"    最大峰峰值: {max_pp:.6f}")
                print(f"    最小峰峰值: {min_pp:.6f}")
        
        return True
    
    def analyze_data(self):
        """分析数据特征"""
        if self.data is None:
            print("请先加载数据!")
            return False
        
        print("=== 数据特征分析 ===")
        
        # 基本统计信息
        print("\n1. 基本统计信息:")
        print(self.data.describe())
        
        # 数据范围
        print("\n2. 数据范围:")
        for col in self.data.columns:
            min_val = self.data[col].min()
            max_val = self.data[col].max()
            range_val = max_val - min_val
            print(f"  {col}: [{min_val:.6f}, {max_val:.6f}], 范围: {range_val:.6f}")
        
        # 相关性分析
        print("\n3. 相关性分析:")
        correlation_matrix = self.data.corr()
        print(correlation_matrix)
        
        # 数据质量检查
        print("\n4. 数据质量检查:")
        print(f"  总数据点数: {len(self.data)}")
        print(f"  缺失值数量: {self.data.isnull().sum().sum()}")
        print(f"  重复行数量: {self.data.duplicated().sum()}")
        
        return True


def main():
    """主函数"""
    # CSV文件路径
    csv_file = "/home/xwj/0-code/0-github/motor/python/data_motor/Bxyz_t_4s_10degrees.csv"
    
    # 检查文件是否存在
    if not os.path.exists(csv_file):
        print(f"文件不存在: {csv_file}")
        return
    
    # 创建数据处理器
    processor = DataProcessor(csv_file)
    
    # 加载数据
    if not processor.load_data():
        return
    
    # 创建角度轴 (0-360度)
    processor.create_angle_axis(total_degrees=360)
    
    # 分析数据
    processor.analyze_data()
    
    # 绘制分离图像
    print("\n正在绘制分离图像...")
    processor.plot_data(save_plot=True, show_plot=True)
    
    # 绘制组合图像
    print("\n正在绘制组合图像...")
    processor.plot_combined(save_plot=True, show_plot=True)
    
    # 数据切片分析
    print("\n正在进行数据切片分析...")
    processor.slice_data_and_analyze(slice_angle=10.0, save_plot=True, show_plot=True)
    
    print("\n数据处理完成!")


if __name__ == "__main__":
    main()

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
    
    def plot_data_points(self, save_plot=True, show_plot=True):
        """
        绘制数据图像 - x轴为数据原始点数
        
        Args:
            save_plot (bool): 是否保存图像
            show_plot (bool): 是否显示图像
        """
        if self.data is None:
            print("请先加载数据!")
            return False
        
        # 设置字体
        # plt.rcParams['font.family'] = 'DejaVu Sans'
        plt.rcParams['axes.unicode_minus'] = False
        
        # 创建图形
        fig, axes = plt.subplots(3, 1, figsize=(12, 10))
        fig.suptitle('Bxyz_t_4s_10degrees Data Visualization (Data Points)', fontsize=16, fontweight='bold')
        
        # 获取列名
        columns = list(self.data.columns)
        colors = ['red', 'green', 'blue']
        
        # 创建数据点索引
        data_points = np.arange(len(self.data))
        
        # 绘制每个数据列
        for i, (col, color) in enumerate(zip(columns, colors)):
            ax = axes[i]
            
            # 绘制数据
            ax.plot(data_points, self.data[col], color=color, linewidth=1, alpha=0.8, label=col)
            
            # 设置标题和标签
            ax.set_title(f'{col} vs Data Points', fontsize=12, fontweight='bold')
            ax.set_xlabel('Data Points', fontsize=10)
            ax.set_ylabel(f'{col} Value', fontsize=10)
            
            # 设置网格
            ax.grid(True, alpha=0.3)
            
            # 添加图例
            ax.legend(fontsize=9)
        
        # 调整布局
        plt.tight_layout()
        
        # 保存图像
        if save_plot:
            output_file = os.path.join(self.images_folder, 'Bxyz_t_4s_10degrees_data_points_plot.png')
            plt.savefig(output_file, dpi=300, bbox_inches='tight')
            print(f"数据点图像已保存为: {output_file}")
        
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
    
    def get_peak_to_peak_array(self, data=None, slice_angle=10.0, total_degrees=360):
        """
        通用的峰峰值计算函数，可以接受任意输入数据
        
        Args:
            data (pd.DataFrame, np.array, list, or None): 输入数据
                - 如果是DataFrame，会对每一列进行计算
                - 如果是np.array或list，对单列数据计算
                - 如果为None，使用self.data
            slice_angle (float): 切片角度 (度)
            total_degrees (float): 总角度范围，默认360度
            
        Returns:
            dict or list: 
                - 如果输入是多列数据（DataFrame或多列array），返回dict，key为列名或索引
                - 如果输入是单列数据，返回list
            
        Examples:
            # 使用类内部数据
            result = processor.get_peak_to_peak_array()
            
            # 使用外部DataFrame数据
            result = processor.get_peak_to_peak_array(data=my_dataframe, slice_angle=15.0)
            
            # 使用单列数组数据
            result = processor.get_peak_to_peak_array(data=my_array, slice_angle=20.0)
        """
        # 确定使用的数据
        if data is None:
            if self.data is None:
                print("请先加载数据或提供输入数据!")
                return None
            data = self.data
        else:
            # 转换输入数据为DataFrame格式
            if isinstance(data, np.ndarray):
                if data.ndim == 1:
                    data = pd.DataFrame({'data': data})
                else:
                    data = pd.DataFrame(data)
            elif isinstance(data, list):
                data = pd.DataFrame({'data': data})
            elif isinstance(data, pd.Series):
                data = pd.DataFrame({'data': data})
            # 如果已经是DataFrame，直接使用
        
        # 获取数据长度
        data_length = len(data)
        
        if data_length == 0:
            print("输入数据为空!")
            return None
        
        # 计算切片参数
        slice_points = int(slice_angle * data_length / total_degrees)
        
        if slice_points <= 0:
            print(f"切片点数必须大于0，请调整slice_angle参数!")
            return None
        
        total_slices = data_length // slice_points
        
        if total_slices == 0:
            print(f"总切片数必须大于0，请调整slice_angle参数!")
            return None
        
        print(f"数据切片分析:")
        print(f"  数据长度: {data_length}")
        print(f"  切片角度: {slice_angle} 度")
        print(f"  每切片点数: {slice_points}")
        print(f"  总切片数: {total_slices}")
        
        # 存储结果
        results = {}
        
        # 对每个数据列进行分析
        columns = list(data.columns)
        
        for col in columns:
            peak_to_peak_values = []
            
            # 计算每个切片的峰峰值
            for i in range(total_slices):
                start_idx = i * slice_points
                end_idx = min((i + 1) * slice_points, data_length)
                slice_data = data[col].iloc[start_idx:end_idx]
                
                if len(slice_data) > 0:
                    peak_to_peak = slice_data.max() - slice_data.min()
                    peak_to_peak_values.append(peak_to_peak)
                else:
                    peak_to_peak_values.append(0)
            
            results[col] = peak_to_peak_values
            
            # 输出统计信息
            # if peak_to_peak_values:
            #     mean_pp = np.mean(peak_to_peak_values)
            #     std_pp = np.std(peak_to_peak_values)
            #     max_pp = np.max(peak_to_peak_values)
            #     min_pp = np.min(peak_to_peak_values)
            #     print(f"\n  {col} 峰峰值统计:")
            #     print(f"    平均: {mean_pp:.6f}")
            #     print(f"    标准差: {std_pp:.6f}")
            #     print(f"    最大值: {max_pp:.6f}")
            #     print(f"    最小值: {min_pp:.6f}")
        
        # 如果只有一列数据，直接返回列表
        if len(columns) == 1:
            return results[columns[0]]
        else:
            return results
    
    def plot_peak_to_peak_array(self, peak_to_peak_data, slice_angle=10.0, save_plot=True, show_plot=True, title="Peak-to-Peak Array Visualization"):
        """
        绘制峰峰值数组的图像
        
        Args:
            peak_to_peak_data (list, dict, or array): 峰峰值数据
                - 如果是list或array，绘制单条曲线
                - 如果是dict，绘制多条曲线（每条一个key）
            slice_angle (float): 切片角度 (度)，用于创建x轴
            save_plot (bool): 是否保存图像
            show_plot (bool): 是否显示图像
            title (str): 图像标题
            
        Returns:
            bool: 是否成功绘制
        """
        # 转换输入数据为统一格式
        if isinstance(peak_to_peak_data, dict):
            data_dict = peak_to_peak_data
            is_single_channel = False
        else:
            # 如果是list或array，转换为dict
            data_dict = {'data': list(peak_to_peak_data)}
            is_single_channel = True
        
        if not data_dict:
            print("峰峰值数据为空!")
            return False
        
        # 设置字体
        plt.rcParams['font.family'] = 'DejaVu Sans'
        plt.rcParams['axes.unicode_minus'] = False
        
        # 创建图形
        fig, ax = plt.subplots(1, 1, figsize=(12, 6))
        
        # 绘制每个通道的峰峰值曲线
        colors = ['red', 'green', 'blue', 'orange', 'purple', 'brown', 'pink', 'gray', 'olive', 'cyan']
        
        for i, (channel, values) in enumerate(data_dict.items()):
            # 计算x轴角度值
            angles = np.arange(len(values)) * slice_angle
            
            # 选择颜色
            color = colors[i % len(colors)]
            
            # 绘制曲线
            ax.plot(angles, values, color=color, linewidth=2, marker='o', 
                   markersize=5, label=str(channel), alpha=0.8)
        
        # 设置标题和标签
        ax.set_title(title, fontsize=14, fontweight='bold')
        ax.set_xlabel('Angle (degrees)', fontsize=12)
        ax.set_ylabel('Peak-to-Peak Value', fontsize=12)
        
        # 设置网格和图例
        ax.grid(True, alpha=0.3, linestyle='--')
        ax.legend(fontsize=10, loc='best')
        
        # 设置坐标轴
        ax.tick_params(axis='both', which='major', labelsize=10)
        
        # 调整布局
        plt.tight_layout()
        
        # 保存图像
        if save_plot:
            output_file = os.path.join(self.images_folder, 'peak_to_peak_visualization.png')
            plt.savefig(output_file, dpi=300, bbox_inches='tight')
            print(f"峰峰值可视化图像已保存为: {output_file}")
        
        # 显示图像
        if show_plot:
            plt.show()
        
        return True
    
    def slice_data_and_analyze(self, slice_angle=10.0, n_divisions=10, save_plot=True, show_plot=True):
        """
        数据切片分析，包含等分结果绘制
        
        Args:
            slice_angle (float): 切片角度 (度)
            n_divisions (int): 等分数
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
        print(f"  等分数: {n_divisions}")
        
        # 创建图形
        fig, axes = plt.subplots(3, 1, figsize=(14, 12))
        fig.suptitle(f'Data Slice Analysis with Divisions (Slice Angle: {slice_angle}°, Divisions: {n_divisions})', 
                     fontsize=16, fontweight='bold')
        
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
            
            # 绘制原始峰峰值曲线
            ax.plot(slice_angles, peak_to_peak_results[col], color=color, 
                    linewidth=2, marker='o', markersize=4, label=f'{col} Peak-to-Peak', alpha=0.7)
            
            # 计算等分区间并绘制
        #     if peak_to_peak_results[col]:
        #         min_pp = min(peak_to_peak_results[col])
        #         max_pp = max(peak_to_peak_results[col])
        #         division_size = (max_pp - min_pp) / n_divisions
                
        #         # 绘制等分边界点
        #         for i in range(n_divisions + 1):
        #             boundary_value = min_pp + i * division_size
                    
        #             # 找到所有匹配的角度值（使用更宽松的精度）
        #             exact_angles = []
        #             for pp_value, angle in zip(peak_to_peak_results[col], slice_angles):
        #                 if abs(pp_value - boundary_value) < 1e-6:
        #                     exact_angles.append(angle)
                    
        #             if exact_angles:
        #                 # 如果有精确匹配的角度值，绘制所有点
        #                 for j, angle in enumerate(exact_angles):
        #                     ax.scatter(angle, boundary_value, color='red', marker='o', 
        #                              s=100, alpha=1.0, edgecolors='black', linewidth=2, 
        #                              label='Exact Boundary Points' if i == 0 and j == 0 else "", zorder=6)
        #             else:
        #                 # 如果没有精确匹配，使用插值预测
        #                 predicted_angle = self._interpolate_angle_for_amplitude(
        #                     peak_to_peak_results[col], slice_angles, boundary_value
        #                 )
        #                 ax.scatter(predicted_angle, boundary_value, color='blue', marker='o', 
        #                          s=100, alpha=1.0, edgecolors='black', linewidth=2, 
        #                          label='Predicted Boundary Points' if i == 1 else "", zorder=6)
            
        #     ax.set_title(f'{col} Peak-to-Peak Variation with {n_divisions} Divisions', fontsize=12, fontweight='bold')
        #     ax.set_xlabel('Angle (degrees)', fontsize=10)
        #     ax.set_ylabel('Peak-to-Peak', fontsize=10)
        #     ax.grid(True, alpha=0.3)
        #     # 调整图例位置和大小
        #     ax.legend(fontsize=7, loc='upper right', bbox_to_anchor=(1.0, 1.0), ncol=1)
        
        # # 调整布局
        # plt.tight_layout()
        
        # # 保存图像
        # if save_plot:
        #     output_file = os.path.join(self.images_folder, f'Bxyz_t_4s_10degrees_slice_analysis_with_divisions_{slice_angle}deg_{n_divisions}div.png')
        #     plt.savefig(output_file, dpi=300, bbox_inches='tight')
        #     print(f"切片分析图像已保存为: {output_file}")
        
        # # 显示图像
        # if show_plot:
        #     plt.show()
        
        # # 输出峰峰值统计信息
        # print(f"\n峰峰值统计信息:")
        # for col in columns:
        #     if peak_to_peak_results[col]:
        #         mean_pp = np.mean(peak_to_peak_results[col])
        #         std_pp = np.std(peak_to_peak_results[col])
        #         max_pp = np.max(peak_to_peak_results[col])
        #         min_pp = np.min(peak_to_peak_results[col])
        #         print(f"  {col}:")
        #         print(f"    平均峰峰值: {mean_pp:.6f}")
        #         print(f"    标准差: {std_pp:.6f}")
        #         print(f"    最大峰峰值: {max_pp:.6f}")
        #         print(f"    最小峰峰值: {min_pp:.6f}")
        
        return True
    
    def get_peak_to_peak_divisions(self, slice_angle=10.0, n_divisions=10):
        """
        将切片数据获取的峰峰值曲线幅度进行n等分，返回等分边界点对应的x轴角度值
        
        Args:
            slice_angle (float): 切片角度 (度)
            n_divisions (int): 等分数
            
        Returns:
            dict: 包含每个数据列的等分边界点对应角度值的字典
        """
        if self.data is None:
            print("请先加载数据!")
            return None
        
        if self.angle_data is None:
            print("请先创建角度轴!")
            return None
        
        print(f"峰峰值曲线幅度n等分边界点分析:")
        print(f"  切片角度: {slice_angle} 度")
        print(f"  等分数: {n_divisions}")
        
        # 计算切片参数
        data_length = len(self.data)
        total_degrees = 360
        slice_points = int(slice_angle * data_length / total_degrees)
        total_slices = data_length // slice_points
        
        columns = list(self.data.columns)
        results = {}
        
        # 对每个数据列进行分析
        for col in columns:
            peak_to_peak_values = []
            slice_angles = []
            
            # 计算每个切片的峰峰值和对应角度
            for i in range(total_slices):
                start_idx = i * slice_points
                end_idx = min((i + 1) * slice_points, len(self.data))
                slice_data = self.data[col].iloc[start_idx:end_idx]
                
                if len(slice_data) > 0:
                    peak_to_peak = slice_data.max() - slice_data.min()
                    peak_to_peak_values.append(peak_to_peak)
                    slice_angles.append(i * slice_angle)
            
            # 计算等分边界点
            if peak_to_peak_values:
                min_pp = min(peak_to_peak_values)
                max_pp = max(peak_to_peak_values)
                division_size = (max_pp - min_pp) / n_divisions
                
                # 计算等分边界点
                division_boundaries = []
                for i in range(n_divisions + 1):
                    boundary_value = min_pp + i * division_size
                    division_boundaries.append(boundary_value)
                
                # 为每个等分边界点找到所有对应的角度值
                boundary_angles = []
                
                for boundary_value in division_boundaries:
                    # 找到所有匹配的角度值（使用更宽松的精度）
                    exact_angles = []
                    for pp_value, angle in zip(peak_to_peak_values, slice_angles):
                        if abs(pp_value - boundary_value) < 1e-6:  # 更宽松的精度检查
                            exact_angles.append(angle)
                    
                    if exact_angles:
                        # 如果有精确匹配的角度值
                        boundary_angles.append({
                            'boundary_value': boundary_value,
                            'angle_values': exact_angles,
                            'is_exact': True,
                            'count': len(exact_angles)
                        })
                    else:
                        # 如果没有精确匹配，使用插值预测
                        predicted_angle = self._interpolate_angle_for_amplitude(
                            peak_to_peak_values, slice_angles, boundary_value
                        )
                        boundary_angles.append({
                            'boundary_value': boundary_value,
                            'angle_values': [predicted_angle],
                            'is_exact': False,
                            'count': 1
                        })
                
                results[col] = {
                    'peak_to_peak_range': (min_pp, max_pp),
                    'division_size': division_size,
                    'boundary_points': boundary_angles
                }
                
                # 输出结果
                print(f"\n{col} 等分边界点结果:")
                print(f"  峰峰值范围: [{min_pp:.6f}, {max_pp:.6f}]")
                print(f"  等分大小: {division_size:.6f}")
                
                for i, boundary_info in enumerate(boundary_angles):
                    boundary_value = boundary_info['boundary_value']
                    angle_values = boundary_info['angle_values']
                    is_exact = boundary_info['is_exact']
                    count = boundary_info['count']
                    
                    # 格式化角度值数组
                    angle_str = ', '.join([f"{angle:.2f}°" for angle in angle_values])
                    
                    if i == 0:
                        print(f"  边界点 {i+1} (最小值): 幅度值 {boundary_value:.6f} -> 角度值数组 [{angle_str}] ({count}个值) {'(精确)' if is_exact else '(预测)'}")
                    elif i == len(boundary_angles) - 1:
                        print(f"  边界点 {i+1} (最大值): 幅度值 {boundary_value:.6f} -> 角度值数组 [{angle_str}] ({count}个值) {'(精确)' if is_exact else '(预测)'}")
                    else:
                        print(f"  边界点 {i+1}: 幅度值 {boundary_value:.6f} -> 角度值数组 [{angle_str}] ({count}个值) {'(精确)' if is_exact else '(预测)'}")
        
        return results
    
    def get_original_data_divisions(self, n_divisions=10):
        """
        将原始数据幅度进行n等分，返回每个等分边界点对应的所有角度值
        
        Args:
            n_divisions (int): 等分数
            
        Returns:
            dict: 每个通道的等分边界点结果
        """
        if self.data is None:
            print("请先加载数据!")
            return None
        
        # 创建角度轴
        total_degrees = 360
        data_points = len(self.data)
        angle_resolution = total_degrees / data_points
        angles = np.linspace(0, total_degrees, data_points)
        
        results = {}
        
        for col in self.data.columns:
            values = self.data[col].values
            
            # 计算原始数据的等分边界点
            min_value = min(values)
            max_value = max(values)
            division_size = (max_value - min_value) / n_divisions
            
            # 计算等分边界点
            division_boundaries = []
            for i in range(n_divisions + 1):
                boundary_value = min_value + i * division_size
                division_boundaries.append(boundary_value)
            
            # 为每个等分边界点找到所有对应的角度值
            boundary_angles = []
            
            for boundary_value in division_boundaries:
                # 找到所有匹配的角度值（使用更宽松的精度）
                exact_angles = []
                for value, angle in zip(values, angles):
                    if abs(value - boundary_value) < 1e-6:  # 更宽松的精度检查
                        exact_angles.append(angle)
                
                if exact_angles:
                    # 如果有精确匹配的角度值
                    boundary_angles.append({
                        'boundary_value': boundary_value,
                        'angle_values': exact_angles,
                        'is_exact': True,
                        'count': len(exact_angles)
                    })
                else:
                    # 如果没有精确匹配，使用插值预测
                    predicted_angle = self._interpolate_angle_for_amplitude(
                        values, angles, boundary_value
                    )
                    boundary_angles.append({
                        'boundary_value': boundary_value,
                        'angle_values': [predicted_angle],
                        'is_exact': False,
                        'count': 1
                    })
            
            results[col] = {
                'data_range': (min_value, max_value),
                'division_size': division_size,
                'boundary_points': boundary_angles
            }
            
            # 输出结果
            print(f"\n{col} 原始数据等分边界点结果:")
            print(f"  数据范围: [{min_value:.6f}, {max_value:.6f}]")
            print(f"  等分大小: {division_size:.6f}")
            
            for i, boundary_info in enumerate(boundary_angles):
                boundary_value = boundary_info['boundary_value']
                angle_values = boundary_info['angle_values']
                is_exact = boundary_info['is_exact']
                count = boundary_info['count']
                
                # 格式化角度值数组
                angle_str = ', '.join([f"{angle:.2f}°" for angle in angle_values])
                
                if i == 0:
                    print(f"  边界点 {i+1} (最小值): 幅度值 {boundary_value:.6f} -> 角度值数组 [{angle_str}] ({count}个值) {'(精确)' if is_exact else '(预测)'}")
                elif i == len(boundary_angles) - 1:
                    print(f"  边界点 {i+1} (最大值): 幅度值 {boundary_value:.6f} -> 角度值数组 [{angle_str}] ({count}个值) {'(精确)' if is_exact else '(预测)'}")
                else:
                    print(f"  边界点 {i+1}: 幅度值 {boundary_value:.6f} -> 角度值数组 [{angle_str}] ({count}个值) {'(精确)' if is_exact else '(预测)'}")
        
        return results
    
    def _interpolate_angle_for_amplitude(self, peak_to_peak_values, slice_angles, target_amplitude):
        """
        使用线性插值预测给定幅度值对应的角度值
        
        Args:
            peak_to_peak_values (list): 峰峰值列表
            slice_angles (list): 对应的角度列表
            target_amplitude (float): 目标幅度值
            
        Returns:
            float: 预测的角度值
        """
        if len(peak_to_peak_values) == 0 or len(slice_angles) == 0:
            return 0.0
        
        # 如果目标幅度值在数据范围内，使用线性插值
        if target_amplitude <= min(peak_to_peak_values):
            return slice_angles[peak_to_peak_values.index(min(peak_to_peak_values))]
        elif target_amplitude >= max(peak_to_peak_values):
            return slice_angles[peak_to_peak_values.index(max(peak_to_peak_values))]
        
        # 找到目标幅度值两侧的数据点
        for i in range(len(peak_to_peak_values) - 1):
            if peak_to_peak_values[i] <= target_amplitude <= peak_to_peak_values[i + 1]:
                # 线性插值
                x1, y1 = peak_to_peak_values[i], slice_angles[i]
                x2, y2 = peak_to_peak_values[i + 1], slice_angles[i + 1]
                
                if x2 == x1:  # 避免除零错误
                    return y1
                
                # 线性插值公式: y = y1 + (y2 - y1) * (x - x1) / (x2 - x1)
                predicted_angle = y1 + (y2 - y1) * (target_amplitude - x1) / (x2 - x1)
                return predicted_angle
        
        # 如果没有找到合适的区间，返回最接近的值
        closest_idx = min(range(len(peak_to_peak_values)), 
                         key=lambda i: abs(peak_to_peak_values[i] - target_amplitude))
        return slice_angles[closest_idx]
    
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
    # csv_file = "/home/xwj/0-code/0-github/motor/python/data_motor/Bxyz_t_4s_10degrees.csv"
    csv_file = "/home/xwj/0-code/0-github/motor/python/data_motor/Bxyz_0.csv"
    
    # 检查文件是否存在
    if not os.path.exists(csv_file):
        print(f"文件不存在: {csv_file}")
        return
    
    # 创建数据处理器
    processor = DataProcessor(csv_file)
    
    # 加载数据
    if not processor.load_data():
        return
    
    # result = processor.get_peak_to_peak_array(data=processor.data, slice_angle=10.0, total_degrees=360)
    # print(result)
    # 创建角度轴 (0-360度)
    # processor.create_angle_axis(total_degrees=360)
    
    # # 分析数据
    # processor.analyze_data()
    
    # # 绘制分离图像
    # print("\n正在绘制分离图像...")
    # # processor.plot_data(save_plot=True, show_plot=False)
    
    # # 绘制组合图像
    # # print("\n正在绘制组合图像...")
    # # processor.plot_combined(save_plot=True, show_plot=False)
    
    # # 绘制数据点图像 (x轴为数据原始点数)
    print("\n正在绘制数据点图像...")
    processor.plot_data_points(save_plot=True, show_plot=False)
    
    n_divisions = 4
    # # 数据切片分析 (包含等分结果绘制)
    # print("\n正在进行数据切片分析...")
    # processor.slice_data_and_analyze(slice_angle=10.0, n_divisions=n_divisions, save_plot=True, show_plot=False)
    
    # # 峰峰值等分分析
    # print("\n正在进行峰峰值等分分析...")
    # division_results = processor.get_peak_to_peak_divisions(slice_angle=10.0, n_divisions=n_divisions)
    
    # # 原始数据等分分析
    # print("\n正在进行原始数据等分分析...")
    # original_division_results = processor.get_original_data_divisions(n_divisions=n_divisions)

    
    # print("\n数据处理完成!")
    plt.show()


if __name__ == "__main__":
    main()

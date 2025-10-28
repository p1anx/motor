
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
        plt.rcParams['font.family'] = 'DejaVu Sans'
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

    def plot_data_slices(self, slice_angle=10.0, total_degrees=360, max_slices=2000, save_plot=True, show_plot=True):
        """
        绘制数据切片图像 - 不同切片用不同颜色
        
        Args:
            slice_angle (float): 切片角度 (度)
            total_degrees (float): 总角度范围，默认360度
            max_slices (int): 最大显示切片数，避免过多切片导致显示混乱
            save_plot (bool): 是否保存图像
            show_plot (bool): 是否显示图像
        """
        if self.data is None:
            print("请先加载数据!")
            return False
        
        # 设置字体
        plt.rcParams['font.family'] = 'DejaVu Sans'
        plt.rcParams['axes.unicode_minus'] = False
        
        # 计算切片参数
        data_length = len(self.data)
        slice_points = int(slice_angle * data_length / total_degrees)
        total_slices = data_length // slice_points
        
        if total_slices == 0:
            print(f"切片数必须大于0，请调整slice_angle参数!")
            return False
        
        # 限制显示的切片数
        display_slices = min(total_slices, max_slices)
        
        print(f"数据切片分析:")
        print(f"  数据长度: {data_length}")
        print(f"  切片角度: {slice_angle} 度")
        print(f"  每切片点数: {slice_points}")
        print(f"  总切片数: {total_slices}")
        print(f"  显示切片数: {display_slices}")
        
        # 创建图形
        fig, axes = plt.subplots(3, 1, figsize=(15, 12))
        fig.suptitle(f'Data Slices Visualization (Slice Angle: {slice_angle}°, Showing {display_slices}/{total_slices} slices)', 
                    fontsize=16, fontweight='bold')
        
        # 获取列名
        columns = list(self.data.columns)
        
        # 生成交替颜色映射 - 只使用两种颜色交替
        colors = ['red', 'blue'] * (display_slices // 2 + 1)
        
        # 绘制每个数据列
        for i, col in enumerate(columns):
            ax = axes[i]
            
            # 绘制每个切片
            for slice_idx in range(display_slices):
                start_idx = slice_idx * slice_points
                end_idx = min((slice_idx + 1) * slice_points, data_length)
                
                # 获取切片数据
                slice_data = self.data[col].iloc[start_idx:end_idx]
                data_points = np.arange(start_idx, end_idx)
                
                # 调试信息
                if slice_idx < 3:  # 只打印前3个切片的信息
                    print(f"  {col} - Slice {slice_idx+1}: 索引 {start_idx}-{end_idx-1}, 实际点数: {len(slice_data)}")
                
                # 绘制切片
                ax.plot(data_points, slice_data, 
                       color=colors[slice_idx], 
                       linewidth=2.0, 
                       alpha=0.8, 
                       label=f'Slice {slice_idx+1}' if slice_idx < 10 else "")
                
                # 添加切片边界垂直线
                if slice_idx > 0:  # 不在第一个切片添加左边界线
                    ax.axvline(x=start_idx, color='black', linestyle='--', alpha=0.5, linewidth=1)
                
                # 在最后一个切片添加右边界线
                if slice_idx == display_slices - 1:
                    ax.axvline(x=end_idx-1, color='black', linestyle='--', alpha=0.5, linewidth=1)
            
            # 设置标题和标签
            ax.set_title(f'{col} - Data Slices (First {display_slices} slices)', fontsize=12, fontweight='bold')
            ax.set_xlabel('Data Points', fontsize=10)
            ax.set_ylabel(f'{col} Value', fontsize=10)
            
            # 设置网格
            ax.grid(True, alpha=0.3)
            
            # 添加图例 - 只显示两种颜色
            from matplotlib.patches import Patch
            legend_elements = [Patch(facecolor='red', label='Odd Slices'), 
                             Patch(facecolor='blue', label='Even Slices')]
            ax.legend(handles=legend_elements, fontsize=8, loc='upper right')
            
            # 添加切片信息
            if total_slices > display_slices:
                ax.text(0.02, 0.98, f'Showing first {display_slices} of {total_slices} slices', 
                       transform=ax.transAxes, fontsize=10, 
                       verticalalignment='top', bbox=dict(boxstyle='round', facecolor='wheat', alpha=0.8))
        
        # 调整布局
        plt.tight_layout()
        
        # 保存图像
        if save_plot:
            output_file = os.path.join(self.images_folder, f'data_slices_{slice_angle}deg_plot.png')
            plt.savefig(output_file, dpi=300, bbox_inches='tight')
            print(f"数据切片图像已保存为: {output_file}")
        
        # 显示图像
        if show_plot:
            plt.show()
        
        return True

    def plot_slice_fft_analysis(self, slice_angle=10.0, total_degrees=360, max_slices=6, 
                                sampling_rate=None, filter_dc=True, filter_range=(0, 1), 
                                zero_padding_factor=4, save_plot=True, show_plot=True):
        """
        绘制每个数据切片的FFT频谱分析 - 每个切片单独绘制，支持滤波和高分辨率
        
        Args:
            slice_angle (float): 切片角度 (度)
            total_degrees (float): 总角度范围，默认360度
            max_slices (int): 最大显示切片数
            sampling_rate (float): 采样率 (Hz)，如果为None则使用归一化频率
            filter_dc (bool): 是否滤除直流分量
            filter_range (tuple): 滤波范围 (Hz)，格式为 (低频截止, 高频截止)
            zero_padding_factor (int): 零填充因子，用于提高FFT分辨率
            save_plot (bool): 是否保存图像
            show_plot (bool): 是否显示图像
        """
        if self.data is None:
            print("请先加载数据!")
            return False
        
        # 设置字体
        plt.rcParams['font.family'] = 'DejaVu Sans'
        plt.rcParams['axes.unicode_minus'] = False
        
        # 计算切片参数
        data_length = len(self.data)
        slice_points = int(slice_angle * data_length / total_degrees)
        total_slices = data_length // slice_points
        
        if total_slices == 0:
            print(f"切片数必须大于0，请调整slice_angle参数!")
            return False
        
        # 限制显示的切片数
        display_slices = min(total_slices, max_slices)
        
        print(f"FFT切片分析:")
        print(f"  数据长度: {data_length}")
        print(f"  切片角度: {slice_angle} 度")
        print(f"  每切片点数: {slice_points}")
        print(f"  总切片数: {total_slices}")
        print(f"  显示切片数: {display_slices}")
        if sampling_rate:
            print(f"  采样率: {sampling_rate} Hz")
        else:
            print(f"  使用归一化频率")
        print(f"  滤波设置: DC滤波={filter_dc}, 滤波范围={filter_range} Hz")
        print(f"  零填充因子: {zero_padding_factor}x (分辨率提升{zero_padding_factor}倍)")
        
        # 获取列名
        columns = list(self.data.columns)
        num_channels = len(columns)
        
        # 为每个切片创建单独的图形
        for slice_idx in range(display_slices):
            # 创建图形 - 每个通道一行，每行显示时域和频域
            fig, axes = plt.subplots(num_channels, 2, figsize=(16, 4*num_channels))
            if num_channels == 1:
                axes = axes.reshape(1, -1)
            
            fig.suptitle(f'Slice {slice_idx+1} FFT Analysis (Slice Angle: {slice_angle}°)', 
                        fontsize=16, fontweight='bold')
            
            # 绘制每个数据列
            for i, col in enumerate(columns):
                # 时域图
                ax_time = axes[i, 0]
                # 频域图
                ax_freq = axes[i, 1]
                
                start_idx = slice_idx * slice_points
                end_idx = min((slice_idx + 1) * slice_points, data_length)
                
                # 获取切片数据
                slice_data = self.data[col].iloc[start_idx:end_idx].values
                data_points = np.arange(start_idx, end_idx)
                
                # 时域绘制
                ax_time.plot(data_points, slice_data, 
                           color='blue', 
                           linewidth=1.5, 
                           alpha=0.8)
                
                # FFT分析
                if len(slice_data) > 4:  # 确保有足够的数据点进行FFT
                    # 去趋势
                    x = np.arange(len(slice_data))
                    trend = np.polyfit(x, slice_data, 1)
                    detrended_data = slice_data - (trend[0] * x + trend[1])
                    
                    # 应用窗函数
                    windowed_data = detrended_data * np.hanning(len(slice_data))
                    
                    # 零填充提高分辨率
                    if zero_padding_factor > 1:
                        padded_length = len(windowed_data) * zero_padding_factor
                        padded_data = np.zeros(padded_length)
                        padded_data[:len(windowed_data)] = windowed_data
                    else:
                        padded_data = windowed_data
                    
                    # 计算FFT
                    fft_data = np.fft.fft(padded_data)
                    
                    # 计算频率轴
                    if sampling_rate:
                        freqs = np.fft.fftfreq(len(padded_data), 1/sampling_rate)
                        freq_label = 'Frequency (Hz)'
                    else:
                        freqs = np.fft.fftfreq(len(padded_data))
                        freq_label = 'Normalized Frequency'
                    
                    # 只保留正频率部分
                    positive_freqs = freqs[:len(padded_data)//2]
                    positive_fft = fft_data[:len(padded_data)//2]
                    magnitude = np.abs(positive_fft) * 2 / len(padded_data)
                    
                    # 应用滤波
                    if filter_dc or (sampling_rate and filter_range):
                        # 创建滤波掩码
                        filter_mask = np.ones_like(positive_freqs, dtype=bool)
                        
                        # DC滤波（滤除0Hz附近的频率）
                        if filter_dc:
                            if sampling_rate:
                                # 滤除0-0.5Hz的直流分量
                                dc_cutoff = 0.5  # Hz
                                filter_mask &= (positive_freqs > dc_cutoff)
                            else:
                                # 归一化频率下滤除前几个频率点
                                filter_mask &= (positive_freqs > 0.01)
                        
                        # 指定频率范围滤波
                        if sampling_rate and filter_range:
                            low_cutoff, high_cutoff = filter_range
                            filter_mask &= (positive_freqs >= low_cutoff) & (positive_freqs <= high_cutoff)
                        
                        # 应用滤波
                        positive_freqs = positive_freqs[filter_mask]
                        magnitude = magnitude[filter_mask]
                    
                    # 频域绘制 - 使用线性坐标显示原始幅度
                    ax_freq.plot(positive_freqs, magnitude, 
                               color='red', 
                               linewidth=1.5, 
                               alpha=0.8)
                
                # 设置时域图
                ax_time.set_title(f'{col} - Time Domain (Slice {slice_idx+1})', fontsize=12, fontweight='bold')
                ax_time.set_xlabel('Data Points', fontsize=10)
                ax_time.set_ylabel(f'{col} Value', fontsize=10)
                ax_time.grid(True, alpha=0.3)
                
                # 设置频域图
                ax_freq.set_title(f'{col} - Frequency Domain (Slice {slice_idx+1})', fontsize=12, fontweight='bold')
                ax_freq.set_xlabel(freq_label, fontsize=10)
                ax_freq.set_ylabel('Magnitude', fontsize=10)
                ax_freq.grid(True, alpha=0.3)
                
                # 添加切片信息
                ax_time.text(0.02, 0.98, f'Slice {slice_idx+1}: Points {start_idx}-{end_idx-1}', 
                           transform=ax_time.transAxes, fontsize=10, 
                           verticalalignment='top', bbox=dict(boxstyle='round', facecolor='wheat', alpha=0.8))
            
            # 调整布局
            plt.tight_layout()
            
            # 保存图像
            if save_plot:
                output_file = os.path.join(self.images_folder, f'slice_{slice_idx+1}_fft_analysis_{slice_angle}deg_plot.png')
                plt.savefig(output_file, dpi=300, bbox_inches='tight')
                print(f"切片{slice_idx+1} FFT分析图像已保存为: {output_file}")
            
            # 显示图像
            if show_plot:
                plt.show()
        
        return True

    def plot_data_slices_by_angle(self, slice_angle=10.0, total_degrees=360, save_plot=True, show_plot=True):
        """
        绘制数据切片图像 - 按角度范围绘制，不同切片用不同颜色
        
        Args:
            slice_angle (float): 切片角度 (度)
            total_degrees (float): 总角度范围，默认360度
            save_plot (bool): 是否保存图像
            show_plot (bool): 是否显示图像
        """
        if self.data is None:
            print("请先加载数据!")
            return False
        
        # 设置字体
        plt.rcParams['font.family'] = 'DejaVu Sans'
        plt.rcParams['axes.unicode_minus'] = False
        
        # 计算切片参数
        data_length = len(self.data)
        slice_points = int(slice_angle * data_length / total_degrees)
        total_slices = data_length // slice_points
        
        if total_slices == 0:
            print(f"切片数必须大于0，请调整slice_angle参数!")
            return False
        
        print(f"数据切片分析:")
        print(f"  数据长度: {data_length}")
        print(f"  切片角度: {slice_angle} 度")
        print(f"  每切片点数: {slice_points}")
        print(f"  总切片数: {total_slices}")
        
        # 创建图形
        fig, axes = plt.subplots(3, 1, figsize=(15, 12))
        fig.suptitle(f'Data Slices by Angle (Slice Angle: {slice_angle}°)', fontsize=16, fontweight='bold')
        
        # 获取列名
        columns = list(self.data.columns)
        
        # 生成颜色映射
        colors = plt.cm.tab20(np.linspace(0, 1, total_slices))
        
        # 绘制每个数据列
        for i, col in enumerate(columns):
            ax = axes[i]
            
            # 绘制每个切片
            for slice_idx in range(total_slices):
                start_idx = slice_idx * slice_points
                end_idx = min((slice_idx + 1) * slice_points, data_length)
                
                # 计算角度范围
                start_angle = slice_idx * slice_angle
                end_angle = min((slice_idx + 1) * slice_angle, total_degrees)
                
                # 获取切片数据
                slice_data = self.data[col].iloc[start_idx:end_idx]
                angles = np.linspace(start_angle, end_angle, len(slice_data))
                
                # 绘制切片
                ax.plot(angles, slice_data, 
                       color=colors[slice_idx], 
                       linewidth=1.5, 
                       alpha=0.8, 
                       label=f'{start_angle:.0f}°-{end_angle:.0f}°' if slice_idx < 8 else "")
            
            # 设置标题和标签
            ax.set_title(f'{col} - Data Slices by Angle', fontsize=12, fontweight='bold')
            ax.set_xlabel('Angle (degrees)', fontsize=10)
            ax.set_ylabel(f'{col} Value', fontsize=10)
            
            # 设置网格
            ax.grid(True, alpha=0.3)
            
            # 添加图例（只显示前8个切片）
            if total_slices <= 8:
                ax.legend(fontsize=8, loc='upper right', ncol=2)
            else:
                ax.text(0.02, 0.98, f'Total Slices: {total_slices}', 
                       transform=ax.transAxes, fontsize=10, 
                       verticalalignment='top', bbox=dict(boxstyle='round', facecolor='wheat', alpha=0.8))
        
        # 调整布局
        plt.tight_layout()
        
        # 保存图像
        if save_plot:
            output_file = os.path.join(self.images_folder, f'data_slices_angle_{slice_angle}deg_plot.png')
            plt.savefig(output_file, dpi=300, bbox_inches='tight')
            print(f"数据切片角度图像已保存为: {output_file}")
        
        # 显示图像
        if show_plot:
            plt.show()
        
        return True

    def get_peak_to_peak_array(self, data=None, slice_angle=10.0, total_degrees=360, 
                              trim_ratio=0.1, method='trim'):
        """
        通用的峰峰值计算函数，可以接受任意输入数据
        
        Args:
            data (pd.DataFrame, np.array, list, or None): 输入数据
            slice_angle (float): 切片角度 (度)
            total_degrees (float): 总角度范围，默认360度
            trim_ratio (float): 修剪比例，默认0.1（修剪前后各10%）
            method (str): 峰峰值计算方法
                - 'trim': 修剪前后部分后计算峰峰值
                - 'robust': 使用中位数绝对偏差的鲁棒方法
                - 'percentile': 使用百分位数方法
                - 'fft': 使用FFT频域分析方法
                - 'fft_peak': 使用FFT频谱最大幅度值作为峰峰值
                - 'original': 原始方法（不修剪）
            
        Returns:
            dict or list: 峰峰值数据
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
        print(f"  峰峰值计算方法: {method}")
        if method == 'trim':
            print(f"  修剪比例: {trim_ratio*100:.1f}%")
        
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
                    peak_to_peak = self._calculate_robust_peak_to_peak(slice_data, method, trim_ratio)
                    peak_to_peak_values.append(peak_to_peak)
                else:
                    peak_to_peak_values.append(0)
            
            results[col] = peak_to_peak_values
        
        # 如果只有一列数据，直接返回列表
        if len(columns) == 1:
            self.data = results[columns[0]]
            return self.data
        else:
            self.data = results
            return self.data

    def _calculate_robust_peak_to_peak(self, data, method='trim', trim_ratio=0.1):
        """
        计算鲁棒的峰峰值
        
        Args:
            data: 输入数据
            method: 计算方法
            trim_ratio: 修剪比例
            
        Returns:
            float: 峰峰值
        """
        data = np.array(data)
        
        if method == 'original':
            # 原始方法
            return data.max() - data.min()
        
        elif method == 'trim':
            # 修剪前后部分
            n = len(data)
            trim_count = int(n * trim_ratio)
            if trim_count > 0:
                trimmed_data = data[trim_count:-trim_count] if trim_count < n//2 else data
            else:
                trimmed_data = data
            return trimmed_data.max() - trimmed_data.min()
        
        elif method == 'robust':
            # 鲁棒方法：使用中位数绝对偏差
            median = np.median(data)
            mad = np.median(np.abs(data - median))
            # 使用1.5倍MAD作为异常值阈值
            threshold = 1.5 * mad
            robust_data = data[np.abs(data - median) <= threshold]
            if len(robust_data) > 0:
                return robust_data.max() - robust_data.min()
            else:
                return data.max() - data.min()
        
        elif method == 'percentile':
            # 百分位数方法：使用5%-95%百分位数
            p5 = np.percentile(data, 5)
            p95 = np.percentile(data, 95)
            return p95 - p5
        
        elif method == 'fft':
            # FFT方法：通过频域分析计算峰峰值
            return self._calculate_fft_peak_to_peak(data)
        
        elif method == 'fft_peak':
            # FFT峰值方法：使用FFT频谱最大幅度值作为峰峰值
            return self._calculate_fft_peak_amplitude(data)
        
        else:
            # 默认使用原始方法
            return data.max() - data.min()

    def _calculate_fft_peak_to_peak(self, data, freq_threshold=0.1):
        """
        使用FFT方法计算峰峰值
        
        Args:
            data: 输入数据
            freq_threshold: 频率阈值，低于此频率的分量将被过滤
            
        Returns:
            float: 峰峰值
        """
        data = np.array(data)
        n = len(data)
        
        if n < 4:  # FFT需要至少4个点
            return data.max() - data.min()
        
        # 去趋势处理
        x = np.arange(n)
        trend = np.polyfit(x, data, 1)
        detrended_data = data - (trend[0] * x + trend[1])
        
        # 应用窗函数减少频谱泄漏
        windowed_data = detrended_data * np.hanning(n)
        
        # 计算FFT
        fft_data = np.fft.fft(windowed_data)
        freqs = np.fft.fftfreq(n)
        
        # 只保留正频率部分
        positive_freqs = freqs[:n//2]
        positive_fft = fft_data[:n//2]
        
        # 过滤低频噪声
        mask = positive_freqs > freq_threshold
        filtered_fft = positive_fft.copy()
        filtered_fft[~mask] = 0
        
        # 重构信号
        full_fft = np.zeros_like(fft_data)
        full_fft[:n//2] = filtered_fft
        # 处理奇数和偶数长度的不同情况
        if n % 2 == 0:
            full_fft[n//2:] = np.conj(filtered_fft[::-1])
        else:
            full_fft[n//2+1:] = np.conj(filtered_fft[::-1])
        
        # 逆FFT
        reconstructed = np.real(np.fft.ifft(full_fft))
        
        # 计算峰峰值
        return reconstructed.max() - reconstructed.min()

    def _calculate_fft_peak_amplitude(self, data, filter_dc=True, filter_range=(0, 20), 
                                    zero_padding_factor=4, sampling_rate=500):
        """
        使用FFT频谱最大幅度值作为峰峰值
        
        Args:
            data: 输入数据
            filter_dc: 是否滤除直流分量
            filter_range: 滤波范围 (Hz)
            zero_padding_factor: 零填充因子
            sampling_rate: 采样率 (Hz)
            
        Returns:
            float: FFT频谱最大幅度值
        """
        data = np.array(data)
        n = len(data)
        
        if n < 4:  # FFT需要至少4个点
            return data.max() - data.min()
        
        # 去趋势
        x = np.arange(n)
        trend = np.polyfit(x, data, 1)
        detrended_data = data - (trend[0] * x + trend[1])
        
        # 应用窗函数
        windowed_data = detrended_data * np.hanning(n)
        
        # 零填充提高分辨率
        if zero_padding_factor > 1:
            padded_length = n * zero_padding_factor
            padded_data = np.zeros(padded_length)
            padded_data[:n] = windowed_data
        else:
            padded_data = windowed_data
        
        # 计算FFT
        fft_data = np.fft.fft(padded_data)
        
        # 计算频率轴
        freqs = np.fft.fftfreq(len(padded_data), 1/sampling_rate)
        
        # 只保留正频率部分
        positive_freqs = freqs[:len(padded_data)//2]
        positive_fft = fft_data[:len(padded_data)//2]
        magnitude = np.abs(positive_fft) * 2 / len(padded_data)
        
        # 应用滤波
        if filter_dc or filter_range:
            # 创建滤波掩码
            filter_mask = np.ones_like(positive_freqs, dtype=bool)
            
            # DC滤波
            if filter_dc:
                dc_cutoff = 0.5  # Hz
                filter_mask &= (positive_freqs > dc_cutoff)
            
            # 指定频率范围滤波
            if filter_range:
                low_cutoff, high_cutoff = filter_range
                filter_mask &= (positive_freqs >= low_cutoff) & (positive_freqs <= high_cutoff)
            
            # 应用滤波
            magnitude = magnitude[filter_mask]
        
        # 返回最大幅度值
        if len(magnitude) > 0:
            return np.max(magnitude)
        else:
            return data.max() - data.min()

    def _calculate_fft_peak_to_peak_advanced(self, data, low_freq_cutoff=0.05, high_freq_cutoff=0.5):
        """
        高级FFT方法计算峰峰值
        
        Args:
            data: 输入数据
            low_freq_cutoff: 低频截止频率
            high_freq_cutoff: 高频截止频率
            
        Returns:
            float: 峰峰值
        """
        data = np.array(data)
        n = len(data)
        
        if n < 8:  # 需要足够的点进行FFT分析
            return data.max() - data.min()
        
        # 去趋势
        x = np.arange(n)
        trend = np.polyfit(x, data, 1)
        detrended_data = data - (trend[0] * x + trend[1])
        
        # 应用窗函数
        windowed_data = detrended_data * np.hanning(n)
        
        # FFT
        fft_data = np.fft.fft(windowed_data)
        freqs = np.fft.fftfreq(n)
        
        # 带通滤波
        mask = (np.abs(freqs) >= low_freq_cutoff) & (np.abs(freqs) <= high_freq_cutoff)
        filtered_fft = fft_data * mask
        
        # 逆FFT
        filtered_signal = np.real(np.fft.ifft(filtered_fft))
        
        # 计算峰峰值
        return filtered_signal.max() - filtered_signal.min()

    def plot_peak_to_peak_array(self, peak_to_peak_data=None, slice_angle=10.0, save_plot=True, show_plot=True, title="Peak-to-Peak Array Visualization"):
        """
        绘制峰峰值数组的图像，每个通道绘制在独立的子图中
        
        Args:
            peak_to_peak_data (list, dict, or array): 峰峰值数据
                - 如果是list或array，绘制单条曲线
                - 如果是dict，绘制多条曲线（每条一个key，每个key一个子图）
            slice_angle (float): 切片角度 (度)，用于创建x轴
            save_plot (bool): 是否保存图像
            show_plot (bool): 是否显示图像
            title (str): 图像标题
            
        Returns:
            bool: 是否成功绘制
        """
        peak_to_peak_data = self.data
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
        
        # 计算子图布局
        num_channels = len(data_dict)
        
        # 根据通道数量确定子图布局
        if num_channels == 1:
            rows, cols = 1, 1
        elif num_channels == 2:
            rows, cols = 1, 2
        elif num_channels <= 4:
            rows, cols = 2, 2
        elif num_channels <= 6:
            rows, cols = 2, 3
        elif num_channels <= 9:
            rows, cols = 3, 3
        else:
            rows, cols = 4, 3  # 最多12个子图
        
        # 创建图形和子图
        fig, axes = plt.subplots(rows, cols, figsize=(4*cols, 3*rows))
        
        # 如果只有一个子图，确保axes是数组格式
        if num_channels == 1:
            axes = [axes]
        elif rows == 1 or cols == 1:
            axes = axes.flatten()
        else:
            axes = axes.flatten()
        
        # 绘制每个通道的峰峰值曲线
        colors = ['red', 'green', 'blue', 'orange', 'purple', 'brown', 'pink', 'gray', 'olive', 'cyan']
        
        for i, (channel, values) in enumerate(data_dict.items()):
            if i >= len(axes):
                break
                
            ax = axes[i]
            
            # 计算x轴角度值
            angles = np.arange(len(values)) * slice_angle
            
            # 选择颜色
            color = colors[i % len(colors)]
            
            # 绘制曲线
            ax.plot(angles, values, color=color, linewidth=2, marker='o', 
                   markersize=5, alpha=0.8)
            
            # 设置每个子图的标题和标签
            ax.set_title(f'{channel}', fontsize=12, fontweight='bold')
            ax.set_xlabel('Angle (degrees)', fontsize=10)
            ax.set_ylabel('Peak-to-Peak Value', fontsize=10)
            
            # 设置网格
            ax.grid(True, alpha=0.3, linestyle='--')
            
            # 设置坐标轴
            ax.tick_params(axis='both', which='major', labelsize=9)
        
        # 隐藏多余的子图
        for i in range(num_channels, len(axes)):
            axes[i].set_visible(False)
        
        # 设置总标题
        fig.suptitle(title, fontsize=14, fontweight='bold')
        
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

    def find_amplitude_crossing_points(self, data, target_amplitude, x_range=(0, 360), tolerance=0.01):
        """
        找到数据中特定幅度值对应的所有x轴位置
        
        Args:
            data: 峰峰值数据（一维数组）
            target_amplitude: 目标幅度值
            x_range: x轴范围，默认(0, 360)
            tolerance: 容差，用于判断是否接近目标幅度
            
        Returns:
            list: 目标幅度对应的x轴位置列表
        """
        data = np.array(data)
        x = np.linspace(x_range[0], x_range[1], len(data))
        
        # 使用插值方法找到所有交叉点
        crossing_points = []
        
        # 找到数据跨越目标幅度的区间
        for i in range(len(data) - 1):
            # 检查是否跨越目标幅度
            if (data[i] <= target_amplitude <= data[i + 1]) or (data[i] >= target_amplitude >= data[i + 1]):
                # 线性插值计算精确位置
                if data[i + 1] != data[i]:  # 避免除零
                    t = (target_amplitude - data[i]) / (data[i + 1] - data[i])
                    x_interp = x[i] + t * (x[i + 1] - x[i])
                    crossing_points.append(x_interp)
        
        # 如果插值没有找到足够的点，尝试更宽松的容差
        if len(crossing_points) < 2:
            # 使用更宽松的容差查找接近的点
            mask = np.abs(data - target_amplitude) <= tolerance * 5  # 放宽5倍容差
            crossing_indices = np.where(mask)[0]
            
            if len(crossing_indices) > 0:
                # 对找到的点进行分组，避免重复
                groups = []
                current_group = [crossing_indices[0]]
                
                for i in range(1, len(crossing_indices)):
                    if crossing_indices[i] - crossing_indices[i-1] <= 3:  # 连续的点归为一组
                        current_group.append(crossing_indices[i])
                    else:
                        groups.append(current_group)
                        current_group = [crossing_indices[i]]
                groups.append(current_group)
                
                # 每组取中点作为交叉点
                for group in groups:
                    mid_idx = group[len(group)//2]
                    crossing_points.append(x[mid_idx])
        
        # 去重并排序
        crossing_points = sorted(list(set(crossing_points)))
        
        return crossing_points

    def plot_amplitude_crossing_points(self, peak_to_peak_data=None, n_divisions=2, slice_angle=10.0, 
                                     x_range=(0, 360), save_plot=True, show_plot=True, 
                                     title="Amplitude Crossing Points Analysis"):
        """
        绘制峰峰值数据的幅度等分点
        
        Args:
            peak_to_peak_data: 峰峰值数据（dict或list）
            n_divisions: 等分数，如2表示二等分
            slice_angle: 切片角度
            x_range: x轴范围
            save_plot: 是否保存图像
            show_plot: 是否显示图像
            title: 图像标题
        """
        if peak_to_peak_data is None:
            peak_to_peak_data = self.data
        
        # 转换数据格式
        if isinstance(peak_to_peak_data, dict):
            data_dict = peak_to_peak_data
            is_multi_channel = True
        else:
            data_dict = {'data': list(peak_to_peak_data)}
            is_multi_channel = False
        
        if not data_dict:
            print("峰峰值数据为空!")
            return False
        
        # 设置字体
        plt.rcParams['font.family'] = 'DejaVu Sans'
        plt.rcParams['axes.unicode_minus'] = False
        
        # 计算子图布局 - 改为多行显示
        num_channels = len(data_dict)
        
        # 每个通道占一行
        rows = num_channels
        cols = 1
        
        # 创建图形和子图
        fig, axes = plt.subplots(rows, cols, figsize=(12, 3*rows))
        
        # 如果只有一个子图，确保axes是数组格式
        if num_channels == 1:
            axes = [axes]
        elif rows == 1 or cols == 1:
            axes = axes.flatten()
        else:
            axes = axes.flatten()
        
        # 处理每个通道
        for i, (channel, values) in enumerate(data_dict.items()):
            if i >= len(axes):
                break
                
            ax = axes[i]
            values = np.array(values)
            
            # 计算x轴（角度）
            x = np.linspace(x_range[0], x_range[1], len(values))
            
            # 绘制原始数据
            ax.plot(x, values, 'b-', linewidth=2, alpha=0.7, label='Peak-to-Peak Data')
            
            # 计算等分幅度值
            min_val = np.min(values)
            max_val = np.max(values)
            
            print(f"\n{channel} 幅度范围: {min_val:.3f} - {max_val:.3f}")
            
            # 计算等分点
            division_amplitudes = []
            crossing_points_all = []
            
            for div in range(1, n_divisions):
                target_amp = min_val + (max_val - min_val) * div / n_divisions
                division_amplitudes.append(target_amp)
                
                # 找到交叉点
                crossing_points = self.find_amplitude_crossing_points(
                    values, target_amp, x_range, tolerance=0.01
                )
                crossing_points_all.append(crossing_points)
                
                print(f"  等分点 {div}/{n_divisions} (幅度={target_amp:.3f}): {len(crossing_points)}个位置")
                if crossing_points:
                    print(f"    位置: {[f'{p:.1f}°' for p in crossing_points]}")
                
                # 绘制等分线和交叉点
                ax.axhline(y=target_amp, color='red', linestyle='--', alpha=0.7, 
                          label=f'Division {div}/{n_divisions}' if div == 1 else "")
                
                if crossing_points:
                    ax.scatter(crossing_points, [target_amp] * len(crossing_points), 
                             c='red', s=50, marker='o', alpha=0.8, zorder=5)
            
            # 设置子图属性
            ax.set_title(f'{channel} - Amplitude Division Points', fontsize=12, fontweight='bold')
            ax.set_xlabel('Angle (degrees)', fontsize=10)
            ax.set_ylabel('Peak-to-Peak Value', fontsize=10)
            ax.grid(True, alpha=0.3, linestyle='--')
            ax.legend(fontsize=8, loc='best')
            ax.tick_params(axis='both', which='major', labelsize=9)
        
        # 隐藏多余的子图
        for i in range(num_channels, len(axes)):
            axes[i].set_visible(False)
        
        # 设置总标题
        fig.suptitle(title, fontsize=14, fontweight='bold')
        
        # 调整布局
        plt.tight_layout()
        
        # 保存图像
        if save_plot:
            output_file = os.path.join(self.images_folder, 'amplitude_crossing_points.png')
            plt.savefig(output_file, dpi=300, bbox_inches='tight')
            print(f"幅度等分点可视化图像已保存为: {output_file}")
        
        # 显示图像
        if show_plot:
            plt.show()
        
        return True

def test_amplitude_crossing_points():
    """测试幅度等分点功能"""
    csv_file = "/home/xwj/0-code/0-github/motor/python/data_motor/Bxyz_t_4s_10degrees.csv" #slice_angle=10
    # csv_file = "/home/xwj/0-code/0-github/motor/python/data_motor/Bxyz_0.csv" #slice_angle=4.0
    # csv_file = "/home/xwj/0-code/0-github/motor/python/data_motor/Bxyz_t_4s_29_144_212_334.csv"  #slice_angle=11.8
    # csv_file = "/home/xwj/0-code/0-github/motor/python/data_motor/Bxyz_t_4s_QAM_29_144_210p6_332.csv"  #slice_angle=9.93, method='trim', trim_ratio=0.15
    csv_file = "/home/xwj/0-code/0-github/motor/python/data_motor/qam16_1.csv" #slice_angle= 8.36
    csv_file = "/home/xwj/0-code/0-github/motor/python/data_motor/qam16_2.csv" #slice_angle= 8.36
    slice_angle = 7.485
    processor = DataProcessor(csv_file)
    processor.load_data()
    
    # processor.plot_data_points(save_plot=True, show_plot=True)
    
    # 绘制数据切片
    processor.plot_data_slices(slice_angle=slice_angle, save_plot=True, show_plot=False)
    # processor.plot_data_slices_by_angle(slice_angle=1.0, save_plot=True, show_plot=True)
    
    # 绘制切片FFT分析
    # 假设采样率为500Hz（你可以根据实际情况调整）
    sampling_rate = 500  # Hz
    processor.plot_slice_fft_analysis(
        slice_angle=slice_angle, 
        max_slices=0, 
        sampling_rate=sampling_rate,
        filter_dc=True,                    # 滤除直流分量
        filter_range=(0, 20),          # 滤波范围：0.5-200Hz
        zero_padding_factor=4,            # 4倍零填充提高分辨率
        save_plot=True, 
        show_plot=False
    )
    
    # 测试FFT峰值方法
    print("\n=== 测试FFT峰值方法 ===")
    # 重新加载数据避免self.data被覆盖
    processor_fft_peak = DataProcessor(csv_file)
    processor_fft_peak.load_data()
    peak_to_peak_result_fft_peak = processor_fft_peak.get_peak_to_peak_array(slice_angle=slice_angle, method='fft_peak')
    # peak_to_peak_result_fft_peak = processor_fft_peak.get_peak_to_peak_array(slice_angle=slice_angle, method='trim', trim_ratio=0.15)
    processor_fft_peak.plot_peak_to_peak_array(peak_to_peak_data=peak_to_peak_result_fft_peak, 
                                    save_plot=True, show_plot=False, 
                                    title="Peak-to-Peak Analysis - FFT Peak Method")
    
    # 使用FFT方法进行最终分析
    # peak_to_peak_result = processor.get_peak_to_peak_array(slice_angle=slice_angle, method='fft')
    # processor.plot_peak_to_peak_array(peak_to_peak_data=peak_to_peak_result, save_plot=True, show_plot=True)
    
    # 绘制幅度等分点
    processor.plot_amplitude_crossing_points(
        peak_to_peak_data=peak_to_peak_result_fft_peak,
        n_divisions=5,  # 四等分
        slice_angle=slice_angle,
        x_range=(0, 360),
        save_plot=True,
        show_plot=True,
        title="Peak-to-Peak Data Amplitude Division Points"
    )

def test_with_real_calculation():
    """测试使用真实计算得到的峰峰值数据"""
    print("=" * 60)
    print("测试3: 使用真实计算得到的峰峰值数据")
    print("=" * 60)
    
    try:
        # 加载真实数据
        csv_file = "/home/xwj/0-code/0-github/motor/python/data_motor/Bxyz_t_4s_10degrees.csv"
        processor = DataProcessor(csv_file)
        processor.load_data()
        
        # 计算峰峰值
        result = processor.get_peak_to_peak_array(slice_angle=10.0)
        
        print(f"计算得到的数据类型: {type(result)}")
        if isinstance(result, dict):
            print(f"通道数量: {len(result)}")
            for key, values in result.items():
                print(f"  {key}: {len(values)}个数据点")
        
        # 绘制
        processor.plot_peak_to_peak_array(
            # peak_to_peak_data=None,
            slice_angle=10.0,
            save_plot=True,
            show_plot=True,
            title="真实数据峰峰值分析"
        )
        print("绘图完成！\n")
        
    except Exception as e:
        print(f"出错: {e}\n")

if __name__ == "__main__":
    test_amplitude_crossing_points()
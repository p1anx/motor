
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

    def plot_qam_constellation(self, i_channel='data1', q_channel='data2', 
                              slice_angle=None, max_points=10000, 
                              save_plot=True, show_plot=True, title="QAM Constellation Diagram"):
        """
        绘制QAM星座图 - 基于I/Q分量的真正QAM调制解调星座图
        
        Args:
            i_channel (str): I通道（同相分量）列名，默认为'data1'
            q_channel (str): Q通道（正交分量）列名，默认为'data2'
            slice_angle (float): 切片角度，如果为None则使用全部数据
            max_points (int): 最大显示点数，避免图像过于密集
            save_plot (bool): 是否保存图像
            show_plot (bool): 是否显示图像
            title (str): 图像标题
        """
        if self.data is None:
            print("请先加载数据!")
            return False
        
        # 检查列名是否存在
        if i_channel not in self.data.columns:
            print(f"列名 '{i_channel}' 不存在! 可用列名: {list(self.data.columns)}")
            return False
        if q_channel not in self.data.columns:
            print(f"列名 '{q_channel}' 不存在! 可用列名: {list(self.data.columns)}")
            return False
        
        # 设置字体
        plt.rcParams['font.family'] = 'DejaVu Sans'
        plt.rcParams['axes.unicode_minus'] = False
        
        # 获取数据
        if slice_angle is not None:
            # 使用切片数据
            data_length = len(self.data)
            slice_points = int(slice_angle * data_length / 360)
            total_slices = data_length // slice_points
            display_slices = min(total_slices, 1)  # 只显示第一个切片
            
            start_idx = 0
            end_idx = min(slice_points, data_length)
            i_data = self.data[i_channel].iloc[start_idx:end_idx].values
            q_data = self.data[q_channel].iloc[start_idx:end_idx].values
            
            print(f"QAM星座图分析 (切片模式):")
            print(f"  切片角度: {slice_angle} 度")
            print(f"  数据点数: {len(i_data)}")
        else:
            # 使用全部数据
            i_data = self.data[i_channel].values
            q_data = self.data[q_channel].values
            
            print(f"QAM星座图分析 (全数据模式):")
            print(f"  数据点数: {len(i_data)}")
        
        # 限制显示点数
        if len(i_data) > max_points:
            step = len(i_data) // max_points
            i_data = i_data[::step]
            q_data = q_data[::step]
            print(f"  显示点数: {len(i_data)} (从{len(self.data)}点中采样)")
        
        # 计算幅度和相位
        complex_data = i_data + 1j * q_data
        amplitude = np.abs(complex_data)
        phase = np.angle(complex_data)
        
        # 创建图形
        fig, axes = plt.subplots(2, 2, figsize=(16, 12))
        
        # 左上图：I-Q星座图
        ax1 = axes[0, 0]
        scatter1 = ax1.scatter(i_data, q_data, c=amplitude, 
                             cmap='viridis', alpha=0.6, s=2)
        ax1.set_xlabel(f'{i_channel} (I - In-phase)', fontsize=12)
        ax1.set_ylabel(f'{q_channel} (Q - Quadrature)', fontsize=12)
        ax1.set_title(f'{title} - I-Q Constellation', fontsize=14, fontweight='bold')
        ax1.grid(True, alpha=0.3)
        ax1.set_aspect('equal', adjustable='box')
        
        # 添加颜色条
        cbar1 = plt.colorbar(scatter1, ax=ax1)
        cbar1.set_label('Amplitude', fontsize=10)
        
        # 右上图：极坐标星座图
        ax2 = axes[0, 1]
        scatter2 = ax2.scatter(phase, amplitude, c=range(len(phase)), 
                             cmap='plasma', alpha=0.6, s=2)
        ax2.set_xlabel('Phase (radians)', fontsize=12)
        ax2.set_ylabel('Amplitude', fontsize=12)
        ax2.set_title(f'{title} - Polar Constellation', fontsize=14, fontweight='bold')
        ax2.grid(True, alpha=0.3)
        
        # 添加颜色条
        cbar2 = plt.colorbar(scatter2, ax=ax2)
        cbar2.set_label('Data Point Index', fontsize=10)
        
        # 左下图：幅度分布
        ax3 = axes[1, 0]
        ax3.hist(amplitude, bins=50, alpha=0.7, color='blue', edgecolor='black')
        ax3.set_xlabel('Amplitude', fontsize=12)
        ax3.set_ylabel('Frequency', fontsize=12)
        ax3.set_title(f'{title} - Amplitude Distribution', fontsize=14, fontweight='bold')
        ax3.grid(True, alpha=0.3)
        
        # 右下图：相位分布
        ax4 = axes[1, 1]
        ax4.hist(phase, bins=50, alpha=0.7, color='red', edgecolor='black')
        ax4.set_xlabel('Phase (radians)', fontsize=12)
        ax4.set_ylabel('Frequency', fontsize=12)
        ax4.set_title(f'{title} - Phase Distribution', fontsize=14, fontweight='bold')
        ax4.grid(True, alpha=0.3)
        
        # 添加统计信息
        i_mean, i_std = np.mean(i_data), np.std(i_data)
        q_mean, q_std = np.mean(q_data), np.std(q_data)
        amp_mean, amp_std = np.mean(amplitude), np.std(amplitude)
        phase_mean, phase_std = np.mean(phase), np.std(phase)
        
        stats_text = f'QAM Statistics:\n'
        stats_text += f'I: μ={i_mean:.3f}, σ={i_std:.3f}\n'
        stats_text += f'Q: μ={q_mean:.3f}, σ={q_std:.3f}\n'
        stats_text += f'Amplitude: μ={amp_mean:.3f}, σ={amp_std:.3f}\n'
        stats_text += f'Phase: μ={phase_mean:.3f}, σ={phase_std:.3f}\n'
        stats_text += f'Points: {len(i_data)}'
        
        ax1.text(0.02, 0.98, stats_text, transform=ax1.transAxes, 
                fontsize=9, verticalalignment='top',
                bbox=dict(boxstyle='round', facecolor='wheat', alpha=0.8))
        
        # 调整布局
        plt.tight_layout()
        
        # 保存图像
        if save_plot:
            if slice_angle is not None:
                output_file = os.path.join(self.images_folder, f'qam_constellation_{slice_angle}deg_plot.png')
            else:
                output_file = os.path.join(self.images_folder, 'qam_constellation_full_plot.png')
            plt.savefig(output_file, dpi=300, bbox_inches='tight')
            print(f"QAM星座图已保存为: {output_file}")
        
        # 显示图像
        if show_plot:
            plt.show()
        
        return True

    def generate_qam_signal(self, data_bits, modulation_order=16, sampling_rate=1000, 
                           symbol_rate=100, carrier_freq=100):
        """
        生成QAM调制信号
        
        Args:
            data_bits (array): 输入的数字比特数据
            modulation_order (int): 调制阶数 (4, 16, 64, 256)
            sampling_rate (int): 采样率 (Hz)
            symbol_rate (int): 符号率 (symbols/sec)
            carrier_freq (int): 载波频率 (Hz)
        
        Returns:
            tuple: (i_signal, q_signal, complex_signal, symbols)
        """
        # 根据调制阶数确定每符号比特数
        bits_per_symbol = int(np.log2(modulation_order))
        
        # 确保数据长度是bits_per_symbol的倍数
        if len(data_bits) % bits_per_symbol != 0:
            # 填充到最近的倍数
            padding = bits_per_symbol - (len(data_bits) % bits_per_symbol)
            data_bits = np.concatenate([data_bits, np.zeros(padding, dtype=int)])
        
        # 将比特分组为符号
        num_symbols = len(data_bits) // bits_per_symbol
        symbol_bits = data_bits.reshape(num_symbols, bits_per_symbol)
        
        # 生成QAM星座点
        if modulation_order == 4:
            # QPSK
            constellation = np.array([1+1j, -1+1j, -1-1j, 1-1j]) / np.sqrt(2)
        elif modulation_order == 16:
            # 16-QAM
            constellation = np.array([
                1+1j, 1+3j, 3+1j, 3+3j,
                -1+1j, -1+3j, -3+1j, -3+3j,
                -1-1j, -1-3j, -3-1j, -3-3j,
                1-1j, 1-3j, 3-1j, 3-3j
            ]) / np.sqrt(10)
        elif modulation_order == 64:
            # 64-QAM
            constellation = np.array([
                1+1j, 1+3j, 1+5j, 1+7j, 3+1j, 3+3j, 3+5j, 3+7j,
                5+1j, 5+3j, 5+5j, 5+7j, 7+1j, 7+3j, 7+5j, 7+7j,
                -1+1j, -1+3j, -1+5j, -1+7j, -3+1j, -3+3j, -3+5j, -3+7j,
                -5+1j, -5+3j, -5+5j, -5+7j, -7+1j, -7+3j, -7+5j, -7+7j,
                -1-1j, -1-3j, -1-5j, -1-7j, -3-1j, -3-3j, -3-5j, -3-7j,
                -5-1j, -5-3j, -5-5j, -5-7j, -7-1j, -7-3j, -7-5j, -7-7j,
                1-1j, 1-3j, 1-5j, 1-7j, 3-1j, 3-3j, 3-5j, 3-7j,
                5-1j, 5-3j, 5-5j, 5-7j, 7-1j, 7-3j, 7-5j, 7-7j
            ]) / np.sqrt(42)
        else:
            raise ValueError(f"不支持的调制阶数: {modulation_order}")
        
        # 将符号比特转换为星座点索引
        symbol_indices = []
        for i in range(num_symbols):
            index = 0
            for j in range(bits_per_symbol):
                index += symbol_bits[i, j] * (2 ** (bits_per_symbol - 1 - j))
            symbol_indices.append(index)
        
        # 获取对应的星座点
        symbols = constellation[symbol_indices]
        
        # 上采样和脉冲整形
        samples_per_symbol = sampling_rate // symbol_rate
        upsampled_i = np.zeros(num_symbols * samples_per_symbol)
        upsampled_q = np.zeros(num_symbols * samples_per_symbol)
        
        for i, symbol in enumerate(symbols):
            start_idx = i * samples_per_symbol
            end_idx = start_idx + samples_per_symbol
            upsampled_i[start_idx:end_idx] = symbol.real
            upsampled_q[start_idx:end_idx] = symbol.imag
        
        # 应用根升余弦滤波器
        alpha = 0.35  # 滚降因子
        span = 6  # 滤波器跨度
        sps = samples_per_symbol
        
        # 生成根升余弦滤波器
        t = np.arange(-span * sps, span * sps + 1) / sps
        h = np.zeros_like(t)
        
        for i, time in enumerate(t):
            if time == 0:
                h[i] = 1 - alpha + 4 * alpha / np.pi
            elif abs(time) == 1 / (4 * alpha):
                h[i] = (alpha / np.sqrt(2)) * ((1 + 2 / np.pi) * np.sin(np.pi / (4 * alpha)) + 
                                              (1 - 2 / np.pi) * np.cos(np.pi / (4 * alpha)))
            else:
                numerator = np.sin(np.pi * time * (1 - alpha)) + 4 * alpha * time * np.cos(np.pi * time * (1 + alpha))
                denominator = np.pi * time * (1 - (4 * alpha * time) ** 2)
                h[i] = numerator / denominator
        
        # 归一化滤波器
        h = h / np.sqrt(np.sum(h ** 2))
        
        # 应用滤波器
        filtered_i = np.convolve(upsampled_i, h, mode='same')
        filtered_q = np.convolve(upsampled_q, h, mode='same')
        
        # 生成载波
        t = np.arange(len(filtered_i)) / sampling_rate
        carrier_i = np.cos(2 * np.pi * carrier_freq * t)
        carrier_q = -np.sin(2 * np.pi * carrier_freq * t)
        
        # 调制到载波
        i_signal = filtered_i * carrier_i
        q_signal = filtered_q * carrier_q
        
        # 复数信号
        complex_signal = i_signal + 1j * q_signal
        
        return i_signal, q_signal, complex_signal, symbols

    def demodulate_qam_signal(self, i_signal, q_signal, modulation_order=16, 
                             sampling_rate=1000, symbol_rate=100, carrier_freq=100):
        """
        解调QAM信号
        
        Args:
            i_signal (array): I通道信号
            q_signal (array): Q通道信号
            modulation_order (int): 调制阶数
            sampling_rate (int): 采样率
            symbol_rate (int): 符号率
            carrier_freq (int): 载波频率
        
        Returns:
            tuple: (demodulated_i, demodulated_q, complex_signal, symbols)
        """
        # 生成载波
        t = np.arange(len(i_signal)) / sampling_rate
        carrier_i = np.cos(2 * np.pi * carrier_freq * t)
        carrier_q = -np.sin(2 * np.pi * carrier_freq * t)
        
        # 解调
        demod_i = i_signal * carrier_i
        demod_q = q_signal * carrier_q
        
        # 复数信号
        complex_signal = demod_i + 1j * demod_q
        
        # 应用低通滤波器去除高频分量
        from scipy import signal
        
        # 设计低通滤波器
        nyquist = sampling_rate / 2
        cutoff = symbol_rate / 2
        b, a = signal.butter(4, cutoff / nyquist, btype='low')
        
        # 滤波
        filtered_i = signal.filtfilt(b, a, demod_i)
        filtered_q = signal.filtfilt(b, a, demod_q)
        
        # 下采样到符号率
        samples_per_symbol = sampling_rate // symbol_rate
        downsampled_i = filtered_i[::samples_per_symbol]
        downsampled_q = filtered_q[::samples_per_symbol]
        
        # 复数符号
        symbols = downsampled_i + 1j * downsampled_q
        
        return downsampled_i, downsampled_q, symbols

    def plot_qam_constellation(self, i_channel='data1', q_channel='data2', 
                              slice_angle=None, max_points=10000, 
                              save_plot=True, show_plot=True, title="QAM Constellation Diagram"):
        """
        绘制QAM星座图 - 基于I/Q分量的真正QAM调制解调星座图
        
        Args:
            i_channel (str): I通道（同相分量）列名，默认为'data1'
            q_channel (str): Q通道（正交分量）列名，默认为'data2'
            slice_angle (float): 切片角度，如果为None则使用全部数据
            max_points (int): 最大显示点数，避免图像过于密集
            save_plot (bool): 是否保存图像
            show_plot (bool): 是否显示图像
            title (str): 图像标题
        """
        if self.data is None:
            print("请先加载数据!")
            return False
        
        # 检查列名是否存在
        if i_channel not in self.data.columns:
            print(f"列名 '{i_channel}' 不存在! 可用列名: {list(self.data.columns)}")
            return False
        if q_channel not in self.data.columns:
            print(f"列名 '{q_channel}' 不存在! 可用列名: {list(self.data.columns)}")
            return False
        
        # 设置字体
        plt.rcParams['font.family'] = 'DejaVu Sans'
        plt.rcParams['axes.unicode_minus'] = False
        
        # 获取数据
        if slice_angle is not None:
            # 使用切片数据
            data_length = len(self.data)
            slice_points = int(slice_angle * data_length / 360)
            total_slices = data_length // slice_points
            display_slices = min(total_slices, 1)  # 只显示第一个切片
            
            start_idx = 0
            end_idx = min(slice_points, data_length)
            i_data = self.data[i_channel].iloc[start_idx:end_idx].values
            q_data = self.data[q_channel].iloc[start_idx:end_idx].values
            
            print(f"QAM星座图分析 (切片模式):")
            print(f"  切片角度: {slice_angle} 度")
            print(f"  数据点数: {len(i_data)}")
        else:
            # 使用全部数据
            i_data = self.data[i_channel].values
            q_data = self.data[q_channel].values
            
            print(f"QAM星座图分析 (全数据模式):")
            print(f"  数据点数: {len(i_data)}")
        
        # 限制显示点数
        if len(i_data) > max_points:
            step = len(i_data) // max_points
            i_data = i_data[::step]
            q_data = q_data[::step]
            print(f"  显示点数: {len(i_data)} (从{len(self.data)}点中采样)")
        
        # 计算幅度和相位
        complex_data = i_data + 1j * q_data
        amplitude = np.abs(complex_data)
        phase = np.angle(complex_data)
        
        # 创建图形
        fig, axes = plt.subplots(2, 2, figsize=(16, 12))
        
        # 左上图：I-Q星座图
        ax1 = axes[0, 0]
        scatter1 = ax1.scatter(i_data, q_data, c=amplitude, 
                             cmap='viridis', alpha=0.6, s=2)
        ax1.set_xlabel(f'{i_channel} (I - In-phase)', fontsize=12)
        ax1.set_ylabel(f'{q_channel} (Q - Quadrature)', fontsize=12)
        ax1.set_title(f'{title} - I-Q Constellation', fontsize=14, fontweight='bold')
        ax1.grid(True, alpha=0.3)
        ax1.set_aspect('equal', adjustable='box')
        
        # 添加颜色条
        cbar1 = plt.colorbar(scatter1, ax=ax1)
        cbar1.set_label('Amplitude', fontsize=10)
        
        # 右上图：极坐标星座图
        ax2 = axes[0, 1]
        scatter2 = ax2.scatter(phase, amplitude, c=range(len(phase)), 
                             cmap='plasma', alpha=0.6, s=2)
        ax2.set_xlabel('Phase (radians)', fontsize=12)
        ax2.set_ylabel('Amplitude', fontsize=12)
        ax2.set_title(f'{title} - Polar Constellation', fontsize=14, fontweight='bold')
        ax2.grid(True, alpha=0.3)
        
        # 添加颜色条
        cbar2 = plt.colorbar(scatter2, ax=ax2)
        cbar2.set_label('Data Point Index', fontsize=10)
        
        # 左下图：幅度分布
        ax3 = axes[1, 0]
        ax3.hist(amplitude, bins=50, alpha=0.7, color='blue', edgecolor='black')
        ax3.set_xlabel('Amplitude', fontsize=12)
        ax3.set_ylabel('Frequency', fontsize=12)
        ax3.set_title(f'{title} - Amplitude Distribution', fontsize=14, fontweight='bold')
        ax3.grid(True, alpha=0.3)
        
        # 右下图：相位分布
        ax4 = axes[1, 1]
        ax4.hist(phase, bins=50, alpha=0.7, color='red', edgecolor='black')
        ax4.set_xlabel('Phase (radians)', fontsize=12)
        ax4.set_ylabel('Frequency', fontsize=12)
        ax4.set_title(f'{title} - Phase Distribution', fontsize=14, fontweight='bold')
        ax4.grid(True, alpha=0.3)
        
        # 添加统计信息
        i_mean, i_std = np.mean(i_data), np.std(i_data)
        q_mean, q_std = np.mean(q_data), np.std(q_data)
        amp_mean, amp_std = np.mean(amplitude), np.std(amplitude)
        phase_mean, phase_std = np.mean(phase), np.std(phase)
        
        stats_text = f'QAM Statistics:\n'
        stats_text += f'I: μ={i_mean:.3f}, σ={i_std:.3f}\n'
        stats_text += f'Q: μ={q_mean:.3f}, σ={q_std:.3f}\n'
        stats_text += f'Amplitude: μ={amp_mean:.3f}, σ={amp_std:.3f}\n'
        stats_text += f'Phase: μ={phase_mean:.3f}, σ={phase_std:.3f}\n'
        stats_text += f'Points: {len(i_data)}'
        
        ax1.text(0.02, 0.98, stats_text, transform=ax1.transAxes, 
                fontsize=9, verticalalignment='top',
                bbox=dict(boxstyle='round', facecolor='wheat', alpha=0.8))
        
        # 调整布局
        plt.tight_layout()
        
        # 保存图像
        if save_plot:
            if slice_angle is not None:
                output_file = os.path.join(self.images_folder, f'qam_constellation_{slice_angle}deg_plot.png')
            else:
                output_file = os.path.join(self.images_folder, 'qam_constellation_full_plot.png')
            plt.savefig(output_file, dpi=300, bbox_inches='tight')
            print(f"QAM星座图已保存为: {output_file}")
        
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
    
    # 绘制QAM星座图
    print("\n=== 绘制QAM星座图 ===")
    processor.plot_qam_constellation(
        i_channel='data1',  # I通道（同相分量）
        q_channel='data2',  # Q通道（正交分量）
        slice_angle=slice_angle,  # 使用切片数据
        max_points=5000,
        save_plot=True, 
        show_plot=True,
        title="QAM Constellation Diagram"
    )
    
    # 测试QAM调制解调功能
    print("\n=== 测试QAM调制解调功能 ===")
    test_qam_modulation_demodulation()
    
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

def test_qam_modulation_demodulation():
    """
    测试QAM调制解调功能 - 解调数据切片
    """
    print("测试QAM调制解调功能...")
    
    csv_file = "/home/xwj/0-code/0-github/motor/python/data_motor/qam16_1.csv"  # slice_angle= 8.36
    # 创建数据处理器实例
    processor = DataProcessor(csv_file)
    processor.load_data()
    
    print(f"加载数据文件: {csv_file}")
    print(f"数据形状: {processor.data.shape}")
    print(f"列名: {list(processor.data.columns)}")
    
    # 获取第一个数据切片
    slice_angle = 8.36  # 度
    data_length = len(processor.data)
    slice_points = int(slice_angle * data_length / 360)
    
    print(f"\n=== 数据切片分析 ===")
    print(f"切片角度: {slice_angle} 度")
    print(f"每切片点数: {slice_points}")
    print(f"总切片数: {data_length // slice_points}")
    
    # 获取第一个切片
    first_slice = processor.data['data1'].iloc[:slice_points].values
    print(f"第一个切片数据点数: {len(first_slice)}")
    
    # 对第一个切片进行QAM解调
    print("\n=== 第一个切片QAM解调 ===")
    demodulate_slice_and_plot_constellation(first_slice, processor, slice_angle)
    
    # 尝试不同的载波频率
    print("\n=== 尝试不同载波频率 ===")
    test_different_carrier_frequencies_for_slice(first_slice, processor, slice_angle)

def demodulate_slice_and_plot_constellation(slice_signal, processor, slice_angle, carrier_freq=100, sampling_rate=500):
    """
    解调数据切片并绘制单点星座图
    """
    print(f"切片解调参数: 载波频率={carrier_freq}Hz, 采样率={sampling_rate}Hz")
    
    # 生成时间轴
    t = np.arange(len(slice_signal)) / sampling_rate
    
    # 生成载波信号
    carrier_i = np.cos(2 * np.pi * carrier_freq * t)
    carrier_q = -np.sin(2 * np.pi * carrier_freq * t)
    
    # 解调得到I/Q分量
    i_signal = slice_signal * carrier_i
    q_signal = slice_signal * carrier_q
    
    # 应用低通滤波器
    from scipy import signal
    
    # 设计低通滤波器
    nyquist = sampling_rate / 2
    cutoff = carrier_freq / 4  # 截止频率设为载波频率的1/4
    b, a = signal.butter(4, cutoff / nyquist, btype='low')
    
    # 滤波
    i_filtered = signal.filtfilt(b, a, i_signal)
    q_filtered = signal.filtfilt(b, a, q_signal)
    
    # 下采样（可选）
    downsample_factor = 2
    i_downsampled = i_filtered[::downsample_factor]
    q_downsampled = q_filtered[::downsample_factor]
    
    print(f"解调后I/Q信号长度: {len(i_downsampled)}")
    
    # 绘制切片解调结果
    fig, axes = plt.subplots(2, 3, figsize=(18, 12))
    fig.suptitle(f'Slice QAM Demodulation (fc={carrier_freq}Hz, {slice_angle}°)', fontsize=16, fontweight='bold')
    
    # 1. 原始切片信号
    ax1 = axes[0, 0]
    ax1.plot(t, slice_signal, alpha=0.7, linewidth=1)
    ax1.set_xlabel('Time (s)')
    ax1.set_ylabel('Amplitude')
    ax1.set_title(f'Original Slice Signal ({slice_angle}°)')
    ax1.grid(True, alpha=0.3)
    
    # 2. I-Q星座图（单点星座图）
    ax2 = axes[0, 1]
    scatter2 = ax2.scatter(i_downsampled, q_downsampled, alpha=0.6, s=2, c=range(len(i_downsampled)), cmap='viridis')
    ax2.set_xlabel('I (In-phase)')
    ax2.set_ylabel('Q (Quadrature)')
    ax2.set_title('I-Q Constellation (Single Point)')
    ax2.grid(True, alpha=0.3)
    ax2.set_aspect('equal', adjustable='box')
    plt.colorbar(scatter2, ax=ax2, label='Sample Index')
    
    # 3. 解调后的I/Q信号
    ax3 = axes[0, 2]
    time_downsampled = t[::downsample_factor]
    ax3.plot(time_downsampled, i_downsampled, label='I Signal', alpha=0.7, linewidth=1)
    ax3.plot(time_downsampled, q_downsampled, label='Q Signal', alpha=0.7, linewidth=1)
    ax3.set_xlabel('Time (s)')
    ax3.set_ylabel('Amplitude')
    ax3.set_title('Demodulated I/Q Signals')
    ax3.legend()
    ax3.grid(True, alpha=0.3)
    
    # 4. 频谱分析
    ax4 = axes[1, 0]
    fft_original = np.fft.fft(slice_signal)
    freqs = np.fft.fftfreq(len(slice_signal), 1/sampling_rate)
    
    ax4.plot(freqs[:len(freqs)//2], np.abs(fft_original[:len(freqs)//2]), alpha=0.7)
    ax4.axvline(x=carrier_freq, color='red', linestyle='--', label=f'Carrier: {carrier_freq}Hz')
    ax4.set_xlabel('Frequency (Hz)')
    ax4.set_ylabel('Magnitude')
    ax4.set_title('Slice Signal Spectrum')
    ax4.legend()
    ax4.grid(True, alpha=0.3)
    
    # 5. 解调后I信号频谱
    ax5 = axes[1, 1]
    fft_i = np.fft.fft(i_downsampled)
    freqs_i = np.fft.fftfreq(len(i_downsampled), downsample_factor/sampling_rate)
    
    ax5.plot(freqs_i[:len(freqs_i)//2], np.abs(fft_i[:len(freqs_i)//2]), alpha=0.7)
    ax5.set_xlabel('Frequency (Hz)')
    ax5.set_ylabel('Magnitude')
    ax5.set_title('I Signal Spectrum')
    ax5.grid(True, alpha=0.3)
    
    # 6. 解调后Q信号频谱
    ax6 = axes[1, 2]
    fft_q = np.fft.fft(q_downsampled)
    
    ax6.plot(freqs_i[:len(freqs_i)//2], np.abs(fft_q[:len(freqs_i)//2]), alpha=0.7)
    ax6.set_xlabel('Frequency (Hz)')
    ax6.set_ylabel('Magnitude')
    ax6.set_title('Q Signal Spectrum')
    ax6.grid(True, alpha=0.3)
    
    plt.tight_layout()
    
    # 保存图像
    output_file = os.path.join(processor.images_folder, f'slice_qam_demodulation_{slice_angle}deg_{carrier_freq}hz.png')
    plt.savefig(output_file, dpi=300, bbox_inches='tight')
    print(f"切片QAM解调分析图像已保存为: {output_file}")
    
    plt.show()
    
    # 分析解调后的星座图
    analyze_slice_constellation(i_downsampled, q_downsampled, processor, slice_angle)

def test_different_carrier_frequencies_for_slice(slice_signal, processor, slice_angle):
    """
    测试切片的不同载波频率
    """
    carrier_frequencies = [25, 50, 100, 200]
    sampling_rate = 500
    
    for fc in carrier_frequencies:
        print(f"\n--- 测试载波频率: {fc}Hz ---")
        try:
            demodulate_slice_and_plot_constellation(slice_signal, processor, slice_angle, fc, sampling_rate)
        except Exception as e:
            print(f"载波频率 {fc}Hz 解调失败: {e}")

def analyze_slice_constellation(i_signal, q_signal, processor, slice_angle):
    """
    分析切片解调后的星座图
    """
    complex_signal = i_signal + 1j * q_signal
    amplitude = np.abs(complex_signal)
    phase = np.angle(complex_signal)
    
    print(f"\n=== 切片解调后星座图分析 ===")
    print(f"切片角度: {slice_angle}°")
    print(f"I信号范围: [{np.min(i_signal):.4f}, {np.max(i_signal):.4f}]")
    print(f"Q信号范围: [{np.min(q_signal):.4f}, {np.max(q_signal):.4f}]")
    print(f"幅度范围: [{np.min(amplitude):.4f}, {np.max(amplitude):.4f}]")
    print(f"相位范围: [{np.min(phase):.4f}, {np.max(phase):.4f}]")
    
    # 绘制单点星座图分析
    fig, axes = plt.subplots(2, 2, figsize=(12, 10))
    fig.suptitle(f'Slice Constellation Analysis ({slice_angle}°)', fontsize=16, fontweight='bold')
    
    # 1. I-Q星座图（单点）
    ax1 = axes[0, 0]
    scatter1 = ax1.scatter(i_signal, q_signal, c=amplitude, cmap='viridis', alpha=0.6, s=2)
    ax1.set_xlabel('I (In-phase)')
    ax1.set_ylabel('Q (Quadrature)')
    ax1.set_title('I-Q Constellation (Single Point)')
    ax1.grid(True, alpha=0.3)
    ax1.set_aspect('equal', adjustable='box')
    plt.colorbar(scatter1, ax=ax1, label='Amplitude')
    
    # 2. 极坐标星座图
    ax2 = axes[0, 1]
    scatter2 = ax2.scatter(phase, amplitude, c=range(len(phase)), cmap='plasma', alpha=0.6, s=2)
    ax2.set_xlabel('Phase (radians)')
    ax2.set_ylabel('Amplitude')
    ax2.set_title('Polar Constellation')
    ax2.grid(True, alpha=0.3)
    plt.colorbar(scatter2, ax=ax2, label='Sample Index')
    
    # 3. 幅度分布
    ax3 = axes[1, 0]
    ax3.hist(amplitude, bins=30, alpha=0.7, color='blue', edgecolor='black')
    ax3.set_xlabel('Amplitude')
    ax3.set_ylabel('Frequency')
    ax3.set_title('Amplitude Distribution')
    ax3.grid(True, alpha=0.3)
    
    # 4. 相位分布
    ax4 = axes[1, 1]
    ax4.hist(phase, bins=30, alpha=0.7, color='red', edgecolor='black')
    ax4.set_xlabel('Phase (radians)')
    ax4.set_ylabel('Frequency')
    ax4.set_title('Phase Distribution')
    ax4.grid(True, alpha=0.3)
    
    plt.tight_layout()
    
    # 保存图像
    output_file = os.path.join(processor.images_folder, f'slice_constellation_analysis_{slice_angle}deg.png')
    plt.savefig(output_file, dpi=300, bbox_inches='tight')
    print(f"切片星座图分析图像已保存为: {output_file}")
    
    plt.show()

def demodulate_and_plot_constellation(modulated_signal, processor, carrier_freq=100, sampling_rate=1000):
    """
    解调信号并绘制星座图
    """
    print(f"载波解调参数: 载波频率={carrier_freq}Hz, 采样率={sampling_rate}Hz")
    
    # 生成时间轴
    t = np.arange(len(modulated_signal)) / sampling_rate
    
    # 生成载波信号
    carrier_i = np.cos(2 * np.pi * carrier_freq * t)
    carrier_q = -np.sin(2 * np.pi * carrier_freq * t)
    
    # 解调得到I/Q分量
    i_signal = modulated_signal * carrier_i
    q_signal = modulated_signal * carrier_q
    
    # 应用低通滤波器
    from scipy import signal
    
    # 设计低通滤波器
    nyquist = sampling_rate / 2
    cutoff = carrier_freq / 4  # 截止频率设为载波频率的1/4
    b, a = signal.butter(4, cutoff / nyquist, btype='low')
    
    # 滤波
    i_filtered = signal.filtfilt(b, a, i_signal)
    q_filtered = signal.filtfilt(b, a, q_signal)
    
    # 下采样（可选）
    downsample_factor = 4
    i_downsampled = i_filtered[::downsample_factor]
    q_downsampled = q_filtered[::downsample_factor]
    
    print(f"解调后I/Q信号长度: {len(i_downsampled)}")
    
    # 绘制解调结果
    fig, axes = plt.subplots(2, 3, figsize=(18, 12))
    fig.suptitle(f'QAM Demodulation Analysis (fc={carrier_freq}Hz)', fontsize=16, fontweight='bold')
    
    # 1. 原始调制信号
    ax1 = axes[0, 0]
    ax1.plot(t[:1000], modulated_signal[:1000], alpha=0.7)
    ax1.set_xlabel('Time (s)')
    ax1.set_ylabel('Amplitude')
    ax1.set_title('Original Modulated Signal')
    ax1.grid(True, alpha=0.3)
    
    # 2. I-Q星座图
    ax2 = axes[0, 1]
    scatter2 = ax2.scatter(i_downsampled, q_downsampled, alpha=0.6, s=1)
    ax2.set_xlabel('I (In-phase)')
    ax2.set_ylabel('Q (Quadrature)')
    ax2.set_title('I-Q Constellation')
    ax2.grid(True, alpha=0.3)
    ax2.set_aspect('equal', adjustable='box')
    
    # 3. 解调后的I/Q信号
    ax3 = axes[0, 2]
    time_downsampled = t[::downsample_factor]
    ax3.plot(time_downsampled[:1000], i_downsampled[:1000], label='I Signal', alpha=0.7)
    ax3.plot(time_downsampled[:1000], q_downsampled[:1000], label='Q Signal', alpha=0.7)
    ax3.set_xlabel('Time (s)')
    ax3.set_ylabel('Amplitude')
    ax3.set_title('Demodulated I/Q Signals')
    ax3.legend()
    ax3.grid(True, alpha=0.3)
    
    # 4. 频谱分析
    ax4 = axes[1, 0]
    fft_original = np.fft.fft(modulated_signal)
    freqs = np.fft.fftfreq(len(modulated_signal), 1/sampling_rate)
    
    ax4.plot(freqs[:len(freqs)//2], np.abs(fft_original[:len(freqs)//2]), alpha=0.7)
    ax4.axvline(x=carrier_freq, color='red', linestyle='--', label=f'Carrier: {carrier_freq}Hz')
    ax4.set_xlabel('Frequency (Hz)')
    ax4.set_ylabel('Magnitude')
    ax4.set_title('Original Signal Spectrum')
    ax4.legend()
    ax4.grid(True, alpha=0.3)
    
    # 5. 解调后I信号频谱
    ax5 = axes[1, 1]
    fft_i = np.fft.fft(i_downsampled)
    freqs_i = np.fft.fftfreq(len(i_downsampled), downsample_factor/sampling_rate)
    
    ax5.plot(freqs_i[:len(freqs_i)//2], np.abs(fft_i[:len(freqs_i)//2]), alpha=0.7)
    ax5.set_xlabel('Frequency (Hz)')
    ax5.set_ylabel('Magnitude')
    ax5.set_title('I Signal Spectrum')
    ax5.grid(True, alpha=0.3)
    
    # 6. 解调后Q信号频谱
    ax6 = axes[1, 2]
    fft_q = np.fft.fft(q_downsampled)
    
    ax6.plot(freqs_i[:len(freqs_i)//2], np.abs(fft_q[:len(freqs_i)//2]), alpha=0.7)
    ax6.set_xlabel('Frequency (Hz)')
    ax6.set_ylabel('Magnitude')
    ax6.set_title('Q Signal Spectrum')
    ax6.grid(True, alpha=0.3)
    
    plt.tight_layout()
    
    # 保存图像
    output_file = os.path.join(processor.images_folder, f'qam_demodulation_{carrier_freq}hz.png')
    plt.savefig(output_file, dpi=300, bbox_inches='tight')
    print(f"QAM解调分析图像已保存为: {output_file}")
    
    plt.show()
    
    # 分析解调后的星座图
    analyze_demodulated_constellation(i_downsampled, q_downsampled, processor)

def test_different_carrier_frequencies(modulated_signal, processor):
    """
    测试不同的载波频率
    """
    carrier_frequencies = [50, 100, 200, 500]
    sampling_rate = 500
    
    for fc in carrier_frequencies:
        print(f"\n--- 测试载波频率: {fc}Hz ---")
        try:
            demodulate_and_plot_constellation(modulated_signal, processor, fc, sampling_rate)
        except Exception as e:
            print(f"载波频率 {fc}Hz 解调失败: {e}")

def analyze_demodulated_constellation(i_signal, q_signal, processor):
    """
    分析解调后的星座图
    """
    complex_signal = i_signal + 1j * q_signal
    amplitude = np.abs(complex_signal)
    phase = np.angle(complex_signal)
    
    print(f"\n=== 解调后星座图分析 ===")
    print(f"I信号范围: [{np.min(i_signal):.4f}, {np.max(i_signal):.4f}]")
    print(f"Q信号范围: [{np.min(q_signal):.4f}, {np.max(q_signal):.4f}]")
    print(f"幅度范围: [{np.min(amplitude):.4f}, {np.max(amplitude):.4f}]")
    print(f"相位范围: [{np.min(phase):.4f}, {np.max(phase):.4f}]")
    
    # 绘制星座图分析
    fig, axes = plt.subplots(2, 2, figsize=(12, 10))
    fig.suptitle('Demodulated Constellation Analysis', fontsize=16, fontweight='bold')
    
    # 1. I-Q星座图
    ax1 = axes[0, 0]
    scatter1 = ax1.scatter(i_signal, q_signal, c=amplitude, cmap='viridis', alpha=0.6, s=1)
    ax1.set_xlabel('I (In-phase)')
    ax1.set_ylabel('Q (Quadrature)')
    ax1.set_title('I-Q Constellation')
    ax1.grid(True, alpha=0.3)
    ax1.set_aspect('equal', adjustable='box')
    plt.colorbar(scatter1, ax=ax1, label='Amplitude')
    
    # 2. 极坐标星座图
    ax2 = axes[0, 1]
    scatter2 = ax2.scatter(phase, amplitude, c=range(len(phase)), cmap='plasma', alpha=0.6, s=1)
    ax2.set_xlabel('Phase (radians)')
    ax2.set_ylabel('Amplitude')
    ax2.set_title('Polar Constellation')
    ax2.grid(True, alpha=0.3)
    plt.colorbar(scatter2, ax=ax2, label='Data Point Index')
    
    # 3. 幅度分布
    ax3 = axes[1, 0]
    ax3.hist(amplitude, bins=50, alpha=0.7, color='blue', edgecolor='black')
    ax3.set_xlabel('Amplitude')
    ax3.set_ylabel('Frequency')
    ax3.set_title('Amplitude Distribution')
    ax3.grid(True, alpha=0.3)
    
    # 4. 相位分布
    ax4 = axes[1, 1]
    ax4.hist(phase, bins=50, alpha=0.7, color='red', edgecolor='black')
    ax4.set_xlabel('Phase (radians)')
    ax4.set_ylabel('Frequency')
    ax4.set_title('Phase Distribution')
    ax4.grid(True, alpha=0.3)
    
    plt.tight_layout()
    
    # 保存图像
    output_file = os.path.join(processor.images_folder, 'demodulated_constellation_analysis.png')
    plt.savefig(output_file, dpi=300, bbox_inches='tight')
    print(f"解调星座图分析图像已保存为: {output_file}")
    
    plt.show()

def analyze_qam_order(i_data, q_data, processor=None):
    """
    分析可能的QAM调制阶数
    """
    complex_signal = i_data + 1j * q_data
    
    # 计算幅度和相位
    amplitude = np.abs(complex_signal)
    phase = np.angle(complex_signal)
    
    # 分析幅度分布
    amp_std = np.std(amplitude)
    amp_mean = np.mean(amplitude)
    
    print(f"幅度统计: 均值={amp_mean:.4f}, 标准差={amp_std:.4f}")
    
    # 分析相位分布
    phase_std = np.std(phase)
    phase_mean = np.mean(phase)
    
    print(f"相位统计: 均值={phase_mean:.4f}, 标准差={phase_std:.4f}")
    
    # 尝试识别星座点
    # 将数据聚类到可能的星座点
    try:
        from sklearn.cluster import KMeans
        sklearn_available = True
    except ImportError:
        print("sklearn未安装，跳过聚类分析")
        sklearn_available = False
    
    if sklearn_available:
        # 尝试不同的聚类数量
        for n_clusters in [4, 8, 16, 32, 64]:
            try:
                kmeans = KMeans(n_clusters=n_clusters, random_state=42, n_init=10)
                cluster_labels = kmeans.fit_predict(complex_signal.reshape(-1, 1))
                cluster_centers = kmeans.cluster_centers_.flatten()
                
                # 计算聚类质量
                inertia = kmeans.inertia_
                
                print(f"{n_clusters}个聚类中心: {len(cluster_centers)}")
                print(f"  聚类中心: {cluster_centers[:10]}...")  # 只显示前10个
                print(f"  聚类质量 (inertia): {inertia:.4f}")
                
                # 如果聚类数量合理，绘制聚类结果
                if n_clusters in [4, 16, 64]:
                    plt.figure(figsize=(8, 6))
                    scatter = plt.scatter(complex_signal.real, complex_signal.imag, 
                                        c=cluster_labels, cmap='tab20', alpha=0.6, s=1)
                    plt.scatter(cluster_centers.real, cluster_centers.imag, 
                               c='red', marker='x', s=100, linewidths=3, label='Cluster Centers')
                    plt.xlabel('I (Real)')
                    plt.ylabel('Q (Imaginary)')
                    plt.title(f'QAM Constellation Analysis - {n_clusters} Clusters')
                    plt.legend()
                    plt.grid(True, alpha=0.3)
                    plt.axis('equal')
                    
                    # 保存图像
                    output_file = os.path.join(processor.images_folder, f'qam_cluster_analysis_{n_clusters}.png')
                    plt.savefig(output_file, dpi=300, bbox_inches='tight')
                    print(f"聚类分析图像已保存为: {output_file}")
                    
                    plt.show()
                    
            except Exception as e:
                print(f"聚类分析失败 (n_clusters={n_clusters}): {e}")
                continue
    else:
        # 简单的星座点分析（不使用sklearn）
        print("\n=== 简单星座点分析 ===")
        
        # 分析数据范围
        i_min, i_max = np.min(i_data), np.max(i_data)
        q_min, q_max = np.min(q_data), np.max(q_data)
        
        print(f"I通道范围: [{i_min:.4f}, {i_max:.4f}]")
        print(f"Q通道范围: [{q_min:.4f}, {q_max:.4f}]")
        
        # 分析数据分布
        i_std, q_std = np.std(i_data), np.std(q_data)
        print(f"I通道标准差: {i_std:.4f}")
        print(f"Q通道标准差: {q_std:.4f}")
        
        # 绘制数据分布
        plt.figure(figsize=(12, 8))
        
        # I-Q散点图
        plt.subplot(2, 2, 1)
        plt.scatter(i_data, q_data, alpha=0.6, s=1)
        plt.xlabel('I (data1)')
        plt.ylabel('Q (data2)')
        plt.title('I-Q Scatter Plot')
        plt.grid(True, alpha=0.3)
        plt.axis('equal')
        
        # I通道直方图
        plt.subplot(2, 2, 2)
        plt.hist(i_data, bins=50, alpha=0.7, color='blue', edgecolor='black')
        plt.xlabel('I Value')
        plt.ylabel('Frequency')
        plt.title('I Channel Distribution')
        plt.grid(True, alpha=0.3)
        
        # Q通道直方图
        plt.subplot(2, 2, 3)
        plt.hist(q_data, bins=50, alpha=0.7, color='red', edgecolor='black')
        plt.xlabel('Q Value')
        plt.ylabel('Frequency')
        plt.title('Q Channel Distribution')
        plt.grid(True, alpha=0.3)
        
        # 幅度分布
        plt.subplot(2, 2, 4)
        plt.hist(amplitude, bins=50, alpha=0.7, color='green', edgecolor='black')
        plt.xlabel('Amplitude')
        plt.ylabel('Frequency')
        plt.title('Amplitude Distribution')
        plt.grid(True, alpha=0.3)
        
        plt.tight_layout()
        
        # 保存图像
        if processor:
            output_file = os.path.join(processor.images_folder, 'simple_qam_analysis.png')
        else:
            output_file = 'simple_qam_analysis.png'
        plt.savefig(output_file, dpi=300, bbox_inches='tight')
        print(f"简单QAM分析图像已保存为: {output_file}")
        
        plt.show()

def analyze_fft_sample_freq(csv_file_path, sampling_rate=1000):
    """
    分析采样频率测量文件的FFT
    """
    print("FFT采样频率分析...")
    
    # 创建数据处理器实例
    processor = DataProcessor(csv_file_path)
    processor.load_data()
    
    print(f"加载数据文件: {csv_file_path}")
    print(f"数据形状: {processor.data.shape}")
    print(f"列名: {list(processor.data.columns)}")
    
    # 获取数据
    data1 = processor.data['data1'].values
    data2 = processor.data['data2'].values
    data3 = processor.data['data3'].values
    
    print(f"数据点数: {len(data1)}")
    print(f"采样率: {sampling_rate} Hz")
    
    # 计算时间轴
    time_axis = np.arange(len(data1)) / sampling_rate
    
    # 对每个通道进行FFT分析
    channels = ['data1', 'data2', 'data3']
    channel_data = [data1, data2, data3]
    colors = ['blue', 'red', 'green']
    
    # 创建图形
    fig, axes = plt.subplots(2, 3, figsize=(18, 12))
    fig.suptitle('FFT Analysis - Sample Frequency Measurement', fontsize=16, fontweight='bold')
    
    # 绘制原始波形
    ax1 = axes[0, 0]
    for i, (data, color, channel) in enumerate(zip(channel_data, colors, channels)):
        ax1.plot(time_axis, data, color=color, alpha=0.7, linewidth=0.5, label=channel)
    ax1.set_xlabel('Time (s)')
    ax1.set_ylabel('Amplitude')
    ax1.set_title('Original Waveforms')
    ax1.legend()
    ax1.grid(True, alpha=0.3)
    
    # 绘制单边幅度谱
    ax2 = axes[0, 1]
    for i, (data, color, channel) in enumerate(zip(channel_data, colors, channels)):
        # 计算FFT
        fft_data = np.fft.fft(data)
        freqs = np.fft.fftfreq(len(data), 1/sampling_rate)
        
        # 只取正频率部分
        positive_freqs = freqs[:len(freqs)//2]
        positive_fft = fft_data[:len(fft_data)//2]
        
        # 计算幅度谱
        magnitude = np.abs(positive_fft) * 2 / len(data)
        
        ax2.plot(positive_freqs, magnitude, color=color, alpha=0.7, linewidth=1, label=channel)
    
    ax2.set_xlabel('Frequency (Hz)')
    ax2.set_ylabel('Magnitude')
    ax2.set_title('Single-Sided Amplitude Spectrum')
    ax2.legend()
    ax2.grid(True, alpha=0.3)
    ax2.set_xlim(0, sampling_rate/2)
    
    # 绘制对数幅度谱
    ax3 = axes[0, 2]
    for i, (data, color, channel) in enumerate(zip(channel_data, colors, channels)):
        # 计算FFT
        fft_data = np.fft.fft(data)
        freqs = np.fft.fftfreq(len(data), 1/sampling_rate)
        
        # 只取正频率部分
        positive_freqs = freqs[:len(freqs)//2]
        positive_fft = fft_data[:len(fft_data)//2]
        
        # 计算对数幅度谱
        magnitude_db = 20 * np.log10(np.abs(positive_fft) + 1e-10)  # 避免log(0)
        
        ax3.plot(positive_freqs, magnitude_db, color=color, alpha=0.7, linewidth=1, label=channel)
    
    ax3.set_xlabel('Frequency (Hz)')
    ax3.set_ylabel('Magnitude (dB)')
    ax3.set_title('Single-Sided Log Magnitude Spectrum')
    ax3.legend()
    ax3.grid(True, alpha=0.3)
    ax3.set_xlim(0, sampling_rate/2)
    
    # 绘制每个通道的详细分析
    for i, (data, color, channel) in enumerate(zip(channel_data, colors, channels)):
        ax = axes[1, i]
        
        # 计算FFT
        fft_data = np.fft.fft(data)
        freqs = np.fft.fftfreq(len(data), 1/sampling_rate)
        
        # 只取正频率部分
        positive_freqs = freqs[:len(freqs)//2]
        positive_fft = fft_data[:len(fft_data)//2]
        
        # 计算幅度谱
        magnitude = np.abs(positive_fft)
        
        # 绘制幅度谱
        ax.plot(positive_freqs, magnitude, color=color, alpha=0.7, linewidth=1)
        ax.set_xlabel('Frequency (Hz)')
        ax.set_ylabel('Magnitude')
        ax.set_title(f'{channel} - Amplitude Spectrum')
        ax.grid(True, alpha=0.3)
        ax.set_xlim(0, sampling_rate/2)
        
        # 找到主要频率成分
        max_freq_idx = np.argmax(magnitude[1:]) + 1  # 跳过DC分量
        max_freq = positive_freqs[max_freq_idx]
        max_magnitude = magnitude[max_freq_idx]
        
        # 标记主要频率
        ax.axvline(x=max_freq, color='red', linestyle='--', alpha=0.7)
        ax.text(max_freq, max_magnitude, f'{max_freq:.1f}Hz', 
                rotation=90, verticalalignment='bottom', fontsize=10)
        
        print(f"{channel} - 主要频率: {max_freq:.2f} Hz, 幅度: {max_magnitude:.2f}")
    
    plt.tight_layout()
    
    # 保存图像
    output_file = os.path.join(processor.images_folder, 'fft_sample_freq_analysis.png')
    plt.savefig(output_file, dpi=300, bbox_inches='tight')
    print(f"FFT分析图像已保存为: {output_file}")
    
    plt.show()
    
    # 详细频率分析
    print("\n=== 详细频率分析 ===")
    for i, (data, color, channel) in enumerate(zip(channel_data, colors, channels)):
        print(f"\n{channel} 频率分析:")
        
        # 计算FFT
        fft_data = np.fft.fft(data)
        freqs = np.fft.fftfreq(len(data), 1/sampling_rate)
        
        # 只取正频率部分
        positive_freqs = freqs[:len(freqs)//2]
        positive_fft = fft_data[:len(fft_data)//2]
        
        # 计算幅度谱
        magnitude = np.abs(positive_fft)
        
        # 找到前5个主要频率成分
        sorted_indices = np.argsort(magnitude[1:])[::-1] + 1  # 跳过DC分量
        top_5_indices = sorted_indices[:5]
        
        print(f"  前5个主要频率成分:")
        for j, idx in enumerate(top_5_indices):
            freq = positive_freqs[idx]
            mag = magnitude[idx]
            print(f"    {j+1}. {freq:.2f} Hz - 幅度: {mag:.2f}")
        
        # 计算频率分辨率
        freq_resolution = sampling_rate / len(data)
        print(f"  频率分辨率: {freq_resolution:.4f} Hz")
        
        # 计算信号统计信息
        signal_mean = np.mean(data)
        signal_std = np.std(data)
        signal_rms = np.sqrt(np.mean(data**2))
        print(f"  信号统计: 均值={signal_mean:.4f}, 标准差={signal_std:.4f}, RMS={signal_rms:.4f}")

if __name__ == "__main__":
    # test_amplitude_crossing_points()
    #2.
    # test_qam_modulation_demodulation()
    
    # 3. FFT采样频率分析
    csv_file = "/home/xwj/0-code/0-github/motor/python/data_motor/measure_sample_freq_fft_0.csv"
    analyze_fft_sample_freq(csv_file, sampling_rate=500)
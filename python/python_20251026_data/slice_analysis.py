#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
数据切片分析脚本
专门用于数据切片、峰峰值计算和FFT分析
"""

import pandas as pd
import numpy as np
import matplotlib.pyplot as plt
import os

class SliceAnalyzer:
    """数据切片分析器"""
    
    def __init__(self, csv_file_path, total_degrees=360):
        """
        初始化切片分析器
        
        Args:
            csv_file_path (str): CSV文件路径
            total_degrees (float): 总角度范围 (度)
        """
        self.csv_file_path = csv_file_path
        self.total_degrees = total_degrees
        self.data = None
        self.angle_data = None
        
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
        """加载数据"""
        try:
            print(f"正在读取数据文件: {self.csv_file_path}")
            self.data = pd.read_csv(self.csv_file_path)
            
            # 创建角度轴
            data_length = len(self.data)
            self.angle_data = np.linspace(0, self.total_degrees, data_length)
            
            print(f"数据加载成功!")
            print(f"数据形状: {self.data.shape}")
            print(f"角度范围: 0-{self.total_degrees} 度")
            print(f"角度分辨率: {self.total_degrees/data_length:.4f} 度/点")
            
            return True
            
        except Exception as e:
            print(f"数据加载失败: {e}")
            return False
    
    def analyze_slices(self, slice_angle=10.0):
        """
        分析数据切片
        
        Args:
            slice_angle (float): 切片角度 (度)
        """
        if self.data is None:
            print("请先加载数据!")
            return None
        
        # 计算切片参数
        data_length = len(self.data)
        slice_points = int(slice_angle * data_length / self.total_degrees)
        total_slices = data_length // slice_points
        
        print(f"\n=== 数据切片分析 ===")
        print(f"切片角度: {slice_angle} 度")
        print(f"每切片点数: {slice_points}")
        print(f"总切片数: {total_slices}")
        
        columns = list(self.data.columns)
        results = {}
        
        # 对每个数据列进行分析
        for col in columns:
            print(f"\n分析列: {col}")
            
            # 计算每个切片的峰峰值
            peak_to_peak_values = []
            fft_results = []
            
            for i in range(total_slices):
                start_idx = i * slice_points
                end_idx = min((i + 1) * slice_points, len(self.data))
                slice_data = self.data[col].iloc[start_idx:end_idx]
                
                if len(slice_data) > 0:
                    # 计算峰峰值
                    peak_to_peak = slice_data.max() - slice_data.min()
                    peak_to_peak_values.append(peak_to_peak)
                    
                    # 计算FFT
                    if len(slice_data) >= slice_points:
                        fft_data = np.fft.fft(slice_data)
                        fft_magnitude = np.abs(fft_data[:slice_points//2])
                        fft_results.append(fft_magnitude)
            
            # 计算统计信息
            if peak_to_peak_values:
                mean_pp = np.mean(peak_to_peak_values)
                std_pp = np.std(peak_to_peak_values)
                max_pp = np.max(peak_to_peak_values)
                min_pp = np.min(peak_to_peak_values)
                
                print(f"  峰峰值统计:")
                print(f"    平均: {mean_pp:.6f}")
                print(f"    标准差: {std_pp:.6f}")
                print(f"    最大: {max_pp:.6f}")
                print(f"    最小: {min_pp:.6f}")
            
            # 计算平均FFT
            if fft_results:
                avg_fft = np.mean(fft_results, axis=0)
                freqs = np.fft.fftfreq(slice_points, 1/self.sampling_rate)
                positive_freqs = freqs[:slice_points//2]
                
                # 找到主要频率成分
                max_freq_idx = np.argmax(avg_fft)
                dominant_freq = positive_freqs[max_freq_idx]
                dominant_magnitude = avg_fft[max_freq_idx]
                
                print(f"  FFT分析:")
                print(f"    主要频率: {dominant_freq:.2f} Hz")
                print(f"    主要幅度: {dominant_magnitude:.6f}")
            
            results[col] = {
                'peak_to_peak': peak_to_peak_values,
                'fft_results': fft_results,
                'slice_angles': np.arange(total_slices) * slice_angle
            }
        
        return results
    
    def plot_slice_analysis(self, results, slice_angle=10.0, save_plot=True, show_plot=True):
        """
        绘制切片分析结果
        
        Args:
            results (dict): 分析结果
            slice_angle (float): 切片角度
            save_plot (bool): 是否保存图像
            show_plot (bool): 是否显示图像
        """
        if results is None:
            print("没有分析结果可绘制!")
            return False
        
        # 设置字体
        plt.rcParams['font.family'] = 'DejaVu Sans'
        plt.rcParams['axes.unicode_minus'] = False
        
        columns = list(self.data.columns)
        colors = ['red', 'green', 'blue']
        
        # 创建图形
        fig, axes = plt.subplots(3, 1, figsize=(12, 10))
        fig.suptitle(f'Data Slice Analysis (Slice Angle: {slice_angle}°)', fontsize=16, fontweight='bold')
        
        # 对每个数据列绘制结果
        for col_idx, (col, color) in enumerate(zip(columns, colors)):
            if col not in results:
                continue
            
            # 绘制峰峰值随角度变化
            ax = axes[col_idx]
            slice_angles = results[col]['slice_angles']
            peak_to_peak = results[col]['peak_to_peak']
            
            ax.plot(slice_angles, peak_to_peak, color=color, linewidth=2, 
                    marker='o', markersize=4, label=f'{col} Peak-to-Peak')
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
        
        return True
    
    def export_results(self, results, output_file='slice_analysis_results.csv'):
        """
        导出分析结果到CSV文件
        
        Args:
            results (dict): 分析结果
            output_file (str): 输出文件名
        """
        if results is None:
            print("没有分析结果可导出!")
            return False
        
        try:
            # 创建结果DataFrame
            export_data = {}
            
            for col in self.data.columns:
                if col in results:
                    slice_times = results[col]['slice_times']
                    peak_to_peak = results[col]['peak_to_peak']
                    
                    export_data[f'{col}_time'] = slice_times
                    export_data[f'{col}_peak_to_peak'] = peak_to_peak
            
            # 保存到CSV
            df_export = pd.DataFrame(export_data)
            df_export.to_csv(output_file, index=False)
            print(f"分析结果已导出到: {output_file}")
            
            return True
            
        except Exception as e:
            print(f"导出失败: {e}")
            return False


def main():
    """主函数"""
    # CSV文件路径
    csv_file = "/home/xwj/0-code/0-github/motor/python/data_motor/Bxyz_t_4s_10degrees.csv"
    
    # 检查文件是否存在
    if not os.path.exists(csv_file):
        print(f"文件不存在: {csv_file}")
        return
    
    # 创建切片分析器
    analyzer = SliceAnalyzer(csv_file, total_degrees=360)
    
    # 加载数据
    if not analyzer.load_data():
        return
    
    # 分析不同切片角度
    slice_angles = [5.0, 10.0, 20.0]  # 不同的切片角度
    
    for angle in slice_angles:
        print(f"\n{'='*50}")
        print(f"分析切片角度: {angle} 度")
        print(f"{'='*50}")
        
        # 进行分析
        results = analyzer.analyze_slices(slice_angle=angle)
        
        if results:
            # 绘制结果
            analyzer.plot_slice_analysis(results, slice_angle=angle, 
                                       save_plot=True, show_plot=True)
            
            # 导出结果
            output_file = f'slice_analysis_results_{angle}deg.csv'
            analyzer.export_results(results, output_file)
    
    print("\n切片分析完成!")


if __name__ == "__main__":
    main()

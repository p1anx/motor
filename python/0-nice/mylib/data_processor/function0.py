
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
            self.data = results[columns[0]]
            return self.data
        else:
            self.data = results
            return self.data

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
    csv_file = "/home/xwj/0-code/0-github/motor/python/data_motor/Bxyz_t_4s_10degrees.csv"
    csv_file = "/home/xwj/0-code/0-github/motor/python/data_motor/Bxyz_0.csv"
    processor = DataProcessor(csv_file)
    processor.load_data()
    
    processor.plot_data_points(save_plot=True, show_plot=True)
    # 计算峰峰值数据
    peak_to_peak_result = processor.get_peak_to_peak_array(slice_angle=4.0)
    
    # 绘制幅度等分点
    processor.plot_amplitude_crossing_points(
        peak_to_peak_data=peak_to_peak_result,
        n_divisions=5,  # 四等分
        slice_angle=10.0,
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
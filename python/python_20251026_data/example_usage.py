#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
峰峰值计算函数使用示例
"""

import numpy as np
import pandas as pd
from data_processor import DataProcessor

def example_basic_usage():
    """基础使用示例"""
    print("=" * 60)
    print("示例: 基础使用")
    print("=" * 60)
    
    # 创建简单的正弦波数据
    t = np.linspace(0, 360, 1000)
    data = np.sin(np.radians(t))
    
    # 创建数据处理器
    processor = DataProcessor("dummy.csv")
    
    # 计算峰峰值数组
    result = processor.get_peak_to_peak_array(data=data, slice_angle=30.0)
    
    print(f"输入数据长度: {len(data)}")
    print(f"切片角度: 30度")
    print(f"返回结果类型: {type(result)}")
    print(f"峰峰值数组长度: {len(result)}")
    print(f"前5个峰峰值: {result[:5]}")
    print()


def example_comparison():
    """比较不同切片角度的效果"""
    print("=" * 60)
    print("示例: 比较不同切片角度")
    print("=" * 60)
    
    # 创建测试数据
    t = np.linspace(0, 360, 1000)
    data = np.sin(np.radians(t))
    
    processor = DataProcessor("dummy.csv")
    
    # 使用不同的切片角度
    angles = [10, 30, 60, 90]
    
    for angle in angles:
        result = processor.get_peak_to_peak_array(data=data, slice_angle=angle)
        print(f"切片角度 {angle}度: {len(result)}个切片, 平均峰峰值={np.mean(result):.4f}")
    print()


def example_multi_channel():
    """多通道数据处理示例"""
    print("=" * 60)
    print("示例: 多通道数据处理")
    print("=" * 60)
    
    # 创建多通道数据
    t = np.linspace(0, 360, 1000)
    data = pd.DataFrame({
        'channel_1': np.sin(np.radians(t)),
        'channel_2': np.cos(np.radians(t)),
        'channel_3': np.sin(2 * np.radians(t))
    })
    
    processor = DataProcessor("dummy.csv")
    result = processor.get_peak_to_peak_array(data=data, slice_angle=30.0)
    
    print("多通道数据分析结果:")
    for channel, values in result.items():
        print(f"  {channel}: {len(values)}个切片, "
              f"平均峰峰值={np.mean(values):.4f}, "
              f"标准差={np.std(values):.4f}")
    print()


def example_real_data():
    """真实数据示例"""
    print("=" * 60)
    print("示例: 处理真实数据")
    print("=" * 60)
    
    try:
        # 加载真实数据
        csv_file = "/home/xwj/0-code/0-github/motor/python/data_motor/Bxyz_t_4s_10degrees.csv"
        processor = DataProcessor(csv_file)
        processor.load_data()
        processor.create_angle_axis(total_degrees=360)
        
        # 计算峰峰值
        result = processor.get_peak_to_peak_array(slice_angle=10.0)
        
        print("真实数据分析结果:")
        for channel, values in result.items():
            print(f"\n  {channel}:")
            print(f"    切片数量: {len(values)}")
            print(f"    平均峰峰值: {np.mean(values):.6f}")
            print(f"    标准差: {np.std(values):.6f}")
            print(f"    最大值: {np.max(values):.6f}")
            print(f"    最小值: {np.min(values):.6f}")
        
    except Exception as e:
        print(f"无法加载真实数据: {e}")


def example_custom_processing():
    """自定义处理示例"""
    print("=" * 60)
    print("示例: 自定义数据处理")
    print("=" * 60)
    
    # 生成带噪声的信号
    np.random.seed(42)
    t = np.linspace(0, 360, 1000)
    signal = np.sin(np.radians(t)) + 0.1 * np.random.randn(1000)
    
    processor = DataProcessor("dummy.csv")
    
    # 计算峰峰值
    peak_to_peak = processor.get_peak_to_peak_array(data=signal, slice_angle=20.0)
    
    # 进行进一步分析
    print("信号分析:")
    print(f"  峰峰值数组: {peak_to_peak}")
    print(f"  平均峰峰值: {np.mean(peak_to_peak):.4f}")
    print(f"  峰峰值稳定性 (变异系数): {np.std(peak_to_peak) / np.mean(peak_to_peak):.4f}")
    
    # 检测异常值
    threshold = np.mean(peak_to_peak) + 2 * np.std(peak_to_peak)
    outliers = [i for i, v in enumerate(peak_to_peak) if v > threshold]
    if outliers:
        print(f"  检测到异常切片索引: {outliers}")
    else:
        print("  未检测到异常值")
    print()


def main():
    """运行所有示例"""
    print("\n")
    print("峰峰值计算函数 - 使用示例")
    print("=" * 60)
    print()
    
    # 运行示例
    example_basic_usage()
    example_comparison()
    example_multi_channel()
    example_custom_processing()
    example_real_data()
    
    print("=" * 60)
    print("所有示例运行完成!")
    print("=" * 60)


if __name__ == "__main__":
    main()

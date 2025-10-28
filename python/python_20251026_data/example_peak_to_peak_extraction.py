#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
峰峰值提取功能使用示例
演示如何使用extract_peak_to_peak_values函数
"""

import os
import sys
import numpy as np

# 添加当前目录到路径
sys.path.append(os.path.dirname(__file__))

from data_processor import DataProcessor

def main():
    """主函数 - 峰峰值提取示例"""
    print("=== Peak-to-Peak Extraction Example ===")
    
    # CSV文件路径
    csv_file = "/home/xwj/0-code/0-github/motor/python/data_motor/Bxyz_t_4s_10degrees.csv"
    
    # 检查文件是否存在
    if not os.path.exists(csv_file):
        print(f"❌ Data file not found: {csv_file}")
        return
    
    print(f"✅ Data file exists: {csv_file}")
    
    # 创建数据处理器
    processor = DataProcessor(csv_file)
    
    # 加载数据
    if not processor.load_data():
        print("❌ Failed to load data")
        return
    
    print("✅ Data loaded successfully")
    
    # 创建角度轴
    processor.create_angle_axis(total_degrees=360)
    
    # 示例1: 基本峰峰值提取
    print("\n=== Example 1: Basic Peak-to-Peak Extraction ===")
    slice_angle = 10.0
    
    results = processor.extract_peak_to_peak_values(
        slice_angle=slice_angle,
        save_plot=True,
        show_plot=False
    )
    
    if results:
        print(f"✅ Successfully extracted peak-to-peak values for {slice_angle}° slices")
        
        # 显示结果
        for col, result in results.items():
            peak_to_peak_values = result['peak_to_peak_values']
            slice_angles = result['slice_angles']
            
            print(f"\n{col} Results:")
            print(f"  Number of slices: {len(peak_to_peak_values)}")
            print(f"  Peak-to-peak range: [{min(peak_to_peak_values):.6f}, {max(peak_to_peak_values):.6f}]")
            print(f"  Mean: {np.mean(peak_to_peak_values):.6f}")
            print(f"  Std: {np.std(peak_to_peak_values):.6f}")
    
    # 示例2: 不同切片角度的比较
    print("\n=== Example 2: Different Slice Angles Comparison ===")
    slice_angles = [5.0, 10.0, 15.0, 20.0]
    
    print("Slice Angle | Slices | Mean PP | Std PP | Min PP | Max PP")
    print("-" * 60)
    
    for slice_angle in slice_angles:
        results = processor.extract_peak_to_peak_values(
            slice_angle=slice_angle,
            save_plot=False,  # 不保存图像，只计算
            show_plot=False
        )
        
        if results:
            # 计算所有通道的平均值
            all_means = []
            all_stds = []
            all_mins = []
            all_maxs = []
            total_slices = 0
            
            for col, result in results.items():
                peak_to_peak_values = result['peak_to_peak_values']
                if peak_to_peak_values:
                    all_means.append(np.mean(peak_to_peak_values))
                    all_stds.append(np.std(peak_to_peak_values))
                    all_mins.append(min(peak_to_peak_values))
                    all_maxs.append(max(peak_to_peak_values))
                    total_slices = len(peak_to_peak_values)
            
            if all_means:
                avg_mean = np.mean(all_means)
                avg_std = np.mean(all_stds)
                avg_min = np.mean(all_mins)
                avg_max = np.mean(all_maxs)
                
                print(f"{slice_angle:10.1f}° | {total_slices:6d} | {avg_mean:7.3f} | {avg_std:6.3f} | {avg_min:6.3f} | {avg_max:6.3f}")
    
    # 示例3: 详细分析特定切片角度
    print("\n=== Example 3: Detailed Analysis for 10° Slices ===")
    slice_angle = 10.0
    
    results = processor.extract_peak_to_peak_values(
        slice_angle=slice_angle,
        save_plot=False,
        show_plot=False
    )
    
    if results:
        for col, result in results.items():
            peak_to_peak_values = result['peak_to_peak_values']
            slice_angles = result['slice_angles']
            
            print(f"\n{col} Detailed Analysis:")
            print(f"  Total slices: {len(peak_to_peak_values)}")
            
            # 统计信息
            min_pp = min(peak_to_peak_values)
            max_pp = max(peak_to_peak_values)
            mean_pp = np.mean(peak_to_peak_values)
            std_pp = np.std(peak_to_peak_values)
            median_pp = np.median(peak_to_peak_values)
            
            print(f"  Statistics:")
            print(f"    Min: {min_pp:.6f}")
            print(f"    Max: {max_pp:.6f}")
            print(f"    Mean: {mean_pp:.6f}")
            print(f"    Median: {median_pp:.6f}")
            print(f"    Std: {std_pp:.6f}")
            print(f"    Range: {max_pp - min_pp:.6f}")
            
            # 找到最大值和最小值对应的角度
            min_idx = peak_to_peak_values.index(min_pp)
            max_idx = peak_to_peak_values.index(max_pp)
            
            print(f"  Extreme values:")
            print(f"    Min at angle: {slice_angles[min_idx]:.1f}°")
            print(f"    Max at angle: {slice_angles[max_idx]:.1f}°")
            
            # 显示前几个值
            print(f"  First 5 values:")
            for i in range(min(5, len(peak_to_peak_values))):
                print(f"    Angle {slice_angles[i]:.1f}°: {peak_to_peak_values[i]:.6f}")
    
    # 示例4: 访问峰峰值数据
    print("\n=== Example 4: Accessing Peak-to-Peak Data ===")
    slice_angle = 10.0
    
    results = processor.extract_peak_to_peak_values(
        slice_angle=slice_angle,
        save_plot=False,
        show_plot=False
    )
    
    if results:
        print("Accessing peak-to-peak data:")
        
        for col, result in results.items():
            peak_to_peak_values = result['peak_to_peak_values']
            slice_angles = result['slice_angles']
            
            print(f"\n{col}:")
            print(f"  peak_to_peak_values: {len(peak_to_peak_values)} values")
            print(f"  slice_angles: {len(slice_angles)} angles")
            print(f"  slice_angle: {result['slice_angle']}°")
            print(f"  num_slices: {result['num_slices']}")
            
            # 示例：找到峰峰值大于平均值的切片
            mean_pp = np.mean(peak_to_peak_values)
            high_pp_slices = [(angle, pp) for angle, pp in zip(slice_angles, peak_to_peak_values) if pp > mean_pp]
            
            print(f"  Slices with peak-to-peak > mean ({mean_pp:.3f}): {len(high_pp_slices)}")
            if high_pp_slices:
                print(f"    Examples: {high_pp_slices[:3]}")
    
    print("\n=== Example completed ===")

if __name__ == "__main__":
    main()

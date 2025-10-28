#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
测试峰峰值提取功能
验证数据切片峰峰值的提取和绘制
"""

import os
import sys
import numpy as np
import matplotlib.pyplot as plt

# 添加当前目录到路径
sys.path.append(os.path.dirname(__file__))

from data_processor import DataProcessor

def test_peak_to_peak_extraction():
    """测试峰峰值提取功能"""
    print("=== Test Peak-to-Peak Extraction Function ===")
    
    # 检查数据文件
    csv_file = "/home/xwj/0-code/0-github/motor/python/data_motor/Bxyz_t_4s_10degrees.csv"
    if not os.path.exists(csv_file):
        print(f"❌ Data file not found: {csv_file}")
        return False
    
    print(f"✅ Data file exists: {csv_file}")
    
    # 创建数据处理器
    processor = DataProcessor(csv_file)
    
    # 加载数据
    if not processor.load_data():
        print("❌ Failed to load data")
        return False
    
    print("✅ Data loaded successfully")
    
    # 创建角度轴
    processor.create_angle_axis(total_degrees=360)
    
    # 测试不同的切片角度
    slice_angles = [5.0, 10.0, 15.0, 20.0]
    
    for slice_angle in slice_angles:
        print(f"\n=== Testing slice angle: {slice_angle}° ===")
        
        # 提取峰峰值
        results = processor.extract_peak_to_peak_values(
            slice_angle=slice_angle, 
            save_plot=True, 
            show_plot=False
        )
        
        if results:
            print(f"✅ Successfully extracted peak-to-peak values for {slice_angle}° slices")
            
            # 验证结果
            for col, result in results.items():
                peak_to_peak_values = result['peak_to_peak_values']
                slice_angles_list = result['slice_angles']
                
                print(f"  {col}:")
                print(f"    Number of slices: {len(peak_to_peak_values)}")
                print(f"    Peak-to-peak range: [{min(peak_to_peak_values):.6f}, {max(peak_to_peak_values):.6f}]")
                print(f"    Mean: {np.mean(peak_to_peak_values):.6f}")
                print(f"    Std: {np.std(peak_to_peak_values):.6f}")
        else:
            print(f"❌ Failed to extract peak-to-peak values for {slice_angle}° slices")
    
    return True

def test_peak_to_peak_analysis():
    """测试峰峰值分析功能"""
    print("\n=== Test Peak-to-Peak Analysis ===")
    
    # 检查数据文件
    csv_file = "/home/xwj/0-code/0-github/motor/python/data_motor/Bxyz_t_4s_10degrees.csv"
    if not os.path.exists(csv_file):
        print(f"❌ Data file not found: {csv_file}")
        return False
    
    # 创建数据处理器
    processor = DataProcessor(csv_file)
    
    # 加载数据
    if not processor.load_data():
        print("❌ Failed to load data")
        return False
    
    # 创建角度轴
    processor.create_angle_axis(total_degrees=360)
    
    # 使用10度切片进行详细分析
    slice_angle = 10.0
    print(f"\n=== Detailed Analysis for {slice_angle}° Slices ===")
    
    results = processor.extract_peak_to_peak_values(
        slice_angle=slice_angle, 
        save_plot=True, 
        show_plot=False
    )
    
    if results:
        print("\n=== Peak-to-Peak Values Analysis ===")
        
        for col, result in results.items():
            peak_to_peak_values = result['peak_to_peak_values']
            slice_angles_list = result['slice_angles']
            
            print(f"\n{col} Analysis:")
            print(f"  Total slices: {len(peak_to_peak_values)}")
            print(f"  Slice angle: {result['slice_angle']}°")
            
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
            print(f"    Min at angle: {slice_angles_list[min_idx]:.1f}°")
            print(f"    Max at angle: {slice_angles_list[max_idx]:.1f}°")
            
            # 显示前几个和后几个值
            print(f"  First 5 values:")
            for i in range(min(5, len(peak_to_peak_values))):
                print(f"    Angle {slice_angles_list[i]:.1f}°: {peak_to_peak_values[i]:.6f}")
            
            if len(peak_to_peak_values) > 5:
                print(f"  Last 5 values:")
                for i in range(max(0, len(peak_to_peak_values) - 5), len(peak_to_peak_values)):
                    print(f"    Angle {slice_angles_list[i]:.1f}°: {peak_to_peak_values[i]:.6f}")
    
    return True

def test_peak_to_peak_comparison():
    """测试不同切片角度的峰峰值比较"""
    print("\n=== Test Peak-to-Peak Comparison ===")
    
    # 检查数据文件
    csv_file = "/home/xwj/0-code/0-github/motor/python/data_motor/Bxyz_t_4s_10degrees.csv"
    if not os.path.exists(csv_file):
        print(f"❌ Data file not found: {csv_file}")
        return False
    
    # 创建数据处理器
    processor = DataProcessor(csv_file)
    
    # 加载数据
    if not processor.load_data():
        print("❌ Failed to load data")
        return False
    
    # 创建角度轴
    processor.create_angle_axis(total_degrees=360)
    
    # 测试不同的切片角度
    slice_angles = [5.0, 10.0, 15.0, 20.0, 30.0]
    
    print("\n=== Peak-to-Peak Comparison ===")
    print("Slice Angle | Slices | Mean PP | Std PP | Min PP | Max PP")
    print("-" * 60)
    
    for slice_angle in slice_angles:
        results = processor.extract_peak_to_peak_values(
            slice_angle=slice_angle, 
            save_plot=False, 
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
    
    return True

if __name__ == "__main__":
    print("Starting peak-to-peak extraction tests...")
    
    # 测试基本功能
    if test_peak_to_peak_extraction():
        print("\n✅ Basic peak-to-peak extraction test passed")
    else:
        print("\n❌ Basic peak-to-peak extraction test failed")
    
    # 测试详细分析
    if test_peak_to_peak_analysis():
        print("\n✅ Peak-to-peak analysis test passed")
    else:
        print("\n❌ Peak-to-peak analysis test failed")
    
    # 测试比较功能
    if test_peak_to_peak_comparison():
        print("\n✅ Peak-to-peak comparison test passed")
    else:
        print("\n❌ Peak-to-peak comparison test failed")
    
    print("\n=== All tests completed ===")

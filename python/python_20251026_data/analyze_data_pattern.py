#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
分析数据模式，检查是否为sin/cos曲线
"""

import pandas as pd
import numpy as np
import matplotlib.pyplot as plt
import os

def analyze_data_pattern():
    """分析数据模式"""
    print("=== Analyze Data Pattern ===")
    
    # 读取数据
    csv_file = "/home/xwj/0-code/0-github/motor/python/data_motor/Bxyz_t_4s_10degrees.csv"
    if not os.path.exists(csv_file):
        print(f"❌ Data file not found: {csv_file}")
        return False
    
    data = pd.read_csv(csv_file)
    print(f"✅ Data loaded: {data.shape}")
    print(f"Columns: {list(data.columns)}")
    
    # 创建角度轴
    total_degrees = 360
    data_points = len(data)
    angle_resolution = total_degrees / data_points
    angles = np.linspace(0, total_degrees, data_points)
    
    print(f"\n=== Data Analysis ===")
    print(f"Data points: {data_points}")
    print(f"Angle range: 0-{total_degrees} degrees")
    print(f"Angle resolution: {angle_resolution:.4f} degrees/point")
    
    # 分析每个通道
    for col in data.columns:
        print(f"\n=== {col} Analysis ===")
        values = data[col].values
        
        # 基本统计
        print(f"Range: [{values.min():.6f}, {values.max():.6f}]")
        print(f"Mean: {values.mean():.6f}")
        print(f"Std: {values.std():.6f}")
        
        # 检查重复值
        unique_values = np.unique(values)
        print(f"Unique values: {len(unique_values)} out of {len(values)}")
        print(f"Duplication rate: {(len(values) - len(unique_values)) / len(values) * 100:.2f}%")
        
        # 检查是否有多个角度对应同一个值
        value_counts = {}
        for i, value in enumerate(values):
            # 使用更宽松的精度
            rounded_value = round(value, 6)
            if rounded_value not in value_counts:
                value_counts[rounded_value] = []
            value_counts[rounded_value].append(angles[i])
        
        # 找到有多个角度对应的值
        multi_angle_values = {k: v for k, v in value_counts.items() if len(v) > 1}
        print(f"Values with multiple angles: {len(multi_angle_values)}")
        
        if multi_angle_values:
            print("Examples of values with multiple angles:")
            count = 0
            for value, angle_list in multi_angle_values.items():
                if count < 5:  # 只显示前5个例子
                    print(f"  Value {value:.6f}: {len(angle_list)} angles")
                    print(f"    Angles: {[f'{a:.2f}°' for a in angle_list[:5]]}{'...' if len(angle_list) > 5 else ''}")
                    count += 1
                else:
                    break
        
        # 检查峰峰值
        slice_angle = 10.0
        points_per_slice = int(slice_angle / angle_resolution)
        num_slices = int(total_degrees / slice_angle)
        
        peak_to_peak_values = []
        slice_angles = []
        
        for i in range(num_slices):
            start_idx = i * points_per_slice
            end_idx = min(start_idx + points_per_slice, len(values))
            slice_data = values[start_idx:end_idx]
            
            if len(slice_data) > 0:
                peak_to_peak = np.max(slice_data) - np.min(slice_data)
                peak_to_peak_values.append(peak_to_peak)
                slice_angles.append(i * slice_angle)
        
        print(f"\nPeak-to-peak analysis:")
        print(f"Number of slices: {len(peak_to_peak_values)}")
        print(f"Peak-to-peak range: [{min(peak_to_peak_values):.6f}, {max(peak_to_peak_values):.6f}]")
        
        # 检查峰峰值的重复情况
        pp_unique = np.unique(np.round(peak_to_peak_values, 6))
        print(f"Unique peak-to-peak values: {len(pp_unique)} out of {len(peak_to_peak_values)}")
        
        # 检查等分边界点的匹配情况
        n_divisions = 4
        min_pp = min(peak_to_peak_values)
        max_pp = max(peak_to_peak_values)
        division_size = (max_pp - min_pp) / n_divisions
        
        print(f"\nDivision analysis (n_divisions={n_divisions}):")
        print(f"Division size: {division_size:.6f}")
        
        for i in range(n_divisions + 1):
            boundary_value = min_pp + i * division_size
            matches = []
            for pp_value, angle in zip(peak_to_peak_values, slice_angles):
                if abs(pp_value - boundary_value) < 1e-6:
                    matches.append(angle)
            
            print(f"  Boundary {i+1}: value {boundary_value:.6f} -> {len(matches)} matches")
            if matches:
                print(f"    Angles: {[f'{a:.2f}°' for a in matches]}")
    
    return True

if __name__ == "__main__":
    analyze_data_pattern()

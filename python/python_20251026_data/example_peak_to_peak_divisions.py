#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
峰峰值等分功能使用示例
演示如何使用get_peak_to_peak_divisions方法进行数据分析
"""

import os
import sys
from data_processor import DataProcessor

def example_peak_to_peak_divisions():
    """峰峰值等分功能使用示例"""
    print("=== Peak-to-Peak Divisions Usage Example ===")
    
    # CSV文件路径
    csv_file = "/home/xwj/0-code/0-github/motor/python/data_motor/Bxyz_t_4s_10degrees.csv"
    
    # 检查文件是否存在
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
    
    # 创建角度轴
    processor.create_angle_axis(total_degrees=360)
    
    print("\n=== Example 1: Default Parameters (10° slices, 10 divisions) ===")
    results1 = processor.get_peak_to_peak_divisions()
    
    if results1:
        print("\n--- Analysis Results ---")
        for col, col_data in results1.items():
            print(f"\n{col}:")
            print(f"  峰峰值范围: [{col_data['peak_to_peak_range'][0]:.6f}, {col_data['peak_to_peak_range'][1]:.6f}]")
            print(f"  等分大小: {col_data['division_size']:.6f}")
            
            # 显示每个等分的信息
            for div in col_data['divisions']:
                if div['count'] > 0:
                    print(f"  第{div['division']}等分 [{div['range'][0]:.6f}, {div['range'][1]:.6f}):")
                    print(f"    角度值: {div['angles']}")
                    print(f"    数量: {div['count']}")
    
    print("\n=== Example 2: Custom Parameters (5° slices, 20 divisions) ===")
    results2 = processor.get_peak_to_peak_divisions(slice_angle=5.0, n_divisions=20)
    
    if results2:
        print("\n--- Analysis Results ---")
        for col, col_data in results2.items():
            print(f"\n{col}:")
            print(f"  峰峰值范围: [{col_data['peak_to_peak_range'][0]:.6f}, {col_data['peak_to_peak_range'][1]:.6f}]")
            print(f"  等分大小: {col_data['division_size']:.6f}")
            
            # 只显示有数据的等分
            non_empty_divisions = [div for div in col_data['divisions'] if div['count'] > 0]
            print(f"  有数据的等分数: {len(non_empty_divisions)}")
            
            # 显示前5个有数据的等分
            for div in non_empty_divisions[:5]:
                print(f"  第{div['division']}等分 [{div['range'][0]:.6f}, {div['range'][1]:.6f}):")
                print(f"    角度值: {div['angles']}")
                print(f"    数量: {div['count']}")
    
    print("\n=== Example 3: Find Specific Amplitude Ranges ===")
    if results1:
        print("\n--- Finding High Amplitude Regions ---")
        for col, col_data in results1.items():
            print(f"\n{col} 高幅度区域 (后3个等分):")
            high_divisions = col_data['divisions'][-3:]  # 最后3个等分
            
            for div in high_divisions:
                if div['count'] > 0:
                    print(f"  第{div['division']}等分 [{div['range'][0]:.6f}, {div['range'][1]:.6f}):")
                    print(f"    角度值: {div['angles']}")
                    print(f"    数量: {div['count']}")
    
    print("\n=== Example 4: Statistical Analysis ===")
    if results1:
        print("\n--- Statistical Analysis ---")
        for col, col_data in results1.items():
            print(f"\n{col} 统计信息:")
            
            # 计算每个等分的数据分布
            total_slices = sum(div['count'] for div in col_data['divisions'])
            print(f"  总切片数: {total_slices}")
            
            # 找到数据最多的等分
            max_count_division = max(col_data['divisions'], key=lambda x: x['count'])
            print(f"  数据最多的等分: 第{max_count_division['division']}等分")
            print(f"    范围: [{max_count_division['range'][0]:.6f}, {max_count_division['range'][1]:.6f}]")
            print(f"    数量: {max_count_division['count']}")
            print(f"    角度值: {max_count_division['angles']}")
            
            # 计算数据分布
            non_empty_divisions = [div for div in col_data['divisions'] if div['count'] > 0]
            print(f"  有数据的等分数: {len(non_empty_divisions)}")
            print(f"  数据覆盖率: {len(non_empty_divisions)/len(col_data['divisions'])*100:.1f}%")
    
    print("\n=== Example 5: Export Results ===")
    if results1:
        print("\n--- Exporting Results to File ---")
        
        # 创建输出文件
        output_file = os.path.join(os.path.dirname(__file__), "peak_to_peak_divisions_results.txt")
        
        with open(output_file, 'w', encoding='utf-8') as f:
            f.write("Peak-to-Peak Divisions Analysis Results\n")
            f.write("=" * 50 + "\n\n")
            
            for col, col_data in results1.items():
                f.write(f"{col} Analysis:\n")
                f.write(f"  峰峰值范围: [{col_data['peak_to_peak_range'][0]:.6f}, {col_data['peak_to_peak_range'][1]:.6f}]\n")
                f.write(f"  等分大小: {col_data['division_size']:.6f}\n\n")
                
                for div in col_data['divisions']:
                    if div['count'] > 0:
                        f.write(f"  第{div['division']}等分 [{div['range'][0]:.6f}, {div['range'][1]:.6f}):\n")
                        f.write(f"    角度值: {div['angles']}\n")
                        f.write(f"    数量: {div['count']}\n\n")
                
                f.write("\n" + "-" * 30 + "\n\n")
        
        print(f"✅ Results exported to: {output_file}")
    
    print("\n=== Example 6: Practical Applications ===")
    print("1. 质量控制: 识别异常幅度范围")
    print("2. 优化采样: 找到关键角度区域")
    print("3. 模式识别: 分析幅度分布规律")
    print("4. 异常检测: 定位异常数据点")
    print("5. 性能评估: 评估系统稳定性")
    
    return True

if __name__ == "__main__":
    example_peak_to_peak_divisions()

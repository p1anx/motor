#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
测试峰峰值计算函数的示例脚本
"""

import numpy as np
import pandas as pd
from data_processor import DataProcessor

def test_with_class_data():
    """测试使用类内部数据"""
    print("=" * 60)
    print("测试1: 使用类内部数据")
    print("=" * 60)
    
    # 创建数据处理器
    csv_file = "/home/xwj/0-code/0-github/motor/python/data_motor/Bxyz_t_4s_10degrees.csv"
    processor = DataProcessor(csv_file)
    processor.load_data()
    processor.create_angle_axis(total_degrees=360)
    
    # 调用新的通用函数
    result = processor.get_peak_to_peak_array(slice_angle=10.0)
    
    print("\n返回结果类型:", type(result))
    print("结果:")
    if isinstance(result, dict):
        for key, value in result.items():
            print(f"  {key}: 数组长度={len(value)}, 前5个值={value[:5]}")
    
    return result


def test_with_external_dataframe():
    """测试使用外部DataFrame数据"""
    print("\n" + "=" * 60)
    print("测试2: 使用外部DataFrame数据")
    print("=" * 60)
    
    # 创建测试数据
    np.random.seed(42)
    test_data = pd.DataFrame({
        'ch1': np.sin(np.linspace(0, 4*np.pi, 1000)) + np.random.normal(0, 0.1, 1000),
        'ch2': np.cos(np.linspace(0, 4*np.pi, 1000)) + np.random.normal(0, 0.1, 1000),
        'ch3': np.sin(2*np.linspace(0, 4*np.pi, 1000)) + np.random.normal(0, 0.1, 1000)
    })
    
    # 创建数据处理器（不需要加载CSV）
    processor = DataProcessor("dummy.csv")
    
    # 调用通用函数
    result = processor.get_peak_to_peak_array(data=test_data, slice_angle=30.0)
    
    print("\n返回结果类型:", type(result))
    print("结果:")
    if isinstance(result, dict):
        for key, value in result.items():
            print(f"  {key}: 数组长度={len(value)}, 平均值={np.mean(value):.4f}")


def test_with_numpy_array():
    """测试使用numpy数组"""
    print("\n" + "=" * 60)
    print("测试3: 使用numpy数组（单列）")
    print("=" * 60)
    
    # 创建测试数据
    np.random.seed(42)
    test_data = np.sin(np.linspace(0, 4*np.pi, 1000)) + np.random.normal(0, 0.1, 1000)
    
    # 创建数据处理器
    processor = DataProcessor("dummy.csv")
    
    # 调用通用函数
    result = processor.get_peak_to_peak_array(data=test_data, slice_angle=30.0)
    
    print("\n返回结果类型:", type(result))
    print("结果:")
    if isinstance(result, list):
        print(f"  数组长度={len(result)}, 前10个值={result[:10]}")


def test_with_list():
    """测试使用list数据"""
    print("\n" + "=" * 60)
    print("测试4: 使用list数据")
    print("=" * 60)
    
    # 创建测试数据
    test_data = [np.sin(x) for x in np.linspace(0, 4*np.pi, 1000)]
    
    # 创建数据处理器
    processor = DataProcessor("dummy.csv")
    
    # 调用通用函数
    result = processor.get_peak_to_peak_array(data=test_data, slice_angle=30.0)
    
    print("\n返回结果类型:", type(result))
    print("结果:")
    if isinstance(result, list):
        print(f"  数组长度={len(result)}, 平均值={np.mean(result):.4f}")


def test_with_multi_column_array():
    """测试使用多列numpy数组"""
    print("\n" + "=" * 60)
    print("测试5: 使用多列numpy数组")
    print("=" * 60)
    
    # 创建测试数据
    np.random.seed(42)
    test_data = np.column_stack([
        np.sin(np.linspace(0, 4*np.pi, 1000)),
        np.cos(np.linspace(0, 4*np.pi, 1000)),
        np.sin(2*np.linspace(0, 4*np.pi, 1000))
    ])
    
    # 创建数据处理器
    processor = DataProcessor("dummy.csv")
    
    # 调用通用函数
    result = processor.get_peak_to_peak_array(data=test_data, slice_angle=30.0)
    
    print("\n返回结果类型:", type(result))
    print("结果:")
    if isinstance(result, dict):
        for key, value in result.items():
            print(f"  {key}: 数组长度={len(value)}, 平均值={np.mean(value):.4f}")


def main():
    """主测试函数"""
    print("\n")
    print("峰峰值计算函数测试")
    print("=" * 60)
    
    # 运行所有测试
    try:
        result1 = test_with_class_data()
        test_with_external_dataframe()
        test_with_numpy_array()
        test_with_list()
        test_with_multi_column_array()
        
        print("\n" + "=" * 60)
        print("所有测试完成!")
        print("=" * 60)
        
    except Exception as e:
        print(f"\n测试过程中出现错误: {e}")
        import traceback
        traceback.print_exc()


if __name__ == "__main__":
    main()

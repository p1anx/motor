#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
测试峰峰值绘图函数
"""

import numpy as np
import pandas as pd
from data_processor import DataProcessor

def test_plot_list_data():
    """测试绘制list类型的峰峰值数据"""
    print("=" * 60)
    print("测试1: 绘制list类型的峰峰值数据")
    print("=" * 60)
    
    # 创建测试数据
    processor = DataProcessor("dummy.csv")
    peak_to_peak = [0.5, 0.6, 0.4, 0.7, 0.5, 0.8, 0.6, 0.4, 0.7, 0.5, 0.6, 0.5]
    
    # 绘制
    processor.plot_peak_to_peak_array(
        peak_to_peak_data=peak_to_peak,
        slice_angle=30.0,
        save_plot=True,
        show_plot=False,
        title="单通道峰峰值曲线"
    )
    print("绘图完成！\n")


def test_plot_dict_data():
    """测试绘制dict类型的峰峰值数据"""
    print("=" * 60)
    print("测试2: 绘制dict类型的峰峰值数据")
    print("=" * 60)
    
    # 创建测试数据
    processor = DataProcessor("dummy.csv")
    peak_to_peak = {
        'channel_1': [0.5, 0.6, 0.4, 0.7, 0.5, 0.8, 0.6, 0.4, 0.7, 0.5, 0.6, 0.5],
        'channel_2': [0.3, 0.5, 0.4, 0.6, 0.5, 0.7, 0.5, 0.4, 0.6, 0.5, 0.5, 0.4],
        'channel_3': [0.8, 0.9, 0.7, 1.0, 0.8, 1.1, 0.9, 0.7, 1.0, 0.8, 0.9, 0.8]
    }
    
    # 绘制
    processor.plot_peak_to_peak_array(
        peak_to_peak_data=peak_to_peak,
        slice_angle=30.0,
        save_plot=True,
        show_plot=False,
        title="多通道峰峰值曲线"
    )
    print("绘图完成！\n")


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
            peak_to_peak_data=result,
            slice_angle=10.0,
            save_plot=True,
            show_plot=False,
            title="真实数据峰峰值分析"
        )
        print("绘图完成！\n")
        
    except Exception as e:
        print(f"出错: {e}\n")


def test_with_simulated_data():
    """测试使用模拟数据"""
    print("=" * 60)
    print("测试4: 使用模拟数据")
    print("=" * 60)
    
    # 创建模拟数据
    np.random.seed(42)
    processor = DataProcessor("dummy.csv")
    
    # 生成带有趋势的峰峰值数据
    angles = np.linspace(0, 360, 36)
    peak_to_peak = {
        'sin_channel': 0.5 + 0.3 * np.sin(np.radians(angles * 2)) + np.random.normal(0, 0.05, 36),
        'cos_channel': 0.4 + 0.2 * np.cos(np.radians(angles * 2)) + np.random.normal(0, 0.05, 36),
        'linear_channel': 0.3 + 0.3 * angles / 360 + np.random.normal(0, 0.05, 36)
    }
    
    # 绘制
    processor.plot_peak_to_peak_array(
        peak_to_peak_data=peak_to_peak,
        slice_angle=10.0,
        save_plot=True,
        show_plot=False,
        title="模拟数据峰峰值分析"
    )
    print("绘图完成！\n")


def main():
    """运行所有测试"""
    print("\n")
    print("峰峰值绘图函数测试")
    print("=" * 60)
    print()
    
    # 运行测试
    test_plot_list_data()
    test_plot_dict_data()
    test_with_simulated_data()
    test_with_real_calculation()
    
    print("=" * 60)
    print("所有测试完成!")
    print("=" * 60)


if __name__ == "__main__":
    main()

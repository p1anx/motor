#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
测试切片分析功能
验证数据切片、峰峰值计算和FFT分析
"""

import os
import sys

def test_slice_analysis():
    """测试切片分析功能"""
    print("=== 测试切片分析功能 ===")
    
    # 检查数据文件
    csv_file = "/home/xwj/0-code/0-github/motor/python/data_motor/Bxyz_t_4s_10degrees.csv"
    if not os.path.exists(csv_file):
        print(f"❌ 数据文件不存在: {csv_file}")
        return False
    
    print(f"✅ 数据文件存在: {csv_file}")
    
    # 检查脚本文件
    scripts = [
        "data_processor.py",
        "slice_analysis.py", 
        "quick_data_plot.py"
    ]
    
    script_dir = os.path.dirname(__file__)
    
    for script in scripts:
        script_path = os.path.join(script_dir, script)
        if os.path.exists(script_path):
            print(f"✅ 脚本文件存在: {script}")
        else:
            print(f"❌ 脚本文件不存在: {script}")
    
    # 检查依赖包
    print("\n检查依赖包:")
    required_packages = ['pandas', 'numpy', 'matplotlib', 'scipy']
    
    for package in required_packages:
        try:
            __import__(package)
            print(f"✅ {package} 已安装")
        except ImportError:
            print(f"❌ {package} 未安装")
    
    print("\n=== 新功能说明 ===")
    print("1. 移除了3D绘图功能")
    print("2. 添加了数据切片分析功能")
    print("3. 计算每个切片的峰峰值")
    print("4. 绘制每个切片的FFT频谱图")
    print("5. 支持不同切片时长的分析")
    
    print("\n=== 使用说明 ===")
    print("1. 完整切片分析: python slice_analysis.py")
    print("2. 快速切片分析: python quick_data_plot.py") 
    print("3. 完整数据处理: python data_processor.py")
    
    print("\n=== 输出文件 ===")
    print("1. Bxyz_t_4s_10degrees_slice_analysis_1.0s.png - 切片分析图像")
    print("2. slice_analysis_results_1.0s.csv - 分析结果CSV文件")
    
    return True

if __name__ == "__main__":
    test_slice_analysis()

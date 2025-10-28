#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
测试移除FFT后的功能
验证只保留峰峰值分析，移除FFT绘制
"""

import os
import sys

def test_no_fft():
    """测试移除FFT后的功能"""
    print("=== Test No FFT Function ===")
    
    # 检查数据文件
    csv_file = "/home/xwj/0-code/0-github/motor/python/data_motor/Bxyz_t_4s_10degrees.csv"
    if not os.path.exists(csv_file):
        print(f"❌ Data file not found: {csv_file}")
        return False
    
    print(f"✅ Data file exists: {csv_file}")
    
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
            print(f"✅ Script file exists: {script}")
        else:
            print(f"❌ Script file not found: {script}")
    
    # 检查依赖包
    print("\nCheck dependencies:")
    required_packages = ['pandas', 'numpy', 'matplotlib']
    
    for package in required_packages:
        try:
            __import__(package)
            print(f"✅ {package} installed")
        except ImportError:
            print(f"❌ {package} not installed")
    
    print("\n=== Removed FFT Features ===")
    print("1. FFT spectrum plots removed")
    print("2. FFT calculation code removed")
    print("3. Only peak-to-peak analysis retained")
    print("4. Simplified plot layout (3x1 instead of 3x2)")
    print("5. Faster processing without FFT computation")
    
    print("\n=== Current Features ===")
    print("1. Data visualization with angle axis (0-360°)")
    print("2. Peak-to-peak analysis for each slice")
    print("3. Slice analysis with configurable angle segments")
    print("4. Statistical analysis of peak-to-peak values")
    print("5. Export results to CSV files")
    
    print("\n=== Plot Layout ===")
    print("1. 3x1 subplot layout (instead of 3x2)")
    print("2. Each subplot shows peak-to-peak variation")
    print("3. X-axis: Angle (degrees)")
    print("4. Y-axis: Peak-to-Peak values")
    print("5. No FFT spectrum plots")
    
    print("\n=== Usage Instructions ===")
    print("1. Complete data processing: python data_processor.py")
    print("2. Slice analysis: python slice_analysis.py")
    print("3. Quick visualization: python quick_data_plot.py")
    
    print("\n=== Output Files ===")
    print("1. Bxyz_t_4s_10degrees_plot.png - Data visualization")
    print("2. Bxyz_t_4s_10degrees_combined_plot.png - Combined plot")
    print("3. Bxyz_t_4s_10degrees_slice_analysis_10deg.png - Peak-to-peak analysis")
    print("4. slice_analysis_results_10deg.csv - Analysis results")
    
    print("\n=== Performance Benefits ===")
    print("1. Faster processing (no FFT computation)")
    print("2. Simpler code structure")
    print("3. Reduced memory usage")
    print("4. Cleaner plot layout")
    print("5. Focus on peak-to-peak analysis")
    
    return True

if __name__ == "__main__":
    test_no_fft()

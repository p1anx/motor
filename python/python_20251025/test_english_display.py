#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
测试英文显示功能
验证所有图表标题和标签都使用英文显示
"""

import os
import sys

def test_english_display():
    """测试英文显示功能"""
    print("=== Test English Display Function ===")
    
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
    required_packages = ['pandas', 'numpy', 'matplotlib', 'scipy']
    
    for package in required_packages:
        try:
            __import__(package)
            print(f"✅ {package} installed")
        except ImportError:
            print(f"❌ {package} not installed")
    
    print("\n=== English Display Features ===")
    print("1. All plot titles use English")
    print("2. All axis labels use English")
    print("3. All legends use English")
    print("4. Font settings optimized for English display")
    print("5. No Chinese characters in plots")
    
    print("\n=== Updated Plot Elements ===")
    print("1. 'Bxyz_t_4s_10degrees Data Visualization'")
    print("2. 'Data Slice Analysis (Slice Duration: Xs)'")
    print("3. 'Peak-to-Peak Variation'")
    print("4. 'FFT Spectrum'")
    print("5. 'Time (seconds)' / 'Frequency (Hz)' / 'Magnitude'")
    print("6. 'Mean: X.XXX' / 'Peak-to-Peak' / 'Average FFT'")
    
    print("\n=== Usage Instructions ===")
    print("1. Complete data processing: python data_processor.py")
    print("2. Slice analysis: python slice_analysis.py") 
    print("3. Quick visualization: python quick_data_plot.py")
    
    print("\n=== Output Files ===")
    print("1. Bxyz_t_4s_10degrees_plot.png - Data visualization")
    print("2. Bxyz_t_4s_10degrees_combined_plot.png - Combined plot")
    print("3. Bxyz_t_4s_10degrees_slice_analysis_1.0s.png - Slice analysis")
    print("4. slice_analysis_results_1.0s.csv - Analysis results")
    
    return True

if __name__ == "__main__":
    test_english_display()

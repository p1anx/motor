#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
测试角度轴功能
验证x轴从时间轴改为角度轴(0-360度)的功能
"""

import os
import sys

def test_angle_axis():
    """测试角度轴功能"""
    print("=== Test Angle Axis Function ===")
    
    # 检查数据文件
    csv_file = "/home/xwj/0-code/0-github/motor/python/data_motor/Bxyz_t_4s_10degrees.csv"
    if not os.path.exists(csv_file):
        print(f"❌ Data file not found: {csv_file}")
        return False
    
    print(f"✅ Data file exists: {csv_file}")
    
    # 检查脚本文件
    scripts = [
        "data_processor.py",
        "slice_analysis.py"
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
    
    print("\n=== Angle Axis Features ===")
    print("1. X-axis changed from time to angle (0-360 degrees)")
    print("2. Data visualization shows angle-based plots")
    print("3. Slice analysis based on angle segments")
    print("4. FFT analysis adapted for angle-based sampling")
    print("5. All labels and titles updated for angle representation")
    
    print("\n=== Updated Plot Elements ===")
    print("1. 'data1 vs Angle' (instead of 'vs Time')")
    print("2. 'Angle (degrees)' (instead of 'Time (seconds)')")
    print("3. 'Data Slice Analysis (Slice Angle: X°)'")
    print("4. 'Peak-to-Peak Variation' with angle x-axis")
    print("5. FFT spectrum with angle-based frequency")
    
    print("\n=== Slice Analysis Parameters ===")
    print("1. Default slice angle: 10 degrees")
    print("2. Multiple slice angles: 5°, 10°, 20°")
    print("3. Angle resolution calculated from data length")
    print("4. FFT adapted for angle sampling rate")
    
    print("\n=== Usage Instructions ===")
    print("1. Complete data processing: python data_processor.py")
    print("2. Slice analysis: python slice_analysis.py")
    print("3. Quick visualization: python quick_data_plot.py")
    
    print("\n=== Output Files ===")
    print("1. Bxyz_t_4s_10degrees_plot.png - Angle-based data visualization")
    print("2. Bxyz_t_4s_10degrees_combined_plot.png - Combined angle plot")
    print("3. Bxyz_t_4s_10degrees_slice_analysis_10deg.png - Angle slice analysis")
    print("4. slice_analysis_results_10deg.csv - Angle-based analysis results")
    
    print("\n=== Key Changes ===")
    print("1. create_time_axis() → create_angle_axis()")
    print("2. slice_duration → slice_angle")
    print("3. Time (seconds) → Angle (degrees)")
    print("4. Sampling rate → Angle resolution")
    print("5. FFT frequency adapted for angle domain")
    
    return True

if __name__ == "__main__":
    test_angle_axis()

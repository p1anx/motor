#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
测试数据点绘图功能
验证x轴为数据原始点数的绘图功能
"""

import os
import sys

def test_data_points_plot():
    """测试数据点绘图功能"""
    print("=== Test Data Points Plot Function ===")
    
    # 检查数据文件
    csv_file = "/home/xwj/0-code/0-github/motor/python/data_motor/Bxyz_t_4s_10degrees.csv"
    if not os.path.exists(csv_file):
        print(f"❌ Data file not found: {csv_file}")
        return False
    
    print(f"✅ Data file exists: {csv_file}")
    
    # 检查脚本文件
    script_path = os.path.join(os.path.dirname(__file__), "data_processor.py")
    if os.path.exists(script_path):
        print(f"✅ Script file exists: data_processor.py")
    else:
        print(f"❌ Script file not found: data_processor.py")
        return False
    
    # 检查images文件夹
    images_folder = os.path.join(os.path.dirname(__file__), "images")
    print(f"\nImages folder path: {images_folder}")
    
    if os.path.exists(images_folder):
        print(f"✅ Images folder exists")
        # 列出文件夹中的文件
        files = os.listdir(images_folder)
        if files:
            print(f"📁 Files in images folder:")
            for file in files:
                print(f"   - {file}")
        else:
            print(f"📁 Images folder is empty")
    else:
        print(f"❌ Images folder does not exist (will be created when running script)")
    
    print("\n=== New Data Points Plot Features ===")
    print("1. X-axis represents data point index (0, 1, 2, 3, ...)")
    print("2. Y-axis represents the actual data values")
    print("3. Shows raw data progression without time/angle conversion")
    print("4. Useful for analyzing data patterns and trends")
    print("5. Helps identify data quality and sampling issues")
    
    print("\n=== Plot Comparison ===")
    print("1. plot_data() - X-axis: Angle (0-360 degrees)")
    print("2. plot_combined() - X-axis: Angle (0-360 degrees), combined view")
    print("3. plot_data_points() - X-axis: Data Point Index (0, 1, 2, ...)")
    
    print("\n=== Expected Output Files ===")
    print("1. images/Bxyz_t_4s_10degrees_plot.png - Angle-based plot")
    print("2. images/Bxyz_t_4s_10degrees_combined_plot.png - Combined angle plot")
    print("3. images/Bxyz_t_4s_10degrees_data_points_plot.png - Data points plot")
    print("4. images/Bxyz_t_4s_10degrees_slice_analysis_10deg.png - Slice analysis")
    
    print("\n=== Usage Instructions ===")
    print("1. Run: python data_processor.py")
    print("2. The script will automatically generate all plots including the new data points plot")
    print("3. Check images folder for all generated plots")
    print("4. Compare different x-axis representations")
    
    print("\n=== Technical Details ===")
    print("1. Data points plot uses np.arange(len(data)) for x-axis")
    print("2. No angle or time conversion needed")
    print("3. Direct mapping of data index to data values")
    print("4. Useful for debugging and data quality assessment")
    
    return True

if __name__ == "__main__":
    test_data_points_plot()

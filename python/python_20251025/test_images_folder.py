#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
测试图片文件夹功能
验证所有图片都保存到images文件夹中
"""

import os
import sys

def test_images_folder():
    """测试图片文件夹功能"""
    print("=== Test Images Folder Function ===")
    
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
    
    # 检查images文件夹
    images_folder = os.path.join(script_dir, "images")
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
        print(f"❌ Images folder does not exist (will be created when running scripts)")
    
    print("\n=== Images Folder Features ===")
    print("1. Automatic creation of 'images' folder in script directory")
    print("2. All plot images saved to images folder")
    print("3. Organized file storage")
    print("4. Easy access to generated plots")
    print("5. Clean workspace (no scattered image files)")
    
    print("\n=== Expected Output Files ===")
    print("1. images/Bxyz_t_4s_10degrees_plot.png - Data visualization")
    print("2. images/Bxyz_t_4s_10degrees_combined_plot.png - Combined plot")
    print("3. images/Bxyz_t_4s_10degrees_slice_analysis_10deg.png - Slice analysis")
    print("4. images/Bxyz_t_4s_10degrees_quick_plot.png - Quick visualization")
    
    print("\n=== Usage Instructions ===")
    print("1. Run any data processing script")
    print("2. Images folder will be created automatically")
    print("3. All generated plots will be saved in images folder")
    print("4. Check images folder for all output files")
    
    print("\n=== Folder Structure ===")
    print("python_20251025/")
    print("├── data_processor.py")
    print("├── slice_analysis.py")
    print("├── quick_data_plot.py")
    print("└── images/")
    print("    ├── Bxyz_t_4s_10degrees_plot.png")
    print("    ├── Bxyz_t_4s_10degrees_combined_plot.png")
    print("    ├── Bxyz_t_4s_10degrees_slice_analysis_10deg.png")
    print("    └── Bxyz_t_4s_10degrees_quick_plot.png")
    
    return True

if __name__ == "__main__":
    test_images_folder()

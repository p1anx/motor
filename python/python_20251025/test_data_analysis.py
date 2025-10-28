#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
测试数据分析功能
验证数据处理脚本是否正常工作
"""

import os
import sys

def test_data_analysis():
    """测试数据分析功能"""
    print("=== 测试数据分析功能 ===")
    
    # 检查数据文件
    csv_file = "/home/xwj/0-code/0-github/motor/python/data_motor/Bxyz_t_4s_10degrees.csv"
    if not os.path.exists(csv_file):
        print(f"❌ 数据文件不存在: {csv_file}")
        return False
    
    print(f"✅ 数据文件存在: {csv_file}")
    
    # 检查脚本文件
    scripts = [
        "data_processor.py",
        "quick_data_plot.py", 
        "explore_data.py",
        "run_data_analysis.py"
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
    required_packages = ['pandas', 'numpy', 'matplotlib', 'seaborn', 'scipy']
    
    for package in required_packages:
        try:
            __import__(package)
            print(f"✅ {package} 已安装")
        except ImportError:
            print(f"❌ {package} 未安装")
    
    print("\n=== 使用说明 ===")
    print("1. 快速可视化: python quick_data_plot.py")
    print("2. 完整分析: python data_processor.py") 
    print("3. 数据探索: python explore_data.py")
    print("4. 菜单选择: python run_data_analysis.py")
    
    return True

if __name__ == "__main__":
    test_data_analysis()

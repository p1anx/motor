#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
运行数据分析脚本
提供菜单选择不同的分析功能
"""

import os
import sys

def check_dependencies():
    """检查依赖包"""
    required_packages = ['pandas', 'numpy', 'matplotlib', 'seaborn', 'scipy']
    missing_packages = []
    
    for package in required_packages:
        try:
            __import__(package)
        except ImportError:
            missing_packages.append(package)
    
    if missing_packages:
        print(f"缺少以下依赖包: {', '.join(missing_packages)}")
        print("请安装: pip install pandas numpy matplotlib seaborn scipy")
        return False
    
    return True

def main():
    """主函数"""
    print("=== 数据分析工具 ===")
    print("1. 快速数据可视化 (quick_data_plot.py)")
    print("2. 完整数据分析 (data_processor.py)")
    print("3. 数据探索分析 (explore_data.py)")
    print("4. 退出")
    
    # 检查依赖
    if not check_dependencies():
        return
    
    # 检查数据文件
    csv_file = "/home/xwj/0-code/0-github/motor/python/data_motor/Bxyz_t_4s_10degrees.csv"
    if not os.path.exists(csv_file):
        print(f"数据文件不存在: {csv_file}")
        return
    
    while True:
        try:
            choice = input("\n请选择功能 (1-4): ").strip()
            
            if choice == '1':
                print("\n运行快速数据可视化...")
                os.system(f"python {os.path.dirname(__file__)}/quick_data_plot.py")
                break
                
            elif choice == '2':
                print("\n运行完整数据分析...")
                os.system(f"python {os.path.dirname(__file__)}/data_processor.py")
                break
                
            elif choice == '3':
                print("\n运行数据探索分析...")
                os.system(f"python {os.path.dirname(__file__)}/explore_data.py")
                break
                
            elif choice == '4':
                print("退出程序")
                break
                
            else:
                print("无效选择，请输入 1-4")
                
        except KeyboardInterrupt:
            print("\n程序被用户中断")
            break
        except Exception as e:
            print(f"运行错误: {e}")

if __name__ == "__main__":
    main()

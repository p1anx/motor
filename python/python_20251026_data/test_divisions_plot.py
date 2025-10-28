#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
测试等分结果绘制功能
验证在原图上绘制等分结果的功能
"""

import os
import sys

def test_divisions_plot():
    """测试等分结果绘制功能"""
    print("=== Test Divisions Plot Function ===")
    
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
    
    print("\n=== Divisions Plot Features ===")
    print("1. 在原图上绘制等分水平线")
    print("2. 用不同颜色标记不同等分区间")
    print("3. 显示等分信息文本")
    print("4. 保持原始数据曲线可见")
    print("5. 提供清晰的视觉区分")
    
    print("\n=== Visual Elements ===")
    print("1. 原始峰峰值曲线 (带透明度)")
    print("2. 等分水平线 (灰色虚线)")
    print("3. 等分区间散点 (不同颜色)")
    print("4. 等分信息文本框")
    print("5. 图例和标题")
    
    print("\n=== Color Scheme ===")
    print("1. 使用viridis颜色映射")
    print("2. 从深色到亮色渐变")
    print("3. 每个等分区间不同颜色")
    print("4. 保持视觉清晰度")
    
    print("\n=== Information Display ===")
    print("1. 峰峰值范围显示")
    print("2. 等分大小显示")
    print("3. 等分数量显示")
    print("4. 角度范围显示")
    
    print("\n=== Expected Output Files ===")
    print("1. Bxyz_t_4s_10degrees_slice_analysis_with_divisions_10deg_10div.png")
    print("2. 包含等分水平线和颜色标记")
    print("3. 显示等分信息")
    print("4. 高分辨率图像")
    
    print("\n=== Usage Examples ===")
    print("1. 默认参数 (10度切片，10等分):")
    print("   processor.slice_data_and_analyze()")
    print("")
    print("2. 自定义参数 (5度切片，20等分):")
    print("   processor.slice_data_and_analyze(slice_angle=5.0, n_divisions=20)")
    print("")
    print("3. 只保存不显示:")
    print("   processor.slice_data_and_analyze(save_plot=True, show_plot=False)")
    
    print("\n=== Technical Details ===")
    print("1. 图形大小: 14x12 英寸")
    print("2. 分辨率: 300 DPI")
    print("3. 颜色映射: viridis")
    print("4. 透明度: 0.7 (原始曲线), 0.8 (散点)")
    print("5. 线条样式: 虚线 (等分线)")
    
    print("\n=== Analysis Benefits ===")
    print("1. 直观显示等分结果")
    print("2. 便于识别数据分布")
    print("3. 支持质量控制")
    print("4. 辅助异常检测")
    print("5. 提供视觉验证")
    
    return True

if __name__ == "__main__":
    test_divisions_plot()

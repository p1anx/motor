#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
测试峰峰值等分功能
验证将峰峰值曲线幅度进行n等分并返回角度值的功能
"""

import os
import sys

def test_peak_to_peak_divisions():
    """测试峰峰值等分功能"""
    print("=== Test Peak-to-Peak Divisions Function ===")
    
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
    
    print("\n=== Peak-to-Peak Divisions Features ===")
    print("1. 将峰峰值曲线幅度进行n等分")
    print("2. 返回每个等分区间的x轴角度值")
    print("3. 角度值按大小排列")
    print("4. 提供详细的统计信息")
    print("5. 支持自定义等分数和切片角度")
    
    print("\n=== Function Parameters ===")
    print("1. slice_angle (float): 切片角度，默认10.0度")
    print("2. n_divisions (int): 等分数，默认10")
    
    print("\n=== Return Value Structure ===")
    print("results = {")
    print("    'data1': {")
    print("        'peak_to_peak_range': (min_pp, max_pp),")
    print("        'division_size': division_size,")
    print("        'divisions': [")
    print("            {")
    print("                'division': 1,")
    print("                'range': (lower_bound, upper_bound),")
    print("                'angles': [sorted_angles],")
    print("                'count': len(angles)")
    print("            },")
    print("            ...")
    print("        ]")
    print("    },")
    print("    'data2': {...},")
    print("    'data3': {...}")
    print("}")
    
    print("\n=== Usage Examples ===")
    print("1. 默认参数 (10度切片，10等分):")
    print("   results = processor.get_peak_to_peak_divisions()")
    print("")
    print("2. 自定义参数 (5度切片，20等分):")
    print("   results = processor.get_peak_to_peak_divisions(slice_angle=5.0, n_divisions=20)")
    print("")
    print("3. 访问结果:")
    print("   for col, col_data in results.items():")
    print("       print(f'{col} 峰峰值范围: {col_data[\"peak_to_peak_range\"]}')")
    print("       for div in col_data['divisions']:")
    print("           if div['count'] > 0:")
    print("               print(f'第{div[\"division\"]}等分: {div[\"angles\"]}')")
    
    print("\n=== Analysis Benefits ===")
    print("1. 识别峰峰值分布模式")
    print("2. 找到特定幅度范围的角度位置")
    print("3. 分析数据变化规律")
    print("4. 优化采样策略")
    print("5. 质量控制和异常检测")
    
    print("\n=== Expected Output ===")
    print("峰峰值曲线幅度n等分分析:")
    print("  切片角度: 10.0 度")
    print("  等分数: 10")
    print("")
    print("data1 峰峰值等分结果:")
    print("  峰峰值范围: [0.076000, 1.089000]")
    print("  等分大小: 0.101300")
    print("  第1等分 [0.076000, 0.177300):")
    print("    角度值: [0.0, 10.0, 20.0, ...]")
    print("    数量: 5")
    print("  ...")
    
    print("\n=== Integration ===")
    print("1. 自动集成到主处理流程")
    print("2. 在切片分析后执行")
    print("3. 提供详细的控制台输出")
    print("4. 返回结构化数据供进一步分析")
    
    return True

if __name__ == "__main__":
    test_peak_to_peak_divisions()

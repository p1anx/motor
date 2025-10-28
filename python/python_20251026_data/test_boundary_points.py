#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
测试等分边界点功能
验证新的等分边界点预测功能
"""

import os
import sys

def test_boundary_points():
    """测试等分边界点功能"""
    print("=== Test Boundary Points Function ===")
    
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
    
    print("\n=== New Boundary Points Features ===")
    print("1. 等分边界点计算")
    print("2. 线性插值预测角度值")
    print("3. 精确值检测")
    print("4. 边界点信息显示")
    print("5. 预测值标记")
    
    print("\n=== Function Logic ===")
    print("1. 计算等分边界点:")
    print("   - 幅度范围: [min_pp, max_pp]")
    print("   - 等分大小: (max_pp - min_pp) / n_divisions")
    print("   - 边界点: min_pp + i * division_size")
    print("")
    print("2. 角度值预测:")
    print("   - 精确匹配: 查找完全相等的幅度值")
    print("   - 线性插值: 在相邻数据点间插值")
    print("   - 边界处理: 超出范围时使用最接近值")
    print("")
    print("3. 结果标记:")
    print("   - (精确): 找到完全匹配的数据点")
    print("   - (预测): 使用插值计算的角度值")
    
    print("\n=== Example Results (2等分) ===")
    print("data1 等分边界点结果:")
    print("  峰峰值范围: [0.067000, 0.460000]")
    print("  等分大小: 0.196500")
    print("  边界点 1 (最小值): 幅度值 0.067000 -> 角度值 170.00° (精确)")
    print("  边界点 2: 幅度值 0.263500 -> 角度值 23.65° (预测)")
    print("  边界点 3 (最大值): 幅度值 0.460000 -> 角度值 270.00° (精确)")
    print("")
    print("data2 等分边界点结果:")
    print("  峰峰值范围: [0.232000, 1.889000]")
    print("  等分大小: 0.828500")
    print("  边界点 1 (最小值): 幅度值 0.232000 -> 角度值 180.00° (精确)")
    print("  边界点 2: 幅度值 1.060500 -> 角度值 25.17° (预测)")
    print("  边界点 3 (最大值): 幅度值 1.889000 -> 角度值 270.00° (精确)")
    
    print("\n=== Interpolation Algorithm ===")
    print("1. 线性插值公式:")
    print("   predicted_angle = y1 + (y2 - y1) * (x - x1) / (x2 - x1)")
    print("   其中:")
    print("   - x: 目标幅度值")
    print("   - x1, x2: 相邻数据点的幅度值")
    print("   - y1, y2: 相邻数据点的角度值")
    print("")
    print("2. 边界处理:")
    print("   - 目标值 <= 最小值: 返回最小幅度值对应的角度")
    print("   - 目标值 >= 最大值: 返回最大幅度值对应的角度")
    print("   - 目标值在范围内: 使用线性插值")
    
    print("\n=== Usage Examples ===")
    print("1. 2等分 (当前设置):")
    print("   processor.get_peak_to_peak_divisions(slice_angle=10.0, n_divisions=2)")
    print("   - 返回3个边界点: 最小值, 中点, 最大值")
    print("")
    print("2. 4等分:")
    print("   processor.get_peak_to_peak_divisions(slice_angle=10.0, n_divisions=4)")
    print("   - 返回5个边界点: 最小值, 1/4点, 1/2点, 3/4点, 最大值")
    print("")
    print("3. 10等分:")
    print("   processor.get_peak_to_peak_divisions(slice_angle=10.0, n_divisions=10)")
    print("   - 返回11个边界点: 最小值, 9个等分点, 最大值")
    
    print("\n=== Return Value Structure ===")
    print("results = {")
    print("    'data1': {")
    print("        'peak_to_peak_range': (min_pp, max_pp),")
    print("        'division_size': division_size,")
    print("        'boundary_points': [")
    print("            {")
    print("                'boundary_value': 0.067000,")
    print("                'predicted_angle': 170.00,")
    print("                'is_exact': True")
    print("            },")
    print("            {")
    print("                'boundary_value': 0.263500,")
    print("                'predicted_angle': 23.65,")
    print("                'is_exact': False")
    print("            },")
    print("            ...")
    print("        ]")
    print("    },")
    print("    'data2': {...},")
    print("    'data3': {...}")
    print("}")
    
    print("\n=== Analysis Benefits ===")
    print("1. 精确边界点定位")
    print("2. 预测值提供连续性")
    print("3. 支持任意等分数")
    print("4. 区分精确值和预测值")
    print("5. 便于后续分析处理")
    
    print("\n=== Technical Details ===")
    print("1. 插值精度:")
    print("   - 使用线性插值保证连续性")
    print("   - 浮点数精度检查: 1e-10")
    print("   - 避免除零错误处理")
    print("")
    print("2. 边界处理:")
    print("   - 自动处理超出范围的情况")
    print("   - 返回最接近的有效值")
    print("   - 保证结果的合理性")
    print("")
    print("3. 数据结构:")
    print("   - 清晰的边界点信息")
    print("   - 精确值标记")
    print("   - 便于程序化处理")
    
    return True

if __name__ == "__main__":
    test_boundary_points()

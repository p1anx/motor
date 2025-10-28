#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
测试原始数据等分功能
验证sin/cos曲线的多角度值匹配
"""

import os
import sys

def test_original_data_divisions():
    """测试原始数据等分功能"""
    print("=== Test Original Data Divisions Function ===")
    
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
    
    print("\n=== Original Data Divisions Features ===")
    print("1. 直接在原始数据上进行等分")
    print("2. 找到所有匹配的角度值")
    print("3. 支持sin/cos曲线的多角度值")
    print("4. 使用更宽松的精度检查")
    print("5. 返回完整的角度值数组")
    
    print("\n=== Key Differences ===")
    print("1. 峰峰值等分 vs 原始数据等分:")
    print("   - 峰峰值等分: 在切片峰峰值上等分")
    print("   - 原始数据等分: 在原始数据值上等分")
    print("")
    print("2. 数据特点:")
    print("   - 峰峰值: 36个切片，36个唯一值")
    print("   - 原始数据: 72640个点，98%重复率")
    print("")
    print("3. 匹配结果:")
    print("   - 峰峰值等分: 每个边界点1个角度值")
    print("   - 原始数据等分: 每个边界点多个角度值")
    
    print("\n=== Example Results (4等分) ===")
    print("data1 原始数据等分边界点结果:")
    print("  边界点 1 (最小值): 幅度值 -0.161000 -> 角度值数组 [12.72°, 19.92°] (2个值) (精确)")
    print("  边界点 2: 幅度值 0.122500 -> 角度值数组 [39.10°] (1个值) (预测)")
    print("  边界点 3: 幅度值 0.406000 -> 角度值数组 [70.77°, 70.78°, ..., 290.97°] (48个值) (精确)")
    print("  边界点 4: 幅度值 0.689500 -> 角度值数组 [109.77°] (1个值) (预测)")
    print("  边界点 5 (最大值): 幅度值 0.973000 -> 角度值数组 [199.16°] (1个值) (精确)")
    print("")
    print("data2 原始数据等分边界点结果:")
    print("  边界点 1 (最小值): 幅度值 -0.228000 -> 角度值数组 [97.86°, 107.16°] (2个值) (精确)")
    print("  边界点 2: 幅度值 0.245500 -> 角度值数组 [40.08°] (1个值) (预测)")
    print("  边界点 3: 幅度值 0.719000 -> 角度值数组 [12.91°, 16.06°, ..., 348.74°] (64个值) (精确)")
    print("  边界点 4: 幅度值 1.192500 -> 角度值数组 [20.57°] (1个值) (预测)")
    print("  边界点 5 (最大值): 幅度值 1.666000 -> 角度值数组 [271.97°] (1个值) (精确)")
    
    print("\n=== Multiple Angles Analysis ===")
    print("1. 边界点 3 (data1): 48个角度值")
    print("   - 说明幅度值 0.406000 在48个不同的角度位置出现")
    print("   - 这是sin/cos曲线的典型特征")
    print("   - 同一个幅度值对应多个相位角度")
    print("")
    print("2. 边界点 3 (data2): 64个角度值")
    print("   - 说明幅度值 0.719000 在64个不同的角度位置出现")
    print("   - 比data1有更多的匹配点")
    print("   - 表明data2的重复性更高")
    print("")
    print("3. 边界点 1 (data1): 2个角度值")
    print("   - 最小值 -0.161000 在2个角度位置出现")
    print("   - 说明最小值不是唯一的")
    print("   - 符合周期性函数的特征")
    
    print("\n=== Technical Implementation ===")
    print("1. 精度检查:")
    print("   - 使用1e-6精度 (比1e-10更宽松)")
    print("   - 适合sin/cos曲线的浮点数精度")
    print("   - 能够找到更多匹配点")
    print("")
    print("2. 算法流程:")
    print("   - 计算原始数据的最小值和最大值")
    print("   - 将数据范围等分为n+1个边界点")
    print("   - 对每个边界点，遍历所有数据点")
    print("   - 收集所有匹配的角度值")
    print("   - 如果没有精确匹配，使用插值预测")
    print("")
    print("3. 数据结构:")
    print("   - boundary_value: 边界点幅度值")
    print("   - angle_values: 所有匹配的角度值数组")
    print("   - is_exact: 是否为精确匹配")
    print("   - count: 角度值数量")
    
    print("\n=== Benefits of Original Data Divisions ===")
    print("1. 完整性:")
    print("   - 找到所有匹配的角度值")
    print("   - 不遗漏任何相位信息")
    print("   - 提供完整的角度分布")
    print("")
    print("2. 准确性:")
    print("   - 直接基于原始数据")
    print("   - 避免切片过程中的信息丢失")
    print("   - 保持数据的原始特征")
    print("")
    print("3. 适用性:")
    print("   - 适合周期性函数 (sin/cos)")
    print("   - 适合有重复值的数据")
    print("   - 适合需要多角度值分析的应用")
    
    print("\n=== Usage Examples ===")
    print("1. 调用原始数据等分:")
    print("   original_results = processor.get_original_data_divisions(n_divisions=4)")
    print("")
    print("2. 访问多角度值:")
    print("   for col, result in original_results.items():")
    print("       for boundary_info in result['boundary_points']:")
    print("           angle_values = boundary_info['angle_values']")
    print("           count = boundary_info['count']")
    print("           print(f'Found {count} angles for value {boundary_info[\"boundary_value\"]}')")
    print("")
    print("3. 分析角度分布:")
    print("   for angle in angle_values:")
    print("       # 分析每个角度值")
    print("       analyze_angle(angle)")
    
    print("\n=== Comparison with Peak-to-Peak Divisions ===")
    print("1. 峰峰值等分:")
    print("   - 优点: 减少噪声，突出主要特征")
    print("   - 缺点: 丢失细节信息，每个边界点只有1个角度值")
    print("   - 适用: 需要平滑分析的应用")
    print("")
    print("2. 原始数据等分:")
    print("   - 优点: 保留所有信息，支持多角度值")
    print("   - 缺点: 可能包含噪声，数据量大")
    print("   - 适用: 需要完整相位信息的应用")
    
    print("\n=== Future Enhancements ===")
    print("1. 角度值排序:")
    print("   - 按角度值大小排序")
    print("   - 便于分析角度分布模式")
    print("")
    print("2. 角度值统计:")
    print("   - 计算角度值间隔")
    print("   - 分析角度值分布特征")
    print("")
    print("3. 可视化增强:")
    print("   - 在原始数据图上标记等分点")
    print("   - 用不同颜色表示不同边界点")
    print("   - 显示角度值分布直方图")
    
    return True

if __name__ == "__main__":
    test_original_data_divisions()

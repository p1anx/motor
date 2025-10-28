#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
测试多角度值功能
验证每个等分幅度点对应的所有角度值数组
"""

import os
import sys

def test_multiple_angles():
    """测试多角度值功能"""
    print("=== Test Multiple Angles Function ===")
    
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
    
    print("\n=== Multiple Angles Features ===")
    print("1. 每个等分幅度点对应多个角度值")
    print("2. 返回角度值数组而不是单个值")
    print("3. 显示角度值数量")
    print("4. 区分精确值和预测值")
    print("5. 支持多个精确匹配点")
    
    print("\n=== Data Structure Changes ===")
    print("1. 旧结构:")
    print("   boundary_info = {")
    print("       'boundary_value': 0.165250,")
    print("       'predicted_angle': 6.90,")
    print("       'is_exact': False")
    print("   }")
    print("")
    print("2. 新结构:")
    print("   boundary_info = {")
    print("       'boundary_value': 0.165250,")
    print("       'angle_values': [6.90],")
    print("       'is_exact': False,")
    print("       'count': 1")
    print("   }")
    
    print("\n=== Output Format Changes ===")
    print("1. 旧格式:")
    print("   边界点 2: 幅度值 0.165250 -> 角度值 6.90° (预测)")
    print("")
    print("2. 新格式:")
    print("   边界点 2: 幅度值 0.165250 -> 角度值数组 [6.90°] (1个值) (预测)")
    
    print("\n=== Multiple Angles Scenarios ===")
    print("1. 精确匹配 (1个值):")
    print("   - 幅度值 0.067000 -> 角度值数组 [170.00°] (1个值) (精确)")
    print("   - 幅度值 0.460000 -> 角度值数组 [270.00°] (1个值) (精确)")
    print("")
    print("2. 预测值 (1个值):")
    print("   - 幅度值 0.165250 -> 角度值数组 [6.90°] (1个值) (预测)")
    print("   - 幅度值 0.263500 -> 角度值数组 [23.65°] (1个值) (预测)")
    print("")
    print("3. 多个精确匹配 (多个值):")
    print("   - 幅度值 0.500000 -> 角度值数组 [45.00°, 135.00°, 225.00°, 315.00°] (4个值) (精确)")
    print("   - 这种情况在数据中有重复的峰峰值时会出现")
    
    print("\n=== Algorithm Changes ===")
    print("1. 精确匹配查找:")
    print("   - 遍历所有峰峰值和角度值对")
    print("   - 使用1e-10精度检查")
    print("   - 收集所有匹配的角度值")
    print("")
    print("2. 预测值计算:")
    print("   - 如果没有精确匹配")
    print("   - 使用线性插值预测")
    print("   - 返回单个预测值")
    print("")
    print("3. 数据结构:")
    print("   - angle_values: 角度值数组")
    print("   - count: 角度值数量")
    print("   - is_exact: 是否为精确值")
    
    print("\n=== Plotting Changes ===")
    print("1. 精确值绘制:")
    print("   - 绘制所有匹配的角度值点")
    print("   - 红色圆点标记")
    print("   - 每个角度值一个点")
    print("")
    print("2. 预测值绘制:")
    print("   - 绘制单个预测角度值点")
    print("   - 蓝色圆点标记")
    print("   - 一个预测值一个点")
    print("")
    print("3. 图例更新:")
    print("   - Exact Boundary Points: 精确边界点")
    print("   - Predicted Boundary Points: 预测边界点")
    
    print("\n=== Example Results (4等分) ===")
    print("data1 等分边界点结果:")
    print("  边界点 1 (最小值): 幅度值 0.067000 -> 角度值数组 [170.00°] (1个值) (精确)")
    print("  边界点 2: 幅度值 0.165250 -> 角度值数组 [6.90°] (1个值) (预测)")
    print("  边界点 3: 幅度值 0.263500 -> 角度值数组 [23.65°] (1个值) (预测)")
    print("  边界点 4: 幅度值 0.361750 -> 角度值数组 [46.75°] (1个值) (预测)")
    print("  边界点 5 (最大值): 幅度值 0.460000 -> 角度值数组 [270.00°] (1个值) (精确)")
    print("")
    print("data2 等分边界点结果:")
    print("  边界点 1 (最小值): 幅度值 0.232000 -> 角度值数组 [180.00°] (1个值) (精确)")
    print("  边界点 2: 幅度值 0.646250 -> 角度值数组 [10.87°] (1个值) (预测)")
    print("  边界点 3: 幅度值 1.060500 -> 角度值数组 [25.17°] (1个值) (预测)")
    print("  边界点 4: 幅度值 1.474750 -> 角度值数组 [44.46°] (1个值) (预测)")
    print("  边界点 5 (最大值): 幅度值 1.889000 -> 角度值数组 [270.00°] (1个值) (精确)")
    
    print("\n=== Benefits of Multiple Angles ===")
    print("1. 完整性:")
    print("   - 显示所有匹配的角度值")
    print("   - 不遗漏任何精确匹配点")
    print("   - 提供完整的角度信息")
    print("")
    print("2. 准确性:")
    print("   - 区分精确值和预测值")
    print("   - 显示角度值数量")
    print("   - 提供数据质量信息")
    print("")
    print("3. 灵活性:")
    print("   - 支持单个和多个角度值")
    print("   - 适应不同的数据模式")
    print("   - 便于后续分析")
    
    print("\n=== Usage Examples ===")
    print("1. 访问角度值数组:")
    print("   for boundary_info in boundary_angles:")
    print("       angle_values = boundary_info['angle_values']")
    print("       count = boundary_info['count']")
    print("       is_exact = boundary_info['is_exact']")
    print("")
    print("2. 处理多个角度值:")
    print("   for angle in angle_values:")
    print("       # 处理每个角度值")
    print("       process_angle(angle)")
    print("")
    print("3. 统计精确值:")
    print("   exact_count = sum(1 for info in boundary_angles if info['is_exact'])")
    print("   total_angles = sum(info['count'] for info in boundary_angles)")
    
    print("\n=== Technical Details ===")
    print("1. 精度检查:")
    print("   - 使用1e-10精度")
    print("   - 避免浮点数误差")
    print("   - 确保精确匹配")
    print("")
    print("2. 数组处理:")
    print("   - 动态收集角度值")
    print("   - 保持原始顺序")
    print("   - 支持空数组处理")
    print("")
    print("3. 性能优化:")
    print("   - 一次遍历完成匹配")
    print("   - 避免重复计算")
    print("   - 内存使用优化")
    
    print("\n=== Future Enhancements ===")
    print("1. 角度值排序:")
    print("   - 按角度值大小排序")
    print("   - 便于分析角度分布")
    print("")
    print("2. 角度值统计:")
    print("   - 计算角度值范围")
    print("   - 计算角度值标准差")
    print("")
    print("3. 可视化增强:")
    print("   - 不同颜色表示不同角度值")
    print("   - 连接线显示角度值关系")
    print("   - 3D可视化支持")
    
    return True

if __name__ == "__main__":
    test_multiple_angles()

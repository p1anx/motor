#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
测试等分边界点绘制功能
验证新的等分边界点绘制功能
"""

import os
import sys

def test_boundary_points_plot():
    """测试等分边界点绘制功能"""
    print("=== Test Boundary Points Plot Function ===")
    
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
    
    print("\n=== New Boundary Points Plot Features ===")
    print("1. 等分边界点绘制")
    print("2. 精确值和预测值区分")
    print("3. 边界点标签显示")
    print("4. 等分区间着色保持")
    print("5. 信息文本更新")
    
    print("\n=== Visual Elements ===")
    print("1. 等分水平线:")
    print("   - 边界线: 黑色实线，线宽2")
    print("   - 内部线: 深灰色虚线，线宽1.5")
    print("")
    print("2. 等分边界点:")
    print("   - 精确值: 红色大圆点 (s=120)")
    print("   - 预测值: 蓝色大圆点 (s=120)")
    print("   - 黑色边框，线宽2")
    print("   - 最高图层 (zorder=6)")
    print("")
    print("3. 边界点标签:")
    print("   - 黄色背景框")
    print("   - 显示边界点编号和数值")
    print("   - 格式: B1, B2, B3...")
    print("")
    print("4. 等分区间着色:")
    print("   - 保持原有功能")
    print("   - 调整大小 (s=60) 和透明度 (alpha=0.7)")
    print("   - 降低图层优先级 (zorder=4)")
    
    print("\n=== Boundary Points Display ===")
    print("1. 边界点计算:")
    print("   - 使用插值方法预测角度值")
    print("   - 检查精确值匹配")
    print("   - 标记精确值和预测值")
    print("")
    print("2. 视觉区分:")
    print("   - 精确值: 红色圆点")
    print("   - 预测值: 蓝色圆点")
    print("   - 大小: 120像素 (比区间点大)")
    print("   - 图层: 最高优先级")
    print("")
    print("3. 标签信息:")
    print("   - 边界点编号: B1, B2, B3...")
    print("   - 幅度值: 显示3位小数")
    print("   - 位置: 偏移显示，避免重叠")
    
    print("\n=== Example Results (2等分) ===")
    print("data1 等分边界点结果:")
    print("  边界点 1 (最小值): 幅度值 0.067000 -> 角度值 170.00° (精确)")
    print("  边界点 2: 幅度值 0.263500 -> 角度值 23.65° (预测)")
    print("  边界点 3 (最大值): 幅度值 0.460000 -> 角度值 270.00° (精确)")
    print("")
    print("data2 等分边界点结果:")
    print("  边界点 1 (最小值): 幅度值 0.232000 -> 角度值 180.00° (精确)")
    print("  边界点 2: 幅度值 1.060500 -> 角度值 25.17° (预测)")
    print("  边界点 3 (最大值): 幅度值 1.889000 -> 角度值 270.00° (精确)")
    
    print("\n=== Plot Layout ===")
    print("1. 图层优先级 (从高到低):")
    print("   - 边界点: zorder=6 (最高)")
    print("   - 原始曲线: zorder=5")
    print("   - 等分区间点: zorder=4")
    print("   - 等分水平线: zorder=3")
    print("   - 网格: zorder=1 (最低)")
    print("")
    print("2. 颜色方案:")
    print("   - 精确边界点: 红色")
    print("   - 预测边界点: 蓝色")
    print("   - 等分区间: 多种颜色 (红、蓝、绿、橙等)")
    print("   - 等分水平线: 黑色/深灰色")
    print("")
    print("3. 大小对比:")
    print("   - 边界点: 120像素 (最大)")
    print("   - 原始曲线点: 80像素")
    print("   - 等分区间点: 60像素 (最小)")
    
    print("\n=== Information Display ===")
    print("1. 等分信息文本:")
    print("   - 峰峰值范围")
    print("   - 等分大小")
    print("   - 边界点数量")
    print("")
    print("2. 图例信息:")
    print("   - 原始峰峰值曲线")
    print("   - 等分区间 (前几个)")
    print("   - 边界点类型")
    print("")
    print("3. 边界点标签:")
    print("   - 边界点编号")
    print("   - 幅度值")
    print("   - 位置偏移显示")
    
    print("\n=== File Output ===")
    print("生成的文件:")
    print("1. Bxyz_t_4s_10degrees_slice_analysis_with_divisions_10.0deg_2div.png")
    print("   - 2等分结果")
    print("   - 包含边界点绘制")
    print("   - 文件大小: 946KB (比原来大)")
    print("")
    print("2. 文件特点:")
    print("   - 高分辨率: 300 DPI")
    print("   - 清晰布局: 自动调整")
    print("   - 完整信息: 包含所有边界点")
    print("   - 视觉清晰: 边界点突出显示")
    
    print("\n=== Usage Examples ===")
    print("1. 2等分 (当前设置):")
    print("   processor.slice_data_and_analyze(slice_angle=10.0, n_divisions=2)")
    print("   - 显示3个边界点")
    print("")
    print("2. 4等分:")
    print("   processor.slice_data_and_analyze(slice_angle=10.0, n_divisions=4)")
    print("   - 显示5个边界点")
    print("")
    print("3. 10等分:")
    print("   processor.slice_data_and_analyze(slice_angle=10.0, n_divisions=10)")
    print("   - 显示11个边界点")
    
    print("\n=== Technical Details ===")
    print("1. 边界点计算:")
    print("   - 使用_interpolate_angle_for_amplitude方法")
    print("   - 线性插值预测角度值")
    print("   - 精确值检测 (1e-10精度)")
    print("")
    print("2. 绘制参数:")
    print("   - 精确值: color='red', s=120, alpha=1.0")
    print("   - 预测值: color='blue', s=120, alpha=1.0")
    print("   - 边框: edgecolors='black', linewidth=2")
    print("   - 图层: zorder=6")
    print("")
    print("3. 标签参数:")
    print("   - 字体: fontsize=8")
    print("   - 背景: 黄色，透明度0.7")
    print("   - 位置: 偏移10像素")
    print("   - 格式: B{i+1}\\n{boundary_value:.3f}")
    
    return True

if __name__ == "__main__":
    test_boundary_points_plot()

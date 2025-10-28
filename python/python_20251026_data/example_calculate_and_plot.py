#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
峰峰值计算和绘图的完整示例
"""

from data_processor import DataProcessor

def main():
    """完整的计算和绘图流程示例"""
    
    print("=" * 60)
    print("峰峰值计算和可视化完整示例")
    print("=" * 60)
    
    # 1. 加载数据
    print("\n步骤1: 加载数据")
    print("-" * 60)
    csv_file = "/home/xwj/0-code/0-github/motor/python/data_motor/Bxyz_t_4s_10degrees.csv"
    processor = DataProcessor(csv_file)
    
    if not processor.load_data():
        print("数据加载失败!")
        return
    
    # 2. 计算峰峰值
    print("\n步骤2: 计算峰峰值")
    print("-" * 60)
    peak_to_peak_result = processor.get_peak_to_peak_array(slice_angle=10.0)
    
    print(f"\n峰峰值计算完成!")
    print(f"数据类型: {type(peak_to_peak_result)}")
    
    if isinstance(peak_to_peak_result, dict):
        for channel, values in peak_to_peak_result.items():
            print(f"\n{channel}:")
            print(f"  数据点数: {len(values)}")
            print(f"  平均峰峰值: {sum(values)/len(values):.6f}")
            print(f"  最大峰峰值: {max(values):.6f}")
            print(f"  最小峰峰值: {min(values):.6f}")
    
    # 3. 绘制图像
    print("\n步骤3: 绘制峰峰值曲线")
    print("-" * 60)
    processor.plot_peak_to_peak_array(
        peak_to_peak_data=peak_to_peak_result,
        slice_angle=10.0,
        save_plot=True,
        show_plot=False,
        title="Motor Data Peak-to-Peak Analysis"
    )
    
    print("\n图像已保存到images文件夹")
    
    print("\n" + "=" * 60)
    print("完成!")
    print("=" * 60)


if __name__ == "__main__":
    main()

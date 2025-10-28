import numpy as np
import matplotlib.pyplot as plt
from scipy.spatial.distance import cdist
import mylib as m

def find_closest_points_to_levels(x_data, y_data, num_levels=5):
    """
    将y数据等幅度划分，并找到靠近每个等分点的x数据
    
    Parameters:
    x_data: 位置信息
    y_data: 幅度信息
    num_levels: 等分的数量
    
    Returns:
    levels: 等分的幅度值
    closest_x_points: 靠近每个等分点的x数据
    closest_y_points: 靠近每个等分点的y数据
    """
    # 计算等分的幅度值
    y_min, y_max = min(y_data), max(y_data)
    levels = np.linspace(y_min, y_max, num_levels)
    
    closest_x_points = []
    closest_y_points = []
    
    for level in levels:
        # 找到与当前level最接近的y值的索引
        distances = np.abs(np.array(y_data) - level)
        min_idx = np.argmin(distances)
        
        # 也可以找到多个接近的点（比如在某个阈值范围内的点）
        threshold = (y_max - y_min) * 0.02  # 设定阈值为幅度范围的5%
        close_indices = np.where(distances <= threshold)[0]
        
        if len(close_indices) > 0:
            closest_x_points.append(x_data[close_indices])
            closest_y_points.append(y_data[close_indices])
        else:
            # 如果没有找到足够接近的点，就取最接近的一个
            closest_x_points.append([x_data[min_idx]])
            closest_y_points.append([y_data[min_idx]])
    
    return levels, closest_x_points, closest_y_points

def plot_data_with_levels(x_data, y_data, levels, closest_x_points, closest_y_points):
    """
    绘制原始数据和等分点
    """
    plt.figure(figsize=(12, 8))
    
    # 绘制原始数据
    plt.subplot(2, 1, 1)
    plt.plot(x_data, y_data, 'b-', linewidth=2, label='原始数据', alpha=0.7)
    
    # 绘制等分线
    for i, level in enumerate(levels):
        plt.axhline(y=level, color=f'C{i}', linestyle='--', alpha=0.6, 
                   label=f'等分线 {i+1}: {level:.2f}')
    
    # 标记靠近等分点的数据
    for i, (x_points, y_points) in enumerate(zip(closest_x_points, closest_y_points)):
        plt.scatter(x_points, y_points, s=80, alpha=0.8, 
                   label=f'靠近等分点 {i+1}', zorder=5)
    
    plt.xlabel('X (位置)')
    plt.ylabel('Y (幅度)')
    plt.title('原始数据与等分幅度点')
    plt.legend()
    plt.grid(True, alpha=0.3)
    
    # 绘制第二幅图：只显示选中的点
    plt.subplot(2, 1, 2)
    colors = plt.cm.tab10(np.linspace(0, 1, len(levels)))
    
    for i, (x_points, y_points) in enumerate(zip(closest_x_points, closest_y_points)):
        plt.scatter(x_points, y_points, s=100, c=[colors[i]], 
                   label=f'等分点 {i+1} (y≈{levels[i]:.2f})', alpha=0.8, zorder=5)
    
    plt.xlabel('X (位置)')
    plt.ylabel('Y (幅度)')
    plt.title('靠近等分幅度点的数据')
    plt.legend()
    plt.grid(True, alpha=0.3)
    
    plt.tight_layout()
    plt.show()

# 示例数据生成
def generate_sample_data():
    """生成示例数据"""
    np.random.seed(42)  # 为了结果可重现
    x = np.linspace(0, 10, 100)
    # 创建一个带有噪声的正弦波数据
    y = 3 * np.sin(x) + 2 * np.cos(2 * x) + 0.5 * np.random.randn(len(x)) + 2
    return x, y

# 主程序
if __name__ == "__main__":
    # 生成或输入数据
    # x_data, y_data = generate_sample_data()
    
    data = m.read_from_csv('data.csv')

    x_data = data[:, 0]
    y_data = data[:, 1]
    # 或者您可以输入自己的数据
    # x_data = [1, 2, 3, 4, 5, 6, 7, 8, 9, 10]
    # y_data = [2.1, 3.5, 1.8, 4.2, 2.9, 5.1, 3.7, 1.2, 4.8, 2.3]
    
    print("原始数据:")
    print(f"X数据范围: {min(x_data):.2f} 到 {max(x_data):.2f}")
    print(f"Y数据范围: {min(y_data):.2f} 到 {max(y_data):.2f}")
    
    # 设置等分数量
    num_levels = 8  # 可以调整这个值来改变等分的数量
    
    # 找到靠近等分点的数据
    levels, closest_x_points, closest_y_points = find_closest_points_to_levels(
        x_data, y_data, num_levels)
    
    print(f"\n等分的幅度值: {levels}")
    print(f"找到的x点数量: {[len(x_pts) for x_pts in closest_x_points]}")
    
    # 绘制结果
    plot_data_with_levels(x_data, y_data, levels, closest_x_points, closest_y_points)
    
    # 打印详细信息
    print("\n详细信息:")
    for i, (level, x_pts, y_pts) in enumerate(zip(levels, closest_x_points, closest_y_points)):
        print(f"等分点 {i+1} (目标幅度: {level:.2f}):")
        print(f"  找到 {len(x_pts)} 个点")
        print(f"  X坐标: {x_pts[:5]}{'...' if len(x_pts) > 5 else ''}")  # 只显示前5个
        print(f"  Y坐标: {y_pts[:5]}{'...' if len(y_pts) > 5 else ''}")
        print()
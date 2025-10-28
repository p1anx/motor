import numpy as np
import matplotlib.pyplot as plt
from scipy.spatial.distance import cdist
import mylib as m

def find_closest_points_to_levels_unique_phases(x_data, y_data, num_levels=5, max_points_per_level=2):
    """
    将y数据等幅度划分，并找到靠近每个等分点的唯一相位x数据
    
    Parameters:
    x_data: 位置信息
    y_data: 幅度信息
    num_levels: 等分的数量
    max_points_per_level: 每个等分点最多返回的x点数量
    
    Returns:
    levels: 等分的幅度值
    all_selected_x: 所有选中的x数据（已排序）
    all_selected_y: 对应的y数据
    """
    # 计算等分的幅度值
    y_min, y_max = min(y_data), max(y_data)
    levels = np.linspace(y_min, y_max, num_levels)
    
    all_selected_x = []
    all_selected_y = []
    
    for level in levels:
        # 找到与当前level最接近的y值的索引
        distances = np.abs(np.array(y_data) - level)
        
        # 找到所有接近该幅度的点
        threshold = (y_max - y_min) * 0.02  # 设定阈值为幅度范围的5%
        close_indices = np.where(distances <= threshold)[0]
        
        if len(close_indices) > 0:
            close_x = x_data[close_indices]
            close_y = y_data[close_indices]
            
            # 对于每个幅度，找到不同的相位（x值）
            # 先按x值排序
            sorted_indices = np.argsort(close_x)
            sorted_close_x = close_x[sorted_indices]
            sorted_close_y = close_y[sorted_indices]
            
            # 使用聚类思想，找到不同的x区域
            selected_x_for_level = []
            selected_y_for_level = []
            
            if len(sorted_close_x) == 1:
                selected_x_for_level.append(sorted_close_x[0])
                selected_y_for_level.append(sorted_close_y[0])
            else:
                # 计算相邻点之间的距离
                x_diffs = np.diff(sorted_close_x)
                avg_x_diff = np.mean(x_diffs)
                
                # 定义相位分离的阈值（可以调整）
                phase_separation_threshold = avg_x_diff * 2  # 超过平均间隔的2倍认为是不同相位
                
                # 找到相位分界点
                phase_boundaries = np.where(x_diffs > phase_separation_threshold)[0]
                
                if len(phase_boundaries) == 0:
                    # 如果没有明显的相位分界，选择两端的点
                    if len(sorted_close_x) >= max_points_per_level:
                        selected_x_for_level = [sorted_close_x[0], sorted_close_x[-1]]
                        selected_y_for_level = [sorted_close_y[0], sorted_close_y[-1]]
                    else:
                        selected_x_for_level = sorted_close_x.tolist()
                        selected_y_for_level = sorted_close_y.tolist()
                else:
                    # 按相位分组
                    start_idx = 0
                    for boundary in phase_boundaries:
                        group_x = sorted_close_x[start_idx:boundary+1]
                        group_y = sorted_close_y[start_idx:boundary+1]
                        
                        # 从每组中选择最接近目标值的点
                        group_distances = np.abs(group_y - level)
                        best_idx_in_group = np.argmin(group_distances)
                        selected_x_for_level.append(group_x[best_idx_in_group])
                        selected_y_for_level.append(group_y[best_idx_in_group])
                        
                        if len(selected_x_for_level) >= max_points_per_level:
                            break
                            
                        start_idx = boundary + 1
                    
                    # 处理最后一组
                    if start_idx < len(sorted_close_x) and len(selected_x_for_level) < max_points_per_level:
                        group_x = sorted_close_x[start_idx:]
                        group_y = sorted_close_y[start_idx:]
                        group_distances = np.abs(group_y - level)
                        best_idx_in_group = np.argmin(group_distances)
                        selected_x_for_level.append(group_x[best_idx_in_group])
                        selected_y_for_level.append(group_y[best_idx_in_group])
            
            # 如果找到的点少于要求的数量，补充最接近的点
            while len(selected_x_for_level) < max_points_per_level and len(close_x) > len(selected_x_for_level):
                current_distances = np.abs(y_data - level)
                # 排除已选的点
                used_indices = []
                for x_val in selected_x_for_level:
                    idx = (np.abs(x_data - x_val)).argmin()
                    used_indices.append(idx)
                
                available_distances = current_distances.copy()
                available_distances[used_indices] = np.inf  # 排除已选的点
                
                new_idx = np.argmin(available_distances)
                if available_distances[new_idx] <= threshold:
                    selected_x_for_level.append(x_data[new_idx])
                    selected_y_for_level.append(y_data[new_idx])
                else:
                    break
            
            all_selected_x.extend(selected_x_for_level[:max_points_per_level])
            all_selected_y.extend(selected_y_for_level[:max_points_per_level])
        else:
            # 如果没有找到足够接近的点，就取最接近的一个
            min_idx = np.argmin(distances)
            all_selected_x.append(x_data[min_idx])
            all_selected_y.append(y_data[min_idx])
    
    # 最终对所有选中的x值进行排序
    sort_indices = np.argsort(all_selected_x)
    all_selected_x = np.array(all_selected_x)[sort_indices]
    all_selected_y = np.array(all_selected_y)[sort_indices]
    
    return levels, all_selected_x, all_selected_y

def plot_data_with_levels(x_data, y_data, levels, all_selected_x, all_selected_y):
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
    plt.scatter(all_selected_x, all_selected_y, s=80, alpha=0.8, 
               color='red', label='选中的相位点', zorder=5)
    
    plt.xlabel('X (位置)')
    plt.ylabel('Y (幅度)')
    plt.title('原始数据与等分幅度点（唯一相位）')
    plt.legend()
    plt.grid(True, alpha=0.3)
    
    # 绘制第二幅图：只显示选中的点
    plt.subplot(2, 1, 2)
    
    # 为每个等分点使用不同颜色
    n_levels = len(levels)
    colors = plt.cm.tab10(np.linspace(0, 1, n_levels))
    
    # 计算每个选中点属于哪个等分点
    point_colors = []
    for y_val in all_selected_y:
        # 找到最接近的等分点
        level_distances = np.abs(levels - y_val)
        closest_level_idx = np.argmin(level_distances)
        point_colors.append(colors[closest_level_idx])
    
    plt.scatter(all_selected_x, all_selected_y, s=100, c=point_colors, 
               alpha=0.8, zorder=5, label='选中的相位点')
    
    plt.xlabel('X (位置)')
    plt.ylabel('Y (幅度)')
    plt.title('选中的唯一相位点（按等分点着色）')
    plt.legend()
    plt.grid(True, alpha=0.3)
    
    plt.tight_layout()
    plt.show()

# 示例数据生成
def generate_sample_data():
    """生成示例数据"""
    np.random.seed(42)  # 为了结果可重现
    x = np.linspace(0, 4*np.pi, 200)  # 更多周期，更好地展示相位特性
    # 创建一个带有噪声的正弦波数据
    y = 3 * np.sin(x) + 0.2 * np.random.randn(len(x)) + 1
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
    max_points_per_level = 2  # 每个等分点最多选择2个相位点
    
    # 找到靠近等分点的唯一相位数据
    levels, all_selected_x, all_selected_y = find_closest_points_to_levels_unique_phases(
        x_data, y_data, num_levels, max_points_per_level)
    
    print(f"\n等分的幅度值: {levels}")
    print(f"总共找到 {len(all_selected_x)} 个唯一相位点")
    
    # 绘制结果
    plot_data_with_levels(x_data, y_data, levels, all_selected_x, all_selected_y)
    
    # 打印详细信息
    print("\n详细信息:")
    print("排序后的所有选中点:")
    for i, (x, y) in enumerate(zip(all_selected_x, all_selected_y)):
        print(f"点 {i+1}: X={x:.3f}, Y={y:.3f}")
    
    # 按等分点分组显示
    print("\n按等分点分组:")
    for i, level in enumerate(levels):
        # 找到接近这个level的选中点
        indices = []
        for j, y_val in enumerate(all_selected_y):
            if abs(y_val - level) <= (max(y_data) - min(y_data)) * 0.02:
                indices.append(j)
        
        if indices:
            x_vals = [all_selected_x[idx] for idx in indices]
            y_vals = [all_selected_y[idx] for idx in indices]
            print(f"等分点 {i+1} (目标幅度: {level:.2f}):")
            print(f"  X坐标: {x_vals}")
            print(f"  Y坐标: {y_vals}")
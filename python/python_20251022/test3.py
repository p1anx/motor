import numpy as np
import matplotlib.pyplot as plt
from scipy.interpolate import interp1d
import mylib as m

def get_positions_for_equal_amplitude_divisions(positions, amplitudes, num_divisions):
    """
    给定位置和幅度数据，将幅度等分后返回对应的位置
    
    Parameters:
    - positions: 位置数组
    - amplitudes: 幅度数组  
    - num_divisions: 幅度等分数
    
    Returns:
    - equal_amplitudes: 等分的幅度值
    - corresponding_positions: 对应的位置值
    """
    # 确保输入数组长度一致
    if len(positions) != len(amplitudes):
        raise ValueError("位置和幅度数组长度必须一致")
    
    # 获取幅度范围
    min_amp = np.min(amplitudes)
    max_amp = np.max(amplitudes)
    
    # 创建等分的幅度值
    equal_amplitudes = np.linspace(min_amp, max_amp, num_divisions)
    
    # 创建插值函数：从幅度到位置的映射
    # 注意：正弦波等函数在某些区间可能不是单射，这里使用线性插值
    # 如果有多个x对应同一个y，我们会得到最接近的x值
    
    # 使用插值找到对应位置
    corresponding_positions = []
    
    for target_amp in equal_amplitudes:
        # 找到最接近目标幅度的位置
        closest_idx = np.argmin(np.abs(amplitudes - target_amp))
        corresponding_positions.append(positions[closest_idx])
    
    return equal_amplitudes, np.array(corresponding_positions)

def get_positions_with_interpolation(positions, amplitudes, num_divisions):
    """
    使用插值方法获取更精确的位置
    注意：这种方法假设数据是单调的，对于正弦波等周期函数需要特殊处理
    """
    # 获取幅度范围
    min_amp = np.min(amplitudes)
    max_amp = np.max(amplitudes)
    
    # 创建等分的幅度值
    equal_amplitudes = np.linspace(min_amp, max_amp, num_divisions)
    
    # 由于正弦波等函数不是单调的，我们需要分段处理
    # 这里使用最近邻方法
    corresponding_positions = []
    
    for target_amp in equal_amplitudes:
        # 找到最接近目标幅度的数据点
        distances = np.abs(amplitudes - target_amp)
        min_idx = np.argmin(distances)
        corresponding_positions.append(positions[min_idx])
    
    return equal_amplitudes, np.array(corresponding_positions)

# 示例使用
def example_usage():
    # 生成示例数据：正弦波
    x = np.linspace(0, 4*np.pi, 1000)  # 位置
    y = np.sin(x)                      # 幅度
    
    # 将幅度等分为10份
    num_divisions = 10
    
    equal_amplitudes, corresponding_positions = get_positions_for_equal_amplitude_divisions(
        x, y, num_divisions
    )
    
    print("等分的幅度值:", equal_amplitudes)
    print("对应的位置值:", corresponding_positions)
    
    # 绘图验证
    plt.figure(figsize=(12, 8))
    
    # 子图1：原始数据
    plt.subplot(2, 1, 1)
    plt.plot(x, y, 'b-', label='原始数据', linewidth=2)
    plt.scatter(corresponding_positions, equal_amplitudes, 
               color='red', s=80, zorder=5, 
               label='等分幅度点', edgecolors='black', linewidth=1)
    plt.grid(True, alpha=0.3)
    plt.xlabel('位置')
    plt.ylabel('幅度')
    plt.title('幅度等分后的位置映射')
    plt.legend()
    
    # 子图2：幅度-位置关系
    plt.subplot(2, 1, 2)
    plt.plot(y, x, 'g-', label='幅度 vs 位置', linewidth=2)
    plt.scatter(equal_amplitudes, corresponding_positions, 
               color='red', s=80, zorder=5, 
               label='等分点', edgecolors='black', linewidth=1)
    plt.grid(True, alpha=0.3)
    plt.xlabel('幅度')
    plt.ylabel('位置')
    plt.title('幅度-位置关系')
    plt.legend()
    
    plt.tight_layout()
    plt.show()
    
    return equal_amplitudes, corresponding_positions

# 更通用的函数，支持不同的插值方法
def get_positions_advanced(positions, amplitudes, num_divisions, method='nearest'):
    """
    高级版本：支持不同插值方法
    
    Parameters:
    - method: 'nearest', 'linear', 'cubic' 等
    """
    min_amp = np.min(amplitudes)
    max_amp = np.max(amplitudes)
    equal_amplitudes = np.linspace(min_amp, max_amp, num_divisions)
    
    if method == 'nearest':
        corresponding_positions = []
        for target_amp in equal_amplitudes:
            closest_idx = np.argmin(np.abs(amplitudes - target_amp))
            corresponding_positions.append(positions[closest_idx])
        return equal_amplitudes, np.array(corresponding_positions)
    
    else:
        # 对于非单调数据，可能需要分段处理
        # 这里提供一个简化版本
        from scipy.interpolate import NearestNDInterpolator
        
        # 创建幅度到位置的插值器
        interpolator = interp1d(amplitudes, positions, kind=method, 
                               bounds_error=False, fill_value='extrapolate')
        
        corresponding_positions = interpolator(equal_amplitudes)
        return equal_amplitudes, corresponding_positions

# 实际使用示例
if __name__ == "__main__":
    # 示例1：正弦波数据
    print("=== 正弦波示例 ===")
    # x_sine = np.linspace(0, 4*np.pi, 1000)
    # y_sine = np.sin(x_sine)
    data = m.read_from_csv('data.csv')

    x_sine = data[:, 0]
    y_sine = data[:, 1]
    
    amp_div, pos_div = get_positions_for_equal_amplitude_divisions(x_sine, y_sine, 16)
    
    print(f"幅度范围: [{np.min(y_sine):.3f}, {np.max(y_sine):.3f}]")
    print(f"等分幅度: {amp_div}")
    print(f"对应位置: {pos_div}")
    
    # 示例2：自定义数据
    print("\n=== 自定义数据示例 ===")
    # custom_positions = np.array([0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10])
    # custom_amplitudes = np.array([0.1, 0.3, 0.7, 0.9, 0.8, 0.5, 0.2, -0.1, -0.4, -0.8, -0.9])
    custom_positions = data[:, 0]
    custom_amplitudes = data[:, 1]
    
    custom_amp_div, custom_pos_div = get_positions_for_equal_amplitude_divisions(
        custom_positions, custom_amplitudes, 16
    )
    
    print(f"自定义幅度: {custom_amplitudes}")
    print(f"自定义位置: {custom_positions}")
    print(f"等分幅度: {custom_amp_div}")
    print(f"对应位置: {custom_pos_div}")
    
    # 绘制自定义数据
    plt.figure(figsize=(10, 6))
    plt.plot(custom_positions, custom_amplitudes, 'bo-', label='原始数据', linewidth=2, markersize=8)
    plt.scatter(custom_pos_div, custom_amp_div, color='red', s=100, zorder=5, 
               label='等分点', edgecolors='black', linewidth=2)
    plt.grid(True, alpha=0.3)
    plt.xlabel('位置')
    plt.ylabel('幅度')
    plt.title('自定义数据的幅度等分')
    plt.legend()
    plt.show()
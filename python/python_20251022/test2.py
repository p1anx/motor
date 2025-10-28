import numpy as np
import matplotlib.pyplot as plt
from scipy.interpolate import interp1d

def get_sine_coordinates_numerical(n_points, period=2*np.pi, samples=10000):
    """
    使用数值方法找到正弦波上幅度均等分割点的x坐标
    """
    # 生成高精度的正弦波数据
    x_full = np.linspace(0, period, samples)
    y_full = np.sin(x_full)
    
    # 创建幅度分割点
    amplitude_levels = np.linspace(-1, 1, n_points)
    
    x_coordinates = []
    
    for target_amplitude in amplitude_levels:
        # 找到最接近目标幅度的x坐标
        # 使用插值方法更精确
        closest_idx = np.argmin(np.abs(y_full - target_amplitude))
        x_coordinates.append(x_full[closest_idx])
    
    return np.array(x_coordinates), amplitude_levels

# 示例
n = 12
x_coords, y_levels = get_sine_coordinates_numerical(n)

print("幅度值:", y_levels)
print("x坐标:", x_coords)

# 绘图
x = np.linspace(0, 2*np.pi, 1000)
y = np.sin(x)

plt.figure(figsize=(12, 6))
plt.plot(x, y, 'b-', label='正弦波', linewidth=2)
plt.scatter(x_coords, y_levels, color='red', s=60, zorder=5, 
           label='幅度分割点', edgecolors='black')
plt.grid(True, alpha=0.3)
plt.xlabel('x')
plt.ylabel('sin(x)')
plt.title(f'sinx (n={n})')
plt.legend()
plt.show()

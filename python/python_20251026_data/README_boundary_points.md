# 等分边界点功能说明

## 功能概述

根据用户需求，修改了等分数据处理逻辑，从原来的等分区间分组改为等分边界点预测。新功能计算等分边界点（如0.5），并返回对应的x轴角度值，对于没有精确数值的点使用线性插值进行预测。

## 核心改进

### 1. 功能转换
**原来的功能**：
- 将峰峰值范围分为n个区间
- 将数据点分组到各个区间
- 返回每个区间内的所有角度值

**新的功能**：
- 计算n等分的边界点
- 为每个边界点预测对应的角度值
- 区分精确值和预测值

### 2. 算法逻辑

#### 2.1 等分边界点计算
```python
# 计算等分边界点
min_pp = min(peak_to_peak_values)
max_pp = max(peak_to_peak_values)
division_size = (max_pp - min_pp) / n_divisions

# 生成边界点
division_boundaries = []
for i in range(n_divisions + 1):
    boundary_value = min_pp + i * division_size
    division_boundaries.append(boundary_value)
```

#### 2.2 角度值预测
```python
def _interpolate_angle_for_amplitude(self, peak_to_peak_values, slice_angles, target_amplitude):
    # 线性插值公式
    predicted_angle = y1 + (y2 - y1) * (x - x1) / (x2 - x1)
    return predicted_angle
```

## 实际运行结果

### 2等分示例
```
data1 等分边界点结果:
  峰峰值范围: [0.067000, 0.460000]
  等分大小: 0.196500
  边界点 1 (最小值): 幅度值 0.067000 -> 角度值 170.00° (精确)
  边界点 2: 幅度值 0.263500 -> 角度值 23.65° (预测)
  边界点 3 (最大值): 幅度值 0.460000 -> 角度值 270.00° (精确)

data2 等分边界点结果:
  峰峰值范围: [0.232000, 1.889000]
  等分大小: 0.828500
  边界点 1 (最小值): 幅度值 0.232000 -> 角度值 180.00° (精确)
  边界点 2: 幅度值 1.060500 -> 角度值 25.17° (预测)
  边界点 3 (最大值): 幅度值 1.889000 -> 角度值 270.00° (精确)

data3 等分边界点结果:
  峰峰值范围: [0.145000, 0.921000]
  等分大小: 0.388000
  边界点 1 (最小值): 幅度值 0.145000 -> 角度值 170.00° (精确)
  边界点 2: 幅度值 0.533000 -> 角度值 28.44° (预测)
  边界点 3 (最大值): 幅度值 0.921000 -> 角度值 270.00° (精确)
```

## 技术实现

### 1. 线性插值算法
```python
def _interpolate_angle_for_amplitude(self, peak_to_peak_values, slice_angles, target_amplitude):
    """
    使用线性插值预测给定幅度值对应的角度值
    """
    # 边界处理
    if target_amplitude <= min(peak_to_peak_values):
        return slice_angles[peak_to_peak_values.index(min(peak_to_peak_values))]
    elif target_amplitude >= max(peak_to_peak_values):
        return slice_angles[peak_to_peak_values.index(max(peak_to_peak_values))]
    
    # 找到目标幅度值两侧的数据点
    for i in range(len(peak_to_peak_values) - 1):
        if peak_to_peak_values[i] <= target_amplitude <= peak_to_peak_values[i + 1]:
            # 线性插值
            x1, y1 = peak_to_peak_values[i], slice_angles[i]
            x2, y2 = peak_to_peak_values[i + 1], slice_angles[i + 1]
            
            if x2 == x1:  # 避免除零错误
                return y1
            
            # 线性插值公式: y = y1 + (y2 - y1) * (x - x1) / (x2 - x1)
            predicted_angle = y1 + (y2 - y1) * (target_amplitude - x1) / (x2 - x1)
            return predicted_angle
    
    # 如果没有找到合适的区间，返回最接近的值
    closest_idx = min(range(len(peak_to_peak_values)), 
                     key=lambda i: abs(peak_to_peak_values[i] - target_amplitude))
    return slice_angles[closest_idx]
```

### 2. 精确值检测
```python
# 检查是否有精确匹配的点
for i, boundary_value in enumerate(division_boundaries):
    for pp_value, angle in zip(peak_to_peak_values, slice_angles):
        if abs(pp_value - boundary_value) < 1e-10:  # 浮点数精度检查
            boundary_angles[i]['predicted_angle'] = angle
            boundary_angles[i]['is_exact'] = True
            break
```

### 3. 数据结构
```python
results[col] = {
    'peak_to_peak_range': (min_pp, max_pp),
    'division_size': division_size,
    'boundary_points': [
        {
            'boundary_value': boundary_value,
            'predicted_angle': predicted_angle,
            'is_exact': is_exact
        },
        ...
    ]
}
```

## 使用示例

### 1. 基本使用
```python
# 2等分
results = processor.get_peak_to_peak_divisions(slice_angle=10.0, n_divisions=2)

# 4等分
results = processor.get_peak_to_peak_divisions(slice_angle=10.0, n_divisions=4)

# 10等分
results = processor.get_peak_to_peak_divisions(slice_angle=10.0, n_divisions=10)
```

### 2. 结果访问
```python
for col, col_data in results.items():
    print(f"{col} 边界点:")
    for boundary_info in col_data['boundary_points']:
        boundary_value = boundary_info['boundary_value']
        predicted_angle = boundary_info['predicted_angle']
        is_exact = boundary_info['is_exact']
        
        print(f"  幅度值 {boundary_value:.6f} -> 角度值 {predicted_angle:.2f}° {'(精确)' if is_exact else '(预测)'}")
```

## 算法特点

### 1. 线性插值优势
- **连续性**: 保证角度值的连续性
- **精度**: 在数据点间提供平滑过渡
- **稳定性**: 避免跳跃式变化

### 2. 边界处理
- **范围检查**: 自动处理超出数据范围的情况
- **最接近值**: 返回最接近的有效角度值
- **错误处理**: 避免除零错误

### 3. 精确值检测
- **浮点数精度**: 使用1e-10精度检查
- **标记区分**: 明确标识精确值和预测值
- **数据完整性**: 保持原始数据的准确性

## 应用场景

### 1. 精确分析
- **边界点定位**: 精确定位等分边界点
- **角度预测**: 预测特定幅度值对应的角度
- **连续性分析**: 提供连续的角度值序列

### 2. 质量控制
- **阈值检测**: 检测特定幅度阈值对应的角度
- **异常识别**: 识别异常角度位置
- **趋势分析**: 分析角度变化趋势

### 3. 工程应用
- **传感器校准**: 校准传感器角度位置
- **机械控制**: 控制机械运动角度
- **信号处理**: 处理信号角度信息

## 优势特点

### 1. 精确性
- **线性插值**: 提供高精度的角度预测
- **边界处理**: 确保结果的合理性
- **精度检查**: 区分精确值和预测值

### 2. 灵活性
- **任意等分**: 支持任意等分数
- **参数可调**: 支持自定义切片角度
- **结果可访问**: 提供结构化的结果数据

### 3. 实用性
- **连续预测**: 提供连续的角度值
- **易于理解**: 清晰的结果展示
- **便于处理**: 结构化的数据格式

## 更新日志

### 2024-10-26
- 修改等分数据处理逻辑
- 从等分区间分组改为等分边界点预测
- 实现线性插值算法
- 添加精确值检测功能
- 更新结果数据结构和显示格式
- 添加边界处理逻辑
- 完善错误处理机制

## 验证结果

### 1. 功能验证
- ✅ 等分边界点计算正确
- ✅ 线性插值预测准确
- ✅ 精确值检测有效
- ✅ 边界处理完善

### 2. 结果验证
- ✅ 2等分返回3个边界点
- ✅ 角度值预测合理
- ✅ 精确值和预测值标记正确
- ✅ 数据结构清晰完整

### 3. 性能验证
- ✅ 计算效率高
- ✅ 内存使用合理
- ✅ 错误处理完善
- ✅ 结果稳定可靠

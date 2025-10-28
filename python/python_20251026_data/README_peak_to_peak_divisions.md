# 峰峰值等分功能说明

## 功能概述

新增了 `get_peak_to_peak_divisions()` 方法，将切片数据获取的峰峰值曲线幅度进行n等分，返回n等分所有的x轴角度值并按大小排列。

## 主要特点

### 1. 核心功能
- **峰峰值等分**: 将峰峰值范围均匀分为n个区间
- **角度值映射**: 找到每个等分区间对应的角度值
- **排序输出**: 角度值按大小排列
- **统计分析**: 提供详细的统计信息

### 2. 方法签名
```python
def get_peak_to_peak_divisions(self, slice_angle=10.0, n_divisions=10):
    """
    将切片数据获取的峰峰值曲线幅度进行n等分，返回n等分所有的x轴角度值并按大小排列
    
    Args:
        slice_angle (float): 切片角度 (度)
        n_divisions (int): 等分数
        
    Returns:
        dict: 包含每个数据列的n等分角度值的字典
    """
```

## 返回值结构

### 数据结构
```python
results = {
    'data1': {
        'peak_to_peak_range': (min_pp, max_pp),  # 峰峰值范围
        'division_size': division_size,           # 等分大小
        'divisions': [                            # 等分信息列表
            {
                'division': 1,                    # 等分序号
                'range': (lower_bound, upper_bound),  # 等分区间
                'angles': [sorted_angles],        # 排序后的角度值
                'count': len(angles)              # 角度值数量
            },
            ...
        ]
    },
    'data2': {...},
    'data3': {...}
}
```

### 输出示例
```
data1 峰峰值等分结果:
  峰峰值范围: [0.067000, 0.460000]
  等分大小: 0.039300
  第1等分 [0.067000, 0.106300):
    角度值: [0.0, 160.0, 170.0, 180.0, 350.0]
    数量: 5
  第2等分 [0.106300, 0.145600):
    角度值: [340.0]
    数量: 1
  ...
```

## 使用示例

### 1. 基本使用
```python
# 默认参数 (10度切片，10等分)
results = processor.get_peak_to_peak_divisions()

# 自定义参数 (5度切片，20等分)
results = processor.get_peak_to_peak_divisions(slice_angle=5.0, n_divisions=20)
```

### 2. 结果访问
```python
for col, col_data in results.items():
    print(f"{col} 峰峰值范围: {col_data['peak_to_peak_range']}")
    
    for div in col_data['divisions']:
        if div['count'] > 0:
            print(f"第{div['division']}等分: {div['angles']}")
```

### 3. 统计分析
```python
# 找到数据最多的等分
max_count_division = max(col_data['divisions'], key=lambda x: x['count'])
print(f"数据最多的等分: 第{max_count_division['division']}等分")

# 计算数据覆盖率
non_empty_divisions = [div for div in col_data['divisions'] if div['count'] > 0]
coverage = len(non_empty_divisions) / len(col_data['divisions']) * 100
print(f"数据覆盖率: {coverage:.1f}%")
```

## 实际应用场景

### 1. 质量控制
- **异常检测**: 识别异常幅度范围
- **质量评估**: 评估数据采集质量
- **稳定性分析**: 分析系统稳定性

### 2. 优化采样
- **关键区域识别**: 找到重要角度区域
- **采样策略优化**: 优化数据采集策略
- **资源分配**: 合理分配采样资源

### 3. 模式识别
- **分布规律**: 分析幅度分布规律
- **周期性分析**: 识别周期性模式
- **趋势分析**: 分析数据变化趋势

### 4. 异常检测
- **异常定位**: 定位异常数据点
- **故障诊断**: 辅助故障诊断
- **预警系统**: 建立预警机制

## 技术实现

### 1. 算法流程
1. **数据切片**: 按指定角度切片数据
2. **峰峰值计算**: 计算每个切片的峰峰值
3. **范围确定**: 确定峰峰值的最小值和最大值
4. **等分区间**: 将范围均匀分为n个区间
5. **角度映射**: 将峰峰值映射到对应角度
6. **排序输出**: 对角度值进行排序

### 2. 关键代码
```python
# 计算切片参数
slice_points = int(slice_angle * data_length / total_degrees)
total_slices = data_length // slice_points

# 计算峰峰值
for i in range(total_slices):
    start_idx = i * slice_points
    end_idx = min((i + 1) * slice_points, len(self.data))
    slice_data = self.data[col].iloc[start_idx:end_idx]
    peak_to_peak = slice_data.max() - slice_data.min()

# 等分区间
division_size = (max_pp - min_pp) / n_divisions
divisions = [(min_pp + i * division_size, min_pp + (i + 1) * division_size) 
             for i in range(n_divisions)]

# 角度映射和排序
for pp_value, angle in zip(peak_to_peak_values, slice_angles):
    for div_idx, (lower, upper) in enumerate(divisions):
        if lower <= pp_value < upper:
            division_angles[div_idx].append(angle)
            break

sorted_angles = sorted(angles)
```

## 性能特点

### 1. 计算效率
- **时间复杂度**: O(n×m)，n为切片数，m为等分数
- **空间复杂度**: O(n×m)
- **内存使用**: 合理的内存使用

### 2. 精度控制
- **浮点精度**: 使用高精度浮点运算
- **边界处理**: 正确处理区间边界
- **数据完整性**: 确保数据完整性

### 3. 扩展性
- **参数可调**: 支持自定义切片角度和等分数
- **多列支持**: 支持多列数据同时分析
- **结果导出**: 支持结果导出到文件

## 集成方式

### 1. 自动集成
- 集成到主处理流程
- 在切片分析后自动执行
- 提供详细的控制台输出

### 2. 独立使用
- 可独立调用方法
- 支持自定义参数
- 返回结构化数据

### 3. 结果导出
- 自动导出到文本文件
- 支持进一步分析
- 便于结果分享

## 测试验证

### 1. 功能测试
- 基本功能验证
- 参数测试
- 边界条件测试

### 2. 性能测试
- 大数据集测试
- 内存使用测试
- 计算时间测试

### 3. 结果验证
- 结果正确性验证
- 统计信息验证
- 导出功能验证

## 更新日志

### 2024-10-26
- 新增 `get_peak_to_peak_divisions()` 方法
- 实现峰峰值等分功能
- 添加角度值排序功能
- 集成到主处理流程
- 创建示例和测试脚本
- 添加详细文档说明

## 未来扩展

### 1. 功能扩展
- 支持更多等分策略
- 添加可视化功能
- 支持实时分析

### 2. 性能优化
- 并行计算支持
- 内存优化
- 计算加速

### 3. 应用扩展
- 机器学习集成
- 实时监控
- 自动化分析

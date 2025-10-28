# 峰峰值计算函数使用说明

## 功能概述

`get_peak_to_peak_array()` 是一个通用的峰峰值计算函数，可以根据指定角度对数据进行切片，并返回每个切片的峰峰值数组。

## 函数签名

```python
def get_peak_to_peak_array(self, data=None, slice_angle=10.0, total_degrees=360):
    """
    通用的峰峰值计算函数，可以接受任意输入数据
    
    Args:
        data (pd.DataFrame, np.array, list, or None): 输入数据
            - 如果是DataFrame，会对每一列进行计算
            - 如果是np.array或list，对单列数据计算
            - 如果为None，使用self.data
        slice_angle (float): 切片角度 (度)
        total_degrees (float): 总角度范围，默认360度
        
    Returns:
        dict or list: 
            - 如果输入是多列数据（DataFrame或多列array），返回dict，key为列名或索引
            - 如果输入是单列数据，返回list
    """
```

## 使用示例

### 1. 使用类内部数据

```python
from data_processor import DataProcessor

# 创建数据处理器并加载数据
processor = DataProcessor("data.csv")
processor.load_data()
processor.create_angle_axis(total_degrees=360)

# 调用函数
result = processor.get_peak_to_peak_array(slice_angle=10.0)
# 返回: dict, 例如 {'data1': [...], 'data2': [...], 'data3': [...]}
```

### 2. 使用外部DataFrame数据

```python
import pandas as pd
import numpy as np

# 创建测试数据
test_data = pd.DataFrame({
    'ch1': np.sin(np.linspace(0, 4*np.pi, 1000)),
    'ch2': np.cos(np.linspace(0, 4*np.pi, 1000))
})

# 使用函数
processor = DataProcessor("dummy.csv")
result = processor.get_peak_to_peak_array(data=test_data, slice_angle=30.0)
# 返回: dict, 例如 {'ch1': [...], 'ch2': [...]}
```

### 3. 使用numpy数组（单列）

```python
import numpy as np

# 创建单列数据
test_data = np.sin(np.linspace(0, 4*np.pi, 1000))

# 使用函数
processor = DataProcessor("dummy.csv")
result = processor.get_peak_to_peak_array(data=test_data, slice_angle=30.0)
# 返回: list, 例如 [0.5, 0.6, 0.4, ...]
```

### 4. 使用多列numpy数组

```python
import numpy as np

# 创建多列数据
test_data = np.column_stack([
    np.sin(np.linspace(0, 4*np.pi, 1000)),
    np.cos(np.linspace(0, 4*np.pi, 1000)),
    np.sin(2*np.linspace(0, 4*np.pi, 1000))
])

# 使用函数
processor = DataProcessor("dummy.csv")
result = processor.get_peak_to_peak_array(data=test_data, slice_angle=30.0)
# 返回: dict, 例如 {0: [...], 1: [...], 2: [...]}
```

### 5. 使用list数据

```python
# 创建list数据
test_data = [np.sin(x) for x in np.linspace(0, 4*np.pi, 1000)]

# 使用函数
processor = DataProcessor("dummy.csv")
result = processor.get_peak_to_peak_array(data=test_data, slice_angle=30.0)
# 返回: list
```

## 参数说明

- **data**: 输入数据，可以是以下类型：
  - `pd.DataFrame`: 多列数据，会对每一列分别计算峰峰值
  - `np.array`: 一维数组作为单列，二维数组作为多列
  - `list`: 一维列表作为单列数据
  - `None`: 使用类的 `self.data` 属性
  
- **slice_angle**: 切片角度（度），默认10.0度
  - 较小的角度值会产生更多的切片
  - 较大的角度值会产生更少的切片
  
- **total_degrees**: 总角度范围，默认360度
  - 用于计算每个切片包含的数据点数

## 返回值

- **多列数据**: 返回 `dict`，key为列名（DataFrame）或索引（array）
  ```python
  {'ch1': [0.5, 0.6, 0.4, ...], 'ch2': [0.3, 0.7, 0.5, ...]}
  ```

- **单列数据**: 返回 `list`
  ```python
  [0.5, 0.6, 0.4, 0.7, ...]
  ```

## 与原始函数的区别

### 原始函数 `slice_data_and_analyze()`

- 只使用类内部数据
- 包含绘图功能
- 返回值类型为 `bool`
- 功能相对固定

### 新函数 `get_peak_to_peak_array()`

- ✅ 可以接受任意输入数据
- ✅ 支持多种数据格式（DataFrame, array, list）
- ✅ 返回峰峰值数组，便于后续处理
- ✅ 不依赖绘图，更轻量级
- ✅ 更灵活，可以在不同场景下使用

## 绘图功能

`plot_peak_to_peak_array()` 函数可以绘制峰峰值数组的可视化图像：

```python
# 绘制峰峰值曲线
processor.plot_peak_to_peak_array(
    peak_to_peak_data=result,      # 峰峰值数据
    slice_angle=10.0,              # 切片角度
    save_plot=True,                # 是否保存图像
    show_plot=False,               # 是否显示图像
    title="My Peak-to-Peak Plot"   # 图像标题
)
```

### 参数说明

- **peak_to_peak_data**: 峰峰值数据，可以是：
  - `list`: 单通道数据，绘制一条曲线
  - `dict`: 多通道数据，每条曲线对应一个key
  
- **slice_angle**: 切片角度（度），用于创建x轴
  
- **save_plot**: 是否保存图像到文件
  
- **show_plot**: 是否显示图像
  
- **title**: 图像标题

## 测试

运行测试脚本查看完整示例：

```bash
# 测试峰峰值计算
python test_peak_to_peak.py

# 测试绘图功能
python test_plot_peak_to_peak.py

# 完整计算和绘图示例
python example_calculate_and_plot.py
```

## 应用场景

1. **数据分析**: 分析周期性数据的峰峰值变化
2. **质量控制**: 检测信号幅度的稳定性
3. **特征提取**: 作为时域特征进行分析
4. **批量处理**: 处理多组数据并比较结果

## 注意事项

1. 确保 `slice_angle` 的值合理，避免产生过多或过少的切片
2. 多列数据的返回值是字典，单列数据的返回值是列表
3. 函数会自动处理数据边界，最后一个切片可能包含的数据点较少
4. 如果输入数据为空或切片参数不合适，函数会返回 `None`

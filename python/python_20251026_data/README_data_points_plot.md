# 数据点绘图功能说明

## 功能概述

新增了一个绘图功能 `plot_data_points()`，该功能以数据原始点数作为x轴来绘制数据图像。

## 主要特点

### 1. X轴表示
- **数据点索引**: 0, 1, 2, 3, ..., n-1
- **无转换**: 直接使用数据点的原始索引
- **线性映射**: 每个数据点对应一个x轴位置

### 2. 与其他绘图方法的对比

| 绘图方法 | X轴表示 | 用途 |
|---------|---------|------|
| `plot_data()` | 角度 (0-360度) | 角度域分析 |
| `plot_combined()` | 角度 (0-360度) | 组合角度视图 |
| `plot_data_points()` | 数据点索引 (0, 1, 2, ...) | 原始数据趋势分析 |

### 3. 技术实现

```python
def plot_data_points(self, save_plot=True, show_plot=True):
    # 创建数据点索引
    data_points = np.arange(len(self.data))
    
    # 绘制每个数据列
    for i, (col, color) in enumerate(zip(columns, colors)):
        ax.plot(data_points, self.data[col], color=color, linewidth=1, alpha=0.8, label=col)
```

## 应用场景

### 1. 数据质量检查
- 识别数据采集问题
- 检测异常数据点
- 验证数据完整性

### 2. 趋势分析
- 观察数据变化趋势
- 识别周期性模式
- 分析数据稳定性

### 3. 调试和验证
- 验证数据处理流程
- 检查数据转换正确性
- 对比不同分析方法

## 输出文件

### 文件位置
```
python_20251026_data/images/Bxyz_t_4s_10degrees_data_points_plot.png
```

### 文件特点
- **高分辨率**: 300 DPI
- **清晰布局**: 自动调整布局
- **英文标签**: 避免字体显示问题
- **网格显示**: 便于读取数值

## 使用方法

### 1. 直接运行
```bash
python data_processor.py
```

### 2. 单独调用
```python
from data_processor import DataProcessor

processor = DataProcessor("data.csv")
processor.load_data()
processor.plot_data_points(save_plot=True, show_plot=True)
```

### 3. 参数控制
```python
# 只保存不显示
processor.plot_data_points(save_plot=True, show_plot=False)

# 只显示不保存
processor.plot_data_points(save_plot=False, show_plot=True)
```

## 数据信息

### 当前数据集
- **文件名**: Bxyz_t_4s_10degrees.csv
- **数据点数**: 72,749
- **数据列**: data1, data2, data3
- **数据类型**: float64

### 数据统计
- **data1**: 范围 [-0.161, 0.973], 均值 0.395
- **data2**: 范围 [-0.228, 1.666], 均值 0.712
- **data3**: 范围 [-0.878, 0.053], 均值 -0.400

## 技术细节

### 1. 坐标系统
- X轴: 数据点索引 (整数)
- Y轴: 数据值 (浮点数)
- 原点: (0, 0)

### 2. 绘图参数
- **线条宽度**: 1
- **透明度**: 0.8
- **颜色**: 红、绿、蓝
- **网格**: 启用，透明度0.3

### 3. 布局设置
- **图形大小**: 12x10 英寸
- **子图排列**: 3行1列
- **标题字体**: 16号，粗体
- **标签字体**: 10号

## 优势

### 1. 直观性
- 直接显示数据点序列
- 无需角度或时间转换
- 便于理解数据采集过程

### 2. 调试友好
- 快速定位问题数据点
- 验证数据处理正确性
- 对比不同分析方法

### 3. 分析价值
- 识别数据趋势
- 检测异常模式
- 评估数据质量

## 注意事项

### 1. 数据量
- 大数据集可能显示密集
- 建议使用缩放功能查看细节
- 可考虑数据采样显示

### 2. 性能
- 大量数据点可能影响渲染速度
- 建议在必要时使用数据采样
- 保存为高分辨率图片

### 3. 对比分析
- 建议与角度域绘图对比
- 结合切片分析结果
- 综合考虑多种视图

## 更新日志

### 2024-10-26
- 新增 `plot_data_points()` 方法
- 添加数据点索引x轴支持
- 集成到主处理流程
- 自动保存到images文件夹

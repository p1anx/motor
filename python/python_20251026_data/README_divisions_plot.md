# 等分结果绘制功能说明

## 功能概述

在原有的切片分析基础上，新增了等分结果绘制功能，将峰峰值等分的结果直接绘制在原图上，提供直观的视觉分析。

## 主要特点

### 1. 视觉元素
- **原始峰峰值曲线**: 保持原有数据曲线，添加透明度
- **等分水平线**: 灰色虚线显示等分边界
- **等分区间散点**: 不同颜色标记不同等分区间
- **等分信息文本**: 显示峰峰值范围和等分大小
- **颜色映射**: 使用viridis颜色映射提供清晰的视觉区分

### 2. 技术实现
```python
def slice_data_and_analyze(self, slice_angle=10.0, n_divisions=10, save_plot=True, show_plot=True):
    # 绘制原始峰峰值曲线
    ax.plot(slice_angles, peak_to_peak_results[col], color=color, 
            linewidth=2, marker='o', markersize=4, label=f'{col} Peak-to-Peak', alpha=0.7)
    
    # 绘制等分水平线
    for i in range(n_divisions + 1):
        level = min_pp + i * division_size
        ax.axhline(y=level, color='gray', linestyle='--', alpha=0.5, linewidth=0.8)
    
    # 为每个等分区间着色
    for i in range(n_divisions):
        division_color = plt.cm.viridis(i / (n_divisions - 1))
        ax.scatter(..., color=division_color, s=30, alpha=0.8)
```

## 视觉设计

### 1. 颜色方案
- **原始曲线**: 红、绿、蓝色，透明度0.7
- **等分水平线**: 灰色虚线，透明度0.5
- **等分区间**: viridis颜色映射，透明度0.8
- **信息文本**: 白色背景框，黑色文字

### 2. 布局设计
- **图形大小**: 14x12 英寸
- **分辨率**: 300 DPI
- **子图排列**: 3行1列
- **标题**: 包含切片角度和等分数信息

### 3. 信息显示
- **等分信息框**: 显示峰峰值范围和等分大小
- **图例**: 显示原始曲线和前3个等分区间
- **网格**: 半透明网格便于读取数值

## 使用示例

### 1. 基本使用
```python
# 默认参数 (10度切片，10等分)
processor.slice_data_and_analyze()

# 自定义参数 (5度切片，20等分)
processor.slice_data_and_analyze(slice_angle=5.0, n_divisions=20)

# 只保存不显示
processor.slice_data_and_analyze(save_plot=True, show_plot=False)
```

### 2. 参数说明
- `slice_angle`: 切片角度 (度)
- `n_divisions`: 等分数
- `save_plot`: 是否保存图像
- `show_plot`: 是否显示图像

## 输出文件

### 1. 文件命名
```
Bxyz_t_4s_10degrees_slice_analysis_with_divisions_{slice_angle}deg_{n_divisions}div.png
```

### 2. 文件特点
- **高分辨率**: 300 DPI
- **清晰布局**: 自动调整布局
- **完整信息**: 包含所有等分信息
- **视觉清晰**: 颜色对比鲜明

## 实际效果

### 1. 等分可视化
- **水平线**: 清晰显示等分边界
- **颜色区分**: 不同等分区间用不同颜色
- **数据点**: 散点图显示具体数据位置
- **信息框**: 显示关键统计信息

### 2. 分析价值
- **直观理解**: 一眼看出数据分布
- **等分验证**: 验证等分算法正确性
- **异常识别**: 快速识别异常数据点
- **质量控制**: 评估数据质量

## 技术细节

### 1. 等分计算
```python
# 计算等分区间
min_pp = min(peak_to_peak_results[col])
max_pp = max(peak_to_peak_results[col])
division_size = (max_pp - min_pp) / n_divisions

# 创建等分边界
for i in range(n_divisions + 1):
    level = min_pp + i * division_size
    ax.axhline(y=level, color='gray', linestyle='--', alpha=0.5, linewidth=0.8)
```

### 2. 颜色映射
```python
# 使用viridis颜色映射
division_color = plt.cm.viridis(i / (n_divisions - 1))

# 为每个等分区间着色
for i in range(n_divisions):
    lower_bound = min_pp + i * division_size
    upper_bound = min_pp + (i + 1) * division_size
    mask = [(lower_bound <= pp <= upper_bound) for pp in peak_to_peak_results[col]]
    if any(mask):
        ax.scatter(..., color=division_color, s=30, alpha=0.8)
```

### 3. 信息显示
```python
# 添加等分信息文本
ax.text(0.02, 0.98, f'Range: [{min_pp:.3f}, {max_pp:.3f}]\nDivision Size: {division_size:.3f}', 
       transform=ax.transAxes, fontsize=9, verticalalignment='top',
       bbox=dict(boxstyle='round', facecolor='white', alpha=0.8))
```

## 应用场景

### 1. 质量控制
- **数据分布检查**: 验证数据分布是否合理
- **异常检测**: 识别异常数据点
- **等分验证**: 验证等分算法正确性

### 2. 数据分析
- **趋势分析**: 分析数据变化趋势
- **模式识别**: 识别数据模式
- **统计验证**: 验证统计分析结果

### 3. 报告展示
- **可视化报告**: 生成可视化分析报告
- **结果展示**: 向他人展示分析结果
- **文档记录**: 记录分析过程和结果

## 优势特点

### 1. 直观性
- **一目了然**: 等分结果直观显示
- **颜色区分**: 不同等分区间清晰区分
- **信息完整**: 包含所有关键信息

### 2. 实用性
- **参数可调**: 支持自定义参数
- **高质量输出**: 高分辨率图像
- **易于理解**: 清晰的视觉设计

### 3. 扩展性
- **灵活配置**: 支持不同等分数
- **易于修改**: 代码结构清晰
- **功能完整**: 集成到主处理流程

## 更新日志

### 2024-10-26
- 新增等分结果绘制功能
- 修改 `slice_data_and_analyze()` 方法签名
- 添加等分水平线绘制
- 实现等分区间颜色标记
- 添加等分信息文本显示
- 更新文件命名规则
- 集成到主处理流程

## 未来扩展

### 1. 功能扩展
- 支持更多颜色映射
- 添加交互式功能
- 支持自定义样式

### 2. 性能优化
- 优化绘制性能
- 支持大数据集
- 添加缓存机制

### 3. 应用扩展
- 支持实时分析
- 添加动画效果
- 集成到Web界面

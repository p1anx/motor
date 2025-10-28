# PyQt入门示例

这是一个简单的PyQt5应用示例，适合新手学习PyQt的基本概念和用法。

## 功能特性

- 简单的用户界面
- 文本输入和显示
- 按钮交互
- 消息对话框
- 退出确认

## 安装依赖

```bash
pip install -r requirements.txt
```

或者直接安装PyQt5：

```bash
pip install PyQt5
```

## 运行应用

```bash
python simple_pyqt_app.py
```

## 代码说明

### 主要组件

1. **QMainWindow**: 主窗口类
2. **QWidget**: 基础控件类
3. **QVBoxLayout/QHBoxLayout**: 垂直/水平布局
4. **QLabel**: 标签控件
5. **QPushButton**: 按钮控件
6. **QLineEdit**: 单行输入框
7. **QTextEdit**: 多行文本显示
8. **QMessageBox**: 消息对话框

### 关键概念

- **布局管理**: 使用QVBoxLayout和QHBoxLayout来组织界面元素
- **信号与槽**: 使用clicked.connect()连接按钮点击事件
- **事件处理**: 重写closeEvent()方法处理窗口关闭事件
- **样式设置**: 使用QFont设置字体样式

## 学习建议

1. 先运行程序，体验界面交互
2. 阅读代码，理解每个组件的作用
3. 尝试修改界面元素（如按钮文字、窗口大小等）
4. 添加新的功能（如新的按钮、新的输入框等）
5. 学习更多PyQt组件和布局方式

## 扩展练习

- 添加菜单栏
- 添加工具栏
- 实现文件打开/保存功能
- 添加更多输入控件（复选框、下拉框等）
- 实现数据表格显示

#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
TCP/UART统一通信应用
支持TCP客户端/服务端和UART串口通信
TCP和UART数据显示独立且并列显示
"""

import sys
import socket
import threading
import time
import serial
import serial.tools.list_ports
import numpy as np
from datetime import datetime
from PyQt5.QtWidgets import (QApplication, QMainWindow, QWidget, QVBoxLayout, 
                             QHBoxLayout, QLabel, QPushButton, QLineEdit, 
                             QTextEdit, QComboBox, QGroupBox, QGridLayout,
                             QMessageBox, QSpinBox, QCheckBox, QSplitter,
                             QStackedWidget, QFrame, QTabWidget)
from PyQt5.QtCore import Qt, QThread, pyqtSignal, QTimer
from PyQt5.QtGui import QFont, QTextCursor
import matplotlib.pyplot as plt
from matplotlib.backends.backend_qt5agg import FigureCanvasQTAgg as FigureCanvas
from matplotlib.backends.backend_qt5agg import NavigationToolbar2QT as NavigationToolbar
from matplotlib.figure import Figure


class RealTimePlotWidget(QWidget):
    """实时绘图组件"""
    
    def __init__(self, title="实时数据绘图"):
        super().__init__()
        self.title = title
        self.data_buffer = []
        self.max_points = 1000  # 默认显示点数
        self.update_timer = None  # 更新定时器
        self.update_interval = 100  # 更新间隔（毫秒）
        self.pending_update = False  # 是否有待更新的数据
        self.init_ui()
        
    def init_ui(self):
        """初始化绘图界面"""
        layout = QVBoxLayout()
        self.setLayout(layout)
        
        # 创建matplotlib图形
        self.figure = Figure(figsize=(8, 4))
        self.canvas = FigureCanvas(self.figure)
        self.ax = self.figure.add_subplot(111)
        
        # 设置图形属性（放大字体，移除标题和轴标签）
        # self.ax.set_title(self.title, fontsize=24)  # 移除标题
        # self.ax.set_xlabel("x", fontsize=20)  # 移除X轴标签
        # self.ax.set_ylabel("y", fontsize=20)  # 移除Y轴标签
        self.ax.tick_params(axis='both', labelsize=16, direction='in')  # 刻度标签字体大小，刻度朝内
        self.ax.grid(True)
        
        # 调整布局以避免标签被裁剪
        self.figure.tight_layout()
        
        # 添加导航工具栏（提供缩放、平移等功能）
        self.toolbar = NavigationToolbar(self.canvas, self)
        layout.addWidget(self.toolbar)
        
        layout.addWidget(self.canvas)
        
        # 初始化线条
        self.lines = []
        self.colors = ['b-', 'r-', 'g-', 'm-', 'c-', 'y-']
        
        # 连接鼠标滚轮事件
        self.canvas.mpl_connect('scroll_event', self.on_scroll)
        
        # 创建定时器用于定期更新图表
        self.update_timer = QTimer()
        self.update_timer.timeout.connect(self._scheduled_update)
        self.update_timer.start(self.update_interval)
        
        # 绘图暂停标志
        self.is_paused = False
        
        # 通道显示控制
        self.channel_visible = {}  # 记录每个通道是否可见
        
    def add_data(self, data_string):
        """添加数据点"""
        # 如果暂停，不添加新数据
        if self.is_paused:
            return
            
        try:
            # 解析数据格式 "1.0, 2.2, 3.3\n"
            # 清理数据字符串
            clean_data = data_string.strip()
            if not clean_data:
                return
            
            # 分割并解析数据
            data_values = []
            for x in clean_data.split(','):
                try:
                    value = float(x.strip())
                    data_values.append(value)
                except ValueError:
                    # 忽略无法解析的单个值
                    continue
            
            if len(data_values) > 0:
                self.data_buffer.append(data_values)
                
                # 保持缓冲区大小
                if len(self.data_buffer) > self.max_points:
                    self.data_buffer.pop(0)
                
                # 标记有待更新的数据，但不立即更新
                self.pending_update = True
        except Exception as e:
            # 出错时打印错误但不中断程序
            print(f"数据解析错误: {e}, 数据: {data_string}")
    
    def _scheduled_update(self):
        """定时更新图表"""
        if self.pending_update:
            self.update_plot()
            self.pending_update = False
    
    def update_plot(self):
        """更新绘图"""
        if not self.data_buffer:
            return
        
        try:
            # 清空当前图形
            self.ax.clear()
            # self.ax.set_title(self.title, fontsize=24)  # 移除标题
            # self.ax.set_xlabel("x", fontsize=20)  # 移除X轴标签
            # self.ax.set_ylabel("y", fontsize=20)  # 移除Y轴标签
            self.ax.tick_params(axis='both', labelsize=16, direction='in')  # 刻度标签字体大小，刻度朝内
            self.ax.grid(True)
            
            # 获取最大通道数
            max_channels = max(len(data) for data in self.data_buffer)
            x_data = list(range(len(self.data_buffer)))
            
            # 绘制每个通道的数据
            for i in range(max_channels):
                # 检查该通道是否应该显示
                if i not in self.channel_visible:
                    self.channel_visible[i] = True  # 默认显示
                
                if not self.channel_visible.get(i, True):
                    continue  # 跳过不显示的通道
                
                y_data = []
                for data in self.data_buffer:
                    if i < len(data):
                        y_data.append(data[i])
                    else:
                        # 如果该行数据没有这个通道，使用None（断开线条）
                        y_data.append(None)
                
                # 只绘制有数据的通道
                if any(v is not None for v in y_data):
                    color = self.colors[i % len(self.colors)]
                    self.ax.plot(x_data, y_data, color, label=f'data{i+1}', linewidth=3)  # 增加线条宽度
            
            # 添加图例（固定在右上角，增大字体）
            if max_channels > 1:
                self.ax.legend(loc='upper right', fontsize=16)  # 图例字体大小
            
            # 调整布局以避免标签被裁剪
            self.figure.tight_layout()
            
            # 刷新画布
            self.canvas.draw()
        except Exception as e:
            # 出错时打印错误但不中断程序
            print(f"绘图错误: {e}")
    
    def set_max_points(self, max_points):
        """设置最大显示点数"""
        self.max_points = max_points
        # 如果当前数据超过新的最大值，截取最新的数据
        if len(self.data_buffer) > self.max_points:
            self.data_buffer = self.data_buffer[-self.max_points:]
            self.pending_update = True
    
    def pause(self):
        """暂停绘图"""
        self.is_paused = True
    
    def resume(self):
        """恢复绘图"""
        self.is_paused = False
    
    def toggle_pause(self):
        """切换暂停状态"""
        self.is_paused = not self.is_paused
        return self.is_paused
    
    def set_channel_visible(self, channel_index, visible):
        """设置通道是否可见"""
        self.channel_visible[channel_index] = visible
        self.pending_update = True  # 标记需要更新
    
    def get_max_channels(self):
        """获取最大通道数"""
        if not self.data_buffer:
            return 0
        return max(len(data) for data in self.data_buffer)
    
    def save_to_csv(self, filename):
        """保存数据到CSV文件"""
        import csv
        try:
            with open(filename, 'w', newline='', encoding='utf-8') as f:
                writer = csv.writer(f)
                # 写入表头
                if self.data_buffer:
                    num_channels = max(len(data) for data in self.data_buffer)
                    header = [f'data{i+1}' for i in range(num_channels)]
                    writer.writerow(header)
                    
                    # 写入数据
                    for data in self.data_buffer:
                        # 补齐缺失的通道数据
                        row = data + [''] * (num_channels - len(data))
                        writer.writerow(row)
            return True
        except Exception as e:
            print(f"保存CSV文件错误: {e}")
            return False
    
    def on_scroll(self, event):
        """鼠标滚轮缩放"""
        if event.inaxes != self.ax:
            return
        
        # 获取当前坐标轴范围
        cur_xlim = self.ax.get_xlim()
        cur_ylim = self.ax.get_ylim()
        
        # 获取鼠标在数据坐标中的位置
        xdata = event.xdata
        ydata = event.ydata
        
        # 设置缩放比例
        scale_factor = 1.2
        
        if event.button == 'up':
            # 向上滚动，放大
            scale = 1 / scale_factor
        elif event.button == 'down':
            # 向下滚动，缩小
            scale = scale_factor
        else:
            return
        
        # 计算新的坐标轴范围（以鼠标位置为中心缩放）
        new_xlim = [xdata - (xdata - cur_xlim[0]) * scale,
                    xdata + (cur_xlim[1] - xdata) * scale]
        new_ylim = [ydata - (ydata - cur_ylim[0]) * scale,
                    ydata + (cur_ylim[1] - ydata) * scale]
        
        # 应用新的坐标轴范围
        self.ax.set_xlim(new_xlim)
        self.ax.set_ylim(new_ylim)
        
        # 刷新画布
        self.canvas.draw_idle()
    
    def clear_data(self):
        """清空数据"""
        self.data_buffer.clear()
        self.pending_update = False
        self.ax.clear()
        # self.ax.set_title(self.title, fontsize=24)  # 移除标题
        # self.ax.set_xlabel("x", fontsize=20)  # 移除X轴标签
        # self.ax.set_ylabel("y", fontsize=20)  # 移除Y轴标签
        self.ax.tick_params(axis='both', labelsize=16, direction='in')  # 刻度标签字体大小，刻度朝内
        self.ax.grid(True)
        
        # 调整布局以避免标签被裁剪
        self.figure.tight_layout()
        
        self.canvas.draw()


class CommunicationPage(QWidget):
    """合并的TCP/UART通信页面"""
    
    def __init__(self, parent_app):
        super().__init__()
        self.parent_app = parent_app
        self.init_ui()
        
    def init_ui(self):
        """初始化通信页面界面"""
        layout = QVBoxLayout()
        self.setLayout(layout)
        
        # 页面标题
        # title = QLabel("TCP/UART通信")
        # title.setStyleSheet("""
        #     font-size: 32px; 
        #     font-weight: bold; 
        #     color: #2c3e50; 
        #     padding: 20px;
        #     background-color: #ecf0f1;
        #     border-radius: 10px;
        #     margin: 10px;
        # """)
        # title.setAlignment(Qt.AlignCenter)
        # title.setFixedHeight(80)
        # layout.addWidget(title)
        
        # 移除返回按钮，因为使用TabWidget不需要
        
        # 创建水平分割器
        splitter = QSplitter(Qt.Horizontal)
        layout.addWidget(splitter)
        
        # 左侧：TCP通信区域
        tcp_widget = QWidget()
        tcp_widget.setMinimumWidth(800)
        self.parent_app.init_tcp_section(tcp_widget)
        splitter.addWidget(tcp_widget)
        
        # 右侧：UART通信区域
        uart_widget = QWidget()
        uart_widget.setMinimumWidth(800)
        self.parent_app.init_uart_section(uart_widget)
        splitter.addWidget(uart_widget)
        
        # 设置分割器比例
        splitter.setSizes([600, 600])
    
    # 移除go_back方法，因为使用TabWidget不需要


class SyncControlPage(QWidget):
    """独立的同步控制页面"""
    
    def __init__(self, parent_app):
        super().__init__()
        self.parent_app = parent_app
        self.init_ui()
        
    def init_ui(self):
        """初始化同步控制页面界面"""
        layout = QVBoxLayout()
        self.setLayout(layout)
        
        # 页面标题
        # title = QLabel("同步控制")
        # title.setStyleSheet("""
        #     font-size: 32px; 
        #     font-weight: bold; 
        #     color: #2c3e50; 
        #     padding: 20px;
        #     background-color: #ecf0f1;
        #     border-radius: 10px;
        #     margin: 10px;
        # """)
        # title.setAlignment(Qt.AlignCenter)
        # title.setFixedHeight(80)
        # layout.addWidget(title)
        
        # 移除返回按钮，因为使用TabWidget不需要
        
        # 同步控制区域
        sync_group = QGroupBox("同步控制")
        sync_group.setStyleSheet("""
            QGroupBox {
                font-weight: bold;
                font-size: 18px;
                color: #2c3e50;
                border: 2px solid #3498db;
                border-radius: 10px;
                margin: 10px;
                padding-top: 20px;
            }
            QGroupBox::title {
                subcontrol-origin: margin;
                left: 20px;
                padding: 0 10px 0 10px;
            }
        """)
        sync_layout = QVBoxLayout()
        
        # 同步模式开关
        sync_mode_layout = QHBoxLayout()
        self.sync_mode_check = QCheckBox("启用同步模式")
        self.sync_mode_check.setStyleSheet("font-size: 16px; font-weight: bold; color: #e74c3c;")
        self.sync_mode_check.setChecked(False)  # 默认关闭，让用户手动决定
        self.sync_mode_check.toggled.connect(self.on_sync_mode_toggled)
        sync_mode_layout.addWidget(self.sync_mode_check)
        sync_mode_layout.addStretch()
        sync_layout.addLayout(sync_mode_layout)
        
        # 自动同步开关
        sync_enable_layout = QHBoxLayout()
        self.auto_sync_check = QCheckBox("连接时自动同步")
        self.auto_sync_check.setStyleSheet("font-size: 16px; font-weight: bold; color: #27ae60;")
        self.auto_sync_check.setChecked(True)  # 默认开启
        self.auto_sync_check.toggled.connect(self.on_auto_sync_toggled)
        self.auto_sync_check.setEnabled(False)  # 默认禁用，需要先启用同步模式
        sync_enable_layout.addWidget(self.auto_sync_check)
        sync_enable_layout.addStretch()
        sync_layout.addLayout(sync_enable_layout)
        
        # 同步字符串配置
        sync_strings_layout = QHBoxLayout()
        
        # TCP同步字符串
        tcp_sync_layout = QVBoxLayout()
        tcp_sync_layout.addWidget(QLabel("TCP同步字符串:"))
        
        # TCP发送同步配置
        tcp_send_layout = QHBoxLayout()
        self.tcp_send_enable_check = QCheckBox("启用发送")
        self.tcp_send_enable_check.setStyleSheet("font-size: 14px; font-weight: bold; color: #e74c3c;")
        self.tcp_send_enable_check.setChecked(False)  # 默认不勾选
        self.tcp_send_enable_check.toggled.connect(self.on_tcp_send_enable_toggled)
        tcp_send_layout.addWidget(self.tcp_send_enable_check)
        tcp_send_layout.addStretch()
        
        self.tcp_sync_send_input = QLineEdit()
        self.tcp_sync_send_input.setText("SYNC_SEND")
        self.tcp_sync_send_input.setPlaceholderText("发送的同步字符串")
        self.tcp_sync_send_input.setEnabled(False)  # 默认禁用
        self.tcp_sync_send_input.setStyleSheet("background-color: #ecf0f1; color: #95a5a6;")  # 禁用样式
        
        tcp_sync_layout.addLayout(tcp_send_layout)
        tcp_sync_layout.addWidget(self.tcp_sync_send_input)
        
        # TCP接收同步配置
        tcp_recv_layout = QHBoxLayout()
        self.tcp_recv_enable_check = QCheckBox("启用接收")
        self.tcp_recv_enable_check.setStyleSheet("font-size: 14px; font-weight: bold; color: #27ae60;")
        self.tcp_recv_enable_check.setChecked(False)  # 默认不勾选
        self.tcp_recv_enable_check.toggled.connect(self.on_tcp_recv_enable_toggled)
        tcp_recv_layout.addWidget(self.tcp_recv_enable_check)
        tcp_recv_layout.addStretch()
        
        self.tcp_sync_receive_input = QLineEdit()
        self.tcp_sync_receive_input.setText("SYNC_RECV")
        self.tcp_sync_receive_input.setPlaceholderText("接收的同步字符串")
        self.tcp_sync_receive_input.setEnabled(False)  # 默认禁用
        self.tcp_sync_receive_input.setStyleSheet("background-color: #ecf0f1; color: #95a5a6;")  # 禁用样式
        
        tcp_sync_layout.addLayout(tcp_recv_layout)
        tcp_sync_layout.addWidget(self.tcp_sync_receive_input)
        
        # UART同步字符串
        uart_sync_layout = QVBoxLayout()
        uart_sync_layout.addWidget(QLabel("UART同步字符串:"))
        
        # UART发送同步配置
        uart_send_layout = QHBoxLayout()
        self.uart_send_enable_check = QCheckBox("启用发送")
        self.uart_send_enable_check.setStyleSheet("font-size: 14px; font-weight: bold; color: #e74c3c;")
        self.uart_send_enable_check.setChecked(False)  # 默认不勾选
        self.uart_send_enable_check.toggled.connect(self.on_uart_send_enable_toggled)
        uart_send_layout.addWidget(self.uart_send_enable_check)
        uart_send_layout.addStretch()
        
        self.uart_sync_send_input = QLineEdit()
        self.uart_sync_send_input.setText("SYNC_SEND")
        self.uart_sync_send_input.setPlaceholderText("发送的同步字符串")
        self.uart_sync_send_input.setEnabled(False)  # 默认禁用
        self.uart_sync_send_input.setStyleSheet("background-color: #ecf0f1; color: #95a5a6;")  # 禁用样式
        
        uart_sync_layout.addLayout(uart_send_layout)
        uart_sync_layout.addWidget(self.uart_sync_send_input)
        
        # UART接收同步配置
        uart_recv_layout = QHBoxLayout()
        self.uart_recv_enable_check = QCheckBox("启用接收")
        self.uart_recv_enable_check.setStyleSheet("font-size: 14px; font-weight: bold; color: #27ae60;")
        self.uart_recv_enable_check.setChecked(False)  # 默认不勾选
        self.uart_recv_enable_check.toggled.connect(self.on_uart_recv_enable_toggled)
        uart_recv_layout.addWidget(self.uart_recv_enable_check)
        uart_recv_layout.addStretch()
        
        self.uart_sync_receive_input = QLineEdit()
        self.uart_sync_receive_input.setText("SYNC_RECV")
        self.uart_sync_receive_input.setPlaceholderText("接收的同步字符串")
        self.uart_sync_receive_input.setEnabled(False)  # 默认禁用
        self.uart_sync_receive_input.setStyleSheet("background-color: #ecf0f1; color: #95a5a6;")  # 禁用样式
        
        uart_sync_layout.addLayout(uart_recv_layout)
        uart_sync_layout.addWidget(self.uart_sync_receive_input)
        
        sync_strings_layout.addLayout(tcp_sync_layout)
        sync_strings_layout.addLayout(uart_sync_layout)
        sync_layout.addLayout(sync_strings_layout)
        
        # 同步指令设置
        sync_command_group = QGroupBox("同步指令设置")
        sync_command_group.setStyleSheet("""
            QGroupBox {
                font-weight: bold;
                font-size: 16px;
                color: #2c3e50;
                border: 2px solid #9b59b6;
                border-radius: 10px;
                margin: 10px;
                padding-top: 20px;
            }
            QGroupBox::title {
                subcontrol-origin: margin;
                left: 20px;
                padding: 0 10px 0 10px;
            }
        """)
        sync_command_layout = QVBoxLayout()
        
        # 同步指令输入
        sync_command_input_layout = QHBoxLayout()
        sync_command_input_layout.addWidget(QLabel("同步指令:"))
        self.sync_command_input = QLineEdit()
        self.sync_command_input.setText("SYNC_COMMAND")
        self.sync_command_input.setPlaceholderText("输入要同时发送给TCP和UART的同步指令")
        sync_command_input_layout.addWidget(self.sync_command_input)
        
        # 发送同步指令按钮
        self.send_sync_command_button = QPushButton("发送同步指令")
        self.send_sync_command_button.setStyleSheet("""
            QPushButton {
                background-color: #9b59b6;
                color: white;
                border: none;
                padding: 10px 20px;
                font-size: 14px;
                font-weight: bold;
                border-radius: 5px;
            }
            QPushButton:hover {
                background-color: #8e44ad;
            }
            QPushButton:disabled {
                background-color: #bdc3c7;
            }
        """)
        self.send_sync_command_button.clicked.connect(self.send_sync_command)
        self.send_sync_command_button.setEnabled(False)  # 默认禁用，需要连接后才能发送
        sync_command_input_layout.addWidget(self.send_sync_command_button)
        
        sync_command_layout.addLayout(sync_command_input_layout)
        sync_command_group.setLayout(sync_command_layout)
        sync_layout.addWidget(sync_command_group)
        
        # 同步控制按钮
        sync_control_layout = QHBoxLayout()
        
        # 同步按钮
        self.sync_button = QPushButton("开始同步")
        self.sync_button.setStyleSheet("""
            QPushButton {
                background-color: #3498db;
                color: white;
                border: none;
                padding: 15px 30px;
                font-size: 16px;
                font-weight: bold;
                border-radius: 8px;
            }
            QPushButton:hover {
                background-color: #2980b9;
            }
            QPushButton:disabled {
                background-color: #bdc3c7;
            }
        """)
        self.sync_button.clicked.connect(self.start_sync)
        self.sync_button.setEnabled(False)  # 默认禁用，需要先启用同步模式
        
        # 重置同步按钮
        self.reset_sync_button = QPushButton("重置同步")
        self.reset_sync_button.setStyleSheet("""
            QPushButton {
                background-color: #e74c3c;
                color: white;
                border: none;
                padding: 15px 30px;
                font-size: 16px;
                font-weight: bold;
                border-radius: 8px;
            }
            QPushButton:hover {
                background-color: #c0392b;
            }
        """)
        self.reset_sync_button.clicked.connect(self.reset_sync)
        
        sync_control_layout.addWidget(self.sync_button)
        sync_control_layout.addWidget(self.reset_sync_button)
        sync_control_layout.addStretch()
        
        # 同步状态显示
        self.sync_status_label = QLabel("同步未启用")
        self.sync_status_label.setStyleSheet("color: #7f8c8d; font-weight: bold; font-size: 16px;")
        sync_control_layout.addWidget(self.sync_status_label)
        
        sync_layout.addLayout(sync_control_layout)
        sync_group.setLayout(sync_layout)
        layout.addWidget(sync_group)
        
        # 使用说明
        help_group = QGroupBox("使用说明")
        help_group.setStyleSheet("""
            QGroupBox {
                font-weight: bold;
                font-size: 16px;
                color: #2c3e50;
                border: 2px solid #27ae60;
                border-radius: 10px;
                margin: 10px;
                padding-top: 20px;
            }
            QGroupBox::title {
                subcontrol-origin: margin;
                left: 20px;
                padding: 0 10px 0 10px;
            }
        """)
        help_layout = QVBoxLayout()
        
        help_text = QLabel("""
        <h3>同步功能使用步骤：</h3>
        <ol>
        <li><b>启用同步模式</b>：勾选"启用同步模式"复选框（必须步骤）</li>
        <li><b>配置同步信息</b>：设置TCP和UART的发送/接收同步字符串</li>
        <li><b>启用同步选项</b>：勾选需要使用的同步选项（发送/接收）</li>
        <li><b>设置自动同步</b>：可选择是否在连接时自动开始同步</li>
        <li><b>建立连接</b>：返回主界面连接TCP和/或UART</li>
        <li><b>开始同步</b>：手动点击"开始同步"或自动开始同步过程</li>
        <li><b>等待确认</b>：系统会发送同步字符串并等待确认</li>
        <li><b>同步完成</b>：收到所有确认后开始正常通信</li>
        </ol>
        
        <h3>同步模式控制：</h3>
        <ul>
        <li><b>同步模式开关</b>：可以手动启用/禁用整个同步功能</li>
        <li><b>默认关闭</b>：应用启动时同步模式默认关闭，需要手动启用</li>
        <li><b>完全控制</b>：用户完全控制是否使用同步功能</li>
        <li><b>状态显示</b>：清楚显示当前同步模式状态</li>
        </ul>
        
        <h3>同步指令功能：</h3>
        <ul>
        <li><b>同步指令</b>：可以设置一个指令同时发送给TCP和UART</li>
        <li><b>手动发送</b>：点击"发送同步指令"按钮手动发送同步指令</li>
        <li><b>同时发送</b>：指令会同时发送给所有已连接的通道</li>
        <li><b>状态反馈</b>：显示发送结果和成功/失败状态</li>
        </ul>
        
        <h3>重要说明：</h3>
        <ul>
        <li><b>手动控制</b>：同步模式默认关闭，需要手动启用</li>
        <li><b>灵活配置</b>：可以随时启用/禁用同步模式</li>
        <li><b>自动同步</b>：启用同步模式后，可选择是否自动同步</li>
        <li><b>完全自主</b>：用户完全控制同步功能的使用</li>
        </ul>
        
        <h3>同步控制逻辑：</h3>
        <ul>
        <li><b>同步前</b>：无法发送数据，只显示同步相关数据</li>
        <li><b>同步中</b>：发送同步字符串，等待确认</li>
        <li><b>同步后</b>：正常收发数据，所有功能可用</li>
        <li><b>无限等待</b>：同步将无限等待确认，不会超时</li>
        </ul>
        """)
        help_text.setStyleSheet("font-size: 14px; line-height: 1.5;")
        help_text.setWordWrap(True)
        help_layout.addWidget(help_text)
        
        help_group.setLayout(help_layout)
        layout.addWidget(help_group)
        
        # 添加弹性空间
        layout.addStretch()
    
    # 移除go_back方法，因为使用TabWidget不需要
    
    def on_sync_enable_toggled(self, checked):
        """同步模式开关处理"""
        self.parent_app.sync_enabled = checked
        if checked:
            self.sync_button.setEnabled(True)
            self.sync_status_label.setText("同步模式已启用")
            self.sync_status_label.setStyleSheet("color: #f39c12; font-weight: bold; font-size: 16px;")
        else:
            self.sync_button.setEnabled(False)
            self.sync_status_label.setText("同步未启用")
            self.sync_status_label.setStyleSheet("color: #7f8c8d; font-weight: bold; font-size: 16px;")
            self.parent_app.reset_sync()
    
    def start_sync(self):
        """开始同步过程"""
        self.parent_app.start_sync_direct()
    
    def reset_sync(self):
        """重置同步状态"""
        self.parent_app.reset_sync()
    
    def update_sync_status(self, status, color="#7f8c8d"):
        """更新同步状态显示"""
        self.sync_status_label.setText(status)
        self.sync_status_label.setStyleSheet(f"color: {color}; font-weight: bold; font-size: 16px;")
    
    def update_sync_button(self, text, enabled=True):
        """更新同步按钮"""
        self.sync_button.setText(text)
        self.sync_button.setEnabled(enabled)
    
    def on_tcp_send_enable_toggled(self, checked):
        """TCP发送同步开关处理"""
        self.tcp_sync_send_input.setEnabled(checked)
        if not checked:
            self.tcp_sync_send_input.setStyleSheet("background-color: #ecf0f1; color: #95a5a6;")
        else:
            self.tcp_sync_send_input.setStyleSheet("")
    
    def on_tcp_recv_enable_toggled(self, checked):
        """TCP接收同步开关处理"""
        self.tcp_sync_receive_input.setEnabled(checked)
        if not checked:
            self.tcp_sync_receive_input.setStyleSheet("background-color: #ecf0f1; color: #95a5a6;")
        else:
            self.tcp_sync_receive_input.setStyleSheet("")
    
    def on_uart_send_enable_toggled(self, checked):
        """UART发送同步开关处理"""
        self.uart_sync_send_input.setEnabled(checked)
        if not checked:
            self.uart_sync_send_input.setStyleSheet("background-color: #ecf0f1; color: #95a5a6;")
        else:
            self.uart_sync_send_input.setStyleSheet("")
    
    def on_uart_recv_enable_toggled(self, checked):
        """UART接收同步开关处理"""
        self.uart_sync_receive_input.setEnabled(checked)
        if not checked:
            self.uart_sync_receive_input.setStyleSheet("background-color: #ecf0f1; color: #95a5a6;")
        else:
            self.uart_sync_receive_input.setStyleSheet("")
    
    def on_sync_mode_toggled(self, checked):
        """同步模式开关处理"""
        # 更新主应用的同步模式状态
        self.parent_app.sync_enabled = checked
        
        # 根据同步模式状态启用/禁用其他控件
        self.auto_sync_check.setEnabled(checked)
        self.sync_button.setEnabled(checked)
        
        if checked:
            self.sync_status_label.setText("同步模式已启用")
            self.sync_status_label.setStyleSheet("color: #e74c3c; font-weight: bold; font-size: 16px;")
        else:
            self.sync_status_label.setText("同步模式已禁用")
            self.sync_status_label.setStyleSheet("color: #7f8c8d; font-weight: bold; font-size: 16px;")
            # 禁用同步模式时重置同步状态
            self.parent_app.reset_sync()
    
    def on_auto_sync_toggled(self, checked):
        """自动同步开关处理"""
        # 更新主应用的自动同步状态
        if hasattr(self.parent_app, 'auto_sync_enabled'):
            self.parent_app.auto_sync_enabled = checked
        else:
            self.parent_app.auto_sync_enabled = checked
    
    def send_sync_command(self):
        """发送同步指令给TCP和UART"""
        self.parent_app.send_sync_command()


class UARTThread(QThread):
    """UART串口通信线程"""
    data_received = pyqtSignal(str)
    status_changed = pyqtSignal(str)
    connection_changed = pyqtSignal(bool)
    
    def __init__(self):
        super().__init__()
        self.serial_port = None
        self.running = False
        
    def connect_uart(self, port, baudrate, timeout=1):
        """连接串口"""
        try:
            self.serial_port = serial.Serial(port, baudrate, timeout=timeout)
            self.running = True
            self.status_changed.emit(f"串口已连接: {port} @ {baudrate}")
            self.connection_changed.emit(True)
            self.start()
        except Exception as e:
            self.status_changed.emit(f"串口连接失败: {str(e)}")
            self.connection_changed.emit(False)
    
    def send_data(self, data):
        """发送数据"""
        try:
            if self.serial_port and self.serial_port.is_open:
                self.serial_port.write(data.encode('utf-8'))
                return True
        except Exception as e:
            self.status_changed.emit(f"串口发送失败: {str(e)}")
        return False
    
    def run(self):
        """线程运行函数"""
        while self.running and self.serial_port and self.serial_port.is_open:
            try:
                data = self.serial_port.readline()
                if data:
                    timestamp = datetime.now().strftime("%H:%M:%S")
                    self.data_received.emit(f"[{timestamp}] {data.decode('utf-8').strip()}")
            except Exception as e:
                if self.running:
                    self.status_changed.emit(f"串口接收错误: {str(e)}")
                break
        
        self._cleanup()
    
    def _cleanup(self):
        """清理资源"""
        self.running = False
        if self.serial_port:
            self.serial_port.close()
            self.serial_port = None
        self.connection_changed.emit(False)
        self.status_changed.emit("串口已断开")
    
    def stop(self):
        """停止线程"""
        self.running = False
        if self.serial_port:
            self.serial_port.close()
        self.quit()
        self.wait()


class TCPThread(QThread):
    """TCP通信线程"""
    data_received = pyqtSignal(str)
    status_changed = pyqtSignal(str)
    connection_changed = pyqtSignal(bool)
    
    def __init__(self, mode='client'):
        super().__init__()
        self.mode = mode
        self.socket = None
        self.client_socket = None
        self.running = False
        self.server_running = False
        self.buffer = ""  # 数据缓冲区，用于处理不完整的数据包
        
    def set_mode(self, mode):
        """设置模式：client 或 server"""
        self.mode = mode
        
    def start_client(self, host, port):
        """启动客户端"""
        try:
            self.socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
            self.socket.settimeout(1.0)  # 设置1秒超时
            self.socket.connect((host, int(port)))
            self.running = True
            self.status_changed.emit(f"已连接到 {host}:{port}")
            self.connection_changed.emit(True)
            self.start()
        except Exception as e:
            self.status_changed.emit(f"连接失败: {str(e)}")
            self.connection_changed.emit(False)
    
    def start_server(self, port):
        """启动服务端"""
        try:
            self.socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
            self.socket.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
            self.socket.settimeout(1.0)  # 设置1秒超时
            self.socket.bind(('', int(port)))
            self.socket.listen(1)
            self.server_running = True
            self.status_changed.emit(f"服务端启动，监听端口 {port}")
            self.start()
        except Exception as e:
            self.status_changed.emit(f"服务端启动失败: {str(e)}")
    
    def send_data(self, data):
        """发送数据"""
        try:
            if self.mode == 'client' and self.socket:
                self.socket.send(data.encode('utf-8'))
                return True
            elif self.mode == 'server' and self.client_socket:
                self.client_socket.send(data.encode('utf-8'))
                return True
        except Exception as e:
            self.status_changed.emit(f"发送失败: {str(e)}")
        return False
    
    def run(self):
        """线程运行函数"""
        if self.mode == 'client':
            self._client_loop()
        else:
            self._server_loop()
    
    def _client_loop(self):
        """客户端循环"""
        while self.running:
            try:
                data = self.socket.recv(1024)
                if data:
                    # 将接收的数据添加到缓冲区
                    self.buffer += data.decode('utf-8')
                    
                    # 按行分割并处理
                    while '\n' in self.buffer:
                        line, self.buffer = self.buffer.split('\n', 1)
                        if line.strip():  # 忽略空行
                            timestamp = datetime.now().strftime("%H:%M:%S")
                            self.data_received.emit(f"[{timestamp}] {line.strip()}")
                else:
                    break
            except socket.timeout:
                # 超时是正常的，继续循环
                continue
            except Exception as e:
                if self.running:
                    self.status_changed.emit(f"接收数据错误: {str(e)}")
                break
        
        self._cleanup()
    
    def _server_loop(self):
        """服务端循环"""
        while self.server_running:
            try:
                self.status_changed.emit("等待客户端连接...")
                self.client_socket, addr = self.socket.accept()
                self.client_socket.settimeout(1.0)  # 为客户端socket设置超时
                self.status_changed.emit(f"客户端已连接: {addr[0]}:{addr[1]}")
                self.connection_changed.emit(True)
                
                # 清空缓冲区
                self.buffer = ""
                
                # 处理客户端数据
                while self.server_running:
                    try:
                        data = self.client_socket.recv(1024)
                        if data:
                            # 将接收的数据添加到缓冲区
                            self.buffer += data.decode('utf-8')
                            
                            # 按行分割并处理
                            while '\n' in self.buffer:
                                line, self.buffer = self.buffer.split('\n', 1)
                                if line.strip():  # 忽略空行
                                    timestamp = datetime.now().strftime("%H:%M:%S")
                                    self.data_received.emit(f"[{timestamp}] {line.strip()}")
                        else:
                            break
                    except socket.timeout:
                        # 超时是正常的，继续循环
                        continue
                    except Exception as e:
                        if self.server_running:
                            self.status_changed.emit(f"接收数据错误: {str(e)}")
                        break
                        
            except socket.timeout:
                # accept超时是正常的，继续循环
                continue
            except Exception as e:
                if self.server_running:
                    self.status_changed.emit(f"服务端错误: {str(e)}")
                break
        
        self._cleanup()
    
    def _cleanup(self):
        """清理资源"""
        self.running = False
        self.server_running = False
        if self.client_socket:
            self.client_socket.close()
            self.client_socket = None
        if self.socket:
            self.socket.close()
            self.socket = None
        self.connection_changed.emit(False)
        self.status_changed.emit("连接已断开")
    
    def stop(self):
        """停止线程"""
        self.running = False
        self.server_running = False
        
        # 先关闭socket以中断阻塞的操作
        try:
            if self.client_socket:
                self.client_socket.shutdown(socket.SHUT_RDWR)
                self.client_socket.close()
                self.client_socket = None
        except:
            pass
        
        try:
            if self.socket:
                self.socket.shutdown(socket.SHUT_RDWR)
                self.socket.close()
                self.socket = None
        except:
            pass
        
        # 等待线程结束，最多等待2秒
        self.quit()
        self.wait(2000)


class TCPUARTUnifiedApp(QMainWindow):
    """TCP/UART统一通信应用主窗口"""
    
    def __init__(self):
        super().__init__()
        self.tcp_thread = None
        self.uart_thread = None
        self.tcp_plot_widget = None
        self.uart_plot_widget = None
        
        # 同步相关状态
        self.sync_enabled = False  # 默认关闭同步模式，让用户手动决定
        self.auto_sync_enabled = True  # 默认开启自动同步
        self.tcp_sync_received = False
        self.uart_sync_received = False
        self.sync_timer = None
        
        # 同步等待状态（避免重复显示等待信息）
        self.tcp_sync_waiting_displayed = False
        self.uart_sync_waiting_displayed = False
        
        # 页面管理
        self.tab_widget = None
        self.communication_page = None
        self.sync_page = None
        
        self.init_ui()
        
    def init_ui(self):
        """初始化用户界面"""
        self.setWindowTitle('TCP/UART统一通信应用')
        self.setGeometry(100, 100, 1600, 800*2)
        
        # 创建中央widget
        central_widget = QWidget()
        self.setCentralWidget(central_widget)
        
        # 创建TabWidget
        self.tab_widget = QTabWidget()
        # 连接Tab切换事件
        self.tab_widget.currentChanged.connect(self.on_tab_changed)
        central_widget_layout = QVBoxLayout()
        central_widget_layout.addWidget(self.tab_widget)
        central_widget.setLayout(central_widget_layout)
        
        # 创建主页面
        self.init_main_page()
        
        # 创建通信页面
        self.init_communication_page()
        
        # 创建同步控制页面
        self.init_sync_page()
        
        # 显示主页面
        self.show_main_page()
    
    def init_main_page(self):
        """初始化主页面"""
        main_page = QWidget()
        layout = QVBoxLayout()
        main_page.setLayout(layout)
        
        # 添加导航栏
        self.init_navigation_bar(layout)
        
        # 主页面内容
        content_layout = QVBoxLayout()
        
        # 欢迎标题
        welcome_title = QLabel("TCP/UART统一通信应用")
        welcome_title.setStyleSheet("""
            font-size: 36px; 
            font-weight: bold; 
            color: #2c3e50; 
            padding: 30px;
            background-color: #ecf0f1;
            border-radius: 15px;
            margin: 20px;
        """)
        welcome_title.setAlignment(Qt.AlignCenter)
        welcome_title.setFixedHeight(100)
        content_layout.addWidget(welcome_title)
        
        # 移除功能选择区域，因为使用TabWidget不需要导航按钮
        
        # 状态信息区域
        status_group = QGroupBox("系统状态")
        status_group.setStyleSheet("""
            QGroupBox {
                font-weight: bold;
                font-size: 18px;
                color: #2c3e50;
                border: 2px solid #27ae60;
                border-radius: 10px;
                margin: 20px;
                padding-top: 20px;
            }
            QGroupBox::title {
                subcontrol-origin: margin;
                left: 20px;
                padding: 0 10px 0 10px;
            }
        """)
        status_layout = QVBoxLayout()
        
        # 连接状态显示
        self.connection_status_label = QLabel("连接状态: 未连接")
        self.connection_status_label.setStyleSheet("font-size: 16px; color: #7f8c8d; padding: 10px;")
        
        # 同步状态显示
        self.sync_status_main_label = QLabel("同步状态: 未启用")
        self.sync_status_main_label.setStyleSheet("font-size: 16px; color: #7f8c8d; padding: 10px;")
        
        status_layout.addWidget(self.connection_status_label)
        status_layout.addWidget(self.sync_status_main_label)
        
        status_group.setLayout(status_layout)
        content_layout.addWidget(status_group)
        
        # 添加弹性空间
        content_layout.addStretch()
        
        layout.addLayout(content_layout)
        
        # 添加到TabWidget
        self.tab_widget.addTab(main_page, "主界面")
    
    def init_communication_page(self):
        """初始化通信页面"""
        self.communication_page = CommunicationPage(self)
        self.tab_widget.addTab(self.communication_page, "TCP/UART通信")
    
    def init_sync_page(self):
        """初始化同步控制页面"""
        self.sync_page = SyncControlPage(self)
        self.tab_widget.addTab(self.sync_page, "同步控制")
        # 注意：不在这里立即更新按钮状态，因为SyncControlPage的init_ui可能还没完成
    
    def init_navigation_bar(self, parent_layout):
        """初始化导航栏"""
        nav_frame = QFrame()
        nav_frame.setStyleSheet("""
            QFrame {
                background-color: #34495e;
                border-radius: 5px;
                margin: 5px;
            }
        """)
        nav_frame.setFixedHeight(60)
        
        nav_layout = QHBoxLayout()
        nav_frame.setLayout(nav_layout)
        
        # 应用标题
        title_label = QLabel("TCP/UART统一通信应用")
        title_label.setStyleSheet("""
            color: white;
            font-size: 20px;
            font-weight: bold;
            padding: 10px;
        """)
        nav_layout.addWidget(title_label)
        
        nav_layout.addStretch()
        
        # 移除导航按钮，因为使用TabWidget不需要
        
        # 同步状态指示器
        self.sync_status_indicator = QLabel("同步: 未启用")
        self.sync_status_indicator.setStyleSheet("""
            color: #95a5a6;
            font-size: 12px;
            font-weight: bold;
            padding: 5px;
        """)
        nav_layout.addWidget(self.sync_status_indicator)
        
        parent_layout.addWidget(nav_frame)
    
    def on_tab_changed(self, index):
        """处理Tab切换事件"""
        if index == 2:  # 同步控制页面
            # 更新同步指令按钮状态
            self.update_sync_command_button()
    
    def show_main_page(self):
        """显示主页面"""
        self.tab_widget.setCurrentIndex(0)
        self.update_sync_status_indicator()
        self.update_connection_status()
    
    def show_communication_page(self):
        """显示通信页面"""
        self.tab_widget.setCurrentIndex(1)
        self.update_sync_status_indicator()
    
    def show_sync_page(self):
        """显示同步控制页面"""
        self.tab_widget.setCurrentIndex(2)
        # 同步页面状态到主应用
        if self.sync_page:
            # 不再需要设置同步模式开关，因为已经移除
            self.sync_page.update_sync_status(
                "同步模式已启用" if self.sync_enabled else "同步未启用",
                "#f39c12" if self.sync_enabled else "#7f8c8d"
            )
            # 更新同步指令按钮状态
            self.update_sync_command_button()
        
    def update_sync_status_indicator(self):
        """更新同步状态指示器"""
        if hasattr(self, 'sync_status_indicator'):
            if self.sync_enabled:
                if self.tcp_sync_received and self.uart_sync_received:
                    self.sync_status_indicator.setText("同步: 已完成")
                    self.sync_status_indicator.setStyleSheet("color: #27ae60; font-size: 12px; font-weight: bold; padding: 5px;")
                elif self.tcp_sync_received or self.uart_sync_received:
                    self.sync_status_indicator.setText("同步: 进行中")
                    self.sync_status_indicator.setStyleSheet("color: #f39c12; font-size: 12px; font-weight: bold; padding: 5px;")
                else:
                    self.sync_status_indicator.setText("同步: 已启用")
                    self.sync_status_indicator.setStyleSheet("color: #f39c12; font-size: 12px; font-weight: bold; padding: 5px;")
            else:
                self.sync_status_indicator.setText("同步: 未启用")
                self.sync_status_indicator.setStyleSheet("color: #95a5a6; font-size: 12px; font-weight: bold; padding: 5px;")
    
    def update_connection_status(self):
        """更新连接状态显示"""
        if hasattr(self, 'connection_status_label'):
            tcp_connected = self.tcp_thread and self.tcp_thread.isRunning()
            uart_connected = self.uart_thread and self.uart_thread.isRunning()
            
            if tcp_connected and uart_connected:
                self.connection_status_label.setText("连接状态: TCP和UART已连接")
                self.connection_status_label.setStyleSheet("font-size: 16px; color: #27ae60; padding: 10px;")
            elif tcp_connected:
                self.connection_status_label.setText("连接状态: TCP已连接")
                self.connection_status_label.setStyleSheet("font-size: 16px; color: #3498db; padding: 10px;")
            elif uart_connected:
                self.connection_status_label.setText("连接状态: UART已连接")
                self.connection_status_label.setStyleSheet("font-size: 16px; color: #e74c3c; padding: 10px;")
            else:
                self.connection_status_label.setText("连接状态: 未连接")
                self.connection_status_label.setStyleSheet("font-size: 16px; color: #7f8c8d; padding: 10px;")
        
        if hasattr(self, 'sync_status_main_label'):
            if self.sync_enabled:
                if self.tcp_sync_received and self.uart_sync_received:
                    self.sync_status_main_label.setText("同步状态: 已完成")
                    self.sync_status_main_label.setStyleSheet("font-size: 16px; color: #27ae60; padding: 10px;")
                elif self.tcp_sync_received or self.uart_sync_received:
                    self.sync_status_main_label.setText("同步状态: 进行中")
                    self.sync_status_main_label.setStyleSheet("font-size: 16px; color: #f39c12; padding: 10px;")
                else:
                    self.sync_status_main_label.setText("同步状态: 已启用")
                    self.sync_status_main_label.setStyleSheet("font-size: 16px; color: #f39c12; padding: 10px;")
            else:
                self.sync_status_main_label.setText("同步状态: 未启用")
                self.sync_status_main_label.setStyleSheet("font-size: 16px; color: #7f8c8d; padding: 10px;")
    
    def update_sync_command_button(self):
        """更新同步指令按钮状态"""
        try:
            if (self.sync_page and 
                hasattr(self.sync_page, 'send_sync_command_button') and 
                self.sync_page.send_sync_command_button is not None):
                tcp_connected = self.tcp_thread and self.tcp_thread.isRunning()
                uart_connected = self.uart_thread and self.uart_thread.isRunning()
                
                # 只要有任一连接就启用按钮
                self.sync_page.send_sync_command_button.setEnabled(tcp_connected or uart_connected)
        except Exception as e:
            print(f"更新同步指令按钮状态时出错: {e}")
            print(f"sync_page: {self.sync_page}")
            if self.sync_page:
                print(f"hasattr send_sync_command_button: {hasattr(self.sync_page, 'send_sync_command_button')}")
                if hasattr(self.sync_page, 'send_sync_command_button'):
                    print(f"send_sync_command_button: {self.sync_page.send_sync_command_button}")
                    print(f"send_sync_command_button type: {type(self.sync_page.send_sync_command_button)}")
        
    def init_tcp_section(self, parent):
        """初始化TCP通信区域"""
        layout = QVBoxLayout()
        parent.setLayout(layout)
        
        # TCP标题
        tcp_title = QLabel("TCP通信")
        tcp_title.setStyleSheet("font-size: 28px; font-weight: bold; color: #2c3e50; padding: 10px;")
        tcp_title.setAlignment(Qt.AlignCenter)
        tcp_title.setFixedHeight(50)  # 增加标题高度
        layout.addWidget(tcp_title)
        
        # 模式选择
        mode_group = QGroupBox("模式选择")
        mode_layout = QHBoxLayout()
        
        self.tcp_mode_combo = QComboBox()
        self.tcp_mode_combo.addItems(['服务端', '客户端'])
        self.tcp_mode_combo.setCurrentText("服务端")  # 设置服务端为默认选项
        self.tcp_mode_combo.currentTextChanged.connect(self.on_tcp_mode_changed)
        mode_layout.addWidget(QLabel("工作模式:"))
        mode_layout.addWidget(self.tcp_mode_combo)
        mode_layout.addStretch()
        
        mode_group.setLayout(mode_layout)
        layout.addWidget(mode_group)
        
        # 连接配置
        config_group = QGroupBox("连接配置")
        config_layout = QGridLayout()
        
        # 主机地址（仅客户端使用）
        self.tcp_host_label = QLabel("IP:")
        self.tcp_host_input = QLineEdit()
        self.tcp_host_input.setText("127.0.0.1")
        self.tcp_host_input.setPlaceholderText("输入服务器IP地址")
        
        # 端口号
        self.tcp_port_label = QLabel("端口号:")
        self.tcp_port_input = QSpinBox()
        self.tcp_port_input.setRange(1, 65535)
        self.tcp_port_input.setValue(1123)
        
        # 连接按钮
        self.tcp_connect_button = QPushButton("连接")
        self.tcp_connect_button.clicked.connect(self.toggle_tcp_connection)
        
        # 状态显示
        self.tcp_status_label = QLabel("未连接")
        self.tcp_status_label.setStyleSheet("color: red; font-weight: bold;")
        
        config_layout.addWidget(self.tcp_host_label, 0, 0)
        config_layout.addWidget(self.tcp_host_input, 0, 1)
        config_layout.addWidget(self.tcp_port_label, 0, 2)
        config_layout.addWidget(self.tcp_port_input, 0, 3)
        config_layout.addWidget(self.tcp_connect_button, 0, 4)
        config_layout.addWidget(self.tcp_status_label, 1, 0, 1, 5)
        
        config_group.setLayout(config_layout)
        layout.addWidget(config_group)
        
        # 数据发送
        send_group = QGroupBox("TCP数据发送")
        send_group.setFixedHeight(230)  # 设置最小高度
        send_layout = QVBoxLayout()
        
        # 发送输入
        send_input_layout = QHBoxLayout()
        self.tcp_send_input = QLineEdit()
        self.tcp_send_input.setPlaceholderText("输入要发送的TCP数据...")
        self.tcp_send_input.returnPressed.connect(self.send_tcp_data)
        
        self.tcp_send_button = QPushButton("发送")
        self.tcp_send_button.clicked.connect(self.send_tcp_data)
        self.tcp_send_button.setEnabled(False)
        
        send_input_layout.addWidget(self.tcp_send_input)
        send_input_layout.addWidget(self.tcp_send_button)
        
        # TCP发送选项
        send_options_layout = QHBoxLayout()
        self.tcp_add_newline_check = QCheckBox("发送时添加换行符")
        self.tcp_add_newline_check.setChecked(True)
        send_options_layout.addWidget(self.tcp_add_newline_check)
        send_options_layout.addStretch()
        
        # TCP快速发送按钮
        tcp_quick_send_layout = QHBoxLayout()
        tcp_quick_buttons = ["Hello", "Start", "Ping", "Data"]
        for text in tcp_quick_buttons:
            btn = QPushButton(text)
            btn.clicked.connect(lambda checked, t=text: self.quick_send_tcp(t))
            btn.setEnabled(True)
            tcp_quick_send_layout.addWidget(btn)
        
        send_layout.addLayout(send_input_layout)
        send_layout.addLayout(send_options_layout)
        send_layout.addLayout(tcp_quick_send_layout)
        
        send_group.setLayout(send_layout)
        layout.addWidget(send_group)
        
        # TCP数据转发选项
        tcp_forward_group = QGroupBox("数据转发")
        tcp_forward_group.setMinimumHeight(160)  # 增加高度以容纳新控件
        tcp_forward_group.setMaximumHeight(160)  # 设置最大高度
        tcp_forward_layout = QVBoxLayout()
        
        self.tcp_to_uart_check = QCheckBox("TCP接收数据转发到UART")
        self.tcp_to_uart_check.setChecked(False)
        tcp_forward_layout.addWidget(self.tcp_to_uart_check)
        
        # TCP转发条件设置
        tcp_condition_layout = QHBoxLayout()
        self.tcp_forward_condition_check = QCheckBox("仅转发包含指定内容的数据:")
        self.tcp_forward_condition_check.setChecked(False)
        self.tcp_forward_condition_text = QLineEdit()
        self.tcp_forward_condition_text.setPlaceholderText("输入转发条件（留空则转发所有数据）")
        self.tcp_forward_condition_text.setEnabled(False)
        
        # 连接信号
        self.tcp_forward_condition_check.toggled.connect(self.tcp_forward_condition_text.setEnabled)
        
        tcp_condition_layout.addWidget(self.tcp_forward_condition_check)
        tcp_condition_layout.addWidget(self.tcp_forward_condition_text)
        tcp_forward_layout.addLayout(tcp_condition_layout)
        
        tcp_forward_group.setLayout(tcp_forward_layout)
        layout.addWidget(tcp_forward_group)
        
        # TCP数据显示区域
        tcp_display_group = QGroupBox("TCP数据接收")
        tcp_display_group.setFixedHeight(500)  # 固定整个区域高度
        tcp_display_layout = QVBoxLayout()
        
        # TCP控制按钮
        tcp_control_layout = QHBoxLayout()
        self.clear_tcp_button = QPushButton("清空TCP显示")
        self.clear_tcp_button.clicked.connect(self.clear_tcp_display)
        
        tcp_control_layout.addWidget(self.clear_tcp_button)
        tcp_control_layout.addStretch()
        
        # TCP数据显示区域
        self.tcp_data_display = QTextEdit()
        self.tcp_data_display.setReadOnly(True)
        self.tcp_data_display.setFont(QFont("Consolas", 10))
        
        tcp_display_layout.addLayout(tcp_control_layout)
        tcp_display_layout.addWidget(self.tcp_data_display)
        
        tcp_display_group.setLayout(tcp_display_layout)
        layout.addWidget(tcp_display_group)
        
        # TCP实时绘图区域（可选）
        self.tcp_plot_group = QGroupBox("TCP实时绘图")
        self.tcp_plot_group.setCheckable(True)  # 设置为可勾选
        self.tcp_plot_group.setChecked(True)    # 默认勾选
        self.tcp_plot_group.setMinimumHeight(650)  # 设置最小高度
        self.tcp_plot_group.setMaximumHeight(650)  # 设置最大高度
        tcp_plot_layout = QVBoxLayout()
        
        # 创建TCP绘图组件
        self.tcp_plot_widget = RealTimePlotWidget("TCP")
        tcp_plot_layout.addWidget(self.tcp_plot_widget)
        
        # TCP绘图控制按钮（第一行）
        tcp_plot_control_layout = QHBoxLayout()
        
        # 显示点数控制
        tcp_plot_control_layout.addWidget(QLabel("显示点数(0-100000):"))
        self.tcp_plot_points_spin = QSpinBox()
        self.tcp_plot_points_spin.setRange(0, 100000)
        self.tcp_plot_points_spin.setValue(1000)
        self.tcp_plot_points_spin.setToolTip("设置显示的数据点数量，范围：0-100000")
        self.tcp_plot_points_spin.valueChanged.connect(self.on_tcp_plot_points_changed)
        tcp_plot_control_layout.addWidget(self.tcp_plot_points_spin)
        
        # 暂停/继续按钮
        self.tcp_plot_pause_button = QPushButton("暂停")
        self.tcp_plot_pause_button.setCheckable(True)
        self.tcp_plot_pause_button.setToolTip("暂停/继续数据采集和绘图")
        self.tcp_plot_pause_button.clicked.connect(self.toggle_tcp_plot_pause)
        tcp_plot_control_layout.addWidget(self.tcp_plot_pause_button)
        
        # 保存按钮
        self.tcp_plot_save_button = QPushButton("保存CSV")
        self.tcp_plot_save_button.setToolTip("保存当前显示的数据到CSV文件")
        self.tcp_plot_save_button.clicked.connect(self.save_tcp_plot_data)
        tcp_plot_control_layout.addWidget(self.tcp_plot_save_button)
        
        # 清空按钮
        self.clear_tcp_plot_button = QPushButton("清空")
        self.clear_tcp_plot_button.setToolTip("清空所有绘图数据")
        self.clear_tcp_plot_button.clicked.connect(self.clear_tcp_plot)
        tcp_plot_control_layout.addWidget(self.clear_tcp_plot_button)
        
        tcp_plot_control_layout.addStretch()
        
        # 通道选择控制（第二行）
        tcp_channel_layout = QHBoxLayout()
        tcp_channel_layout.addWidget(QLabel("显示通道:"))
        
        # 创建通道复选框容器
        self.tcp_channel_checkboxes = []
        for i in range(6):  # 最多支持6个通道
            checkbox = QCheckBox(f"data{i+1}")
            checkbox.setChecked(True)
            checkbox.setToolTip(f"显示/隐藏data{i+1}通道")
            checkbox.stateChanged.connect(lambda state, idx=i: self.on_tcp_channel_toggled(idx, state))
            tcp_channel_layout.addWidget(checkbox)
            self.tcp_channel_checkboxes.append(checkbox)
        
        tcp_channel_layout.addStretch()
        
        tcp_plot_layout.addLayout(tcp_plot_control_layout)
        tcp_plot_layout.addLayout(tcp_channel_layout)
        self.tcp_plot_group.setLayout(tcp_plot_layout)
        
        # 连接勾选状态变化信号
        self.tcp_plot_group.toggled.connect(self.on_tcp_plot_toggled)
        layout.addWidget(self.tcp_plot_group)
        
        # 初始化界面状态
        self.on_tcp_mode_changed("服务端")
        
    def init_uart_section(self, parent):
        """初始化UART通信区域"""
        layout = QVBoxLayout()
        parent.setLayout(layout)
        
        # UART标题
        uart_title = QLabel("UART通信")
        uart_title.setStyleSheet("font-size: 28px; font-weight: bold; color: #2c3e50; padding: 10px;")
        uart_title.setAlignment(Qt.AlignCenter)
        uart_title.setFixedHeight(50)  # 增加标题高度
        layout.addWidget(uart_title)
        
        # 串口配置
        uart_config_group = QGroupBox("串口配置")
        uart_config_layout = QGridLayout()
        
        # 串口选择
        self.uart_port_combo = QComboBox()
        self.refresh_ports_button = QPushButton("刷新端口")
        self.refresh_ports_button.clicked.connect(self.refresh_uart_ports)
        
        # 波特率选择
        self.uart_baudrate_combo = QComboBox()
        self.uart_baudrate_combo.addItems(['9600', '19200', '38400', '57600', '115200', '230400', '460800', '921600'])
        self.uart_baudrate_combo.setCurrentText('115200')
        
        # 连接按钮
        self.uart_connect_button = QPushButton("连接串口")
        self.uart_connect_button.clicked.connect(self.toggle_uart_connection)
        
        # 状态显示
        self.uart_status_label = QLabel("串口未连接")
        self.uart_status_label.setStyleSheet("color: red; font-weight: bold;")
        
        uart_config_layout.addWidget(QLabel("串口:"), 0, 0)
        uart_config_layout.addWidget(self.uart_port_combo, 0, 1)
        uart_config_layout.addWidget(self.refresh_ports_button, 0, 2)
        uart_config_layout.addWidget(QLabel("波特率:"), 1, 0)
        uart_config_layout.addWidget(self.uart_baudrate_combo, 1, 1)
        uart_config_layout.addWidget(self.uart_connect_button, 1, 2)
        uart_config_layout.addWidget(self.uart_status_label, 2, 0, 1, 3)
        
        uart_config_group.setLayout(uart_config_layout)
        layout.addWidget(uart_config_group)
        
        # UART数据发送
        uart_send_group = QGroupBox("UART数据发送")
        uart_send_group.setFixedHeight(230)  # 设置最小高度
        uart_send_layout = QVBoxLayout()
        
        # 发送输入
        uart_send_input_layout = QHBoxLayout()
        self.uart_send_input = QLineEdit()
        self.uart_send_input.setPlaceholderText("输入要发送的UART数据...")
        self.uart_send_input.returnPressed.connect(self.send_uart_data)
        
        self.uart_send_button = QPushButton("发送")
        self.uart_send_button.clicked.connect(self.send_uart_data)
        self.uart_send_button.setEnabled(False)
        
        uart_send_input_layout.addWidget(self.uart_send_input)
        uart_send_input_layout.addWidget(self.uart_send_button)
        
        # UART发送选项
        uart_send_options_layout = QHBoxLayout()
        self.uart_add_newline_check = QCheckBox("发送时添加换行符")
        self.uart_add_newline_check.setChecked(True)
        uart_send_options_layout.addWidget(self.uart_add_newline_check)
        uart_send_options_layout.addStretch()
        
        # UART快速发送按钮
        uart_quick_send_layout = QHBoxLayout()
        uart_quick_buttons = ["Hello", "Start", "AT+STATUS", "PING"]
        for text in uart_quick_buttons:
            btn = QPushButton(text)
            btn.clicked.connect(lambda checked, t=text: self.quick_send_uart(t))
            btn.setEnabled(True)
            uart_quick_send_layout.addWidget(btn)
        
        uart_send_layout.addLayout(uart_send_input_layout)
        uart_send_layout.addLayout(uart_send_options_layout)
        uart_send_layout.addLayout(uart_quick_send_layout)
        
        uart_send_group.setLayout(uart_send_layout)
        layout.addWidget(uart_send_group)
        
        # UART数据转发选项
        uart_forward_group = QGroupBox("数据转发")
        uart_forward_group.setMinimumHeight(160)  # 增加高度以容纳新控件
        uart_forward_group.setMaximumHeight(160)  # 设置最大高度
        uart_forward_layout = QVBoxLayout()
        
        self.uart_to_tcp_check = QCheckBox("UART接收数据转发到TCP")
        self.uart_to_tcp_check.setChecked(False)
        uart_forward_layout.addWidget(self.uart_to_tcp_check)
        
        # UART转发条件设置
        uart_condition_layout = QHBoxLayout()
        self.uart_forward_condition_check = QCheckBox("仅转发包含指定内容的数据:")
        self.uart_forward_condition_check.setChecked(False)
        self.uart_forward_condition_text = QLineEdit()
        self.uart_forward_condition_text.setPlaceholderText("输入转发条件（留空则转发所有数据）")
        self.uart_forward_condition_text.setEnabled(False)
        
        # 连接信号
        self.uart_forward_condition_check.toggled.connect(self.uart_forward_condition_text.setEnabled)
        
        uart_condition_layout.addWidget(self.uart_forward_condition_check)
        uart_condition_layout.addWidget(self.uart_forward_condition_text)
        uart_forward_layout.addLayout(uart_condition_layout)
        
        uart_forward_group.setLayout(uart_forward_layout)
        layout.addWidget(uart_forward_group)
        
        # UART数据显示区域
        uart_display_group = QGroupBox("UART数据接收")
        uart_display_group.setFixedHeight(500)  # 固定整个区域高度
        uart_display_layout = QVBoxLayout()
        
        # UART控制按钮
        uart_control_layout = QHBoxLayout()
        self.clear_uart_button = QPushButton("清空UART显示")
        self.clear_uart_button.clicked.connect(self.clear_uart_display)
        
        uart_control_layout.addWidget(self.clear_uart_button)
        uart_control_layout.addStretch()
        
        # UART数据显示区域
        self.uart_data_display = QTextEdit()
        self.uart_data_display.setReadOnly(True)
        self.uart_data_display.setFont(QFont("Consolas", 10))
        
        uart_display_layout.addLayout(uart_control_layout)
        uart_display_layout.addWidget(self.uart_data_display)
        
        uart_display_group.setLayout(uart_display_layout)
        layout.addWidget(uart_display_group)
        
        # UART实时绘图区域（可选）
        self.uart_plot_group = QGroupBox("UART实时绘图")
        self.uart_plot_group.setCheckable(True)  # 设置为可勾选
        self.uart_plot_group.setChecked(True)    # 默认勾选
        self.uart_plot_group.setMinimumHeight(650)  # 设置最小高度
        self.uart_plot_group.setMaximumHeight(650)  # 设置最大高度
        uart_plot_layout = QVBoxLayout()
        
        # 创建UART绘图组件
        self.uart_plot_widget = RealTimePlotWidget("UART")
        uart_plot_layout.addWidget(self.uart_plot_widget)
        
        # UART绘图控制按钮
        uart_plot_control_layout = QHBoxLayout()
        
        # 显示点数控制
        uart_plot_control_layout.addWidget(QLabel("显示点数(0-100000):"))
        self.uart_plot_points_spin = QSpinBox()
        self.uart_plot_points_spin.setRange(0, 100000)
        self.uart_plot_points_spin.setValue(1000)
        self.uart_plot_points_spin.setToolTip("设置显示的数据点数量，范围：0-100000")
        self.uart_plot_points_spin.valueChanged.connect(self.on_uart_plot_points_changed)
        uart_plot_control_layout.addWidget(self.uart_plot_points_spin)
        
        # 暂停/继续按钮
        self.uart_plot_pause_button = QPushButton("暂停")
        self.uart_plot_pause_button.setCheckable(True)
        self.uart_plot_pause_button.setToolTip("暂停/继续数据采集和绘图")
        self.uart_plot_pause_button.clicked.connect(self.toggle_uart_plot_pause)
        uart_plot_control_layout.addWidget(self.uart_plot_pause_button)
        
        # 保存按钮
        self.uart_plot_save_button = QPushButton("保存CSV")
        self.uart_plot_save_button.setToolTip("保存当前显示的数据到CSV文件")
        self.uart_plot_save_button.clicked.connect(self.save_uart_plot_data)
        uart_plot_control_layout.addWidget(self.uart_plot_save_button)
        
        # 清空按钮
        self.clear_uart_plot_button = QPushButton("清空")
        self.clear_uart_plot_button.setToolTip("清空所有绘图数据")
        self.clear_uart_plot_button.clicked.connect(self.clear_uart_plot)
        uart_plot_control_layout.addWidget(self.clear_uart_plot_button)
        
        uart_plot_control_layout.addStretch()
        
        # 通道选择控制（第二行）
        uart_channel_layout = QHBoxLayout()
        uart_channel_layout.addWidget(QLabel("显示通道:"))
        
        # 创建通道复选框容器
        self.uart_channel_checkboxes = []
        for i in range(6):  # 最多支持6个通道
            checkbox = QCheckBox(f"data{i+1}")
            checkbox.setChecked(True)
            checkbox.setToolTip(f"显示/隐藏data{i+1}通道")
            checkbox.stateChanged.connect(lambda state, idx=i: self.on_uart_channel_toggled(idx, state))
            uart_channel_layout.addWidget(checkbox)
            self.uart_channel_checkboxes.append(checkbox)
        
        uart_channel_layout.addStretch()
        
        uart_plot_layout.addLayout(uart_plot_control_layout)
        uart_plot_layout.addLayout(uart_channel_layout)
        self.uart_plot_group.setLayout(uart_plot_layout)
        
        # 连接勾选状态变化信号
        self.uart_plot_group.toggled.connect(self.on_uart_plot_toggled)
        layout.addWidget(self.uart_plot_group)
        
        # 初始化串口列表
        self.refresh_uart_ports()
        
        
    def on_tcp_mode_changed(self, mode):
        """TCP模式切换处理"""
        is_client = (mode == "客户端")
        self.tcp_host_label.setVisible(is_client)
        self.tcp_host_input.setVisible(is_client)
        
        if is_client:
            self.tcp_connect_button.setText("连接")
            self.tcp_port_label.setText("端口:")
        else:
            self.tcp_connect_button.setText("启动服务")
            self.tcp_port_label.setText("监听端口:")
    
    def toggle_tcp_connection(self):
        """切换TCP连接状态"""
        if self.tcp_thread and self.tcp_thread.isRunning():
            self.disconnect_tcp()
        else:
            self.connect_tcp()
    
    def connect_tcp(self):
        """建立TCP连接"""
        mode = self.tcp_mode_combo.currentText()
        port = self.tcp_port_input.value()
        
        if mode == "客户端":
            host = self.tcp_host_input.text().strip()
            if not host:
                QMessageBox.warning(self, "警告", "请输入主机地址!")
                return
            self.start_tcp_client(host, port)
        else:
            self.start_tcp_server(port)
    
    def start_tcp_client(self, host, port):
        """启动TCP客户端"""
        self.tcp_thread = TCPThread('client')
        self.tcp_thread.data_received.connect(self.on_tcp_data_received)
        self.tcp_thread.status_changed.connect(self.on_tcp_status_changed)
        self.tcp_thread.connection_changed.connect(self.on_tcp_connection_changed)
        self.tcp_thread.start_client(host, port)
    
    def start_tcp_server(self, port):
        """启动TCP服务端"""
        self.tcp_thread = TCPThread('server')
        self.tcp_thread.data_received.connect(self.on_tcp_data_received)
        self.tcp_thread.status_changed.connect(self.on_tcp_status_changed)
        self.tcp_thread.connection_changed.connect(self.on_tcp_connection_changed)
        self.tcp_thread.start_server(port)
    
    def disconnect_tcp(self):
        """断开TCP连接"""
        if self.tcp_thread:
            self.tcp_thread.stop()
            self.tcp_thread = None
    
    def send_tcp_data(self):
        """发送TCP数据"""
        data = self.tcp_send_input.text()
        if data and self.tcp_thread and self.tcp_thread.isRunning():
            # 检查同步模式
            if self.sync_enabled and not self.tcp_sync_received:
                self.add_tcp_display_text(f"[同步等待] 无法发送数据，等待TCP同步确认")
                return
            
            # 根据复选框决定是否添加换行符
            if self.tcp_add_newline_check.isChecked():
                data_to_send = data + '\n'
            else:
                data_to_send = data
                
            if self.tcp_thread.send_data(data_to_send):
                timestamp = datetime.now().strftime("%H:%M:%S")
                display_data = data  # 显示时不包含换行符
                self.add_tcp_display_text(f"[{timestamp}] TCP发送: {display_data}")
                self.tcp_send_input.clear()
    
    def quick_send_tcp(self, text):
        """快速发送TCP数据"""
        self.tcp_send_input.setText(text)
        self.send_tcp_data()
    
    def on_tcp_data_received(self, data):
        """处理TCP接收到的数据"""
        # 提取纯文本内容（去掉时间戳）
        pure_content = data
        if data.startswith('[') and '] ' in data:
            # 去掉时间戳 [HH:MM:SS]
            pure_content = data.split('] ', 1)[1] if len(data.split('] ', 1)) > 1 else data
        
        # 检查同步模式（现在总是检查）
        if self.sync_enabled:
            # 检查是否收到同步确认
            if self.sync_page and self.sync_page.tcp_recv_enable_check.isChecked():
                tcp_sync_recv = self.sync_page.tcp_sync_receive_input.text().strip()
                if tcp_sync_recv and tcp_sync_recv in pure_content:
                    self.tcp_sync_received = True
                    self.update_sync_status()
                    self.add_tcp_display_text(f"[同步] TCP收到同步确认: {pure_content}")
                    return
            
            # 如果同步未完成，不处理其他数据（阻止数据显示）
            if not self.tcp_sync_received:
                # 只显示一次同步等待信息，避免重复显示
                if not self.tcp_sync_waiting_displayed:
                    self.add_tcp_display_text(f"[同步等待] 无限等待TCP同步确认...")
                    self.tcp_sync_waiting_displayed = True
                return
        
        # 正常数据处理
        self.add_tcp_display_text(data)
        
        # 提取数据内容用于绘图（仅在绘图功能启用时）
        if self.tcp_plot_widget and self.tcp_plot_group.isChecked():
            # 尝试解析数据并绘图
            self.tcp_plot_widget.add_data(pure_content)
        
        # 如果启用了TCP到UART转发
        if self.tcp_to_uart_check.isChecked() and self.uart_thread and self.uart_thread.isRunning():
            # 检查转发条件
            if self.tcp_forward_condition_check.isChecked():
                condition_text = self.tcp_forward_condition_text.text().strip()
                if condition_text and condition_text in pure_content:
                    # 只转发指定的内容本身，结尾添加换行符
                    content_with_newline = condition_text.strip() + '\n'
                    self.uart_thread.send_data(content_with_newline)
            else:
                # 如果未启用条件转发，转发完整的纯文本内容
                content_with_newline = pure_content.strip() + '\n'
                self.uart_thread.send_data(content_with_newline)
    
    def on_tcp_status_changed(self, status):
        """处理TCP状态变化"""
        self.tcp_status_label.setText(status)
        if "连接" in status or "启动" in status:
            self.tcp_status_label.setStyleSheet("color: green; font-weight: bold;")
        else:
            self.tcp_status_label.setStyleSheet("color: red; font-weight: bold;")
    
    def on_tcp_connection_changed(self, connected):
        """处理TCP连接状态变化"""
        self.tcp_send_button.setEnabled(connected)
        self.tcp_send_input.setEnabled(connected)
        
        # 启用/禁用快速发送按钮
        for i in range(4):  # 4个快速发送按钮
            widget = self.findChild(QPushButton)
            if widget and widget.text() in ["Hello", "Test", "Ping", "Data"]:
                widget.setEnabled(connected)
        
        if connected:
            self.tcp_connect_button.setText("断开")
            # 如果启用了同步模式且自动同步开启，自动开始同步
            if self.sync_enabled and self.auto_sync_enabled:
                self.auto_start_sync()
        else:
            self.tcp_connect_button.setText("连接" if self.tcp_mode_combo.currentText() == "客户端" else "启动服务")
        
        # 更新主界面状态显示
        self.update_connection_status()
        
        # 更新同步指令按钮状态
        self.update_sync_command_button()
    
    def refresh_uart_ports(self):
        """刷新串口列表"""
        self.uart_port_combo.clear()
        ports = serial.tools.list_ports.comports()
        valid_ports = []
        
        # 只显示有效的串口（ttyUSB*或ttyACM*）
        for port in ports:
            if port.device.startswith('/dev/ttyUSB') or port.device.startswith('/dev/ttyACM'):
                valid_ports.append(port)
                self.uart_port_combo.addItem(f"{port.device} - {port.description}")
        
        # 如果有有效串口，自动选择第一个
        if valid_ports:
            self.uart_port_combo.setCurrentIndex(0)
    
    def toggle_uart_connection(self):
        """切换UART连接状态"""
        if self.uart_thread and self.uart_thread.isRunning():
            self.disconnect_uart()
        else:
            self.connect_uart()
    
    def connect_uart(self):
        """连接UART"""
        port_text = self.uart_port_combo.currentText()
        if not port_text:
            QMessageBox.warning(self, "警告", "请选择串口!")
            return
        
        port = port_text.split(' - ')[0]
        baudrate = int(self.uart_baudrate_combo.currentText())
        
        self.uart_thread = UARTThread()
        self.uart_thread.data_received.connect(self.on_uart_data_received)
        self.uart_thread.status_changed.connect(self.on_uart_status_changed)
        self.uart_thread.connection_changed.connect(self.on_uart_connection_changed)
        self.uart_thread.connect_uart(port, baudrate)
    
    def disconnect_uart(self):
        """断开UART连接"""
        if self.uart_thread:
            self.uart_thread.stop()
            self.uart_thread = None
    
    def send_uart_data(self):
        """发送UART数据"""
        data = self.uart_send_input.text()
        if data and self.uart_thread and self.uart_thread.isRunning():
            # 检查同步模式
            if self.sync_enabled and not self.uart_sync_received:
                self.add_uart_display_text(f"[同步等待] 无法发送数据，等待UART同步确认")
                return
            
            # 根据复选框决定是否添加换行符
            if self.uart_add_newline_check.isChecked():
                data_to_send = data + '\n'
            else:
                data_to_send = data
                
            if self.uart_thread.send_data(data_to_send):
                timestamp = datetime.now().strftime("%H:%M:%S")
                display_data = data  # 显示时不包含换行符
                self.add_uart_display_text(f"[{timestamp}] UART发送: {display_data}")
                self.uart_send_input.clear()
    
    def quick_send_uart(self, text):
        """快速发送UART数据"""
        self.uart_send_input.setText(text)
        self.send_uart_data()
    
    def on_uart_data_received(self, data):
        """处理UART接收到的数据"""
        # 提取纯文本内容（去掉时间戳）
        pure_content = data
        if data.startswith('[') and '] ' in data:
            # 去掉时间戳 [HH:MM:SS]
            pure_content = data.split('] ', 1)[1] if len(data.split('] ', 1)) > 1 else data
        
        # 检查同步模式（现在总是检查）
        if self.sync_enabled:
            # 检查是否收到同步确认
            if self.sync_page and self.sync_page.uart_recv_enable_check.isChecked():
                uart_sync_recv = self.sync_page.uart_sync_receive_input.text().strip()
                if uart_sync_recv and uart_sync_recv in pure_content:
                    self.uart_sync_received = True
                    self.update_sync_status()
                    self.add_uart_display_text(f"[同步] UART收到同步确认: {pure_content}")
                    return
            
            # 如果同步未完成，不处理其他数据（阻止数据显示）
            if not self.uart_sync_received:
                # 只显示一次同步等待信息，避免重复显示
                if not self.uart_sync_waiting_displayed:
                    self.add_uart_display_text(f"[同步等待] 无限等待UART同步确认...")
                    self.uart_sync_waiting_displayed = True
                return
        
        # 正常数据处理
        self.add_uart_display_text(data)
        
        # 提取数据内容用于绘图（仅在绘图功能启用时）
        if self.uart_plot_widget and self.uart_plot_group.isChecked():
            # 尝试解析数据并绘图
            self.uart_plot_widget.add_data(pure_content)
        
        # 如果启用了UART到TCP转发
        if self.uart_to_tcp_check.isChecked() and self.tcp_thread and self.tcp_thread.isRunning():
            # 检查转发条件
            if self.uart_forward_condition_check.isChecked():
                condition_text = self.uart_forward_condition_text.text().strip()
                if condition_text and condition_text in pure_content:
                    # 只转发指定的内容本身，结尾添加换行符
                    content_with_newline = condition_text.strip() + '\n'
                    self.tcp_thread.send_data(content_with_newline)
            else:
                # 如果未启用条件转发，转发完整的纯文本内容
                content_with_newline = pure_content.strip() + '\n'
                self.tcp_thread.send_data(content_with_newline)
    
    def on_uart_status_changed(self, status):
        """处理UART状态变化"""
        self.uart_status_label.setText(status)
        if "连接" in status:
            self.uart_status_label.setStyleSheet("color: green; font-weight: bold;")
        else:
            self.uart_status_label.setStyleSheet("color: red; font-weight: bold;")
    
    def on_uart_connection_changed(self, connected):
        """处理UART连接状态变化"""
        self.uart_send_button.setEnabled(connected)
        self.uart_send_input.setEnabled(connected)
        
        # 启用/禁用快速发送按钮
        for i in range(4):  # 4个快速发送按钮
            widget = self.findChild(QPushButton)
            if widget and widget.text() in ["AT", "AT+VERSION", "AT+STATUS", "PING"]:
                widget.setEnabled(connected)
        
        if connected:
            self.uart_connect_button.setText("断开串口")
            # 如果启用了同步模式且自动同步开启，自动开始同步
            if self.sync_enabled and self.auto_sync_enabled:
                self.auto_start_sync()
        else:
            self.uart_connect_button.setText("连接串口")
        
        # 更新主界面状态显示
        self.update_connection_status()
        
        # 更新同步指令按钮状态
        self.update_sync_command_button()
    
    def add_tcp_display_text(self, text):
        """添加TCP显示文本"""
        self.tcp_data_display.append(text)
        # 自动滚动到底部
        cursor = self.tcp_data_display.textCursor()
        cursor.movePosition(QTextCursor.End)
        self.tcp_data_display.setTextCursor(cursor)
    
    def add_uart_display_text(self, text):
        """添加UART显示文本"""
        self.uart_data_display.append(text)
        # 自动滚动到底部
        cursor = self.uart_data_display.textCursor()
        cursor.movePosition(QTextCursor.End)
        self.uart_data_display.setTextCursor(cursor)
    
    def clear_tcp_display(self):
        """清空TCP显示"""
        self.tcp_data_display.clear()
    
    def clear_uart_display(self):
        """清空UART显示"""
        self.uart_data_display.clear()
    
    def clear_tcp_plot(self):
        """清空TCP绘图"""
        if self.tcp_plot_widget:
            self.tcp_plot_widget.clear_data()
    
    def clear_uart_plot(self):
        """清空UART绘图"""
        if self.uart_plot_widget:
            self.uart_plot_widget.clear_data()
    
    def on_tcp_plot_toggled(self, checked):
        """TCP绘图功能切换处理"""
        # 不管是否勾选，都保持绘图界面可见
        # 只是控制是否接收新数据
        pass
    
    def on_uart_plot_toggled(self, checked):
        """UART绘图功能切换处理"""
        # 不管是否勾选，都保持绘图界面可见
        # 只是控制是否接收新数据
        pass
    
    def on_tcp_plot_points_changed(self, value):
        """TCP绘图点数变化处理"""
        if self.tcp_plot_widget:
            self.tcp_plot_widget.set_max_points(value)
    
    def on_uart_plot_points_changed(self, value):
        """UART绘图点数变化处理"""
        if self.uart_plot_widget:
            self.uart_plot_widget.set_max_points(value)
    
    def toggle_tcp_plot_pause(self):
        """切换TCP绘图暂停状态"""
        if self.tcp_plot_widget:
            is_paused = self.tcp_plot_widget.toggle_pause()
            self.tcp_plot_pause_button.setText("继续" if is_paused else "暂停")
    
    def toggle_uart_plot_pause(self):
        """切换UART绘图暂停状态"""
        if self.uart_plot_widget:
            is_paused = self.uart_plot_widget.toggle_pause()
            self.uart_plot_pause_button.setText("继续" if is_paused else "暂停")
    
    def save_tcp_plot_data(self):
        """保存TCP绘图数据到CSV"""
        from PyQt5.QtWidgets import QFileDialog
        if self.tcp_plot_widget and self.tcp_plot_widget.data_buffer:
            filename, _ = QFileDialog.getSaveFileName(
                self, "保存TCP数据", "", "CSV文件 (*.csv);;所有文件 (*)"
            )
            if filename:
                if self.tcp_plot_widget.save_to_csv(filename):
                    QMessageBox.information(self, "成功", f"数据已保存到: {filename}")
                else:
                    QMessageBox.warning(self, "错误", "保存数据失败")
        else:
            QMessageBox.warning(self, "提示", "没有数据可保存")
    
    def save_uart_plot_data(self):
        """保存UART绘图数据到CSV"""
        from PyQt5.QtWidgets import QFileDialog
        if self.uart_plot_widget and self.uart_plot_widget.data_buffer:
            filename, _ = QFileDialog.getSaveFileName(
                self, "保存UART数据", "", "CSV文件 (*.csv);;所有文件 (*)"
            )
            if filename:
                if self.uart_plot_widget.save_to_csv(filename):
                    QMessageBox.information(self, "成功", f"数据已保存到: {filename}")
                else:
                    QMessageBox.warning(self, "错误", "保存数据失败")
        else:
            QMessageBox.warning(self, "提示", "没有数据可保存")
    
    def on_tcp_channel_toggled(self, channel_index, state):
        """TCP通道显示切换"""
        if self.tcp_plot_widget:
            self.tcp_plot_widget.set_channel_visible(channel_index, state == 2)  # 2 = Qt.Checked
    
    def on_uart_channel_toggled(self, channel_index, state):
        """UART通道显示切换"""
        if self.uart_plot_widget:
            self.uart_plot_widget.set_channel_visible(channel_index, state == 2)  # 2 = Qt.Checked
    
    def on_sync_enable_toggled(self, checked):
        """同步模式开关处理"""
        self.sync_enabled = checked
        self.update_sync_status_indicator()
        if not checked:
            self.reset_sync()
    
    def auto_start_sync(self):
        """自动开始同步（连接时自动调用）"""
        # 直接启用同步模式
        self.sync_enabled = True
        
        # 检查是否有连接
        tcp_connected = self.tcp_thread and self.tcp_thread.isRunning()
        uart_connected = self.uart_thread and self.uart_thread.isRunning()
        
        if not tcp_connected and not uart_connected:
            return
        
        # 自动发送同步字符串
        if tcp_connected and self.sync_page:
            # 检查TCP发送开关是否开启
            if self.sync_page.tcp_send_enable_check.isChecked():
                tcp_sync_send = self.sync_page.tcp_sync_send_input.text().strip()
                if tcp_sync_send:
                    self.tcp_thread.send_data(tcp_sync_send + '\n')
                    self.add_tcp_display_text(f"[自动同步] 发送TCP同步字符串: {tcp_sync_send}")
                else:
                    self.add_tcp_display_text(f"[自动同步] TCP同步字符串为空，等待配置...")
            else:
                self.add_tcp_display_text(f"[自动同步] TCP发送同步已禁用，等待配置...")
        
        if uart_connected and self.sync_page:
            # 检查UART发送开关是否开启
            if self.sync_page.uart_send_enable_check.isChecked():
                uart_sync_send = self.sync_page.uart_sync_send_input.text().strip()
                if uart_sync_send:
                    self.uart_thread.send_data(uart_sync_send + '\n')
                    self.add_uart_display_text(f"[自动同步] 发送UART同步字符串: {uart_sync_send}")
                else:
                    self.add_uart_display_text(f"[自动同步] UART同步字符串为空，等待配置...")
            else:
                self.add_uart_display_text(f"[自动同步] UART发送同步已禁用，等待配置...")
        
        # 更新状态
        if self.sync_page:
            self.sync_page.update_sync_button("自动同步中...", False)
            self.sync_page.update_sync_status("无限等待同步确认...", "#f39c12")
        
        # 移除同步超时，改为死等
        # 不再设置超时定时器，同步将无限等待
        
        # 更新同步状态指示器
        self.update_sync_status_indicator()
    
    def start_sync_direct(self):
        """直接开始同步过程（不需要启用同步模式）"""
        # 直接启用同步模式
        self.sync_enabled = True
        self.update_sync_status_indicator()
        
        # 开始同步过程
        self.start_sync()
    
    def start_sync(self):
        """开始同步过程（手动点击同步按钮）"""
        if not self.sync_enabled:
            return
        
        # 重置同步状态
        self.tcp_sync_received = False
        self.uart_sync_received = False
        # 重置等待状态标志
        self.tcp_sync_waiting_displayed = False
        self.uart_sync_waiting_displayed = False
        
        # 检查连接状态
        tcp_connected = self.tcp_thread and self.tcp_thread.isRunning()
        uart_connected = self.uart_thread and self.uart_thread.isRunning()
        
        # 如果没有连接，只开启同步模式，等待连接
        if not tcp_connected and not uart_connected:
            if self.sync_page:
                self.sync_page.update_sync_button("同步已开启，等待连接...", False)
                self.sync_page.update_sync_status("同步模式已开启，等待TCP/UART连接...", "#f39c12")
            self.update_sync_status_indicator()
            return
        
        # 发送同步字符串
        if tcp_connected and self.sync_page:
            # 检查TCP发送开关是否开启
            if self.sync_page.tcp_send_enable_check.isChecked():
                tcp_sync_send = self.sync_page.tcp_sync_send_input.text().strip()
                if tcp_sync_send:
                    self.tcp_thread.send_data(tcp_sync_send + '\n')
                    self.add_tcp_display_text(f"[同步] 发送TCP同步字符串: {tcp_sync_send}")
                else:
                    self.add_tcp_display_text(f"[同步] TCP同步字符串为空，等待配置...")
            else:
                self.add_tcp_display_text(f"[同步] TCP发送同步已禁用，等待配置...")
        
        if uart_connected and self.sync_page:
            # 检查UART发送开关是否开启
            if self.sync_page.uart_send_enable_check.isChecked():
                uart_sync_send = self.sync_page.uart_sync_send_input.text().strip()
                if uart_sync_send:
                    self.uart_thread.send_data(uart_sync_send + '\n')
                    self.add_uart_display_text(f"[同步] 发送UART同步字符串: {uart_sync_send}")
                else:
                    self.add_uart_display_text(f"[同步] UART同步字符串为空，等待配置...")
            else:
                self.add_uart_display_text(f"[同步] UART发送同步已禁用，等待配置...")
        
        # 更新状态
        if self.sync_page:
            self.sync_page.update_sync_button("同步中...", False)
            self.sync_page.update_sync_status("无限等待同步确认...", "#f39c12")
        
        self.update_sync_status_indicator()
        
        # 移除同步超时，改为死等
        # 不再设置超时定时器，同步将无限等待
    
    def reset_sync(self):
        """重置同步状态"""
        self.tcp_sync_received = False
        self.uart_sync_received = False
        # 重置等待状态标志
        self.tcp_sync_waiting_displayed = False
        self.uart_sync_waiting_displayed = False
        
        if self.sync_timer:
            self.sync_timer.stop()
            self.sync_timer = None
        
        if self.sync_page:
            if self.sync_enabled:
                self.sync_page.update_sync_button("开始同步", True)
                self.sync_page.update_sync_status("同步模式已启用", "#f39c12")
            else:
                self.sync_page.update_sync_button("开始同步", False)
                self.sync_page.update_sync_status("同步未启用", "#7f8c8d")
        
        self.update_sync_status_indicator()
    
    def is_sync_info_configured(self):
        """检查同步信息是否已配置"""
        if not self.sync_page:
            return False
        
        # 检查是否至少有一个同步选项被启用
        tcp_send_enabled = self.sync_page.tcp_send_enable_check.isChecked()
        tcp_recv_enabled = self.sync_page.tcp_recv_enable_check.isChecked()
        uart_send_enabled = self.sync_page.uart_send_enable_check.isChecked()
        uart_recv_enabled = self.sync_page.uart_recv_enable_check.isChecked()
        
        # 检查TCP同步配置
        tcp_configured = False
        if tcp_send_enabled:
            tcp_send_text = self.sync_page.tcp_sync_send_input.text().strip()
            if tcp_send_text:
                tcp_configured = True
        if tcp_recv_enabled:
            tcp_recv_text = self.sync_page.tcp_sync_receive_input.text().strip()
            if tcp_recv_text:
                tcp_configured = True
        
        # 检查UART同步配置
        uart_configured = False
        if uart_send_enabled:
            uart_send_text = self.sync_page.uart_sync_send_input.text().strip()
            if uart_send_text:
                uart_configured = True
        if uart_recv_enabled:
            uart_recv_text = self.sync_page.uart_sync_receive_input.text().strip()
            if uart_recv_text:
                uart_configured = True
        
        # 至少需要配置一个同步选项
        return tcp_configured or uart_configured

    def update_sync_status(self):
        """更新同步状态"""
        # 现在总是更新同步状态
        
        # 检查是否所有连接的通道都收到同步确认
        tcp_connected = self.tcp_thread and self.tcp_thread.isRunning()
        uart_connected = self.uart_thread and self.uart_thread.isRunning()
        
        sync_complete = True
        
        # 检查TCP同步完成（只有在TCP连接且接收开关开启时才检查）
        if tcp_connected and self.sync_page and self.sync_page.tcp_recv_enable_check.isChecked():
            if not self.tcp_sync_received:
                sync_complete = False
        
        # 检查UART同步完成（只有在UART连接且接收开关开启时才检查）
        if uart_connected and self.sync_page and self.sync_page.uart_recv_enable_check.isChecked():
            if not self.uart_sync_received:
                sync_complete = False
        
        if sync_complete:
            # 同步完成
            if self.sync_timer:
                self.sync_timer.stop()
                self.sync_timer = None
            
            if self.sync_page:
                self.sync_page.update_sync_button("同步完成", False)
                self.sync_page.update_sync_status("同步完成 - 开始正常通信", "#27ae60")
            
            self.update_sync_status_indicator()
            
            # 显示同步完成消息
            if tcp_connected:
                self.add_tcp_display_text("[同步] TCP同步完成，开始正常通信")
            if uart_connected:
                self.add_uart_display_text("[同步] UART同步完成，开始正常通信")
    
    def send_sync_command(self):
        """发送同步指令给TCP和UART"""
        if not self.sync_page:
            return
        
        sync_command = self.sync_page.sync_command_input.text().strip()
        if not sync_command:
            QMessageBox.warning(self, "警告", "请输入同步指令!")
            return
        
        # 检查连接状态
        tcp_connected = self.tcp_thread and self.tcp_thread.isRunning()
        uart_connected = self.uart_thread and self.uart_thread.isRunning()
        
        if not tcp_connected and not uart_connected:
            QMessageBox.warning(self, "警告", "请先连接TCP或UART!")
            return
        
        # 发送给TCP
        if tcp_connected:
            if self.tcp_thread.send_data(sync_command + '\n'):
                self.add_tcp_display_text(f"[同步指令] 发送给TCP: {sync_command}")
            else:
                self.add_tcp_display_text(f"[同步指令] TCP发送失败: {sync_command}")
        
        # 发送给UART
        if uart_connected:
            if self.uart_thread.send_data(sync_command + '\n'):
                self.add_uart_display_text(f"[同步指令] 发送给UART: {sync_command}")
            else:
                self.add_uart_display_text(f"[同步指令] UART发送失败: {sync_command}")
        
        # 显示发送结果
        if tcp_connected and uart_connected:
            QMessageBox.information(self, "成功", f"同步指令已同时发送给TCP和UART: {sync_command}")
        elif tcp_connected:
            QMessageBox.information(self, "成功", f"同步指令已发送给TCP: {sync_command}")
        elif uart_connected:
            QMessageBox.information(self, "成功", f"同步指令已发送给UART: {sync_command}")

    def sync_timeout(self):
        """同步超时处理"""
        if not self.sync_enabled:
            return
        
        if self.sync_page:
            self.sync_page.update_sync_button("同步超时", True)
            self.sync_page.update_sync_status("同步超时 - 请重试", "#e74c3c")
        
        self.update_sync_status_indicator()
        
        # 显示超时消息
        if self.tcp_thread and self.tcp_thread.isRunning():
            self.add_tcp_display_text("[同步] TCP同步超时")
        if self.uart_thread and self.uart_thread.isRunning():
            self.add_uart_display_text("[同步] UART同步超时")
    
    def closeEvent(self, event):
        """关闭事件处理"""
        if self.tcp_thread and self.tcp_thread.isRunning():
            self.tcp_thread.stop()
        if self.uart_thread and self.uart_thread.isRunning():
            self.uart_thread.stop()
        event.accept()


def main():
    """主函数"""
    app = QApplication(sys.argv)
    window = TCPUARTUnifiedApp()
    window.show()
    sys.exit(app.exec_())


if __name__ == '__main__':
    main()

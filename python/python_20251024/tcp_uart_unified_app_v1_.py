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
from datetime import datetime
from PyQt5.QtWidgets import (QApplication, QMainWindow, QWidget, QVBoxLayout, 
                             QHBoxLayout, QLabel, QPushButton, QLineEdit, 
                             QTextEdit, QComboBox, QGroupBox, QGridLayout,
                             QMessageBox, QSpinBox, QCheckBox, QSplitter)
from PyQt5.QtCore import Qt, QThread, pyqtSignal, QTimer
from PyQt5.QtGui import QFont, QTextCursor


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
                    self.data_received.emit(f"[{timestamp}] UART接收: {data.decode('utf-8').strip()}")
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
        
    def set_mode(self, mode):
        """设置模式：client 或 server"""
        self.mode = mode
        
    def start_client(self, host, port):
        """启动客户端"""
        try:
            self.socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
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
                    timestamp = datetime.now().strftime("%H:%M:%S")
                    self.data_received.emit(f"[{timestamp}] TCP接收: {data.decode('utf-8')}")
                else:
                    break
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
                self.status_changed.emit(f"客户端已连接: {addr[0]}:{addr[1]}")
                self.connection_changed.emit(True)
                
                # 处理客户端数据
                while self.server_running:
                    data = self.client_socket.recv(1024)
                    if data:
                        timestamp = datetime.now().strftime("%H:%M:%S")
                        self.data_received.emit(f"[{timestamp}] TCP接收: {data.decode('utf-8')}")
                    else:
                        break
                        
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
        if self.socket:
            self.socket.close()
        if self.client_socket:
            self.client_socket.close()
        self.quit()
        self.wait()


class TCPUARTUnifiedApp(QMainWindow):
    """TCP/UART统一通信应用主窗口"""
    
    def __init__(self):
        super().__init__()
        self.tcp_thread = None
        self.uart_thread = None
        self.init_ui()
        
    def init_ui(self):
        """初始化用户界面"""
        self.setWindowTitle('TCP/UART统一通信应用')
        self.setGeometry(100, 100, 1200, 800)
        
        # 创建中央widget
        central_widget = QWidget()
        self.setCentralWidget(central_widget)
        
        # 创建主布局
        main_layout = QVBoxLayout()
        central_widget.setLayout(main_layout)
        
        # 创建水平分割器
        splitter = QSplitter(Qt.Horizontal)
        main_layout.addWidget(splitter)
        
        # 左侧：TCP通信区域（包含设置和显示）
        tcp_widget = QWidget()
        tcp_widget.setMinimumWidth(800)  # 设置最小宽度
        # tcp_widget.setMaximumWidth(800)  # 设置最大宽度
        self.init_tcp_section(tcp_widget)
        splitter.addWidget(tcp_widget)
        
        # 右侧：UART通信区域（包含设置和显示）
        uart_widget = QWidget()
        uart_widget.setMinimumWidth(800)  # 设置最小宽度
        # uart_widget.setMaximumWidth(800)  # 设置最大宽度
        self.init_uart_section(uart_widget)
        splitter.addWidget(uart_widget)
        
        # 设置分割器比例
        splitter.setSizes([600, 600])
        
    def init_tcp_section(self, parent):
        """初始化TCP通信区域"""
        layout = QVBoxLayout()
        parent.setLayout(layout)
        
        # TCP标题
        tcp_title = QLabel("TCP通信")
        tcp_title.setStyleSheet("font-size: 32px; font-weight: bold; color: #2c3e50; padding: 10px;")
        tcp_title.setAlignment(Qt.AlignCenter)
        tcp_title.setFixedHeight(60)  # 增加标题高度
        layout.addWidget(tcp_title)
        
        # 模式选择
        mode_group = QGroupBox("模式选择")
        mode_layout = QHBoxLayout()
        
        self.tcp_mode_combo = QComboBox()
        self.tcp_mode_combo.addItems(['客户端', '服务端'])
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
        send_group.setFixedHeight(240)  # 设置最小高度
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
        tcp_quick_buttons = ["Hello", "Test", "Ping", "Data"]
        for text in tcp_quick_buttons:
            btn = QPushButton(text)
            btn.clicked.connect(lambda checked, t=text: self.quick_send_tcp(t))
            btn.setEnabled(False)
            tcp_quick_send_layout.addWidget(btn)
        
        send_layout.addLayout(send_input_layout)
        send_layout.addLayout(send_options_layout)
        send_layout.addLayout(tcp_quick_send_layout)
        
        send_group.setLayout(send_layout)
        layout.addWidget(send_group)
        
        # TCP数据转发选项
        tcp_forward_group = QGroupBox("数据转发")
        tcp_forward_group.setMinimumHeight(120)  # 设置最小高度
        tcp_forward_group.setMaximumHeight(120)  # 设置最大高度
        tcp_forward_layout = QVBoxLayout()
        
        self.tcp_to_uart_check = QCheckBox("TCP接收数据转发到UART")
        self.tcp_to_uart_check.setChecked(False)
        tcp_forward_layout.addWidget(self.tcp_to_uart_check)
        
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
        
        # 初始化界面状态
        self.on_tcp_mode_changed("客户端")
        
    def init_uart_section(self, parent):
        """初始化UART通信区域"""
        layout = QVBoxLayout()
        parent.setLayout(layout)
        
        # UART标题
        uart_title = QLabel("UART通信")
        uart_title.setStyleSheet("font-size: 32px; font-weight: bold; color: #2c3e50; padding: 10px;")
        uart_title.setAlignment(Qt.AlignCenter)
        uart_title.setFixedHeight(60)  # 增加标题高度
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
        uart_send_group.setFixedHeight(240)  # 设置最小高度
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
        uart_quick_buttons = ["AT", "AT+VERSION", "AT+STATUS", "PING"]
        for text in uart_quick_buttons:
            btn = QPushButton(text)
            btn.clicked.connect(lambda checked, t=text: self.quick_send_uart(t))
            btn.setEnabled(False)
            uart_quick_send_layout.addWidget(btn)
        
        uart_send_layout.addLayout(uart_send_input_layout)
        uart_send_layout.addLayout(uart_send_options_layout)
        uart_send_layout.addLayout(uart_quick_send_layout)
        
        uart_send_group.setLayout(uart_send_layout)
        layout.addWidget(uart_send_group)
        
        # UART数据转发选项
        uart_forward_group = QGroupBox("数据转发")
        uart_forward_group.setMinimumHeight(120)  # 设置最小高度
        uart_forward_group.setMaximumHeight(120)  # 设置最大高度
        uart_forward_layout = QVBoxLayout()
        
        self.uart_to_tcp_check = QCheckBox("UART接收数据转发到TCP")
        self.uart_to_tcp_check.setChecked(False)
        uart_forward_layout.addWidget(self.uart_to_tcp_check)
        
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
        self.add_tcp_display_text(data)
        
        # 如果启用了TCP到UART转发
        if self.tcp_to_uart_check.isChecked() and self.uart_thread and self.uart_thread.isRunning():
            # 提取数据内容（去掉时间戳和标签）
            if "TCP接收:" in data:
                content = data.split("TCP接收: ")[1]
                if self.uart_thread.send_data(content):
                    self.add_uart_display_text(f"[{datetime.now().strftime('%H:%M:%S')}] TCP→UART: {content}")
    
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
        else:
            self.tcp_connect_button.setText("连接" if self.tcp_mode_combo.currentText() == "客户端" else "启动服务")
    
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
        self.add_uart_display_text(data)
        
        # 如果启用了UART到TCP转发
        if self.uart_to_tcp_check.isChecked() and self.tcp_thread and self.tcp_thread.isRunning():
            # 提取数据内容（去掉时间戳和标签）
            if "UART接收:" in data:
                content = data.split("UART接收: ")[1]
                if self.tcp_thread.send_data(content):
                    self.add_tcp_display_text(f"[{datetime.now().strftime('%H:%M:%S')}] UART→TCP: {content}")
    
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
        else:
            self.uart_connect_button.setText("连接串口")
    
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

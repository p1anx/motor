#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
TCP客户端/服务端切换应用
支持实时数据收发和显示
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
                             QMessageBox, QSpinBox, QCheckBox, QTabWidget)
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
                    self.data_received.emit(f"[{timestamp}] 接收: {data.decode('utf-8')}")
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
                        self.data_received.emit(f"[{timestamp}] 接收: {data.decode('utf-8')}")
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


class TCPClientServerApp(QMainWindow):
    """TCP客户端/服务端应用主窗口"""
    
    def __init__(self):
        super().__init__()
        self.tcp_thread = None
        self.uart_thread = None
        self.init_ui()
        
    def init_ui(self):
        """初始化用户界面"""
        self.setWindowTitle('TCP/UART桥接应用')
        self.setGeometry(100, 100, 1000, 700)
        
        # 创建中央widget
        central_widget = QWidget()
        self.setCentralWidget(central_widget)
        
        # 创建主布局
        main_layout = QVBoxLayout()
        central_widget.setLayout(main_layout)
        
        # 创建标签页
        self.tab_widget = QTabWidget()
        main_layout.addWidget(self.tab_widget)
        
        # TCP标签页
        self.tcp_tab = QWidget()
        self.tab_widget.addTab(self.tcp_tab, "TCP通信")
        self.init_tcp_tab()
        
        # UART标签页
        self.uart_tab = QWidget()
        self.tab_widget.addTab(self.uart_tab, "UART通信")
        self.init_uart_tab()
        
        # 数据转发标签页
        self.bridge_tab = QWidget()
        self.tab_widget.addTab(self.bridge_tab, "数据转发")
        self.init_bridge_tab()
        
    def init_tcp_tab(self):
        """初始化TCP标签页"""
        layout = QVBoxLayout()
        self.tcp_tab.setLayout(layout)
        
        # 创建模式选择区域
        mode_group = QGroupBox("模式选择")
        mode_layout = QHBoxLayout()
        
        self.mode_combo = QComboBox()
        self.mode_combo.addItems(['客户端', '服务端'])
        self.mode_combo.currentTextChanged.connect(self.on_mode_changed)
        mode_layout.addWidget(QLabel("工作模式:"))
        mode_layout.addWidget(self.mode_combo)
        mode_layout.addStretch()
        
        mode_group.setLayout(mode_layout)
        layout.addWidget(mode_group)
        
        # 创建连接配置区域
        config_group = QGroupBox("连接配置")
        config_layout = QGridLayout()
        
        # 主机地址（仅客户端使用）
        self.host_label = QLabel("主机地址:")
        self.host_input = QLineEdit()
        self.host_input.setText("127.0.0.1")
        self.host_input.setPlaceholderText("输入服务器IP地址")
        
        # 端口号
        self.port_label = QLabel("端口号:")
        self.port_input = QSpinBox()
        self.port_input.setRange(1, 65535)
        self.port_input.setValue(8080)
        
        # 连接按钮
        self.connect_button = QPushButton("连接")
        self.connect_button.clicked.connect(self.toggle_connection)
        
        # 状态显示
        self.status_label = QLabel("未连接")
        self.status_label.setStyleSheet("color: red; font-weight: bold;")
        
        config_layout.addWidget(self.host_label, 0, 0)
        config_layout.addWidget(self.host_input, 0, 1)
        config_layout.addWidget(self.port_label, 0, 2)
        config_layout.addWidget(self.port_input, 0, 3)
        config_layout.addWidget(self.connect_button, 0, 4)
        config_layout.addWidget(self.status_label, 1, 0, 1, 5)
        
        config_group.setLayout(config_layout)
        layout.addWidget(config_group)
        
        # 创建数据发送区域
        send_group = QGroupBox("数据发送")
        send_layout = QVBoxLayout()
        
        send_input_layout = QHBoxLayout()
        self.send_input = QLineEdit()
        self.send_input.setPlaceholderText("输入要发送的数据...")
        self.send_input.returnPressed.connect(self.send_data)
        
        self.send_button = QPushButton("发送")
        self.send_button.clicked.connect(self.send_data)
        self.send_button.setEnabled(False)
        
        send_input_layout.addWidget(self.send_input)
        send_input_layout.addWidget(self.send_button)
        
        # 发送选项
        send_options_layout = QHBoxLayout()
        self.add_newline_check = QCheckBox("发送时添加换行符")
        self.add_newline_check.setChecked(True)
        send_options_layout.addWidget(self.add_newline_check)
        send_options_layout.addStretch()
        
        # 快速发送按钮
        quick_send_layout = QHBoxLayout()
        quick_buttons = ["Hello", "Test", "Ping", "Data"]
        for text in quick_buttons:
            btn = QPushButton(text)
            btn.clicked.connect(lambda checked, t=text: self.quick_send(t))
            btn.setEnabled(False)
            quick_send_layout.addWidget(btn)
        
        send_layout.addLayout(send_input_layout)
        send_layout.addLayout(send_options_layout)
        send_layout.addLayout(quick_send_layout)
        
        send_group.setLayout(send_layout)
        layout.addWidget(send_group)
        
        # 创建数据显示区域
        data_group = QGroupBox("数据接收")
        data_layout = QVBoxLayout()
        
        # 控制按钮
        control_layout = QHBoxLayout()
        self.clear_button = QPushButton("清空显示")
        self.clear_button.clicked.connect(self.clear_display)
        
        self.auto_scroll_check = QCheckBox("自动滚动")
        self.auto_scroll_check.setChecked(True)
        
        control_layout.addWidget(self.clear_button)
        control_layout.addWidget(self.auto_scroll_check)
        control_layout.addStretch()
        
        # 数据显示区域
        self.data_display = QTextEdit()
        self.data_display.setReadOnly(True)
        self.data_display.setFont(QFont("Consolas", 10))
        
        data_layout.addLayout(control_layout)
        data_layout.addWidget(self.data_display)
        
        data_group.setLayout(data_layout)
        layout.addWidget(data_group)
        
        # 初始化界面状态
        self.on_mode_changed("客户端")
        
    def init_uart_tab(self):
        """初始化UART标签页"""
        layout = QVBoxLayout()
        self.uart_tab.setLayout(layout)
        
        # 串口配置区域
        uart_config_group = QGroupBox("串口配置")
        uart_config_layout = QGridLayout()
        
        # 串口选择
        self.uart_port_combo = QComboBox()
        self.refresh_ports_button = QPushButton("刷新端口")
        self.refresh_ports_button.clicked.connect(self.refresh_uart_ports)
        
        # 波特率选择
        self.baudrate_combo = QComboBox()
        self.baudrate_combo.addItems(['9600', '19200', '38400', '57600', '115200', '230400', '460800', '921600'])
        self.baudrate_combo.setCurrentText('115200')
        
        # 连接按钮
        self.uart_connect_button = QPushButton("连接串口")
        self.uart_connect_button.clicked.connect(self.toggle_uart_connection)
        
        # 状态显示
        self.uart_status_label = QLabel("串口未连接")
        self.uart_status_label.setStyleSheet("color: red; font-weight: bold;")
        
        uart_config_layout.addWidget(QLabel("串口:"), 0, 0)
        uart_config_layout.addWidget(self.uart_port_combo, 0, 1)
        uart_config_layout.addWidget(self.refresh_ports_button, 0, 2)
        uart_config_layout.addWidget(QLabel("波特率:"), 0, 3)
        uart_config_layout.addWidget(self.baudrate_combo, 0, 4)
        uart_config_layout.addWidget(self.uart_connect_button, 0, 5)
        uart_config_layout.addWidget(self.uart_status_label, 1, 0, 1, 6)
        
        uart_config_group.setLayout(uart_config_layout)
        layout.addWidget(uart_config_group)
        
        # UART数据发送区域
        uart_send_group = QGroupBox("UART数据发送")
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
        
        # UART数据显示区域
        uart_data_group = QGroupBox("UART数据接收")
        uart_data_layout = QVBoxLayout()
        
        # 控制按钮
        uart_control_layout = QHBoxLayout()
        self.uart_clear_button = QPushButton("清空显示")
        self.uart_clear_button.clicked.connect(self.clear_uart_display)
        
        self.uart_auto_scroll_check = QCheckBox("自动滚动")
        self.uart_auto_scroll_check.setChecked(True)
        
        uart_control_layout.addWidget(self.uart_clear_button)
        uart_control_layout.addWidget(self.uart_auto_scroll_check)
        uart_control_layout.addStretch()
        
        # UART数据显示区域
        self.uart_data_display = QTextEdit()
        self.uart_data_display.setReadOnly(True)
        self.uart_data_display.setFont(QFont("Consolas", 10))
        
        uart_data_layout.addLayout(uart_control_layout)
        uart_data_layout.addWidget(self.uart_data_display)
        
        uart_data_group.setLayout(uart_data_layout)
        layout.addWidget(uart_data_group)
        
        # 初始化串口列表
        self.refresh_uart_ports()
        
    def init_bridge_tab(self):
        """初始化数据转发标签页"""
        layout = QVBoxLayout()
        self.bridge_tab.setLayout(layout)
        
        # 转发配置区域
        bridge_config_group = QGroupBox("数据转发配置")
        bridge_config_layout = QVBoxLayout()
        
        # 转发选项
        forward_options_layout = QHBoxLayout()
        self.uart_to_tcp_check = QCheckBox("UART → TCP 转发")
        self.tcp_to_uart_check = QCheckBox("TCP → UART 转发")
        forward_options_layout.addWidget(self.uart_to_tcp_check)
        forward_options_layout.addWidget(self.tcp_to_uart_check)
        forward_options_layout.addStretch()
        
        # 转发状态
        self.forward_status_label = QLabel("转发功能未启用")
        self.forward_status_label.setStyleSheet("color: orange; font-weight: bold;")
        
        bridge_config_layout.addLayout(forward_options_layout)
        bridge_config_layout.addWidget(self.forward_status_label)
        
        bridge_config_group.setLayout(bridge_config_layout)
        layout.addWidget(bridge_config_group)
        
        # 转发数据显示
        bridge_data_group = QGroupBox("转发数据显示")
        bridge_data_layout = QVBoxLayout()
        
        # 控制按钮
        bridge_control_layout = QHBoxLayout()
        self.bridge_clear_button = QPushButton("清空显示")
        self.bridge_clear_button.clicked.connect(self.clear_bridge_display)
        
        bridge_control_layout.addWidget(self.bridge_clear_button)
        bridge_control_layout.addStretch()
        
        # 转发数据显示区域
        self.bridge_data_display = QTextEdit()
        self.bridge_data_display.setReadOnly(True)
        self.bridge_data_display.setFont(QFont("Consolas", 10))
        
        bridge_data_layout.addLayout(bridge_control_layout)
        bridge_data_layout.addWidget(self.bridge_data_display)
        
        bridge_data_group.setLayout(bridge_data_layout)
        layout.addWidget(bridge_data_group)
        
    def on_mode_changed(self, mode):
        """模式切换处理"""
        is_client = (mode == "客户端")
        self.host_label.setVisible(is_client)
        self.host_input.setVisible(is_client)
        
        if is_client:
            self.connect_button.setText("连接")
            self.port_label.setText("服务器端口:")
        else:
            self.connect_button.setText("启动服务")
            self.port_label.setText("监听端口:")
    
    def toggle_connection(self):
        """切换连接状态"""
        if self.tcp_thread and self.tcp_thread.isRunning():
            self.disconnect()
        else:
            self.connect()
    
    def connect(self):
        """建立连接"""
        mode = self.mode_combo.currentText()
        port = self.port_input.value()
        
        if mode == "客户端":
            host = self.host_input.text().strip()
            if not host:
                QMessageBox.warning(self, "警告", "请输入主机地址!")
                return
            self.start_client(host, port)
        else:
            self.start_server(port)
    
    def start_client(self, host, port):
        """启动客户端"""
        self.tcp_thread = TCPThread('client')
        self.tcp_thread.data_received.connect(self.on_data_received)
        self.tcp_thread.status_changed.connect(self.on_status_changed)
        self.tcp_thread.connection_changed.connect(self.on_connection_changed)
        self.tcp_thread.start_client(host, port)
    
    def start_server(self, port):
        """启动服务端"""
        self.tcp_thread = TCPThread('server')
        self.tcp_thread.data_received.connect(self.on_data_received)
        self.tcp_thread.status_changed.connect(self.on_status_changed)
        self.tcp_thread.connection_changed.connect(self.on_connection_changed)
        self.tcp_thread.start_server(port)
    
    def disconnect(self):
        """断开连接"""
        if self.tcp_thread:
            self.tcp_thread.stop()
            self.tcp_thread = None
    
    def send_data(self):
        """发送数据"""
        data = self.send_input.text()
        if data and self.tcp_thread and self.tcp_thread.isRunning():
            # 根据复选框决定是否添加换行符
            if self.add_newline_check.isChecked():
                data_to_send = data + '\n'
            else:
                data_to_send = data
                
            if self.tcp_thread.send_data(data_to_send):
                timestamp = datetime.now().strftime("%H:%M:%S")
                display_data = data  # 显示时不包含换行符
                self.add_display_text(f"[{timestamp}] 发送: {display_data}")
                self.send_input.clear()
    
    def quick_send(self, text):
        """快速发送"""
        self.send_input.setText(text)
        self.send_data()
    
    def on_data_received(self, data):
        """处理接收到的数据"""
        self.add_display_text(data)
        
        # 如果启用了TCP到UART转发
        if self.tcp_to_uart_check.isChecked() and self.uart_thread and self.uart_thread.isRunning():
            # 提取数据内容（去掉时间戳和标签）
            if "接收:" in data:
                content = data.split("接收: ")[1]
                if self.uart_thread.send_data(content):
                    self.add_bridge_display_text(f"[{datetime.now().strftime('%H:%M:%S')}] TCP→UART: {content}")
    
    def on_status_changed(self, status):
        """处理状态变化"""
        self.status_label.setText(status)
        if "连接" in status or "启动" in status:
            self.status_label.setStyleSheet("color: green; font-weight: bold;")
        else:
            self.status_label.setStyleSheet("color: red; font-weight: bold;")
    
    def on_connection_changed(self, connected):
        """处理连接状态变化"""
        self.send_button.setEnabled(connected)
        self.send_input.setEnabled(connected)
        
        # 启用/禁用快速发送按钮
        for i in range(4):  # 4个快速发送按钮
            widget = self.findChild(QPushButton)
            if widget and widget.text() in ["Hello", "Test", "Ping", "Data"]:
                widget.setEnabled(connected)
        
        if connected:
            self.connect_button.setText("断开")
        else:
            self.connect_button.setText("连接" if self.mode_combo.currentText() == "客户端" else "启动服务")
    
    def add_display_text(self, text):
        """添加显示文本"""
        self.data_display.append(text)
        if self.auto_scroll_check.isChecked():
            cursor = self.data_display.textCursor()
            cursor.movePosition(QTextCursor.End)
            self.data_display.setTextCursor(cursor)
    
    def clear_display(self):
        """清空显示"""
        self.data_display.clear()
    
    def refresh_uart_ports(self):
        """刷新串口列表"""
        self.uart_port_combo.clear()
        ports = serial.tools.list_ports.comports()
        for port in ports:
            self.uart_port_combo.addItem(f"{port.device} - {port.description}")
    
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
        baudrate = int(self.baudrate_combo.currentText())
        
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
                    self.add_bridge_display_text(f"[{datetime.now().strftime('%H:%M:%S')}] UART→TCP: {content}")
    
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
            widget = self.uart_tab.findChild(QPushButton)
            if widget and widget.text() in ["AT", "AT+VERSION", "AT+STATUS", "PING"]:
                widget.setEnabled(connected)
        
        if connected:
            self.uart_connect_button.setText("断开串口")
        else:
            self.uart_connect_button.setText("连接串口")
    
    def add_uart_display_text(self, text):
        """添加UART显示文本"""
        self.uart_data_display.append(text)
        if self.uart_auto_scroll_check.isChecked():
            cursor = self.uart_data_display.textCursor()
            cursor.movePosition(QTextCursor.End)
            self.uart_data_display.setTextCursor(cursor)
    
    def clear_uart_display(self):
        """清空UART显示"""
        self.uart_data_display.clear()
    
    def add_bridge_display_text(self, text):
        """添加转发显示文本"""
        self.bridge_data_display.append(text)
    
    def clear_bridge_display(self):
        """清空转发显示"""
        self.bridge_data_display.clear()
    
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
    window = TCPClientServerApp()
    window.show()
    sys.exit(app.exec_())


if __name__ == '__main__':
    main()

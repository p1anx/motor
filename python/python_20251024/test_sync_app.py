#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
测试同步功能的简单脚本
"""

import sys
import time
import socket
import threading
from tcp_uart_unified_app_main_v2 import TCPUARTUnifiedApp
from PyQt5.QtWidgets import QApplication

def create_test_server():
    """创建测试TCP服务器"""
    server = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    server.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
    server.bind(('localhost', 1123))
    server.listen(1)
    
    def handle_client():
        while True:
            try:
                client, addr = server.accept()
                print(f"测试服务器: 客户端连接 {addr}")
                
                while True:
                    data = client.recv(1024)
                    if not data:
                        break
                    
                    message = data.decode('utf-8').strip()
                    print(f"测试服务器收到: {message}")
                    
                    # 模拟同步响应
                    if "SYNC_SEND" in message:
                        client.send(b"SYNC_RECV\n")
                        print("测试服务器发送同步确认")
                    elif "Hello" in message:
                        client.send(b"Hello Response\n")
                        print("测试服务器发送Hello响应")
                
                client.close()
            except Exception as e:
                print(f"测试服务器错误: {e}")
                break
    
    thread = threading.Thread(target=handle_client, daemon=True)
    thread.start()
    return server

def main():
    """主函数"""
    print("启动TCP/UART同步应用测试...")
    
    # 创建测试服务器
    test_server = create_test_server()
    print("测试TCP服务器已启动 (localhost:1123)")
    
    # 创建应用
    app = QApplication(sys.argv)
    window = TCPUARTUnifiedApp()
    window.show()
    
    print("\n=== 使用说明 ===")
    print("1. 启用同步模式复选框")
    print("2. 配置同步字符串（默认: SYNC_SEND/SYNC_RECV）")
    print("3. 连接TCP客户端到 localhost:1123")
    print("4. 点击'开始同步'按钮")
    print("5. 观察同步过程和数据收发控制")
    print("\n=== 测试步骤 ===")
    print("1. 勾选'启用同步模式'")
    print("2. 设置TCP模式为'客户端'")
    print("3. 点击'连接'连接到测试服务器")
    print("4. 点击'开始同步'")
    print("5. 尝试发送数据，观察同步控制效果")
    
    try:
        sys.exit(app.exec_())
    finally:
        test_server.close()

if __name__ == '__main__':
    main()

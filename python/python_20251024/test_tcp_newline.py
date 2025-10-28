#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
测试TCP应用换行符功能
"""

import socket
import threading
import time

def test_server():
    """测试服务端"""
    server = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    server.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
    server.bind(('localhost', 8080))
    server.listen(1)
    print("服务端启动，监听端口 8080")
    
    while True:
        client, addr = server.accept()
        print(f"客户端连接: {addr}")
        
        while True:
            data = client.recv(1024)
            if not data:
                break
            print(f"接收数据: {repr(data)}")  # 使用repr显示原始数据
            
        client.close()

def test_client():
    """测试客户端"""
    time.sleep(1)  # 等待服务端启动
    
    client = socket.socket(socket.AF_INET, socket.SOCKET_STREAM)
    client.connect(('localhost', 8080))
    print("客户端连接成功")
    
    # 测试发送带换行符的数据
    client.send(b"Hello with newline\n")
    time.sleep(0.1)
    
    # 测试发送不带换行符的数据
    client.send(b"Hello without newline")
    time.sleep(0.1)
    
    client.close()
    print("客户端断开")

if __name__ == '__main__':
    # 启动服务端线程
    server_thread = threading.Thread(target=test_server)
    server_thread.daemon = True
    server_thread.start()
    
    # 启动客户端
    test_client()

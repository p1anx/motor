#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
简单的TCP服务器实现
功能：监听客户端连接，接收消息并回显给客户端
"""

import socket  # 导入socket模块，用于网络通信
import time

if __name__ == '__main__':
    # 服务器配置
    host = '0.0.0.0'  # 监听所有网络接口（0.0.0.0表示监听所有可用的网络接口）
    port = 1123       # 服务器监听端口号
    
    # 创建TCP socket
    server_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    # AF_INET: 使用IPv4地址族
    # SOCK_STREAM: 使用TCP协议（面向连接的可靠传输）
    
    # 设置socket选项，允许地址重用
    server_socket.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
    # SO_REUSEADDR: 允许在TIME_WAIT状态下重用地址，避免"Address already in use"错误
    
    # 绑定服务器地址和端口
    server_socket.bind((host, port))
    # 将socket绑定到指定的主机和端口上
    
    # 开始监听连接请求
    server_socket.listen(10)
    # 参数10表示最多允许10个连接在队列中等待accept()
    # 超过这个数量的连接会被拒绝
    
    print("listening.....")  # 打印服务器启动信息

    try:
        # 主服务器循环 - 持续接受新的客户端连接
        while True:
            # 接受客户端连接请求
            client_socket, client_address = server_socket.accept()
            # accept()是阻塞调用，会等待客户端连接
            # client_socket: 与客户端通信的socket对象
            # client_address: 客户端的地址信息 (IP地址, 端口号)
            
            print(f"{client_address}")  # 打印客户端连接信息
            
            # 客户端通信循环 - 处理单个客户端的所有消息
            while True:
                # 从客户端接收数据
                send_str = 'enable_wifi\n'
                client_socket.send(send_str.encode('utf-8'))

                data = client_socket.recv(1024)
                # recv(1024): 一次最多接收1024字节的数据
                # 这也是阻塞调用，会等待客户端发送数据
                
                # 检查是否接收到数据
                if not data:
                    break# 如果没有数据，继续循环等待
                    # 注意：这里使用continue而不是break，可能是个逻辑错误
                    # 通常当data为空时，表示客户端断开连接，应该break退出循环
                
                # 解码接收到的数据
                msg = data.decode('utf-8').strip()
                # decode('utf-8'): 将字节数据转换为UTF-8编码的字符串
                # strip(): 去除字符串两端的空白字符（空格、换行等）
                
                print(f'recv: {msg}\n')  # 打印接收到的消息
                
                # 构造回显消息
                reponse = f"recv: {msg}\n"
                # 注意：变量名"reponse"应该是"response"的拼写错误
                send_str = 'enable_wifi\n'
                
                # 发送回显消息给客户端
                # client_socket.send(reponse.encode('utf-8'))
                client_socket.send(send_str.encode('utf-8'))
                
                # encode('utf-8'): 将字符串转换为UTF-8编码的字节数据
                # send(): 发送数据给客户端
                
                # 如果客户端发送"quit"，断开连接
                if msg.lower() == 'quit':
                    break  # 退出客户端通信循环，关闭这个客户端连接
            
    except KeyboardInterrupt:
        # 捕获Ctrl+C中断信号
        server_socket.close()  # 关闭服务器socket
        print("Server stopped")  # 打印服务器停止信息
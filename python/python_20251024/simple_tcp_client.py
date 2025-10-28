#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
简单的TCP客户端示例
"""

import socket
import threading
import time

class SimpleTCPClient:
    def __init__(self, host='localhost', port=8080):
        self.host = host
        self.port = port
        self.client_socket = None
        self.connected = False
        
    def connect(self):
        """连接到服务器"""
        try:
            # 创建socket
            self.client_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
            
            # 连接到服务器
            self.client_socket.connect((self.host, self.port))
            self.connected = True
            
            print(f"成功连接到服务器 {self.host}:{self.port}")
            
            # 启动接收消息的线程
            receive_thread = threading.Thread(target=self.receive_messages)
            receive_thread.daemon = True
            receive_thread.start()
            
            return True
            
        except Exception as e:
            print(f"连接服务器失败: {e}")
            return False
    
    def receive_messages(self):
        """接收服务器消息"""
        try:
            while self.connected:
                data = self.client_socket.recv(1024)
                if not data:
                    break
                
                message = data.decode('utf-8')
                print(f"收到服务器消息: {message}")
                
        except Exception as e:
            if self.connected:
                print(f"接收消息时出错: {e}")
        finally:
            self.disconnect()
    
    def send_message(self, message):
        """发送消息到服务器"""
        if not self.connected:
            print("未连接到服务器")
            return False
            
        try:
            self.client_socket.send(message.encode('utf-8'))
            return True
        except Exception as e:
            print(f"发送消息失败: {e}")
            return False
    
    def disconnect(self):
        """断开连接"""
        self.connected = False
        if self.client_socket:
            self.client_socket.close()
        print("已断开连接")

def main():
    """主函数 - 交互式客户端"""
    client = SimpleTCPClient(host='localhost', port=8080)
    
    if not client.connect():
        return
    
    print("\n=== TCP客户端 ===")
    print("输入消息发送给服务器，输入 'quit' 退出")
    print("=" * 30)
    
    try:
        while client.connected:
            message = input("请输入消息: ")
            if message.lower() == 'quit':
                client.send_message('quit')
                break
            
            if message.strip():
                client.send_message(message)
                
    except KeyboardInterrupt:
        print("\n客户端被用户中断")
    finally:
        client.disconnect()

def send_test_messages():
    """发送测试消息的示例"""
    client = SimpleTCPClient(host='localhost', port=8080)
    
    if not client.connect():
        return
    
    # 发送一些测试消息
    test_messages = [
        "Hello Server!",
        "How are you?",
        "This is a test message",
        "quit"  # 最后发送quit断开连接
    ]
    
    for message in test_messages:
        print(f"发送: {message}")
        client.send_message(message)
        time.sleep(1)  # 等待1秒
    
    client.disconnect()

if __name__ == "__main__":
    # 选择运行模式
    print("选择运行模式:")
    print("1. 交互式客户端")
    print("2. 发送测试消息")
    
    choice = input("请输入选择 (1 或 2): ").strip()
    
    if choice == '2':
        send_test_messages()
    else:
        main()

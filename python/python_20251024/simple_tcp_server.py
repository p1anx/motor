#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
简单的TCP服务器示例
"""

import socket
import threading
import time

class SimpleTCPServer:
    def __init__(self, host='localhost', port=8080):
        self.host = host
        self.port = port
        self.server_socket = None
        self.running = False
        
    def start(self):
        """启动TCP服务器"""
        try:
            # 创建socket
            self.server_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
            self.server_socket.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
            
            # 绑定地址和端口
            self.server_socket.bind((self.host, self.port))
            self.server_socket.listen(5)
            
            print(f"TCP服务器启动成功，监听 {self.host}:{self.port}")
            print("等待客户端连接...")
            
            self.running = True
            
            while self.running:
                try:
                    # 接受客户端连接
                    client_socket, client_address = self.server_socket.accept()
                    print(f"客户端连接: {client_address}")
                    
                    # 为每个客户端创建处理线程
                    client_thread = threading.Thread(
                        target=self.handle_client,
                        args=(client_socket, client_address)
                    )
                    client_thread.daemon = True
                    client_thread.start()
                    
                except socket.error:
                    if self.running:
                        print("接受连接时出错")
                        continue
                        
        except Exception as e:
            print(f"服务器启动失败: {e}")
        finally:
            self.stop()
    
    def handle_client(self, client_socket, client_address):
        """处理客户端连接"""
        try:
            while self.running:
                # 接收数据
                data = client_socket.recv(1024)
                if not data:
                    break
                
                # 解码数据
                message = data.decode('utf-8').strip()
                print(f"收到来自 {client_address} 的消息: {message}")
                
                # 回显消息
                response = f"服务器收到: {message}"
                client_socket.send(response.encode('utf-8'))
                
                # 如果客户端发送"quit"，断开连接
                if message.lower() == 'quit':
                    break
                    
        except Exception as e:
            print(f"处理客户端 {client_address} 时出错: {e}")
        finally:
            client_socket.close()
            print(f"客户端 {client_address} 断开连接")
    
    def stop(self):
        """停止服务器"""
        self.running = False
        if self.server_socket:
            self.server_socket.close()
        print("服务器已停止")

def main():
    """主函数"""
    server = SimpleTCPServer(host='localhost', port=8080)
    try:
        server.start()
    except KeyboardInterrupt:
        print("\n服务器被用户中断")
        server.stop()

if __name__ == "__main__":
    main()

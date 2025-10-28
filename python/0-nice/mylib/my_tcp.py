
import socket
import threading
import time
import json
import queue
from datetime import datetime
import pandas as pd
import numpy as np

class TCPServerBidirectional:
    def __init__(self, host='0.0.0.0', port=1123):
        self.host = host
        self.port = port
        self.server_socket = None
        self.running = False
        self.clients = {}
        self.client_lock = threading.Lock()
        self.broadcast_queue = queue.Queue()
        self.data_num = 10
        self.dataBuffer = []
        self.index = 0
        self.first = 1
        
    def start_server(self):
        """启动双向TCP服务器"""
        try:
            self.server_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
            self.server_socket.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
            
            self.server_socket.bind((self.host, self.port))
            self.server_socket.listen(10)
            
            print(f"双向TCP服务器启动，监听 {self.host}:{self.port}")
            print("等待客户端连接...")
            
            self.running = True
            
            # 启动广播线程
            broadcast_thread = threading.Thread(target=self.broadcast_loop)
            broadcast_thread.daemon = True
            broadcast_thread.start()
            
            # 启动终端输入线程
            input_thread = threading.Thread(target=self.handle_terminal_input)
            input_thread.daemon = True
            input_thread.start()
            
            while self.running:
                try:
                    client_socket, client_address = self.server_socket.accept()
                    print(f"客户端连接: {client_address}")
                    
                    # 添加客户端
                    with self.client_lock:
                        self.clients[client_address] = client_socket
                    
                    # 启动客户端处理线程
                    client_thread = threading.Thread(
                        target=self.handle_client_save_data,
                        args=(client_socket, client_address)
                    )
                    client_thread.daemon = True
                    client_thread.start()

                    if(self.first):
                        print('enable motor\n')
                        self.broadcast_message('m=1\n')
                        time.sleep(5)
                        self.first = 0
                        break
                    
                except socket.error:
                    if self.running:
                        continue
                        
        except Exception as e:
            print(f"服务器错误: {e}")
        finally:
            self.stop_server()
    
    def start(self):
        """启动双向TCP服务器"""
        try:
            self.server_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
            self.server_socket.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
            
            self.server_socket.bind((self.host, self.port))
            self.server_socket.listen(10)
            
            print(f"双向TCP服务器启动，监听 {self.host}:{self.port}")
            print("等待客户端连接...")
            
            self.running = True
            
            # 启动广播线程
            broadcast_thread = threading.Thread(target=self.broadcast_loop)
            broadcast_thread.daemon = True
            broadcast_thread.start()
            
            # 启动终端输入线程
            input_thread = threading.Thread(target=self.handle_terminal_input)
            input_thread.daemon = True
            input_thread.start()
            
            while self.running:
                try:
                    client_socket, client_address = self.server_socket.accept()
                    print(f"客户端连接: {client_address}")
                    
                    # 添加客户端
                    with self.client_lock:
                        self.clients[client_address] = client_socket
                    
                    # 启动客户端处理线程
                    client_thread = threading.Thread(
                        target=self.handle_client_save_data,
                        args=(client_socket, client_address)
                    )
                    client_thread.daemon = True
                    client_thread.start()

                    if(self.first):
                        print('enable motor\n')
                        self.broadcast_message('m=1\n')
                        time.sleep(5)
                        self.first = 0
                        break
                    
                except socket.error:
                    if self.running:
                        continue
                        
        except Exception as e:
            print(f"服务器错误: {e}")
        finally:
            self.stop_server()
    def handle_terminal_input(self):
        """处理终端输入"""
        print("\n=== 终端控制命令 ===")
        print("直接输入消息 - 发送给所有客户端")
        print("  /list - 列出所有连接的客户端")
        print("  /to IP PORT 消息 - 发送给特定客户端")
        print("  /broadcast 消息 - 广播消息给所有客户端")
        print("  /help - 显示帮助")
        print("  /quit - 退出程序")
        print("=" * 50)
        
        while self.running:
            try:
                user_input = input()
                
                if user_input.lower() == '/quit':
                    print("正在退出程序...")
                    self.running = False
                    break
                elif user_input.lower() == '/list':
                    self.list_clients()
                elif user_input.lower() == '/help':
                    print("命令列表:")
                    print("  直接输入消息 - 发送给所有客户端")
                    print("  /list - 列出所有连接的客户端")
                    print("  /to IP PORT 消息 - 发送给特定客户端")
                    print("  /broadcast 消息 - 广播消息给所有客户端")
                    print("  /help - 显示帮助")
                    print("  /quit - 退出程序")
                elif user_input.startswith('/to '):
                    # /to IP PORT message
                    parts = user_input[4:].strip().split(' ', 2)
                    if len(parts) >= 3:
                        try:
                            ip = parts[0]
                            port = int(parts[1])
                            message = parts[2]
                            target_addr = (ip, port)
                            self.send_to_client_addr(target_addr, message)
                        except ValueError:
                            print("格式错误: /to IP PORT 消息")
                    else:
                        print("格式错误: /to IP PORT 消息")
                elif user_input.startswith('/broadcast '):
                    message = user_input[11:].strip()
                    print(f'[SERVER] {message}')
                    if message:
                        self.broadcast_message(f"{message}\n")
                    else:
                        print("请输入广播消息")
                elif user_input.strip():
                    # 发送给所有客户端
                    print(f'[SERVER] {user_input}\n')
                    self.broadcast_message(f"{user_input}\n")
                else:
                    continue
                    
            except EOFError:
                # 处理输入流结束（如Ctrl+D）
                print("\n输入流结束，正在退出...")
                self.running = False
                break
            except Exception as e:
                print(f"输入处理错误: {e}")
    
    def list_clients(self):
        """列出所有连接的客户端"""
        with self.client_lock:
            if self.clients:
                print("当前连接的客户端:")
                for i, addr in enumerate(self.clients.keys(), 1):
                    print(f"  {i}. {addr[0]}:{addr[1]}")
            else:
                print("当前没有客户端连接")
    
    def send_to_client_addr(self, target_addr, message):
        """发送消息给指定地址的客户端"""
        with self.client_lock:
            if target_addr in self.clients:
                try:
                    if isinstance(message, str):
                        message = message + '\n'
                    self.clients[target_addr].send(message.encode('utf-8'))
                    print(f"发送给 {target_addr}: {message.strip()}")
                except Exception as e:
                    print(f"发送给 {target_addr} 失败: {e}")
                    del self.clients[target_addr]
            else:
                print(f"客户端 {target_addr} 不存在或已断开")
    
    def handle_client(self, client_socket, client_address):
        """处理客户端连接"""
        buffer = ""
        try:
            while self.running:
                try:
                    data = client_socket.recv(1024)
                    if not data:
                        break
                    
                    buffer += data.decode('utf-8')
                    
                    # 处理完整的行
                    while '\n' in buffer:
                        line, buffer = buffer.split('\n', 1)
                        line = line.strip()
                        if line:
                            self.handle_client_data(line, client_address, client_socket)
                            
                except socket.error:
                    break
        except Exception as e:
            print(f"处理客户端 {client_address} 错误: {e}")
        finally:
            # 移除客户端
            with self.client_lock:
                if client_address in self.clients:
                    del self.clients[client_address]
            try:
                client_socket.close()
            except:
                pass
            print(f"客户端 {client_address} 断开")
    def handle_client_save_data(self, client_socket, client_address):
        """处理客户端连接"""
        buffer = ""
        dataBufferList = []
        try:
            while self.running:
                try:
                    data = client_socket.recv(1024)
                    if not data:
                        break
                    
                    buffer += data.decode('utf-8')
                    
                    # 处理完整的行
                    while '\n' in buffer:
                        line, buffer = buffer.split('\n', 1)
                        line = line.strip()

                        if line:
                            print(f"{line}")
                            # line = str(line)
                            data_list = [float(x) for x in str(line).split(',')]
                            dataBufferList.append(data_list)
                            dataBuffer = np.array(dataBufferList)
                            n = len(dataBuffer[:, 0])
                            if n == self.data_num:
                                self.dataBuffer = dataBuffer
                                df = pd.DataFrame(dataBuffer, columns=['col1', 'col2'])
                                df.to_csv('data.csv', index=False)
                                print('data is saved\n')
                                return self.dataBuffer
                            
                except socket.error:
                    break
        except Exception as e:
            print(f"处理客户端 {client_address} 错误: {e}")
        finally:
            # 移除客户端
            with self.client_lock:
                if client_address in self.clients:
                    del self.clients[client_address]
            try:
                client_socket.close()
            except:
                pass
            print(f"客户端 {client_address} 断开")
    
    def handle_client_data(self, data, client_address, client_socket):
        """处理客户端数据"""
        timestamp = datetime.now().strftime('%H:%M:%S.%f')[:-3]
        print(f"[{timestamp}] {client_address}: {data}")
        
        # 根据数据类型处理
        if data.lower() == "ping":
            self.send_to_client(client_socket, "pong")
        elif data.lower() == "time":
            current_time = datetime.now().strftime('%Y-%m-%d %H:%M:%S')
            self.send_to_client(client_socket, f"time:{current_time}")
        elif data.startswith("broadcast:"):
            # 广播消息
            broadcast_msg = data[10:]  # 移除"broadcast:"前缀
            self.broadcast_message(f"[客户端广播] {broadcast_msg}", exclude=client_address)
        else:
            # 回显消息
            self.send_to_client(client_socket, f"echo: {data}")
    
    def send_to_client(self, client_socket, message):
        """发送消息给指定客户端"""
        try:
            if isinstance(message, str):
                message = message + '\n'
            client_socket.send(message.encode('utf-8'))
        except Exception as e:
            print(f"发送给客户端错误: {e}")
    
    def broadcast_message(self, message, exclude=None):
        """广播消息给所有客户端"""
        with self.client_lock:
            if not self.clients:
                print("没有连接的客户端")
                return
                
            for addr, sock in list(self.clients.items()):
                if addr != exclude:  # 排除发送者
                    try:
                        if isinstance(message, str):
                            message = message + '\n'
                        sock.send(message.encode('utf-8'))
                    except Exception as e:
                        print(f"广播给 {addr} 错误: {e}")
                        # 移除无效连接
                        if addr in self.clients and self.clients[addr] == sock:
                            del self.clients[addr]
    
    def broadcast_loop(self):
        """广播循环"""
        while self.running:
            try:
                # 从队列获取广播消息
                if not self.broadcast_queue.empty():
                    msg = self.broadcast_queue.get(timeout=0.1)
                    self.broadcast_message(msg)
                else:
                    time.sleep(0.01)
            except queue.Empty:
                continue
    
    def stop_server(self):
        """停止服务器"""
        self.running = False
        # 关闭所有客户端连接
        with self.client_lock:
            for client_socket in self.clients.values():
                try:
                    client_socket.close()
                except:
                    pass
            self.clients.clear()
        
        if self.server_socket:
            self.server_socket.close()
        print("服务器已停止")

# 服务端使用示例
def run_server():
    server = TCPServerBidirectional(port=1123)
    try:
        server.start_server()
    except KeyboardInterrupt:
        print("\n服务器被用户中断")
        server.stop_server()

if __name__ == "__main__":
    run_server()
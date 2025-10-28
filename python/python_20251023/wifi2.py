import socket
import threading
import time
import queue
from datetime import datetime

class InteractiveTCPServer:
    def __init__(self, host='0.0.0.0', port=8080):
        self.host = host
        self.port = port
        self.server_socket = None
        self.running = False
        self.clients = {}
        self.send_queue = queue.Queue()
        self.client_lock = threading.Lock()
        
    def start_server(self):
        """启动TCP服务器"""
        try:
            self.server_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
            self.server_socket.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
            
            self.server_socket.bind((self.host, self.port))
            self.server_socket.listen(10)
            
            print(f"TCP服务器启动，监听 {self.host}:{self.port}")
            print("等待客户端连接...")
            
            self.running = True
            
            # 启动发送线程
            send_thread = threading.Thread(target=self.send_loop)
            send_thread.daemon = True
            send_thread.start()
            
            while self.running:
                try:
                    client_socket, client_address = self.server_socket.accept()
                    print(f"客户端连接: {client_address}")
                    
                    with self.client_lock:
                        self.clients[client_address] = client_socket
                    
                    # 启动客户端处理线程
                    client_thread = threading.Thread(
                        target=self.handle_client,
                        args=(client_socket, client_address)
                    )
                    client_thread.daemon = True
                    client_thread.start()
                    
                except socket.error:
                    if self.running:
                        continue
                        
        except Exception as e:
            print(f"服务器错误: {e}")
        finally:
            self.stop_server()
    
    def handle_client(self, client_socket, client_address):
        """处理客户端数据"""
        try:
            while self.running:
                data = client_socket.recv(1024)
                if not data:
                    break
                
                message = data.decode('utf-8', errors='ignore').strip()
                if message:
                    timestamp = datetime.now().strftime('%H:%M:%S.%f')[:-3]
                    print(f"[{timestamp}] {client_address}: {message}")
                    
        except Exception as e:
            print(f"处理客户端错误: {e}")
        finally:
            with self.client_lock:
                if client_address in self.clients:
                    del self.clients[client_address]
            try:
                client_socket.close()
            except:
                pass
    
    def send_loop(self):
        """发送循环"""
        while self.running:
            try:
                if not self.send_queue.empty():
                    target, message = self.send_queue.get(timeout=0.1)
                    self.send_to_clients(message, target)
                else:
                    time.sleep(0.01)
            except queue.Empty:
                continue
    
    def send_to_clients(self, message, target='all'):
        """发送消息给客户端"""
        if isinstance(message, str):
            message = message + '\n'
        
        message_bytes = message.encode('utf-8')
        
        with self.client_lock:
            if target == 'all':
                # 发送给所有客户端
                for addr, sock in list(self.clients.items()):
                    try:
                        sock.send(message_bytes)
                    except Exception as e:
                        print(f"发送给 {addr} 失败: {e}")
                        # 移除无效连接
                        if addr in self.clients and self.clients[addr] == sock:
                            del self.clients[addr]
            else:
                # 发送给特定客户端
                if target in self.clients:
                    try:
                        self.clients[target].send(message_bytes)
                    except Exception as e:
                        print(f"发送给 {target} 失败: {e}")
                        del self.clients[target]
    
    def send_message(self, message, target='all'):
        """添加消息到发送队列"""
        self.send_queue.put((target, message))
    
    def list_clients(self):
        """列出当前连接的客户端"""
        with self.client_lock:
            if self.clients:
                print("当前连接的客户端:")
                for addr in self.clients.keys():
                    print(f"  - {addr}")
            else:
                print("当前没有客户端连接")
    
    def stop_server(self):
        """停止服务器"""
        self.running = False
        with self.client_lock:
            for client_socket in self.clients.values():
                try:
                    client_socket.close()
                except:
                    pass
            self.clients.clear()
        if self.server_socket:
            self.server_socket.close()

# 交互式输入函数
def interactive_input(server):
    """交互式输入处理"""
    print("\n=== 交互式TCP服务器控制 ===")
    print("命令格式:")
    print("  直接输入消息 - 发送给所有客户端")
    print("  /list - 列出所有连接的客户端")
    print("  /to IP:PORT 消息 - 发送给特定客户端")
    print("  /quit - 退出程序")
    print("=" * 40)
    
    while True:
        try:
            user_input = input("输入命令 > ")
            
            if user_input.lower() == '/quit':
                print("正在退出...")
                server.stop_server()
                break
            elif user_input.lower() == '/list':
                server.list_clients()
            elif user_input.startswith('/to '):
                # /to IP:PORT message
                parts = user_input[4:].split(' ', 1)
                if len(parts) >= 2:
                    target = parts[0]
                    message = parts[1]
                    # 解析IP和端口
                    try:
                        ip, port = target.split(':')
                        port = int(port)
                        target_addr = (ip, port)
                        server.send_message(message, target_addr)
                        print(f"发送给 {target_addr}: {message}")
                    except ValueError:
                        print("格式错误，应为: /to IP:PORT message")
                else:
                    print("格式错误，应为: /to IP:PORT message")
            else:
                # 发送给所有客户端
                if user_input.strip():
                    server.send_message(user_input)
                    print(f"发送给所有客户端: {user_input}")
                    
        except KeyboardInterrupt:
            print("\n正在退出...")
            server.stop_server()
            break
        except Exception as e:
            print(f"输入处理错误: {e}")

# 主函数
def main():
    server = InteractiveTCPServer(1123)
    
    # 启动服务器线程
    server_thread = threading.Thread(target=server.start_server)
    server_thread.daemon = True
    server_thread.start()
    
    # 等待服务器启动
    time.sleep(1)
    
    # 启动交互式输入
    interactive_input(server)

if __name__ == "__main__":
    main()
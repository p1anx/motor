import socket
import threading
import time

class TCPServer:
    def __init__(self, host='0.0.0.0', port=8080):
        self.host = host
        self.port = port
        self.server_socket = None
        self.running = False
        
    def start_server(self):
        """启动TCP服务器"""
        try:
            # 创建socket
            self.server_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
            # 设置socket选项，允许地址重用
            self.server_socket.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
            
            # 绑定地址和端口
            self.server_socket.bind((self.host, self.port))
            
            # 开始监听，最大连接数为5
            self.server_socket.listen(5)
            print(f"TCP服务器启动，监听 {self.host}:{self.port}")
            
            self.running = True
            
            while self.running:
                try:
                    # 接受客户端连接
                    client_socket, client_address = self.server_socket.accept()
                    print(f"客户端连接: {client_address}")
                    
                    # 为每个客户端创建新线程处理
                    client_thread = threading.Thread(
                        target=self.handle_client, 
                        args=(client_socket, client_address)
                    )
                    client_thread.daemon = True
                    client_thread.start()
                    
                except socket.error as e:
                    if self.running:
                        print(f"接受连接时出错: {e}")
                        
        except Exception as e:
            print(f"服务器启动失败: {e}")
        finally:
            self.stop_server()
    
    def handle_client(self, client_socket, client_address):
        """处理客户端数据"""
        try:
            while True:
                # 接收数据
                data = client_socket.recv(1024)  # 每次接收1024字节
                if not data:
                    break
                
                # 解码数据
                message = data.decode('utf-8')
                print(f"从 {client_address} 收到数据: {message}")
                
                # 可以在这里处理数据
                self.process_data(message, client_address)
                
                # 可选：发送响应给客户端
                response = "数据已接收"
                client_socket.send(response.encode('utf-8'))
                
        except socket.error as e:
            print(f"处理客户端数据时出错: {e}")
        finally:
            client_socket.close()
            print(f"客户端 {client_address} 断开连接")
    
    def process_data(self, data, client_address):
        """处理接收到的数据"""
        # 在这里添加你的数据处理逻辑
        print(f"处理来自 {client_address} 的数据: {data}")
    
    def stop_server(self):
        """停止服务器"""
        self.running = False
        if self.server_socket:
            self.server_socket.close()
        print("服务器已停止")

# 使用示例
if __name__ == "__main__":
    server = TCPServer(host='192.168.1.11', port=1123)
    try:
        server.start_server()
    except KeyboardInterrupt:
        print("\n服务器被用户中断")
        server.stop_server()
import socket
import sys

class TCPClient:
    def __init__(self, host='localhost', port=8888):
        self.host = host
        self.port = port
        self.socket = None
        self.connected = False

    def connect(self):
        """连接到服务器"""
        try:
            self.socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
            self.socket.connect((self.host, self.port))
            self.connected = True
            print(f"已连接到服务器 {self.host}:{self.port}")
            return True
        except Exception as e:
            print(f"连接失败: {e}")
            return False

    def send_data(self, data):
        """发送数据"""
        if not self.connected or self.socket is None:
            print("未连接到服务器")
            return False
        
        try:
            # 将字符串转换为字节
            if isinstance(data, str):
                data = data.encode('utf-8')
            
            # 发送数据长度
            data_length = len(data)
            length_bytes = data_length.to_bytes(4, byteorder='big')
            self.socket.sendall(length_bytes)
            
            # 发送数据
            self.socket.sendall(data)
            print(f"已发送 {data_length} 字节数据")
            return True
        except Exception as e:
            print(f"发送数据失败: {e}")
            self.connected = False
            return False

    def receive_data(self):
        """接收数据"""
        if not self.connected or self.socket is None:
            print("未连接到服务器")
            return None
        
        try:
            # 先接收数据长度（4字节）
            length_bytes = self._recv_exact(4)
            if length_bytes is None:
                return None
            
            data_length = int.from_bytes(length_bytes, byteorder='big')
            
            # 接收指定长度的数据
            data = self._recv_exact(data_length)
            if data is None:
                return None
            
            # 解码数据
            try:
                decoded_data = data.decode('utf-8')
                print(f"接收到 {data_length} 字节数据")
                return decoded_data
            except UnicodeDecodeError:
                print("数据解码失败，返回原始字节")
                return data
                
        except Exception as e:
            print(f"接收数据失败: {e}")
            self.connected = False
            return None

    def _recv_exact(self, num_bytes):
        """精确接收指定字节数的数据"""
        data = b''
        while len(data) < num_bytes:
            try:
                chunk = self.socket.recv(num_bytes - len(data))
                if not chunk:
                    # 连接关闭
                    print("连接已关闭")
                    self.connected = False
                    return None
                data += chunk
            except socket.error as e:
                print(f"接收数据时发生错误: {e}")
                self.connected = False
                return None
        return data

    def close(self):
        """关闭连接"""
        if self.socket:
            try:
                self.socket.close()
            except:
                pass
        self.connected = False
        print("连接已关闭")


class TCPServer:
    def __init__(self, host='localhost', port=8888):
        self.host = host
        self.port = port
        self.socket = None
        self.client_socket = None
        self.client_address = None

    def start(self):
        """启动服务器"""
        try:
            self.socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
            # 设置地址重用
            self.socket.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
            self.socket.bind((self.host, self.port))
            self.socket.listen(1)
            print(f"服务器启动，监听 {self.host}:{self.port}")
            
            print("等待客户端连接...")
            self.client_socket, self.client_address = self.socket.accept()
            print(f"客户端 {self.client_address} 已连接")
            return True
        except Exception as e:
            print(f"服务器启动失败: {e}")
            return False

    def send_data(self, data):
        """发送数据给客户端"""
        if self.client_socket is None:
            print("没有客户端连接")
            return False
        
        try:
            # 将字符串转换为字节
            if isinstance(data, str):
                data = data.encode('utf-8')
            
            # 发送数据长度
            data_length = len(data)
            length_bytes = data_length.to_bytes(4, byteorder='big')
            self.client_socket.sendall(length_bytes)
            
            # 发送数据
            self.client_socket.sendall(data)
            print(f"已发送 {data_length} 字节数据给客户端")
            return True
        except Exception as e:
            print(f"发送数据失败: {e}")
            return False

    def receive_data(self):
        """接收客户端数据"""
        if self.client_socket is None:
            print("没有客户端连接")
            return None
        
        try:
            # 先接收数据长度（4字节）
            length_bytes = self._recv_exact(4)
            if length_bytes is None:
                return None
            
            data_length = int.from_bytes(length_bytes, byteorder='big')
            
            # 接收指定长度的数据
            data = self._recv_exact(data_length)
            if data is None:
                return None
            
            # 解码数据
            try:
                decoded_data = data.decode('utf-8')
                print(f"从客户端接收到 {data_length} 字节数据")
                return decoded_data
            except UnicodeDecodeError:
                print("数据解码失败，返回原始字节")
                return data
                
        except Exception as e:
            print(f"接收数据失败: {e}")
            return None

    def _recv_exact(self, num_bytes):
        """精确接收指定字节数的数据"""
        data = b''
        while len(data) < num_bytes:
            try:
                chunk = self.client_socket.recv(num_bytes - len(data))
                if not chunk:
                    # 客户端断开连接
                    print("客户端断开连接")
                    return None
                data += chunk
            except socket.error as e:
                print(f"接收数据时发生错误: {e}")
                return None
        return data

    def close(self):
        """关闭服务器"""
        if self.client_socket:
            try:
                self.client_socket.close()
            except:
                pass
        if self.socket:
            try:
                self.socket.close()
            except:
                pass
        print("服务器已关闭")


def client_example():
    """客户端示例"""
    print("=== TCP客户端示例 ===")
    client = TCPClient('localhost', 8888)
    
    if client.connect():
        # 发送数据
        client.send_data("Hello, Server!")
        
        # 接收服务器响应
        response = client.receive_data()
        if response:
            print(f"收到服务器响应: {response}")
        
        # 再次发送数据
        client.send_data("This is another message from client.")
        response = client.receive_data()
        if response:
            print(f"收到服务器响应: {response}")
        
        client.close()


def server_example():
    """服务器示例"""
    print("=== TCP服务器示例 ===")
    server = TCPServer('192.168.1.11',1123)
    
    if server.start():
        try:
            # 接收客户端数据
            while True:
                print("wainting data\n")
                data = server.receive_data()
                if data:
                    print(f"收到客户端数据: {data}")
                    # 发送响应
                    server.send_data(f"Server received: {data}")
            
            # 再次接收数据
            data = server.receive_data()
            if data:
                print(f"收到客户端数据: {data}")
                # 发送响应
                server.send_data(f"Server received: {data}")
                
        except KeyboardInterrupt:
            print("\n服务器被中断")
        finally:
            server.close()


if __name__ == "__main__":
    server_example()
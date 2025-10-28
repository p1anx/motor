import socket
import threading
import time
import json
import queue
from datetime import datetime

class ESP8266Bidirectional:
    def __init__(self, port=8080):
        self.port = port
        self.socket = None
        self.running = False
        self.receive_thread = None
        self.send_thread = None
        self.send_queue = queue.Queue()
        self.client_socket = None
        self.lock = threading.Lock()
        
    def start(self):
        """启动ESP8266双向通信"""
        try:
            self.socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
            self.socket.bind(('0.0.0.0', self.port))
            self.socket.listen(1)
            print(f"ESP8266服务器启动，端口: {self.port}")
            
            # 接受ESP8266连接
            self.client_socket, addr = self.socket.accept()
            print(f"ESP8266连接: {addr}")
            
            self.running = True
            
            # 启动收发线程
            self.receive_thread = threading.Thread(target=self.receive_loop)
            self.receive_thread.daemon = True
            self.receive_thread.start()
            
            self.send_thread = threading.Thread(target=self.send_loop)
            self.send_thread.daemon = True
            self.send_thread.start()
            
            return True
        except Exception as e:
            print(f"启动失败: {e}")
            return False
    
    def receive_loop(self):
        """接收ESP8266数据"""
        buffer = ""
        try:
            while self.running:
                data = self.client_socket.recv(1024)
                if not data:
                    break
                
                buffer += data.decode('utf-8')
                
                # 处理传感器数据
                while '\n' in buffer:
                    line, buffer = buffer.split('\n', 1)
                    line = line.strip()
                    if line:
                        self.process_esp8266_data(line)
                        
        except Exception as e:
            print(f"接收错误: {e}")
    
    def process_esp8266_data(self, data):
        """处理ESP8266发送的数据"""
        timestamp = datetime.now().strftime('%H:%M:%S.%f')[:-3]
        print(f"[{timestamp}] ESP8266: {data}")
        
        # 根据ESP8266发送的数据类型响应
        if "SENSOR" in data.upper():
            # 发送控制命令
            self.send_data("CMD:LED_ON")
        elif "STATUS" in data.upper():
            # 发送状态信息
            status = {"status": "online", "timestamp": timestamp}
            self.send_data(json.dumps(status))
    
    def send_loop(self):
        """发送数据循环"""
        while self.running:
            try:
                if not self.send_queue.empty():
                    data = self.send_queue.get(timeout=0.1)
                    self.send_data_direct(data)
                else:
                    time.sleep(0.01)
            except queue.Empty:
                continue
    
    def send_data(self, data):
        """发送数据到ESP8266"""
        if isinstance(data, str):
            data = data + '\n'
        self.send_queue.put(data.encode('utf-8'))
    
    def send_data_direct(self, data):
        """直接发送数据"""
        try:
            with self.lock:
                self.client_socket.send(data)
        except Exception as e:
            print(f"发送错误: {e}")
    
    def control_esp8266(self, command):
        """控制ESP8266"""
        self.send_data(command)
    
    def stop(self):
        """停止服务"""
        self.running = False
        if self.client_socket:
            self.client_socket.close()
        if self.socket:
            self.socket.close()

# ESP8266控制示例
def esp8266_control_example():
    esp = ESP8266Bidirectional(port=1123)
    
    if esp.start():
        try:
            print("ESP8266双向通信已启动")
            print("输入命令控制ESP8266 (输入'quit'退出):")
            
            while True:
                cmd = input("输入命令: ")
                if cmd.lower() == 'quit':
                    break
                esp.control_esp8266(cmd)
                
        except KeyboardInterrupt:
            print("\n用户中断")
        finally:
            esp.stop()

if __name__ == "__main__":
    esp8266_control_example()
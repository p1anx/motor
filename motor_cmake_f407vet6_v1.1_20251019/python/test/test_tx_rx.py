import serial
import threading
import time
import queue

class UARTTransceiver:
    def __init__(self, port, baudrate=115200):
        self.ser = serial.Serial(port, baudrate, timeout=1)
        
        # 标志位
        self.running = False
        
        # 线程
        self.rx_thread = None
        self.tx_thread = None
        
        # 发送队列
        self.tx_queue = queue.Queue()
        
        # 接收数据存储（字符串列表）
        self.rx_strings = []
        
    def start(self):
        """启动收发线程"""
        self.running = True
        
        # 启动接收线程
        self.rx_thread = threading.Thread(target=self._receive_loop, daemon=True)
        self.rx_thread.start()
        
        # 启动发送线程
        self.tx_thread = threading.Thread(target=self._transmit_loop, daemon=True)
        self.tx_thread.start()
        
        print("串口收发已启动")
    
    def _receive_loop(self):
        """接收循环（独立线程）"""
        print("接收线程运行中...")
        
        # 跳过第一行（可能不完整）
        time.sleep(0.5)
        # self.ser.reset_input_buffer()
        # self.ser.readline()
        # print("已跳过第一行\n")
        
        while self.running:
            try:
                if self.ser.in_waiting > 0:
                    # 读取一行字符串
                    line = self.ser.readline().decode('utf-8', errors='ignore').strip()
                    
                    if line:
                        print(f"<< 接收: {line}")
                        self.rx_strings.append(line)
                
                time.sleep(0.01)
            
            except Exception as e:
                print(f"接收错误: {e}")
        
        print("接收线程已停止")
    
    def _transmit_loop(self):
        """发送循环（独立线程）"""
        print("发送线程运行中...")
        
        while self.running:
            try:
                # 从队列获取待发送数据
                data = self.tx_queue.get(timeout=0.1)
                
                # 发送数据（自动添加换行符）
                self.ser.write(f"{data}\n".encode('utf-8'))
                print(f">> 发送: {data}")
                
            except queue.Empty:
                pass
            except Exception as e:
                print(f"发送错误: {e}")
        
        print("发送线程已停止")
    
    def send(self, data):
        """发送字符串"""
        self.tx_queue.put(str(data))
    
    def get_received_strings(self):
        """获取所有接收到的字符串"""
        return self.rx_strings.copy()
    
    def get_latest_string(self):
        """获取最新接收的字符串"""
        if self.rx_strings:
            return self.rx_strings[-1]
        return None
    
    def stop(self):
        """停止收发"""
        print("\n正在停止...")
        self.running = False
        
        # 等待线程结束
        if self.rx_thread:
            self.rx_thread.join(timeout=2)
        if self.tx_thread:
            self.tx_thread.join(timeout=2)
        
        self.ser.close()
        print("串口已关闭")

# 使用示例
if __name__ == "__main__":
    uart = UARTTransceiver('/dev/ttyACM0', 115200)
    uart.start()
    
    try:
        # 循环发送字符串
        # count = 0
        # while True:
            # 发送不同格式的字符串
            # uart.send(f"Hello STM32 - Count: {count}")
        uart.send(f"p=1.2")
        
        # count += 1
        # time.sleep(1)  # 每秒发送一次
    
    except KeyboardInterrupt:
        print("\n用户中断")
    
    finally:
        uart.stop()
        
        # 显示接收到的字符串
        rx_strings = uart.get_received_strings()
        print(f"\n共接收 {len(rx_strings)} 条字符串")
        if rx_strings:
            print(f"前 5 条: {rx_strings[:5]}")
            print(f"最后 5 条: {rx_strings[-5:]}")
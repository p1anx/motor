import serial
import time
import matplotlib.pyplot as plt
import numpy as np

class UARTDataReader:
    def __init__(self, port, baudrate=115200):
        self.ser = serial.Serial(
            port=port,
            baudrate=baudrate,
            bytesize=serial.EIGHTBITS,
            parity=serial.PARITY_NONE,
            stopbits=serial.STOPBITS_ONE,
            timeout=1
        )
        
        # 数据存储数组
        self.data1 = []
        self.data2 = []
        self.data3 = []
    
    def read_data(self, num_samples=None):
        """
        读取 UART 数据
        num_samples: 读取的样本数量，None 表示持续读取
        """
        count = 0
        
        try:
            while True:
                if num_samples and count >= num_samples:
                    break
                
                # 读取一行
                if self.ser.in_waiting > 0:
                    line = self.ser.readline().decode('utf-8', errors='ignore').strip()
                    
                    if line:
                        # 解析数据
                        success = self.parse_line(line)
                        
                        if success:
                            count += 1
                            print(f"[{count}] {line}")
                
                time.sleep(0.01)
        
        except KeyboardInterrupt:
            print("\n停止接收")
        
        finally:
            self.print_summary()
    
    def parse_line(self, line):
        """解析一行数据"""
        try:
            values = line.split(',')
            
            if len(values) == 3:
                v1 = float(values[0])
                v2 = float(values[1])
                v3 = float(values[2])
                
                self.data1.append(v1)
                self.data2.append(v2)
                self.data3.append(v3)
                
                return True
            else:
                print(f"警告: 数据格式错误 - {line}")
                return False
        
        except ValueError as e:
            print(f"错误: 无法解析 - {line}, {e}")
            return False
    
    def print_summary(self):
        """打印统计信息"""
        print("\n=== 数据统计 ===")
        print(f"接收样本数: {len(self.data1)}")
        print(f"数组1: {self.data1[:10]}..." if len(self.data1) > 10 else f"数组1: {self.data1}")
        print(f"数组2: {self.data2[:10]}..." if len(self.data2) > 10 else f"数组2: {self.data2}")
        print(f"数组3: {self.data3[:10]}..." if len(self.data3) > 10 else f"数组3: {self.data3}")
    
    def get_data(self):
        """获取数据数组"""
        return self.data1, self.data2, self.data3
    
    def close(self):
        """关闭串口"""
        self.ser.close()

# 使用示例
if __name__ == "__main__":
    # 创建读取器
    reader = UARTDataReader(port='/dev/ttyUSB0', baudrate=115200)
    
    # 读取 100 个样本
    reader.read_data(num_samples=1000)
    
    # 获取数据
    data1, data2, data3 = reader.get_data()
    x = np.linspace(0, len(data1), len(data1))
    
    
    # 关闭串口
    reader.close()
    # print(x)
    plt.plot(x, data1)
    plt.show()
    
    # 使用数据
    print(f"\n总共接收: {len(data1)} 个样本")
import serial
import numpy as np
import matplotlib.pyplot as plt
from matplotlib.animation import FuncAnimation

class RealtimePlotter:
    def __init__(self, port, baudrate=115200, max_points=100):
        self.ser = serial.Serial(port, baudrate, timeout=1)
        self.max_points = max_points
        
        # 数据存储
        self.data1 = []
        self.data2 = []
        self.data3 = []
        
        # 创建图形
        self.fig, self.axes = plt.subplots(3, 1, figsize=(10, 8))
        self.lines = []
        
        for i, ax in enumerate(self.axes):
            line, = ax.plot([], [], 'b-')
            self.lines.append(line)
            ax.set_ylim(-1, 1)
            ax.set_xlim(0, max_points)
            ax.set_ylabel(f'数据 {i+1}')
            ax.grid(True)
        
        self.axes[-1].set_xlabel('样本')
    
    def read_serial(self):
        """读取串口数据"""
        if self.ser.in_waiting > 0:
            line = self.ser.readline().decode('utf-8', errors='ignore').strip()
            
            if line:
                try:
                    values = [float(x) for x in line.split(',')]
                    
                    if len(values) == 3:
                        self.data1.append(values[0])
                        self.data2.append(values[1])
                        self.data3.append(values[2])
                        
                        # 限制数据长度
                        if len(self.data1) > self.max_points:
                            self.data1.pop(0)
                            self.data2.pop(0)
                            self.data3.pop(0)
                
                except ValueError:
                    pass
    
    def update_plot(self, frame):
        """更新图形"""
        self.read_serial()
        
        data_arrays = [self.data1, self.data2, self.data3]
        
        for i, (line, data) in enumerate(zip(self.lines, data_arrays)):
            if data:
                x = np.arange(len(data))
                line.set_data(x, data)
                
                # 自动调整 Y 轴
                if len(data) > 0:
                    y_min, y_max = min(data), max(data)
                    y_range = y_max - y_min
                    self.axes[i].set_ylim(y_min - y_range * 0.1, y_max + y_range * 0.1)
        
        return self.lines
    
    def start(self):
        """开始实时绘图"""
        ani = FuncAnimation(self.fig, self.update_plot, interval=50, blit=True)
        plt.tight_layout()
        plt.show()
    
    def close(self):
        self.ser.close()

# 使用示例
if __name__ == "__main__":
    plotter = RealtimePlotter('/dev/ttyUSB0', 115200, max_points=1000)
    
    try:
        plotter.start()
    except KeyboardInterrupt:
        print("停止")
    finally:
        plotter.close()
import serial
import serial.tools.list_ports
import matplotlib.pyplot as plt
import numpy as np
import pandas as pd
import os
import time
import threading
import queue

class Serial:
    def __init__(self, port='/dev/ttyUSB0', baudrate=115200, data_n=100, skip_first=True):
        self.port = port
        self.speed = baudrate
        self.rxData_num = data_n
        self.skip_first = skip_first
        
        # 数据存储
        self.Bx_lists = []
        self.By_lists = []
        self.Bz_lists = []
        self.data1 = 0
        self.data2 = 0
        self.data3 = 0
        
        # 串口对象
        self.ser = None
        self.is_open = False
        
        # 打开串口
        self._open_serial()
        
    def _open_serial(self):
        """打开串口"""
        try:
            self.ser = serial.Serial(
                port=self.port,
                baudrate=self.speed,
                bytesize=8,
                parity='N',
                stopbits=1,
                timeout=1
            )
            self.is_open = True
            print(f"串口已打开: {self.port} @ {self.speed}")
            
            if self.skip_first:
                self._initialize()
            
            return True
        except Exception as e:
            print(f"打开串口失败: {e}")
            self.is_open = False
            return False
    
    def _initialize(self):
        """初始化：清空缓冲区并跳过第一行"""
        print("初始化中...")
        time.sleep(0.5)
        self.ser.reset_input_buffer()
        first_line = self.ser.readline()
        print(f"已跳过第一行: {first_line}")
        print("初始化完成，开始接收数据\n")
    
    def readline_1data(self):
        """读取一行数据，返回三个值（不关闭串口）"""
        if not self.is_open or self.ser is None:
            print("错误: 串口未打开")
            return None, None, None
        
        try:
            if self.ser.in_waiting > 0:
                data = self.ser.readline().decode('utf-8', errors='ignore').strip()
                
                if data:
                    dataB = [float(x) for x in data.split(",")]
                    if len(dataB) >= 3:
                        self.data1 = dataB[0]
                        self.data2 = dataB[1]
                        self.data3 = dataB[2]
                        return self.data1, self.data2, self.data3
            
            return None, None, None
        
        except Exception as e:
            print(f"读取错误: {e}")
            return None, None, None
    
    def readline_3data(self):
        """读取指定数量的数据"""
        try:
            while len(self.Bx_lists) < self.rxData_num:
                if self.ser.in_waiting > 0:
                    data = self.ser.readline().decode('utf-8', errors='ignore').strip()
                    
                    if data:
                        dataB = [float(x) for x in data.split(",")]
                        if len(dataB) >= 3:
                            self.Bx_lists.append(dataB[0])
                            self.By_lists.append(dataB[1])
                            self.Bz_lists.append(dataB[2])
                            
                            print(f"n_data = {len(self.Bx_lists)}", end="\r", flush=True)
            
            # 返回后清空列表，以便下次使用
            result = (self.Bx_lists.copy(), self.By_lists.copy(), self.Bz_lists.copy())
            self.Bx_lists.clear()
            self.By_lists.clear()
            self.Bz_lists.clear()
            
            return result
        
        except KeyboardInterrupt:
            print("\n停止接收")
            return None, None, None
    
    def send_line(self, data):
        """发送一行（自动添加换行符）"""
        if self.is_open and self.ser:
            self.ser.write(f"{data}\n".encode('utf-8'))
            print(f"发送: {data}")
    
    def close(self):
        """关闭串口"""
        if self.ser and self.is_open:
            self.ser.close()
            self.is_open = False
            print(f"串口 {self.port} 已关闭")
    
    def __del__(self):
        """析构函数"""
        self.close()


class UARTTransceiver:
    def __init__(self, port, baudrate=115200):
        self.port = port
        self.baudrate = baudrate
        self.ser = None
        self.is_open = False
        
        # 线程控制
        self.running = False
        self.rx_thread = None
        self.tx_thread = None
        
        # 队列
        self.tx_queue = queue.Queue()
        self.rx_strings = []
        
        # 打开串口
        self._open_serial()
    
    def _open_serial(self):
        """打开串口"""
        try:
            self.ser = serial.Serial(self.port, self.baudrate, timeout=1)
            self.is_open = True
            print(f"串口已打开: {self.port} @ {self.baudrate}")
            return True
        except Exception as e:
            print(f"打开串口失败: {e}")
            self.is_open = False
            return False
    
    def start(self):
        """启动收发线程"""
        if not self.is_open:
            print("串口未打开，无法启动")
            return False
        
        self.running = True
        
        # 启动接收线程
        self.rx_thread = threading.Thread(target=self._receive_loop, daemon=True)
        self.rx_thread.start()
        
        # 启动发送线程
        self.tx_thread = threading.Thread(target=self._transmit_loop, daemon=True)
        self.tx_thread.start()
        
        print(f"串口 {self.port} 收发已启动\n")
        return True
    
    def _receive_loop(self):
        """接收循环（独立线程）"""
        print(f"[{self.port}] 接收线程运行中...")
        
        # 跳过第一行
        time.sleep(0.5)
        self.ser.reset_input_buffer()
        first_line = self.ser.readline()
        print(f"[{self.port}] 已跳过第一行: {first_line}\n")
        
        while self.running:
            try:
                if self.ser and self.ser.is_open and self.ser.in_waiting > 0:
                    line = self.ser.readline().decode('utf-8', errors='ignore').strip()
                    
                    if line:
                        print(f"<< [{self.port}] 接收: {line}")
                        self.rx_strings.append(line)
                
                time.sleep(0.01)
            
            except Exception as e:
                print(f"[{self.port}] 接收错误: {e}")
                break
        
        print(f"[{self.port}] 接收线程已停止")
    
    def _transmit_loop(self):
        """发送循环（独立线程）"""
        print(f"[{self.port}] 发送线程运行中...")
        
        while self.running:
            try:
                data = self.tx_queue.get(timeout=0.1)
                
                if self.ser and self.ser.is_open:
                    self.ser.write(f"{data}\n".encode('utf-8'))
                    print(f">> [{self.port}] 发送: {data}")
            
            except queue.Empty:
                pass
            except Exception as e:
                print(f"[{self.port}] 发送错误: {e}")
                break
        
        print(f"[{self.port}] 发送线程已停止")
    
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
        print(f"\n正在停止 {self.port}...")
        self.running = False
        
        if self.rx_thread:
            self.rx_thread.join(timeout=2)
        if self.tx_thread:
            self.tx_thread.join(timeout=2)
        
        if self.ser and self.ser.is_open:
            self.ser.close()
            self.is_open = False
        
        print(f"串口 {self.port} 已关闭")


def test_0():
    """测试：读取多个数据点并发送统计信息"""
    # 接收串口（USB0）
    uart_rx = Serial(port='/dev/ttyUSB0', baudrate=115200, data_n=100)
    
    # 发送串口（ACM0）
    uart_tx = UARTTransceiver('/dev/ttyACM0', 115200)
    uart_tx.start()
    
    try:
        while True:
            # 读取 100 个数据点
            data1, data2, data3 = uart_rx.readline_3data()
            
            if data1 is None:
                print("读取失败")
                continue
            
            # 计算统计信息
            data1_deltaB = max(data1) - min(data1)
            data2_deltaB = max(data2) - min(data2)
            data3_deltaB = max(data3) - min(data3)
            
            data1_avg = np.mean(data1)
            data2_avg = np.mean(data2)
            data3_avg = np.mean(data3)
            
            print(f"\ndata1 avg = {data1_avg:.3f}, delta = {data1_deltaB:.3f}")
            print(f"data2 avg = {data2_avg:.3f}, delta = {data2_deltaB:.3f}")
            print(f"data3 avg = {data3_avg:.3f}, delta = {data3_deltaB:.3f}")
            
            # 发送数据
            uart_tx.send(f"p={data1_deltaB:.3f}")
            
            time.sleep(0.1)
    
    except KeyboardInterrupt:
        print("\n用户中断")
    
    finally:
        uart_tx.stop()
        uart_rx.close()
        
        # 显示接收到的字符串
        rx_strings = uart_tx.get_received_strings()
        print(f"\n共接收 {len(rx_strings)} 条字符串")
        if rx_strings:
            print(f"前 5 条: {rx_strings[:5]}")
            print(f"最后 5 条: {rx_strings[-5:]}")


def test_1():
    """测试：持续读取单个数据点并发送"""
    # 接收串口（USB0）
    uart_rx = Serial(port='/dev/ttyUSB0', baudrate=115200, data_n=1, skip_first=False)
    
    # 发送串口（ACM0）
    uart_tx = UARTTransceiver('/dev/ttyACM0', 115200)
    uart_tx.start()
    
    try:
        count = 0
        
        while True:
            # 持续读取单个数据点
            data1, data2, data3 = uart_rx.readline_1data()
            
            if data1 is not None:
                count += 1
                
                print(f"[{count}] data1={data1:.3f}, data2={data2:.3f}, data3={data3:.3f}")
                
                # 发送数据
                uart_tx.send(f"p={data1:.3f}")
            
            time.sleep(0.01)  # 短暂延时，避免 CPU 占用过高
    
    except KeyboardInterrupt:
        print("\n用户中断")
    
    finally:
        uart_tx.stop()
        uart_rx.close()
        
        # 显示接收到的字符串
        rx_strings = uart_tx.get_received_strings()
        print(f"\n共接收 {len(rx_strings)} 条字符串")
        if rx_strings:
            print(f"最后 5 条: {rx_strings[-5:]}")


def test_2():
    """测试：双向通信 - 两个串口都收发"""
    # 串口 1（USB0）- 主要接收数据
    uart1 = UARTTransceiver('/dev/ttyUSB0', 115200)
    uart1.start()
    
    # 串口 2（ACM0）- 主要发送数据
    uart2 = UARTTransceiver('/dev/ttyACM0', 115200)
    uart2.start()
    
    try:
        count = 0
        
        while count < 100:
            # 从串口 1 获取最新数据
            latest = uart1.get_latest_string()
            
            if latest:
                print(f"\n从 USB0 收到: {latest}")
                
                # 解析数据
                try:
                    values = [float(x) for x in latest.split(',')]
                    if len(values) >= 3:
                        data1, data2, data3 = values[0], values[1], values[2]
                        
                        # 计算并发送到串口 2
                        result = (data1 + data2 + data3) / 3
                        uart2.send(f"avg={result:.3f}")
                        
                        count += 1
                except ValueError:
                    pass
            
            time.sleep(0.1)
    
    except KeyboardInterrupt:
        print("\n用户中断")
    
    finally:
        uart1.stop()
        uart2.stop()


if __name__ == "__main__":
    # 选择测试函数
    
    # 1. 读取多个数据点并计算统计信息
    # test_0()
    
    # 2. 持续读取单个数据点（推荐）
    test_1()
    
    # 3. 双向通信测试
    # test_2()
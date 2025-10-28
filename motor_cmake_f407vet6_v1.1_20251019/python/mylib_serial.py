import serial
import serial.tools.list_ports
import matplotlib.pyplot as plt
import numpy as np
import pandas as pd
import os
import time
import mylib as m
# lis3mdl sample rate 320Hz
# 列出所有可用串口
#  ports = serial.tools.list_ports.comports()
# for port in ports:
#     print(port.device)

# here is all your config
class Serial:
    def __init__(self, port='/dev/ttyUSB', baudrate = 115200, data_n = 100, skip_first = True):
        self.ser = serial.Serial(port, baudrate, timeout=1)
        self.skip_first = skip_first
        self.port = port
        self.speed = baudrate
        self.rxData_num = data_n
        # self.file_path = './data/fc2Hz2Hz_3/'
        # self.file_path = './data/qam4_fc1Hz1Hz_2/'
        self.file_path = './data/test/'
        self.ser = serial.Serial(
            port= self.port,          # 替换为你的串口号
            baudrate=self.speed,     # 波特率
            bytesize=8,          # 数据位
            parity='N',          # 校验位
            stopbits=1,          # 停止位
            timeout=1            # 读取超时时间(秒)
        )
        self.Bx_lists = []
        self.By_lists = []
        self.Bz_lists = []
        self.data1 = 0
        self.data2 = 0
        self.data3 = 0
                
        # 初始化
        print(f"串口已打开: {port} @ {baudrate}")
        if self.skip_first:
            self._initialize()

    def _initialize(self):
            """初始化：清空缓冲区并跳过第一行"""
            print("初始化中...")
            
            # 等待数据填充缓冲区
            time.sleep(0.5)
            
            # 清空输入缓冲区
            self.ser.reset_input_buffer()
            
            # 读取并丢弃第一行
            first_line = self.ser.readline()
            # print(f"已跳过第一行: {first_line}")
            
            print("初始化完成，开始接收数据\n")
    def get_3data(self):
        return self.Bx_lists, self.By_lists, self.Bz_lists
    
    def readline_1data(self):
        try:
            while True:
                if self.ser.in_waiting > 0:  # 检查是否有可读数据
                    # data = ser.readline().decode('utf-8').strip()  # 读取一行并解码
                    data = self.ser.readline().decode('utf-8', errors='ignore').strip()  # 忽略错误字符
                    dataB= [float(x) for x in data.split(",")]
                    self.data1 = dataB[0]
                    self.data2 = dataB[1]
                    self.data3 = dataB[2]
                    return self.data1, self.data2, self.data3
        except KeyboardInterrupt:
            print("\n停止接收")

        # finally:
        #     self.ser.close()  # 关闭串口
    def readline_3data(self):
        try:
            while True:
                if self.ser.in_waiting > 0:  # 检查是否有可读数据
                    # data = ser.readline().decode('utf-8').strip()  # 读取一行并解码
                    data = self.ser.readline().decode('utf-8', errors='ignore').strip()  # 忽略错误字符
                    dataB= [float(x) for x in data.split(",")]
                    Bx = dataB[0]
                    By = dataB[1]
                    Bz = dataB[2]

                    self.Bx_lists.append(float(Bx))
                    self.By_lists.append(float(By))
                    self.Bz_lists.append(float(Bz))

                    Bx_lists_n = len(self.Bx_lists)
                    print(f"n_data = {Bx_lists_n}", end="\r", flush=True)
                    if(Bx_lists_n == self.rxData_num):
                        return self.Bx_lists, self.By_lists, self.Bz_lists
                    
        except KeyboardInterrupt:
            print("\n停止接收")

        finally:
            self.ser.close()  # 关闭串口
    def readline(self):
        try:
            while True:
                if self.ser.in_waiting > 0:  # 检查是否有可读数据
                    # data = ser.readline().decode('utf-8').strip()  # 读取一行并解码
                    data = self.ser.readline().decode('utf-8', errors='ignore').strip()  # 忽略错误字符
                    dataB= [float(x) for x in data.split(",")]
                    Bx = dataB[0]
                    By = dataB[1]
                    Bz = dataB[2]

                    self.Bx_lists.append(float(Bx))
                    self.By_lists.append(float(By))
                    self.Bz_lists.append(float(Bz))

                    Bx_lists_n = len(self.Bx_lists)
                    print(f"n_data = {Bx_lists_n}", end="\r", flush=True)
                    if(Bx_lists_n == self.rxData_num):
                        return self.Bx_lists, self.By_lists, self.Bz_lists
                    
        except KeyboardInterrupt:
            print("\n停止接收")

        finally:
            self.ser.close()  # 关闭串口
    def plotData(self):
        data1, data2, data3 = self.readline()
        n = len(data1)
        x = np.linspace(0, n - 1, n)
        plt.subplot(311)
        plt.plot(x, data1,label = 'data1')
        plt.subplot(312)
        plt.plot(x, data2,label = 'data2')
        plt.subplot(313)
        plt.plot(x, data3,label = 'data3')
        # plt.show()

    def send_string(self, data):
        """发送字符串"""
        self.ser.write(data.encode('utf-8'))
        print(f"发送字符串: {data}")
    def send_line(self, data):
        """发送一行（自动添加换行符）"""
        self.ser.write(f"{data}\n".encode('utf-8'))
        print(f"发送: {data}")
        
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
def test_getDeltaB():
    # uart = UARTTransceiver('/dev/ttyACM0', 115200)
    uart_rx = Serial(port='/dev/ttyACM0', baudrate=115200, data_n=1000)
    # uart.start()
    
    try:
        # 循环发送字符串
        # count = 0
        # while True:
            data1,data2,data3 = uart_rx.readline_3data()
            # uart_rx.plotData()
            # data1, data2, data3 = uart_rx.get_3data
            data1_deltaB = max(data1) - min(data1)
            data2_deltaB = max(data2) - min(data2)
            data3_deltaB = max(data3) - min(data3)
            # m.plot(data1)
            data1_avg = np.mean(data1)
            data2_avg = np.mean(data2)
            data3_avg = np.mean(data3)
            print(f"data1 avg = {data1_avg:.3f}, delta = {data1_deltaB:.3f}, max = {max(data1):.3f}, min = {min(data1):.3f}\n")
            print(f"data2 avg = {data2_avg:.3f}, delta = {data2_deltaB:.3f}, max = {max(data2):.3f}, min = {min(data2):.3f}\n")
            print(f"data3 avg = {data3_avg:.3f}, delta = {data3_deltaB:.3f}, max = {max(data3):.3f}, min = {min(data3):.3f}\n")

            # uart.send(f"p={data1_deltaB}")
            #1.
            # test_0()
            #2.
    
    except KeyboardInterrupt:
        print("\n用户中断")
    
    # finally:
        # uart_rx.close()
        # uart.stop()
        
        # # 显示接收到的字符串
        # rx_strings = uart.get_received_strings()
        # print(f"\n共接收 {len(rx_strings)} 条字符串")
        # if rx_strings:
        #     print(f"前 5 条: {rx_strings[:5]}")
        #     print(f"最后 5 条: {rx_strings[-5:]}")
def test_rx_tx():
    uart = UARTTransceiver('/dev/ttyACM0', 115200)
    uart_rx = Serial(port='/dev/ttyUSB0', baudrate=115200, data_n=1)
    uart.start()
    buffer = []
    
    try:
        # 循环发送字符串
        # count = 0
        while True:
            data1, data2, data3 = uart_rx.readline_1data()
            # data1_avg = np.mean(data1)
            # data2_avg = np.mean(data2)
            # data3_avg = np.mean(data3)

            print(f"data1 avg = {data1}\n")
            print(f"data2 avg = {data2}\n")
            print(f"data3 avg = {data3}\n")
            # buffer.append(data1)
            # if len(buffer) ==2000:
            #     m.plot(buffer)
            #     plt.show()
            uart.send(f"p={data1}")
            # time.sleep(1)
    
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
def test_2():
    uart = UARTTransceiver('/dev/ttyACM0', 115200)
    uart_rx = Serial(port='/dev/ttyUSB0', baudrate=115200, data_n=3)
    uart.start()
    
    try:
        # 循环发送字符串
        # count = 0
        while True:
            data1,data2,data3 = uart_rx.readline_3data()
            # uart_rx.plotData()
            # data1, data2, data3 = uart_rx.get_3data
            data1_deltaB = max(data1) - min(data1)
            data2_deltaB = max(data2) - min(data2)
            data3_deltaB = max(data3) - min(data3)
            # m.plot(data1)
            data1_avg = np.mean(data1)
            data2_avg = np.mean(data2)
            data3_avg = np.mean(data3)
            print(f"data1 avg = {data1_avg}, delta = {data1_deltaB}\n")
            print(f"data2 avg = {data2_avg}, delta = {data2_deltaB}\n")
            print(f"data3 avg = {data3_avg}, delta = {data3_deltaB}\n")

            uart.send(f"p=1.1")
            #1.
            # test_0()
            #2.
    
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
# 使用示例
if __name__ == "__main__":
    #1.
    test_getDeltaB()
    #2.
    #  test_rx_tx()
    #3.
    # test_2()
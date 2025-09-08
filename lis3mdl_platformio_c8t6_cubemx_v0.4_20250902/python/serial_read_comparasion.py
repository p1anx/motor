import serial
import serial.tools.list_ports
import matplotlib.pyplot as plt
import numpy as np
import pandas as pd
import os
# lis3mdl sample rate 320Hz
# 列出所有可用串口
#  ports = serial.tools.list_ports.comports()
# for port in ports:
#     print(port.device)

# here is all your config
class Config:
    def __init__(self):
        self.serial_port = '/dev/ttyUSB0'
        self.rxData_num = 320*100
        # self.file_path = './data/fc2Hz2Hz_3/'
        self.file_path = './data/16qam_fc2Hz1Hz_3/'
        # self.file_path = './data/test/'
        

config = Config() 
# 配置串口参数
ser = serial.Serial(
    port=config.serial_port,          # 替换为你的串口号
    baudrate=115200,     # 波特率
    bytesize=8,          # 数据位
    parity='N',          # 校验位
    stopbits=1,          # 停止位
    timeout=1            # 读取超时时间(秒)
)
def get_every_n_points(data, n=280):
    if not data:
        return []
    return data[::n]

def uart_decode(bitstream):
    result = []
    i = 0
    n = len(bitstream)
    
    while i <= n - 10:  # 确保剩余比特足够组成一个UART帧
        # 检查起始位和停止位（直接比较整数）
        if bitstream[i] == 0 and bitstream[i+9] == 1:
            # 提取数据位（高位在前）
            data_bits = bitstream[i+1:i+9]
            # 将整数列表转换为二进制字符串
            binary_str = ''.join(map(str, data_bits))
            byte_value = int(binary_str, 2)
            result.append(chr(byte_value))
            i += 10  # 成功匹配后跳过整个帧
        else:
            i += 1  # 未找到有效帧，逐位检查
    
    return ''.join(result)

try:
    Bx_lists = []
    By_lists = []
    Bz_lists = []
    # sample_per_bit = 310
    # rxData_num = sample_per_bit * 1
    rxData_num = config.rxData_num

    saved_file_path = config.file_path
    if not os.path.exists(saved_file_path):
        os.makedirs(saved_file_path)

    while True:
        if ser.in_waiting > 0:  # 检查是否有可读数据
            # data = ser.readline().decode('utf-8').strip()  # 读取一行并解码
            data = ser.readline().decode('utf-8', errors='ignore').strip()  # 忽略错误字符
            datalen = len(data)

            # print(f"data length = {len(data)}\n")
            if len(data) > 28:
            # data = ser.readline()
                dataB= [float(x) for x in data.split(",")]
            # if(len(dataB) == 3):
                Bx = dataB[0]
                By = dataB[1]
                Bz = dataB[2]

            # Bx_lists = np.array(Bx_lists)
            # print(f"bx {Bx}")
                Bx_lists.append(dataB[0])
                By_lists.append(float(By))
                Bz_lists.append(float(Bz))
                # By_lists = np.array(By_lists)
                Bx_lists_n = len(Bx_lists)

                print(f"n_data = {len(Bx_lists)}", end="\r", flush=True)
                if(Bx_lists_n > rxData_num):
                    pd.DataFrame(Bx_lists).to_csv(saved_file_path+"Bx.csv", index=False, header=False)
                    pd.DataFrame(By_lists).to_csv(saved_file_path+"By.csv", index=False, header=False)
                    pd.DataFrame(Bz_lists).to_csv(saved_file_path+"Bz.csv", index=False, header=False)

                    Bx_n_group = get_every_n_points(Bx_lists, n=280)
                    By_n_group = get_every_n_points(By_lists, n=280)
                    Bz_n_group = get_every_n_points(Bz_lists, n=280)
                    bit0 = []
                    bit1 = []
                    bit2 = []
                    bit3 = []
                    for i,value in enumerate(Bx_n_group):
                        if i%4==0:
                            bit0.append(value)
                        elif i%4==1:
                            bit1.append(value)
                        elif i%4==2:
                            bit2.append(value)
                        elif i%4==3:
                            bit3.append(value)
                        # elif 
                    Bx_n = bit0
                    By_n = bit1
                    Bz_n = bit2
                    print(f"Bx: {Bx_n_group}")
                    print(f"Bx 1-4: {Bx_n}")
                    plt.figure(1)
                    plt.subplot(211)
                    plt.plot(np.linspace(0, len(Bx_n_group), len(Bx_n_group)), Bx_n_group)
                    # plt.plot(np.linspace(0, len(By_n_group), len(By_n_group)), By_n_group)
                    # plt.plot(np.linspace(0, len(Bz_n_group), len(Bz_n_group)), Bz_n_group)
                    plt.subplot(212)
                    plt.plot(np.linspace(0, len(Bx_n), len(Bx_n)),Bx_n)
                    plt.plot(np.linspace(0, len(By_n), len(By_n)),By_n)
                    plt.plot(np.linspace(0, len(Bz_n), len(Bz_n)),Bz_n)

                    # By_lists = By_lists - np.min(By_lists)
                    # By_lists = By_lists/ np.max(By_lists)

                    # Bz_lists = Bz_lists - np.min(Bz_lists)
                    # Bz_lists = Bz_lists/ np.max(Bz_lists)


                    plt.figure(2)
                    t = np.linspace(0, Bx_lists_n, Bx_lists_n)
                    # plt.subplot(311)
                    plt.plot(t, Bx_lists,  label = "Bx")
                    # plt.subplot(312)
                    plt.plot(t, By_lists,  label = "By")
                    # plt.subplot(313)
                    plt.plot(t, Bz_lists,  label = "Bz")
                    plt.legend()
                    plt.show()
                    break
                




                
            # print(float_array)  # Output: [1.855013, 2.285881, -1.368606]
            # print(f"Bx = {dataB[0]}\n")
            # print(f"By = {By}\n")
            # print(f"Bz = {Bz}\n\n")
            # print(f"type: {type(data)}")
            # print(f"data = {data}")
            
except KeyboardInterrupt:
    print("程序终止")
    
finally:
    ser.close()  # 关闭串口
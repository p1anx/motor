import serial
import serial.tools.list_ports
import matplotlib.pyplot as plt
import numpy as np
import pandas as pd
import os
import mylib as m
# lis3mdl sample rate 320Hz
# 列出所有可用串口
#  ports = serial.tools.list_ports.comports()
# for port in ports:
#     print(port.device)


# here is all your config
class Config:
    def __init__(self):
        self.serial_port = "/dev/ttyUSB0"
        self.rxData_num = 1024
        # self.file_path = './data/fc2Hz2Hz_3/'
        self.file_path = "./test/"
        # self.file_path = './data/test/'


config = Config()
# 配置串口参数
ser = serial.Serial(
    port=config.serial_port,  # 替换为你的串口号
    baudrate=115200,  # 波特率
    bytesize=8,  # 数据位
    parity="N",  # 校验位
    stopbits=1,  # 停止位
    timeout=1,  # 读取超时时间(秒)
)


def plot_fft_magnitude(data, fs):
    data = np.array(data)
    data = data - np.mean(data)
    x = np.linspace(0, len(data), len(data))

    freq_fft, complex_fft = m.to_fft(data, fs)
    amp_fft = abs(complex_fft)
    plt.subplot(211)
    plt.plot(x, data, label="time")
    plt.subplot(212)
    plt.plot(freq_fft, amp_fft, label="fft")
    plt.xlabel("freq(Hz)")
    plt.ylabel("amplitude")
    plt.legend()
    plt.tight_layout()


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
        if bitstream[i] == 0 and bitstream[i + 9] == 1:
            # 提取数据位（高位在前）
            data_bits = bitstream[i + 1 : i + 9]
            # 将整数列表转换为二进制字符串
            binary_str = "".join(map(str, data_bits))
            byte_value = int(binary_str, 2)
            result.append(chr(byte_value))
            i += 10  # 成功匹配后跳过整个帧
        else:
            i += 1  # 未找到有效帧，逐位检查

    return "".join(result)


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
            data = (
                ser.readline().decode("utf-8", errors="ignore").strip()
            )  # 忽略错误字符
            datalen = len(data)

            # print(f"data length = {len(data)}\n")
            if len(data) > 28:
                # data = ser.readline()
                dataB = [float(x) for x in data.split(",")]
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
                fs = 284

                print(f"n_data = {len(Bx_lists)}", end="\r", flush=True)
                if Bx_lists_n > rxData_num:
                    m.plot_fft_magnitude(Bx_lists, fs)
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


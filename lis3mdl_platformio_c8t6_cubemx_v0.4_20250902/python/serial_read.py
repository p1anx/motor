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

# 配置串口参数
ser = serial.Serial(
    port='/dev/ttyACM0',          # 替换为你的串口号
    baudrate=115200,     # 波特率
    bytesize=8,          # 数据位
    parity='N',          # 校验位
    stopbits=1,          # 停止位
    timeout=1            # 读取超时时间(秒)
)
def ask_to_bits_0(signal, sample_rate, carrier_freq, threshold=0.5):
    """
    将ASK信号转换为比特流
    参数:
        signal: 输入的ASK信号波形
        sample_rate: 采样率(Hz)
        carrier_freq: 载波频率(Hz)
        threshold: 幅度判决阈值(0-1)
    返回:
        bits: 解调出的比特流
    """
    # 计算每个比特的采样点数
    bit_duration = 1 / carrier_freq  # 假设每个比特持续一个载波周期
    samples_per_bit = int(bit_duration * sample_rate)
    
    bits = []
    for i in range(0, len(signal), samples_per_bit):
        # 提取一个比特周期内的信号
        bit_samples = signal[i:i+samples_per_bit]
        
        # 计算该周期的平均幅度
        avg_amplitude = np.mean(np.abs(bit_samples))
        print(f"avg = {avg_amplitude}\n")
        
        # 幅度判决
        bit = 1 if avg_amplitude > threshold else 0
        bits.append(bit)
    
    return np.array(bits)

def ask_to_bits(signal, sample_rate, carrier_freq, threshold=0.5):
    """
    将ASK信号转换为比特流（从第一个有效边沿开始）
    参数:
        signal: 输入的ASK信号波形
        sample_rate: 采样率(Hz)
        carrier_freq: 载波频率(Hz)
        threshold: 幅度判决阈值(0-1)
    返回:
        bits: 解调出的比特流
        start_index: 检测到的起始位置
    """
    # 计算每个比特的采样点数
    bit_duration = 1 / carrier_freq  # 假设每个比特持续一个载波周期
    samples_per_bit = int(bit_duration * sample_rate)
    
    # 1. 寻找第一个有效上升沿（信号超过阈值的位置）
    start_index = 0
    for i in range(1, len(signal)):
        if signal[i-1] < threshold and signal[i] >= threshold:
            start_index = i
            break
    
    print(f"检测到起始位置: {start_index} (采样点)")
    
    # 2. 从起始位置开始解码
    bits = []
    for i in range(start_index, len(signal), samples_per_bit):
        # 提取一个比特周期内的信号
        end_idx = min(i + samples_per_bit, len(signal))
        bit_samples = signal[i:end_idx]
        
        # 计算该周期的平均幅度
        avg_amplitude = np.mean(np.abs(bit_samples))
        print(f"位置 {i}-{end_idx}, 平均幅度 = {avg_amplitude:.3f}")
        
        # 幅度判决
        bit = 1 if avg_amplitude > threshold else 0
        bits.append(bit)
    
    return np.array(bits), start_index

def ask_to_bits_single_sample(signal, sample_rate, carrier_freq, threshold=0.5, sample_offset_ratio=0.5):
    """
    单点采样法将ASK信号转换为比特流
    参数:
        signal: 输入的ASK信号波形
        sample_rate: 采样率(Hz)
        carrier_freq: 载波频率(Hz)
        threshold: 幅度判决阈值(0-1)
        sample_offset_ratio: 采样点位置比例(0-1, 0.5表示周期中点)
    返回:
        bits: 解调出的比特流
        sample_points: 实际采样点位置
    """
    # 计算每个比特的采样点数
    bit_duration = 1 / carrier_freq
    samples_per_bit = int(bit_duration * sample_rate)
    
    # 计算采样点偏移量
    sample_offset = int(samples_per_bit * sample_offset_ratio)
    
    # 寻找第一个有效上升沿
    start_index = 0
    for i in range(1, len(signal)):
        if signal[i-1] < threshold and signal[i] >= threshold:
            start_index = i
            break
    
    # 从起始位置开始，在每个比特周期采样一个点
    bits = []
    sample_points = []
    for i in range(start_index, len(signal), samples_per_bit):
        sample_pos = i + sample_offset
        if sample_pos >= len(signal):
            break
            
        sample_points.append(sample_pos)
        bit = 1 if signal[sample_pos] > threshold else 0
        bits.append(bit)
    
    return np.array(bits), np.array(sample_points)

# def uart_decode(bitstream: str) -> str:
#     result = []
#     i = 0
#     n = len(bitstream)
    
#     while i <= n - 10:  # 确保剩余比特足够组成一个UART帧
#         # 检查起始位和停止位
#         if bitstream[i] == '0' and bitstream[i+9] == '1':
#             # 提取数据位（高位在前）
#             data_bits = bitstream[i+1:i+9]
#             byte_value = int(data_bits, 2)
#             result.append(chr(byte_value))
#             i += 10  # 成功匹配后跳过整个帧
#         else:
#             i += 1  # 未找到有效帧，逐位检查
    
#     return ''.join(result)
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
    sample_per_bit = 310
    rxData_num = sample_per_bit * 1

    saved_file_path = "./data/"
    if not os.path.exists(saved_file_path):
        os.makedirs(saved_file_path)

    while True:
        if ser.in_waiting > 0:  # 检查是否有可读数据
            data = ser.readline().decode('utf-8').strip()  # 读取一行并解码
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

                    # Bx_lists = Bx_lists - np.min(Bx_lists)
                    # Bx_lists = Bx_lists/ np.max(Bx_lists)
                    # Bx_bits, Bx_sample_pos = ask_to_bits_single_sample(Bx_lists, sample_per_bit, 1, 0.45)
                    # print(f"bits = {Bx_bits}, sample_pos = {Bx_sample_pos}\n")
                    # Bx_str_decode = uart_decode(Bx_bits)
                    # print(f"Bx str = {Bx_str_decode}")

                    By_lists = By_lists - np.min(By_lists)
                    By_lists = By_lists/ np.max(By_lists)
                    By_bits, By_sample_pos = ask_to_bits_single_sample(By_lists, sample_per_bit, 1, 0.45)
                    print(f"bits = {By_bits}, sample_pos = {By_sample_pos}\n")
                    # By_bits = 1 -By_bits
                    # By_lists = 1 - By_lists

                    Bz_lists = Bz_lists - np.min(Bz_lists)
                    Bz_lists = Bz_lists/ np.max(Bz_lists)
                    Bz_bits, Bz_sample_pos = ask_to_bits_single_sample(Bz_lists, sample_per_bit, 1)
                    print(f"bits = {Bz_bits}, sample_pos = {Bz_sample_pos}\n")

                    # bits = ask_to_bits(By_lists, 160, 1, 0.5)

                    bits = Bz_bits
                    str_decode = uart_decode(bits)

                    By_str_decode = uart_decode(By_bits)
                    print(f"By str = {By_str_decode}")
                    Bz_str_decode = uart_decode(Bz_bits)
                    print(f"Bz str = {Bz_str_decode}")

                    print(f"decode str = {str_decode}")
                    print(f"num of bits = {len(bits)}\n")
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
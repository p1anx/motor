import pandas as pd
import matplotlib.pyplot as plt
import numpy as np
from scipy.signal import butter, filtfilt


def butter_lowpass(cutoff, fs, order=5):
    nyq = 0.5 * fs
    normal_cutoff = cutoff / nyq
    b, a = butter(order, normal_cutoff, btype='low', analog=False)
    return b, a

def lowpass_filter(data, cutoff, fs, order=5):
    b, a = butter_lowpass(cutoff, fs, order=order)
    y = filtfilt(b, a, data)
    return y

def filter_B(data):
    
# 使用示例
    fs = 320       # 采样频率
    cutoff = 320     # 截止频率(根据信号特性调整)
    filtered_data = lowpass_filter(data, cutoff, fs)
    t = np.linspace(0, len(data), len(data))

    plt.figure(figsize=(8, 6))
    plt.subplot(211)
    plt.plot(t, filtered_data)
    plt.subplot(212)
    plt.plot(t, data)

def wave_bits(data):
    bits = []
    judge = np.mean(np.abs(data))
    # judge = (max(np.abs(data))+ min(np.abs(data)))/2 
    print(f"judge = {judge}")
    for element in np.abs(data):
        bit = 1 if element > judge else 0
        bits.append(bit)
    return bits
def bits_plot(data):
    bits = wave_bits(data)    
    t = np.linspace(0, len(bits), len(bits))
    
    plt.plot(t, data)
    # plt.plot(t, bits)
    plt.xlabel("Sample points")
    plt.ylabel("Amplitude")
    plt.savefig("data/hello_raw_bits.png", dpi=300)

def bits_plot_Bxyz(datax,datay, dataz):
    datax_bits = wave_bits(datax)    
    datay_bits = wave_bits(datay)    
    dataz_bits = wave_bits(dataz)    
    t = np.linspace(0, len(datax), len(datax))

    plt.plot(t, datax, label = "Bx")
    plt.plot(t, datay, label = "By")
    plt.plot(t, dataz, label = "Bz")
    # plt.plot(t, bits)
    plt.xlabel("Sample points")
    plt.ylabel("Amplitude")
    plt.legend()
    plt.savefig("data/hello_raw_xyz.png", dpi=300)

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
def test(data):
    sample_rate = 155
    carrier_freq = 1
    signal = wave_bits(data)
    bits, start_index= ask_to_bits(signal, sample_rate, carrier_freq)
    str_bits = uart_decode(bits)
    print(f"bits = {bits}")
    print(f"string = {str_bits}")
    

if __name__ == "__main__":
    print("hello")
    filePath = "data/"
    fileBx = filePath + "Bx.csv"
    fileBy = filePath + "By.csv"
    fileBz = filePath + "Bz.csv"
    # dataBx = pd.read_csv(fileBx)
    # dataBy = pd.read_csv(fileBy)
    # dataBz = pd.read_csv(fileBz)
    dataBx = pd.read_csv(fileBx).values.flatten().astype(float)
    dataBy = pd.read_csv(fileBy).values.flatten().astype(float)
    dataBz = pd.read_csv(fileBz).values.flatten().astype(float)
    data = dataBz
    # data = data - min(data)
    # B_filter_read_csv()
    # filter_B(data)

    bits_plot(data)
    # bits_plot_Bxyz(dataBx, dataBy, dataBz) # plot Bx, By, Bz
    test(data)

    plt.show()


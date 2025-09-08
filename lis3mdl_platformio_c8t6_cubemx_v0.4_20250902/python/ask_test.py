import numpy as np
from scipy.signal import butter, filtfilt
import pandas as pd
import matplotlib.pyplot as plt

filePath = "data/"
fileName = filePath + "ask_demodulated.png"

def demodulate_2ask_demo(signal, sample_rate, carrier_freq, symbol_duration):
    """
    解调2ASK信号并输出数字方波
    
    参数:
    signal -- 输入的2ASK信号 (numpy数组)
    sample_rate -- 采样率 (Hz)
    carrier_freq -- 载波频率 (Hz)
    symbol_duration -- 单个符号持续时间 (秒)
    
    返回:
    digital_wave -- 解调后的数字方波 (numpy数组)
    """
    # 1. 全波整流（取绝对值）
    rectified = np.abs(signal)
    
    # 2. 设计低通滤波器提取包络
    # 截止频率设置为载波频率的1/10和符号速率的2倍中的较小值
    symbol_rate = 1 / symbol_duration
    cutoff_freq = min(0.1 * carrier_freq, 2 * symbol_rate)
    nyquist = 0.5 * sample_rate
    
    # 归一化截止频率
    normal_cutoff = cutoff_freq / nyquist
    
    # 创建4阶巴特沃斯低通滤波器
    b, a = butter(4, normal_cutoff, btype='low', analog=False)
    
    # 零相位滤波
    envelope = filtfilt(b, a, rectified)
    
    # 3. 抽样判决
    samples_per_symbol = int(sample_rate * symbol_duration)
    num_symbols = len(signal) // samples_per_symbol
    
    # 在每个符号中间位置抽样
    sampling_points = [int((i + 0.5) * samples_per_symbol) 
                      for i in range(num_symbols)]
    
    # 计算自适应阈值（使用整个包络的中间值）
    threshold = (np.max(envelope) + np.min(envelope)) * 0.5
    
    # 恢复数字信号
    digital_wave = np.zeros(num_symbols, dtype=int)
    for i, pos in enumerate(sampling_points):
        if pos < len(envelope):
            digital_wave[i] = 1 if envelope[pos] > threshold else 0
    
    return digital_wave
def fft_signal(data, fs):
    N = len(data)
    data_fft = np.fft.fft(data)
    data_fft = 2/N*abs(data_fft)[:N//2]
    data_freq = np.fft.fftfreq(N, 1/fs)[:N//2]
    return data_fft, data_freq
def wave_bits(data):
    bits = []
    judge = np.mean(np.abs(data))
    # judge = (max(np.abs(data))+ min(np.abs(data)))/2 
    print(f"judge = {judge}")
    for element in np.abs(data):
        bit = 1 if element > judge else 0
        bits.append(bit)
    return bits
def demodulate_2ask(signal, sample_rate, carrier_freq, symbol_duration):
    signal = np.abs(signal)
    len_signal = len(signal)
    t = np.linspace(0, len_signal, len_signal)

    signal_fft, signal_freq = fft_signal(signal, sample_rate)

    cutoff_freq = 0.9

    nyquist = 0.5 * sample_rate
    
    # 归一化截止频率
    normal_cutoff = cutoff_freq / nyquist
    
    # 创建4阶巴特沃斯低通滤波器
    b, a = butter(4, normal_cutoff, btype='low', analog=False)
    
    # 零相位滤波
    envelope = filtfilt(b, a, signal)
    bits = wave_bits(envelope)
    plt.figure(figsize=(8,6))
    plt.subplot(311)
    plt.plot(t, signal, label="raw signal", color = 'g')
    plt.ylabel("Amplitude")
    plt.legend()

    plt.subplot(312)
    # plt.plot(signal_freq, signal_fft)
    plt.plot(t, envelope, label = "envelope of signal", color = 'b')
    plt.ylabel("Amplitude")
    plt.legend()
    # plt.ylabel("envelope of signal")

    plt.subplot(313)
    plt.plot(t, bits, label = "digital signal", color = 'r')
    plt.ylabel("Amplitude")
    plt.legend()
    # plt.xlim((0.5, 20))
    # plt.ylim((0, 0.1))
    plt.tight_layout()
    plt.savefig(fileName, dpi = 600)

    
    return bits

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
if __name__ == '__main__':
    # 参数设置
    sample_rate = 310# 采样率 (Hz)
    carrier_freq = 1   # 载波频率 (Hz)
    symbol_duration = 1 # 符号持续时间 (秒)

    # 生成示例ASK信号 (实际使用时替换为真实信号)

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

    ask_signal = dataBz
    # 解调
    demodulated = demodulate_2ask(
        signal=ask_signal,
        sample_rate=sample_rate,
        carrier_freq=carrier_freq,
        symbol_duration=symbol_duration
    )

    # sample_rate = 310
    # carrier_freq = 1
    signal = demodulated
    bits, start_index= ask_to_bits(signal, sample_rate, carrier_freq)
    str_bits = uart_decode(bits)
    print(f"bits = {bits}")
    print(f"string = {str_bits}")
    # print("解调结果:", demodulated)  # 应输出 [1, 0, 1, 1, 0]
    plt.show()
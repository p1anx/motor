from os import stat_result
import matplotlib.pyplot as plt
from pandas.core.frame import DataFrameInfo
import mylib as m
import numpy as np

# m.plot_fft(data, fs)
# plt.show()

from datetime import datetime

def get_current_time() -> str:
    """返回当前时间字符串，精度为年月日时分秒。"""
    now = datetime.now()
    return now.strftime("%Y%m%d_%H:%M:%S")

# def qam_slice(file):
#     df = pd.read_csv(file)
#     data = df.to_numpy()
#     data = data[:, 0]
#     n_data = len(data)
#     print(f"n = {n_data}")
#     x = np.linspace(0, n_data - 1, n_data)
#     # plt.plot(x[5000:10000], data[5000:10000])
#     # for
#     step = 570
#     fs = 280
#     start = 430
#     magnitude_list = []
#     phase_list = []
#
#     for i in range(4):
#         # plt.figure(i)
#
#         end = start + step
#         x_segment = x[start:end]
#         data_segment = data[start:end]
#         start = end
#         plot_fft(x_segment, data_segment, fs, step)
#         magnitude, phase = find_amp_phase(x_segment, data_segment, fs)
#         magnitude_list.append(magnitude)
#         phase_list.append(phase)
#     plt_qam.plot_qam_constellation(magnitude_list, phase_list, M=16)
#     print(f"mag = {magnitude_list}")
#     # x = np.linspace(0, 50, 50)
#     # plt.figure(2)
#     # plt.plot(x, magnitude_list, label="magnitude")
#     plt.legend()
#
#     print(f"phase = {phase_list}")

# g_n_point = 20


def plot_qam_constellation(file, fs=280, start=231, step=270, g_n_point=10, column = 2):
    data = m.read_csv(file, column)
    # fs = 280
    # start = 430
    # step = 570

    magnitudes = []
    phases = []
    plt.figure()
    for i in range(g_n_point):
        # 每次按 step 取一段数据
        data_segment = data[start : start + step]
        # x 和 data_segment 保持相同长度
        x = range(start, start + len(data_segment))
        # print(f"x = {len(x)}, data = {len(data_segment)}")
        plt.plot(x, data_segment)

        start = start + step
        magnitude, phase = m.get_magnitude_phase(data_segment, fs)
        magnitudes.append(magnitude)
        phases.append(phase)

    # plt.subplot(212)
    plt.figure()
    m.plot_qam_constellation(
        magnitudes, phases, 5, show_decision_regions=True, set_tag=False
    )
    # m.plot_fft_magnitude(data_segment, fs)


def plot_qam_constellation_filtered(file, fs=284, start=231, step=270, g_n_point=10, column=2):
    data = m.read_csv(file, column)
    # fs = 280
    # start = 430
    # step = 570

    magnitudes = []
    phases = []

    # 根据数据长度自动计算最多可以分成多少段
    n_total = len(data)
    remain = max(0, n_total - start)
    if remain <= 0:
        return
    max_segments = remain // step + (1 if remain % step != 0 else 0)
    n_segments = min(g_n_point, max_segments)

    for i in range(n_segments):
        # plt.subplot(211)
        data_segment = data[start : start + step]
        # 段太短时跳过，避免 filtfilt 报错
        if len(data_segment) <= 12:
            start = start + step
            continue
        # x 和 data_segment 保持相同长度
        x = range(start, start + len(data_segment))
        # print(f"x = {len(x)}, data = {len(data_segment)}")
        plt.subplot(211)
        plt.plot(x, data_segment)
        plt.title("without filtered")
        # m.butter_bandpass()
        # plt.figure()

        # data_segment = m.bandpass_filter(data_segment, 0.01, 2, fs)
        # data_filtered = m.bandpass_filter(data_segment, 1, 2, fs)
        data_filtered = m.lowpass_filter(data_segment, 2, fs, order=3)
        data_segment = data_filtered

        # plt.figure()
        # plt.plot(np.arange(len(data_filtered)), data_filtered)
        # m.plot_fft_magnitude(data_filtered, fs)
        plt.subplot(212)
        plt.plot(x, data_segment)
        plt.title("filtered")
        plt.tight_layout()

        start = start + step
        magnitude, phase = m.get_magnitude_phase(data_segment, fs)
        magnitudes.append(magnitude)
        phases.append(phase)

    # plt.subplot(212)
    # plt.savefig(f"./{file}_time.png", dpi=300)

    #1.
    time_str = get_current_time()
    plt.savefig(
        f"./{file}_{g_n_point}_step{step}_{time_str}.png",
        dpi=300,
    )
    plt.figure()
    m.plot_qam_constellation(
        magnitudes, phases, 4, show_decision_regions=True, set_tag=False
    )


def plot_qam_constellation_test(file, fs=284, start=231, step=270, g_n_point=10):
    data = m.read_csv(file)
    # fs = 280
    # start = 430
    # step = 570

    magnitudes = []
    phases = []

    # 根据数据长度自动计算最多可以分成多少段
    n_total = len(data)
    remain = max(0, n_total - start)
    if remain <= 0:
        return
    max_segments = remain // step + (1 if remain % step != 0 else 0)
    n_segments = min(g_n_point, max_segments)

    for i in range(n_segments):
        # plt.subplot(211)
        data_segment = data[start : start + step]
        # 段太短时跳过
        if len(data_segment) <= 12:
            start = start + step
            continue
        # x 和 data_segment 保持相同长度
        x = range(start, start + len(data_segment))
        # print(f"x = {len(x)}, data = {len(data_segment)}")

        plt.subplot(211)
        plt.plot(x, data_segment)
        plt.title("without filtered")
        # m.butter_bandpass()
        # plt.figure()

        # data_segment = m.bandpass_filter(data_segment, 0.01, 2, fs)
        if 1:
            # data_filtered = m.bandpass_filter(data_segment, 1, 2, fs)
            data_filtered = m.lowpass_filter(data_segment, 2, fs, order=3)
            data_segment = data_filtered

        # plt.figure()
        # plt.plot(np.arange(len(data_filtered)), data_filtered)
        # m.plot_fft_magnitude(data_filtered, fs)
        plt.subplot(212)
        plt.plot(x, data_segment)
        plt.title("filtered")
        plt.tight_layout()

        start = start + step
        magnitude, phase = m.get_magnitude_phase(data_segment, fs)
        magnitudes.append(magnitude)
        phases.append(phase)

    # plt.subplot(212)
    plt.savefig(f"./{file}_time.png", dpi=300)
    plt.figure()
    m.plot_qam_constellation(
        magnitudes, phases, 5, show_decision_regions=True, set_tag=False
    )


def plot_fft_1(file):
    data = m.read_csv(file)
    fs = 280
    data_filtered = m.lowpass_filter(data, 2, fs)
    plt.figure()
    m.plot_fft_magnitude(data, fs)
    plt.figure()
    m.plot_fft_magnitude(data_filtered, fs)


def plot_constellation_for_qam4_1Hz_1s():
    xyz = "y"
    g_n_point = 10
    step = 285
    folderName = "data0/qam4_1Hz_1s_0"
    file = f"./{folderName}B{xyz}.csv"

    plot_qam_constellation(file, start=278, step=step, g_n_point=g_n_point)
    plt.savefig(f"{folderName}_{g_n_point}p_{xyz}_step{step}.png", dpi=300)


def plot_constellation_for_qam4_1Hz_1_05s():
    xyz = "y"
    g_n_point = 10
    start = 690  # 84
    step = 294  # 280, 1180
    folderName = "data1/qam4_1Hz_1s_0/"
    file = f"./{folderName}/B{xyz}.csv"

    # plot_qam_constellation_test(file, start=start, step=step, g_n_point=g_n_point)
    plot_qam_constellation_filtered(file, start=start, step=step, g_n_point=g_n_point)

    # plt.savefig(f"{folderName}_{g_n_point}p_{xyz}_step{step}.png", dpi=300)
    plt.savefig(
        f"{folderName}_{g_n_point}p_{xyz}_step{step}_test_filtered_bandpass.png",
        dpi=300,
    )


def plot_constellation_for_data_after_t():
    xyz = "z"
    g_n_point = 20
    # start = 293  # 84
    # step = 285  # 280, 1180

    # start = 427  # 84
    # step = 285  # 280, 1180
    # folderName = "data_after_offset_t/qam4_1Hz_1s_1/"
    #
    # start = 550  # 84
    # step = 285  # 280, 1180
    # folderName = "data_after_offset_t/qam4_1Hz_1s_2/"

    start = 408  # 84
    step = 280  # 280, 1180
    folderName = "data_after_offset_t/qam4_1Hz_1s_7/"
    file = f"./{folderName}/B{xyz}.csv"

    # plot_qam_constellation_test(file, start=start, step=step, g_n_point=g_n_point)
    plot_qam_constellation_filtered(file, start=start, step=step, g_n_point=g_n_point)

    # plt.savefig(f"{folderName}_{g_n_point}p_{xyz}_step{step}.png", dpi=300)
    plt.savefig(
        f"{folderName}_{g_n_point}p_{xyz}_step{step}_test_filtered_bandpass.png",
        dpi=300,
    )



def plot_constellation_for_equal_amplitude():
    img_folder = "data2026317"
    time_str = get_current_time()
    g_n_point = 8

    start = 0  # 84
    step = 260# 280, 1180
    # folderName = "data_after_offset_t/qam4_1Hz_1s_7/"
    # file = f"./{folderName}/B{xyz}.csv"

    # csv_file = f"./data_amplitude/qam16_1.csv"
    csv_file = f"./data2026317/sin_30_20_20_20.csv"
    # csv_file = f"./data2026317/sin_10_10_10_10.csv"

    # 绘制并保存时域波形（上下两幅）
    plot_qam_constellation_filtered(
        csv_file, fs=200, start=start, step=step, g_n_point=g_n_point
    )

    # 绘制并保存星座图（在 plot_qam_constellation_filtered 末尾已生成 figure）
    plt.savefig(
        f"./{img_folder}/qam_constellation_g{g_n_point}_step{step}_{time_str}.png",
        dpi=300,
    )


if __name__ == "__main__":
    # file = "test0/Bx.csv"
    # 1.
    # xyz = "y"
    # step = 285
    # folderName = "data0/qam4_1Hz_1s_0"
    # file = f"./{folderName}/B{xyz}.csv"
    # xyz = "y"
    # step = 285
    # folderName = "data0/qam4_1Hz_1s_0"
    # file = f"./{folderName}B{xyz}.csv"

    # plot_qam_constellation(file, start=855)
    # plot_qam_constellation(file, start=420)
    # plt.savefig(f"{folderName}/{folderName}_{g_n_point}p_{xyz}.png", dpi=300)

    # file = "./qam4_2Hz_2s_1/Bz.csv"
    # plot_qam_constellation(file, start=231)
    # plt.savefig("qam4_2Hz_2s_1/qam4_1Hz_2s_0_10p.png", dpi=300)

    # 2.
    # xyz = "y"
    # g_n_point = 10
    # start = 293  # 84
    # step = 294  # 280, 1180
    # # folderName = "data_after_offset_t/qam4_1Hz_1s_0/"
    # folderName = "fft_sample_test"
    # file = f"./{folderName}/B{xyz}.csv"
    # # plot_fft_1(file)
    # # plot_fft_1("./data1/qam4_1Hz_1s_0/Bx.csv")
    # # plot_constellation_for_qam4_1Hz_1_05s()
    # plot_constellation_for_data_after_t()
    # plt.show()
    #3.
    plot_constellation_for_equal_amplitude()
    plt.show()

    
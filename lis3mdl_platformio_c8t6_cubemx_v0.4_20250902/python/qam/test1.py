import numpy as np
import matplotlib.pyplot as plt


def test():
    t_change = [36, 18, 31, 29]
    t_change = np.array(t_change)
    # y = [642, 1847, 3018, 4198, 5361]
    y = [938, 1232, 1531, 1827]
    x = np.arange(len(y))

    y_list = []
    for i in range(len(y) - 1):
        y_delata = y[i + 1] - y[i]
        y_list.append(y_delata)

    print(f"delta = {y_list}")
    y_list = np.array(y_list)

    fs = 285
    print(f"{y_list / fs}s, {1200 / 4.2} Hz, change time = {t_change / fs} s")

    # plt.plot(x, y)
    # plt.show()

    # print("hello")


def calc_interval():
    fs = 280
    # interval_point = [1812, 2054, 2312, 2556, 2819, 3074, 3337, 3573, 3842]
    # interval_point = [529, 839, 1261, 1780, 2303, 2597]
    # interval_point = [998, 1311, 1714, 2245]
    # interval_point = [968, 992]
    # interval_point = [1243, 1313]
    # interval_point = [2188, 2249]
    # interval_point = [2718, 2748]
    #
    # interval_point = [447, 762, 1003, 1302, 1579, 1889]
    interval_point = [759, 1047, 1319, 1618, 1889, 2180]
    # 1. 0.085 (2)0.25 (3) 0.135 (4)0.21

    y = interval_point
    y_list = []
    for i in range(len(y) - 1):
        y_delata = y[i + 1] - y[i]
        y_list.append(y_delata)

    y_list = np.array(y_list)
    print(f"delta point = {y_list}, delta_time = {y_list / fs} s")


def calc_t():
    fs = 280
    t_sample = [
        [1236, 1258],
        [1379, 1412],
        [1659, 1686],
        [2095, 2117],
        [2597, 2616],
        [2737, 2772],
    ]
    t = []
    print(len(t_sample))
    for i in range(len(t_sample)):
        t.append(t_sample[i][1] - t_sample[i][0])
    t = np.array(t)
    t = t / fs
    print(t)

    # t = [0.085, 0.25, 0.135, 0.21]
    # t1 = t[0]
    # t2 = t[1]

    should_set_interval = []
    bit_update_time = 1
    n_t = len(t)
    for i in range(len(t)):
        if i == n_t - 1:
            should_set_interval.append(bit_update_time - (t[0] - t[n_t - 1]))
        else:
            should_set_interval.append(bit_update_time - (t[i + 1] - t[i]))

    print(f"should_set_interval = {should_set_interval}")


# [0.835, 1.115, 0.925, 1.125] s
# [0.07857143 0.11785714 0.09642857 0.07857143 0.06785714 0.125     ]
# should_set_interval = [np.float64(0.9607142857142857), np.float64(1.0214285714285714), np.float64(1.0178571428571428), np.float64(1.0107142857142857),
# np.float64(0.9428571428571428), np.float64(1.0464285714285715)]


if __name__ == "__main__":
    # test()

    # a = 2054 - 1812
    # print(a)
    # 1.
    calc_interval()
    # 2.
    # calc_t()

    # (x - t1) + t2 = n

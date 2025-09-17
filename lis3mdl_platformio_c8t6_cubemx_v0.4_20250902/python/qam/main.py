import mylib as m
import matplotlib.pyplot as plt


if __name__ == "__main__":
    xyz = "y"
    # g_n_point = 20*5
    g_n_point = 16*2

    start = 2  # 84
    step = 283  # 280, 1180
    folderName = "data/qam16_fc2Hz1Hz_2"
    file = f"./{folderName}/B{xyz}.csv"
    m.plot_constellation(xyz= xyz, g_n_point = g_n_point, start =start, step = step, folderName =folderName)
    # m.plot_constellation()
    plt.show()

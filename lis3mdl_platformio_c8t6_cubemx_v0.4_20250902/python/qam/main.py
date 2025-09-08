import mylib as m
import matplotlib.pyplot as plt


if __name__ == "__main__":
    xyz = "y"
    # g_n_point = 20*5
    g_n_point = 16

    start = 549  # 84
    step = 283  # 280, 1180
    folderName = "data/16qam_fc2Hz1Hz_0"
    file = f"./{folderName}/B{xyz}.csv"
    m.plot_constellation(xyz= "x", g_n_point = g_n_point, start =1833, step = 283, folderName =folderName)
    # m.plot_constellation()
    plt.show()

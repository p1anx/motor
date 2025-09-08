import mylib as m
import matplotlib.pyplot as plt

def read():
    file = "./data/1Hz1Hz_0/Bx.csv"
    data = m.read_csv(file)
    fs = 320
    m.plot_fft_magnitude(data, fs)
    # m.plot(data)


if __name__ == "__main__":
    read()
    plt.show()
    
import serial
import serial.tools.list_ports
import matplotlib.pyplot as plt
import numpy as np
import pandas as pd
import os
# 配置串口参数
def serial_plot_only():
    ser = serial.Serial(
        port='/dev/ttyACM0',          # 替换为你的串口号
        baudrate=115200,     # 波特率
        bytesize=8,          # 数据位
        parity='N',          # 校验位
        stopbits=1,          # 停止位
        timeout=1            # 读取超时时间(秒)
    )
    try:
        Bx_lists = []
        By_lists = []
        Bz_lists = []
        sample_per_bit = 310
        rxData_num = sample_per_bit * 10*2

        saved_file_path = "./test0/"
        if not os.path.exists(saved_file_path):
            os.makedirs(saved_file_path)
            print("folder is create")

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
                    Bx_lists.append(float(Bx))
                    By_lists.append(float(By))
                    Bz_lists.append(float(Bz))
                    # By_lists = np.array(By_lists)
                    Bx_lists_n = len(Bx_lists)

                    print(f"n_data = {len(Bx_lists)}", end="\r", flush=True)
                    if(Bx_lists_n > rxData_num):
                        pd.DataFrame(Bx_lists).to_csv(saved_file_path+"Bx.csv", index=False, header=False)
                        pd.DataFrame(By_lists).to_csv(saved_file_path+"By.csv", index=False, header=False)
                        pd.DataFrame(Bz_lists).to_csv(saved_file_path+"Bz.csv", index=False, header=False)


                        t = np.linspace(0, Bx_lists_n, Bx_lists_n)
                        plt.subplot(311)
                        plt.plot(t, Bx_lists,  label = "Bx")
                        plt.legend()

                        plt.subplot(312)
                        plt.plot(t, By_lists,  label = "By")
                        plt.legend()

                        plt.subplot(313)
                        plt.plot(t, Bz_lists,  label = "Bz")
                        plt.legend()

                        plt.savefig(saved_file_path+"qam.png", dpi = 300)
                        plt.tight_layout()
                        break
                
    except KeyboardInterrupt:
        print("程序终止")
        
    finally:
        ser.close()  # 关闭串口

if __name__ == "__main__":
    serial_plot_only()
    plt.show()
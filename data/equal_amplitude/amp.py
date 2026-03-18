import serial
import threading
import time
from typing import Union

import matplotlib.pyplot as plt
import numpy as np
import pandas as pd

# 打印锁，避免两路接收输出交错
_print_lock = threading.Lock()


def send_cmd_to_two_ports(
    port1: str,
    port2: str,
    cmd: Union[str, bytes],
    baudrate1: int = 115200,
    baudrate2: int = 115200,
    encoding: str = "utf-8",
    add_newline: bool = True,
    timeout: float = 1.0,
) -> None:
    """
    将同一条命令同时发送到两个串口。

    port1, port2: 串口号，例如 'COM3' 或 '/dev/ttyUSB0'
    cmd: 要发送的命令，字符串或 bytes
    baudrate: 波特率
    encoding: 字符串时使用的编码
    add_newline: 是否在结尾自动添加 '\r\n'
    timeout: 串口读写超时时间（秒）
    """
    # 处理待发送数据
    if isinstance(cmd, str):
        if add_newline:
            cmd_to_send = (cmd + "\n").encode(encoding)
        else:
            cmd_to_send = cmd.encode(encoding)
    else:
        cmd_to_send = cmd

    ser1 = None
    ser2 = None
    try:
        ser1 = serial.Serial(port=port1, baudrate=baudrate1, timeout=timeout)
        ser2 = serial.Serial(port=port2, baudrate=baudrate2, timeout=timeout)

        ser1.write(cmd_to_send)
        ser1.flush()
        ser2.write(cmd_to_send)
        ser2.flush()

    finally:
        if ser1 is not None and ser1.is_open:
            ser1.close()
        if ser2 is not None and ser2.is_open:
            ser2.close()


def _reader_loop(
    port_name: str,
    ser: serial.Serial,
    stop_event: threading.Event,
    encoding: str = "utf-8",
) -> None:
    """后台线程：持续读串口并打印，直到 stop_event 被置位."""
    try:
        while not stop_event.is_set():
            if ser.in_waiting:
                data = ser.read(ser.in_waiting)
                text = data.decode(encoding, errors="replace")
                with _print_lock:
                    print(f"[{port_name}] {text}", end="", flush=True)
            else:
                time.sleep(0.02)
    except Exception as e:
        if not stop_event.is_set():
            with _print_lock:
                print(f"[{port_name}] error: {e}")
    finally:
        try:
            ser.close()
        except Exception:
            pass


def send_and_receive_two_ports(
    port1: str,
    port2: str,
    cmd: Union[str, bytes],
    baudrate: int = 115200,
    encoding: str = "utf-8",
    add_newline: bool = True,
    timeout: float = 0.1,
    recv_duration: float = 5.0,
) -> None:
    """
    边发送边接收：先打开两个串口并启动接收线程，再发送命令，然后在一段时间内持续打印两路接收数据。

    port1, port2: 串口号
    cmd: 要发送的命令（字符串或 bytes）
    recv_duration: 发送后继续接收的时长（秒），期间会打印来自两个串口的数据
    timeout: 串口 read 超时（秒），用于后台读线程
    """
    if isinstance(cmd, str):
        cmd_to_send = (cmd + "\r\n").encode(encoding) if add_newline else cmd.encode(encoding)
    else:
        cmd_to_send = cmd

    ser1 = serial.Serial(port=port1, baudrate=baudrate, timeout=timeout)
    ser2 = serial.Serial(port=port2, baudrate=baudrate, timeout=timeout)

    stop = threading.Event()
    t1 = threading.Thread(target=_reader_loop, args=(port1, ser1, stop, encoding))
    t2 = threading.Thread(target=_reader_loop, args=(port2, ser2, stop, encoding))
    t1.daemon = True
    t2.daemon = True
    t1.start()
    t2.start()

    try:
        # 边接收边发送：先发命令
        ser1.write(cmd_to_send)
        ser1.flush()
        ser2.write(cmd_to_send)
        ser2.flush()

        # 持续接收一段时间
        time.sleep(recv_duration)
    finally:
        stop.set()
        t1.join(timeout=1.0)
        t2.join(timeout=1.0)
        try:
            ser1.close()
        except Exception:
            pass
        try:
            ser2.close()
        except Exception:
            pass


def collect_and_plot_three(
    port: str,
    baudrate: int = 115200,
    encoding: str = "utf-8",
    duration: float = 5.0,
    csv_path: str | None = "data.csv",
) -> None:
    """
    从指定串口采集格式为 "f1,f2,f3\\n" 的三路数据，并绘制曲线。

    每一行形如: "1.23,4.56,7.89\\n"
    三个通道依次画为三条曲线，横轴为样本序号。
    """
    ser = serial.Serial(port=port, baudrate=baudrate, timeout=0.1)

    ch1: list[float] = []
    ch2: list[float] = []
    ch3: list[float] = []

    t0 = time.time()
    try:
        while time.time() - t0 < duration:
            line = ser.readline()
            if not line:
                continue

            try:
                text = line.decode(encoding, errors="replace").strip()
                if not text:
                    continue
                parts = text.split(",")
                if len(parts) != 3:
                    continue
                v1, v2, v3 = map(float, parts)
            except ValueError:
                # 非法格式或无法转成 float，忽略该行
                continue

            ch1.append(v1)
            ch2.append(v2)
            ch3.append(v3)
    finally:
        ser.close()

    if not ch1:
        print("未采集到有效的 \"f,f,f\" 三通道数据")
        return

    # 可选：使用 pandas 保存到 CSV 文件
    if csv_path is not None:
        try:
            df = pd.DataFrame({"ch1": ch1, "ch2": ch2, "ch3": ch3})
            df.to_csv(csv_path, index=False)
            print(f"数据已保存到 CSV: {csv_path}")
        except OSError as e:
            print(f"保存 CSV 失败: {e}")

    x = range(len(ch1))
    plt.figure()
    plt.plot(x, ch1, label="ch1")
    plt.plot(x, ch2, label="ch2")
    plt.plot(x, ch3, label="ch3")
    plt.xlabel("sample index")
    plt.ylabel("value")
    plt.legend()
    plt.grid(True)
    plt.tight_layout()
    plt.show()


def equal_amplitude_phase_points(
    one_period: list[float] | np.ndarray,
    num_levels: int,
    start_phase_deg: float = 360.0 * 0.01,
    end_phase_deg: float = 360.0 * 0.99,
) -> tuple[np.ndarray, np.ndarray]:
    """
    在一个周期正弦数据中，求等幅度的相位点。

    - one_period: 一周期采样数据（长度 N，对应 0~360 度）
    - num_levels: 需要多少个等幅度电平
    - 返回 (phase_deg_array, amp_array)
        phase_deg_array: 每个等幅度对应的相位（度）
        amp_array:      对应的幅度值
    """
    y = np.asarray(one_period, dtype=float)
    N = y.size
    if N < 2 or num_levels <= 0:
        return np.array([]), np.array([])

    phases = np.linspace(0.0, 360.0, N, endpoint=False)
    mask = (phases >= start_phase_deg) & (phases <= end_phase_deg)
    if not np.any(mask):
        return np.array([]), np.array([])

    phases_win = phases[mask]
    y_win = y[mask]

    y_min, y_max = float(y_win.min()), float(y_win.max())
    target_amps = np.linspace(y_min, y_max, num_levels)

    phase_points: list[float] = []
    amps: list[float] = []

    for A in target_amps:
        diff = y_win - A
        sign = np.sign(diff)
        idx = np.where(sign[:-1] * sign[1:] <= 0)[0]
        if idx.size == 0:
            continue
        k = int(idx[0])
        x0, x1 = phases_win[k], phases_win[k + 1]
        y0, y1 = y_win[k], y_win[k + 1]
        if y1 == y0:
            phi = x0
        else:
            phi = x0 + (A - y0) * (x1 - x0) / (y1 - y0)
        phase_points.append(float(phi))
        amps.append(float(A))

    return np.asarray(phase_points), np.asarray(amps)


def start_two_and_plot_one(
    tx_port1: str,
    tx_port2: str,
    rx_port: str,
    cmd: Union[str, bytes] = "start",
    baudrate1: int = 115200,
    baudrate2: int = 115200,
    encoding: str = "utf-8",
    add_newline: bool = True,
    duration: float = 5.0,
) -> None:
    """
    先向两个串口发送同一命令（默认 \"start\"），然后只从一个串口接收三通道数据并绘图。

    - tx_port1, tx_port2: 需要下发启动命令的两个串口
    - rx_port: 实际回传 \"f,f,f\\n\" 数据的那个串口
    """
    # 1) 先给两个串口发命令
    send_cmd_to_two_ports(
        port1=tx_port1,
        port2=tx_port2,
        cmd=cmd,
        baudrate1=baudrate1,
        baudrate2=baudrate2,
        encoding=encoding,
        add_newline=add_newline,
        timeout=1.0,
    )

    # 2) 然后只从 rx_port 接收并画图
    collect_and_plot_three(
        port=rx_port,
        baudrate=baudrate1,
        encoding=encoding,
        duration=duration,
    )


if __name__ == "__main__":
    # 示例：先向两个串口发送 \"start\"，再从其中一个串口接收三通道并绘图
    start_two_and_plot_one(
        tx_port1="/dev/ttyUSB0",  # 第一个发送端口
        tx_port2="/dev/ttyUSB0",  # 第二个发送端口
        rx_port="/dev/ttyUSB0",   # 实际回传数据的端口（根据你的硬件修改）
        cmd="a",
        baudrate1=115200,
        baudrate2=2000000,
        duration=0.5*20,             # 接收并绘图前采集 5 秒
    )
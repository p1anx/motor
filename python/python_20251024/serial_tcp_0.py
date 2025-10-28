import mylib.mylib as m
import mylib.my_serial as ser
from simple_tcp_server import SimpleTCPServer

if __name__ == '__main__':
    port = '/dev/ttyACM1'
    
    print("选择运行模式:")
    print("1. 运行简单TCP服务器")
    print("2. 运行串口测试")
    
    choice = input("请输入选择 (1 或 2): ").strip()
    
    if choice == '1':
        # 运行简单的TCP服务器
        server = SimpleTCPServer(host='localhost', port=8080)
        try:
            server.start()
        except KeyboardInterrupt:
            print("\n服务器被用户中断")
            server.stop()
    else:
        # 运行串口测试
        # ser.test_thread_rx(port)
        ser.test_read3data()
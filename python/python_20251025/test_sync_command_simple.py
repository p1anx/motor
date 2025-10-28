#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
简单测试同步指令功能
"""

import sys
import os

# 添加当前目录到Python路径
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))

try:
    from tcp_uart_unified_app_main_v2 import TCPUARTUnifiedApp
    from PyQt5.QtWidgets import QApplication
    from PyQt5.QtCore import QTimer
    
    def test_sync_command_simple():
        """简单测试同步指令功能"""
        app = QApplication(sys.argv)
        window = TCPUARTUnifiedApp()
        
        print("=== 简单测试同步指令功能 ===")
        
        # 显示窗口
        window.show()
        
        # 设置定时器自动关闭
        timer = QTimer()
        timer.timeout.connect(app.quit)
        timer.start(20000)  # 20秒后自动关闭
        
        print("应用已启动，20秒后自动关闭")
        print("\n请按以下步骤测试:")
        print("1. 点击'同步控制'Tab")
        print("2. 在'同步指令设置'区域输入测试指令")
        print("3. 点击'发送同步指令'按钮")
        print("4. 查看是否有错误消息或成功消息")
        print("\n如果按钮是灰色的，请先连接TCP或UART")
        
        return app.exec_()
    
    if __name__ == '__main__':
        test_sync_command_simple()
        
except ImportError as e:
    print(f"导入错误: {e}")
    print("请确保已安装PyQt5: pip install PyQt5")
    sys.exit(1)
except Exception as e:
    print(f"运行错误: {e}")
    import traceback
    traceback.print_exc()
    sys.exit(1)

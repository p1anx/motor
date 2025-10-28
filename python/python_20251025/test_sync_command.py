#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
测试同步指令功能
验证可以同时发送指令给TCP和UART
"""

import sys
import os

# 添加当前目录到Python路径
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))

try:
    from tcp_uart_unified_app_main_v2 import TCPUARTUnifiedApp
    from PyQt5.QtWidgets import QApplication
    from PyQt5.QtCore import QTimer
    
    def test_sync_command():
        """测试同步指令功能"""
        app = QApplication(sys.argv)
        window = TCPUARTUnifiedApp()
        
        # 测试1: 检查同步指令功能
        print("测试1: 检查同步指令功能")
        if window.sync_page and hasattr(window.sync_page, 'sync_command_input'):
            print(f"同步指令输入框存在: {window.sync_page.sync_command_input.text()}")
            print(f"发送同步指令按钮存在: {hasattr(window.sync_page, 'send_sync_command_button')}")
        else:
            print("同步指令功能未找到")
        
        # 测试2: 检查同步指令按钮状态
        print("\n测试2: 检查同步指令按钮状态")
        if window.sync_page and hasattr(window.sync_page, 'send_sync_command_button'):
            print(f"发送同步指令按钮启用状态: {window.sync_page.send_sync_command_button.isEnabled()}")
        
        # 测试3: 模拟连接状态
        print("\n测试3: 模拟连接状态")
        window.update_sync_command_button()
        if window.sync_page and hasattr(window.sync_page, 'send_sync_command_button'):
            print(f"更新后按钮状态: {window.sync_page.send_sync_command_button.isEnabled()}")
        
        # 显示窗口
        window.show()
        
        # 设置定时器自动关闭
        timer = QTimer()
        timer.timeout.connect(app.quit)
        timer.start(10000)  # 10秒后自动关闭
        
        print("\n应用已启动，10秒后自动关闭")
        print("请手动测试以下功能:")
        print("1. 进入同步控制页面")
        print("2. 查看同步指令设置区域")
        print("3. 设置同步指令内容")
        print("4. 连接TCP或UART")
        print("5. 点击发送同步指令按钮")
        print("6. 观察同步指令发送结果")
        
        return app.exec_()
    
    if __name__ == '__main__':
        test_sync_command()
        
except ImportError as e:
    print(f"导入错误: {e}")
    print("请确保已安装PyQt5: pip install PyQt5")
    sys.exit(1)
except Exception as e:
    print(f"运行错误: {e}")
    sys.exit(1)

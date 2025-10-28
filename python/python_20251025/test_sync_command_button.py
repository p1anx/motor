#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
测试同步指令按钮状态更新
验证按钮状态是否正确更新
"""

import sys
import os

# 添加当前目录到Python路径
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))

try:
    from tcp_uart_unified_app_main_v2 import TCPUARTUnifiedApp
    from PyQt5.QtWidgets import QApplication
    from PyQt5.QtCore import QTimer
    
    def test_sync_command_button():
        """测试同步指令按钮状态"""
        app = QApplication(sys.argv)
        window = TCPUARTUnifiedApp()
        
        # 测试1: 检查初始状态
        print("测试1: 检查初始状态")
        print(f"TCP连接状态: {window.tcp_thread is not None and window.tcp_thread.isRunning() if window.tcp_thread else False}")
        print(f"UART连接状态: {window.uart_thread is not None and window.uart_thread.isRunning() if window.uart_thread else False}")
        
        if window.sync_page and hasattr(window.sync_page, 'send_sync_command_button'):
            print(f"同步指令按钮存在: True")
            print(f"同步指令按钮启用状态: {window.sync_page.send_sync_command_button.isEnabled()}")
        else:
            print("同步指令按钮未找到")
        
        # 测试2: 手动更新按钮状态
        print("\n测试2: 手动更新按钮状态")
        window.update_sync_command_button()
        if window.sync_page and hasattr(window.sync_page, 'send_sync_command_button'):
            print(f"更新后按钮状态: {window.sync_page.send_sync_command_button.isEnabled()}")
        
        # 测试3: 切换到同步页面
        print("\n测试3: 切换到同步页面")
        window.show_sync_page()
        if window.sync_page and hasattr(window.sync_page, 'send_sync_command_button'):
            print(f"切换页面后按钮状态: {window.sync_page.send_sync_command_button.isEnabled()}")
        
        # 显示窗口
        window.show()
        
        # 设置定时器自动关闭
        timer = QTimer()
        timer.timeout.connect(app.quit)
        timer.start(10000)  # 10秒后自动关闭
        
        print("\n应用已启动，10秒后自动关闭")
        print("请手动测试以下功能:")
        print("1. 进入同步控制页面")
        print("2. 查看同步指令按钮状态（应该禁用）")
        print("3. 连接TCP或UART")
        print("4. 返回同步控制页面，查看按钮是否启用")
        print("5. 尝试发送同步指令")
        
        return app.exec_()
    
    if __name__ == '__main__':
        test_sync_command_button()
        
except ImportError as e:
    print(f"导入错误: {e}")
    print("请确保已安装PyQt5: pip install PyQt5")
    sys.exit(1)
except Exception as e:
    print(f"运行错误: {e}")
    sys.exit(1)

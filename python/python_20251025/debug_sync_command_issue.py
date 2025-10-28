#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
调试同步指令发送问题
检查按钮状态、连接状态和发送功能
"""

import sys
import os

# 添加当前目录到Python路径
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))

try:
    from tcp_uart_unified_app_main_v2 import TCPUARTUnifiedApp
    from PyQt5.QtWidgets import QApplication
    from PyQt5.QtCore import QTimer
    
    def debug_sync_command_issue():
        """调试同步指令问题"""
        app = QApplication(sys.argv)
        window = TCPUARTUnifiedApp()
        
        print("=== 调试同步指令发送问题 ===")
        
        # 1. 检查同步页面和按钮
        print("1. 检查同步页面和按钮:")
        if window.sync_page:
            print(f"   同步页面存在: True")
            if hasattr(window.sync_page, 'send_sync_command_button'):
                button = window.sync_page.send_sync_command_button
                print(f"   同步指令按钮存在: True")
                print(f"   按钮对象: {button}")
                print(f"   按钮类型: {type(button)}")
                print(f"   按钮是否为None: {button is None}")
                if button is not None:
                    print(f"   按钮启用状态: {button.isEnabled()}")
                    print(f"   按钮文本: {button.text()}")
            else:
                print("   ❌ 同步指令按钮属性不存在!")
        else:
            print("   ❌ 同步页面不存在!")
        
        # 2. 检查连接状态
        print("\n2. 检查连接状态:")
        tcp_connected = window.tcp_thread and window.tcp_thread.isRunning()
        uart_connected = window.uart_thread and window.uart_thread.isRunning()
        print(f"   TCP连接状态: {tcp_connected}")
        print(f"   UART连接状态: {uart_connected}")
        print(f"   任一连接状态: {tcp_connected or uart_connected}")
        
        # 3. 检查按钮更新方法
        print("\n3. 测试按钮更新方法:")
        try:
            window.update_sync_command_button()
            print("   ✅ 按钮更新方法调用成功")
            if window.sync_page and hasattr(window.sync_page, 'send_sync_command_button'):
                button = window.sync_page.send_sync_command_button
                if button is not None:
                    print(f"   更新后按钮状态: {button.isEnabled()}")
        except Exception as e:
            print(f"   ❌ 按钮更新方法调用失败: {e}")
            import traceback
            traceback.print_exc()
        
        # 4. 检查同步指令输入框
        print("\n4. 检查同步指令输入框:")
        if window.sync_page and hasattr(window.sync_page, 'sync_command_input'):
            input_widget = window.sync_page.sync_command_input
            print(f"   输入框存在: True")
            print(f"   输入框对象: {input_widget}")
            print(f"   输入框类型: {type(input_widget)}")
            print(f"   输入框是否为None: {input_widget is None}")
            if input_widget is not None:
                print(f"   当前输入内容: '{input_widget.text()}'")
                print(f"   输入框启用状态: {input_widget.isEnabled()}")
        else:
            print("   ❌ 同步指令输入框不存在!")
        
        # 5. 测试切换到同步页面
        print("\n5. 测试切换到同步页面:")
        try:
            window.show_sync_page()
            print("   ✅ 切换到同步页面成功")
        except Exception as e:
            print(f"   ❌ 切换到同步页面失败: {e}")
            import traceback
            traceback.print_exc()
        
        # 6. 再次检查按钮状态
        print("\n6. 切换页面后检查按钮状态:")
        if window.sync_page and hasattr(window.sync_page, 'send_sync_command_button'):
            button = window.sync_page.send_sync_command_button
            if button is not None:
                print(f"   按钮启用状态: {button.isEnabled()}")
                print(f"   按钮是否可见: {button.isVisible()}")
        
        # 显示窗口
        window.show()
        
        # 设置定时器自动关闭
        timer = QTimer()
        timer.timeout.connect(app.quit)
        timer.start(15000)  # 15秒后自动关闭
        
        print("\n应用已启动，15秒后自动关闭")
        print("请手动测试以下功能:")
        print("1. 切换到'同步控制'Tab")
        print("2. 查看同步指令按钮是否可见和启用")
        print("3. 在输入框中输入测试指令")
        print("4. 点击'发送同步指令'按钮")
        print("5. 查看是否有错误消息或成功消息")
        
        return app.exec_()
    
    if __name__ == '__main__':
        debug_sync_command_issue()
        
except ImportError as e:
    print(f"导入错误: {e}")
    print("请确保已安装PyQt5: pip install PyQt5")
    sys.exit(1)
except Exception as e:
    print(f"运行错误: {e}")
    import traceback
    traceback.print_exc()
    sys.exit(1)

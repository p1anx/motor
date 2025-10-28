#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
测试同步指令按钮修复
验证按钮状态更新不会出现NoneType错误
"""

import sys
import os

# 添加当前目录到Python路径
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))

try:
    from tcp_uart_unified_app_main_v2 import TCPUARTUnifiedApp
    from PyQt5.QtWidgets import QApplication
    from PyQt5.QtCore import QTimer
    
    def test_sync_command_fix():
        """测试同步指令按钮修复"""
        app = QApplication(sys.argv)
        window = TCPUARTUnifiedApp()
        
        print("测试1: 检查同步页面初始化")
        if window.sync_page:
            print(f"同步页面存在: True")
            if hasattr(window.sync_page, 'send_sync_command_button'):
                print(f"同步指令按钮属性存在: True")
                if window.sync_page.send_sync_command_button is not None:
                    print(f"同步指令按钮对象存在: True")
                else:
                    print(f"同步指令按钮对象为None: True")
            else:
                print(f"同步指令按钮属性不存在: True")
        else:
            print(f"同步页面不存在: True")
        
        print("\n测试2: 安全调用更新方法")
        try:
            window.update_sync_command_button()
            print("更新方法调用成功，没有错误")
        except Exception as e:
            print(f"更新方法调用失败: {e}")
        
        print("\n测试3: 切换到同步页面")
        try:
            window.show_sync_page()
            print("切换到同步页面成功")
        except Exception as e:
            print(f"切换到同步页面失败: {e}")
        
        # 显示窗口
        window.show()
        
        # 设置定时器自动关闭
        timer = QTimer()
        timer.timeout.connect(app.quit)
        timer.start(5000)  # 5秒后自动关闭
        
        print("\n应用已启动，5秒后自动关闭")
        print("请手动测试以下功能:")
        print("1. 进入同步控制页面")
        print("2. 查看同步指令按钮是否存在")
        print("3. 连接TCP或UART")
        print("4. 返回同步控制页面，查看按钮状态")
        
        return app.exec_()
    
    if __name__ == '__main__':
        test_sync_command_fix()
        
except ImportError as e:
    print(f"导入错误: {e}")
    print("请确保已安装PyQt5: pip install PyQt5")
    sys.exit(1)
except Exception as e:
    print(f"运行错误: {e}")
    sys.exit(1)

#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
测试同步模式开关功能
验证用户可以手动控制是否使用同步
"""

import sys
import os

# 添加当前目录到Python路径
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))

try:
    from tcp_uart_unified_app_main_v2 import TCPUARTUnifiedApp
    from PyQt5.QtWidgets import QApplication
    from PyQt5.QtCore import QTimer
    
    def test_sync_mode_switch():
        """测试同步模式开关功能"""
        app = QApplication(sys.argv)
        window = TCPUARTUnifiedApp()
        
        # 测试1: 检查同步模式开关
        print("测试1: 检查同步模式开关")
        print(f"初始同步模式状态: {window.sync_enabled}")
        if window.sync_page and hasattr(window.sync_page, 'sync_mode_check'):
            print(f"同步模式开关存在: {window.sync_page.sync_mode_check.isChecked()}")
            print(f"同步模式开关启用状态: {window.sync_page.sync_mode_check.isEnabled()}")
        else:
            print("同步模式开关未找到")
        
        # 测试2: 检查相关控件状态
        print("\n测试2: 检查相关控件状态")
        if window.sync_page:
            print(f"自动同步开关启用状态: {window.sync_page.auto_sync_check.isEnabled()}")
            print(f"同步按钮启用状态: {window.sync_page.sync_button.isEnabled()}")
            print(f"同步状态标签: {window.sync_page.sync_status_label.text()}")
        
        # 测试3: 模拟启用同步模式
        print("\n测试3: 模拟启用同步模式")
        if window.sync_page and hasattr(window.sync_page, 'sync_mode_check'):
            window.sync_page.sync_mode_check.setChecked(True)
            window.sync_page.on_sync_mode_toggled(True)
            print(f"启用后自动同步开关状态: {window.sync_page.auto_sync_check.isEnabled()}")
            print(f"启用后同步按钮状态: {window.sync_page.sync_button.isEnabled()}")
            print(f"启用后同步状态: {window.sync_page.sync_status_label.text()}")
        
        # 显示窗口
        window.show()
        
        # 设置定时器自动关闭
        timer = QTimer()
        timer.timeout.connect(app.quit)
        timer.start(10000)  # 10秒后自动关闭
        
        print("\n应用已启动，10秒后自动关闭")
        print("请手动测试以下功能:")
        print("1. 进入同步控制页面")
        print("2. 查看同步模式开关（默认关闭）")
        print("3. 勾选'启用同步模式'")
        print("4. 观察其他控件的启用/禁用状态")
        print("5. 取消勾选同步模式，观察状态变化")
        print("6. 测试连接TCP/UART时的同步行为")
        
        return app.exec_()
    
    if __name__ == '__main__':
        test_sync_mode_switch()
        
except ImportError as e:
    print(f"导入错误: {e}")
    print("请确保已安装PyQt5: pip install PyQt5")
    sys.exit(1)
except Exception as e:
    print(f"运行错误: {e}")
    sys.exit(1)

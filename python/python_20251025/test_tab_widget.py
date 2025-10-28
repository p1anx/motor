#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
测试TabWidget功能
验证页面切换是否正常工作
"""

import sys
import os

# 添加当前目录到Python路径
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))

try:
    from tcp_uart_unified_app_main_v2 import TCPUARTUnifiedApp
    from PyQt5.QtWidgets import QApplication
    from PyQt5.QtCore import QTimer
    
    def test_tab_widget():
        """测试TabWidget功能"""
        app = QApplication(sys.argv)
        window = TCPUARTUnifiedApp()
        
        print("=== 测试TabWidget功能 ===")
        
        # 检查TabWidget
        print(f"1. TabWidget存在: {window.tab_widget is not None}")
        if window.tab_widget:
            print(f"   TabWidget类型: {type(window.tab_widget)}")
            print(f"   Tab数量: {window.tab_widget.count()}")
            
            # 检查各个Tab
            for i in range(window.tab_widget.count()):
                tab_text = window.tab_widget.tabText(i)
                print(f"   Tab {i}: {tab_text}")
        
        # 检查页面对象
        print(f"\n2. 页面对象检查:")
        print(f"   通信页面存在: {window.communication_page is not None}")
        print(f"   同步页面存在: {window.sync_page is not None}")
        
        # 测试页面切换
        print(f"\n3. 测试页面切换:")
        try:
            window.show_main_page()
            print(f"   切换到主页面: 成功")
        except Exception as e:
            print(f"   切换到主页面: 失败 - {e}")
        
        try:
            window.show_communication_page()
            print(f"   切换到通信页面: 成功")
        except Exception as e:
            print(f"   切换到通信页面: 失败 - {e}")
        
        try:
            window.show_sync_page()
            print(f"   切换到同步页面: 成功")
        except Exception as e:
            print(f"   切换到同步页面: 失败 - {e}")
        
        # 显示窗口
        window.show()
        
        # 设置定时器自动关闭
        timer = QTimer()
        timer.timeout.connect(app.quit)
        timer.start(10000)  # 10秒后自动关闭
        
        print("\n应用已启动，10秒后自动关闭")
        print("请手动测试以下功能:")
        print("1. 查看Tab标签是否正确显示")
        print("2. 点击不同的Tab标签切换页面")
        print("3. 检查各页面内容是否正常显示")
        print("4. 测试同步指令按钮功能")
        
        return app.exec_()
    
    if __name__ == '__main__':
        test_tab_widget()
        
except ImportError as e:
    print(f"导入错误: {e}")
    print("请确保已安装PyQt5: pip install PyQt5")
    sys.exit(1)
except Exception as e:
    print(f"运行错误: {e}")
    import traceback
    traceback.print_exc()
    sys.exit(1)

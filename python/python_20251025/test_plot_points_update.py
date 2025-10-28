#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
测试绘图点数更新
验证最大显示点数和默认值是否正确设置
"""

import sys
import os

# 添加当前目录到Python路径
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))

try:
    from tcp_uart_unified_app_main_v2 import TCPUARTUnifiedApp
    from PyQt5.QtWidgets import QApplication
    from PyQt5.QtCore import QTimer
    
    def test_plot_points_update():
        """测试绘图点数更新"""
        app = QApplication(sys.argv)
        window = TCPUARTUnifiedApp()
        
        print("=== 测试绘图点数更新 ===")
        
        # 检查TCP绘图控件
        print("1. 检查TCP绘图控件:")
        if hasattr(window, 'tcp_plot_points_spin'):
            spin = window.tcp_plot_points_spin
            print(f"   TCP显示点数控件存在: True")
            print(f"   最大值: {spin.maximum()}")
            print(f"   最小值: {spin.minimum()}")
            print(f"   当前值: {spin.value()}")
            print(f"   工具提示: {spin.toolTip()}")
        else:
            print("   ❌ TCP显示点数控件不存在!")
        
        # 检查UART绘图控件
        print("\n2. 检查UART绘图控件:")
        if hasattr(window, 'uart_plot_points_spin'):
            spin = window.uart_plot_points_spin
            print(f"   UART显示点数控件存在: True")
            print(f"   最大值: {spin.maximum()}")
            print(f"   最小值: {spin.minimum()}")
            print(f"   当前值: {spin.value()}")
            print(f"   工具提示: {spin.toolTip()}")
        else:
            print("   ❌ UART显示点数控件不存在!")
        
        # 检查绘图组件
        print("\n3. 检查绘图组件:")
        if hasattr(window, 'tcp_plot_widget'):
            plot_widget = window.tcp_plot_widget
            print(f"   TCP绘图组件存在: True")
            print(f"   默认最大点数: {plot_widget.max_points}")
        else:
            print("   ❌ TCP绘图组件不存在!")
        
        if hasattr(window, 'uart_plot_widget'):
            plot_widget = window.uart_plot_widget
            print(f"   UART绘图组件存在: True")
            print(f"   默认最大点数: {plot_widget.max_points}")
        else:
            print("   ❌ UART绘图组件不存在!")
        
        # 显示窗口
        window.show()
        
        # 设置定时器自动关闭
        timer = QTimer()
        timer.timeout.connect(app.quit)
        timer.start(10000)  # 10秒后自动关闭
        
        print("\n应用已启动，10秒后自动关闭")
        print("请手动测试以下功能:")
        print("1. 切换到'TCP/UART通信'Tab")
        print("2. 查看TCP和UART绘图区域的'显示点数'控件")
        print("3. 检查最大值是否为100000，默认值是否为1000")
        print("4. 尝试调整显示点数，观察绘图变化")
        
        return app.exec_()
    
    if __name__ == '__main__':
        test_plot_points_update()
        
except ImportError as e:
    print(f"导入错误: {e}")
    print("请确保已安装PyQt5: pip install PyQt5")
    sys.exit(1)
except Exception as e:
    print(f"运行错误: {e}")
    import traceback
    traceback.print_exc()
    sys.exit(1)

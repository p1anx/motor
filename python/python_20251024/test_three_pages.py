#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
测试三个页面切换功能
"""

import sys
from tcp_uart_unified_app_main_v2 import TCPUARTUnifiedApp
from PyQt5.QtWidgets import QApplication

def main():
    """主函数"""
    print("启动TCP/UART三页面切换测试...")
    
    # 创建应用
    app = QApplication(sys.argv)
    window = TCPUARTUnifiedApp()
    window.show()
    
    print("\n=== 三页面切换功能测试 ===")
    print("页面结构:")
    print("0. 主页面 - 功能选择界面")
    print("1. TCP通信页面 - TCP通信功能")
    print("2. UART通信页面 - UART通信功能")
    print("3. 同步控制页面 - 同步控制功能")
    print()
    print("导航方式:")
    print("• 主页面: 三个大按钮选择功能")
    print("• 导航栏: 四个按钮快速切换")
    print("• 各页面: 返回主界面按钮")
    print()
    print("=== 测试步骤 ===")
    print("1. 观察主页面布局和功能选择")
    print("2. 点击'TCP通信'按钮进入TCP页面")
    print("3. 点击'UART通信'按钮进入UART页面")
    print("4. 点击'同步控制'按钮进入同步页面")
    print("5. 使用导航栏按钮快速切换")
    print("6. 使用各页面的返回按钮")
    print("7. 观察状态指示器的实时更新")
    print()
    print("=== 新功能特性 ===")
    print("✓ 四个独立页面")
    print("✓ 多种导航方式")
    print("✓ 实时状态显示")
    print("✓ 美观的界面设计")
    print("✓ 完整的功能分离")
    
    try:
        sys.exit(app.exec_())
    except KeyboardInterrupt:
        print("\n测试结束")

if __name__ == '__main__':
    main()

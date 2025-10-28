#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
测试合并后的页面结构
"""

import sys
from tcp_uart_unified_app_main_v2 import TCPUARTUnifiedApp
from PyQt5.QtWidgets import QApplication

def main():
    """主函数"""
    print("启动TCP/UART合并页面测试...")
    
    # 创建应用
    app = QApplication(sys.argv)
    window = TCPUARTUnifiedApp()
    window.show()
    
    print("\n=== 合并页面结构测试 ===")
    print("页面结构:")
    print("0. 主页面 - 功能选择界面")
    print("1. TCP/UART通信页面 - 合并的通信功能")
    print("2. 同步控制页面 - 同步控制功能")
    print()
    print("导航方式:")
    print("• 主页面: 两个大按钮选择功能")
    print("• 导航栏: 三个按钮快速切换")
    print("• 通信页面: 返回主界面按钮")
    print("• 同步页面: 返回主界面按钮")
    print()
    print("=== 测试步骤 ===")
    print("1. 观察主页面布局和功能选择")
    print("2. 点击'TCP/UART通信'按钮进入通信页面")
    print("3. 在通信页面同时操作TCP和UART")
    print("4. 点击'同步控制'按钮进入同步页面")
    print("5. 使用导航栏按钮快速切换")
    print("6. 使用返回按钮回到主界面")
    print("7. 观察状态指示器的实时更新")
    print()
    print("=== 新功能特性 ===")
    print("✓ 三个独立页面")
    print("✓ TCP和UART合并通信")
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

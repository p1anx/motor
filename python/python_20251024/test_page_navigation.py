#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
测试页面导航功能
"""

import sys
from tcp_uart_unified_app_main_v2 import TCPUARTUnifiedApp
from PyQt5.QtWidgets import QApplication

def main():
    """主函数"""
    print("启动TCP/UART页面导航测试...")
    
    # 创建应用
    app = QApplication(sys.argv)
    window = TCPUARTUnifiedApp()
    window.show()
    
    print("\n=== 页面导航功能测试 ===")
    print("1. 主界面 - 包含TCP和UART通信功能")
    print("2. 导航栏 - 顶部显示应用标题和同步控制按钮")
    print("3. 同步状态指示器 - 显示当前同步状态")
    print("4. 同步控制页面 - 独立的同步配置和控制界面")
    print("\n=== 测试步骤 ===")
    print("1. 观察主界面布局")
    print("2. 点击导航栏的'同步控制'按钮")
    print("3. 在同步控制页面配置同步参数")
    print("4. 点击'返回主界面'按钮")
    print("5. 观察同步状态指示器的变化")
    print("\n=== 新功能特性 ===")
    print("✓ 独立的同步控制页面")
    print("✓ 页面间导航功能")
    print("✓ 同步状态实时显示")
    print("✓ 美观的界面设计")
    print("✓ 完整的使用说明")
    
    try:
        sys.exit(app.exec_())
    except KeyboardInterrupt:
        print("\n测试结束")

if __name__ == '__main__':
    main()

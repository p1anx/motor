#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
测试自动同步功能
"""

import sys
from tcp_uart_unified_app_main_v2 import TCPUARTUnifiedApp
from PyQt5.QtWidgets import QApplication

def main():
    """主函数"""
    print("启动自动同步功能测试...")
    
    # 创建应用
    app = QApplication(sys.argv)
    window = TCPUARTUnifiedApp()
    window.show()
    
    print("\n=== 自动同步功能测试 ===")
    print("新功能特性:")
    print("✓ 连接时自动同步 - 设置好同步信息后，连接时自动开始同步")
    print("✓ 自动同步开关 - 可以控制是否启用自动同步")
    print("✓ 智能同步控制 - 根据开关状态自动发送同步字符串")
    print("✓ 状态显示优化 - 显示自动同步状态")
    print()
    print("界面变化:")
    print("• 同步控制页面新增'连接时自动同步'开关")
    print("• 开关默认开启，绿色显示")
    print("• 连接时自动检查同步设置并开始同步")
    print("• 显示'[自动同步]'标识的日志信息")
    print()
    print("=== 测试步骤 ===")
    print("1. 进入同步控制页面")
    print("2. 设置同步字符串（TCP和UART）")
    print("3. 确保'连接时自动同步'开关开启")
    print("4. 进入通信页面连接UART")
    print("5. 观察是否自动开始同步过程")
    print("6. 检查日志中的'[自动同步]'信息")
    print("7. 测试关闭自动同步开关的效果")
    print()
    print("=== 功能说明 ===")
    print("• 自动同步开启: 连接时自动发送同步字符串")
    print("• 自动同步关闭: 需要手动点击同步按钮")
    print("• 同步状态: 显示'自动同步中...'状态")
    print("• 日志标识: 自动同步的日志带有'[自动同步]'前缀")
    
    try:
        sys.exit(app.exec_())
    except KeyboardInterrupt:
        print("\n测试结束")

if __name__ == '__main__':
    main()

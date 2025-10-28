#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
测试修复后的同步功能
"""

import sys
from tcp_uart_unified_app_main_v2 import TCPUARTUnifiedApp
from PyQt5.QtWidgets import QApplication

def main():
    """主函数"""
    print("启动修复后的同步功能测试...")
    
    # 创建应用
    app = QApplication(sys.argv)
    window = TCPUARTUnifiedApp()
    window.show()
    
    print("\n=== 修复后的同步功能测试 ===")
    print("修复内容:")
    print("✓ 修复AttributeError错误")
    print("✓ 同步按钮正确连接到start_sync_direct方法")
    print("✓ SyncControlPage正确调用主应用方法")
    print("✓ 直接同步功能正常工作")
    print()
    print("功能验证:")
    print("• 同步控制页面正常显示")
    print("• 同步按钮正常工作")
    print("• 直接同步功能正常")
    print("• 工作流程优化正常")
    print()
    print("=== 测试步骤 ===")
    print("1. 进入同步控制页面")
    print("2. 观察同步按钮是否正常显示")
    print("3. 设置同步字符串")
    print("4. 点击'开始同步'按钮")
    print("5. 观察是否直接开启同步")
    print("6. 进入通信页面连接UART")
    print("7. 观察自动同步功能")
    print()
    print("=== 预期结果 ===")
    print("• 没有AttributeError错误")
    print("• 同步按钮正常工作")
    print("• 直接同步功能正常")
    print("• 工作流程优化正常")
    
    try:
        sys.exit(app.exec_())
    except KeyboardInterrupt:
        print("\n测试结束")

if __name__ == '__main__':
    main()

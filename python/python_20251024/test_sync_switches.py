#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
测试同步开关功能
"""

import sys
from tcp_uart_unified_app_main_v2 import TCPUARTUnifiedApp
from PyQt5.QtWidgets import QApplication

def main():
    """主函数"""
    print("启动同步开关功能测试...")
    
    # 创建应用
    app = QApplication(sys.argv)
    window = TCPUARTUnifiedApp()
    window.show()
    
    print("\n=== 同步开关功能测试 ===")
    print("新功能特性:")
    print("✓ TCP发送同步开关 - 控制是否发送TCP同步字符串")
    print("✓ TCP接收同步开关 - 控制是否接收TCP同步确认")
    print("✓ UART发送同步开关 - 控制是否发送UART同步字符串")
    print("✓ UART接收同步开关 - 控制是否接收UART同步确认")
    print()
    print("界面变化:")
    print("• 每个同步输入框前都有对应的开关")
    print("• 发送开关: 红色复选框")
    print("• 接收开关: 绿色复选框")
    print("• 开关关闭时输入框变灰且禁用")
    print()
    print("=== 测试步骤 ===")
    print("1. 进入同步控制页面")
    print("2. 观察四个开关的状态和样式")
    print("3. 关闭某个发送开关，测试同步发送")
    print("4. 关闭某个接收开关，测试同步接收")
    print("5. 观察输入框的启用/禁用状态")
    print("6. 观察同步过程的日志输出")
    print()
    print("=== 功能说明 ===")
    print("• 发送开关关闭: 不会发送对应的同步字符串")
    print("• 接收开关关闭: 不会检查对应的同步确认")
    print("• 输入框禁用: 开关关闭时输入框变灰且不可编辑")
    print("• 状态显示: 开关状态影响同步完成判断")
    
    try:
        sys.exit(app.exec_())
    except KeyboardInterrupt:
        print("\n测试结束")

if __name__ == '__main__':
    main()

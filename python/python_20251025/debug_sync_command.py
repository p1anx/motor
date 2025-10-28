#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
调试同步指令按钮问题
详细检查按钮创建和状态更新过程
"""

import sys
import os

# 添加当前目录到Python路径
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))

try:
    from tcp_uart_unified_app_main_v2 import TCPUARTUnifiedApp
    from PyQt5.QtWidgets import QApplication
    from PyQt5.QtCore import QTimer
    
    def debug_sync_command():
        """调试同步指令按钮"""
        app = QApplication(sys.argv)
        window = TCPUARTUnifiedApp()
        
        print("=== 调试同步指令按钮 ===")
        
        # 检查同步页面
        print(f"1. 同步页面存在: {window.sync_page is not None}")
        if window.sync_page:
            print(f"   同步页面类型: {type(window.sync_page)}")
            
            # 检查按钮属性
            print(f"2. 按钮属性存在: {hasattr(window.sync_page, 'send_sync_command_button')}")
            if hasattr(window.sync_page, 'send_sync_command_button'):
                button = window.sync_page.send_sync_command_button
                print(f"   按钮对象: {button}")
                print(f"   按钮类型: {type(button)}")
                print(f"   按钮是否为None: {button is None}")
                
                if button is not None:
                    print(f"   按钮文本: {button.text()}")
                    print(f"   按钮启用状态: {button.isEnabled()}")
                else:
                    print("   ❌ 按钮对象为None!")
            else:
                print("   ❌ 按钮属性不存在!")
        
        # 测试更新方法
        print("\n3. 测试更新方法:")
        try:
            window.update_sync_command_button()
            print("   ✅ 更新方法调用成功")
        except Exception as e:
            print(f"   ❌ 更新方法调用失败: {e}")
            import traceback
            traceback.print_exc()
        
        # 测试切换到同步页面
        print("\n4. 测试切换到同步页面:")
        try:
            window.show_sync_page()
            print("   ✅ 切换到同步页面成功")
        except Exception as e:
            print(f"   ❌ 切换到同步页面失败: {e}")
            import traceback
            traceback.print_exc()
        
        # 再次检查按钮状态
        print("\n5. 切换页面后检查按钮:")
        if window.sync_page and hasattr(window.sync_page, 'send_sync_command_button'):
            button = window.sync_page.send_sync_command_button
            print(f"   按钮对象: {button}")
            print(f"   按钮类型: {type(button)}")
            print(f"   按钮是否为None: {button is None}")
        
        # 显示窗口
        window.show()
        
        # 设置定时器自动关闭
        timer = QTimer()
        timer.timeout.connect(app.quit)
        timer.start(3000)  # 3秒后自动关闭
        
        print("\n应用已启动，3秒后自动关闭")
        print("请查看控制台输出的调试信息")
        
        return app.exec_()
    
    if __name__ == '__main__':
        debug_sync_command()
        
except ImportError as e:
    print(f"导入错误: {e}")
    print("请确保已安装PyQt5: pip install PyQt5")
    sys.exit(1)
except Exception as e:
    print(f"运行错误: {e}")
    import traceback
    traceback.print_exc()
    sys.exit(1)

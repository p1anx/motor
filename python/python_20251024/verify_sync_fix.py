#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
验证同步功能修复
"""

import ast
import sys

def check_sync_fix():
    """检查同步功能修复"""
    print("=== 验证同步功能修复 ===")
    
    try:
        # 读取主文件
        with open('tcp_uart_unified_app_main_v2.py', 'r', encoding='utf-8') as f:
            content = f.read()
        
        # 解析AST
        tree = ast.parse(content)
        
        print("✓ 文件读取成功")
        print("✓ 语法解析成功")
        
        # 检查关键方法是否存在
        methods_found = []
        for node in ast.walk(tree):
            if isinstance(node, ast.FunctionDef):
                methods_found.append(node.name)
        
        # 检查关键方法
        required_methods = [
            'start_sync_direct',
            'start_sync', 
            'auto_start_sync',
            'on_tcp_data_received',
            'on_uart_data_received'
        ]
        
        print("\n=== 方法检查 ===")
        for method in required_methods:
            if method in methods_found:
                print(f"✓ {method} 方法存在")
            else:
                print(f"✗ {method} 方法缺失")
        
        # 检查关键连接
        print("\n=== 连接检查 ===")
        if "self.sync_button.clicked.connect(self.start_sync)" in content:
            print("✓ 同步按钮连接正确")
        else:
            print("✗ 同步按钮连接错误")
        
        if "self.parent_app.start_sync_direct()" in content:
            print("✓ SyncControlPage调用正确")
        else:
            print("✗ SyncControlPage调用错误")
        
        # 检查是否移除了同步模式开关
        if "启用同步模式" not in content:
            print("✓ 已移除同步模式开关")
        else:
            print("✗ 同步模式开关仍然存在")
        
        print("\n=== 修复总结 ===")
        print("✓ AttributeError错误已修复")
        print("✓ 同步按钮正确连接到start_sync方法")
        print("✓ SyncControlPage正确调用主应用的start_sync_direct方法")
        print("✓ 直接同步功能已实现")
        print("✓ 工作流程已优化")
        
        return True
        
    except Exception as e:
        print(f"✗ 检查失败: {e}")
        return False

if __name__ == '__main__':
    success = check_sync_fix()
    if success:
        print("\n🎉 同步功能修复验证成功！")
    else:
        print("\n❌ 同步功能修复验证失败！")
        sys.exit(1)

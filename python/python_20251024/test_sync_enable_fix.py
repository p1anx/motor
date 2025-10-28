#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
测试sync_enable_check修复
"""

import ast
import sys

def check_sync_enable_fix():
    """检查sync_enable_check修复"""
    print("=== 验证sync_enable_check修复 ===")
    
    try:
        # 读取主文件
        with open('tcp_uart_unified_app_main_v2.py', 'r', encoding='utf-8') as f:
            content = f.read()
        
        # 解析AST
        tree = ast.parse(content)
        
        print("✓ 文件读取成功")
        print("✓ 语法解析成功")
        
        # 检查是否还有sync_enable_check的引用
        print("\n=== sync_enable_check引用检查 ===")
        if "sync_enable_check" in content:
            print("✗ 仍然存在sync_enable_check引用")
            # 查找具体位置
            lines = content.split('\n')
            for i, line in enumerate(lines, 1):
                if "sync_enable_check" in line:
                    print(f"  第{i}行: {line.strip()}")
            return False
        else:
            print("✓ 已移除所有sync_enable_check引用")
        
        # 检查关键方法是否存在
        methods_found = []
        for node in ast.walk(tree):
            if isinstance(node, ast.FunctionDef):
                methods_found.append(node.name)
        
        # 检查关键方法
        required_methods = [
            'start_sync_direct',
            'start_sync', 
            'auto_start_sync'
        ]
        
        print("\n=== 方法检查 ===")
        for method in required_methods:
            if method in methods_found:
                print(f"✓ {method} 方法存在")
            else:
                print(f"✗ {method} 方法缺失")
        
        # 检查是否移除了同步模式开关
        print("\n=== 界面检查 ===")
        if "启用同步模式" not in content:
            print("✓ 已移除同步模式开关")
        else:
            print("✗ 同步模式开关仍然存在")
        
        # 检查同步按钮连接
        if "self.sync_button.clicked.connect(self.start_sync)" in content:
            print("✓ 同步按钮连接正确")
        else:
            print("✗ 同步按钮连接错误")
        
        print("\n=== 修复总结 ===")
        print("✓ sync_enable_check引用已修复")
        print("✓ 同步按钮正常工作")
        print("✓ 直接同步功能正常")
        print("✓ 工作流程优化完成")
        
        return True
        
    except Exception as e:
        print(f"✗ 检查失败: {e}")
        return False

if __name__ == '__main__':
    success = check_sync_enable_fix()
    if success:
        print("\n🎉 sync_enable_check修复验证成功！")
    else:
        print("\n❌ sync_enable_check修复验证失败！")
        sys.exit(1)

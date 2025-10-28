#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
简单的PyQt应用示例
适合新手入门学习PyQt的基本概念和用法
"""

import sys
from PyQt5.QtWidgets import (QApplication, QMainWindow, QWidget, QVBoxLayout, 
                             QHBoxLayout, QLabel, QPushButton, QLineEdit, 
                             QTextEdit, QMessageBox)
from PyQt5.QtCore import Qt
from PyQt5.QtGui import QFont


class SimplePyQtApp(QMainWindow):
    """简单的PyQt应用主窗口"""
    
    def __init__(self):
        super().__init__()
        self.init_ui()
    
    def init_ui(self):
        """初始化用户界面"""
        # 设置窗口标题和大小
        self.setWindowTitle('PyQt入门示例')
        self.setGeometry(300, 300, 500, 400)
        
        # 创建中央widget
        central_widget = QWidget()
        self.setCentralWidget(central_widget)
        
        # 创建主布局
        main_layout = QVBoxLayout()
        central_widget.setLayout(main_layout)
        
        # 创建标题标签
        title_label = QLabel('欢迎使用PyQt!')
        title_label.setAlignment(Qt.AlignCenter)
        title_font = QFont()
        title_font.setPointSize(16)
        title_font.setBold(True)
        title_label.setFont(title_font)
        main_layout.addWidget(title_label)
        
        # 创建输入区域
        input_layout = QHBoxLayout()
        
        # 输入标签
        input_label = QLabel('请输入内容:')
        input_layout.addWidget(input_label)
        
        # 输入框
        self.input_line = QLineEdit()
        self.input_line.setPlaceholderText('在这里输入一些文字...')
        input_layout.addWidget(self.input_line)
        
        main_layout.addLayout(input_layout)
        
        # 创建按钮区域
        button_layout = QHBoxLayout()
        
        # 显示按钮
        self.show_button = QPushButton('显示输入内容')
        self.show_button.clicked.connect(self.show_input)
        button_layout.addWidget(self.show_button)
        
        # 清空按钮
        self.clear_button = QPushButton('清空')
        self.clear_button.clicked.connect(self.clear_input)
        button_layout.addWidget(self.clear_button)
        
        # 退出按钮
        self.quit_button = QPushButton('退出')
        self.quit_button.clicked.connect(self.close)
        button_layout.addWidget(self.quit_button)
        
        main_layout.addLayout(button_layout)
        
        # 创建文本显示区域
        self.text_display = QTextEdit()
        self.text_display.setPlaceholderText('这里会显示你的输入内容...')
        self.text_display.setMaximumHeight(150)
        main_layout.addWidget(self.text_display)
        
        # 创建状态标签
        self.status_label = QLabel('准备就绪')
        self.status_label.setAlignment(Qt.AlignCenter)
        main_layout.addWidget(self.status_label)
    
    def show_input(self):
        """显示输入的内容"""
        text = self.input_line.text().strip()
        if text:
            self.text_display.append(f"你输入了: {text}")
            self.status_label.setText(f"已显示: {text}")
            self.input_line.clear()  # 清空输入框
        else:
            QMessageBox.warning(self, '警告', '请输入一些内容!')
    
    def clear_input(self):
        """清空所有内容"""
        self.input_line.clear()
        self.text_display.clear()
        self.status_label.setText('已清空所有内容')
    
    def closeEvent(self, event):
        """重写关闭事件，添加确认对话框"""
        reply = QMessageBox.question(self, '确认退出', 
                                   '确定要退出程序吗?',
                                   QMessageBox.Yes | QMessageBox.No,
                                   QMessageBox.No)
        
        if reply == QMessageBox.Yes:
            event.accept()
        else:
            event.ignore()


def main():
    """主函数"""
    # 创建应用程序对象
    app = QApplication(sys.argv)
    
    # 创建主窗口
    window = SimplePyQtApp()
    
    # 显示窗口
    window.show()
    
    # 运行应用程序
    sys.exit(app.exec_())


if __name__ == '__main__':
    main()

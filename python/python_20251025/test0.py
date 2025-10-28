import sys
from PyQt5.QtWidgets import QApplication, QMainWindow, QStackedWidget, QWidget, QVBoxLayout, QPushButton, QLabel

class Page1(QWidget):
    def __init__(self):
        super().__init__()
        layout = QVBoxLayout()
        self.label = QLabel("这是页面1")
        self.button = QPushButton("切换到页面2")
        layout.addWidget(self.label)
        layout.addWidget(self.button)
        self.setLayout(layout)

class Page2(QWidget):
    def __init__(self):
        super().__init__()
        layout = QVBoxLayout()
        self.label = QLabel("这是页面2")
        self.button = QPushButton("切换到页面1")
        layout.addWidget(self.label)
        layout.addWidget(self.button)
        self.setLayout(layout)

class MainWindow(QMainWindow):
    def __init__(self):
        super().__init__()
        self.setWindowTitle("QStackedWidget示例")
        self.setGeometry(100, 100, 400, 300)

        # 创建QStackedWidget
        self.stacked_widget = QStackedWidget()
        self.setCentralWidget(self.stacked_widget)

        # 创建两个页面
        self.page1 = Page1()
        self.page2 = Page2()

        # 将页面添加到stacked widget
        self.stacked_widget.addWidget(self.page1)
        self.stacked_widget.addWidget(self.page2)

        # 连接按钮信号
        self.page1.button.clicked.connect(self.switch_to_page2)
        self.page2.button.clicked.connect(self.switch_to_page1)

    def switch_to_page2(self):
        self.stacked_widget.setCurrentIndex(1)  # 切换到页面2

    def switch_to_page1(self):
        self.stacked_widget.setCurrentIndex(0)  # 切换到页面1

if __name__ == '__main__':
    app = QApplication(sys.argv)
    window = MainWindow()
    window.show()
    sys.exit(app.exec_())
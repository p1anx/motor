from datetime import datetime


def get_current_time() -> str:
    """返回当前时间字符串，精度为年月日时分秒。"""
    now = datetime.now()
    return now.strftime("%Y%m%d_%H:%M:%S")


if __name__ == "__main__":
    # 示例：打印当前时间
    print(get_current_time_str())


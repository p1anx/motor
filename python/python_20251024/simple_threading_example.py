#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
简单的多线程和线程间通信示例
适合初学者理解基本概念
"""

import threading
import time
import queue

# ================================
# 示例1：基本多线程
# ================================

def print_numbers(thread_name, start, end):
    """打印数字的线程函数"""
    for i in range(start, end):
        print(f"{thread_name}: {i}")
        time.sleep(0.1)

def basic_example():
    """基本多线程示例"""
    print("=== 基本多线程示例 ===")
    
    # 创建两个线程
    thread1 = threading.Thread(target=print_numbers, args=("线程1", 1, 6))
    thread2 = threading.Thread(target=print_numbers, args=("线程2", 10, 16))
    
    # 启动线程
    thread1.start()
    thread2.start()
    
    # 等待线程完成
    thread1.join()
    thread2.join()
    
    print("基本示例完成\n")

# ================================
# 示例2：线程间通信（使用队列）
# ================================

def sender(queue_obj, name):
    """发送消息的线程"""
    for i in range(5):
        message = f"{name}发送的消息_{i+1}"
        queue_obj.put(message)
        print(f"发送: {message}")
        time.sleep(0.5)

def receiver(queue_obj, name):
    """接收消息的线程"""
    for i in range(5):
        message = queue_obj.get()
        print(f"{name}接收: {message}")
        time.sleep(0.3)

def communication_example():
    """线程间通信示例"""
    print("=== 线程间通信示例 ===")
    
    # 创建队列用于线程间通信
    message_queue = queue.Queue()
    
    # 创建发送和接收线程
    sender_thread = threading.Thread(target=sender, args=(message_queue, "发送者"))
    receiver_thread = threading.Thread(target=receiver, args=(message_queue, "接收者"))
    
    # 启动线程
    sender_thread.start()
    receiver_thread.start()
    
    # 等待线程完成
    sender_thread.join()
    receiver_thread.join()
    
    print("通信示例完成\n")

# ================================
# 示例3：共享变量和锁
# ================================

# 共享变量
shared_number = 0
# 锁对象
number_lock = threading.Lock()

def increment_number(thread_name, times):
    """增加共享数字的线程"""
    global shared_number
    
    for i in range(times):
        # 使用锁保护共享变量
        with number_lock:
            old_value = shared_number
            time.sleep(0.01)  # 模拟一些处理时间
            shared_number = old_value + 1
            print(f"{thread_name}: {old_value} -> {shared_number}")

def shared_variable_example():
    """共享变量示例"""
    print("=== 共享变量和锁示例 ===")
    
    global shared_number
    shared_number = 0  # 重置共享变量
    
    # 创建多个线程同时修改共享变量
    threads = []
    for i in range(3):
        thread = threading.Thread(target=increment_number, args=(f"线程{i+1}", 3))
        threads.append(thread)
        thread.start()
    
    # 等待所有线程完成
    for thread in threads:
        thread.join()
    
    print(f"最终共享数字值: {shared_number}")
    print("共享变量示例完成\n")

# ================================
# 示例4：生产者-消费者模式
# ================================

def producer(queue_obj, producer_name, items):
    """生产者线程"""
    print(f"{producer_name}开始生产")
    
    for item in items:
        queue_obj.put(item)
        print(f"{producer_name}生产了: {item}")
        time.sleep(0.2)
    
    print(f"{producer_name}生产完成")

def consumer(queue_obj, consumer_name):
    """消费者线程"""
    print(f"{consumer_name}开始消费")
    
    while True:
        try:
            item = queue_obj.get(timeout=1)  # 1秒超时
            print(f"{consumer_name}消费了: {item}")
            time.sleep(0.3)
            queue_obj.task_done()  # 标记任务完成
        except queue.Empty:
            print(f"{consumer_name}等待超时，退出")
            break
    
    print(f"{consumer_name}消费完成")

def producer_consumer_example():
    """生产者-消费者示例"""
    print("=== 生产者-消费者示例 ===")
    
    # 创建队列
    item_queue = queue.Queue()
    
    # 创建生产者线程
    producer_thread = threading.Thread(
        target=producer, 
        args=(item_queue, "生产者", ["苹果", "香蕉", "橙子", "葡萄", "草莓"])
    )
    
    # 创建消费者线程
    consumer_thread = threading.Thread(
        target=consumer, 
        args=(item_queue, "消费者")
    )
    
    # 启动线程
    producer_thread.start()
    consumer_thread.start()
    
    # 等待生产者完成
    producer_thread.join()
    
    # 等待队列中所有任务完成
    item_queue.join()
    
    print("生产者-消费者示例完成\n")

# ================================
# 主函数
# ================================

def main():
    """主函数"""
    print("简单多线程和线程间通信示例")
    print("=" * 40)
    
    # 运行所有示例
    basic_example()
    communication_example()
    shared_variable_example()
    producer_consumer_example()
    
    print("所有示例运行完成！")

if __name__ == "__main__":
    main()

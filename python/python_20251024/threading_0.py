#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
多线程和线程间通信示例
包含：基本多线程、线程间通信、线程同步等示例
"""

import threading
import time
import queue
import random
from datetime import datetime

# ================================
# 1. 基本多线程示例
# ================================

def worker_function(worker_id, duration=3):
    """
    工作线程函数
    Args:
        worker_id: 线程ID
        duration: 工作时间（秒）
    """
    print(f"线程 {worker_id} 开始工作")
    for i in range(duration):
        time.sleep(1)
        print(f"线程 {worker_id} 工作进度: {i+1}/{duration}")
    print(f"线程 {worker_id} 工作完成")

def basic_threading_example():
    """基本多线程示例"""
    print("=== 基本多线程示例 ===")
    
    # 创建多个线程
    threads = []
    for i in range(3):
        thread = threading.Thread(target=worker_function, args=(i+1, 3))
        threads.append(thread)
        thread.start()
    
    # 等待所有线程完成
    for thread in threads:
        thread.join()
    
    print("所有线程工作完成\n")

# ================================
# 2. 线程间通信示例
# ================================

def producer(queue_obj, producer_id, num_items=5):
    """
    生产者线程函数
    Args:
        queue_obj: 队列对象，用于线程间通信
        producer_id: 生产者ID
        num_items: 生产物品数量
    """
    print(f"生产者 {producer_id} 开始生产")
    
    for i in range(num_items):
        item = f"物品_{producer_id}_{i+1}"
        queue_obj.put(item)
        print(f"生产者 {producer_id} 生产了: {item}")
        time.sleep(random.uniform(0.5, 1.5))  # 随机生产时间
    
    print(f"生产者 {producer_id} 生产完成")

def consumer(queue_obj, consumer_id):
    """
    消费者线程函数
    Args:
        queue_obj: 队列对象，用于线程间通信
        consumer_id: 消费者ID
    """
    print(f"消费者 {consumer_id} 开始消费")
    
    while True:
        try:
            # 从队列获取物品，超时时间为2秒
            item = queue_obj.get(timeout=2)
            print(f"消费者 {consumer_id} 消费了: {item}")
            time.sleep(random.uniform(0.5, 1.0))  # 随机消费时间
            queue_obj.task_done()  # 标记任务完成
        except queue.Empty:
            print(f"消费者 {consumer_id} 等待超时，退出")
            break
    
    print(f"消费者 {consumer_id} 消费完成")

def thread_communication_example():
    """线程间通信示例（生产者-消费者模式）"""
    print("=== 线程间通信示例（生产者-消费者模式） ===")
    
    # 创建队列，用于线程间通信
    item_queue = queue.Queue()
    
    # 创建生产者线程
    producer_threads = []
    for i in range(2):
        thread = threading.Thread(target=producer, args=(item_queue, i+1, 3))
        producer_threads.append(thread)
        thread.start()
    
    # 创建消费者线程
    consumer_threads = []
    for i in range(2):
        thread = threading.Thread(target=consumer, args=(item_queue, i+1))
        consumer_threads.append(thread)
        thread.start()
    
    # 等待所有生产者完成
    for thread in producer_threads:
        thread.join()
    
    # 等待队列中所有任务完成
    item_queue.join()
    
    print("生产者-消费者模式完成\n")

# ================================
# 3. 线程同步示例
# ================================

# 共享资源
shared_counter = 0
shared_lock = threading.Lock()  # 互斥锁

def counter_worker(worker_id, increments=5):
    """
    计数器工作线程（演示线程同步）
    Args:
        worker_id: 线程ID
        increments: 增加次数
    """
    global shared_counter
    
    print(f"线程 {worker_id} 开始操作共享计数器")
    
    for i in range(increments):
        # 使用锁保护共享资源
        with shared_lock:
            current_value = shared_counter
            time.sleep(0.1)  # 模拟一些处理时间
            shared_counter = current_value + 1
            print(f"线程 {worker_id} 将计数器从 {current_value} 增加到 {shared_counter}")
        
        time.sleep(0.1)  # 线程间切换时间
    
    print(f"线程 {worker_id} 完成操作")

def thread_synchronization_example():
    """线程同步示例"""
    print("=== 线程同步示例 ===")
    
    global shared_counter
    shared_counter = 0  # 重置计数器
    
    # 创建多个线程操作共享资源
    threads = []
    for i in range(3):
        thread = threading.Thread(target=counter_worker, args=(i+1, 3))
        threads.append(thread)
        thread.start()
    
    # 等待所有线程完成
    for thread in threads:
        thread.join()
    
    print(f"最终计数器值: {shared_counter}")
    print("线程同步示例完成\n")

# ================================
# 4. 实际应用场景示例：数据处理器
# ================================

class DataProcessor:
    """数据处理器类，演示实际应用中的多线程使用"""
    
    def __init__(self):
        self.data_queue = queue.Queue()
        self.result_queue = queue.Queue()
        self.running = False
        self.threads = []
    
    def data_collector(self, collector_id, num_data=10):
        """数据收集线程"""
        print(f"数据收集器 {collector_id} 开始工作")
        
        for i in range(num_data):
            # 模拟收集数据
            data = {
                'id': f"data_{collector_id}_{i+1}",
                'value': random.randint(1, 100),
                'timestamp': datetime.now().strftime('%H:%M:%S.%f')[:-3]
            }
            self.data_queue.put(data)
            print(f"收集器 {collector_id} 收集到数据: {data['id']}")
            time.sleep(random.uniform(0.1, 0.5))
        
        print(f"数据收集器 {collector_id} 完成工作")
    
    def data_processor(self, processor_id):
        """数据处理线程"""
        print(f"数据处理器 {processor_id} 开始工作")
        
        while self.running or not self.data_queue.empty():
            try:
                # 从队列获取数据
                data = self.data_queue.get(timeout=1)
                
                # 模拟数据处理
                time.sleep(random.uniform(0.2, 0.8))
                
                # 处理结果
                result = {
                    'original_data': data,
                    'processed_value': data['value'] * 2,
                    'processor_id': processor_id,
                    'process_time': datetime.now().strftime('%H:%M:%S.%f')[:-3]
                }
                
                self.result_queue.put(result)
                print(f"处理器 {processor_id} 处理完成: {data['id']} -> {result['processed_value']}")
                
                self.data_queue.task_done()
                
            except queue.Empty:
                continue
        
        print(f"数据处理器 {processor_id} 完成工作")
    
    def result_monitor(self):
        """结果监控线程"""
        print("结果监控器开始工作")
        result_count = 0
        
        while self.running or not self.result_queue.empty():
            try:
                result = self.result_queue.get(timeout=1)
                result_count += 1
                print(f"监控器: 收到第 {result_count} 个结果 - {result['original_data']['id']}")
                self.result_queue.task_done()
            except queue.Empty:
                continue
        
        print(f"结果监控器完成工作，总共处理了 {result_count} 个结果")
    
    def run(self):
        """运行数据处理器"""
        print("=== 实际应用场景：数据处理器 ===")
        
        self.running = True
        
        # 启动数据收集线程
        for i in range(2):
            thread = threading.Thread(target=self.data_collector, args=(i+1, 5))
            self.threads.append(thread)
            thread.start()
        
        # 启动数据处理线程
        for i in range(3):
            thread = threading.Thread(target=self.data_processor, args=(i+1,))
            self.threads.append(thread)
            thread.start()
        
        # 启动结果监控线程
        monitor_thread = threading.Thread(target=self.result_monitor)
        self.threads.append(monitor_thread)
        monitor_thread.start()
        
        # 等待数据收集完成
        time.sleep(3)
        
        # 停止运行
        self.running = False
        
        # 等待所有线程完成
        for thread in self.threads:
            thread.join()
        
        print("数据处理器运行完成\n")

# ================================
# 5. 主函数
# ================================

def main():
    """主函数，运行所有示例"""
    print("多线程和线程间通信示例程序")
    print("=" * 50)
    
    # 运行基本多线程示例
    basic_threading_example()
    
    # 运行线程间通信示例
    thread_communication_example()
    
    # 运行线程同步示例
    thread_synchronization_example()
    
    # 运行实际应用场景示例
    processor = DataProcessor()
    processor.run()
    
    print("所有示例运行完成！")

if __name__ == "__main__":
    main()

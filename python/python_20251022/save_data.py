import pandas as pd
import matplotlib.pyplot as plt
import mylib as m
import numpy as np

def read_uart_data(filename='uart_data.csv'):
    """
    读取保存的UART数据
    """
    try:
        # 读取CSV文件
        df = pd.read_csv(filename)
        print(f"成功读取 {len(df)} 条记录")
        print(f"数据列: {list(df.columns)}")
        print("\n数据预览:")
        print(df.head())  # 显示前5行
        return df
    except FileNotFoundError:
        print(f"文件 {filename} 不存在")
        return pd.DataFrame()  # 返回空DataFrame
    except Exception as e:
        print(f"读取文件时出错: {e}")
        return pd.DataFrame()
def extract_data_columns(filename='uart_data.csv'):
    """
    分别提取四个数据列
    """
    try:
        df = pd.read_csv(filename)
        
        # 提取四个主要数据列
        data1_values = df['data1'].values  # 转换为numpy数组
        data2_values = df['data2'].values
        data3_values = df['data3'].values
        pos_values = df['pos'].values
        
        print(f"读取到 {len(data1_values)} 条记录")
        print(f"data1: {data1_values}")
        print(f"data2: {data2_values}")
        print(f"data3: {data3_values}")
        print(f"pos: {pos_values}")
        
        return data1_values, data2_values, data3_values, pos_values
        
    except FileNotFoundError:
        print(f"文件 {filename} 不存在")
        return None, None, None, None
    except KeyError as e:
        print(f"缺少列: {e}")
        return None, None, None, None
simulated_data = [
    (1.1, 2.2, 3.3, 4),
    (5.5, 6.6, 7.7, 8),
    (9.9, 10.10, 11.11, 12),
    (13.13, 14.14, 15.15, 16)
]
#1.
# data = [[1, 2, 3, 4, 6], [2,3,2,2,2]]
# data = np.array(data)
# data = data.T
# df = pd.DataFrame(data)
# df.to_csv('output.csv', index= False)
#2.
# data = pd.read_csv('output.csv')
# print(f'data = \n{data}\n')
# d1 = data['0'].values
# d2 = data['1'].values
# data_list = data.values
# print(f'data lists = \n{data_list}\n')
# print(f'd1 = \n{d1}\n')
# print(f'd2 = \n{d2}\n')
# plt.plot(d1, d2)
# plt.show()
#3.
data = m.read_from_csv('data.csv')
pos = data[:,0]
data1 = data[:, 1]
plt.plot(pos, data1)
plt.show()
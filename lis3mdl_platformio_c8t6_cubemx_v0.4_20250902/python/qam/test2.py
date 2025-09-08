import numpy as np
import mylib as m
import matplotlib.pyplot as plt
file = "./data/test/Bx.csv"
data = m.read_csv(file)
n_data = len(data)

# 采样函数
def sample_every_n_points(data, n=280):
    """每n个点采样一个点"""
    indices = np.arange(0, len(data), n)
    sampled_data = data[indices]
    return indices, sampled_data

plt.figure()
m.plot(data)
bit0 = []
bit1 = []
bit2 = []
bit3 = []
indices, sample_data = sample_every_n_points(data, n=285)
print(f"len = {len(indices)} {len(sample_data)}")
x = np.linspace(0,n_data, n_data)
print(f'{len(x)}, {n_data}')
sample_x = x[indices]

plt.figure()
# m.plot(sample_data)
plt.plot(x, data)
plt.plot(sample_x, sample_data)

for i,value in enumerate(sample_data):
    if i%4==0:
        bit0.append(value)
    elif i%4==1:
        bit1.append(value)
    elif i%4==2:
        bit2.append(value)
    elif i%4==3:
        bit3.append(value)
plt.figure()
m.plot(bit0)
plt.show()
import numpy as np

# position = [-10053,-8337,-7488,-6500,-5772, -4910, -3985, -2798, -770]
# position = [-10053,-8337,-7488,-6500,-5772, -4910, -3985, -2798, -53]
position = [-16673,-13759,-12629,  -11815 ,  -10982 , -10237 , -9242 , -8252 , -6362 , -3692 , -2562 , -1727 , -893  , 0     , 848   , 1937  , 3754 ]

n  = len(position)
delta_pos = []

for i in np.arange(n-1):
    delta_pos.append(position[i+1] -position[i])
delta_pos = np.array(delta_pos)
angle = delta_pos/10000 * 180
print(f'delta pos = {delta_pos}')
print(f'angle = {angle}')
print(f'sum = {sum(angle[:n-1])}')
print(f'sum pos = {sum(delta_pos)}')
print(f"len = {len(delta_pos)}")

offset = 427 /16
offset_pos = delta_pos - offset
print(f"offset pos = {offset_pos}")
print(f'sum offset pos = {sum(offset_pos)}')
import matplotlib as mpl
from mpl_toolkits.mplot3d import Axes3D
import numpy as np
import matplotlib.pyplot as plt
import pandas as pd

FILE = 'C:/Users/Niko-AMD/Documents/Drone/FeriDrone/ST/python_demos/data/comport_recv_imu_pos'
df = pd.read_csv(FILE + '.csv')

x = df['pos_z']
y = df['pos_y']
z = df['pos_x']

mpl.rcParams['legend.fontsize'] = 10

fig = plt.figure()
ax = fig.gca(projection='3d')

ax.plot(x, y, z, label='position')
plotObj = ax.legend()

plt.show()
#plt.savefig('{}_{}.png'.format(FILE, 'xyz'))

x = df['roll']
y = df['pitch']
z = df['yaw']

mpl.rcParams['legend.fontsize'] = 10

fig = plt.figure()
ax = fig.gca(projection='3d')

ax.plot(x, y, z, label='orientation')
ax.legend()

plt.show()
#plt.savefig('{}_{}.png'.format(FILE, 'rpy'))

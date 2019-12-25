import pandas as pd
import matplotlib.pyplot as plt
import os

FILE = 'C:/Users/Niko-AMD/Documents/Drone/FeriDrone/ST/comport_recv_3float'
PLOTS = [['roll','pitch','yaw']]
#PLOTS = [['header'],['gyrX','gyrY','gyrZ'],['accX','accY','accZ']]
#FILE = 'C:/Users/Niko-AMD/Documents/Drone/FeriDrone/ST/comport_recv_imu_pos'
#PLOTS = [['roll','pitch','yaw'],['pos_x', 'pos_y', 'pos_z'],['pos_x', 'pos_y', 'pos_z','roll','pitch','yaw']]

df = pd.read_csv(FILE + '.csv')
df.plot(subplots=True)
plt.show()

for plot_group in PLOTS:
    for feat in plot_group:
        plt.plot(df[feat])
        plt.show()
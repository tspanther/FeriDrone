import pandas as pd
import matplotlib.pyplot as plt
import os

FILE = 'C:/Users/Niko-AMD/Documents/Drone/FeriDrone/ST/python_demos/data/drone_app2/NAGIB'
#PLOTS = [['roll','pitch','yaw'], ['pos_x', 'pos_y', 'pos_z']]
PLOTS = [['roll','pitch','yaw']]

df = pd.read_csv(FILE + '.csv')
df.plot(subplots=True)
plt.show()

for plot_group in PLOTS:
    for feat in plot_group:
        plt.plot(df[feat])
    plt.show()
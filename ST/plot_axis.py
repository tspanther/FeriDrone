import pandas as pd
import matplotlib.pyplot as plt
import os

SHOW = False
SAVE = True
FILE = 'C:/Users/Niko-AMD/Documents/Drone/FeriDrone/ST/comport_recv_3float'
#PLOTS = [['header'],['gyrX','gyrY','gyrZ'],['accX','accY','accZ']]
PLOTS = [['roll','pitch','yaw']]

df = pd.read_csv(FILE + '.csv')
df.plot(subplots=True)
if (SHOW):
    plt.show()
if (SAVE):
    if (SHOW):
        df.plot(subplots=True) # context was cleared on show
    plt.savefig('{}_plot.png'.format(FILE))
plt.clf()

for plot_group in PLOTS:
    for feat in plot_group:
        plt.plot(df[feat])
    if (SHOW):
        plt.show()
    if (SAVE):
        fn = "_".join(plot_group)
        if (SHOW):
            for feat in plot_group:
                plt.plot(df[feat]) # context was cleared on show
        plt.savefig('{}_plot_{}.png'.format(FILE, fn))
    plt.clf()
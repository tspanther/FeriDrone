import numpy as np
import matplotlib.pyplot as plt
import pandas as pd
import low_pass

FILE = 'C:/Users/Niko-AMD/Documents/Drone/FeriDrone/ST/python_demos/data/comport_recv_pwm_tyrp_little'
df = pd.read_csv(FILE + '.csv')

periods = ['1e','2e','3e','4e','5e','6e','7e','8e','9e']
pulses = ['1u','2u','3u','4u','5u','6u','7u','8u','9u']

kernel = low_pass.gauss_kernel(7)

for i in range(9):
    per = low_pass.convolution(df[periods[i]].to_numpy(), kernel)
    plt.plot(per)
    plt.savefig(FILE + '_period{}.png'.format(i))
    plt.clf()

    pul = low_pass.convolution(df[pulses[i]].to_numpy(), kernel)
    plt.plot(pul)
    plt.savefig(FILE + '_pulse{}.png'.format(i))
    plt.clf()

    u_minus_e = low_pass.convolution(df[pulses[i]].to_numpy() - df[periods[i]].to_numpy(), kernel)
    plt.plot(u_minus_e)
    plt.savefig(FILE + '_uminuse{}.png'.format(i))
    plt.clf()

    e_minus_u = low_pass.convolution(df[periods[i]].to_numpy() - df[pulses[i]].to_numpy(), kernel)
    plt.plot(e_minus_u)
    plt.savefig(FILE + '_eminusu{}.png'.format(i))
    plt.clf()

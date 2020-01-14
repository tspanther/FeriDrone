import numpy as np 

with open('C:/Users/Niko-AMD/Documents/Drone/FeriDrone/ST/python_demos/data/test.csv') as fd:
    sums1 = []
    sums2 = []
    while (True):
        line = fd.readline()
        if line == '':
            break
        arr = line.split(',')
        sum1 = 0
        sum2 = 0
        for i in range(1, len(arr), 2):
            sum1 += int(arr[i])
            sum2 += int(arr[i + 1])
        sums1.append(sum1)
        sums2.append(sum2)
        print('sum1: {}, sum2: {}, sum1-sum2: {}'.format(sum1, sum2, sum1-sum2))
    s1 = np.array(sums1)
    s2 = np.array(sums2)
    print('sums1: avg: {} std: {} min: {} max: {}\nsums2: avg: {} std: {} min: {} max: {}'
    .format(np.average(s1), np.std(s1), np.max(s1), np.min(s1),
            np.average(s2), np.std(s2), np.max(s2), np.min(s2)))
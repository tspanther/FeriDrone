import numpy as np
from scipy import signal

def convolution(arr, kernel):
    arr_out = np.copy(arr)
    for i in range(len(kernel) // 2, len(arr) - len(kernel) // 2 - 1):
        arr_out[i] = _conv(kernel, arr[i - len(kernel) // 2:i + len(kernel) // 2 + 1])
    return arr_out

def _conv(arr, kernel):
    sigm = 0
    for i in range(len(kernel)):
        sigm += kernel[i] * arr[i]
    return sigm

def gauss_kernel(length):
    krnl = signal.gaussian(length, std=np.max([length // 8, 1]))
    krnl /= np.sum(krnl)
    return krnl
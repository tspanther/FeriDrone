import struct
import numpy as np

def parse_uint16s(packet):
    arr = []
    for i in range(8):
        bytes_ = packet[i * 2 + 4:i * 2 + 6]
        val = int.from_bytes(bytes_, byteorder='little', signed=False)
        arr.append(val)
    return arr

def parse_floats(packet):
    arr = []
    for i in range(4):
        bytes_ = packet[i * 4 + 4:i * 4 + 8]
        val = struct.unpack('<f', bytes_)[0]
        arr.append(val)
    return arr

def parse_uint32s(packet):
    arr = []
    for i in range(4):
        bytes_ = packet[i * 4 + 4:i * 4 + 8]
        val = int.from_bytes(bytes_, byteorder='little', signed=False)
        arr.append(val)
    return arr

def parse_autolander_debug(packet):
    arr = parse_floats(packet)
    arr[2] = int(np.round(arr[2]))
    arr[3] = int(np.round(arr[3]))
    return arr

def join_pwm(arr):
    joined = []
    for i in range(0, len(arr) - 1, 2):
        joined.append(arr[i] + arr[i + 1])
    return joined
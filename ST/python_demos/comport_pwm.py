import serial
import struct

FREQ = 50
DURATION = 20
PORT = 'COM3'
OTPT_FILE = 'pwm_tyrp'

port = serial.Serial(PORT, 9600, timeout=1, parity=serial.PARITY_NONE, stopbits=serial.STOPBITS_ONE, bytesize=serial.EIGHTBITS)
with open('ST/python_demos/data/comport_recv_{}_little.csv'.format(OTPT_FILE), 'w') as fd:
    fd.write('idx,1e,1u,2e,2u,3e,3u,4e,4u,5e,5u,6e,6u,7e,7u,8e,8u,9e,9u\n')
    dataArr = []
    for i in range(FREQ * DURATION):
        data = port.read(9 * 2 * 4)
        dataArr.append(data)
    for i in range(FREQ * DURATION):
        pData = []
        for k in range(0, 9 * 4 * 2, 4):
            bytes_ = dataArr[i][k:k+4]
            pData.append(int.from_bytes(bytes_, byteorder='little', signed=False))
        fd.write(str(i) + "," + ",".join(str(x) for x in pData) + '\n')
        
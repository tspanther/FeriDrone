import serial
import struct

FREQ = 60
DURATION = 15
PORT = 'COM3'
OTPT_FILE = '3float'

port = serial.Serial(PORT, 9600, timeout=1, parity=serial.PARITY_NONE, stopbits=serial.STOPBITS_ONE, bytesize=serial.EIGHTBITS)
with open('comport_recv_{}.csv'.format(OTPT_FILE), 'w') as fd:
    fd.write('roll,pitch,yaw\n')
    while(True):
        temp = port.read(1)
        if (temp == b'\xab'):
            temp = port.read(1)
            if (temp == b'\xaa'):
                break
    data = port.read(14)
    dataArr = []
    for i in range(FREQ * DURATION):
        data = port.read(16)
        dataArr.append(data)
    for i in range(FREQ * DURATION):
        pData = []
        for k in range(4,13,4):
            bytes_ = dataArr[i][k:k+4]
            pData.append(struct.unpack('>f', bytes_)[0])
        fd.write(str(i) + "," + ",".join(str(x) for x in pData) + '\n')
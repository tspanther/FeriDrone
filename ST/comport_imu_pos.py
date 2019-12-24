import serial
import struct

FREQ = 200 + 100
DURATION = 10
PORT = 'COM3'
OTPT_FILE = 'imu_pos'

port = serial.Serial(PORT, 9600, timeout=1, parity=serial.PARITY_NONE, stopbits=serial.STOPBITS_ONE, bytesize=serial.EIGHTBITS)
with open('comport_recv_{}.csv'.format(OTPT_FILE), 'w') as fd:
    fd.write('pos_x,pos_y,pos_z,roll,pitch,yaw\n')
    while(True):
        temp = port.read(1)
        if (temp == b'\xaa'):
            temp = port.read(1)
            if (temp == b'\xab'):
                break
    data = port.read(14)
    dataArr = []
    for i in range(FREQ * DURATION):
        data = port.read(16)
        dataArr.append(data)
    prev_pos = [0.0, 0.0, 0.0]
    for i in range(FREQ * DURATION):
        pData = []
        if dataArr[i][1] == 171:
            pData.append(prev_pos[0])
            pData.append(prev_pos[1])
            pData.append(prev_pos[2])
            for k in range(4,13,4):
                bytes_ = dataArr[i][k:k+4]
                pData.append(struct.unpack('<f', bytes_)[0])
            fd.write(str(i) + "," + ",".join(str(x) for x in pData) + '\n')
        else:
            prev_pos = []
            for k in range(4,13,4):
                bytes_ = dataArr[i][k:k+4]
                prev_pos.append(struct.unpack('<f', bytes_)[0])
        
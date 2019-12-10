import serial

FREQ = 100
DURATION = 10
PORT = 'COM3'
OTPT_FILE = 'helloworld'

port = serial.Serial(PORT, 9600, timeout=1, parity=serial.PARITY_NONE, stopbits=serial.STOPBITS_ONE, bytesize=serial.EIGHTBITS)
with open('comport_recv_{}.csv'.format(OTPT_FILE), 'w') as fd:
    fd.write('index,header,gyrX,gyrY,gyrZ,accX,accY,accZ\n')
    while(True):
        temp = port.read(1)
        if (temp == b'\xaa'):
            temp = port.read(1)
            if (temp == b'\xaa'):
                break
    data = port.read(12)
    for i in range(FREQ * DURATION):
        data = port.read(14)
        pData = []
        for k in range(0,13,2):
            pData.append(int.from_bytes(data[k:k+2], byteorder='big', signed=True))
        fd.write(str(i) + "," + ",".join(str(x) for x in pData) + '\n')
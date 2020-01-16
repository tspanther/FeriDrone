import serial
import parse
import os
from timeit import default_timer as timer

# send modes
SEND_TRILATERATION = 1
SEND_NAGIB = 1
SEND_ALTITUDE = 1
SEND_PWM_RAW = 1
SEND_PWM_GEN = 1
SEND_PWM_OUTGOING = 0
SEND_AUTOLANDER_DEBUG = 1

# freq
FREQ_TRILATERATION = 30
FREQ_NAGIB = 60
FREQ_ALTITUDE = 30
FREQ_PWM = 50

# headers
headers = {
    'HEADER_TRILATERATION': b'\xAC',
    'HEADER_NAGIB': b'\xAB',
    'HEADER_ALTITUDE': b'\xA1',
    'HEADER_PWM_RAW': b'\xAE',
    'HEADER_PWM_GEN': b'\xAF',
    'HEADER_PWM_OUTGOING': b'\xA0',
    'HEADER_AUTOLANDER_DEBUG': b'\xA3'
}

DURATION = 50

PORT = 'COM3'
OTPT_FOLDER = 'drone_app'

freq= FREQ_TRILATERATION * SEND_TRILATERATION + FREQ_NAGIB * SEND_NAGIB + FREQ_ALTITUDE * SEND_ALTITUDE + FREQ_PWM * (SEND_PWM_GEN + SEND_PWM_OUTGOING + SEND_PWM_RAW) * 2
packet_size = 5 * 4
packets_n = (freq * DURATION + SEND_AUTOLANDER_DEBUG)

# recv
port = serial.Serial(PORT, 9600, timeout=1, parity=serial.PARITY_NONE, stopbits=serial.STOPBITS_ONE, bytesize=serial.EIGHTBITS)
packets = []
while(True): # find first packet
    temp = port.read(1)
    if (temp == b'\xAA'):
        temp = port.read(1)
        if (temp in headers.values()):
            temp = port.read(1)
            if (temp == b'\xAA'):
                port.read(packet_size - 3)
            else:
                port.read(packet_size - 5)
            break
print('aligned on packet. ready!')

# read packets
start = timer()
for i in range(packets_n):
    data = port.read(packet_size)
    packets.append(data)
end = timer()
print('Time elapsed: {}'.format(end - start))

# parse
TRILATERATION = []
NAGIB = []
ALTITUDE = []
PWM_RAW = []
PWM_GEN = []
PWM_OUTGOING = []
AUTOLANDER_DEBUG = []

for packet in packets:
    if packet[1] == 161:
        ALTITUDE.append(parse.parse_floats(packet)[0])
    elif packet[1] == 163:
        AUTOLANDER_DEBUG.append(parse.parse_autolander_debug(packet))
    elif packet[1] == 171:
        NAGIB.append(parse.parse_floats(packet)[0:3])
    elif packet[1] == 175:
        PWM_GEN.append(parse.parse_uint32s(packet))
    elif packet[1] == 160:
        PWM_OUTGOING.append(parse.parse_uint32s(packet))
    elif packet[1] == 174:
        PWM_RAW.append(parse.parse_uint32s(packet))
    elif packet[1] == 172:
        TRILATERATION.append(parse.parse_floats(packet)[0:3])
    else:
        print('invalid packet {}'.format(packet))

PWM_GEN = parse.join_pwm(PWM_GEN)
PWM_OUTGOING = parse.join_pwm(PWM_OUTGOING)
PWM_RAW = parse.join_pwm(PWM_RAW)

files = [
    [ TRILATERATION, 'TRILATERATION', 'idx,pos_x,pos_y,pos_z' ],
    [ NAGIB, 'NAGIB', 'idx,roll,pitch,yaw' ],
    [ ALTITUDE, 'ALTITUDE', 'idx,altitude' ],
    [ PWM_RAW, 'PWM_RAW', 'idx,ch0,ch1,ch2_throttle,ch3,ch4,ch5_autolander,ch6,ch7' ],
    [ PWM_GEN, 'PWM_GEN', 'idx,ch0,ch1,ch2_throttle,ch3,ch4,ch5_autolander,ch6,ch7' ],
    [ PWM_OUTGOING, 'PWM_OUTGOING', 'idx,ch0,ch1,ch2_throttle,ch3,ch4,ch5_autolander,ch6,ch7' ],
    [ AUTOLANDER_DEBUG, 'AUTOLANDER_DEBUG', 'idx,sVec,sAlt,tsDecel,tsStopDecel' ]
]

try:
    os.makedirs('ST/python_demos/data/{}'.format(OTPT_FOLDER))
except:
    pass
for file_ in files:
    with open('ST/python_demos/data/{}/{}.csv'.format(OTPT_FOLDER, file_[1]), 'w') as fd:
        fd.write('{}\n'.format(file_[2]))
        for i in range(len(file_[0])):
            fd.write(str(i) + "," + ",".join(str(x) for x in file_[0][i]) + '\n')

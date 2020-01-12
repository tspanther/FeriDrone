import numpy
import serial
import struct
from time import sleep

FREQ = 50
DURATION = 20
PORT = 'COM3'

class AutoLander():
    def __init__(self, objectMass, objectMaxThrust, objectNeutralThrust):
        self.currentAltitude = 0.0
        self.currentThrust = 0.0

        self.port = serial.Serial(PORT, 9600, timeout=1, parity=serial.PARITY_NONE, stopbits=serial.STOPBITS_ONE, bytesize=serial.EIGHTBITS)
        

    def addHeightMeasurement(self, time, height):
        self.currentAltitude = height
        sleep(0.035)

        # transmit usb
        height_bytes = struct.pack('<f', height)
        self.port.write(height_bytes)

        # receive usb
        #thrust = self.port.read(4)
        #self.currentThrust = struct.unpack('<f', thrust)[0]


    @property
    def heightFiltered(self):
        return self.currentAltitude

    @property
    def thrust(self):
        return self.currentThrust

import numpy
import airsim
import time
from threading import Thread
import win32api
import matplotlib.pyplot as plt

class Recorder:
    def __init__(self, seconds):
        self.controls = [0x57, 0x53 , 0x41, 0x44, 0x26, 0x28, 0x25, 0x27]
        self.keyPressDurations = numpy.zeros(8)
        self.seconds = seconds
        self.remaining = seconds
        self.historicalValues = numpy.array([0, 0, 0, 0])

    def record(self):
        threads = []
        for i in range(8):
            t = Thread(target=self.recordKey, args=(self.controls[i], i))
            threads.append(t)
            t.start()

        commThread = Thread(target=self.airSimCommunicator, args=())
        commThread.start()

        for i in range(8):
            threads[i].join()

        commThread.join()
        
        client.armDisarm(False)
        client.enableApiControl(False)

        plt.plot(self.historicalValues.T)
        plt.show()

    def f(self, x):
        if (x > 500): # max after 500 samples (1s)
            x = 500
        return numpy.sqrt(numpy.sqrt(x))

    def airSimCommunicator(self):
        while(self.remaining > 0):
            time.sleep(0.02)
            self.remaining -= 0.02

            kpDur = numpy.array([self.f(xi) for xi in self.keyPressDurations])

            max_ = numpy.sqrt(numpy.sqrt(500))
            throttle = (kpDur[0] - kpDur[1]) / max_ * 0.5
            yaw = (kpDur[2] - kpDur[3]) / max_ * 0.5
            pitch = (kpDur[4] - kpDur[5]) / max_ * 0.5
            roll = (kpDur[6] - kpDur[7]) / max_ * 0.5

            rcdata = airsim.RCData(0, pitch=pitch, roll=roll, throttle=throttle, yaw=yaw, is_initialized=True, is_valid=True)
            global client
            client.moveByRC(rcdata)

            # debug
            self.historicalValues = numpy.c_[self.historicalValues, [throttle, yaw, pitch, roll]]

    def recordKey(self, key, index):
        while(self.remaining > 0):
            if win32api.GetAsyncKeyState(key):
                self.keyPressDurations[index] += 1
            else:
                self.keyPressDurations[index] = 0
            time.sleep(0.002)
            
client = None

if __name__ == '__main__':
    client = airsim.MultirotorClient()
    client.confirmConnection()
    client.enableApiControl(True)
    client.armDisarm(True)
    client.moveByManualAsync(vx_max = 1E6, vy_max = 1E6, z_min = -1E6, duration = 1E10)
    
    seconds = 100
    recorder = Recorder(seconds)
    recorder.record()

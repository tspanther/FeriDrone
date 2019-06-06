import numpy
import airsim
import pyaudio
import time
from threading import Thread
import win32api
import matplotlib.pyplot as plt
import autolander
import queue
import logging
from timeit import default_timer as timer

mode = False # False = manual, True = automatic

class Recorder():
    def __init__(self, key=0x57):
        self.key = key
        self.stop = False
        self.keyThread = Thread(target=self.recordKey, args=())
        self.keyThread.setDaemon(True)
        self.keyThread.start()

    def recordKey(self):
        while(self.stop is False):
            if win32api.GetAsyncKeyState(self.key):
                global mode
                mode = not mode
                time.sleep(1)
            time.sleep(0.002)
            
    def poor_mans_destructor(self):
        self.stop = True
        self.keyThread.join()
        print("joined record thread")
            
CHUNK = 882
FORMAT = pyaudio.paInt16
CHANNELS = 2
RATE = 44100
RECORD_SECONDS = 100

q = None
hist = numpy.zeros((8, 1))
histVhod = numpy.zeros(RATE * (RECORD_SECONDS))
done = False

def normaliziraj_vektorsko(vektor):
    largest = max(abs(numpy.max(vektor)), abs(numpy.min(vektor)))
    ret = vektor
    if (largest != 0):
        ret = vektor*(1/largest)
    return ret


def thread_obdelava(q):
    global hist
    global histVhod
    global done

    chunkN = 0

    while(True):
        try:
            data = q.get()

            vhod = normaliziraj_vektorsko(numpy.fromstring(data, numpy.int16))
            vhod = vhod.flatten()[0:vhod.size:2]
            # najprej iscemo prvi vzorec manjsi od -0.5, nato alterniramo med manjsi/vecji
            vecOd05 = False
            threshhold = -0.5
            downs = numpy.zeros(9)
            ups = numpy.zeros(9)
            kanal = 0
            for i in range(0, CHUNK, 1):
                if (kanal >= 9):
                    break
                if (vecOd05):
                    if (vhod[i] > threshhold):
                        ups[kanal] = i
                        #prepare for next
                        kanal += 1
                        vecOd05 = False
                        i += 5
                else:
                    if (vhod[i] < threshhold):
                        downs[kanal] = i
                        # prepare for next
                        vecOd05 = True
                        i += 5
            kanali = numpy.zeros(8)
            for i in range(0, 8, 1):
                kanali[i] = downs[i + 1] - ups[i]
            
            MAX_KANAL = 71
            MIN_KANAL = 26
            RANGE = MAX_KANAL - MIN_KANAL
            global client
            # throttle = (kanali[2] - MIN_KANAL)/RANGE-0.5
            # print(throttle)
            client.moveByRC(rcdata=airsim.RCData(throttle=(kanali[2] - MIN_KANAL)/RANGE-0.5, yaw=(kanali[3] - MIN_KANAL)/RANGE-0.5, pitch=(
                kanali[1] - MIN_KANAL)/RANGE - 0.5, roll=(kanali[0] - MIN_KANAL)/RANGE - 0.5, is_initialized=True, is_valid=True))

            hist = numpy.c_[hist, kanali.T]
            for k in range(CHUNK):
                histVhod[k + chunkN * CHUNK] = vhod[k]

            chunkN += 1
            q.task_done()
        except Exception as e:
            logging.exception("error get")
            break

    return None

client = None

def find_offset(data):
    data = normaliziraj_vektorsko(numpy.fromstring(data, numpy.int16))
    data = data.flatten()[0:data.size:2]

    samplesAbove05 = 0
    for i in range(data.size):
        if (data[i] >= -0.5):
            samplesAbove05 += 1
        else:
            samplesAbove05 = 0
        if (samplesAbove05 >= 200):
            return i - 180

    raise(Exception('niko je neumen'))
    return -1



if __name__ == '__main__':
    keyRecorder = Recorder()
    
    client = airsim.MultirotorClient()
    client.confirmConnection()
    client.enableApiControl(True)
    client.armDisarm(True)
    client.moveByManualAsync(vx_max = 1E6, vy_max = 1E6, z_min = -1E6, duration = 1E10)
    
    p = pyaudio.PyAudio()

    stream = p.open(format=FORMAT,
                    channels=CHANNELS,
                    rate=RATE,
                    input=True,
                    frames_per_buffer=CHUNK)

    thread_obd = None
    al = None
    start = timer()
    
    historicalAltitudes = []

    for i in range(0, int(RATE / CHUNK * RECORD_SECONDS)):
        if (mode == False):
            if (thread_obd is None):
                q = queue.Queue()
                thread_obd = Thread(target=thread_obdelava, args=(q,))
                thread_obd.setDaemon(True)
                thread_obd.start()
            if i == 0:
                data = stream.read(RATE)
                data = stream.read(CHUNK + 200)
                start = timer()
                offset = find_offset(data)
                print(offset)
                end = timer()
                offset -= int((end-start) * RATE)
                print(end-start)
                offset %= CHUNK
                data = stream.read(offset) # junk, line up with
            else:
                if (i % 1000 == 0):
                    data = stream.read(CHUNK + 200)
                    start = timer()
                    offset = find_offset(data)
                    print(offset)
                    end = timer()
                    offset -= int((end-start) * RATE)
                    print(end-start)
                    offset %= CHUNK
                    data = stream.read(offset) # junk, line up with
                try:
                    data = stream.read(CHUNK)
                    q.put(data)
                except Exception as e:
                    logging.exception("error put")
        else:
            if (thread_obd is not None):
                done = True
                thread_obd.join()
                
                q.join()
                
                al = autolander.AutoLander(0.55, 13, 0.4)
            
            altitude = client.simGetVehiclePose().z_val
            historicalAltitudes.append(altitude)
            now = timer()
            al.addHeightMeasurement(now-start, altitude)
            throttle = al.thrust / 13 - 0.5
            client.moveByRC(airsim.RCData(throttle=throttle, is_initialized=True, is_valid=True))
            time.sleep(0.002)

    stream.stop_stream()
    stream.close()
    p.terminate()
    keyRecorder.poor_mans_destructor()
    
    plt.plot(histVhod)
    plt.show()
    
    plt.plot(historicalAltitudes)
    plt.show()

    for i in range(8):
        plt.plot(hist[i])
    plt.show()
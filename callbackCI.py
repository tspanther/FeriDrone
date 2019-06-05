import numpy
import matplotlib.pyplot as plt
from scipy.io import wavfile
import pyaudio
import struct
import wave
import airsim

CHUNK = 882
FORMAT = pyaudio.paInt16
CHANNELS = 2
RATE = 44100
RECORD_SECONDS = 1
hist = numpy.zeros((8, 1))
histVhod = numpy.zeros(RATE * (RECORD_SECONDS+1))
nChunks = int(RECORD_SECONDS * (RATE/CHUNK))
cChunks = 0

def normaliziraj_vektorsko(vektor):
    largest = max(abs(numpy.max(vektor)), abs(numpy.min(vektor)))
    ret = vektor
    if (largest != 0):
        ret = vektor*(1/largest)
    return ret
     
def callback(in_data, frame_count, time_info, status_flags):
    global hist
    global histVhod
    global nChunks
    global cChunks

    vhod = numpy.fromstring(in_data, numpy.int16)
    vhod = vhod.flatten()[0:vhod.size:2]
    for k in range(CHUNK):
        histVhod[k+cChunks*CHUNK] = vhod[k]
    vecOd05 = False # najprej iscemo prvi vzorec manjsi od -0.5, nato alterniramo med manjsi/vecji
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
    hist = numpy.c_[hist, kanali.T]

    cChunks += 1
    if (cChunks == nChunks):
        return (None, pyaudio.paComplete)
    else:
        return (None, pyaudio.paContinue)

if __name__ == '__main__':
	p = pyaudio.PyAudio()

	stream = p.open(format=FORMAT,
					channels=CHANNELS,
					rate=RATE,
					input=True,
					frames_per_buffer=CHUNK,
                    stream_callback=callback)
			
    
	plt.plot(histVhod)
	plt.show()

	for i in range(8):
		plt.plot(hist[i])
	plt.show()
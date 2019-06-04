import numpy
import matplotlib.pyplot as plt
from scipy.io import wavfile

def normaliziraj_vektorsko(vektor):
    largest = max(abs(numpy.max(vektor)), abs(numpy.min(vektor)))
    ret = vektor
    if (largest != 0):
        ret = vektor*(1/largest)
    return ret
            


if __name__ == '__main__':
    filename = 'left_joystick.wav'
    Fs, y = wavfile.read(filename)
    y = y.copy()
    y.setflags(write=1)
    #y = normaliziraj_vektorsko(y)

    y = y[1000:y.size:1] # trim

    sirinaPulza = 882
    nPulz = 0
    hist = numpy.zeros((8, 1))
    while(True):
        if ((nPulz + 1) * sirinaPulza >= int(y.size / 2)):
        #if ((nPulz + 1) * sirinaPulza >= 44100 + 1000):
            break
        vhod = y[nPulz * sirinaPulza:(nPulz + 1) * sirinaPulza:1]
        vhod = normaliziraj_vektorsko(vhod)
        vecOd05 = False # najprej iscemo prvi vzorec manjsi od -0.5, nato alterniramo med manjsi/vecji
        threshhold = -0.5
        downs = numpy.zeros(9)
        ups = numpy.zeros(9)
        kanal = 0
        for i in range(0, sirinaPulza, 1):
            if (kanal >= 9):
                break
            if (vecOd05):
                if (vhod[i][0] > threshhold):
                    ups[kanal] = i
                    #prepare for next
                    kanal += 1
                    vecOd05 = False
                    i += 5
            else:
                if (vhod[i][0] < threshhold):
                    downs[kanal] = i
                    # prepare for next
                    vecOd05 = True
                    i += 5
        kanali = numpy.zeros(8)
        for i in range(0, 8, 1):
            kanali[i] = downs[i + 1] - ups[i]
        hist = numpy.c_[hist, kanali.T]
        nPulz += 1
    
    #hist = hist.T
    for i in range(8):
        plt.plot(hist[i])
    plt.show()


    

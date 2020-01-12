import numpy as np
import matplotlib.pyplot as plt

## Noise generator - do not touch!!!
def awgn_noise_single_element(x, SNR_dB):
    #np.random.seed(1)   # set the random generator seed to same value (for comparison only)
    L = x.size
    SNR = np.power(10, (SNR_dB/10))     # SNR to linear scale
    Esym = np.sum(np.abs(x)**2) / (L)   # Calculate actual symbol energy
    N0 = Esym / SNR                     # Find the noise spectral density
    if np.isreal(x).all():
        noiseSigma = np.sqrt(N0)        # Standard deviation for AWGN Noise when x is real
        n = noiseSigma * np.random.randn(1, L)
    else:
        noiseSigma = np.sqrt(N0/2)      # Standard deviation for AWGN Noise when x is complex
        n = noiseSigma * (np.random.randn(1, L) + complex(0, 1) * np.random.randn(1, L))

    return n[-1, -1]

class GravitySim():
    '''
    Ustvarimo nov simulator.

    AutoLanderClass - razred AutoLander (ne objekt!), v katerem ste implementirali
    lastno implementacijo pristajanja.

    Parametri:
    AutoLanderClass - razred, ki implemetira zahtevan vmesnik
    timeDuration - čas trajanja simulacije
    timeStep - korak simulacije
    snr_dB - izbrana snr dB vrednost za šum, lahko ekperimentiramo
    objectMass - float, masa objekta v kg
    objectMaxThrust - maksimalni potisk motorjev, v N
    startHeight - začetna višina simulacije v m
    startVelocity - začetna hitrost v m/s (pozitivna je navzgor, negativna navzdol)
    '''
    def __init__(self,
                 AutoLanderClass,
                 animate=False,         # animate the plots
                 timeDuration=3,       # seconds
                 timeStep=0.0035,        # seconds
                 snr_dB=12.0,           # power of signal over noise
                 objectMass=0.5,        # kg
                 objectMaxThrust=11.06,  # F (force of engines)
                 startHeight=1,         # meters (initial height)
                 startVelocity=0.0,       # meters / seconds (initial velocity)
                 startThrust=0):        # percentage of maxThrust (initial force)

        self.simTimeIndex = 0    # index of time step in simulation
        self.AutoLanderClass = AutoLanderClass
        self.autoLanderObj = None
        self.animate = animate

        ## simulation variables
        self.timeDuration = timeDuration   # seconds
        self.timeStep = timeStep # seconds

        self.g = -9.81           # meters / seconds * seconds (gravitational change of velocity)
        self.snr_dB = snr_dB # power of signal over noise

        ## object variables

        # properties
        self.objectMass = objectMass
        self.objectMaxThrust = objectMaxThrust
        self.objectNeutralThrust = (self.objectMass * self.g / objectMaxThrust) * -1

        # state
        self.startHeight = startHeight # meters (initial height)
        self.startVelocity = startVelocity  # meters / seconds (initial velocity)
        self.startThrust = startThrust # percentage of maxThrust (initial force)

        # history
        self.historyTime = np.arange(self.timeStep,
                                     self.timeDuration+self.timeStep,
                                     self.timeStep)

        self.historyHeight = np.zeros(self.historyTime.shape)
        self.historyHeightWithNoise = np.zeros(self.historyTime.shape)
        self.historyHeightWithFilter = np.zeros(self.historyTime.shape)

        self.historyThrust = np.zeros(self.historyTime.shape)
        self.historyForce = np.zeros(self.historyTime.shape)
        self.historyVelocity = np.zeros(self.historyTime.shape)
        self.historyEnergy = np.zeros(self.historyTime.shape)

    def run(self):
        '''
        Pozene simulacijo, stopi skozi vse korake 
        in sproti prikazuje rezultate (ce je animate=True).
        '''
        # prepare model
        currentHeight = self.startHeight
        currentVelocity = self.startVelocity
        currentThrust = self.startThrust
        currentEnergy = 0.5 * self.objectMass * np.power(currentVelocity, 2)

        autoLanderObj = self.AutoLanderClass(self.objectMass,
                                             self.objectMaxThrust,
                                             self.objectNeutralThrust)
        self.autoLanderObj = autoLanderObj

        ts_last_update = 0
        if self.animate:
            self.animation_setup()

        # simulation model loop
        for ts in np.arange(1, self.historyTime.shape[0], 1):
            ## prepare for new loop ...
            previousHeight = currentHeight
            previousVelocity = currentVelocity

            # simulate noise :P
            currentNoise = awgn_noise_single_element(self.historyHeight[0:ts+1], self.snr_dB)
            currentHeight = currentHeight + currentNoise

            autoLanderObj.addHeightMeasurement(self.historyTime[ts], currentHeight)
            currentHeightFiltered = autoLanderObj.heightFiltered
            self.historyHeightWithFilter[ts] = currentHeightFiltered
            currentThrust = autoLanderObj.thrust

            ## comply with controll restraints
            if currentThrust < 0:
                currentThrust = 0

            if currentThrust > 1:
                currentThrust = 1

            self.historyThrust[ts] = currentThrust

            ## calculate new state ...
            # new force on object = force of gravity + current thrust
            currentForce = (self.objectMass * self.g) + (currentThrust * self.objectMaxThrust)
            self.historyForce[ts] = currentForce
            # new velocity of object = previous velocity + force * time / mass
            currentVelocity = previousVelocity + (currentForce * self.timeStep) / self.objectMass
            self.historyVelocity[ts] = currentVelocity
            # new displacement of object

            #dH = previousVelocity*timeStep + 0.5*timeStep**2*currentAcc
            #dH = (previousVelocity + currentVelocity) * 0.5 * self.timeStep
            currentAcc = currentForce*self.timeStep/self.objectMass
            dH = previousVelocity*self.timeStep + \
                0.5 * currentAcc*self.timeStep**2
            # new height of object
            currentHeight = previousHeight + dH
            self.historyHeight[ts] = currentHeight
            self.historyHeightWithNoise[ts] = currentHeight + currentNoise
            # new kinetic energy of object (or force at distance 0)
            currentEnergy = 0.5 * self.objectMass * np.power(currentVelocity, 2)
            self.historyEnergy[ts] = currentEnergy

            if currentHeight <= 0:
                self.simTimeIndex = ts
                return

            if self.historyTime[ts]-self.historyTime[ts_last_update] >= 1/30:
                if self.animate:
                    self.animation_update()
                    ts_last_update = ts
        self.simTimeIndex = -1

    def printResult(self):
        '''
        Izpise koncne rezultate.
        '''
        ts = self.simTimeIndex
        if ts > 0:
            print('Simulation results:')
            print('Landing time:', self.historyTime[ts], 's')
            print('Landing speed:', self.historyVelocity[ts], 'm/s')
            print('Landing force:', self.historyEnergy[ts], 'N')
        else:
            print('Simulation failed ...')


    def animation_setup(self):
        '''
        setup animation frame
        '''
        plt.figure()

        self.ax1 = plt.subplot(2, 2, 1)
        plt.title('Height of object')
        self.heightWithNoiseLine, = plt.plot(self.historyTime,
                                             self.historyHeightWithNoise,
                                             'r',
                                             label='noise')
        self.heightWithFilterLine, = plt.plot(self.historyTime,
                                              self.historyHeightWithFilter,
                                              'g',
                                              label='noise - filter')
        self.historyHeightLine, = plt.plot(self.historyTime,
                                           self.historyHeight,
                                           'b',
                                           label='real')
        plt.xlabel('time (s)'); plt.ylabel('height (m)')
        plt.legend()

        self.ax2 = plt.subplot(2, 2, 2)
        self.axx2 = self.ax2.twinx()
        plt.title('Force acted on object')
        self.historyForceLine, = plt.plot(self.historyTime,
                                          self.historyForce,
                                          label='force')
        self.historyThrustLine, = plt.plot(self.historyTime,
                                           self.historyThrust,
                                           label='thrust')
        self.ax2.set_ylabel('thrust (%)')
        self.axx2.set_ylabel('force (N)')
        plt.xlabel('time (s)')
        plt.legend()

        self.ax3 = plt.subplot(2, 2, 3)
        plt.title('Velocity of object')
        self.historyVelocityLine, = plt.plot(self.historyTime,
                                             self.historyVelocity)
        plt.xlabel('time (s)'); plt.ylabel('velocity (m/s)')

        self.ax4 = plt.subplot(2, 2, 4)
        plt.title('Kinectic energy of object')
        self.historyEnergyLine, = plt.plot(self.historyTime, self.historyEnergy)
        plt.xlabel('time (s)'); plt.ylabel('energy (J)')

        plt.show(block=False)
        plt.waitforbuttonpress(0.01)

    def animation_update(self):
        '''
        update animation frame
        '''
        self.heightWithNoiseLine.set_ydata(self.historyHeightWithNoise)
        self.heightWithFilterLine.set_ydata(self.historyHeightWithFilter)
        self.historyHeightLine.set_ydata(self.historyHeight)
        self.ax1.relim()
        self.ax1.autoscale_view()
        self.historyForceLine.set_ydata(self.historyForce)
        self.ax2.relim()
        self.ax2.autoscale_view()
        self.historyThrustLine.set_ydata(self.historyThrust)
        self.axx2.relim()
        self.axx2.autoscale_view()
        self.historyVelocityLine.set_ydata(self.historyVelocity)
        self.ax3.relim()
        self.ax3.autoscale_view()
        self.historyEnergyLine.set_ydata(self.historyEnergy)
        self.ax4.relim()
        self.ax4.autoscale_view()
        plt.draw()
        plt.waitforbuttonpress(0.01)


    def plot(self):
        '''
        plot progress
        '''
        plt.figure()

        plt.subplot(2, 2, 1)
        plt.title('Height of object')
        plt.plot(self.historyTime, self.historyHeightWithNoise, 'r', label='noise')
        plt.plot(self.historyTime, self.historyHeightWithFilter, 'g', label='noise - filter')
        plt.plot(self.historyTime, self.historyHeight, 'b', label='real')
        plt.xlabel('time (s)'); plt.ylabel('height (m)')
        plt.legend()

        ax = plt.subplot(2, 2, 2)
        axx = ax.twinx()
        plt.title('Force acted on object')
        plt.plot(self.historyTime, self.historyForce, label='force')
        plt.plot(self.historyTime, self.historyThrust, label='thrust')
        ax.set_ylabel('thrust (%)')
        axx.set_ylabel('force (N)')
        plt.xlabel('time (s)')
        plt.legend()

        plt.subplot(2, 2, 3)
        plt.title('Velocity of object')
        plt.plot(self.historyTime, self.historyVelocity)
        plt.xlabel('time (s)'); plt.ylabel('velocity (m/s)')

        plt.subplot(2, 2, 4)
        plt.title('Kinectic energy of object')
        plt.plot(self.historyTime, self.historyEnergy)
        plt.xlabel('time (s)'); plt.ylabel('energy (J)')

        plt.show()

from autolander_comport import AutoLander

if __name__ == '__main__':
    grav_sim = GravitySim(AutoLander,
                          animate=False)

    grav_sim.run()
    grav_sim.printResult()
    grav_sim.plot()

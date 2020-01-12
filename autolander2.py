import numpy

class AutoLander():
    def __init__(self, objectMass, objectMaxThrust, objectNeutralThrust):
        self.currentAltitude = 0
        self.objectMass = objectMass
        self.objectMaxThrust = objectMaxThrust
        self.objectNeutralThrust = objectNeutralThrust
        # compute g
        a_d = self.objectMaxThrust / self.objectMass
        Fg = self.objectNeutralThrust * self.objectMaxThrust
        self.g = Fg / self.objectMass
        
        self.timeStep = 0.035

        self.ts = 0 # time sample (index)

        # estimated start velocity and altitude
        self.sAlt = None
        self.sVel = None

        # properties for deceleration decision
        self.tsDecel = None # time sample to start decelerating
        self.tsStopDecel = None # time sample to stop decelerating
        self.stopDecel = 0 # deceleration complete when this reaches 3; increment when altitudes_MA > altitudes_HistMA during decel phase
        self.altitudes_MA = None # moving average of altitudes

        # prepare velocity computation arrays
        reactionTime = 0.35
        self.roughEstimationSampleNum = int(reactionTime / self.timeStep / 2) # number of velocities we can afford to compute to estimate start velocity
        self.averageVelocities = numpy.zeros(self.roughEstimationSampleNum) # velocities on first few (overlapping) time intervals
        self.h_Altitudes = numpy.zeros(self.roughEstimationSampleNum)

    def addHeightMeasurement(self, time, height):
        self.currentThrust = 0 # free fall
        self.currentAltitude = height

        if self.ts < self.roughEstimationSampleNum:
            self.h_Altitudes[self.ts] = height
        if self.ts > self.roughEstimationSampleNum and self.ts < self.roughEstimationSampleNum * 2:
            velocityStep = (self.ts - self.roughEstimationSampleNum // 2) * self.timeStep * self.g # substract from velocity based on which ts it was taken in (_v = v0 + g*t, we seek v0)
            self.averageVelocities[self.ts - self.roughEstimationSampleNum] = (self.h_Altitudes[self.ts-self.roughEstimationSampleNum] - self.currentAltitude) / (self.timeStep * self.roughEstimationSampleNum) - velocityStep
        # make rough estimate based on first few samples (to be able to react soon enough)
        if (self.ts == self.roughEstimationSampleNum * 2):
            self.makeDecision()
        
        # deceleration
        if (self.tsDecel is not None and self.tsDecel <= self.ts):
            self.currentThrust = self.objectMaxThrust
        if (self.tsStopDecel is not None and self.tsStopDecel <= self.ts):
            self.currentThrust = self.objectNeutralThrust - self.objectNeutralThrust / 20

        # end
        self.ts += 1

    def makeDecision(self):
        # estimate start velocity
        self.sVel = -numpy.average(self.averageVelocities) # statistically average velocity
        # estimate start altitude
        if (self.sAlt is None):
            for i in range(self.roughEstimationSampleNum):
                v_i = self.sVel - i * self.timeStep / 2 * self.g
                self.h_Altitudes[i] + v_i * self.timeStep * i
            self.sAlt = numpy.average(self.h_Altitudes)
        
        # decide deceleration start sample
        # h: height if v0 = 0 and aimedAltitude = 0
        h = self.sAlt + self.sVel**2 / 2 / self.g
        aimedAltitude = numpy.max([h / 20, 0.25])
        h -= aimedAltitude

        # calculate time sample to start slowing down
        F_d = self.objectMaxThrust - self.objectMass * self.g # Fd = Fmax - Fg
        a_d = F_d / self.objectMass
        x = (self.g * h / (self.g + a_d))
        h_allowTravel = h - x

        timeToStartDecel = numpy.sqrt(2*(h_allowTravel)/self.g)
        moveTimeStep = self.sVel / self.g / self.timeStep # due to the assumption that v0 = 0 and aimedAltitude = 0

        self.tsDecel = int(timeToStartDecel / self.timeStep + moveTimeStep)

        maxVelocity = timeToStartDecel * self.g
        decelTime = maxVelocity / a_d

        self.tsStopDecel = int(self.tsDecel + decelTime / self.timeStep) + 1

        return

    @property
    def heightFiltered(self):
        return self.currentAltitude

    @property
    def thrust(self):
        return self.currentThrust

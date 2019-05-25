import numpy

class KalmanFilter(object):

    def __init__(self, process_variance, estimated_measurement_variance):
        self.process_variance = process_variance
        self.estimated_measurement_variance = estimated_measurement_variance
        self.posteri_estimate = 0.0
        self.posteri_error_estimate = 1.0

    def input_latest_noisy_measurement(self, measurement):
        priori_estimate = self.posteri_estimate
        priori_error_estimate = self.posteri_error_estimate + self.process_variance

        blending_factor = priori_error_estimate / (priori_error_estimate + self.estimated_measurement_variance)
        self.posteri_estimate = priori_estimate + blending_factor * (measurement - priori_estimate)
        self.posteri_error_estimate = (1 - blending_factor) * priori_error_estimate

    def get_latest_estimated_measurement(self):
        return self.posteri_estimate

class AutoLander():
    def __init__(self, objectMass, objectMaxThrust, objectNeutralThrust):
        '''
        Inicializacija AutoLander implementacije.
        Tu prejmete informacije o letalniku.

        objectMass - float
            masa letalnika v kg

        objectMaxThrust - float
            maksimalna sila potiska motorjev v N

        objectNeutralThrust - float
            relativni delež potiska, da letalnik lahko lebdi, med 0 in 1
        '''
        self.currentAltitude = 0
        self.objectMass = objectMass
        self.objectMaxThrust = objectMaxThrust
        self.objectNeutralThrust = objectNeutralThrust
        # compute g
        a_d = self.objectMaxThrust / self.objectMass
        Fg = self.objectNeutralThrust * self.objectMaxThrust
        self.g = Fg / self.objectMass
        
        self.timeStep = None
        self.ttl = 10 # time to land

        self.ts = 0 # time sample (index)
        self.strategy = 'Niko'

        # Kalman Filter
        # self.snr_dB = snr_dB
        self.kFilter = None

        # history of (filtered altitudes)
        self.h_Altitudes = numpy.zeros(2)
        # estimated start velocity and altitude
        self.sAlt = None
        self.sVel = None

        # prepare velocity computation arrays
        self.roughEstimationSampleNum = None # number of velocities we can afford to compute to estimate start velocity
        self.averageVelocities = None # velocities on first few (overlapping) time intervals

        # properties for deceleration decision
        self.tsDecel = None # time sample to start decelerating
        self.tsStopDecel = None # time sample to stop decelerating
        self.stopDecel = 0 # deceleration complete when this reaches 3; increment when altitudes_MA > altitudes_HistMA during decel phase
        self.altitudes_HistMA = None # moving average of altitudes <self.roughEstimationSampleNum> samples back
        self.altitudes_MA = None # moving average of altitudes

        # time sample in which we want to reevaluate deceleration start
        self.tsReevaluate = None
        self.tsReevStep = 512 # by how much we move the reevaluation step; halved with each reevaluation


    def addHeightMeasurement(self, time, height):
        '''
        Prejemek posamezne meritve.
        time - float
            čas zajema, v s

        height - float
            izmerjena višina v m
        '''
        # filter
        # if (self.kFilter is None):
        #     SNR = numpy.power(10, (self.snr_dB/10))
        #     stdDevi = 2 * height / SNR
        #     process_variance = 3*10e-4
        #     self.kFilter = KalmanFilter(process_variance, stdDevi)

        # self.kFilter.input_latest_noisy_measurement(height)
        # self.h_Altitudes[self.ts] = self.kFilter.get_latest_estimated_measurement()
        # self.currentAltitude = self.h_Altitudes[self.ts]

        self.h_Altitudes[self.ts] = height
        self.currentAltitude = height

        # decision
        self.currentThrust = 0 # free fall

        if (self.strategy == 'PID'):
            if (pidController is None):
                pidController = PID(P, I, D, self.currentAltitude)
                pidController.setSetPoint(15)
                pidController.setWindup()
            thrustCorrection = pidController.update(self.currentAltitude) / objectMaxThrust
            if (self.ts % 50 == 0):
                debugBr = True
            if (numpy.absolute(thrustCorrection) < objectMaxThrust/10):
                pidController.setSetPoint(self.currentAltitude/2)
        elif (self.strategy == 'Niko'):
            '''
            TODO: 
            - parametrization of reaction time (greater SNR, greater reaction time)
            - better filter? Kalman with parametrization? Current one works worse as if there wasn't any
                dropped; - the best filter for white noise data in our application is moving average 
                (Kalman would only be good if we needed to know current altitude - but we are only interested in historical data, 
                which is best analyzed with averages for white noise data)
            IMPROVEMENTS:
            - graph of deceleration thrust not constant on thrust = maxThrust, but rather some function (linear, square?) with thrust 
            going from max to lower thrust levels - this way, the mistake in computing the time sample to stop decelerating will result in
            smaller mistake in thrust output (since only the smaller area of thrust*time will be cut)
            - if we notice low starting altitude (object would fall before we can react), add phase of preemptive levitation
            - parametrization of landing thrust (higher mass, higher ratio of neutral thrust, higher aimedAltitude, higher ratio again)
            DONE - correction of estimations of start velocity and altitude throughout the free fall stage
            - recursive decisions, multiple deceleration phases
            '''
            # gather first few samples for estimation of start velocity and altitude
            if (self.ts == 0):
                self.timeStep = time
            elif (self.ts == 1):
                self.timeStep = time-self.timeStep
                temp = self.h_Altitudes
                self.h_Altitudes = numpy.zeros(int(self.ttl / self.timeStep))
                self.h_Altitudes[0] = temp[0]
                self.h_Altitudes[1] = temp[1]
                # prepare velocity computation arrays
                reactionTime = 0.2
                self.roughEstimationSampleNum = int(reactionTime / self.timeStep) # number of velocities we can afford to compute to estimate start altitude and velocity
                self.averageVelocities = numpy.zeros(int(self.ttl / self.timeStep) - self.roughEstimationSampleNum + 1) # velocities
            else:
                velocityStep = self.ts * self.timeStep * self.g # substract from velocity based on which ts it was taken in (_v = v0 + g*t, we seek v0)
                self.averageVelocities[self.ts - self.roughEstimationSampleNum] = (self.h_Altitudes[self.ts-self.roughEstimationSampleNum] - self.currentAltitude) / (self.timeStep * self.roughEstimationSampleNum) - velocityStep
            
            
            # make rough estimate based on first few samples (to be able to react soon enough)
            if (self.roughEstimationSampleNum is not None and self.ts == self.roughEstimationSampleNum * 2):
                self.makeDecision()
            # dynamic decision re-evaluation
            if (self.tsReevaluate is not None and self.ts == self.tsReevaluate):
                self.makeDecision()
            
            # deceleration
            if (self.tsDecel is not None and self.tsDecel <= self.ts):
                self.currentThrust = self.objectMaxThrust
            if (self.tsStopDecel is not None and self.tsStopDecel <= self.ts):
                self.currentThrust = self.objectNeutralThrust - self.objectNeutralThrust / 15

        # end
        self.ts += 1

    def makeDecision(self):
        samplesCollected = self.ts - self.roughEstimationSampleNum
        # estimate start velocity
        savffi = - numpy.average(self.averageVelocities[0:samplesCollected:1]) # statistically average velocity
        self.sVel = savffi - (self.roughEstimationSampleNum*self.timeStep)/2 * self.g
        # estimate start altitude
        if (self.sAlt is None):
            adjustedAltitudes = numpy.zeros(samplesCollected) # altitudes, moved back up (back in time)
            for i in range(samplesCollected):
                tempAlt = self.h_Altitudes[i]
                if (i != 0):
                    averageVelocityFromStartAltTo_tempAlt_ = (i * self.timeStep * self.g + 2 * self.sVel) / 2 # (v0 + vk) / 2
                    tempAlt -= averageVelocityFromStartAltTo_tempAlt_ * i * self.timeStep
                adjustedAltitudes[i] = tempAlt
            self.sAlt = numpy.average(adjustedAltitudes)
        
        # decide deceleration start sample
        # h: height if v0 = 0 and aimedAltitude = 0
        h = self.sAlt + self.sVel**2 / 2 / self.g
        aimedAltitude = h / 20
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

        # reevaluate
        if (self.tsDecel - self.ts > 50):
            reevBasedOnReevStep = int(self.ts + self.tsReevStep)
            reevBasedOnTsDecel = int((self.tsDecel + self.ts) / 2)
            if (reevBasedOnReevStep < reevBasedOnTsDecel):
                self.tsReevaluate = reevBasedOnReevStep
            else:
                self.tsReevaluate = reevBasedOnTsDecel
            self.tsReevStep /= 2


    @property
    def heightFiltered(self):
        '''
        Tukaj vrnete trenutno trenutno višino ocenjeno z filtri.
        To omogoča kasnejši izris, vrnete vašo interno spremenljivko.
        '''
        return self.currentAltitude

    @property
    def thrust(self):
        '''
        Tukaj vrnete relativno količino moči motorjev, vrednost med 0 in 1.
        '''
        return self.currentThrust
    


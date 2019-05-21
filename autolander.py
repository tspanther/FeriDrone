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
    def __init__(self, objectMass, objectMaxThrust, objectNeutralThrust, timeStep, ttl, snr_dB):
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
        
        self.timeStep = timeStep
        self.ttl = ttl # time to land

        self.ts = 0 # time sample (index)
        self.strategy = 'Niko'

        # Kalman Filter
        self.snr_dB = snr_dB
        self.kFilter = None

        # history of (filtered altitudes)
        self.h_Altitudes = numpy.zeros(int(self.ttl / self.timeStep))
        # estimated start velocity and altitude
        self.sAlt = None
        self.sVel = None

        # prepare velocity computation arrays
        reactionTime = 0.2
        self.velocitiesCompute = int(reactionTime / self.timeStep) # number of velocities we can afford to compute to estimate start velocity
        self.averageVelocities = numpy.zeros(self.velocitiesCompute) # velocities on first few (overlapping) time intervals

        # properties for deceleration decision
        self.tsDecel = None # time sample to start decelerating
        self.tsStopDecel = None # time sample to stop decelerating
        self.stopDecel = 0 # deceleration complete when this reaches 3; increment when altitudes_MA > altitudes_HistMA during decel phase
        self.altitudes_HistMA = None # moving average of altitudes <self.velocitiesCompute> samples back
        self.altitudes_MA = None # moving average of altitudes


    def addHeightMeasurement(self, time, height):
        '''
        Prejemek posamezne meritve.
        time - float
            čas zajema, v s

        height - float
            izmerjena višina v m
        '''
        # filter
        if (self.kFilter is None):
            SNR = numpy.power(10, (self.snr_dB/10))
            stdDevi = 2 * height / SNR
            process_variance = 3*10e-4
            self.kFilter = KalmanFilter(process_variance, stdDevi)

        self.kFilter.input_latest_noisy_measurement(height)
        self.h_Altitudes[self.ts] = self.kFilter.get_latest_estimated_measurement()
        self.currentAltitude = self.h_Altitudes[self.ts]

        # uncomment to ignore filter
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
            - better filter? Kalman with parametrization? Current one works worse as if there wasn't any :D
            - * estimation of signal to noise ratio based on measurement variance *
            IMPROVEMENTS:
            - combination of calculated stopDecel time and measurements
            - if we notice low starting altitude (object would fall before we can react), add phase of preemptive levitation
            - parametrization of landing thrust (higher mass, higher ratio of neutral thrust, higher aimedAltitude, higher ratio again)
            LARGE FEATURES:
            - * refactor so that constant and known time intervals of measurements taken are no longer needed *
            - correction of estimations of start velocity and altitude throughout the free fall stage
            - recursive decisions, multiple deceleration phases
            '''
            # gather first few samples for estimation of start velocity and altitude
            if (self.ts < self.velocitiesCompute * 2 and self.ts >= self.velocitiesCompute):
                velocityStep = self.ts * self.timeStep * self.g # substract from velocity based on which ts it was taken in
                self.averageVelocities[self.ts - self.velocitiesCompute] = (self.h_Altitudes[self.ts-self.velocitiesCompute] - self.currentAltitude) / (self.timeStep * self.velocitiesCompute) - velocityStep
            # decide deceleration start time sample
            elif (self.ts == self.velocitiesCompute * 2):
                # estimate start velocity
                savffi = - numpy.average(self.averageVelocities) # statistically average velocity on first few intervals
                self.sVel = savffi - (self.velocitiesCompute*self.timeStep)/2 * self.g
                # estimate start altitude
                adjustedAltitudes = numpy.zeros(self.velocitiesCompute)
                for i in range(self.velocitiesCompute):
                    tempAlt = self.h_Altitudes[i]
                    averageVelocityFromStartAltTo_tempAlt_ = self.sVel + self.g * i * self.timeStep/2
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

            # deceleration
            if (self.tsDecel is not None and self.tsDecel <= self.ts):
                self.currentThrust = self.objectMaxThrust
            if (self.tsStopDecel is not None and self.tsStopDecel <= self.ts):
                self.currentThrust = self.objectNeutralThrust - self.objectNeutralThrust / 15

            # implementation of decision on when to stop decelerating 
            # done by sampling altitudes and estimating velocity based on moving averages;
            #     
            # elif (self.tsDecel is not None and self.ts >= self.tsDecel):
            #     # Moving average of altitudes
            #     if (self.altitudes_MA is None or self.altitudes_HistMA is None):
            #         # Init
            #         self.altitudes_MA = numpy.average(self.h_Altitudes[self.ts-self.velocitiesCompute:self.ts-1:1])
            #         if (self.ts-self.velocitiesCompute*2 >= 0):
            #             self.altitudes_HistMA = numpy.average(self.h_Altitudes[self.ts-self.velocitiesCompute*2:self.ts-self.velocitiesCompute-1:1])
            #         else:
            #             partialHistory = self.h_Altitudes[0:self.ts-1:1]
            #             fillerL = self.velocitiesCompute*2 - self.ts
            #             filler = numpy.multiply(numpy.ones(fillerL), self.h_Altitudes[0])
            #             self.altitudes_HistMA = numpy.average(numpy.concatenate([filler, partialHistory]))
            #     else:
            #         # Update
            #         self.altitudes_MA -= self.h_Altitudes[self.ts-self.velocitiesCompute]/self.velocitiesCompute
            #         self.altitudes_MA += self.h_Altitudes[self.ts]/self.velocitiesCompute
            #         if (self.ts - self.velocitiesCompute * 2 >= 0):
            #             self.altitudes_HistMA -= self.h_Altitudes[self.ts-self.velocitiesCompute*2]/self.velocitiesCompute
            #         else:
            #             self.altitudes_HistMA -= self.h_Altitudes[0]/self.velocitiesCompute
            #         self.altitudes_HistMA += self.h_Altitudes[self.ts-self.velocitiesCompute]/self.velocitiesCompute
                
            #     # Thrust
            #     self.currentThrust = self.objectNeutralThrust - self.objectNeutralThrust / 10
            #     if (self.stopDecel < 3):
            #         if (self.altitudes_MA < self.altitudes_HistMA):
            #             self.stopDecel -= 1
            #             self.currentThrust = self.objectMaxThrust # Full decel
            #         else:
            #             self.stopDecel += 1

        # end
        self.ts += 1

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
    


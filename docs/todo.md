# Vaje drone

vs / srds / mm
	senzorji/komponente (todo)
	features
        - IMU
            X hardware: gyro, accel, magnet
            - kalibracija magnetometra
            X pretvorba v roll,pitch,yaw
            X posiljanje podatkov; queue, new task, sinhronizacija
            A kompresija
            - prejemanje podatkov pc side
                - C++ comport / wifi / iz datoteke
                A dekompresija
        - trilateracija
            - hardware: mikrofon
            N zaznava pulzov (signal proc)
            - racunanje polozaja (trilateracija C)
            - posiljanje dodat v queue
        N upravljanje / autolanding
            - hardware: ToF
            - dekodiranje PWM signala
            - generiranje novega signala;
                - isto vrnit
                - implementirat autolander v C
                    - specifikacije (neutral thrust, full thrust, weight)
                    - podatki iz senzorja -> AL domain -> decision -> PWM gen -> debug with LA
            - hardware: i2c med f3 in f4
        - posiljanje prek WiFi (namesto USB)
            - hardware: Wifi
            - pc-side sprejem
	A izdelava sheme
        - ko imamo vse splanirano - popravit, tako, kot je dejansko

rg
	- init app; framework
		S loadanje obj
			- drone, virtualni svet, trilateracija oddajniki, vektorji
		- 3rd person kamera (premikanje, rotiranje..)
		+ teksture, sencenje
	- UI / logic
		- kontrola kamer
		- loadanje/izbira scene
	- lokacija, rotacija drona
		- filtriranje trilateracija / IMU podatkov
		- updatanje M
			- dron (translacija - trilateracija, rotacija - IMU)
			- vektorji lastnosti -||-
	- 1st person
		- updatanje PV glede na trilateracijo in IMU
			- camPos; trilateracija
			- upVec, lookAt; IMU
	- trajektorija poti
		- izrisovanje pravokotnikov/kvadrov pod koordinatami drona
	+ minigame
		- upravljanje
			- dekodiranje signala s controllerja
			- fizikalni model
		- metanje bomb

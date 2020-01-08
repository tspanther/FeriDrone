# Vaje drone

```
vs / srds / mm
    - IMU
        X hardware: gyro, accel, magnet
            N kalibracija magnetometra
        X pretvorba v roll,pitch,yaw
        X posiljanje podatkov; queue, new task, sinhronizacija
        A kompresija
    - trilateracija
        - hardware: mikrofon
        N zaznava pulzov (signal proc)
        - racunanje polozaja (trilateracija C)
        X posiljanje dodat v queue
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
	A izdelava sheme
        - ko imamo vse splanirano - popravit, tako, kot je dejansko

rg
	- teksture
        A razdelit sceno v blenderju na vec obj-jev; vsak objekt, ki ima drugo teksturo v svoj obj (ce ima vec objektov isto teksturo, so lahko vsi skupaj) => debla v en obj, krosnje v en obj, tla v en obj
        S nalaganje vseh objektov scene
	- sencenje
        - Phongov model
        + sence
            - https://learnopengl.com/Advanced-Lighting/Shadows/Shadow-Mapping
	- real-time mode
        - WiFi vmesnik
        A dekompresija / dekodiranje
		N filtriranje trilateracija / IMU podatkov
	N 1st person
		- popravit nagib; lookAt, upVec
	S trajektorija poti
		- izrisovanje platojev (nagibov drona);; pravokotnik v izhodiscu na ravnini XZ => ista PVM kot za dron
    N refaktorirat kodo
    S menu
        - App modes:
            - Real-time renderiranje iz toka podatkov
            - animacija iz datoteke .flight (shranjen not podatkov z drona)
            - (Igra)
        - Drugo:
            - obkljukanje/odkljukanje atributov za prikaz
	+ minigame
		- fizikalni model
            - https://github.com/bulletphysics
        - dekodiranje PWM signala s controllerja
            - IPC s Python skripto
        ...
        - dodatni ukazi zraven usmerjanja (streljanje, metanje bomb) - kodirani v preostalih kanalih
```





aljaz shema;
    - receiver
        - f3-f4;; GND
        - receiver-f4;; PE11
        - receiver-f3;; GND
        - receiver-f3;; 5V
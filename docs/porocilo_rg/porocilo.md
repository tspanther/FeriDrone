# Porocilo: Vizualizacija drona v 3D sceni

## Izbira tehnologije

C++, plain OpenGL

- najbolje poznana
- predpripravljeno veliko kode iz vaj (manipulacija objektov in kamere, nalaganje objektov, tekstur)
- zadostuje vsem ciljem, ki smo si jih zadali

## Opravljeno delo

### Objekti, kamera

- nalaganje scene in drona
- 3rd person kamera
    - premikanje po oseh x, z v smeri lookAt (hoja v smeri gledanja)
    - seemless rotiranje (premik miske v levo / desno -> yaw, gor / dol -> pitch)
- 1st person kamera
    - sledenje dronu

![1st person](drone1stP.PNG)

### Dronova pot in prikaz atributov

- nalaganje dronovih pozicij in rotacij iz datoteke
- korakanje skozi pozicije in rotacija

- prikaz atributov; puscice
    - lookAt
    - upVec
    - hitrost

![atributi](drone3rdP.PNG)

- trajektorija

![trajektorija](drone3rdP2.PNG)

``` C++
std::vector<float> novaTrikotnika = {
    previousPosition.x, previousPosition.y, previousPosition.z
    currentPosition.x, currentPosition.y, currentPosition.z
    previousPosition.x, 0.0, previousPosition.z
    currentPosition.x, currentPosition.y, currentPosition.z
    previousPosition.x, 0.0, previousPosition.z
    currentPosition.x, 0.0, currentPosition.z
};
```

### Modifikacija prikaza poti : Todo: Niko ... napiši nekaj za horizontalno trajektorijo(nagibe).



### Teksturiranje scene in drona

Objekte teksturiramo v C++, vendar objekti samo nimajo podanih koordinat tekstur -> vedno je koordinata teksture v vsakem oglišču (u, v) = (0, 0).

- Testno sceno smo razdelili na več delov in teksturirali dele, ki imajo enako teksturo. Pri nalaganju vsak objekt z pripadajočo sceno naložimo ločeno

### Izboljšava 1st person kamere : TODO: Niko ... modify

Kamera trenutno potuje za dronom, se pa ne rotira skupaj z njim.

``` C++
camPos = dronePos + glm::vec3(0, 0, -3); // ne povsem realisticen - tocno za dronom samo ko dron ni nagnjen
lookAt = glm::vec3(0.0, 0.0, -1.0); // staticen
camUp = glm::vec3(0.0, 1.0, 0.0); // staticen
```

zelimo doseci:

``` C++
camPos = dronePos - 3.0f * glm::normalize(lookAt); // realisticno - za dronom ne glede na nagib in usmerjenost
lookAt = todo(pitch, yaw);
camUp = todo(roll);
```

### Senčenje

- Uporabili smo kodo iz vaje 5.

#### Sence ?

https://learnopengl.com/Advanced-Lighting/Shadows/Shadow-Mapping

### Real-time branje podatkov iz toka, prikazovanje ?

- Wifi vmesnik
- filtriranje podatkov (low pass)

### Opcijsko: Igra ; ?

- fizikalni model
    - https://github.com/bulletphysics
- dekodiranje PWM signala s controllerja
    - IPC s Python skripto
...
- dodatni ukazi zraven usmerjanja (streljanje, metanje bomb) - kodirani v preostalih kanalih

### Menu - TODO: Štefan ; Low-priority

Uporabniski vmesnik za izbiro nacina delovanja aplikacije (preprost) 

App modes:
- Real-time renderiranje iz toka podatkov
- animacija iz datoteke .flight (shranjen not podatkov z drona)
- (Igra)

Drugo:
- obkljukanje/odkljukanje atributov za prikaz

### Razdelitev dela

Niko:
- Implementacija FPS (aka. drone view)
- Izdelava animacije
- Puščisce smeri gibanja
- Izdelava prikaza nagiba
- Integracija senčenja
- Razno ostalo delo

Štefan:
- Implementacija 3PS
- Izdelava osnovne trajektorije
- OBJ nalaganje kompleksnejših objektov
- Integracija tekstur

Aljaž:
- Izdelava modela za prikaz (drone)
- Teksturiranje scene

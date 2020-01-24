# Vizualizacija dronovega leta, trajektorije in atributov v 3D prostoru

Izdelali smo aplikacijo, ki prikazuje dronov let v 3D prostoru, ob enem pa vizualno predstavi atribute drona v dolocenem trenutku.

Namen aplikacije je analiza leta in ugotavljanje morebitnih nepravilnosti v delovanju drona, ko so tresenje, zanasanje in slaba odzivnost na ukaze.

Izbrana tehnologija, v kateri smo aplikacijo izdelali je C++, vmesnik za komunikacijo z graficnim cevovodom pa OpenGL. Ceprav tak pristop prinese pocasnejsi razvoj zaradi nizjega nivoja abstrakcije, je bil prikaz atributov, povezanih z dronom, preprost, natancen, nedvoumen in razsirljiv (uporaba enakih / rahlo spremenjenih matrik za transformacijo drona za transformacije atributov). Prav tako smo vse to lahko dosegli skoraj povsem brez uporabe knjiznic.

> knjiznice: [glm](https://glm.g-truc.net/0.9.9/index.html) in [OpenGL v razvojnem okolju Qt](https://doc.qt.io/qt-5/qtopengl-index.html)

## Osnovni gradniki scene

![osnovna scena](./docs_material/vizualizacija/osnovna_scena.png)

### Object

`Object` opisujejo naslednji podatki;

- seznam koordinat oglisc, normal v ogliscih in koordinat teksture
- tekstura
- odmik od prvotnega polozaja v prostoru
- eulerjevi koti rotacije od zacetnega nagiba
- lastnosti materiala za uporabo pri sencenju

Izris objekta je mogoc z metodo `draw`.

#### Drone

`Drone` razsirja `Objekt` z naslednjimi lastnostmi;

- seznam koordinat oglisc, normal v ogliscih in koordinat teksture za naslednje 3D gradnike - atribute
    - vektor (puscica) hitrosti
    - vektor (puscica) usmerjenosti drona
    - vektor (puscica) v smeri pravokotno navzgor od drona
    - trajektorije, razdeljene na vec podseznamov
- vrsta predhodnjih polozajev in nagibov (za izris trajektorije in vektorja hitrosti/gibanja)
- `Camera` - 1st Person

`Drone` kot premicen objekt v sceni uvede metodi `moveTo` in `tiltTo`, ki offline in real-time premikanje.

### Camera

`Camera` opisujejo podatki;

- pozicija v globalnih koordinatah
- vektor smeri gledanja
- vektor pokoncnosti
- eulerjevi koti nagibov od zacetnega nagiba

### Light

`Light` opisujejo podatki;

- pozicija v globalnih koordinatah
- barva svetlobe

## Kamera

Kamera lahko deluje v dveh nacinih, med katerima je mogoce preklapljati brez omejitev;

### Prva oseba

Pozicija; za in malo nad dronom
Usmerjenost; enaka kot dron

![1stP kamera](./docs_material/vizualizacija/1stP.png)

### Tretja oseba

Pozicija; premikanje s smernimi tipkami po povrsju
Usmerjenost; premikanje z misko

![3rdP kamera](./docs_material/vizualizacija/3rdP.png)

## Atributi

Vizualizacijo leta obogatimo z naslednjimi vizualnimi atributi;

### Vektorji (puscice) hitrosti in nagiba

- smer gibanja (hitrost) - roza puscica
- usmerjenost (lookAt) - modra puscica
- navpicno navzgor (upVec) - zelena puscica

![vektorji atributi](./docs_material/vizualizacija/arrows.png)

### Trajektorija

- zavesa; pod krivuljo leta do tal
- navpicne in vodoravne osi nagibov skozi cas

![trajektorija atributi](./docs_material/vizualizacija/trajektorija_1.png)

![trajektorija atributi](./docs_material/vizualizacija/trajektorija_2.png)

### Moc delovanja motorjev

todo; obarvati modra-zelena-rumena-rdeca

## Nacini delovanja aplikacije

V aplikaciji lahko prikazujemo podatke na sledece nacine;

### Real-time

todo (small thing), bomo posneli video

### Prikaz leta iz datoteke

Iz datoteke csv beremo zaporedje vrstic - dronovih koordinat, nagibov (todo in moci delovanja motorjev).

### Igra

todo

### Za asistenta: razdelitev dela

Niko:
- Implementacija FPS (aka. drone view)
- Puščisce smeri gibanja
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
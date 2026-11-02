# Lesung BODEN UND TIEFE (Bahnsteig-Form)

Quellen der Form: nur `bild1.png` und `bild2.png`. Pixel: Originalbild 640x480. Einheiten: Spiel, 2750 = 1 Mensch.
Raumdaten (Kamera, Kollision) dienten nur als Maßstab. Sicherheit: sicher / wahrscheinlich / offen.

## Kurzfazit

- Bild 1: **ein** sicherer Bodenkontakt, die rechte Stütze bei (398,374). Vorn drei wahrscheinliche Bodenstellen (K2, K3, K4). Ein vierter Kontakt ist nicht erkennbar.
- Der Rumpf liegt hinten nicht auf: durch L1 und L2 ist Boden unter ihm sichtbar, Bauchkante 790-1080 Einheiten über Grund (wahrscheinlich). Vorn reichen die Massen bis nahe an den Boden (wahrscheinlich).
- Größe aus Bild 1: 3220 seitlich, 2480 in der Tiefe, 2590-2790 hoch. Das sind 1,17 / 0,90 / 0,94-1,01 Leon.
- Bild 2: kein sicherer Kontakt. Höhe 2960-3050 (wahrscheinlich), seitlich rund 2950 mit Bogen links.
- Ein weicher Bodenschatten in Bild 1 ist kreisförmig, Mitte (280,416), Radius rund 1112. Die Stütze rechts steht außerhalb.

## 1 Kalibrierung

| Größe | Bild 1 | Bild 2 |
|---|---|---|
| Raum | Zugbahnsteig, ähnelt ROOM5080 (Zug, Gitterturm, gestreiftes Dach), kein Cut passt; Kamera nur aus dem Bild (offen) | ROOM5090 Cut 11 (wahrscheinlich) |
| Horizont | y=243 ±8 | y=142 |
| Fluchtpunkte | Zug (482,243), rechts (104,250), Rückwand (-812,230) | Gang gemessen (162,142), Cut 11 (161,143) |
| Brennweite (640er Pixel) | 440 ±15 (Zug/Rückwand 428, 45-Grad-Paar 455) | 2·208·s, s=1,00-1,06 (Manhattan-Fit 440) |
| Kamerahöhe | 1983 ±139 (aus Leon) | 3251 (RDT) |
| Mensch | Leon: Kopf y=197, Fuß y=362, 165 px | Frau: Kopf (170,187), Fuß nicht lesbar |

- Methodenprobe: ROOM5080 Cut 3 gemessen (530,171), wahr (534,173).
- Bodenprobe Bild 2: helle Leiste (459,398)-(537,465) liegt mit dieser Kamera auf z=-21954 bis -21991, Kollisionszelle z=-21990.
- Probe Mensch Bild 2: 2750 hoch mit Kopf y=187 ergibt Fuß y=408; 7,5 Kopflängen (30 px) ergeben y=412.
- Bild 1: Senkrechte laufen auf (317,4261) zu, der Horizont spräche für waagerechte Kamera. Vermutlich leicht schräg abgefilmt. Wirkung auf Höhen unter 5 %.
- Grobe Gegenprobe Bild 1: Zugmaße aus ROOM5080 Cut 3 (Dach 3450, Rot-Unterkante 1258) ergeben Kamerahöhe rund 1760, also 11 % unter dem Wert aus Leon. Absolutmaße aus Bild 1 können so viel zu groß sein, Verhältnisse zu Leon nicht.
- Projektion Bild 1 für Modellproben: `x=320+440*X/Z`, `y=243+440*(1983-h)/Z`.

## 2 Bild 1

### 2.1 Bodenkontakte

| Id | Teil | Pixel (x,y) | Grundriss X / Z (1 Sigma) | Sicherheit | Beleg |
|---|---|---|---|---|---|
| K1 | Stütze rechts, Fuß | (398,374) | 1181 ±161 / 6656 ±286 | sicher | Dunkle Säule endet mit Fußverbreiterung x=380-424 auf hellem Boden, Unterkante y=375 bei x=394-402 (G<120) |
| K2 | Klumpen links, Unterkante | (232,433) | -915 ±154 / 4589 ±274 | wahrscheinlich | Unterkante nur bis y=437 lesbar, dort beginnt das Wasserzeichen; links davon weiche Schattenzone |
| K3 | Heller Block, Unterkante | (312,452) | -75 ±134 / 4175 ±236 | wahrscheinlich | Hellere Fläche (288-330,405-452) mit Querstreifen, darunter weicher Übergang zum Boden |
| K4 | Masse vorn, Außenkante | (352,432) | 336 ±135 / 4613 ±260 | wahrscheinlich | Harte Kante (372,412)-(335,448), Übergang 6,5 px; liegt auf oder knapp über dem Boden |
| K5 | Glied A, Fuß verdeckt | (258,395) | -804 ±268 / 5733 ±858 | offen | Rechte Kante von Glied A reicht bis mindestens y=368; Fuß selbst hinter Klumpen und Masse nicht erkennbar |

Leon im Grundriss: -3216 ±202 / 7334 ±346.

Abstände: K1-K2 2944, K1-K3 2781, K1-K4 2211, K2-K3 936, K3-K4 601.
Berührt ein vorderes Teil den Boden nicht, liegt es noch näher an der Kamera; die Z-Werte von K2-K4 sind dann Obergrenzen.

### 2.2 Freier Boden unter der Figur

- L1 (345-378,329-368): Boden sichtbar zwischen Glied B und Stütze. sicher.
- L2 (283-325,327-368): Boden sichtbar zwischen Glied A und Glied B. sicher.
- Beide reichen im Grundriss von Z rund 7000 bis über 10000. Auf diesen Sehstrahlen steht nichts am Boden.

### 2.3 Schatten

- Weiche Zone rechts (345-378,372-405), links (197-225,398-432), unten (295-335, bis y=465). Helligkeit dort 35-60 % des Bodens, Körper rund 15 %. wahrscheinlich.
- Kantenbreite 10-90 %: Schattenränder 17-26 px, Modellkanten 4-7 px (Stütze 4,7, Klumpen 4,3, Leon 3,7).
- Kreisfit im Grundriss: Mitte -457 / 5043, R 1112, Streuung 129, 26 Randpunkte. Bild: x 183-377, y 385-465.
- Leons Schatten (103-145,355-368) dagegen rund 350-420 Radius. Der Figurschatten ist also rund dreimal so groß.
- Kein gerichteter Schlagschatten: unter der Stütze ist der Boden ab y=380 hell.
- Ob die Kreismitte der Modellursprung ist: offen. Sie liegt 2300 von K1 und 640-950 von K2/K3 entfernt, also nicht mittig zwischen den Kontakten.
- Masse vorn ist kein Schatten: Helligkeit wie Körper, Kante (372,412)-(335,448) nur 6,5 px breit. wahrscheinlich.

### 2.4 Verdeckung und Tiefenfolge

| Id | Pixel | Aussage | Sicherheit |
|---|---|---|---|
| V1 | (285,368)-(325,345) | Untere Kante von L2: Figurteil liegt vor dem Boden | sicher |
| V2 | (225,345)-(240,341)-(250,350)-(252,365) | Heller Rand des Klumpens läuft ungebrochen vor Glied A | wahrscheinlich |
| V3 | (300,312)-(325,345) | Glied B liegt vor dem Boden, rechte Kante von L2 | sicher |
| V4 | (380,297)-(416,297) | Kegel endet mit harter Unterkante über der Stütze | wahrscheinlich |
| V5 | (318,348)-(340,358) | Heller Querbalken zwischen Glied B und Masse, Reihenfolge nicht erkennbar | offen |
| V6 | (287,415)-(278,465) | Dunkler Strich trennt Block und Rotfeld, Reihenfolge nicht erkennbar | offen |

Nah nach fern: Masse, Block, Klumpen (Z 4175-4615), Glied A und B, Rumpf, Stütze rechts mit Kegel (Z 6658), Leon (Z 7335). Grundlage: Bodenzeilen, dazu V1-V4.

Offene Zuordnung: Glied B, Querbalken, Masse und Block liegen auf einer Linie (330,300)-(308,450), könnten also ein Bein sein. Glied A und Klumpen ebenso. Beides wahrscheinlich, nicht belegt.

### 2.5 Maße gegen Leon

| Maß | Einheiten | Leonhöhen | Sicherheit |
|---|---|---|---|
| seitliche Ausdehnung x=197 bis 430 | 3219 | 1,17 | wahrscheinlich |
| Tiefe K3 bis K1 | 2481 | 0,90 | wahrscheinlich |
| Höhe obere Kante y=190 | 2590-2791 | 0,94-1,01 | wahrscheinlich |
| Kegelspitze y=277 über K1 | 1468 | 0,53 | sicher |
| Stütze Dicke 33 px | 500 | 0,18 | sicher |
| Stützenfuß x=380-424 | 670 | 0,24 | wahrscheinlich |
| Bauchkante y=322 | 787-1077 | 0,29-0,39 | wahrscheinlich |

Die Spanne der Höhen kommt aus der unbekannten Tiefe des Teils (Bodenzeile 373 bis 416). Die lange Achse K2-K1 steht rund 45 Grad rechts der Blickachse, K3-K1 rund 27 Grad. Offen, weil die Enden der Figur nicht belegt sind.

### 2.6 Draufsicht

Spalte = 250, Zeile = 500 Einheiten. Ziffern = K1-K5, L = Leon, o = Schattenkreis, + = Mitte. Kamera unterhalb. Grafik mit Ellipsen: `lesung_tiefe_bild1.png`.

```
  8000 |
  7500 |   L
  7000 |
  6500 |                     1
  6000 |           oooooooo
  5500 |          oo 5    oo
  5000 |          o   +   oo
  4500 |          oo2    4o
  4000 |           ooooo3o
  3500 |
       +---------------------------
        -4000   -2000   0       2000
```

## 3 Bild 2

### 3.1 Bodenkontakte

| Id | Teil | Pixel (x,y) | Grundriss X / Z (1 Sigma) | Sicherheit | Beleg |
|---|---|---|---|---|---|
| F1 | Unterkante dunkle Masse rechts | (370,368) | 728 ±303 / 5692 ±261 | wahrscheinlich | Rotkanal springt zwischen y=355 und y=380 von rund 95 auf 195 (x=340-400); Grenze Körper oder Schatten gegen Boden, kein Kontaktpunkt |
| F2 | Dunkelband rechts, Unterende | (398,378) | 1092 ±133 / 5414 ±244 | offen | Dunkle Spalte x=385-405 (R 68-111) von y=290 bis y=375, geht unten in den Bodenstreifen über; Bein oder Schatten nicht unterscheidbar |
| F3 | Feinstab, Spitze | (275,398) | -584 ±84 / 4941 ±154 | offen | Dünner heller Strich (292,362)-(275,398) endet über dem hellen Balken; ob er den Boden berührt, ist nicht erkennbar |
| F4 | Kuppel unten, Unterkante | (245,450) | -807 ±202 / 3986 ±204 | offen | Dunkle Rundform (212-280,385-445+), nicht in Cut 11; Unterkante vom Wasserzeichen verdeckt; Zugehörigkeit zur Figur nicht belegt |

Frau: Füße nicht lesbar. Rechnung über Kopfpunkt: Höhe 2600 ergibt y=353, 2680 ergibt y=380, 2750 ergibt y=408. Grundriss bei y=400: -1682 ±151 / 4883 ±409. offen.

Sichere Bodenaussage nur als Grenze: für x=340-400 ist ab y=380 freier Boden sichtbar (Z unter rund 5500). Dort steht nichts.

Kein Figurteil:
- Dunkler Keil (420-470,350-420) folgt der Bodenkante z=-21990. wahrscheinlich.
- Heller Balken (278-335,405-415) läuft quer zur Gangachse bei x rund 3150. wahrscheinlich.

### 3.2 Schatten

Weicher Übergang im Rotkanal unter der Masse (y=355-380), aber nicht als Kreis abgrenzbar. offen.

### 3.3 Verdeckung

| Id | Pixel | Aussage | Sicherheit |
|---|---|---|---|
| W1 | (335,322)-(262,385) | Gliederband liegt auf der Unterkante der Masse, darunter folgt Boden | wahrscheinlich |
| W2 | (207,228) | Bogen links endet an der Schulter der Frau; davor oder dahinter nicht erkennbar | offen |
| W4 | (361,302) | Heller Fleck (354-368,295-310) liegt auf dunklem Grund | wahrscheinlich |
| W5 | (212,400)-(279,430) | Dunkle Fläche liegt dort, wo die Füße der Frau sein müssten; Kuppel oder Hintergrund | offen |

Kuppel (212-280,385-445): fehlt in Cut 11, ist aber kaum dunkler als die Fläche links daneben (R 65-95, Körper 40-48). Figurteil oder Hintergrund des Prototyps: offen.

### 3.4 Maße gegen die Frau

| Maß | Einheiten | Menschhöhen | Sicherheit |
|---|---|---|---|
| Höhe Bogen y=160 | 2963-3052 | 1,08-1,11 | wahrscheinlich |
| seitlich x=207 bis 425 (Kamera-Z rund 5800) | 2950 | 1,07 | wahrscheinlich |
| seitlich ohne Bogen links, x=262 bis 425 | 2200 | 0,80 | wahrscheinlich |
| Unterkante (345,350) über Grund, Abstand 4500-6000 | 912-132 | 0,05-0,33 | offen |

Die Höhe ist wenig empfindlich gegen die Tiefe, weil die Oberkante nahe am Horizont liegt.

### 3.5 Draufsicht

Spalte = 250, Zeile = 500 Einheiten. Ziffern = F1-F4, W = Frau, # = Wand, = = Bodenkante z=-21990. Grafik mit Ellipsen: `lesung_tiefe_bild2.png`.

```
  9000 |                 #######
  8500 |            ######=
  8000 |       ######     ==
  7500 | #######           ==
  7000 |##                  =
  6500 |#                   ==
  6000 |##                   ==
  5500 | ##                12 =
  5000 |  #      W    3        =
  4500 |  ##                   ==
  4000 |   ##        4          ==
  3500 |    ##                   =
       +---------------------------
        -4000   -2000   0       2000
```

## 4 Beide Bilder im Vergleich

- Die Figur steht in beiden Bildern in gleichem Kameraabstand (Z rund 4000-6700) bei gleicher Brennweite. Bildbreite 233 px gegen 218 px, das passt zu einem Modell.
- Höhe: Bild 1 0,94-1,01 Mensch, Bild 2 1,08-1,11. Differenz rund 10 %. Haltung oder Maßfehler nicht trennbar. Arbeitswert 2600-3000.
- Der Abstand zur Bezugsperson ist verschieden: Leon rund 3600 von der Schattenmitte, Frau rund 1250 von F4 und 1100 von F3 (Lage der Frau offen).

## 5 Nicht erkennbar

- Vierter Bodenkontakt in Bild 1
- Sicherer Bodenkontakt in Bild 2
- Füße der Frau
- Natur der weißen Lappen (160-228,330-388) in Bild 2
- Natur des Rotfelds (252-283,425-478) in Bild 1
- Blickrichtung der Figur
- Obere Kante zwischen x=250 und x=320 in Bild 1 (dunkler Pfeiler dahinter)

Ein vierter Kontakt in Bild 1 kann nur auf verdecktem Boden liegen, also hinter Glied A, Klumpen oder Masse (Grundriss X -1100 bis 400, Z 4700 bis 7000). Das Bild belegt ihn nicht.

## 6 Prüfpunkte für ein Modell

Bild 1, Kamera wie in Abschnitt 1, Figur im Grundriss wie 2.1:

| Punkt | Soll im Bild |
|---|---|
| K1 auf dem Boden | (398,374) |
| Kegelspitze über K1, h=1468 | (400,277) |
| rechter Rand der Figur | x=430 bei y=292 |
| obere Kante | y=190 bis 205 bei x=300-340 |
| Boden durch L1 und L2 sichtbar | Flächen wie in 2.2 |

Bild 2, Kamera ROOM5090 Cut 11, Bildmaßstab 1,03 (`x=(2*sx-320)*1.03+320`, `y=(2*sy-240)*1.03+240`):

| Punkt | Soll im Bild |
|---|---|
| Oberkante Bogen | y=160 bei x=320 |
| Figur seitlich | x=207 bis 425 |
| Unterkante gegen freien Boden | y=368 ±8 bei x=340-400, darunter frei |

## Dateien

- `lesung_tiefe.json`: alle Zahlen, Grundrisse mit Kovarianz, Verdeckungen.
- `lesung_tiefe_bild1.png`, `lesung_tiefe_bild2.png`: Überlagerung links, Grundriss rechts.
- `lesehilfen/tiefe_*`: Ausschnitte, Kanalbilder, Falschfarben.

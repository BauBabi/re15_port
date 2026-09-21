# Runde 19c — ROOM10D0 Cut 7: der Saum, den der Nutzer selbst markiert hat

Nutzer, 2026-09-21: *„Na, der Stuhl ist doch fast korrekt erkannt, aber es fehlt noch minimal
was von den Rahmen"* — und dann: *„So, ich habe dir jetzt im workspace bei
40_nutzerpng_auf_bg nochmal in gruen markiert wo stuhl fehlt in etwa"*.

**Ergebnis in einem Satz.** Seine 187 Punkte sind eingebaut, der Cut ist mit dem echten
Werkzeug neu gebaut, und die Verdeckung **steigt** — von 6912 auf **7429** verdeckte
Koerperkastenpunkte ueber die 6841 begehbaren Standplaetze dieses Winkels (+517), an **21**
Standplaetzen aendert sich das Urteil und **an keinem einzigen nach unten**. Die
VOR-Verdeckung bleibt **0**, die Rechteckzahl bleibt **104 von 105**, und die im Riegel
festgenagelte Modellklasse (eine Tiefe je Bildzeile, monoton, Naht ≤ 1 Eimer) **haelt** auf
der neuen Silhouette.

| | |
|---|---|
| Beleg des Nutzers | `analysis/befunde_2026-09-21/pri-runde19c-saum/nutzer_gruen_2026-09-21.png`, 1280x960, md5 `9e9eee6da292da33c75ecf34f4fa3778` — **bitgleich** mit seiner Arbeitsdatei `40_nutzerpng_auf_bg.png` |
| Silhouette neu | `pri/STAGE1/10D0/07_01_mit_saum.png`, 2677 → **2864** opake Punkte (Alpha > 110) |
| Hintergrund | `build/bg_ppm/ROOM10D07.ppm`, selbst erzeugt mit `build_r19c/tests/unit/probe_bg_dump.exe`, md5 `9890da5a1457d5f647ca73563be6cdb4` (gleich wie in Runde 19b) |
| Standplaetze | `build/p2/dump_klemmpfad.txt`, Sonde `probe_p2_floor_dump 1000 7000`, Herkunftsmarke `# QUELLE klemmpfad`, ROOM10D0 Band 0 = 43147 Punkte |
| Abzuege | im Lauf `build/r19c/abzug_vorher/bild000260.ppm` und `…/abzug_nachher/bild000260.ppm`, hier abgelegt als `pri-runde19c-saum/40_abzug_vorher_ganz.png` und `41_abzug_nachher_ganz.png`; je 960x720, Vollbild-Readback **vor** `SDL_RenderPresent` (`RE15_FRAMEDUMP`, main.c:9597-9625) |
| Messskripte | `analysis/befunde_2026-09-21/pri-runde19c-saum/protokoll/` (`bau_alt.sh`, `bau_neu.sh`, `bau_ausliefern.sh`, `abzug_F3843.sh`, `abzug_vorher.sh`, `mess_01`…`mess_12`) |

⛔ **Diese Runde ist eine NUTZER-ENTSCHEIDUNG, keine byte-true Herleitung.** Das Original
liefert fuer diesen Winkel gar keine Masken (im Riegel geprueft:
`re15_pri_parse_section(rdt, …, cuts[7].pri_offset)` ≤ 0). Die Zuordnung „dieser Punkt
gehoert zum Stuhl" ist die Handarbeit des Nutzers; ich habe sie **gemessen und eingebaut**,
nicht begruendet. So steht es auch im `grund`-Text der Auswahl.

---

## 1. Was er markiert hat — jede Zahl selbst nachgemessen

`protokoll/mess_01_saum_pruefen.py`, `mess_02_farbklassen.py`, `mess_12_gruppen.py`.

Seine Marken tragen **genau eine** Farbe, `(34,177,76)`:

```
gruene Quellpunkte (genau (34,177,76)):  2651
   Schwelle g>r+10 und g>b+10 :  9067     <- Antialias des Pinsels
   Schwelle g>r+20 und g>b+20 :  2763
   Schwelle g>r+30 und g>b+30 :  2651     <- ab hier stabil bis +100
Zielpunkte 320x240 (4x4, "einer genuegt"):  190
   davon schon gedeckt 3, NEU 187, ausserhalb seines 73x117-Fensters bei (0,123):  0
```

Es ist ein **Saum**, kein neuer Gegenstand: **116 der 187** sind direkte 8er-Nachbarn der
bisherigen Silhouette, der Abstand zur alten Form hat Median **1,0**, 90-%-Quantil **3,2**,
Maximum **5,0** Punkte. Die 18 zusammenhaengenden Gruppen, nach Groesse:

| Punkte | Fenster | was dort im Hintergrund liegt |
|---|---|---|
| 79 | x1..14 y224..239 | vorderes Bein und untere Querstrebe, bis an die Bildunterkante |
| **28** | **x59..62 y142..156** | **die rechte Kante der Lehne — genau „was von den Rahmen" fehlte** |
| 17 | x0 y123..139 | linke Kante der Lehne, in Bildspalte 0 |
| 9 | x26..28 y139..142 | Lehnenoberkante |
| 8 | x57..59 y201..203 | Knick der rechten Diagonalstrebe |
| 8 | x15..18 y234..235 | Fussende links |
| 8 | x15..17 y190..192 | Sitzkante |
| 5 | x44..45 y187..189 | Sitzkante Mitte |
| 5 | x3..7 y123 | Lehnenoberkante links |
| 9 weitere Gruppen | 2..4 Punkte | Einzelpunkte entlang derselben Kanten (2 davon einzeln) |

Bild `42_saum_auf_hintergrund.png` (10-fach): seine alte Freistellung hell, die 187 neuen
Punkte gruen, auf dem gemalten Hintergrund.

**Die Farbe stuetzt seine Zuordnung, sie ist nicht ihr Grund** (b-r am Hintergrund,
Quartile):

| Menge | n | b-r 25 % / 50 % / 75 % |
|---|---|---|
| Saum, alle | 187 | **+0 / +3 / +3** |
| Saum, hell ≥ 150 | 94 | +1 / +3 / +4 |
| Chrom INNERHALB seiner Freistellung (hell ≥ 150) | 351 | **-1 / +1 / +3** |
| heller Behaelter dahinter (x41..64 y118..134, hell) | 90 | **+15 / +21 / +25** |

Also dieselbe neutrale Klasse wie sein eigenes Chrom, nicht die des Behaelters — der
Behaelter bleibt draussen, wie in Runde 19b belegt.

**Die Datei des Nutzers ist unangetastet.** `07_01_mit_saum.png` ist die Vereinigung:
seine 2677 Punkte sind darin **bitgleich** (2677 von 2677 Pixel gleich), die 187 neuen
tragen Alpha 255 und die Farbe des Hintergrundbilds an **187 von 187** Punkten (0
Abweichungen). `07_01.png` selbst ist nicht verändert (Hash-Riegel `unit_pri_hashes`
fuehrt sie weiter mit `36175c6153f5ecb01469da39fb8d2b092fbd5340`).

### 1.1 Berichtigung an meiner eigenen Vorlage

Der Auftragstext sagte, die **12** offenen Punkte aus `pri-runde19b-marke4.md` §3.3 seien in
seinen 187 enthalten. Gemessen sind es **8 von 12**: er hat `x61 y142..145` und
`x60 y148..151` markiert, die vier oberen `x55..57 y132..133` **nicht**. Die bleiben
draussen — er hat sie nicht getroffen, und ich erfinde sie nicht dazu.

---

## 2. Der Bau — mit dem echten Werkzeug, und die Kette ist belegt

### 2.1 Gegenprobe zuerst: reproduziert meine Kette den Auslieferungsstand?

`protokoll/bau_alt.sh` baut Cut 7 mit der **alten** `07_01.png` neu, ueber
`raum.py → bau_p2.bau_cut`, `--stufe 16 --statistik max`. Alle vier geschriebenen Dateien
sind **byte-identisch** mit dem Auslieferungsstand:

```
fcae4848f4106f6c8746278bc77caf58   ROOM10D0.MSK          (neu gebaut == ausgeliefert)
84f25318e149144d5528141e780bff82   ROOM10D0_PRI07.TIM    (neu gebaut == ausgeliefert)
5116c6c6425aa8c4fb681c319fecbd7e   ROOM10D0_PRI07.PBM    (neu gebaut == ausgeliefert)
c96b0efa37dcee810353ac9125cb262e   ROOM10D0_PRI07.STAND  (neu gebaut == ausgeliefert)
```

Damit ist **gemessen** (nicht angenommen), dass `--stufe 16 --statistik max` die
Auslieferungsparameter sind und dass der Unterschied unten allein von der Silhouette kommt.

### 2.2 Der neue Bau

`protokoll/bau_neu.sh` (Probebau) und `bau_ausliefern.sh` (in den Auslieferungsbaum,
Ergebnis hash-gleich mit dem Probebau):

```
"Stuhl" (png, ebene 0): standpunkte: 73 Spalten, davon 73 eigene, 0 geerbt; Profil-Tiefe 52..61
zerlegung Stufe 1: Stuhl=zeilen/16, Tischplattenec=zeilen/16, Holztisch mit =zeilen/12
                   -> 104 Rechtecke, 25838 Atlaspunkte
Cut 7: Soll 5603 px, Deckung fehlt 0 / zuviel 0 | 104 Rechtecke, 53 Tiefenstufen (52..105),
       25838 Atlaspunkte | Standplaetze 6841, beruehrt 49 | VOR n=21 verdeckt=0 teil=0 |
       HINTER n=29 frei=3 -> GESCHRIEBEN
```

| | vorher | nachher |
|---|---|---|
| Soll-Punkte des Cuts | 5435 | **5603** (+168) |
| Rechtecke | 104 | **104** |
| Tiefenstufen | 53 (53..105) | 53 (**52**..105) |
| Atlaspunkte | 25887 | **25838** |
| MSK / TIM | `fcae4848…` / `84f25318…` | `acc6c806846fae3562653cb83ffab550` / `bf3553c22ce5036e8a377a67de86e082` |

⛔ **+168, nicht +187.** 19 der 187 Punkte waren schon vom Quader „Holztisch mit
Klappstuhl" gedeckt; sie wechseln nur ihre Tiefe (§3.2).

### 2.3 Nebenwirkung, benannt statt verschwiegen: der Standpunkt wandert um EINEN Eimer

`protokoll/mess_03_standpunkt.py`. Der `aufrecht`-Zweig nimmt **einen** Standpunkt:
Schwerpunkt der belegten Spalten (`cx = mean(xs) + 0,5`, `geometrie.py:125`) und **tiefste
Silhouettenzeile** (`geometrie.py:127`). Der Saum aendert beides:

| | Spalten | cx | tiefste Zeile | Standpunkt Welt | Kamera-z | Eimer | Zeilenprofil |
|---|---|---|---|---|---|---|---|
| vorher | 72 (x1..72) | 37,0 | 238 | (-1669, 26016) | 3985 | 62 | 53..62 |
| nachher | 73 (**x0**..72) | 36,5 | **239** | (-1691, 26008) | 3962 | 61 | **52..61** |

Das ist **kein eingeschleuster Parameter**, sondern die eigene Regel des Modells auf der
Silhouette des Nutzers: er hat Bildspalte 0 und Bildzeile 239 mitmarkiert. Die Richtung ist
die verlangte — **naeher** heisst **mehr** Verdeckung (Maske verdeckt gdw.
`Tiefe < Eimer der Figur`; `re15_pri.h`: Masken-OT = `depth*1` @0x80039650-64, Figur-OT =
`otz>>4` @0x8002565c, ZSF3 = 341 @0x80066c70).

---

## 3. Auftrag 2 — die Wirkung, gemessen ueber die begehbaren Standplaetze

`protokoll/mess_04_wirkung.py`. Beide Staende ueber den **echten** Ladeweg gelesen
(`maskenbild.masken` + `lies_tim_bytes` + `abnahme.deckung_und_tiefe`), Standplaetze aus dem
**Klemmpfad**-Dump.

```
VORHER : 104 Rechtecke, 5435 gedeckte Punkte, Tiefen 53..103
NACHHER: 104 Rechtecke, 5603 gedeckte Punkte, Tiefen 52..103
Deckung: nur nachher 168, nur vorher 0, gemeinsam 5435
```

**Kein einziger vorher gedeckter Punkt fällt weg.**

| Koerperkasten | Standplaetze | beruehrt | verdeckte Kastenpunkte vorher → nachher | Urteil geaendert | davon MEHR / WENIGER |
|---|---|---|---|---|---|
| KOPF=1500, HALB=450 | 6841 | 49 | **6912 → 7429 (+517)** | **21** | **21 / 0** |
| KOPF=3000, HALB=450 | 6587 | 101 | **7096 → 7652 (+556)** | **22** | **22 / 0** |

Die staerksten Plaetze (KOPF=3000): (-900, 25750) 555 → 594, (-700, 25750) 186 → 216, dann
19 Plaetze mit gleichmaessig **+28** (x 500..1500, z 26750..27350) — der Korridor, aus dem
der Nutzer auf den Stuhl schaut.

**Spiegelregel gehalten:**

| | VORn | VORverd | VORteil | HINTn | HINTfrei |
|---|---|---|---|---|---|
| vorher | 21 | **0** | 0 | 28 | 2 |
| nachher | 21 | **0** | 0 | 29 | 3 |

### 3.1 Der eine neue HINTfrei-Platz ist ein Artefakt der Schiene, kein Bildfehler

`protokoll/mess_06_hintfrei.py`. Neu beanstandet: (-1700, 24950), Quote 0,00.

```
NACHHER (-1700, 24950) scr=( 87,248) kasten x63..112 y173..240  Kamera-z 3761
   Spalten mit Maske 10, davon HINTER 3 / VOR 6 ; Verdeckungsquote HINTER 0.00
   HINTER-Spalten 70..72 ; Standlinie dort 3687..3697 ; Referenz 3761
   Maskentiefen dort 57..57 ; Figur-Eimer 52..57 -> verdeckt 0 von 14
VORHER  (-1700, 24950) … Spalten mit Maske 10, davon HINTER 0 / VOR 6 ; Quote nan
   (Standlinie dort 3714..3729, Referenz 3761 -> Abstand 32..47, also INNERHALB eines Eimers)
```

Der Platz liegt mit **3761** gegen eine Standlinie von **3687..3697** — Abstand **64..74**
Einheiten, und ein OT-Eimer ist **64,06** breit. Vor der Verschiebung waren es 32..47, also
weniger als ein Eimer: die Schiene stufte ihn als „unentschieden" ein, jetzt als „HINTER".
Was dort wirklich im Bild steht: die Fuesse der Figur liegen auf Bildzeile **248**, also
**unter** dem Bild; die Maske existiert in ihrem Kasten nur in den Spalten 70..72 mit Tiefe
57, und die Figur traegt in genau diesen Zeilen die Eimer 52..57 — sie ist dort **naeher**
(Fuss-Kamera-z 3761 gegen Stuhl-Standpunkt 3962) und darf nicht verdeckt werden. Die
Schiene vergleicht auf der **Fussebene** gegen eine **Spalten-Standlinie**, die in 70..72
von der duennen Diagonalstrebe kommt — genau das Phaenomen aus 19b §1. Der Nachbarplatz
(-1500, 24950) stand mit derselben Ursache schon vorher offen.

⛔ **Damit steht ein zweiter Fall in derselben Sache auf der Liste: die
HINTfrei-Kennzahl der Abnahme reagiert an Eimergrenzen auf Spalten-Standlinien duenner
Streben. Eigener offener Posten, hier gemessen gemeldet, nicht gefixt** — die Kennzahl ist
kein Abbruchkriterium (das ist `VORverd`, und das bleibt 0).

### 3.2 Wo sich die Tiefe aendert — und dass keine fremde Deutung mitrutscht

`protokoll/mess_05_tiefendiff.py`:

```
Stuhl alt (2677)           n= 2677  geaendert  1093  Differenzen: -1:1093
Tischplattenecke           n=  986  geaendert     0  Differenzen: keine
Holztisch-Quader (Rest)    n= 1794  geaendert    19  Differenzen: -24:1, -22:2, -19:3, -13:5, -8:8
Saum: Tiefen nachher 52..61 ; 187 Punkte, davon vorher von einem ANDEREN Objekt gedeckt 19
```

1093 der 2677 Stuhlpunkte ruecken um **genau einen** Eimer naeher (die Standpunkt-
Verschiebung, §2.3; die uebrigen bleiben, weil sie den Eimerrand nicht ueberschreiten). Die
Tischplattenecke bleibt **unberuehrt**. Die **19** Punkte mit -8..-24 sind genau die
Ueberschneidung von Saum und Tisch-Quader: dort steht jetzt die Tiefe des **Stuhls**, und
das ist richtig — der Stuhl steht vor dem Tisch.

---

## 4. Auftrag 3 — die Grenzen

`protokoll/mess_07_grenzen.py`, aus den **geschriebenen** Dateien gelesen:

```
VORHER  Rechtecke 104 von 105 (99 %) | Atlasblatt 256x256, belegte Flaeche 25887 von 65536
        (39,5 %), Summe der Rechteckflaechen 25887 | TIM 66080 Bytes | Tiefenstufen 53 (53..105)
NACHHER Rechtecke 104 von 105 (99 %) | Atlasblatt 256x256, belegte Flaeche 25838 von 65536
        (39,4 %), Summe der Rechteckflaechen 25838 | TIM 66080 Bytes | Tiefenstufen 53 (52..105)
```

Die Engine-Schranke ist **105** (`re15_port/include/re15_pri.h:50`,
`RE15_PRI_MAX_MASKS_PER_CUT`, belegt aus ROOM3000/ROOM3001 Cut 3, `hdr byte[7] = 0x69`).
Der Saum traegt den Cut **nicht** darueber — er bleibt bei 104, und die Atlasflaeche sinkt
sogar leicht (die Zeilenzerlegung findet auf der breiteren Silhouette laengere Streifen).
Es war also **keine** feinere Tiefenstufung und **kein** anderes Packverfahren noetig; die
gewaehlte Stufe ist unveraendert die feinste (Stufe 1).

---

## 5. Auftrag 4 — der Riegel, und ob die Modellklasse haelt

**JA, sie haelt.** `re15_port/tests/unit/r19b_marke4_10d0_c7_zeilenmodell.c`, Referenz
`probes/r19b_marke4_stuhl.pbm` auf **2864** Punkte nachgezogen (vorher 2677; die alte Datei
war vor dem Ueberschreiben gegen die alte Freistellung geprueft, `mess_08_riegel_pbm.py`),
`SOLL_PX` = 2864 mit der Herkunft im Kommentar.

```
=== RIEGEL ROOM10D0 C7 — der Stuhl traegt EIN Zeilenmodell ===
  Kamerasatz Cut 7: H=208  R[7]=1040
  PASS: R[7] > 0 — die Kamera schaut nach unten, was aufragt ist NAEHER
  PASS: ROOM10D0 Cut 7 hat im ORIGINAL keine Masken
  ROOM10D0.MSK Cut 7: Sektion @0x9EC, 104 Masken, 104 gezeichnet
Zeilenmodell: 117 Silhouettenzeilen (y123..239), 0 ohne durchgehende Tiefe, 0 mehrdeutig,
              0 Monotonie-Rueckschritte, groesster Zeilensprung 1
  PASS: (B) jede Silhouettenzeile traegt EINE Tiefe an ALLEN ihren Punkten
  PASS: (C) die Zeilentiefe ist monoton (Vorzeichen aus R[7] geprueft)
  Freistellung 07_01_mit_saum: 2864 Punkte, gedeckt 2864, ungedeckt 0
  PASS: die Referenz-Silhouette hat die erwarteten 2864 Punkte
  PASS: jeder Punkt der Nutzer-Freistellung ist gedeckt
  117 Bildzeilen mit Punkten, Tiefen 52..61, Zeilen mit mehreren Tiefen 0, Rueckschritte 0
  Naht: 10396 Nachbarpaare, groesster Tiefensprung 1 bei (0,126)=52 -> (0,127)=53,
        Paare mit Sprung > 1: 0
  PASS: der starre Gegenstand springt an seiner eigenen Naht um hoechstens EINEN Eimer
```

Die **Gegenprobe wird durch den Saum staerker**, nicht schwaecher:

| Gegenprobe | vorher (19b) | nachher |
|---|---|---|
| Spaltenregel: Zeilen mit mehreren Tiefen | 114 von 116 | **116 von 117** |
| Spaltenregel: groesster Eimer | 86 | **145** |
| Spaltenregel: Naht-Sprung / Paare > 1 | 24 / 318 | **79 / 165** (bei (0,123)=145 → (1,123)=66) |
| + `bodenkante 1..68`: Naht-Sprung / Paare > 1 | 10 | **11 / 93** |

### 5.1 Ein latenter Fehler im Riegel, den der Saum aufgedeckt hat

Die Schranke „der Weltsprung 68→69 ist groesser als jeder Sprung **innerhalb 1..68**" war
als `x <= BK_X1` geschrieben und nahm damit auch das Paar **(0 → 1)** mit. Solange Spalte 0
leer war, war das wirkungslos; mit dem Saum (Spalte 0 belegt, **nur** y123..139) schlug der
Riegel fehl — nicht weil die Aussage falsch ist, sondern weil Spalte 0 zu **derselben**
Gruppe gehoert wie 69..72: ihre unterste Zeile ist **kein** Bodenkontakt, ihr Sehstrahl
landet **6521** Einheiten von Spalte 1 entfernt. Der Scan zaehlt jetzt nur Paare **ganz**
innerhalb 1..68, und die neue Spalte ist als **zusaetzliche** Pruefung drin:

```
  Weltsprung des Bodenpunkts 68->69 = 1458 Einheiten, groesster Sprung innerhalb 1..68 = 805
  PASS: der Weltsprung 68->69 ist groesser als jeder Sprung innerhalb 1..68
  Weltsprung 0->1 (die Saum-Spalte des Nutzers) = 6521 Einheiten
  PASS: auch Spalte 0 hat keinen Bodenkontakt: ihr Sprung nach 1 ist groesser als jeder
        Sprung innerhalb 1..68 (der Saum belegt dort nur y123..139)
```

---

## 6. Auftrag 5 — Bild aus dem laufenden Spiel, vorher und nachher, selbst angesehen

⛔ In Runde 19 stand hier noch „Live-Abzug ist mir nicht gelungen" (Dossier
`pri-runde19/marke2-10E0-C7.md` §8). **Diesmal ist er gelungen**, und die Bilder sind aus
dem Spiel, nicht gerechnet. Lauf (`protokoll/abzug_F3843.sh`, `abzug_vorher.sh`):

```
RE15_NO_INTRO=1 RE15_NOAUDIO=1 RE15_TITLE_SHOT=… RE15_TITLE_SHOT_AF=2
RE15_DEBUG_JUMP=10D0@120 RE15_PLAYER_POS=418,26497,-2361 RE15_FORCE_CUT=7
RE15_FRAMEDUMP=200-260/20:…/bild        (build_r19c/platform/pc/re15_pc.exe)
```

`RE15_FRAMEDUMP` liest das komplett komponierte Bild **innerhalb** von
`re15_render_end_frame()` unmittelbar **vor** `SDL_RenderPresent` zurueck (main.c:9597-9625)
— kein `RE15_AUTOSHOT`, kein Softwarerenderer. Der Weg in den Raum ist der
Debug-Menue-Sprung, also derselbe Pfad wie ein Quadrat-Druck des Nutzers.

**Dass es dieselbe Stelle ist, steht im Protokoll des Spiels selbst** — die Zeile aus
`befund.log` des Laufs gegen die Marke des Nutzers:

```
Nutzer F3843 : R10D0 C7 pos=(418,0,26497) rot=-2361 vz=6073/5883/5692 scr=(77,179) kasten=x59..95,y118..182 masken=104
mein Lauf    : R10D0 C7 pos=(418,0,26497) rot=-2361 vz=6073/5883/5692 scr=(77,179) kasten=x59..95,y118..182 masken=104
```

Fuer den VORHER-Abzug wurden die vier Cut-7-Dateien des Auslieferungsstandes kurz eingesetzt
und danach zurueckgeschrieben, mit Hash-Beleg vor und nach dem Tausch (`abzug_vorher.sh`).

### 6.1 Was man sieht

Bilder: `40_abzug_vorher_ganz.png`, `41_abzug_nachher_ganz.png`,
`50_vorher_nachher_stuhl.png` (9-fach, links vorher / rechts nachher),
`52_kante_vorher_nachher_bg.png` (36-fach, vorher | nachher | **gemalter Hintergrund**),
`51_differenz_rot.png` (die geaenderten Punkte rot).

**Vorher** laeuft die helle Chromkante der Lehne von unten links herauf und **bricht an
Leons linkem Bein ab** — seine Hose ist ueber die gemalte Strebe gezeichnet, die Kante
endet mitten in der Luft und setzt oberhalb wieder an. **Nachher** laeuft dieselbe Kante
**durch**: der helle Chromstrich zieht ohne Unterbrechung an seinem Bein vorbei, und der
Verlauf stimmt mit der dritten Spalte (dem gemalten Hintergrund) ueberein. Unten am Stiefel
setzt sich das Rohr ebenso fort.

Und das ist nicht „sieht besser aus", sondern nachgezaehlt
(`protokoll/mess_10_ausschnitt.py`; Helligkeit r+g+b in der Mitte des 3x3-Blocks):

```
  Zeile | Spalte  vorher / nachher / Hintergrund
   y142 | x61  79/216/214   x62  71/296/301
   y143 | x61  74/248/241   x62  79/248/247
   y145 | x61  87/248/246   x62  79/176/170
   y148 | x60  74/168/162   x61  71/176/177
   y152 | x59  79/120/121   x60  87/160/161
   y155 | x59  74/176/172   x60  79/120/120
```

Vorher stand dort Leons dunkle Hose (**60..87**), nachher der gemalte Hintergrund
(**120..301**), und zwar auf **± 7** genau (der Rest ist die 5-Bit-CLUT-Rundung des Atlas).

### 6.2 Selbstpruefung des Vergleichs

`protokoll/mess_09_bildvergleich.py`:

```
unterschiedliche Bildpunkte (960x720): 8388 von 691200 (1,214 %), groesste Abweichung 230
Rechteck der Unterschiede: x0..212 y369..719   (in 320x240: x0..70 y123..239)
im Koerperkasten der Marke (x59..95 y118..182): 414 Punkte
```

Die Unterschiede liegen **ausschliesslich** im Fenster der Freistellung (x0..70, y123..239).
Der Koerperkasten der Figur reicht bis x95 — dort ist **nichts** verschieden. Damit ist
belegt, dass beide Laeufe dieselbe Pose und dieselbe Beleuchtung zeigen und die Differenz
allein von den Masken kommt.

---

## 7. Auftrag 6 — Suite

`RE15_BUILD_DIR=re15_port/build_r19c bash re15_port/tools/local_build.sh all`:

```
100% tests passed, 0 tests failed out of 334
Total Test time (real) = 134.09 sec
=== LOCAL-BUILD-OK (all) — Tests 334/334
```

**334/334 Tests gruen.** Kein neuer Test — der Riegel `unit_r19b_marke4_zeilenmodell` ist
derselbe, mit nachgezogener Referenz und einer zusaetzlichen Pruefung darin (§5.1); auch die
beiden Riegel, die der Saum beruehrt, stehen gruen: `unit_pri_hashes` (118 Dateien) und
`unit_pri_silhouette` (Engine-Deckung == Ziel-PBM bitgenau).

Mitgezogen, weil der Bau eine Datei hinzufuegt:
`re15_port/tests/unit/pri_stage1_hashes.inc` neu erzeugt mit
`tools/maske/pri_hashes.py` (117 → **118** Dateien; neu
`pri/STAGE1/10D0/07_01_mit_saum.png` mit Blob `d2bdaca0f348ca5dba542673c42fb96929bb37da`,
Herkunft „neu"). ⛔ Diese Datei ist **abgeleitet**, nicht vom Nutzer geliefert — sie steht
mit ihrer Herkunft im Riegel, damit niemand sie fuer ein Original haelt.

---

## 8. Was offen bleibt

1. ⛔ **Die HINTfrei-Kennzahl der Abnahme** stuft an Eimergrenzen nach einer
   Spalten-Standlinie ein, die bei duennen Streben vom falschen Bildpunkt kommt (§3.1).
   Zwei Plaetze betroffen ((-1700, 24950) neu, (-1500, 24950) schon vorher), beide ohne
   Bildfehler. Nicht gefixt, gemessen gemeldet.
2. ⛔ **Die vier Punkte `x55..57 y132..133`** aus der Rueckfrage der Runde 19b hat der
   Nutzer **nicht** markiert (§1.1). Sie bleiben draussen.
3. ⛔ **Der Quader „Holztisch mit Klappstuhl"** ist weiterhin meine eigene Quelle
   (1794 von 5603 Deckungspunkten, unveraendert). Offener Posten aus Runde 19b.
4. ⛔ **Der zweite Stuhl** (Chromrohr x70..75, y125..139) und der **helle Behaelter**
   dahinter sind nach wie vor nicht freigestellt — beide stehen HINTER dem Spieler
   (19b §3.1), also ohne Verdeckungsauftrag an dieser Marke.

---

## 9. Dateien dieser Runde

| Datei | Aenderung |
|---|---|
| `pri/STAGE1/10D0/07_01_mit_saum.png` | neu (2864 Punkte; 2677 Nutzer + 187 gruene Marken) |
| `analysis/esp_masken_2026-09-03/auswahl.json` | ROOM10D0 „7" / „Stuhl": `png` auf `07_01_mit_saum.png`, `grund` mit Herkunft und Zahlen; `aufrecht`/Lage/Massstab/oben/grow unveraendert |
| `re15_port/shared_assets/PSX/MASKS/ROOM10D0.MSK` | `fcae4848…` → `acc6c806846fae3562653cb83ffab550` |
| `…/ROOM10D0_PRI07.TIM` | `84f25318…` → `bf3553c22ce5036e8a377a67de86e082` |
| `…/ROOM10D0_PRI07.PBM` | `5116c6c6…` → `a25cbcefce53cd3aeadece933dc304dc` |
| `…/ROOM10D0_PRI07.STAND` | `c96b0efa…` → `091b765019d7dc4f24d828175f6348b5` |
| `re15_port/tests/unit/probes/r19b_marke4_stuhl.pbm` | 2677 → 2864 Punkte |
| `re15_port/tests/unit/r19b_marke4_10d0_c7_zeilenmodell.c` | `SOLL_PX` 2864, Herkunft im Kopf, Spalten-0-Pruefung, Fehler im Gegenprobe-Scan behoben (§5.1) |
| `re15_port/tests/unit/pri_stage1_hashes.inc` | 117 → 118 Dateien |
| `analysis/befunde_2026-09-21/pri-runde19c-saum{.md,/}` | dieses Dossier, Bilder, 12 Messskripte, 5 Laufskripte |

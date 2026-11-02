# Spur GR — Handgranate zerfetzt Zombies (Runde 41, 2026-11-02)

Zweig `r41/granate`, Baum `.claude/worktrees/r41_granate`, Basis `r41/spinnengore` 2f886e91a.
Dossier wird FORTLAUFEND geschrieben (Sitzungslimit-Schutz).

## N1. NACHBESSERUNG 1 (2026-11-02) — Berichtigung nach Nutzer-Korrektur

> 1. "Das stimmt nicht, bei complete disc video 2 bei 15 Sekunden wirft Leon eine Granate. Und auch die Hunde und andere
>    Gegner die zerfetzen koennen sollen natuerlich entsprechend zerfetzen von einer Granate."
> 2. "Nein, nicht die Spinne ist das was zersprengt wird durch die Granate, sondern die Zombies im Sewer"
> 3. (Nachtrag) "Bei der Granate das es garkeine Koerperteile mehr hinterher gibt die uebrig bleiben ist unwahrscheinlich.
>    Wahrscheinlich wuerden die einzelnen Koerperteile nach der Explosion dann da liegen. Man sieht es nur in dem Video
>    Ausschnitt nicht so richtig."

### N1.0 ⛔ BERICHTIGT — was in der ersten Fassung falsch war

| Stelle erste Fassung | Falsche Aussage | Richtig (Einzelbild, unten belegt) |
|---|---|---|
| §2.1 Ueberschrift + Text, §8 Punkt 2 | "Es gibt im 80%-Material keinen Handgranaten-Wurf" | Es gibt GENAU EINEN: Leon wirft in CD2 16,95-17,9 s eine Handgranate, Detonation 19,253 s am Khaki-Zombie im Kanal (N1.1). Dieselbe Szene ist B 5:20.35-5:24.0. |
| §2.1 Zeile "B 5:18-5:23.2 (Tunnel)", Spur S Z14 | "Schuetze nicht im Bild, Waffe offen" | Handgranate, Werfer Leon (N1.1) |
| Auftrag/Spur S Z14 | "Wiederholung um B 7:06.9" | dort keine Explosion, nur eine Trefferwolke (Beleg video2/x01) |
| §2.2, §5 letzter Punkt, §9.4 | "der Rumpf bleibt", "Rumpf-Platzen waere Erfindung" | Am Ort des Zombies bleibt NICHTS stehen (CD2 ab 19,65 s). Wo die Teile landen, zeigt das Video nicht; Nutzer-Entscheid: die Teile bleiben verstreut liegen (N1.4). |
| Spinne CD2 14,6 s | (Nutzer-Hinweis) | Die Spinne bei CD2 14,65 s platzt nach einem SCHUSS (Leon, Handfeuerwaffe, Muendungsblitze 11,8/13,2/14,4 s), nicht durch eine Granate. |
| Elza-Granatwerfer B 3:30 / 3:36 / 8:05 / 8:42 | — | bestaetigt (Muendungsblitz an der Huefte, Einschlag 0,07-0,33 s spaeter, nie ein Ausholen), Beleg video2/g01-g04 |

### N1.1 Was das Video zeigt (CD2, Bild fuer Bild)

Datei `Version_80%/Resident Evil 1.5 Complete Disc Video 2 (480p Upscale) (480p).mp4`, 640x480, 29,97 B/s; Bild i = i/29,97 s
(exakt per cv2). Werkzeuge `scratchpad/GR2/sheet.py` (Boegen), Messreihe `GR_belege/n1/n1_cd2_messreihe_566_639.txt`.

| Zeit (Bild) | Was | Beleg |
|---|---|---|
| 14,65 s | Spinne platzt nach Schuessen (Leon, blaue Uniform, Maschinenraum mit rotem Rad) — keine Granate | video2/x02 |
| 15,62 s (#468) | Schnitt: Abwasser-Tunnel mit oranger Tuer. Links auf dem Steg 3 Khaki-Zombies, rechts hinten die Spielerfigur (Leon; im Kanallicht gruenlich, B-Ortsliste `video_b.md` §4: "Leon rechts hinten, Zombies in Khaki"). Unten in der Kanalrinne glimmt es: orange-gelbe Pixel (R>180, G>90, B<90) 76 im ersten Bild 15,62 s, flackernd abklingend auf 0 ab 16,42 s (Nachflackern 16,55 s), rote Pixel 792 -> 52 bis 17,35 s — wie ein ausgehender Brand. Es ist schon im ERSTEN Bild der Einstellung da; eine fruehere Explosion ist im Material NICHT zu sehen (davor B 5:14.5-5:19 der Spinnenraum), Ursache offen | nutzer_cd2/cd2_15s5_17s6_kanal.jpg |
| 16,95-17,15 s (#508-514) | Leon hebt einen Arm ueber den Kopf (Ausholen) | n1/n1_cd2_1655_1755_wurf_ausholen.jpg |
| 17,18-17,9 s (#515-536) | Vorlage mit weit nach hinten gestrecktem Bein = Wurf-Nachschwung, ueber den Schnitt 17,58 s hinweg; ab ~18,2 s steht er wieder | n1/n1_cd2_1758_1859_wurf_nachschwung.jpg |
| 17,6-19,2 s | Nahe Kamera: der Khaki-Zombie haengt/kriecht ueber der Stegkante, Oberkoerper auf dem Steg, Beine in der Rinne. Granate im Flug nicht zu erkennen | nutzer_cd2/cd2_17s5_19s2_kanal.jpg |
| **19,253 s (#577)** | **erstes Detonationsbild**: rote Wolke am Zombie, 3-4 khakifarbene Brocken (Glied-Laenge, 30-50 px) steigen ueber den Koerper; #576 (19,219 s) ist der Zombie noch ganz | n1/n1_cd2_576_579_vorher_detonation_raster.jpg |
| 19,29-19,52 s | rote Wolke ueber dem ganzen Koerper und groesser (x 290-560, y 255-470 bei #582), roter Bildanteil 1,7 % (#577) -> 2,6 % (#579/580) -> 0,4 % (#585); rote Brocken fliegen nach vorn links | Messreihe, n1_cd2_1925_1965_* |
| 19,29-19,62 s | weiss-gruenlicher, grob gepixelter Blitz/Qualm im Vordergrund unten; heller Bildanteil 0,4 % -> 7,75 % (#585, 19,52 s) -> 0,2 % (#588) = ~0,3 s | Messreihe |
| 19,32-19,69 s | die Brocken steigen bis an die Tunneldecke (Hoehe der Deckenleuchten, y 85-160), Scheitel 19,55-19,69 s = 9-13 Bilder nach der Detonation; Streuung nach links bis an die linke Wand und nach rechts neben Leon | n1_cd2_1925_1965_*, nutzer_cd2/cd2_19s0_21s0_explosion.jpg |
| 19,686 s (#590) -> 19,953 s (#598) | schwarze Rauchsaeule steigt an der Stelle des Zombies auf (x 230-470, y 120-480), Dunkelanteil der Bildmitte 41 % (Grundwert, Tunnelende) -> 74 % | n1/n1_cd2_1969_2102_rauch_teile_flecken.jpg, Messreihe |
| 19,95-21,09 s (#598-632) | der Rauch steht unveraendert (70-76 %) bis zum Schnitt 21,12 s (#633) — Dauer >= 1,4 s, Ende nicht im Bild | Messreihe |
| ab 19,75 s | dunkle Flecken an linker Wand und Decke (mittlere Luma der linken Wand 56 -> 31-45), bleiben bis zum Schnitt | n1_cd2_592_598_rauch_raster.jpg |
| 19,82-20,02 s | ein Brocken faellt neben Leon herunter und kommt ~20,02 s (#600) auf dem Steg an (23 Bilder nach der Detonation) | n1_cd2_1969_2102_* |
| 20,15 s | rote Spritzer auf beiden Stegkanten | n1_cd2_1969_2102_* |
| 20,55-20,62 s (#616-618) | ein grauer Brocken faellt noch durchs Bild (39-41 Bilder nach der Detonation) | n1_cd2_1969_2102_* |
| 19,65-21,1 s | am Ort des Zombies ist NICHTS mehr — kein Rumpf, keine Beine. Wo die Teile liegen bleiben, zeigt das Video NICHT (Rauch, Bildausschnitt, Schnitt nach 1,8 s) | — |
| Wurf -> Detonation | Ausholen 17,1 s -> Detonation 19,25 s = 2,0-2,2 s | — |

**Welcher Zombie, welcher Raum:** Der Tunnel mit oranger Tuer ist NICHT auf der 40%-Disc (Bildvergleich `video_b.md` §10.3: kein
Treffer 0,42; aehnlich nur der Fan-Raum 202 der Mod, ohne Tuer). Die Zombies der Kanal-Raeume sind auf Disc und Mod
**Typ 0x16** (eigener `Sce_em_set`-Zensus, `scratchpad/GR2/tools/emset.py`): 40%-Disc ROOM2030 3 x 0x16; Mod ROOM2000 2 x 0x16
(liegend, g=84), ROOM2020 4 x 0x16. Typ 0x16 = Port-Zombie-Familie (re15_re2z_owns_type), RE1.5-Modell aus `EMD/CDEMD0.EMS`
Bank 0x16; unter RE2-KI + RE2-Modellen das RE2-EM16. Der Zombie im Video ist khakifarben, kahl, gefleckt, gross — die
Kleidung passt zu "Zombies in Braun = Arbeitskleidung" (BEFUND 5.1, B 4:06-5:12); dass es genau 0x16 ist, folgt aus dem Ort
(Kanal) und dem Zensus, nicht aus dem Bild selbst.

### N1.2 Alle 80%-Videos erneut durchgesehen (Handgranaten und Explosions-Tode)

Durchsicht B 0:00-14:19 lueckenlos mit 2 B/s + Hilfsmessung an allen 25 774 Einzelbildern, CD1-4 mit 3 B/s, Final Build alle
174 Standbilder; jede gemeldete Stelle am Einzelbild bei voller Bildrate. Belege `GR_belege/video2/`. Zeitzuordnung (Helligkeits-
Korrelation): CD2 + 303,6 s = B, CD3 + 197,9 s = B, CD4 + 738,1 s = B.

| ID | Datei, Zeit | Ort | Werfer | Waffe, Merkmal | Ziel | Ausloesen -> Detonation | Ausgang | Beleg |
|---|---|---|---|---|---|---|---|---|
| **W1** | CD2 0:16.75-0:20.5 = B 5:20.35-5:24.0 | Tunnel mit oranger Tuer | Leon | **HANDGRANATE**: Ausholen, Vorlage mit Bein, kein Muendungsblitz | Khaki-Zombie (0x16) ueber der Stegkante | 17,1 -> 19,25 s (2,0-2,2 s) | zerfetzt, am Ort nichts mehr | video2/w01, w02 |
| G1a-c | B 3:30.91-3:34.42 (CD3 13,0-16,5) | 3000 | Elza | Granatwerfer, Muendungslicht an der Huefte | 3 Zombies | 0,07-0,33 s | 1. zerfetzt wahrscheinlich, 2./3. offen | video2/g01 |
| G2 | B 3:35.95 | 3010 | Elza | Granatwerfer | RPD-Zombie | 0,1 s | rote Saeule, weg, Brocken + Haufen in der Lache | video2/g02 |
| G3a-e | B 8:05.47-8:17.4 | 1070 Cut 3 | Elza | Granatwerfer, 5 Schuesse | >= 2 Zombies | 0,07-0,2 s | einer bleibt ganz und richtet sich auf (Z23) | video2/g03 |
| G4 | B 8:42.0 | 1070 Cut 7 | Elza | Granatwerfer, rosa Kugel sichtbar | Zombie in der Lache | 0,13 s | nur roter Fleischhaufen | video2/g04 |
| G5 | B 7:58.07 | 1030 Cut 1 blau | offen (Figur am Rand) | Muendungsblitz | Zombie | <= 0,07 s | Funkenexplosion, faellt; Rest offen | video2/g05 |
| G6 | CD4 25,86 = B 12:43.96 | 4080 | Elza | Granatwerfer wahrscheinlich | **Affenwesen** | 0,27 s | Feuerball, bleibt ganz | video2/o02 |
| O1 | B 7:50.73 | 1030 Cut 11 | offen | Waffe offen | Zombie | — | faellt, orange Reste | video2/o01 |
| O2 | CD4 23,7 = B 12:41.8 | 4080 | offen | Ausbruch am Ruecken | Affenwesen | — | faellt, steht wieder auf | video2/o02 |
| O3 | Final Build A005/A006/A142/A146/A158 | Gaenge | — | Standbilder | offen | — | offen | video2/o03 |

**Nicht-Zombies an einer Explosion:** nur das Affenwesen in 4080 (G6, O2) — es UEBERLEBT beide Treffer und zerfaellt nicht.
Hunde (1190), Kraehe (1170), Spinnen (Maschinenraum, 20B0, 2070), Birkin, Brandzombies: keine Explosion im Material. Damit gibt es
fuer Nicht-Zombies keinen Video-Beleg des Granaten-Zerfetzens; die Quelle fuer Hund/Kraehe/Spinne ist RE2/RE1.5 (N1.3).

### N1.3 Inventur aller Gegnertypen (Code gelesen, Datei:Zeile im Explore-Bericht; HG = Handgranate w9/Art 2, WE = Werfer-Explosivrunde w15/RE2-Zeile 9; "K0/K1" = Direkttreffer/Explosion)

| Typ | Name | HP RE2-KI / RE1.5-KI | Schaden HG / WE (K0 / K1) | stirbt an 1 Granate? | Gore-Quelle (Beleg) | Explosionstod VORHER | NACHHER |
|---|---|---|---|---|---|---|---|
| 0x10/0x12/0x13/0x18 | Zombie | 50..143 / dto. (Import an; ohne Import 0x10 61-101 ...) | 1000 / 200 / 50 | HG ja; WE direkt ja, Explosion nur bei HP < 50 | Video CD2 19,25 s (HG) + B 3:31/3:36/8:42 (WE); RE2 8BEC/9610/92C4; RE1.5 0x80107ee0/0x80108abc | RE2: Kopf/Oberarme/Oberschenkel fliegen, Rest platzt, Rumpf + Becken bleiben; RE1.5: w9 dto., w15 Standardtod | beide KIs, w9 UND w15: 7 Stuecke fliegen bis ~2400-2900 ueber den Boden und BLEIBEN verstreut liegen, am Ort nichts, Rauch >= 1,6 s, Lache |
| 0x11 | dicker Zombie (Brad) | 250 / dto. (ohne Import 71-107) | 1000 / 200 / 50 | HG ja; **WE nein (Rest 50 bzw. 200)** | wie oben | wie oben | wie oben |
| 0x16 | Zombie (Kanal, Video-Typ) | 50..143 / dto. | 1000 / 80 / 50 | HG ja; **WE meist nein (Rest bis 63 / 93)** | wie oben | wie oben | wie oben |
| 0x20 | Hund | 69..132 / 65..111 | RE2 300 / 300 / 50; RE1.5 1000 / 100 | HG ja; WE direkt ja, **Explosion RE2 nein (Rest 19-82)**; RE1.5 WE bei HP < 100 | RE2 Teile-Wurf 0x80104440 (7 Parts @0x80105680) | RE2: 7 Parts driften/platzen (0x4A) und sind nach 29 Bildern weg, Rumpf bleibt; WE-Explosion (1D2 >= 3) nur Schrei; RE1.5: nur Blut (Luft-Tod) | beide KIs, Zeile 9 jeder Spalte: dieselben 7 Parts fliegen (Wurf 0x1062) und BLEIBEN liegen, Rumpf bleibt (RE2), Rauch |
| 0x21 | Kraehe | 10 / 0 | 60 / 60 / 60 (RE1.5 1000/100) | ja | RE2 GIB 0x80102CA0 + Federn 0x80104678/0x8010459C; RE1.5 GIB @0x801149c4 | GIB unter beiden KIs (w9/w15 schon drin) | unveraendert (Teile nach Original-Ablauf) — offen, s. N1.8 |
| 0x25 | Spinne | 99..132 / 76..136 | RE2 60 / 60 / 60; RE1.5 1000 / 100 | **RE2: nein (Rest 39-72)**; RE1.5: HG ja, WE bei HP < 100 | RE1.5 Platzer 0x80113ff8; RE2 FUN_80104890; Video B 5:18.3 (Schuss) | RE2: platzt erst beim Tod; RE1.5: w9 platzt, w15 nur Blut | RE1.5: auch w15 platzt; RE2 unveraendert |
| 0x26 | RE2-Babyspinne / RE1.5-Feueremitter | 1 / — | 60 | Baby ja | keine | Baby-Tod | unveraendert, keine Quelle |
| 0x27 | Gorilla | 360 / 360 | 1000 / 100 | HG ja; **WE nein (Rest 260)** | keine (nur ESP-Blut) | Absturz-Tod | unveraendert, keine Quelle |
| 0x29 | kleiner Affe ("Kakerlake" im alten Test) | 81..121 | 1000 / 100 | HG ja; WE nur HP < 100 | keine; Video 4080 (G6/O2): ueberlebt, zerfaellt nicht | Explosionstod Clip | unveraendert, keine Quelle |
| 0x23 | Alligator | 300 | 1000 / 100 | HG ja; **WE nein (Rest 200)** | keine (Kiefer-Platzen nur Boss) | 0x8010ea30 | unveraendert, keine Quelle |
| 0x23 (2090) | Gator-Boss | 25300 | 1000 / 100 | **nein** | Kiefer platzt nur beim Kanister-Tod | Treffer absorbiert | unveraendert (Boss) |
| 0x2B | Tyrant | 86..126 | 1000 / 100 | HG ja; WE nur HP < 100 | keine | 0x80114cb0 | unveraendert, keine Quelle |
| 0x30/0x36 | Birkin G1/G-Form | 300 | 1000 / 100 | HG ja (ausser Mutation); **WE nein (Rest 200)** | keine | 0x8011a3f0 | unveraendert (Boss) |
| 0x36 (5090) | G5 | 600 | 80 / 80 | **nein (Rest 520)** | keine | — | unveraendert (Boss) |
| 0x1A | Zellenarm | 250 | 1000 / 60 | HG: Tod-Zustand, setzt HP auf 250 zurueck; **WE nein** | RE1.5 "Hand ab" 0x8010D5D0 unerreichbar | nur Blut | unveraendert |
| 0x37, 0x40-0x4D | Tentakel, NPCs | -1 | — | kein Kandidat | — | — | — |
| 0x24, 0x2D, 0x22 | FX-Emitter, Efeu, Stub | — | nicht geprueft | — | — | — | — |

**Typen, die an EINER Granate nicht sterben (Schaden NICHT geaendert, zur Entscheidung):** Werfer-Explosivrunde an 0x11 (Rest 50
bzw. 200), 0x16 (bis 63 bzw. 93), allen Zombies bei reiner Explosion K1 = 50 (stirbt nur, wer weniger als 50 HP hat), Hund
(RE2, Explosion K1 50: Rest 19-82), Gorilla 0x27, Alligator 0x23, Birkin; **die Spinne unter RE2-KI an der Handgranate UND am
Werfer (60 gegen 99..132)**; G5, Gator-Boss, Zellenarm (setzt HP zurueck).

### N1.4 Umsetzung (GESTALTUNG / PORT-WAHL; jeder Video-Wert mit Datei + Zeit im Code-Kommentar)

**Zombie (RE2-Gehirn `re2z_granate_zerfetzen`, RE1.5-Gehirn `z_granate` -> `re15_re2z_granate_teile_r15`; enemy_ai_re2_zombie.c,
zombiegore_r15.c).** Ausloeser Zeile 9 (w9 Handgranate, w15 Werfer-Explosivrunde) unter beiden KIs, einmal je Tod.
- **Sieben Stuecke fliegen**: Rumpf 0, Becken 1, Kopf 8, Oberarme 9/12 (mit Unterarm + Hand), Oberschenkel 2/5 (mit Unterschenkel +
  Fuss) — RE2-Wurf mit Aufschlag FUN_80028AD8, Flagwort `ori 0x1062` (@0x80108E5C / @0x8010537C-80), Felder wie @0x80108E6C-98.
  Bit 0 von Part 0 bleibt stehen (re15_re2z_gore_active liest es als INIT-Seed). Keine RE1.5-Scherben mehr am Zombie (die
  erste Messung mit Rumpf-Scherben FUN_80108d04 ergab riesige rote Dreiecke bis zur Decke — verworfen).
- **Hoehe/Zeit (PORT-WAHL, Video):** Schwerkraft GR_G = 40 je Bild, Zielhoehe des Scheitels GR_DECKE = 2400 + 0..510 ueber dem
  Boden (CD2 19,55-19,69 s Teile an der Tunneldecke), Steig-Geschwindigkeit sqrt(2 g Steighoehe) (PsyQ SquareRoot0) aus der
  WIRKLICHEN Lage im ersten Wurfbild (Zielhoehe in +0x9A abgelegt, das Drift-Feld liest ein 0x1062-Teil nie), mindestens 600;
  waagrecht 30 + (rand & 0x3f) je Bild (CD2 #577-#592 linke Teile bis an die Wand), Richtungen 4096/7 je Stueck + Zufall.
- **Liegen bleiben (Nutzer-Entscheid):** beim zweiten Aufschlag entfaellt fuer diese Teile der Kaskaden-Ausloeser
  `and v0,v0,-2` @0x80028CA0; st86 = 1 haelt die Physik an (`slti v0,v0,2` @0x80028B60) -> das Stueck liegt bis zum
  Raumwechsel (der Aktor bleibt als Leiche Zustand 7).
- **Boden am Teil + Wandtest** (bisher [PORT (1)]/[PORT (2)] offen, nur fuer diese Teile, Maske re2z_part_gr): Boden =
  RE1.5-Bodensonde FUN_8001c6e8 mit den Argumenten der RE1.5-Brocken-Routine 36 (`0, 8, 0x100` @0x800187dc-f8), Rueckfall
  Aktor-y, wenn die Sonde den eigenen Standboden nicht erklaert; Wand = Original-Zweig @0x80028CAC-D20 (Radius 100,
  `vx = -(vx >> 2)`, `vz = -(vz >> 2)`), RE2-Kollision FUN_8004C1BC -> re15_collision_box_blocked (feste Klasse 4, Band des
  Bodens unter dem Teil).
- Blut wie die erste Fassung (FUN_80109610 P0 Wolken + Fleischbrocken, 0x0800-Brocken + Wolke 8000, Fontaene 0x00021F40,
  Spreng-Russ FUN_8010640C) + Blut 2000 an jedem Teil beim Aufschlag (@0x80028C34-4C) + Spritzer am Boden **2 x 2 Ausloeser**
  (wie der RE1.5-Gore-Aufbau "0x2000 zweimal"; die erste Fassung nahm 2 x 16 = 96 Plaetze = der ganze ESP-Pool).
- **Rauch-Nachlauf (re15_esp.c):** das Original-Rauchkind 0x030B5800 (@0x80018698-a8) erneut an P = Leiche (y - 500), erstmals 13
  Takte nach dem Zerfetzen (CD2 #577 -> #590), dann alle 8 Takte, 6 Mal (Rauch im Video >= 1,4 s; Routine 31 allein zeigt
  ~0,2-0,3 s). Ein Nachlauf je Stelle (naeher als 1000 = derselbe).
- **Explosionskinder vor Blutteilchen:** gemessen (gr.log ROOM2000): die Kinder der Routine 31 im Zuender-2-/0-Bild
  (0x03195000/0x030B5400 @0x80018640/60, 0x030B5800 @0x800186c8) bekamen bei vollem Pool (96, `sltiu 0x60` @0x8001978c)
  keinen Platz (n = 0). Jetzt raeumt — wie fuer den Wurfkoerper (Runde 36 Spur D Punkt 2b) — ein Blutteilchen den Platz.
- RE1.5-Gehirn ohne RE2-Modellblock (RE15_RE15_RE2Z_IMPORT=0, nicht die Vorgabe): Glieder platzen (FUN_80108d04), Wurzel + Rumpf
  weg (Teil-Wort 0 @0x801081d0) — dort keine liegenden Teile (OFFEN, N1.8).

**Hund (enemy_ai_re2_dog.c `re15_re2d_granate_teile`, Aufruf im RE2-Tod-Router und im RE1.5-Tod `case 3`).** Dieselben 7 Parts
des RE2-Teile-Wurfs 0x80104440 (Tabelle @0x80105680, Farbe 0x00101040 @0x801044c8), aber mit dem Wurf 0x1062 statt Drift/
Scherben (0x4A @0x801044a4) — sie fliegen wie die Zombie-Stuecke und bleiben liegen; der Rumpf bleibt wie im RE2-Original. RE2-KI:
Zeile 9 zerfetzt mit 80%-Gore in jeder Spalte (Original bei +0x1D2 >= 3 nur der Kern @0x801046A8-D4). RE1.5-KI: +0x5 = 9 oder 15;
die Part-Bruecke (Spur S) ist fuer den RE1.5-Hund nach dem Wurf aktiv (re15_re2z_gore_active). Rauch-Nachlauf wie beim Zombie.

**Spinne (RE1.5-Gehirn, enemy_ai_common.c Spinnen-Tod):** +0x5 = 15 platzt wie die Handgranate (Tabelle @0x80118f38[9] =
0x80113ff8; Original [15] = 0x8011461c), danach +0x5 = 9 (Muster @0x80114b1c-20).

Alles haengt an `RE15_KEIN_80PROZENT_GORE=1` (aus = Stand vor Spur GR, Schaden bleibt 1000).

**Mess-Haken (kein Spielverhalten) `RE15_FORCE_GRANATE="<bild>:<slot>[,...]"`** (fx_plattform_pc.c): eine liegende Handgranate
zuendet am Gegner mit der Zuender-Folge der Routine 31 (Licht-Latch, Resolver, Kind 0x03195000, SE; 5 Takte spaeter
0x03195000 + 0x030B5400; 7 Takte spaeter 0x030B5800) — fuer Sichtlaeufe auf engen Kanalstegen, wo ein echter Wurf in der
Wandzelle zuendet (gemessen ROOM2000/2020: `EV wand ... explosion sofort`).

### N1.5 Messung im Port (echte exe, gdigrab am eigenen Fenster, ohne AUTOSHOT/SOFTWARE_RENDER)

Werkzeug `scratchpad/GR2/cap.ps1` (eigene PID, Fenster-Handle), Fluglog `RE15_GR_LOG=1` (`[gr-teil]` je Bild und Teil,
Auswertung `scratchpad/GR2/teil_check.py`).

- **ROOM1140, echter TIEF-Wurf** (Aufstellung integration_r35_granate, RE2-KI, RE1.5-Modelle): vier `[gr]`-Zeilen (0x10, 0x10,
  0x11, 0x11). Ablauf im Bild: Blitz (+0,0 s), Feuerball + Blutwolke (+0,1..+0,4 s), die Stuecke (blaue RPD-Teile) fliegen bis
  an den oberen Bildrand, schwarzer Rauch +0,5..+2,0 s (vorher ~0,3 s), danach liegen die Stuecke mit Blut am Boden, am Ort der
  Zombies steht nichts mehr. Fluglog: alle 28 Stuecke kommen nach 21-32 Bildern am Boden zur Ruhe (st 2 -> 1, y = Boden 0).
- **ROOM2000 (Kanal, Typ 0x16, liegender Zombie wie im Video)**, Kamera Cut 6 (`RE15_FORCE_CUT=6`), `RE15_FORCE_GRANATE=100:4`:
  zwei `[gr] ... Typ 0x16 ... ZERFETZT`, die Stuecke fliegen, Blutwolke; gr.log: Explosionskinder jetzt alle mit Platz
  (vorher n=0), Rauch-Nachlauf 6 Kinder je Leiche. ⛔ In ROOM2000 werden die RE1.5-Explosionseffekte (Kind 0x03195000 /
  Rauch 0x030B5800) NICHT gezeichnet, obwohl sie Plaetze bekommen — auch beim echten Wurf; in ROOM1140/1230 sind sie da.
  Ursache offen (vermutlich Textur/Bank im Kanal-Raum), N1.8.
- **ROOM1230, Hund (RE2-KI)**, Cut 6, `RE15_FORCE_GRANATE=45:1`: `[gr] Hund Slot 1 Waffe 9: ZERFETZT (Teile-Maske 0x079c)`;
  Blitz, Feuerball, ein Bein fliegt hoch, Rauch +0,5..+1,7 s, danach rote Teile und Lache am Boden, nach 12 s unveraendert.
  Fluglog: 7 Parts, Scheitel 2349..2672 ueber dem Boden, Ruhe nach 27-30 Bildern. Bild
  `GR_belege/n1/n1_port_hund_1230_explosivgranate.jpg`.

**Vergleichsbilder (Bildleiste Video oben / Port unten, Zeit relativ zur Detonation, gdigrab 30 B/s am echten Fenster):**
- `GR_belege/n1/n1_vergleich_video_cd2_gegen_port_1140.jpg` — ROOM1140, echter Wurf. Gleich: Blutwolke ueber den Koerpern
  ab +0,03 s, Stuecke steigen bis zum oberen Bildrand +0,1..+0,43 s, schwarzer Rauch ab +0,43 s bis ueber +1,77 s, am Ort kein
  Koerper mehr. Anders (benannt): der Blitz ist im Port die RE1.5-Explosion (weiss -> ORANGE Feuerball, Original-Kind
  0x03195000 @0x800185dc), im Video weiss-gruenlicher Qualm ohne Orange; die Stuecke sind im Port blaue RPD-Teile (Typ
  0x10/0x11), im Video khaki (0x16); die dunklen Wandflecken fehlen (N1.8). Letzte Kachel +8 s: die Stuecke liegen.
- `GR_belege/n1/n1_port_1140_vorher_nachher_liegen.jpg` — vorher vier Zombies, +11 s nur noch verstreute Stuecke in Blut.
- `GR_belege/n1/n1_vergleich_video_cd2_gegen_port_2000_kanal_0x16.jpg` — derselbe Typ 0x16 wie im Video, liegend, im Kanal.
  Das RE1.5-Modell 0x16 der Mod ist weiss-grau (nicht khaki). Die Stuecke steigen ueber den Bildausschnitt der hohen Kamera und
  landen im dunklen Kanal; Explosions- und Rauch-Sprites fehlen in diesem Raum (N1.8).
- `GR_belege/n1/n1_port_hund_1230_explosivgranate.jpg` — Hund RE2-KI (kein Video-Vorbild).
- `GR_belege/n1/n1_port_hund_1230_re15ki.jpg` — Hund RE1.5-KI (`RE15_AI_FLAVOR=re15`): `[gr] Hund Slot 1 Waffe 9: ZERFETZT`,
  Fluglog 7 Parts, Scheitel 2229..2690, Ruhe nach 25-28 Bildern; Explosion, Rauch, danach liegen die Teile in Blut (+7,8 s).
- Pruefpunkte aus dem Auftrag: kein heiler Koerper bleibt stehen (alle drei Laeufe), nichts haengt in der Luft (Fluglog: jedes
  der 7 Parts je Gegner endet mit st 2 -> 1 auf y = Boden; Unit [2]/[3]/[5]), keine Teile durch Waende (Wandtest; im Bild keine
  Teile in Wand-/Moebel-Flaechen), kein Flackern (Folgebilder 30 B/s ohne Springen; die gezeichneten Stuecke bleiben ueber 11 s
  unveraendert).

### N1.6 Tests

- `unit_r41_granate` neu: [2] RE2-Gehirn, sechs Typen (0x10/0x11/0x16 aus ROOM1140, umgetypt 0x12/0x13/0x18): Zerfetzung
  einmal, die sieben Stuecke tragen 0x1062 + Liegen-Maske 0x1327, Kinder bleiben dran, keine Scherben, Sturz 0x80108530,
  Leiche, Zeichengang 150 Bilder: alle Stuecke liegen am Boden, Scheitel gegen die Zielhoehe (max(Ziel, Start + 600)
  -350..+50, mindestens 1800), Ruhe <= 45 Bilder, >= 5/7 gestreut, alle 15 Teile weiter gezeichnet, Rauch-Nachlauf 6;
  [3] RE1.5-Gehirn dieselben Stuecke, liegen, Leiche, kein Kriecher; [5] Hund RE2-KI und RE1.5-KI (ROOM11D0): 7 Parts
  0x1062 + Farbe 0x101040, Maske 0x079C, Bruecke aktiv, liegen. [1]/[4] unveraendert (Schaden, Schalter aus).
- `unit_zombie_gore` Granate: w9 UND w15 (RE1.5-Gehirn), RE2-Block bzw. Teilschicht je nach Import-Option.
- `unit_r34_reaktion` 121/122/152/220/221 schalterabhaengig; NEU `unit_r34_reaktion_ohne80` (RE15_KEIN_80PROZENT_GORE=1)
  pinnt das RE2-Original (Hund Zeile 9: 0x4A-Drift, 1D2 >= 3 nur Kern; Zombie 8BEC/9610, Spalte 0 Kriecher 0x80107438).
- Gelaufen (Stand f95dd987a): volle Unit-/Prop-/Probe-Suite `ctest -E ^integration_ -j 6` **1077 Tests, 1074 im ersten Lauf
  gruen**; die drei uebrigen (unit_r39_titelversion_cmake "git fehlt", unit_r38_releaseskript_fehler_umfeld Timeout 1200 s,
  unit_r35_android_bash_mutanten) lagen an meinem Mess-PATH ohne git bzw. an der Last und sind im Wiederholungslauf mit git im
  PATH **3/3 gruen** -> **1077/1077**. Granaten-Integration integration_r34_granaten / integration_r35_granate /
  integration_r36_granate **3/3 gruen** (359 / 212 / 186 s).

### N1.7 Fuer den Nutzer (Kurzfassung N1)

1. **Video:** Ja — CD2 16,95-17,9 s holt Leon aus und wirft, Detonation 19,253 s am Khaki-Zombie, der ueber der Stegkante
   liegt (Kanal, Typ 0x16). Rote Wolke, khakifarbene Teile fliegen bis an die Tunneldecke (Scheitel nach 0,3-0,4 s, ein Teil
   landet nach 0,77 s, ein letztes faellt noch nach 1,3 s), weiss-gruenlicher Blitz 0,3 s, schwarze Rauchsaeule ab 0,43 s bis
   zum Schnitt (>= 1,4 s), dunkle Flecken an der Wand, rote Spritzer auf den Stegen. Am Ort des Zombies bleibt nichts stehen; wo
   die Teile landen, zeigt das Video nicht. Die Spinne bei 14,6 s platzt nach Schuessen. Es ist der einzige Handgranatenwurf im
   ganzen 80%-Material; die anderen Explosionen sind Elzas Granatwerfer.
2. **Port jetzt:** Explosivgranate (Handgranate UND Werfer-Explosivrunde, RE2-KI UND RE1.5-KI) — der Zombie zerfliegt in
   sieben Stuecke (Rumpf, Becken, Kopf, beide Arme, beide Beine), die bis ~2,4-2,9 m hoch fliegen und dann verstreut am Boden in
   Blut LIEGEN BLEIBEN (bis zum Raumwechsel); schwarzer Rauch steht jetzt ~1,6 s; Teile prallen an Waenden ab und fallen auf den
   Boden unter sich (auch in den Kanal). Der Hund wirft bei Granaten-Tod seine 7 RE2-Teile, die ebenfalls liegen bleiben. Die
   RE1.5-Spinne platzt auch am Werfer.
3. **Nicht geaendert (zur Entscheidung):** Wer an EINER Granate nicht stirbt (N1.3): Werfer-Explosivrunde an dickem Zombie
   0x11 und 0x16, reine Werfer-Explosion (50) an allen Zombies mit mehr als 50 HP, RE2-Hund an der Werfer-Explosion, die
   Spinne unter RE2-KI (60 Schaden gegen 99-132 HP, Handgranate und Werfer), Gorilla, Alligator, Birkin, G5, Gator-Boss,
   Zellenarm.

### N1.8 Offen

1. Dunkle Flecken an Wand/Decke (CD2 19,75-21,1 s): der Port hat keine Wand-Abziehbilder (Spur S Z28) — nicht gebaut.
2. Der Blitz: Port = RE1.5-Explosion weiss -> orange (Original-Kind der Disc), Video weiss-gruenlich ohne Orange — nicht
   angepasst (Original-Effekt; die 80%-Fassung hat offenbar einen anderen Explosions-Effekt).
3. In ROOM2000 (Kanal) werden die Explosions- und Rauch-Sprites (Kinder 0x03195000/0x030B5400/0x030B5800) nicht gezeichnet,
   obwohl sie Plaetze bekommen (gr.log n=1) — in ROOM1140/1230 sind sie da. Ursache nicht untersucht (eigene Spur).
4. RE1.5-Gehirn OHNE RE2-Modellblock (RE15_RE15_RE2Z_IMPORT=0): Glieder platzen in Scherben, keine liegenden Teile (die RE1.5-
   Flugphysik FUN_8001f024 laesst ein ruhendes Teil bei y == 0 verschwinden, @0x8001f104-134).
5. Kraehe: der GIB laeuft wie bisher (RE2 0x80102CA0 / RE1.5 @0x801149c4), ihre Teile verschwinden nach dem Original-Ablauf —
   nicht auf "liegen bleiben" umgestellt (Federn/Fetzen; Entscheidung des Nutzers offen).
6. Spinne unter RE2-KI stirbt an keiner Granate (Schaden nicht eigenmaechtig geaendert, N1.3).
7. Das rote/orange Glimmen in der Kanalrinne CD2 15,6-17,4 s: Ursache vor dem Schnitt, nicht im Material.
8. Elza: figurunabhaengig (kein Figurbezug im Pfad, wie §6.4) — kein eigener Sichtlauf in N1.

---

# ERSTE FASSUNG (Stand 1cfbcb074) — Videobefunde in §2.1/§2.2, §5 "Rumpf bleibt", §8.2, §9.4 sind durch N1 BERICHTIGT

## 0. Auftrag (Nutzer, woertlich, 2026-11-02)

> Und die Granate sollte laut den Videos der 80% Version die Zombies zerfetzen, also die normale, richtige
> Handgranate. Ausserdem ist sie noch zu schwach, die sollte auch die dicken Zombies zerfetzen

Gore-Regel (bindend): Gore = Maximum aus RE1.5-Disc, RE2-Retail und 80%-Videos. Gestaltung = PORT-WAHL,
jeder Einzelmechanismus mit @0x-Beleg.

## 1. Welches Item

* "normale, richtige Handgranate" = **Item 0x09 Hand Grenade** (Namensleser FUN_80028840, Runde 30;
  BAUPLAN Runde 34 E1). Die anderen beiden Granaten sind 0x0A Acid / 0x0B Incendiary. Der Granatwerfer ist
  die Waffe 0x0F (RE2-Explosivrunde, `SOUND/ARMS0F`).
* Explosion: ESP-Routine 31 @0x8001854c, Zuender 7 @0x8001856c, Resolver `FUN_80012d60(500, &P, 2)`
  (`ori a0,zero,0x1f4` @0x80018598, `addiu v0,v0,-500` @0x800185a8, a2 = 2 @0x800185b4).
  Art 2 -> Schaden DAT_8006f418[2] = 1000 (@0x8006f41c), Waffen-Id DAT_8006f430[2] = 9 (@0x8006f432).
* Port: Reichweite/Kandidaten RE2 Op 47 (`granate_r35.c re15_granate_r35_explosion`), Anwendung RE1.5-Gegnerzweig
  (`re15_resolver_gegnerzweig`, re15_damage.c), Stempel `re2_gl_explosion_stempel` (Zombie-Familie: Zeile 9,
  K0, Zone 1 = Spalte 1 -> DEATH[9][1] = 0x80108BEC Zerreissen, Runde 35 Spur A §3.5).

## 2. Video-Befund (80%-Material)

Material: B = `Version_80%/Resident Evil 1.5 (All Footage).mp4` (320x240, 30 B/s), A = `Version_80%/Biohazard 1.5
Final Build.mp4` (Diashow), CD1-4 = `documentation/Biohazard 1.5 Complete Disc Video N of 4 (360p).mp4`
(laut Spur S §1 ausser Vorspann/Abspann vollstaendig in B enthalten). Ausgangspunkt: die Gore-Sichtung der
Spur S (`S_video_gore.md` §2.1, Z08-Z11, Z14, Z21-Z24), hier jede Explosionsstelle neu am Einzelbild auf die
WAFFE geprueft (Werkzeug `scratchpad/GR/strip.py`, 5-30 B/s, Ausschnitte 2-2,5-fach).

### 2.1 ⛔ BERICHTIGT (N1.0): ~~Es gibt im 80%-Material keinen Handgranaten-Wurf~~ — es gibt einen (CD2 / B 5:20, Leon). Die Elza-Stellen sind Granatwerfer (bestaetigt)

| Stelle | Was man sieht | Waffe | Beleg |
|---|---|---|---|
| B 3:30.8-3:31.8 (3000 Cut 2) | Elza steht hinten, Waffe an der Huefte; **Muendungsblitz 3:30.9-3:31.0** an ihrem Rumpf; 0,3 s spaeter (3:31.3-3:31.4) platzt der Vordergrund-Zombie (weisses Hemd) zur roten Wolke mit Feuer, ab 3:31.6 ist er weg, am Boden Brocken/Blut | Granatwerfer (Schuss, kein Wurf, kein Bogen) | `GR_belege/video/v01_B0330_elza_muendungsblitz_dann_zombie_zerplatzt_300.jpg` |
| B 3:35.5-3:37.2 (3010 Draufsicht) | 3:35.5 Elza mit langer Waffe im Anschlag; 3:36.0 Blitz bei Elza, der RPD-Zombie vor ihr wird getroffen; 3:36.1-3:36.2 rote Saeule/Wolke ueber dem ganzen Koerper; ab 3:36.3 **ist der Zombie weg**, es bleiben eine grosse rote Lache und ein grauer Brocken | Granatwerfer | `v02_B0335_3010_elza_schuss_zombie_weg_lache.jpg` |
| B 8:05.2-8:08.0 (1070 Cut 3) | Muendungsblitz an Elzas Huefte 8:05.4 und 8:06.6, jeweils 0,2 s spaeter Explosion am Zombie vor der Kamera | Granatwerfer | `v03_B0805_1070_elza_muendungsblitz_explosion.jpg` |
| B 8:40.5-8:44 (1070 Cut 7 Draufsicht) | Elza zielt waagrecht mit langer Waffe, weisser Blitz an der Muendung 8:42.1; Explosion am Zombie in der Lache 8:42.1-8:42.3; danach **nur noch ein roter Fleischhaufen in einer grossen runden Lache** | Granatwerfer | `v04_B0840_1070_elza_schuss_haufen_in_lache.jpg` |
| B 5:18-5:23.2 (Tunnel, nicht auf der Disc) | Der Khaki-Zombie haengt ueber der Bruestung, 5:23.0 rote Wolke + weisse Wolke + helle Brocken, danach ist er weg | Schuetze nicht im Bild, Waffe offen | `v05_B0518_tunnel_zombie_zerfetzt_schuetze_unsichtbar.jpg`, Spur S g08 |
| B 7:50.8-7:51.6 (1030 blau) | orange Funken/Brocken am Zombie | offen (kein Schuetze im Bild) | Spur S g30 |
| A 0:00-2:07 | Diashow-Standbilder, keine Wurfpose, keine Granate in der Hand | – | `scratchpad/GR/a_*.jpg` (nicht im Repo) |

Kennzeichen eines Handgranaten-Wurfs (Port/RE1.5: Wurfclip W09 Clip 7/9/11, 35 Bilder, Flug mit Taumel-Sprite,
Abpraller, Explosion erst 36 Bilder nach dem Liegen = >= 1,2 s nach dem Aufschlag; BAUPLAN Runde 34 §1.1) kommt
an keiner Stelle vor. Alle Explosionen mit sichtbarem Schuetzen haben Muendungsblitz an der Huefte und
Einschlag 0,2-0,5 s spaeter ohne Bogen = Granatwerfer.

**Folgerung:** Das Video belegt, was eine **Explosivgranate** mit einem Zombie macht (Koerper verschwindet in
roter Saeule/Wolke, Lache + Brocken/Fleischhaufen bleiben). Dass die **Handgranate** dasselbe tut, ist die
Uebertragung nach der Gore-Regel des Nutzers (selbe Sprengladung, Gore = Maximum) — **PORT-WAHL**, nicht Video.
Ein dicker Zombie (Typ 0x11/EM11) wird im Video an keiner Explosionsstelle getroffen (3000/3010/1070: RPD- und
Hemd-Zombies). "Auch die dicken zerfetzen" ist damit Nutzer-Vorgabe, kein Videobefund.

### 2.2 Was das Video beim Zerfetzen zeigt (Soll-Bild)

1. Einschlag: rote Wolke/Saeule ueber dem GANZEN Koerper (3:31.4, 3:36.1-2, 8:42.3), mit Feuer/oranger Wolke.
2. Der Koerper ist danach weg (3:31.6, 3:36.3, 5:23.2) bzw. nur noch ein roter Haufen (8:42.5ff).
3. Es bleiben: grosse rote Lache (3:36.3ff, 8:42.5ff), ein grauer Brocken (3:36.4), Blut/Reste am Boden (3:31.8).
4. Einzelne fliegende Teile sind bei 320x240 als helle Brocken zu sehen (5:23.0-5:23.2), Gliedmassen als
   solche nicht unterscheidbar.

## 3. Ist-Stand VORHER (gemessen, echte exe, Messkopie `re15_pc_r41GR_vorher.exe` = Basis 2f886e91a)

Lauf `v1_leon_vorher` (Werkzeug `scratchpad/GR/cap.ps1`: eigene PID, gdigrab am Fenster-Handle, beschleunigter Renderer
direct3d, ohne AUTOSHOT/SOFTWARE_RENDER, 10 Bilder/s, `RE15_WINDOW_SCALE=2`). Echter Titel + echte Spielerwahl
(`RE15_TITLE_CONFIRM_MS=1500`, `RE15_PSELECT_AUTO`), dann `RE15_DEBUG_JUMP=1140@250`, Leon (-1676,-18070) Blick 1076,
Standard-KI (`RE15_AI_FLAVOR=re2`), Modelle wie in der Nutzer-Sitzung (Hybrid, `RE15_AI_MODELS=re15`, ZA-Dossier §6),
Handgranate (`RE15_GIVE=9:5`, `RE15_EQUIP=9`), TIEF-Wurf `MD0.6,MDA0.2,MD2.5,W5` (Aufstellung integration_r35_granate).

`state.log` (Bild nach dem Sprung), Explosion F119 (gr.log T=365, `resolver art=2 ... r=2000 eingriffe=4`):

| Slot | Typ | HP vor | HP nach | Zustand | Ausgang |
|---|---|---|---|---|---|
| 2 | 0x10 | 50 | -150 | 3 (Tod) | 0x80108BEC Zerreissen bzw. 0x80109610 Wegschleudern, Leiche F180 |
| 3 | 0x10 | 80 | -120 | 3 (Tod) | dito, Leiche bis F400 |
| 4 | **0x11 dick** | 250 | **50** | **2 (HURT)** | **ueberlebt**, F400 noch HP 50, greift weiter an |
| 5 | **0x11 dick** | 250 | **50** | **2 (HURT)** | **ueberlebt** (stirbt spaeter an anderem) |

Schaden = 200 = RE2-Granatwerfer-Zeile 9 Klammer 0 (`0x00a0c8c8` @0x800A41CC), seit Runde 34 E4 als Ersatz fuer die
RE1.5-Zahl. Brad/dicker Zombie 0x11 hat RE2-HP 250 (`addiu v1,zero,250` @0x801008C8) -> ueberlebt jeden Wurf mit 50.
Typ 0x16 bekommt sogar nur 80 (@0x800A4348) bei HP bis 143 -> ueberlebt meist ebenfalls.
Bild: `GR_belege/port/p01_leon_1140_wurf_vorher_nachher.jpg` (obere Zeile) — die zwei dicken Zombies (weisse Hemden) stehen nach der Explosion
weiter und gehen auf Leon los; die zwei normalen fallen (Sturz bzw. Zerreissen, Rumpf ganz).

## 4. Staerke — Herleitung (PORT-WAHL)

Quellen (alle selbst gelesen):
* RE1.5-Handgranate: `FUN_80012d60(500, &P, 2)` (`ori a0,zero,0x1f4` @0x80018598, `ori a2,zero,0x2` @0x800185b4),
  Art 2 -> `DAT_8006f418[2] = 1000` (`e8 03` @0x8006f41c). Gleich in `info/Re1.5/PSX.EXE` und `Version_40%/PSX.EXE`
  (eigener Vergleich: Tabelle `10,20,1000,1000,1000,50,100,200,300,1000,0`, Reaktionstabelle @0x8006f430 `3,3,9,10,...`).
* RE2: keine Handgranate (BAUPLAN Runde 34 E1, Musterscan). Die Zeile 9 ist die Granatwerfer-Explosivrunde
  (Zombie K0 200 / K1 50 / K2 10 @0x800A41CC; 0x16: 80 @0x800A4348). Staerkster RE2-Explosivwert am Zombie: Rakete
  Zeile 17 = 900 (`0x384E1384` @0x800A426C).
* HP: RE2-Zombie max 128 (@0x8010C670) + 15 (`sltiu ..,0x4` @0x801007FC / @0x80100810) = 143; **dicker Zombie 0x11 =
  Brad 250** (@0x801008C8). RE1.5-HP (`0x8011f034 + Typ*0x20`, STAGE1.BIN, Disc = Mod): 0x10 max 101, **0x11 max 107**,
  0x12 max 111, 0x13 max 88, 0x16 max 99, **0x18 einmal 1058** (Index 3 @0x8011f33a, sonst max 109).
* Video: ein Treffer = Zombie zerfetzt (Abschnitt 2).

**PORT-WAHL:** Die Handgranate (Art 2) traegt an der Zombie-Familie (re15_re2z_owns_type 0x10/0x11/0x12/0x13/0x16/0x18)
wieder ihre eigene Zahl **1000** (@0x8006f41c) statt der RE2-Granatwerfer-Zeile. Begruendung: Die Zeile 9 war eine
Zuordnung (RE2 kennt keine Handgranate), die 1000 ist der Wert des Gegenstands im Original; sie liegt ueber jeder
RE2-HP (max 250) und jeder RE1.5-HP ausser dem Einzelwert 1058 von 0x18 (nur ohne RE2-Modell, d. h.
`RE15_RE15_RE2Z_IMPORT=0` + RE1.5-KI; dort ueberlebt dieser eine Zombie mit 58, wie im Original). Die Regel
"HP und Schaden nur zusammen" (E4) wird damit fuer genau diesen Fall bewusst durchbrochen (Nutzer-Auftrag "zu schwach").
Nicht geaendert: Saeure/Brand (Art 3/4, E4 = 200), Werfer-Arten 6..9, alle Nicht-Zombies (Hund 300, Kraehe 60,
Spinne 60, G5 80 nach E16, Gorilla/Kakerlake/Alligator/Birkin 1000 RE1.5, Gator-Boss Modul).
Code: `re15_damage.c re15_enemy_take_damage_at` (Block "RUNDE 41 SPUR GR").

## 5. Zerfetzen — Umsetzung (GESTALTUNG / PORT-WAHL, Einzelmechanismen mit @0x)

Keine der drei Quellen nimmt dem Zombie alles auf einmal (RE2 Zeile 9 DEATH[9][1..2] = 0x80108BEC: Kopf + Arm(e),
Rumpf bleibt; RE1.5 0x80107ee0 ZERFETZEN: Kopf + Arme; RE1.5 0x80108abc BEINE WEG: Beine). Das Video zeigt aber den
ganzen Koerper weg und Reste in der Lache. Gebaut ist die Vereinigung, je Teil ein belegter Mechanismus (Code-Block
"RUNDE 41 SPUR GR" ueber `re2z_granate_zerfetzen`, enemy_ai_re2_zombie.c; RE2-Slots nach den Parser-Bindlaengen):

| Teil (RE2-Slot) | Was | Mechanismus / Beleg |
|---|---|---|
| Kopf 8 | fliegt weg, Halsfontaene | Drift 0x4A, Felder 0x80108BEC @0x80108DD4-E3C, Effekt 3000 @0x80108E18; Fontaene 0x00021F40 @0x80109428-38 |
| Oberarme 9/12 | fliegen (beide) | Wurf 0x1062, Felder @0x80108E54-E98 / @0x80108F10-F70 (RE2 Arm B nur 50 % @0x80108F04-08 -> hier immer) |
| Unterarme 10/13 | platzen in Scherben | RE1.5 FUN_80108d04 (@0x80108d04-6c: Blut 0x2000, +0x68 = 0x8f, Wort |= 0x4a), wie ZERFETZEN @0x8010818c/@0x80108224 |
| Haende 11/14 | weg | RE1.5 Teilwort 0 @0x801081d0 / @0x8010826c |
| Oberschenkel 2/5 | fliegen | Wurf 0x1062 mit den RE2-Beinwurf-Feldern @0x80105384-C0, Effekt 7000 @0x801053CC |
| Unterschenkel/Fuesse 3/4/6/7 | platzen | RE1.5 BEINE WEG @0x80108ba0 / @0x80108bb8 / @0x80108bd0 / @0x80108be8 (RE1.5-Teile 2/3/5/6 -> RE2 3/4/6/7, re2_hybrid_perm) |
| Rumpf 0 + Becken 1 | bleiben liegen | Spreng-Russ FUN_8010640C (Zeile 9, @0x80108CEC; Flammen weiter aus, Runde 36 D), Stumpf-Marker Part 0 @0x80108F98-B4, Rumpf-Stumpf Record 16 @0x80108FB8-FDC (nur RE2-MD1) |
| Blut/Brocken | Wolken, Fleischbrocken, Lache | FUN_80109610 P0 (8096/6096, n = (rand&3)+1 Brocken 0x0902xxxx @0x801096E8-0x801097B8), Brocken 0x0800xxxx + Wolke 8000 @0x80108FF0-0x80109054, grosse Lache (2 x 16 Spritzer, wie der fruehere Zeile-9-Zusatz) |
| danach | Sturz zur Leiche | 0x80108530 (`re2z_death_main`, wie re2z_80_zerfetzen) -> Zustand 7 |

* Ausloeser: DEATH-Wurzel mit Zeile 9 (Handgranate w9 und Granatwerfer-Explosivrunde w15, `re2z_row_from_weapon`), in
  jedem Zweig (stehend, liegend, Kriecher), genau einmal (Latch `zg_9f` Bit 0x08). Der Kriecher-Wiederbeleber
  0x80107438 (Spalte 0) wird damit fuer Zeile 9 nicht mehr erreicht — die Handgranate stempelt ohnehin Spalte 1.
* Farbwort der platzenden Teile: RE1.5 +0x68 = 0x8f; bei aktiver RE2-Bruecke nimmt der Zeichner das RE2-Farbwort
  +0x70 (main.c `gore_on`), deshalb steht dort derselbe Wert (Port-Mapping, beide multiplizieren das NCCT-Ergebnis).
* RE1.5-Gehirn (Flavor re15, `zombiegore_r15.c z_granate`): w9 -> ZERFETZEN (@0x8010800c-0x8010826c, ohne den
  Muenzwurf ZERFETZEN/OBERKOERPER @0x801080c8-d4) + BEINE WEG (@0x80108ba0-c50), Leiche statt Kriecher. Saeure/Brand
  (w10/w11) behalten die fruehere Gestaltung (Oberkoerper ab, kriecht weiter) — nicht Teil dieses Auftrags.
* Schalter wie alle 80%-Gestaltungen: `RE15_KEIN_80PROZENT_GORE=1` -> RE2 0x80108BEC wie vorher. Die Staerke (1000)
  haengt NICHT am Schalter.
* Nicht gebaut / nicht erfunden: das Verschwinden des Rumpfes (keine Quelle hat es; der Rumpf bleibt als Brocken in der
  Lache — Video 8:42.5 zeigt genau so einen Haufen). Die fliegenden RE2-Teile verschwinden nach dem zweiten
  Bodenkontakt (Kaskade @0x80028CA0), die RE1.5-Scherben nach 0x1d Bildern (@0x8001f208-214) — beides Original.

## 6. Messung NACHHER (echte exe, Messkopie `re15_pc_r41GR_nachher.exe` = Stand dieses Commits)

### 6.1 Leon, ROOM1140, echter TIEF-Wurf (gleiche Aufstellung wie §3, Lauf `v1_leon_nachher`)

`state.log` nach dem Sprung, Explosion F119 (gr.log `resolver art=2 ... eingriffe=4`), `debug.log` (RE15_GR_LOG=1):

| Slot | Typ | HP vor -> nach | F120 | F180 | debug.log |
|---|---|---|---|---|---|
| 2 | 0x10 | 50 -> -950 | Zustand 3, Sturz (mo 2) | 7 Leiche | `[gr] RE2 Slot 2 Typ 0x10 Zeile 9: ZERFETZT (HP -950)` |
| 3 | 0x10 | 80 -> -920 | 3, Sturz (mo 1) | 7 | `... Slot 3 Typ 0x10 ... ZERFETZT (HP -920)` |
| 4 | **0x11 dick** | **250 -> -750** | 3, Sturz | **7 Leiche** | `... Slot 4 Typ 0x11 ... ZERFETZT (HP -750)` |
| 5 | **0x11 dick** | **250 -> -750** | 3, Sturz | **7 Leiche** | `... Slot 5 Typ 0x11 ... ZERFETZT (HP -750)` |

Slot 1 (0x16, liegt seit Raumstart in seiner Lache, g=88) liegt ausserhalb von Band/Tor und bleibt unberuehrt —
wie vorher. Bild (gdigrab, eigene PID): `GR_belege/port/p01_leon_1140_wurf_vorher_nachher.jpg` untere Zeile —
Explosion, rote Scherben fliegen strahlenfoermig, Arme/Beine/Kopf fliegen weg, ab ~1 s liegen nur noch Rumpfe in
grossen Lachen; die dicken Zombies stehen nicht mehr auf.

### 6.2 Explosion direkt am dicken Zombie (Messhaken `RE15_FORCE_EXPLOSION=2@100:4`, Slot 4 = 0x11)

`[harness] RE15_FORCE_EXPLOSION F100 art=2 slot=4 t=11 P=(-48,-500,-20878) Treffer=4`.
Vorher (`v2_dick_vorher`): beide 0x11 bleiben stehen und packen Leon (Bilder #303-#324). Nachher (`v2_dick_nachher`):
alle vier zerfetzt (vier `[gr]`-Zeilen, HP -950/-920/-750/-750). Bild: `GR_belege/port/p02_leon_1140_dicke_zombies_vorher_nachher.jpg`.

### 6.3 Video gegen Port

`GR_belege/vergleich_video_80_gegen_port_nachher.jpg`: oben 80%-Video B 3:35.9-3:36.8 (3010) und 8:42.1-8:43.5 (1070),
unten Port nachher. Gleich: Einschlag als rote Wolke ueber dem ganzen Koerper, der Koerper ist danach nicht mehr als
Figur da, es bleibt ein Rest in einer grossen Lache. Anders (benannt): im Port fliegen sichtbare Glieder und rote
Scherben (RE2-Wurf + RE1.5-Scherben), das Video zeigt bei 320x240 nur Wolke und Brocken; der Rumpf bleibt im Port als
erkennbarer Torso liegen (RE2-Stumpf-Mesh 16 nur mit RE2-Modellen; mit den RE1.5-Modellen der Nutzer-Sitzung der
Original-Torso, Grenze (1) im Code-Kopf von 0x80108BEC).

### 6.4 Elza

* Gleicher Weg wie Leon: Waffen-Id 9 und Art 2, kein Figurenbezug im Schadens- und Todespfad (re15_damage.c,
  enemy_ai_re2_zombie.c, zombiegore_r15.c lesen keine Figur); die Wurfbank PL04W09 hat dieselben Cliplaengen
  (Runde 36 D §6.8).
* Gemessen (echte exe, echte Spielerwahl Elza `RE15_PSELECT_AUTO_SWITCH`, `RE15_DEBUG_JUMP=1041@250`, Elza
  (-23500,-1000) Blick 3259, Wurf ab Bild 60): `resolver art=2 P=(-24194,-729,-1190) r=2000 eingriffe=2`,
  `[gr] RE2 Slot 1 Typ 0x16 ... ZERFETZT (HP -921)`, `[gr] RE2 Slot 2 Typ 0x16 ... ZERFETZT (HP -950)`, beide ab
  Bild 180 Leiche (Zustand 7). Vorher derselbe Lauf: beide sterben (HP 79/50 - 80 = -1/-30) ohne Zerfetzung.
  Die Granate zuendet hier ueber Elza selbst (Wandzelle im Wurfbild, Runde 35 N1.2 Wurfbild-Regel), kein Spielerzweig.
  Bild `GR_belege/port/p03_elza_1041_wurf_vorher_nachher.jpg` + `p03_elza_1041_log_auszug.txt`: die Kamera dieses
  Schnitts zeigt nur Elza am Rand und die roten Scherben (#294-#306), die Zombies liegen ausserhalb des Bildes —
  als Bildbeleg schwach, als Logbeleg vollstaendig.
* Dicker Zombie mit Elza: in keinem der geprueften Elza-Raeume ist beim Eintritt ein 0x11 erreichbar (Zensus
  1001/1011/1021/1031/1041/1061/1071/1081/1091/10D1/10E1/1101/11E1/1201; 0x11 nur in 1021/1071/1091, dort im
  Ruhezustand ohne Treffer, `RE15_FORCE_EXPLOSION ... Treffer=0`). Der dicke Zombie ist deshalb mit Leon belegt (§6.1,
  §6.2) und fuer beide Figuren im Riegel (unit_r41_granate [1]/[2], figurunabhaengig).

## 7. Tests

* NEU `unit_r41_granate` (tests/unit/test_r41_granate.c, probes/r41_granate.cmake): [1] Schaden 1000 an
  0x10/0x11/0x12/0x13/0x16/0x18 (dick 0x11 mit 250 -> -750, Tod), RE1.5-KI-Dicker (107 -> -893), Saeure/Brand bleiben
  200, Nicht-Zombies unveraendert (Hund 300, Kraehe 60, Spinne 60, Gorilla/Kakerlake/Alligator 1000); [2] RE2-Gehirn,
  sechs Typen (0x10/0x11/0x16 aus ROOM1140, 0x12/0x13/0x18 umgetypt): Zerfetzung genau einmal, Kopf/Oberarme/
  Oberschenkel fliegen, Unterarme/Unterschenkel/Fuesse platzen, Haende weg, Sturz 0x80108530, Leiche, kein Kriecher,
  mit nachgefahrenem Zeichengang (Teilstufe + Part-Matrix wie main.c) nach 150 Bildern nur Rumpf + Becken gezeichnet;
  [3] RE1.5-Gehirn z_granate, Leiche ohne Kriecher.
* NEU `unit_r41_granate_aus` (eigener Prozess, `RE15_KEIN_80PROZENT_GORE=1`): keine Zerfetzung (RE2 0x80108BEC wie
  vorher), Schaden bleibt 1000.
* Verhaltensbedingt angepasst (alte Pins der schwaecheren Granate / des Zerreissens): probe_r34_schaden 31/34/35/38/99,
  probe_r34_reaktion 220/221/222, test_r35_granate 120-130/173/234, test_r36_granate 105, test_re2_gore PIN 8
  (Latch vorbelegt, misst weiter den Russ allein), test_zombie_gore (w9 zerfetzt, w10 behaelt Oberkoerper/Kriecher),
  integration_r35_granate und integration_r36_granate (Zerfetzung je getroffenem Slot ueber RE15_GR_LOG statt
  "weggeschleudert > 600"; 0x11 mitgezaehlt).
* Gelaufen: Zielauswahl 101 Tests (`granate|gore|zgore|re2z|ztear|teardeath|r34_schaden|r34_reaktion|zombie_death|
  corpse|deathgetup|werfer|weapon_rows|dog|hund|crow|kraehe|spider|spinne`, ohne integration) 101/101 gruen;
  integration_r34_granaten, integration_r35_granate, integration_r36_granate 3/3 gruen (528 s).
* Volle Unit-/Prop-/Probe-Suite im Baum (`ctest -E ^integration_ -j 8`, 2026-11-02): **1076/1076 gruen** (1023 s).
* Gegenprobe RE2-Modelle (Menue "RE2 AI + Model", ohne `RE15_AI_MODELS`): derselbe Wurf, vier `[gr]`-Zeilen, Bild
  `GR_belege/port/p04_leon_1140_re2modelle_nachher.jpg` (Rumpf traegt dort das RE2-Stumpf-Mesh 16).

## 8. Fuer den Nutzer (Kurzfassung)

1. **Welche Granate:** die normale Handgranate = Item 0x09 (nicht Saeure 0x0A / Brand 0x0B, nicht der Granatwerfer).
2. **Video:** In den 80%-Videos wird keine Handgranate geworfen. Die Zombie-Explosionen (B 3:31, 3:36, 8:05, 8:42) kommen
   aus Elzas Granatwerfer (Muendungsblitz an der Huefte, Einschlag 0,2-0,5 s spaeter). Dort zerplatzt der Zombie in einer
   roten Saeule, uebrig bleiben Lache und Fleischhaufen. Genau das macht jetzt die Handgranate (und die
   Granatwerfer-Explosivrunde, die dieselbe RE2-Zeile 9 nimmt).
3. **Staerke:** Die Handgranate macht an Zombies wieder ihre eigenen 1000 Schaden aus dem RE1.5-Original statt der 200 der
   RE2-Granatwerfer-Zeile. Die dicken Zombies (Typ 0x11, 250 HP) ueberlebten bisher mit 50 HP; jetzt sterben alle Zombies
   im Wirkradius. Hund, Kraehe, Spinne, Gorilla und Bosse: unveraendert.
4. **Zerfetzen:** Kopf, Oberarme und Oberschenkel fliegen weg, Unterarme, Unterschenkel und Fuesse platzen in rote
   Scherben, die Haende sind weg, Rumpf und Becken bleiben verrusst in einer grossen Lache liegen. Alles aus vorhandenen
   RE2-/RE1.5-Mechanismen zusammengesetzt, die Zusammenstellung ist Gestaltung nach dem Video.

## 9. Offen

1. Elza am dicken Zombie im echten Fenster: kein Elza-Raum mit beim Eintritt erreichbarem 0x11 gefunden (§6.4). Logik
   figurunabhaengig und im Riegel; ein Bild Elza + 0x11 braucht einen Spielstand mitten im Elza-Ablauf.
2. Saeure-/Brandgranate (w10/w11) im RE1.5-Gehirn behalten die alte Gestaltung "Oberkoerper ab, kriecht weiter", deren
   Videobeleg (BEFUND 5.2, 8:10-8:22) laut Spur S nicht traegt. Nicht Teil dieses Auftrags.
3. 0x18 mit dem Einzel-HP-Wert 1058 (@0x8011f33a) ueberlebt die 1000 nur ohne RE2-Modell (Import-Option aus + RE1.5-KI,
   nicht die Vorgabe) — wie im Original.
4. Der Rumpf verschwindet nicht ganz (Video 3:31.6/3:36.3: Koerper fast weg). Keine Quelle hat ein Rumpf-Platzen fuer
   Zombies; der Haufen in der Lache entspricht 8:42.5. Ein Rumpf-Platzen waere reine Erfindung.

# Spur S — Gore-Sichtung der 80%-Videos (Runde 41, 2026-11-01)

Auftrag: das gesamte 80%-Material Bild für Bild auf Gore durchsehen, jede Stelle mit Zeit, Raum, Gegner, Waffe,
Ablauf und Sicherheit festhalten, jede Stelle gegen den Port unter der Standard-KI (RE2-Gehirn) halten.
Es wurde nichts gebaut und kein Code geändert.

Regel des Nutzers (bindend): Beim Gore gilt je Gegnertyp das Stärkste aus RE1.5, RE2 und den 80%-Videos.
Ob ein Merkmal eingebaut wird, ist eine PORT-WAHL. Merkmale, die nur die Videos zeigen, sind mit Video und Zeitmarke
belegt. Ihr Nachbau ist Gestaltung nach Bild und keine Byte-Treue.

## 0. Ergebnis in Kürze

1. **Die Spinne platzt im Video, im Port unter der Standard-KI nicht sichtbar.** B 5:18.3: Nach drei Schüssen aus
   einer kurzen Waffe fliegen gelbgrüne Brocken etwa 0,5 s lang strahlenförmig weg. Grüne Flecken bleiben gut 1 s
   liegen, der dunkle Kadaver mit Beinen bleibt liegen (g09).
   Im Port gehört jede Spinne 0x25 dem RE2-Gehirn (`enemy_ai_re2_spider.c:3071`, Abgabe `enemy_ai_common.c:11156`).
   Dort setzt der Tod "ZERPLATZEN" (`:2214`) nur Clip 12, **einen** ESP-Effekt am Aktor-Ursprung (`re2s_gore`
   `:1754`) und Babyspinnen. Der Flug der Teile ist im Code ausdrücklich OPEN, die Zeilen 1 bis 4 (Handfeuerwaffen)
   platzen gar nicht. Das RE1.5-Platzen (`enemy_ai_common.c:11459-11466`, Teile 0/19) wird unter RE2 nicht erreicht.
   Der Verdacht des Orchestrators stimmt also am Code. Gemessen zur Laufzeit ist er damit noch nicht.
2. **Zombies zerplatzen durch Explosionen ganz.** Im Video bleibt danach nur Blut, ein Brocken oder ein Fleischhaufen
   in einer Lache (B 3:31.4, 3:36.0, 8:42.2, Tunnel 5:22.9). Im Port nimmt die Granatenzeile 9 den Zerreiß-Tod 8BEC:
   Kopf und Beine sind ab, der Rumpf bleibt liegen. Der Port hat das also nur **teilweise**.
3. **Kopf weg und kopflos weiterlaufen kommen im Video vor** (B 2:28.4, 2:34.9, beide wahrscheinlich), mit einer
   langen Waffe, wahrscheinlich der Schrotflinte (nicht sicher).
   Im Port gibt es den kopflosen Läufer nur in der Magnum-Zeile 5 (92C4). Den Kopf verliert der Zombie sonst nur im
   80%-Zerfetzen bei Schrot-Krit und Münzwurf.
4. **Die Krähe zerfällt in schwarze Fetzen, die am Boden verstreut liegen** (B 1:11.8-1:15.8). Der Port zeigt nur
   rotes Blut, das Feder-Sprite ist OPEN.
5. **Drei Aussagen von BEFUND §5 halten dem Einzelbild nicht stand** (Abschnitt 3). Eine davon dient im Code als
   Begründung (`enemy_ai_re2_zombie.c:7697`).

## 1. Material und Methode

| Kürzel | Datei | Umfang | Durchsicht |
|---|---|---|---|
| A | `Version_80%/Biohazard 1.5 Final Build.mp4` | 2:19, 640x480, vom Fernseher abgefilmt, Diashow | Schnitterkennung ergab **174 Standbilder** zu je 0,8 s, alle angesehen (6 Bögen zu je 30 Bildern), Verdachtsbilder einzeln vergrößert |
| B | `Version_80%/Resident Evil 1.5 (All Footage).mp4` | 14:19, 320x240, 30 Bilder/s | Bögen mit 2 Bildern/s, lückenlos (48 Bögen zu je 36 Bildern, 1718 Bilder). Dazu 10 Bilder/s für die ganze Datei (8591 Bilder) als Messreihe. An jeder Gore-Stelle Ausschnitte mit 10 bis 30 Bildern/s, vergrößert und aufgehellt |
| CD1-4 | `documentation/Biohazard 1.5 Complete Disc Video N of 4 (360p).mp4` | 73/44/54/59 s, 320x240 | jedes Bild mit 2 Bildern/s gegen B korreliert: Außer Vorspann, Blenden und Abspann liegt alles in B (r >= 0,83). Die Ränder einzeln angesehen. CD4 0:45.9-0:48 hat bessere Qualität als B 12:56 und wurde dafür genommen (g14) |

- Hilfsmessung: Für jedes Bild bei 10 Bildern/s ist der Anteil roter Pixel (R > 90, R > 1,8·G, R > 1,8·B) und
  gelbgrüner Pixel festgehalten, dazu Sprünge gegen den gleitenden Median. Die Messung diente nur als Suchhilfe.
  Jede Stelle unten ist am Bild geprüft.
- Raumnummern stammen aus `analysis/video_80prozent/BEFUND.md` §3 und `analysis/story_begegnungen/pruefung/video_b.md` §4.
  Diese Zuordnung habe ich übernommen und nicht neu gerechnet.
- Grenzen der Sichtung:
  - B hat 320x240, Figuren sind 20 bis 60 Pixel hoch.
  - A ist ein abgefilmter Fernseher mit Blaustich und Überstrahlung, **Farben aus A sind kein Beleg**.
  - Was bei dieser Auflösung nicht eindeutig ist, steht als "offen".
- Werkzeuge (Scratch, nicht im Repo): `…/scratchpad/S_tools/{metrics,sheet,stills,cdmatch,multi,crop,spikes}.py`.
  Die Zwischenbilder liegen in `…/scratchpad/S_video/{A,B,CD1-4,sheets,z,h30}`.
- Belegbilder: `analysis/befunde_runde41/S_belege/video/g01…g31_*.jpg`. Das sind kleine Streifen mit Zeitstempel je
  Einzelbild, Zeitangaben im Bild als `B m:ss.s`. Die älteren `b01…b06` desselben Ordners decken sich mit g09, g10,
  g08, g12, g03 und g17.

Sicherheit: **sicher** heißt, in mehreren Einzelbildern eindeutig. **wahrscheinlich** heißt, die Bildfolge spricht
dafür, ein Einzelbild lässt aber eine andere Deutung zu. **offen** heißt, bei dieser Qualität nicht zu entscheiden.

Port-Spalte: Geprüft ist der Code unter der Standard-KI (RE2-Flavor, `re15_ai_re2_for_type`
`enemy_ai_re2_zombie.c:170`). Laufzeit und Bild des Ports wurden **nicht** gemessen.

## 2. Tabelle je Stelle

### 2.1 Zombie-Familie

| ID | Video, Zeit | Raum | Waffe | Was geschieht, Ablauf | Sicherheit | Port (Standard-KI) | Code-Stelle | Beleg |
|---|---|---|---|---|---|---|---|---|
| Z01 | B 0:36.5-0:41 | 11E0 Cut 13 | offen | Trefferwolke 0:38.5, am Boden Leiche mit großer Blutlache | sicher | Wolke: vorhanden. Leiche und Lache stammen aus Raumbild oder Objekt, nicht geprüft | Treffer-ESP `re2z_gore_fx` `enemy_ai_re2_zombie.c:4448` | – |
| Z02 | B 0:41.3-0:42.1, 0:46.0-0:46.6 | 1070 Cut 6 | Handfeuerwaffe (Elza) | Nahschuss, große rote Wolken am Oberkörper, der Zombie bleibt ganz, mehrere liegen | sicher | vorhanden | wie Z01 | g27 |
| Z03 | B 0:50-0:54 | 1040 Cut 1 | – | Ein Wesen auf allen Vieren kriecht unter dem halb offenen Rolltor durch, große Lache im Vordergrund. Ob Beine fehlen: nicht zu erkennen | Kriechen sicher, Beine offen | Kriecher vorhanden | `re15_re2z_enter_crawler` `:3231` | g16 |
| Z04 | B 1:34.0, 1:52.5, 3:13.0 | 1200 Cut 2, 11E0 Cut 5 | offen | Wolken am Kopf, liegende RPD-Zombies | sicher | vorhanden | wie Z01 | – |
| Z05 | B 2:28.4-2:29.6 | 1030 Cut 1 | lange Waffe vor der Brust, **wahrscheinlich Schrot**, nicht sicher | Schuss in den Rücken des Vordergrund-Zombies: große rote Wolke am Kopf. Der Körper kippt, an Halshöhe leuchtet eine rote Stelle etwa 1 s. Ein Kopf ist danach nicht mehr erkennbar | Kopf weg **wahrscheinlich** | **teilweise**: Den Kopf verliert der Zombie im RE2-Gehirn nur in Magnum-Zeile 5, Spalte 1 (92C4), oder im 80%-Zerfetzen (Schrot-Zeile 7, Spalte 1, Krit + rand&1) | Tabelle `:7673`, `re2z_80_zerfetzen` `:7740` | g01 |
| Z06 | B 2:34.8-2:37.4 | 1070 Cut 4, 7 | offen (wie Z05) | Wolke über dem ganzen Oberkörper, rote Trümmer am Boden. Der Körper geht **ohne Kopf** einige Schritte weiter, der Halsstumpf leuchtet rot. Dann liegt er, Draufsicht 2:37.4: Blut am Kopfende | kopflos weiter **wahrscheinlich** | **teilweise**: kopflos weiter nur 92C4 P4 (Magnum-Zeile 5). Mit Schrot fehlt es | `re2z_death_magnum` `:5941` | g02 |
| Z07 | B 2:43.9, 2:45.6 | 1040 Cut 4, 5 | **Schrotflinte** (sichtbar 2:47.6) | zwei Wolken, der Zombie fällt und taumelt, steht wieder auf, **der Kopf bleibt** | sicher | vorhanden | – | g03 (= b05) |
| Z08 | B 3:31.4-3:31.8 | 3000 Cut 2 | Granatwerfer (Elza, Explosion mit Feuer) | Der Vordergrund-Zombie (weißes Hemd) wird zur roten Wolke mit Feuer, im nächsten Bild ist er weg. Am Boden bleiben Blut und Reste | Körper zerplatzt **wahrscheinlich** | **teilweise**: Zeile 9 geht über die 80%-Weiche auf 8BEC (Kopf und Beine ab, Rumpf bleibt) | `re2z_death_zelle` `:7766` | g04 |
| Z09 | B 3:32.8-3:33.2 | 3000 Cut 2 | Granatwerfer | zweiter Zombie: Explosion und rote Wolke, danach Körper und Lachen am Boden | Explosion sicher, Teilverlust offen | teilweise (wie Z08) | `:7766` | g04 |
| Z10 | B 3:34.4-3:34.8 | 3000 Cut 2 | Granatwerfer | dritte Explosion an einem Zombie, Ausgang verdeckt | offen | – | – | – |
| Z11 | B 3:35.6-3:39.8 | 3010 Draufsicht | Granatwerfer | Der RPD-Zombie vor Elza wird getroffen (3:36.0), eine senkrechte rote Säule steigt auf (3:36.2), **der Zombie ist weg**. Es bleiben eine große Lache und ein grauer Brocken. Im nächsten Schnitt liegen ein grauer Klumpen und ein zerfetzter roter Haufen | zerfetzt **wahrscheinlich** | **teilweise**: Es bleibt immer ein Rumpf. Fleischbrocken (9610) gibt es nur in Zeile 8/17, Spalte 4 | Tabelle `:7682-7690` | g05 |
| Z12 | B 3:36-3:46, 7:20-7:26 | 3010, 3000 Cut 1 | – | viele Leichen und Blutlachen am Boden (nach dem Gefecht und im Raumbild) | sicher | Lache an der Leiche vorhanden. Leichen-Objekte nicht geprüft | `re2z_corpse_pool_grow` `:8331` | g26 |
| Z13 | B 4:31.3-4:37.8 | Rippen-Tunnel (Mod-Ort, nicht auf der Disc) | offen | weiße Spritzer (Wasser oder Treffer), Zombies fallen in den Kanal, kein Teil fehlt | sicher (kein Gore außer Treffer) | – | – | g29 |
| Z14 | B 5:22.6-5:24 (Wiederholung 7:06.9) | Tunnel mit oranger Tür (nicht auf der Disc) | offen, weiße Explosionswolke | Der Khaki-Zombie hängt über der Brüstung. Treffer: rote Wolke, helle Körperbrocken fliegen hoch, große weiße Wolke. Danach ist der Zombie weg, ein heller Brocken fällt herab | zerfetzt **wahrscheinlich** | teilweise (wie Z11) | `:7766` | g08 (= b03) |
| Z15 | B 5:28.8-5:29.6 | Tunnel | Leon, lange Waffe | Wolken am Oberkörper von zwei Zombies, beide fallen | sicher | vorhanden | – | – |
| Z16 | B 6:01.8-6:05.1, 6:13.5-6:32.2 | 1040 Cut 6, 2 | offen | Nahkampf mit Wolken an Kopf und Oberkörper, Leichenhaufen, kein Teilverlust erkennbar | sicher (Wolken), Teile offen | vorhanden | – | – |
| Z17 | B 7:31.6-7:39 | 5060 Cut 6, 5 | Elza | große rote Wolke, der Zombie fällt und liegt als roter Haufen | Wolke sicher, Teile offen | vorhanden | – | g31 |
| Z18 | B 7:43.5-7:47, 9:12-9:31, 10:37-10:43, 12:52-13:03 | 5140, 5120 | – | Brennende Zombies laufen weiter, überall Bodenfeuer | sicher | vorhanden | `re15_brand_re2_takt` `brandzombie_r15.c:124` | g15 |
| Z19 | B 12:59-13:01 (CD4 0:48.5-0:51) | 5120 Cut 6 | – | Eine brennende Leiche liegt am Boden und brennt weiter | sicher | **offen**: Ob die Flammen an der Leiche im Zustand 7 bleiben, ist am Port nicht geprüft | `re15_brand_tod` `brandzombie_r15.c:89` | g15 |
| Z20 | CD4 0:45.9-0:47.9 (= B 12:56) | 5140 Cut 4-6 | Leon, Handfeuerwaffe | Ein brennender Zombie wird getroffen: rote Wolke am Kopf (46.3-46.6), der Körper geht gebeugt weiter und fällt brennend. Ob der Kopf danach fehlt, ist nicht zu erkennen | Wolke sicher, Kopf offen | Wolke vorhanden | – | g14 |
| Z21 | B 7:50.9, 7:58.2 | 1030 Cut 11, 1 (blau) | Explosion mit orangen Funken (Brand-Granate?) | Explosion am Zombie, orange Brocken und Funken fliegen, der Zombie fällt. Leichen am Boden | Explosion sicher, Teile und Waffe offen | offen | Brand-Zeile 10 `re2z_dismember_row` `:5100` | g30 |
| Z22 | B 8:06.9-8:07.5 | 1070 Cut 3 | Granatwerfer | Orange Säule an einem Zombie in der Bildmitte. Ob er verschwindet, ist offen | offen | – | – | – |
| Z23 | B 8:10.3-8:25 | 1070 Cut 3 | **Granatwerfer** | Granate auf einen RPD-Zombie vor der Kamera: orange Säule, der Körper fällt rückwärts, liegt (8:11-8:13) und **richtet sich mit beiden Beinen auf** (8:14-8:16). Zweite Granate 8:16-8:17. Ab 8:19 wächst eine **sehr große runde Blutlache** mit gezacktem Rand, 8:24.5 richtet er sich erneut auf. **Kein Torso-Abriss** | sicher (ganzer Körper, große Lache) | **teilweise**: Die Lache wächst im Port nur an der **Leiche**. Unter einem lebenden, liegenden Zombie gibt es keine. Größe nicht verglichen | `:8331` | g06 |
| Z24 | B 8:40.5-8:44 | 1070 Cut 7 Draufsicht | **Granatwerfer** (lange Waffe sichtbar) | Ein Zombie richtet sich in der Lache auf, Granate 8:42.2, orange Säule. Danach liegt **nur noch ein roter Fleischhaufen in einer runden Lache**, Kopf und Glieder nicht erkennbar | zerfetzt **wahrscheinlich** | teilweise (wie Z11) | `:7766` | g07 |
| Z25 | B 11:31.4, 11:33.0, 11:35.4, 11:38.4 | 1070 Cut 1-2 | Leon | Vier rote Wolken am Kopf mit Trümmerteilchen. Der Zombie fällt 11:38.6 **mit Kopf** | sicher (Kopf bleibt) | vorhanden | – | g17 (= b06) |
| Z26 | B 12:03.4, 12:08.6, 12:18.0 | 1070 Cut 4, 7 | Leon | Wolken an Zombies im Hintergrund | sicher | vorhanden | – | – |
| Z27 | Trailer B 9:42.9-9:43.5, 9:54.2, 9:55.5 | 1150 Cut 0, 2 | Leon-Modell | rote Wolken, Zombies fallen | sicher | vorhanden | – | – |
| Z28 | Trailer B 10:00.8-10:01.5 | 1040 Cut 5 | **Schrotflinte**, Nahschuss | Rote Wolke am Kopf, **rote Spritzer fliegen an die Wand und bleiben dort etwa 0,6 s bis zum Schnitt sichtbar**. Der Zombie sackt auf die Knie. Kopf danach offen | Wandspritzer **wahrscheinlich**, Kopf offen | **Wandspritzer fehlt**: Es gibt keine Wand-Decals, nur die Wund-Decals des Spielers (`re15_damage.c:115`) | – | g18 |
| Z29 | B 10:10.9-10:11.4 (= 7:06.9) | Tunnel, Nahaufnahme | – | Rote Wolke an Schulter und Kopf, der Kopf bleibt (Haar sichtbar) | sicher | vorhanden | – | – |
| Z30 | B 13:58.5-14:15 | 1030 Cut 7, 8 (blau), Todeseinschnitte | – | **Spielerfigur** (wohl Leon) liegt, darunter Blut, Zombies stehen darüber, dazu die Todeseinschnitte | Blut sicher, Figur wahrscheinlich | Todeslache vorhanden (`game_step_common.c:175`). Fressen über der liegenden Figur nicht geprüft | – | g19 |
| Z31 | B 1:03-1:08.4 | 1200 Cut 2 | **Flammenwerfer** | Leon sprüht Feuer über die Bahre. Ob der Zombie brennt, ist im Feuerstrahl nicht zu trennen | Flammenwerfer sicher, Brand am Ziel offen | Flammenwerfer vorhanden (`werfer_r35.c:228`, `kleinkram_r38.c`), Brand-Gore Zeile 10 (`:5100`) | – | g13 |
| Z32 | A 0:02.8-0:05.2 | Gang, Raum offen | offen | 0:03.6 rote Wolke an einem Zombie hinter Elza. 0:04.4 Nahaufnahme einer großen rot-orangen **Blutfontäne mit Brocken**. 0:05.2 kauernde Zombies | Fontäne sicher, Ausgang offen | teilweise: Brocken-Fontänen gibt es nur in den Zellen 92C4, 8BEC, 9610 und im 80%-Zerfetzen | `:7740` | g20 |
| Z33 | A 0:11.6, 0:15.6, 0:24.5, 0:55.7, 0:57.3 | 1190, Gänge, Garage | – | Blutlachen und Spritzer am Boden | sicher (Blut). Raumbild oder Effekt: offen | Lachen an Leichen vorhanden | – | g21 |
| Z34 | A 0:21.3 | Raum offen | – | Blut an der Wand hinter Elza | offen (Raumbild oder Spritzer) | – | – | g21 |
| Z35 | A 0:38.9, 0:50.1, 0:53.3, 1:18.9 | 3000 Cut 1, Laufsteg (Raum offen) | – | Leichen und Leichenhaufen mit Blut | sicher | Leichen und Raumdaten nicht einzeln geprüft | – | g22 |
| Z36 | A 1:47.7, 1:48.5, 2:06.2 | 1200, Gang, oranger Gang (wohl 5120) | Leon | Trefferwolken, 2:06.2 Wolke über dem Kopf, Ausgang offen | Wolke sicher | vorhanden | – | g23 |

### 2.2 Spinne 0x25

| ID | Video, Zeit | Raum | Waffe | Was geschieht, Ablauf | Sicherheit | Port (Standard-KI) | Code-Stelle | Beleg |
|---|---|---|---|---|---|---|---|---|
| S01 | B 5:14.5-5:19.1 | Maschinenraum mit rotem Rad (nicht auf der Disc, auch A 1:25.3) | **kurze Waffe**, Mündungsblitze 5:15.4, 5:16.8, 5:18.2. Ob Handfeuerwaffe oder Magnum, ist offen | **Der Körper platzt** 5:18.3: gelbgrüne Brocken fliegen in alle Richtungen (rund 0,5 s), grüne Flecken bleiben gut 1 s auf Boden und Wand. Danach liegt der Kadaver dunkel und flach, die Beine sind noch dran. Keine Babyspinnen zu sehen (bei 320x240 offen) | Platzen **sicher**, Beine bleiben wahrscheinlich | **Bild fehlt, Mechanik teilweise.** RE2 besitzt jede 0x25. ZERPLATZEN (Zeilen 5/6/7/8/17 = w7 Magnum, w8 Schrot, w13 SPAS, w18 Rakete) setzt Clip 12, `re2s_gore`, also **einen** ESP-Effekt 0 am Aktor-Ursprung, und 6 bis 9 Babys. Der Teile-Flug (Part-Array, FUN_801056DC-Records) ist OPEN. Handfeuerwaffen (Zeilen 1-4) nehmen den generischen Tod. Das RE1.5-Platzen (Teil 0 + 19 bei Krit) ist unerreichbar | `enemy_ai_re2_spider.c:3071`, `:2214`, `:1754`, `:1732`. RE1.5: `enemy_ai_common.c:11459-11466`, Abgabe `:11156` | g09 (= b01) |
| S02 | B 5:30.7-5:33.7 | 20B0 Cut 11-14 (grauer Gang, Mod: 2070) | Dauerfeuer, Hülsen fliegen (MG oder Gewehr, offen) | Spinnen am Boden. Bei jedem Treffer **grüne Spritzer** (5:31.3, 5:31.9, 5:32.5) | grün wahrscheinlich | **offen**: Der Treffer ist ESP-Effekt 0 am Ursprung, seine Farbe in diesen Räumen ist nicht geprüft | `:1754` | g10 (= b02) |
| S03 | B 5:34-5:40 | 20B0 | – | Weiße Wolken füllen den Gang (BEFUND G4) | kein Gore-Beleg | – | – | – |
| S04 | B 5:42-6:01 | 2070 Moosgang | Elza | Eine Spinne kommt von der Decke. Eine tote Spinne liegt auf dem Rücken mit angezogenen Beinen (5:50-5:56), der Kadaver bleibt. Kein Platzen | sicher | vorhanden (CORPSE) | `:2190` | g28 |
| S05 | B 7:39-7:43 (= 10:33) | 20B0 Cut 14 | Leon | Große gestreifte Spinne am Boden, 7:40.4 **grüner Fleck** an der Spinne (Treffer), die Spinne bleibt ganz | wahrscheinlich | offen (Farbe wie S02) | `:1754` | g11 |
| S06 | A 1:25.3 | Maschinenraum mit rotem Rad | Leon | Spinne im Vordergrund, rote Flecken am Boden | offen (Raumbild oder Blut) | – | – | – |

### 2.3 Krähe 0x21, Hund 0x20, Affen, Birkin

| ID | Video, Zeit | Raum | Was geschieht | Sicherheit | Port (Standard-KI) | Code-Stelle | Beleg |
|---|---|---|---|---|---|---|---|
| K01 | B 1:09-1:16 | 1170 Cut 3 (Elza) | 1:11.8-1:12.2: Eine getroffene Krähe zerfällt, **schwarze Fetzen und Federn** verteilen sich am Boden. 1:13.5 rote Wolke an einer Krähe neben Elza. 1:15.0-1:15.8 die nächste Krähe zerfällt in schwarz-weiße Fetzen, die liegen bleiben | rote Wolke sicher, Fetzen wahrscheinlich | **teilweise**: Der RE2-Tod ist 3× `re2c_featherburst` = rotes ESP-Spritzen. Das Feder-Sprite ist OPEN (`re2c_feather` nur Würfe). Die RE1.5-GIB-Spur (13 Knochen-Teile @0x80114a50) versteckt nur das Mesh | `enemy_ai_re2_crow.c:213`, `:225`, `:1333`. `enemy_ai_common.c:6777-6795` | g12 (= b04) |
| H01 | B 2:58-3:05.5 | 1190 (Elza) | Blutspritzer am Boden, blutige Hundekadaver | sicher | vorhanden (Gore 0x85, Lache) | `enemy_ai_re2_dog.c:317-378` | g24 |
| H02 | B 8:52.5-9:05 | 1190 (Leon) | Hunde springen Leon an, er liegt am Boden. Blutige Hundekadaver | sicher | vorhanden | wie H01 | g24 |
| F01 | B 12:40.7-12:46.7 | 4080 | Dunkelrotes Affenwesen springt Elza an. 12:41.6-12:42.5 **zerfällt das Modell in Blöcke mit weißen Löchern**: ein Polygon- oder Clip-Fehler der Aufnahme, **kein Gore**. 12:44.3 Treffer mit orangen Funken | sicher (kein Gore) | – | – | g25 |
| F02 | A 0:10.8, 0:14.8, 1:40.5, 1:41.3 | 1141, 11C0 | Rote Gorillas. Das Rot ist die Modellfarbe, kein Blut | sicher | – | – | – |
| B01 | B 2:05, 3:11, 3:54-4:03, 7:10-7:14, 13:03-13:56 | 3070, 50A0 | Birkin, Textur blutig, kein Gore-Ereignis | sicher | – | – | – |

## 3. Korrekturen an früheren Aussagen

| Stelle | Frühere Aussage | Einzelbild-Befund |
|---|---|---|
| BEFUND §5.1/§5.2, video_b §4 "Kopf platzt 2:45.7" | Nahschuss, der Kopf platzt | **Der Kopf bleibt** (Schrot, Zombie steht wieder auf), g03. Die Kopf-Kandidaten sind **2:28.4** und **2:34.9** (Z05, Z06) |
| BEFUND §5.2 "Nahschuss … 11:36" | Der Kopf platzt | **Der Kopf bleibt**, der Zombie fällt mit Kopf, g17 |
| BEFUND §5.1/§5.2 "Granate 8:10-8:12, 8:22: Ober- und Unterkörper getrennt, Oberkörper kriecht weiter" | Torso-Abriss | **Nicht bestätigt.** Der Zombie bleibt ganz, richtet sich zweimal mit beiden Beinen auf und liegt in einer sehr großen Lache (Z23, g06). Zerplatzen durch Granaten zeigen dagegen **3:31.4, 3:36.0 und 8:42.2** (Z08, Z11, Z24) |
| Code-Kommentar `enemy_ai_re2_zombie.c:7697` | Video 3 8:10-8:22 (BEFUND 5.2) als Beleg für Zeile 9 auf 8BEC | Die Zeitmarke trägt das nicht. Tragfähig sind Z08, Z11 und Z24, und die zeigen **mehr** als 8BEC (Körper fast ganz weg). Den Kommentar beim nächsten Bau berichtigen |

## 4. Was die Videos zeigen und dem Port unter der Standard-KI fehlt (je Gegnertyp)

### Zombie (0x10-0x1F)

1. **Ganzes Zerplatzen durch Explosion** (Z08, Z11, Z14, Z24; Belege g04, g05, g07, g08).
   - Video: rote Säule oder Wolke. Der Körper ist danach weg, es bleiben Lache, grauer Brocken und Fleischhaufen.
   - Port: Zeile 9 geht auf 8BEC, der Rumpf bleibt.
   - Bausteine im Bestand:
     - RE2 9610 "Wegschleudern + Fleischbrocken" (`re2z_death_burst` `:5800`, heute nur Zeile 8/17, Spalte 4);
     - RE1.5 Schrot-Tod 0x80107ee0, Zweig "blow-apart" (rng&1==1 @0x801080c8-d4, `video_80prozent/zombie_gore_spec.md` §2d).
   - PORT-WAHL: welche Zelle und ob der Rumpf verschwindet.
2. **Kopf weg und kopflos weiter mit einer Nicht-Magnum-Waffe** (Z05, Z06, beide wahrscheinlich, Waffe nicht sicher).
   - Port: kopflos weiter nur Magnum (92C4 P4).
   - Erst die Waffe sichern, dann entscheiden. Sonst bleibt es offen.
3. **Wandspritzer, die an der Wand bleiben** (Z28, Trailer, wahrscheinlich). Port: keine Wand-Decals.
4. **Blutlache unter einem noch lebenden, liegenden Zombie** nach Explosionstreffern, sehr groß (Z23).
   Port: Die Lache wächst nur an der Leiche (`:8331`).
5. **Die brennende Leiche brennt am Boden weiter** (Z19). Port: nicht geprüft (offen, nicht als fehlend gezählt).

### Spinne (0x25)

1. **Der Körper platzt in gelbgrüne Brocken, grüne Flecken bleiben, der Kadaver mit Beinen bleibt liegen** (S01, g09).
   - Port unter der Standard-KI:
     - Ein sichtbares Platzen gibt es nicht. RE2-ZERPLATZEN ist nur Clip 12 mit einem ESP-Effekt am Ursprung, der
       Teile-Flug ist OPEN.
     - Mit Handfeuerwaffen gibt es kein Platzen.
     - Das RE1.5-Platzen ist unerreichbar.
   - Im Video genügen drei Schüsse aus einer kurzen Waffe.
   - Das deckt sich mit dem Nutzerbefund "platzen bei Magnum-Schuss nicht".
2. **Grüne Treffer-Spritzer** (S02, S05). Port: Treffer-ESP am Ursprung, Farbe offen. Am Bild prüfen.
3. Beine ab, Babyspinnen: **in keinem Video zu sehen.** Der Port hat dafür RE2-Mechanik (Bein-Maske, Babys).
   Die Beine werden aber nicht gezeichnet (`re2s_legs220` hat keinen Leser außerhalb des Moduls).

### Krähe (0x21)

1. **Schwarze Fetzen und Federn, die am Boden verstreut liegen bleiben** (K01, g12). Port: nur rotes Blut.
   Feder-Sprite und RE1.5-Teile-Streuung (13 Knochen @0x80114a50-aa4) sind OPEN.

### Hund (0x20), Affen (0x27/0x29), Birkin (0x30), Spielerfigur

- Kein Gore-Merkmal, das dem Port fehlt. Hundekadaver und Lachen sind vorhanden.
- 4080 (F01) ist ein Darstellungsfehler, kein Gore.
- Spielerfigur: Die Todeslache ist vorhanden. Ob Zombies über der liegenden Figur fressen (Z30), ist nicht geprüft.

## 5. Offen (benannt, nicht gedeutet)

1. Waffe in Z05, Z06 (B 2:21-2:39) und S01 (B 5:15-5:18). In 320x240 ist sie nicht eindeutig.
   Nächster Weg: die besseren Vorlagen der Complete Disc enthalten diese Abschnitte nicht. Eine andere Quelle wäre nötig.
2. Spinnen- und Krähen-Treffer im Port **am Bild**: Farbe des ESP-Effekts 0 in 20B0, 2070 und 1170. Das braucht
   einen Fensterlauf nach `re15-port-visual-verify`, in dieser Sichtung nicht gemacht.
3. Z19 (Weiterbrennen der Leiche) und Z30 (Fressen) am Port-Bild.

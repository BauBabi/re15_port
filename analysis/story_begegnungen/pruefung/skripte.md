# Prüfung „skripte“: BERICHT.md Abschnitte 3, 4, 6, 7, 8 gegen die rohen Disc-Bytes

Geprüft wurde `analysis/story_begegnungen/BERICHT.md` und `TEXTE_40.md` gegen `Version_40%/`.
Alles unten ist statisch: Bytes der RDTs plus Disassembly von Disc-`PSX.EXE` und Disc-`STAGE*.BIN`/`TITLE.BIN`.
Es lief kein Emulator. Wo eine Aussage nur aus Bytes folgt, steht das so da.

## 0. Ergebnis in Kürze

Die wichtigste Korrektur betrifft den Kernsatz des Berichts: „Alle drei Weltspuren zeigen in dieselbe Richtung. Elzas Gruppe ist zuerst da.“
Das stimmt nicht. Es gibt Spuren in **beide** Richtungen.

- **Elza zuerst, Leon sieht die Folgen:** Fabrik (306, 307, 309). Zusätzlich zum Bericht: abgenommenes Lüftungsgitter und Hunde in 306, Stange in 307.
- **Leon zuerst, Elza sieht die Folgen:** Transporter in 109. Elzas Datei zeigt ihn offen, ohne Feuer, ohne Ada. Das ist exakt der Zustand, den Leon erst herstellt.
- **Leon vor Elza:** Dach 117. Bei Leon stehen die Kisten noch, die Elza und John später hinunterstoßen.
- **Angelegt, aber tot:** Elzas 1031 kennt einen Zweig „Rolltor durchbrochen“ an Flag (3,134), das niemand setzt.

Weitere harte Funde:

- `Sce_em_set`-Byte 4 ist die **Etage**, kein „Startbyte“. Marvin steht in 6041 auf der Klappliege.
- Die Marvin-Szene in Garage 11B (`sub14`) ist auf der Disc **unerreichbar**. 11C0 fragt Bank 8 ab, Gorillas schreiben in Stage 1 aber Bank 7. Die Mod hat das korrigiert.
- Leon startet laut EXE auf dem Dach (117), Elza in 103.
- 23 statt 25 Textzeilen sind unbenutzt. 506 #7 und #9 werden über lange AOTs aufgerufen.

Bestätigt:

- Keine Auslöser für die übrigen Zug-Szenen, nach Prüfung aller Startwege.
- Leon sieht Sherry in 11B0, die Szene ist erreichbar.
- Die Zellentür ist echt, 306 Slot 1 ist die Hintertür, die Schutzschalter-Deutung trägt.
- (4,22) und (4,23) haben keinen Schreiber. PL0D/E/F sind die Begleiter.

## 1. Methode und Vertrauensbasis

- **EXE-Adressen:** Disc-EXE und Mod-EXE sind gleich lang, `t_addr 0x80010000`, `t_size 0xAF000`. Ein Wortvergleich ergibt 3630 Unterschiede in 67 Bereichen (`exe_diff.py`). Der SCD/AOT-Kern `0x8003e…–0x80043…` liegt außerhalb. Alle unten zitierten Adressen habe ich trotzdem aus der **Disc**-Datei disassembliert (`mdis.py`).
- **Eigener Walker** `w.py`, unabhängig von `zensus.py`: Er läuft über alle 96 RDTs und 743 Regionen, stoppt am letzten `Evt_end` und meldet Reste. Die Reste sind Padding (2 Byte), bei 2030 sub01 und 2070 sub08 läuft die Region in den nächsten Abschnitt.
- **Lücke im Berichtswerkzeug:** `zensus.py`, `quer.py`, `spuren.py`, `figuren.py` laufen hinter dem letzten `Evt_end` weiter. Das war folgenlos: Sie stoppen dort am ersten unbekannten Opcode (2030 sub01 Op 0x90, 2070 sub08 Op 0xB5).
- **Flag-Schreiber:** Geprüft wurden alle Wege, nicht nur `Set` 0x22: indirektes `Set` 0x59 (Handler `0x8003fe90`, einziger Einsatz 1030 @0x26E6 auf Bank 5), AOT sce=4 (20 Einträge plus 2 Aot_reset, alle Bank 5), EXE-Aufrufer von `FUN_8004ef90` (Bank 5), Overlays (Kill-Bits Bank 7/8) und direkte Referenzen auf Bank 3/4 (nur die Nullung @`0x8003e560`).
- **Rohsuche** über alle 333 Disc-Dateien (`rawset.py`): `22 04 16 xx`, `22 04 17 xx`, `22 03 62 xx`, `22 03 86 xx` kommen in keiner RDT, BIN, EXE, DO2 oder SCD vor, nur als Zufallstreffer in VB-Audio und einem BSS.

Skripte und Bilder liegen im selben Ordner wie dieses Dossier: `exe_diff.py`, `mdis.py`, `w.py`, `reach.py`, `flags.py`, `paare.py`, `kill.py`, `writers.py`, `vars.py`, `items.py`, `msgs.py`, `cnt.py`, `prop.py`, `pts.py`, `bgsheet.py`, `plcmp.py`, `uscd.py`, `rawset.py`, `dumps/ROOM*.txt`, `paare.txt`, `doors.txt`, `asym.txt`, Bilder `ROOM1090_mark.jpg`, `ROOM1170_mark.jpg`, `ROOM1210_mark.jpg`, `ROOM3061_mark.jpg`, `ROOM6041_pts.jpg`, `ROOM50A0_mark.jpg`, `bg309.jpg`, `bg11B.jpg`.

## 2. (a) Auslöser in 603/604

### Alle Wege, auf denen ein RDT-sub starten kann

`FUN_8003ee3c` ist der einzige Thread-Starter mit Sub-Index. Aufrufer laut `jal`-Suche in EXE und allen Overlays:

| Aufrufer | Was | Für RDT-subs relevant? |
|---|---|---|
| `0x8003efb0`, `0x8003efd4` | Raum-Init, main00 und sub00 | Ja, nur Index 0 |
| `0x8003f080` | Bild-Reseed sub01 | Ja, nur Index 1 |
| `0x8003f2c0` | `Evt_exec` | Ja, Index aus `pc[3]` |
| `0x80043104` | AOT sce=3, Index aus Nutzlast +3 | Ja |
| `0x8004c734`, `0x8004cd18`, `0x8004d74c`, `0x8004d7b0` | Modus `0x800b3f7a=2`, Basis `0x80198000` | **Nein.** Eigene Skriptdateien, siehe unten |

Dazu `FUN_8003edec` direkt: `Evt_chain` (`0x8003f284`, Index aus `pc[3]`, auf der Disc 4 Einsätze, keiner in 60xx) und `0x800163f4` (fremde Basis, Index 0).

Die Zweitskripte sind neu im Projektwissen: Datei-ID 0x1A = `PSX/DATA/ROOM115U.SCD` (396 B), 0x1B = `ROOM506U.SCD` (390 B). Beleg: Dateitabelle `0x8006f43c`, Längen passen, Auswahl @`0x8004c6cc` über `V[8]==0` bzw. `==4`. Sie benutzen nur Bank 13 und (2,10), keine Story-Flags (`uscd.py`).

Keine Overlay-Datei ruft `FUN_8003ee3c` per `jal`. In EXE und Overlays gibt es keine `j`-Sprünge auf die beiden Starter und keine Datenzeiger auf sie. Kein Overlay greift auf den Thread-Bereich ab `0x800b2b4c` zu (0 Treffer in allen 7 BINs). `Sce_em_set` (`0x800420a0`) startet keinen Thread, seine Felder sind unten aufgeschlüsselt.

### Befund je Raum

| Raum | Tür-AOT | sce=3 / Aot_reset | Evt_exec-Ziele | Gestartet |
|---|---|---|---|---|
| 6040 | 0→603, 1→514 | keine | sub01→6; sub02→3; sub04→5; sub06→7 | sub00, sub01, **sub06**, sub07, Gosub sub08 |
| 6041 | 0→603, 1→514 | keine | sub01→11; sub02→3; sub04→5; sub06→7,8; sub09→8; sub11→12 | sub00, sub01, **sub11**, sub12, Gosub sub13 |
| 6030 | 0→604 | keine | sub01→11; sub02→3,4; sub05→6,7; sub08→9,10; sub11→12,13 | sub00, sub01, **sub11**, sub12, sub13 |
| 6031 | 0→604 | keine | sub01→8; sub02→3,4; sub05→6,7; sub08→9,10 | sub00, sub01, **sub08**, sub09, sub10 |

**Bestätigt:** sub02/sub04 in 6040, sub02/sub04/sub06/sub09 in 6041, sub02/sub05/sub08 in 6030, sub02/sub05 in 6031 haben auf der Disc keinen Auslöser. Das gilt für jeden geprüften Weg. Der Türsatz trägt ab +12 nur Nullen. Ein Sprung aus einem anderen Raum scheidet aus, die Räume lesen außer Bank 5 nur (4,22)/(4,23).

Zwei Präzisierungen:

1. **604:** Die Zonen sind da, nur die Abfrage fehlt. `main00` legt in beiden Dateien drei Zonen an (sce=5, Flags 0x41, Slots 2/3/4). Der AOT-Scan stempelt `V[1]=Slot` (@`0x80042ee4`). `sub01` prüft genau einen Wert: 6040 `V[1]==4` (@0x620), 6041 `V[1]==3` (@0x630). Die anderen Zonen liest niemand. Welche Zone zu welcher Szene gehörte, ist unklar.
2. **603:** sub02, sub05, sub08 sind keine weiteren Szenen, sondern **Fassungen derselben Szene**: gleicher Aufbau (Spieler-Thread plus Figur-Thread), dieselben Wegpunkte `(0xA11E,0xA900)`, `(0x9981,0xA4D8)`, andere Zeiten. 6030 trägt eine Fassung mehr (sub11–13). Die Formulierung „fertig geschrieben, aber ihr Auslöser fehlt noch“ passt dort nicht.

### Was beim Betreten wirklich läuft

Der Weg ist 511 → 514 → 604 → 603, zurück 603 → 604, außerdem 604 → 514.

| Aus | Ziel | Spawn | Cut/Etage |
|---|---|---|---|
| 5140/5141 Slot 0 @0x1A1A/@0x1A12 | 604 | (-17970, 0, -20380), Richtung 1100 | Cut 0, Etage 0 |
| 6030/6031 Slot 0 @0x346 | 604 | (-9600, -1800, -23450), Richtung 2048 | Cut 2, Etage 1 |
| 604 Slot 0 @0x52E | 603 | (-26373, 0, -23487), Richtung 0 | Cut 0 |
| 604 Slot 1 @0x54E | 514 | (15368, 0, -9944), Richtung 1080 | Cut 13 |

- **6040:** `sub00` setzt die drei Figuren immer gleich, unabhängig von der Tür. Marvin steht rund 660 Einheiten neben dem Spawn aus 514. `sub06` startet, sobald Leon Zone 4 betritt (x -19730…-18130, z -25570…-19270, Spawn liegt 160 Einheiten östlich). Danach läuft Leon vier Wegpunkte ab. **In `sub06`/`sub07` gibt es keinen `Plc_neck`.** Leon blickt in der ausgeführten Szene weder John noch Sherry an.
- **6041:** `sub11` startet in Zone 3 (x -25130…-23470). Elza läuft nach (-23817,-25388), Sherry (`sub12`) sieht sie an, Elza blickt dreimal zwischen (-25090,-500,-25787) und (-23548,-500,-26229) hin und her. Das sind **nicht** die Figurenpositionen (John -26055/-25784, Sherry -24026/-26208), nur grob ihre Richtung. Marvin kommt in der ausgeführten Szene nicht vor.
- **603:** `sub01` startet die Szene im ersten Bild. Das `Ifel_ck` hat Länge 0. Das ist unschädlich: Im Folgebild schlägt `Ck` fehl, der Dispatcher springt auf dasselbe `Ck`, die Tiefe ist dann negativ und der Thread endet für dieses Bild (@`0x8003f13c`). Es hängt nichts.
- (5,x) wird beim Raumladen gelöscht. Die Szene läuft also bei jedem Betreten erneut.

Folge für Abschnitt 3 und 1: „Leon blickt sie an“, „Leon blickt ihn an, John geht“ (`@0x6EA`, `sub05`) und „Elza blickt ihn an“ (`@0x72E`) liegen alle in nicht gestarteten subs. Auf der Disc **stehen** die Figuren im Raum, mehr läuft nicht. Der Bericht sagt das erst in Abschnitt 6.

## 3. (b) Byte 4 von Sce_em_set

Handler `0x800420a0`, `s2=pc+2` (`0x800420f4`):

| Byte | Ziel | Beleg |
|---|---|---|
| `pc[1]` | Slot & 0x7F | `0x800420f8` |
| `pc[2]` | Typ, +0x08 | `0x800422bc` |
| `pc[3]` | Verhalten, +0x09 | `0x80042164` |
| **`pc[4]`** | **Etage +0x82; +0x1BA = -1800·`pc[4]`** | `0x800421d0`, Rechnung `0x800421f8–0x80042210` |
| `pc[5]` | ≠0: Status 0x2001 | `0x8004229c–0x800422b0` |
| `pc[6]` | +0x97 | `0x8004225c` |
| `pc[7]` | Kill-Bit, 0xFF = keins | `0x80042120–0x80042128`, +0x1C6 @`0x80042170` |
| `pc[8..13]` | Lage | `0x80042174–0x8004218c` |
| `pc[16..17]` | Richtung | `0x800421bc` |

Der Zensus bestätigt das: In den allermeisten Einträgen gilt `y == -1800·pc[4]` (`floor.py`). Die 21 Ausnahmen sind Parkplätze, Deckenkriecher und Sonderlagen. In `STAGE6.BIN` ist +0x1BA die Schattenhöhe (@`0x80100120`, Aufruf `FUN_8001b064`). `pc[3]=0x40` setzt beim Init Status 4/6, also Skriptfigur (@`0x801002b8–0x801002d4`).

**Folge:** „Marvin … hat ein anderes Startbyte, Sherry ebenfalls“ ist falsch. Marvin hat Etage 1 statt 0. Sherry hat in 6040 und 6041 dieselben Bytes `4b 40 00 00 00 ff`, nur Lage und Richtung unterscheiden sich.

**Ada in 1090/1091:** Etage 1 mit Lage y=0 auf dem Parkplatz. Leons Szene spielt in 109 auf Etage 1 (`sub12` setzt sie auf y=-1800). 1091 hat dasselbe `main00`. Das Byte sagt nichts über Story.

**Marvin in 6041 ist die einzige Figur außerhalb eines Parkplatzes mit Etage 1 und y=0.** Projiziert man (-21241, -26518) in die Hintergründe, trifft er die **Klappliege** (`ROOM6041_pts.jpg`, Cuts 3/4/5). Der Punkt liegt in SCA-Zelle 15 (x -23080…-18840, z -27221…-25221) und damit im festen Block der Liege. Elza läuft in `sub02`/`sub06`/`sub09` bis auf rund 180–260 Einheiten heran und blickt in `sub06`/`sub09` auf `y=0` hinunter (@0x72E, @0x798).

Das macht Abschnitt 6 konkreter: In Elzas Zug ist Marvin bei der Liege und Elza geht zu ihm. In Leons Zug steht er an der Tür aus 514. Pose und Anlass stehen nicht auf der Disc, das bleibt Deutung.

## 4. (c) Garage 11B0

**Slot 0 ist Sherry:** `Sce_em_set` @0xE90 `44 00 4b 40 …` auf Parkplatz (-30000,-30000). Typ 0x4B hat einen eigenen Blob von 139264 B, 99,7 % von `PL0F.PLD` liegen darin (`plcmp.py`). In Elzas 11B1 trägt derselbe Slot denselben Typ, dort nennt der Text sie Sherry.

**Wann läuft sub04:** `sub01` @0xF9A: `Ck(3,130)=0` und `Ck(4,6)=1`, dann `Evt_exec sub04`.
- (4,6) setzt `main00` selbst @0xE0E, unbedingt.
- (3,130) setzt sub04 @0xFEC, damit läuft es einmal.
- Weitere Schreiber von (3,130): nur 10B1 sub06 @0x13D8 (Elza), in Leons Spiel ohne Wirkung. 10B0 hat leere subs.
- Weitere Schreiber von (4,6): 1030 sub01 @0x205A, 1031 sub00 @0x1EB6. Für die Szene unerheblich.

**Erreichbarkeit:** Ja, beim ersten Betreten. Weg über Türen ohne Aot_reset: 1170 Slot 0 → 1160 Slot 1 → 1210 Slot 0 → 11E0 Slot 1 → 11B0. Die Szene: Cut 11 (Blick durch das Gitter), Sherry bei (-14886,-24048) (`sub06` @0x10A0), läuft nach (-5913,-23975) (`sub13` @0x11DA), Leon: Text 9 (`sub07`), danach `sub09` Rückkehr auf Parkplatz.

**Bestätigt:** „Leon sieht Sherry und ruft ihr nach“ ist auf der Disc wirklich ausgeführt. Das ist die einzige ausgeführte Begegnung mit dem Begleiter der anderen Seite.

**Neu, Marvin-Szene unerreichbar:**
- `sub14` (Texte 10–21) braucht (3,67)=1 und (3,131)=0 (@0xFB6, @0xFBE).
- (3,67) setzt nur 11C0 `sub05` @0x198E.
- `sub05` startet nur über `Ck(8,95)=1` und `Ck(8,96)=1` (@0x180A, @0x1812). Das sollen die Kill-Bits der Gorillas sein (`pc[7]`=0x5F, 0x60).
- Kill-Bits schreibt STAGE1 für Typ 0x27 nach `0x800b1038` = Bank 7 (@`0x8011b8ec`, `0x8011baf4`, `0x8011bd1c`). Die Auswahl im Spawn folgt `V[8]<3` (@`0x800420ec`), ebenso im Zombie-Pfad (@`0x80106dd4`).
- `Ck` mit Bank 8 liest `0x800b1058`. Kein Raum außer 11C0 nutzt Kill-Bit 95/96.
- Damit bleibt das Bit auf der Disc immer 0. Alle anderen Kill-Abfragen der Disc passen zur Bank (`kill.py`), 11C0 ist die einzige Abweichung.
- Die Mod hat die Stelle geändert: In `info/Re1.5/PSX/STAGE1/ROOM11C0.RDT` steht `21 07 60` @0x1836, also Bank 7. `21 08 5f` und `21 08 60` fehlen dort.

Folge: Unerreichbar sind Marvin in 11B („Leon! Du lebst!“, Kanaldeckel-Plan) und 11C #7–9. TEXTE_40 führt sie als „Leon“, das heißt nur: Das Skript ruft sie. Für die Frage „was war geplant“ ändert das nichts, für „was läuft auf der Disc“ schon. Statisch hergeleitet, nicht gemessen.

**Elza-Zweig:** 11B1 `sub00` hängt am selben Bild-Tausch wie Leon, aber an (3,131). Das setzt 10B1 `sub12` @0x1452 direkt vor `Aot_on(1)`. 11B1 `sub04` startet mit `V[3]==0x30` (@0xF94). `V[3]` ist die zuletzt aufgenommene Item-Id (`0x8001e0b4`, `0x80021fec`). Elza löst Sherrys Szene also durch das Aufheben von Item 0x30 aus.

**Abschnitt 4, „Dieselbe Garage“:** Beide Dateien nutzen `(0xC5DA,0xA210)`, das stimmt. „Beide Figuren sind also etwa zur selben Zeit dort“ folgt daraus nicht. Bei Elza steht Sherry und redet, bei Leon rennt sie weg. Gleiche Koordinate ist kein Zeitbeleg.

## 5. (d) Zelle 121

- **Objekt 0 ist eine Zellentür.** Mesh X[0,268] Y[-3600,0] Z[-1890,0], Drehpunkt (-21750,0,-20760). Mit der echten Kamera projiziert sitzt Drehung 0 exakt im Türrahmen (`ROOM1210_mark.jpg`, Cuts 5, 6, 8).
- **Drehung 0x4C0** ist 1216/4096 = 106,9° um Y, also aufgeschwenkt. 1211 @0x1E3C setzt mit `Member_set 4` denselben Wert.
- **(3,98):** Kein Schreiber auf der Disc, über alle in Kapitel 1 genannten Wege. Der Bericht nennt das bei (4,22), hier nicht. „Geht erst mit Flag (3,98) auf“ ist die Absicht im Skript. Auf der Disc passiert es nie, und niemand zeigt, dass Elzas Szenario das auslöst.
- **Die geschlossene Tür sperrt nichts.** Objekt-Box ist 0, SCA von 1210 und 1211 ist byte-gleich, die Wand hat bei z -22520…-20828 eine Lücke. Elza kann auf der Disc durch die Tür laufen.
- **Zeitlich heikel:** Die Zelle liegt auf Leons kürzestem Weg vom Dach (1170 → 1160 → 1210). Wenn die offene Tür Elzas Werk wäre, müsste sie das sehr früh getan haben. Nachbarbits: (3,96) schreiben 1021, 1150 und 1151, (3,97) schreibt Elzas 3061 (ohne Leser). Liefe die Nummerierung nach der Reihenfolge, läge (3,98) hinter 306. Das ist ein schwacher Hinweis gegen die Lesart, unklar.

Fazit: Die Tür ist bestätigt, „hart belegt“ gilt nur für die zwei Lagen, nicht für die Ursache.

Nebenbefund: 1211 `sub02` (Gitterhände wecken) hat keinen Auslöser, weil die AOT aus Leons `sub00` fehlt.

## 6. (e) Hintertür 3060/3061

**Bestätigt.** Slot 1 (Rechteck -4500,-27300,2600,1300) führt in beiden Dateien nach 309. Elza drückt dort: `Aot_reset(1, sce=3, sub=2)` @0xF0C, Text 0 „Abgeschlossen“, dann setzt `sub02` sie auf (-2991,-26170), also an genau diese Tür. Sherrys Satz (Text 7) gehört dazu. Gegenstück: 3091 Slot 1 führt nach (-2812,0,-26085), wieder an diese Tür.

**Ungenau:** „erzwingt den Zustand, den Elzas Szenario erst herstellt“. Elzas Dateien kennen (3,160) gar nicht. Ihr Zustand heißt (4,16). 3060 liest (4,16) nicht. In 3060 steht `Set(3,160)=1` @0xEFE direkt vor `Ck(3,160)=0` @0xF06. Der Zweig „verschlossen“ ist damit toter Code.

Dieses Muster ist kein Einzelfall, sondern Entwicklerpraxis:

| Raum | Setzt | Prüft | Szenario |
|---|---|---|---|
| 3060 | (3,160)=1 @0xEFE | @0xF06 | Leon |
| 1021 | (3,133)=1 @0x1CBC | @0x1CF0 | **Elza** |
| 1030 | (3,87)=1 @0x1EB2 | @0x204E | Leon |
| 11B0/11B1 | (4,6)=1 @0xE0E | @0xF60 | beide |
| 1210 | (4,14)=0 @0x1E0E | @0x1E16 | Leon |
| 4030 | (5,4)=0 dann 1 @0x528/@0x52C | sub01 | Leon, schaltet Szene ab |

Ein festes `Set` belegt deshalb allein nichts über Zapping. Tragfähig sind der **Text** „いつの間にか カギがはずされている“ und die Lage der Objekte.

**Neu, stärkere Spur als der Text:**
- Objekt 0 ist ein Lüftungsgitter (Mesh 90×630×810). Bei Elza mit (4,16)=0 sitzt es an der Wand (-19095,-495,-27045) @0xF52. Sherrys Szene löst es (`sub04`).
- In Elzas Zweig (4,16)=1 @0xFB4 und in Leons `sub00` @0xF74 liegt es am Boden, (-18317,-266,-26828), Kippung 72. Die 34 Byte sind **identisch**.
- Die drei Hunde (Kill 0x50–0x52) sind ebenfalls byte-gleich zu Elzas Zweig nach der Szene.
- Leons 306 ist damit Elzas 306 nach ihrer Szene (`ROOM3061_mark.jpg`).

## 7. (f) Schutzschalter 3090/3091

**Bestätigt:**
- 3091 setzt (4,8) @0x1672 (`sub05`, nach Ja bei Text 5) und (4,16) @0x1806 (Ende `sub07`, Rettung).
- 3090 setzt beide unbedingt @0x1412, @0x1416.
- (4,8)=0 in 3091 tauscht auf dunkle Cuts 8–14 (`Cut_replace`, @0x142E; `bg309.jpg` zeigt die dunklen Fassungen).
- 3090 `sub02–04` haben nur den Zweig (4,8)=1. Die Dunkel-Texte wurden in Leons Datei gestrichen. Das spricht für Absicht und gegen eine bloße Kopie.

**Zur Gegenrichtung:**
- (4,8) lesen nur 3090 und 3091.
- (4,16) lesen nur **3061** und 3091. In Leons Dateien liest es niemand. `Set(4,16)=1` @0x1416 ist in Leons Spiel wirkungslos. Es ergibt nur Sinn, wenn man die Welt als gemeinsam denkt, oder es ist ein mitkopierter Befehl. Unklar.

**Abschwächungen:**
- Der Text „Das Licht ist bereits an…“ ist kein Leon-Text. 3091 nutzt ihn ebenfalls (`sub03`, @0x1568), wenn man selbst eingeschaltet hat.
- Die Kiste (Objekt 0, pushbar, Typ 4) steht in 3090 an **Startposition** (-23550,0,-8024). In 3091 braucht man Etage 1 an Slot 6 (z -5500…-4390), also vermutlich die Kiste an anderer Stelle. Leons Raum ist demnach kein vollständiger Nachzustand. Das ist Herleitung, nicht gemessen.

**„Sherry spielbar“ in Abschnitt 5:** Auf der Disc bleibt die Figur Elzas Modell. In `sub07` stehen John und Typ 0x4B (Sherry) als NPC im Raum, und der **Spieler ist dort Elza**: John sagt „…シェリー お前は俺と来な“, dann gehen John und Slot 1 weg und der Spieler bleibt. Die Spielerrolle wechselt also inmitten des Raums ohne Figurentausch. Die Stelle ist als Sherry-Abschnitt gedacht, belegt aber nur über Text 8, 3 und den Spawn am Lüftungsschacht.

## 8. (g) Weltspuren in beide Richtungen

Paarvergleich aller Räume: `paare.txt`. Einzeldateien: 1060, 1180, 2030, 2070, 2090, 3020. 19 Paare sind im Skript byte-gleich (104, 107, 110, 115, 116, 119, 11A, 11E, 200, 301, 303, 304, 409, 503, 504, 506, 510, 511, 512).

### Elza zuerst, Leon sieht die Folgen

| Ort | Beleg | Stärke |
|---|---|---|
| 306 Tür | Text 9 @0xF9A | Stark (Text) |
| 306 Gitter | 3060 @0xF74 = 3061 @0xFB4, 34 Byte | Stark, **neu** |
| 306 Hunde | 3060 @0xF38–0xF60 = 3061 @0xF78–0xFA0 | Mittel, **neu** |
| 309 Licht | 3090 @0x1412, nur Zweig (4,8)=1 | Stark |
| 307 Stange | 3070 @0x3114 = 3071 Zweig (3,3)=1 @0x31AE. Bei Elza davor zwei Stangen in anderer Lage @0x3166/@0x3188, Szene `sub08` räumt sie | Mittel, **neu**. Leons Datei hat dieselbe Hallenszene, der Zusammenhang ist nicht eindeutig |
| 121 Zellentür | Lagen bestätigt, Ursache fehlt | Schwach |

### Leon zuerst, Elza sieht die Folgen

**109 Transporter, neu und am stärksten:**
- Objekt 3 sitzt exakt in der Hecktür des Polizeitransporters, Objekt 2 ist die aufgeschobene Tür (`ROOM1090_mark.jpg`, Cuts 6/7).
- Leon, (3,129)=0: Objekt 2 aus (@0x21E2), Objekt 3 sichtbar, Feuer (14 Typ 0x26) und Feuer-Cuts 8–15. Ada ruft von innen.
- Sind die Feuer aus (Typ 0x26 setzt beim Ende (5,29–31), `0x80116ac8`), startet `sub10`: Objekt 3 fährt auf, Ada ist frei.
- Danach Zweig (3,129)=1 @0x2320: Objekt 3 aus, Objekt 2 sichtbar.
- **1091 `sub00` @0x21BC besteht nur aus genau diesen 8 Byte** `2e 03 03 00 34 06 00 00`. Kein Feuer, keine Ada-Szene, Tür offen. Member 6 ist Statuswort (`sw a2,0(a0)` @`0x800411c8`), 0 blendet aus.

Elzas Datei gibt also unbedingt den Zustand vor, den Leon erst herstellt. Mit derselben Schlusslogik wie in Abschnitt 4 hieße das: Leon und Ada waren zuerst da.

**103 Rolltor, angelegt:** Leon erlebt den Durchbruch live (1030 `sub01`, `sub08`, setzt (4,15)). Elzas 1031 hat `sub01` leer und keinen eigenen Durchbruch, kennt aber @0x1EAE: `Ck(3,134)=1` → (4,15)=1, (4,6)=1 und dieselben `Cut_replace`/`Sca_id_set`. Ihr Raum kann den Zustand also übernehmen. (3,134) hat keinen Schreiber. Tot auf der Disc.

**117 Kisten, Leon früher:**
- Elza und John stoßen zwei Kisten vom Dach (Text 1, `sub10`, `sub11`), dann `Aot_on(1)` nach 10B.
- Leons 1170 `sub00` hat dieselben Objekte byte-gleich (@0x8E8 = 1171 @0x946) und schaltet Slot 1 ab (@0x9B8).
- Bei Leon stehen die Kisten also noch. Elzas Zweig (3,0)=1 hat keine mehr.
- Einschränkung: Die Tür-Sperre kann auch reine Levelführung sein.

### Startorte

Neu aus dem EXE: Leon startet in 117, Elza in 103 (`0x8001d2a4` `bltz` auf Bit 31 von `0x800aca3c`, `0x8001d2b0` `V[9]=0x17`, `0x8001d32c` `V[9]=3`). Das passt zum Bild: Leon beginnt oben, Elza unten, ihre Wege überlappen zeitlich. Die Tabelle in Abschnitt 5 beginnt für beide mit dem Dach und nennt die Startorte nicht.

### Sonstige Unterschiede

| Ort | Befund | Urteil |
|---|---|---|
| 40A1 | Item 0x3C nur bei Elza @0xED0, Icon trägt 仮 | Unklar, Leon hat es nirgends |
| 50A0, 5140 | Nur Leon: Deckenplatte 2880×1440 und Gitter 1800×900, Elza hat die Modelle gar nicht (nOmodel 0 bzw. 1) | Unklar, kein Skript nutzt sie |
| 20B0/20B1 | Tür Slot 2 und zwei Türobjekte verschieden, aber **keine Tür führt nach 20B** | Beide unerreichbar. „John im Skript (20B1)“ in Abschnitt 5 läuft auf der Disc nie |
| Flags | Einziger Fall Leon liest, nur Elza setzt: (3,5). Bank-3-Bits sind in 3070/3071 nur verschoben belegt, keine Spur | Kein Befund |
| (4,21) | Nur Leon setzt, 3071 liest. Steuert Aufzugsziel | Kein Weltzustand der Gegenseite |

Gegnerlisten liefern außer 109, 3060 und 1171 nichts. Tote Gegner überträgt die Disc nicht, Kill-Bits sind spielstandlokal.

## 9. (h) Flags (4,22) und (4,23)

**Bestätigt:** Keine Schreiber. Leser nur 6030 @0x396, @0x39E und 6031 @0x38C. Bank-4-Referenz in Code nur Nullung @`0x8003e560`. Bedingungsformel in Abschnitt 6 ist korrekt.

**Unklar bleibt die Bedeutung.** Belegt ist nur die Struktur:
- (4,22)=0: Leon 0x4D, Elza Sherry.
- (4,22)=1: Elza Ada. Leon Ada nur, wenn (4,23)=0.

Die direkten Nachbarn gehören zu (4,19) 40A, (4,20) 400, (4,21) 307/30E/308 und (4,25) 403, also Stage 3/4. Die Reihe ist aber nicht nach Stage sortiert: (4,36) gehört zu 11E, (4,64) zu 11C. Daraus folgt nichts Sicheres.

„Der Setzer lag in nicht vorhandenen Räumen“ ist eine Vermutung, ebenso „Ihr Überleben oder ein Routenwechsel“. Beides gehört als unklar markiert. Ebenfalls möglich ist ein unfertiger Testzweig.

Nebenbei zu Typ 0x4D: `STAGE5.BIN` registriert ihn auch (`0x8011d6d4`), kein Raum spawnt ihn dort. Er war also auch im Labor vorgesehen. Das passt besser zur Frau im Kittel als zu einem NPC-Ersatz für Elza, beweist es aber nicht.

## 10. Weitere Zeilenprüfung

| Stelle | Ergebnis |
|---|---|
| Abschn. 3 Adressen | Alle stimmen |
| Abschn. 6 „exakt“ | Sherry exakt. John weicht in x um 1 ab (`0x9A3A` zu `0x9A39`). Höhen: Sherry -1200, John -1000, also tiefer als das Kind |
| Abschn. 6, 11 sub11 „zwischen John und Sherry“ | Nur ungefähr, siehe Kapitel 2 |
| Abschn. 7 PL | Bestätigt: PL0D/0x40 99,4 %, PL0E/0x49 98,0 %, PL0F/0x4B 99,7 % (Gegenprobe 0,7–18 %) |
| Abschn. 7 Marvin | Bestätigt: 3000 @0x161A, 3001 @0x1630. `V[0x10]` schreibt kein Skript (`vars.py`). TITLE.BIN setzt nur 0 oder 4 (`0x801024c0`, `0x801024d4`) |
| Abschn. 7 Lader | Bestätigt: `0x80039768`. Scenario-Wahl: Bit 31 von `0x800aca3c`, SCD-Flag (1,0), kein Skript schreibt es |
| Abschn. 8 4030 | Bestätigt: @0x528, @0x52C |
| Abschn. 8 501 #6–16 | Bestätigt |
| Abschn. 8 Parkpositionen | Bestätigt. Spiegelfall fehlt: John in **Leons** 10B0 ebenso, gleiches `main00` |
| Abschn. 8 „aus 3001 kopiert“ | Ungenau: Eigene Lage (-24458,-16261 gegen -23363,-14359) und eigene Texte, Kopierrichtung nicht belegbar. Platzhalter bleibt plausibel |
| Abschn. 4 gleiche Zeile | Ungenau: 401 #18 und 501 #43 sind Untersuchen-AOTs in **beiden** Dateien beider Räume, gleiches `main00`. Elza liest die Zeile auch in 401, Leon auch in 501 |
| Abschn. 12 „25 Zeilen“ | **Falsch: 23.** 5060/5061 rufen #7 und #9 über lange AOTs (Flag 0x80, Nutzlast `pc+22`). `quer.py` liest fest `pc+14`. TEXTE_40 ist bei 506 #7 und #9 zu korrigieren. #9 ist der Entwicklervermerk, er wird also angezeigt |

## 11. Vorschläge für BERICHT.md

**Abschnitt 1, Reihenfolge:**
> Die Spuren zeigen in beide Richtungen. In der Fabrik ist Elzas Gruppe zuerst da (Tür und Gitter in 306, Licht in 309). Im Revier ist es umgekehrt: Elza findet den Transporter in 109 offen und ohne Feuer, also so, wie Leon ihn hinterlässt, und Leon sieht auf dem Dach noch die Kisten, die Elza und John später hinunterstoßen. Beide Wege überlappen sich zeitlich.

**Abschnitt 1, Begegnungen:**
> Auf der Disc ausgeführt ist nur Leon–Sherry in 11B. In 604 stehen die Figuren im Raum, ihre Blick- und Laufszenen haben keinen Auslöser.

**Abschnitt 4, Einleitung:**
> Mehrere Stellen zeigen, dass eine Datei den Zustand vorgibt, den das andere Szenario herstellt. Ein festes `Set` allein ist kein Beleg, das Muster steht auch ohne Bezug zur Gegenseite im Skript (1021, 1030, 4030). Tragend sind Text und Objektlage.

**Abschnitt 4, Zeile Zelle:** Ergänzen:
> Schreiber von (3,98) fehlt auf der Disc, die geschlossene Tür hat keine Kollision.

**Abschnitt 5:**
- Neue Zeile oder Hinweis zu den Startorten Leon 117, Elza 103.
- Bei 11B: Marvin-Teil als „angelegt, auf der Disc wegen Bank 8 in 11C0 nicht erreichbar“.
- Bei 309: „als Sherry-Abschnitt angelegt, Figur bleibt Elza“.
- Bei Kanal: 20B ohne Zugang.

**Abschnitt 6:**
> Marvin steht in 6041 auf Etage 1 bei der Klappliege, Sherry nur an anderer Stelle.

Und statt „fertig geschrieben“:
> In 604 fehlen die Abfragen für zwei von drei Zonen. In 603 sind sub02/05/08 ältere Fassungen derselben Szene.

„Der Setzer lag in nicht vorhandenen Räumen“ und „Überleben oder Routenwechsel“ als Vermutung kennzeichnen.

**Abschnitt 12:** 23 statt 25, 506 aus der Liste nehmen.

## 12. Was offen bleibt

- Bedeutung von (3,98), (3,134), (4,22), (4,23).
- Ob die Gegenrichtung in 109 eine bewusste Zeitlinie ist oder nur ein aufgeräumter Elza-Raum. Für Absicht spricht, dass das Skript eine Tür aktiv abschaltet. Beweisen lässt es sich nicht.
- Marvins Pose in 6041 und warum Etage 1 gesetzt ist. Die Kollision legt nahe, dass er so im festen Block der Liege stehen kann. Gemessen ist das nicht.
- Item 0x3C in 40A1, Deckenplatte und Gitter in 50A0/5140.
- Alle Erreichbarkeits-Aussagen sind statisch. Ein Lauf im Emulator steht aus.

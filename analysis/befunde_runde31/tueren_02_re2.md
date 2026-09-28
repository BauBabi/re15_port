# T2 — RE2-Tuersemantik + Port-Anschluss-Plan (Runde 31)

Auftrag: analysis/befunde_runde31/AUFTRAG.md (Nutzer woertlich). Teil T2: zu einer RE1.5-Tuer mit
zugeordnetem RE2-Archiv die RICHTIGE Variante, den RICHTIGEN Griff, den RICHTIGEN Ton — auch fuer
Tueren zwischen zwei Raeumen. Kein Spielcode; Werkzeuge unter re15_port/tools/tueren/re2_*.py.

Stand: 2026-09-29, Abschnitte 0-7 + Anhang fertig (Kurzfassung ganz oben).

## Kurzfassung

1. **Variante <-> Griff (Abschnitt 1).** In der Standardgruppe (26 einfluegelige Drehtueren: 00 01 02 03 04 05 06 07 08 09
   0A 0B 0D 13 15 17 18 1A 1C 1D 22 23 24 29 2E 2F) zeigt **V0 Angel rechts / Griff links / Blatt geht weg** (aufdruecken),
   **V1 Angel links / Griff rechts / Blatt kommt hin** (aufziehen) - beides von der Raumseite, vor der Tuer stehend.
   Doppeltueren V2/V3 (ein Fluegel) bzw. hin/weg (beide Fluegel), Schott 26/31 V0..V3, Treppe/Leiter **V4 hinauf,
   V5 hinab**. Tabelle aller 152 Varianten in Anhang A1, Bilder `tueren_belege/t2_varianten_anfang_mitte.jpg`.
2. **Paar-Regel (Abschnitt 2).** 572 Saetze -> 534 Seiten -> 238 physische Tueren; beide Seiten haben immer dasselbe
   Archiv; Varianten 0/1 in 151, 4/5 in 33, **0/0 in 33** (25/26/27/2B/31), 2/3 in 18, 2/2 in 3. Stichprobe im RE2-Hintergrund:
   **24 von 25** eindeutig gesehenen Seiten haben den gemalten Griff dort, wo die Variante ihn zeigt (12 Archive). ->
   **Regel fuer RE1.5: jede Seite bekommt ihre Variante aus IHREM gemalten Griff (links -> V0, rechts -> V1), ohne
   Komplementaer-Pflicht.** RE2 spiegelt nie.
3. **Griffe (Abschnitt 3).** 5 Formfamilien sind zwischen Archiven bytegleich (Knauf A, Knauf B, Druecker, Stange,
   Druecker flach); Anhaengepunkt immer x = +-130 am Standardblatt. Griff-Form falsch -> Griff-Mesh tauschen; RE2 tut
   das selbst per **Bit 7** in DOOR01/DOOR05 (gleicher Anhaengepunkt, gleiche Textur); archivuebergreifend muss der
   Beschlagstreifen v 219..255 des Spenders mit.
4. **Ton (Abschnitt 4).** Tonkopf in allen 55 Archiven identisch (Satz 0 = Skript-Se_on Stimme 22, Satz 1 = Door_exit
   Stimme 23); 7 Tonfamilien (35 verschiedene Tonteile); Aufbau = TORSE-Aufbau, direkt ladbar. Treppen: drei
   Schrittgeraeusche, Leiter: vier Sprossen, kein Schliesston.
5. **Port (Abschnitt 6).** Maschine nimmt jedes DOORxx.DO2 unveraendert; es fehlen: Archiv aus Datei + Tabelle
   @0x8009a520, Bit 7 an die Maschine, DOOR28-Seitenkorrektur, Member-/Vibrations-Opcodes, Flags 0x2000/0x4000/0x0100,
   Anfrage im Kreuz-Raum-Zweig, Einblendung erst nach dem Raumwechsel, Tuertoene ueberleben `re15_audio_load_room_banks`
   (RE2 @0x800597a4: nur SPU 0x14441..0x3DC4F wird abgeschaltet, Tuerbank 0x3DC50). Schrittfolge 6.2.

## 0. Quellen und Werkzeuge

Herkunftsmarken: `@0x8...` = selbst disassemblierte Instruktion (RE2 = `info/re2leon/PSX.EXE`, t_addr 0x80010000,
`.claude/skills/re15-psx-disasm/scripts/re2_disasm.py`); `Datei 0x...` = Byte-Offset; `[SIM]` = Ergebnis des
Skript-Simulators `re15_port/tools/tor/tuerkatalog.py` (VM, Handler je Opcode mit PC-Vorschub-Adresse, siehe
analysis/tor_1170/03/04); `[BILD]` = selbst angesehenes Bild.

| Werkzeug (neu, nur lesend) | Zweck | Ausgabe |
|---|---|---|
| `re15_port/tools/tueren/re2_tuer_zensus.py` | alle 572 RE2-`Door_aot_set`/`_4p` lesen, Seiten bilden, zu physischen Tueren paaren | `build/r31_tueren/t2/re2_tueren.json` |
| `re15_port/tools/tueren/re2_tuer_varianten.py` | je Archiv und Variante: Rollen der Objekte, Angel/Griff im Bild, Richtung, Bilder Anfang/Mitte | `build/r31_tueren/t2/varianten.json`, `bilder/DOORxx_vN.png`, `bogen_varianten_NN.png` |

Satzformat (selbst gelesen):
- `0x3B Door_aot_set`: Handler `0x80054be4` (Tabelle `0x800a74c8[59]`), Satzzeiger = pc+2 (`@0x80054c30 addiu v0,v0,2`),
  Laenge 32 (`@0x80054c40 addiu v0,v0,32`). Rechteck pc+6..13, Nutzlast pc+14.
- `0x68 Door_aot_set_4p`: Handler `0x80054c50` (Tabelle `[104]`), Satzzeiger pc+2 (`@0x80054c9c`), sat |= 0x80
  (`@0x80054cb4 ori v0,v0,0x80`), Laenge 40 (`@0x80054cc4 addiu v0,v0,40`). Vier Punkte pc+6..21, Nutzlast pc+22.
- Nutzlast +12 = Archiv (`@0x80015088 lbu v1,12(v0)`), +13 = Variante & 0x7f / Bit 7 (`@0x80013e5c`, `@0x80013e6c andi 0xff7f`,
  `@0x80013e84 andi 0x80`), +15/+16 Schloss (`@0x800515a8`, `@0x800515d0`); Rest 03_tuersequenz 4.
- Tuerkamera: Auge (10000,0,0), Ziel 0, H 290 -> Bild-x = 160 + 290*z/(10000-x) (03 3.2; Bildmitte (160,120) aus den
  Spiel-Aufrufern `@0x80049cac/b0`, `@0x80068e80/88`).


## 1. Variante <-> Griff je Archiv (Angel/Griff links/rechts, weg/hin, Meshes)

**Verfahren.** `re2_tuer_varianten.py` faehrt jede Variante, die Skript 0 verteilt (Case-Werte des `Switch var 12`),
durch den Simulator `[SIM]` und nimmt Bild t0 (erstes Bild mit allen Objekten) und das Bild, in dem die Bewegung des
Blatts endet (Kamerafahrt herausgerechnet). Angel = Ursprung des bewegten Blatt-Meshes (03 9.2: Ursprung = Drehachse),
freie Kante = Blatt-Ecke mit dem groessten |z|, Griff = Kind-Objekt des Blatts (Flag 0x10, Eltern = Blatt) mit
Mesh-Hoehe < 3000. Bild-x nach der Tuerkamera (Abschnitt 0). "links" < 150, "rechts" > 170, dazwischen "mitte".
"weg"/"hin" = die freie Kante geht in der Offen-Stellung um mehr als 50 Einheiten nach -x / +x (Kamera steht bei +x).
Sichtbar ist ein Griff, wenn sein Weltpunkt in t0 vor der Blattebene liegt (x groesser als die Angel).
Die Tuerkamera zeigt die Tuer so, wie der Spieler VOR ihr steht: "Griff links" heisst also links, wenn man die
Tuer von dieser Raumseite ansieht.

Bilder: je Archiv/Variante `build/r31_tueren/t2/bilder/DOORxx_vN.png` (Bild t0 mit Marken Angel/Griff | Bild Mitte der
Bewegung); Uebersicht aller Archive (je zwei Seiten) im Repo: `analysis/befunde_runde31/tueren_belege/t2_varianten_anfang_mitte.jpg`
`[BILD]`. Vollstaendige Tabelle aller 152 Varianten (inkl. Bit-7-Laeufe) in Anhang A1.

### 1.1 Ergebnis nach Gruppen

| Gruppe | Archive | V0 | V1 | weitere Varianten |
|---|---|---|---|---|
| Einfluegelige Drehtuer, Standardaufstellung | 00 01 02 03 04 05 06 07 08 09 0A 0B 0D 13 15 17 18 1A 1C 1D 22 23 24 29 2E 2F (26) | Angel **rechts** (Bild-x 226..234), Griff **links**, Blatt geht **weg** (aufdruecken) | Angel **links** (86..107), Griff **rechts**, Blatt kommt **hin** (aufziehen) | s. u. |
| Doppeltuer, ein Fluegel geht | 01 04 06 15 1D: V2 / V3; 11: V0=V2, V1=V3 | V2: rechter Fluegel (Angel x 289/290) geht weg, linker steht | V3: linker Fluegel (Angel x 30) kommt hin, rechter steht | 15 V2/V3 wie 01 |
| Doppeltuer, beide Fluegel gehen | 0C (V0 hin, V1 weg), 1B (V0=V2 hin, V1=V3 weg), 0D (V2 hin, V3 weg), 30 (V0 weg, V1 hin) | | | Angeln aussen (x 26/289) |
| Schott mit Handrad | 26, 31 | V0: zweiteilig, linker Teil (Angel x 51) geht weg, Rad links; V1: Angel rechts (269), Rad rechts, hin | | V2: einteilig, Angel links (106), Rad rechts, weg; V3: Angel rechts (214), Rad links, hin |
| Schiebetuer | 10 (Scherengitter), 14 (Maschendraht), 19, 27, 2A, 2C | 10: Pfosten links, schiebt nach links; 14: nach links, Griff rechts; 19: nach rechts; 2A: nach rechts | 10: Pfosten rechts; 14: nach rechts, Griff links; 19: nach links; 2A: nach links | 27 V0..V3 gleich (zweiteilig); 2A V2/V3, 2C V0..V3 zweiteilig |
| Hub-/Klapptuer | 1F (zwei Platten), 25 (Aufzugtuer, hebt), 2B (Klappe, hebt) | 1F: Griffplatte links; 25/2B nur V0 | 1F: Griffplatte rechts | - |
| Klappe / Lueftung | 1E, 33, 35 | V0/V1 (Skript 1, gleich), V6 (Skript 2, naeher), V7 (nur Rahmen), V8 (Kette, Kamerafahrt) | | Klappe dreht um z (-1600) |
| Treppe | 0E, 0F, 12 | - | - | **V4 = hinauf** (Stufen mit Setzstufen, Modell sinkt und kommt naeher), **V5 = hinab** (Trittflaechen unter dem Horizont) |
| Leiter | 16 | - | - | **V4 = hinauf** (Leiter faehrt nach unten, y +300), **V5 = hinab** (y -300) |
| Bodenluke | 28 | V0/V2/V4 (Skript 2, 281 Bilder) | V1/V3/V5 (Skript 1, 301 Bilder) | Kamerafahrt, keine Blattbewegung |
| Hubbuehne | 2D | V0/V2/V4 (Skript 1, 301) | V1/V3/V5 (Skript 2, 451) | Kamerafahrt |
| Ohne Objekt | 20 21 32 34 36 | nur Blende + Ton (271..321 Bilder) | | Uebergaenge per Skript (Rechteck 0/20000/25000) |

Beleg der Standardgruppe am Skript (Datei-Offsets `[SIM]`, Bytes selbst gelesen): DOOR00 Skript 1 @Datei 0x5036
`4d 00 00 00 01 00 a0 0a 10 00 d0 07 ce 0e 00 08 ...` = Blatt Obj 0 bei (2000,3790,2048) Drehung 0; Knauf Obj 1 als Kind
bei (130,-3224,-3372). Bild-x Angel = 160 + 290*2048/8000 = 234,2; Knauf 160 + 290*(2048-3372)/(10000-2000-130) = 111.
Skript 2 stellt das Blatt bei z = -1600 mit Drehung y = 2048 auf -> Angel 160 - 290*1600/8000 = 102.

### 1.2 Meshes: Blatt / Griff / Anbauteil

- **Blatt:** 36 Archive benutzen als Mesh 0 dieselbe Platte (8 Ecken, 12 Dreiecke, 288 x 6602 x 3599; 34 davon auch mit
  denselben UV): 00 01 02 03 04 05 06 07 08 09 0B 0C 0D 11 13 15 17 1A 1B 1C 1D 20 21 22 23 24 25 29 2A 2C 2F 32 34 36
  (+ 18/19 geometriegleich, andere UV). Unterschied zwischen diesen Tueren = nur Textur (Blatt v 0..218) und Beschlag.
- **Griff/Beschlag:** Mesh 1 (seltener 2/3), immer Kind des Blatts (Flags 0x00d0/0x04d0 = Eltern Obj 0), Anhaengepunkt
  x = +130 (Vorderseite) bzw. -130 (Rueckseite, Drehung x 2048 oder z 2048), y -2464..-4234, z -2660..-3594
  (= 5..940 Einheiten von der freien Kante z = -3599). Tabelle Abschnitt 3.
- **Anbauteile:** fester zweiter Fluegel (Wurzelobjekt ohne Bewegung: 01/04/06/11/15/1D V2/V3), Pfosten (10 Mesh 1),
  Schild/Tafel (1F Mesh 2/3, 29 Mesh 3), zweite Platte (1F Mesh 1), Rahmen (1E/33/35 Mesh 1), Fahrkorb/Gelaender (2D).
- **Vorder- und Rueckseitengriff:** V1 setzt bei 00 01 02 03 04 05 06 08 09 0A 0B 0D 11 13 15 17 18 1A 1C 1D 23 24 2E 2F
  (24 Archive) zwei Griff-Objekte (vorn + hinten; das Blatt zeigt beim Aufziehen seine Rueckseite), V0 nur den vorderen.
  Nur einen Griff in V1: 07 0C 14 19 1B 26 2C 31. (29 V1: Tafel vorn + hinten, Schild; 1F: drei Kinder.)

## 2. Paar-Regel in RE2 (572 Door_aot_set -> physische Tueren, Variante je Seite, Griff im Hintergrund)

### 2.1 Zensus und Paarung (`re2_tuer_zensus.py`)

- **572** Saetze (552 x 0x3B, 20 x 0x68) in 237 RDT-Dateien (von 250, die der Walker `re2_scd_walk.all_subs` liefert).
  Varianten-Verteilung 0:253, 1:178, 2:27, 3:22, 4:44, 5:45, 7:1, 8:2, Bit 7 in 14 - identisch mit 03 Abschnitt 4.
- **Seite** = gleiches Viereck + gleiches Ziel (RE2 setzt dieselbe Seite oft mehrfach, je Szenario-Flag in einem anderen
  Sub): **534** Seiten.
- **Paar** = Seite A (Raum R1, Ziel R2) + Seite B (Raum R2, Ziel R1) mit kleinstem Abstand "Ziel von A im Viereck von B"
  + "Ziel von B im Viereck von A", gegenseitig beste Wahl, Ziel-Datei in derselben Reihe (ROOM1..7 bzw. ROOMA..G).
  **238 Paare**, 58 Seiten ohne Gegenstueck (Skript-Uebergaenge mit Null-/Fernrechteck, Einbahn-Tueren,
  Aufzugschaechte mit mehreren Zielen).
- **Beide Seiten eines Paares tragen IMMER dasselbe Archiv** (238 von 238).

Variantenpaare (ungeordnet):

| Varianten A/B | Paare | Archive |
|---|---|---|
| 0 / 1 | 151 | 00 01 02 03 05 06 07 08 09 0A 0B 0C 0D 10 13 14 17 18 19 1A 1C 1D 1F 22 23 24 29 2A 2E 2F 30 |
| 4 / 5 | 33 | 0E 0F 12 16 28 (Treppe, Leiter, Luke: eine Seite hinauf, die andere hinab) |
| 0 / 0 | 33 | 25 (15), 26 (10), 27 (4), 2B (2), 31 (2) |
| 2 / 3 | 18 | 04 06 0D 11 15 1B 1D 26 (Doppeltueren / Schott einteilig) |
| 2 / 2 | 3 | 2A (1), 2C (2) (zweiteilige Schiebetueren) |

Pro Archiv und Paar: `build/r31_tueren/t2/re2_tueren.json` (`paare`, `seiten`); Zaehlung je Archiv in Anhang A2.

### 2.2 Lesart: die Variante folgt dem GEMALTEN Griff, nicht der Physik

Bei 0/1 (und 2/3) sind beide Seiten physisch stimmig: V0 zeigt von Seite A Angel rechts + aufdruecken, V1 zeigt dieselbe
Tuer von Seite B mit Angel links + aufziehen (Abschnitt 1.1) - genau die Rueckseite. Die 33 Paare 0/0 sind es NICHT
(von beiden Seiten Angel an derselben Bildseite, beide aufdruecken). Beispiel DOOR26 (Schott "TYPE-P", Paar
ROOM6020/ROOM6030): beide Hintergruende zeigen das Handrad links (Stichprobe Nr. 18/19) - RE2 malt beide Seiten gleich
und nimmt beide Male V0 (Rad links). Die Hub-/Schiebetueren 25/27/2B sehen von beiden Seiten ohnehin gleich aus.

### 2.3 Stichprobe: Griff im RE2-Hintergrund gegen die Variante `[BILD]`

`re2_tuer_hintergrund.py` projiziert das Tuer-Viereck in jede Kamera des Raums (RID-Satz 32 B ab RDT+0x24,
H = fov >> 7, Modell wie `tools/re2_sicherung/re2_aot_on_bg.py`), waehlt die Kamera, die am ehesten GEGEN die
Eintrittsrichtung der Gegenseite blickt (Blickvektor (cos d, -sin d) wie der Laeufer FUN_800245d8,
`engine/src/actor_locomotion.c:340-344`), und legt beide Seiten eines Paares mit vergroessertem Ausschnitt nebeneinander
(`build/r31_tueren/t2/hg/paarNNN_*.png`, 128 Paare der Reihe A). Selbst angesehen, Ausschnitte im Repo:
`analysis/befunde_runde31/tueren_belege/t2_stichprobe_griff.jpg` (`re2_tuer_hintergrund.py --stichprobe`).

| # | Raum / Kamera | Archiv, Variante | Griff im Bild (vor der Tuer stehend) | passt |
|---|---|---|---|---|
| 1 | ROOM1090 c6 | DOOR00 V1 | rechts (frontal) | ja |
| 2 | ROOM2080 c0 | DOOR00 V0 | links (frontal) | ja |
| 3 | ROOM2070 c4 | DOOR00 V1 | rechts (Tuer in der rechten Wand, Knauf an der nahen Kante) | ja |
| 4 | ROOM2050 c8 | DOOR00 V0 | links (frontal) | ja |
| 5 | ROOM2070 c2 | DOOR00 V1 | rechts (von oben) | ja |
| 6 | ROOM10E0 c0 | DOOR00 V1 | rechts (frontal) | ja |
| 7 | ROOM20E0 c0 | DOOR08 V0 | links (frontal) | ja |
| 8 | ROOM1080 c4 | DOOR07 V1 | rechts | ja |
| 9 | ROOM1140 c0 | DOOR03 V0 | links (frontal) | ja |
| 10 | ROOM5010 c3 | DOOR1A V1 | rechts (Druecker + Kartenleser) | ja |
| 11 | ROOM5020 c0 | DOOR1A V0 | links | ja |
| 12 | ROOM2000 c9 | DOOR0D V0 | links (frontal) | ja |
| 13 | ROOM20A0 c5 | DOOR09 V0 | links (Tuer in der linken Wand, nahe Kante) | ja |
| 14 | ROOM20C0 c0 | DOOR09 V1 | rechts (frontal) | ja |
| 15 | ROOM1000 c8 | DOOR0B V0 | links (Stangengriff) | ja |
| 16 | ROOM1010 c5 | DOOR0B V1 | rechts (Stangengriff) | ja |
| 17 | ROOM21B0 c4 | DOOR05 V1 Bit 7 | rechts, DRUECKER (Bit 7 waehlt den Druecker, 3.2) | ja |
| 18 | ROOM6020 c6 | DOOR26 V0 | Handrad links | ja |
| 19 | ROOM6030 c0 | DOOR26 V0 | Handrad links (Gegenseite, ebenfalls V0) | ja |
| 20 | ROOM3030 c9 | DOOR26 V2 | Baender links, Rad rechts (einteilig) | ja |
| 21 | ROOM3030 c0 | DOOR24 V0 | links (schraeg) | ja |
| 22 | ROOM30B0 c2 | DOOR24 V1 | rechts (frontal) | ja |
| 23 | ROOM20C0 c3 | DOOR1C V0 | links (frontal) | ja |
| 24 | ROOM20E0 c3 | DOOR1C V1 | rechts (frontal) | ja |
| 25 | ROOM10D0 c0 | DOOR05 V0 Bit 7 | Griff an der FERNEN Kante einer Tuer in der linken Wand = rechts; sehr flacher Blick | **eher nein** |

Nicht entscheidbar (zu flach oder verdeckt): ROOM10A0 c0 (DOOR00 V0), ROOM20C0 c6 (DOOR08 V1), ROOM1090 c15 (DOOR07 V0),
ROOM1130 c0 (DOOR03 V1), ROOM20A0 c0 (DOOR0D V1), ROOM4050/4070 (DOOR0A: Riegelkasten mittig), ROOM3040 c0 (DOOR26 V3).

**Ergebnis: 24 von 25 eindeutig gesehenen Seiten passen** (Griff links <-> V0, Griff rechts <-> V1; beim Schott die Lage
von Rad/Baendern gegen V0..V3), quer ueber 12 Archive (00 03 05 07 08 09 0B 0D 1A 1C 24 26). Die eine Abweichung (ROOM10D0) ist die Seite eines 0/1-Paares,
dessen Gegenseite (ROOM21B0, Nr. 17) passt: dort hat RE2 die Physik (komplementaer) ueber das Bild gestellt - oder der
Blick ist zu flach, um es sicher zu sagen. Eine Regel "0 und 1 immer komplementaer" saehe dagegen die 33 Paare 0/0 falsch.

### 2.4 Regel fuer eine RE1.5-Tuerseite

Gegeben: RE1.5-Tuerseite (Raum inkl. Variante, Rechteck, Band), ihr zugeordnetes RE2-Archiv (T1/T3) und der im
RE1.5-Hintergrund GEMALTE Griff dieser Seite (T1-Ausschnitt), gesehen wie der Spieler vor der Tuer steht.

1. **Einfluegelige Drehtuer (Standardgruppe, 26 Archive):** Griff links -> **V0**, Griff rechts -> **V1**. Das ergibt
   Angel rechts + aufdruecken bzw. Angel links + aufziehen wie RE2 (1.1). Die zweite Seite derselben Tuer bekommt ihre
   Variante UNABHAENGIG aus IHREM Bild - ist die Tuer stimmig gemalt, entsteht von selbst das RE2-Paar 0/1.
2. **Doppeltuer:** geht nur ein Fluegel: Griff/Fluegel rechts -> V2 (rechter geht weg), links -> V3 (linker kommt hin)
   bei 01/04/06/15/1D; bei 11 V0 bzw. V1. Gehen beide Fluegel: 0C/1B/0D/30 - die eine Seite "weg", die andere "hin"
   (RE2-Paare 2/3 bzw. 0/1), welche Seite welche, ist aus dem Bild nicht ablesbar -> wie RE2: Seite A weg, Seite B hin.
3. **Schott 26/31:** zweiteilig V0 (Rad links) / V1 (Rad rechts); einteilig V2 (Baender links, Rad rechts) / V3.
4. **Schiebetuer:** 14 V0 laeuft nach links (Griff rechts), V1 nach rechts (Griff links); 2A V0 nach rechts, V1 nach
   links; 19 V0 nach rechts, V1 nach links; 10 V0 Pfosten links, V1 Pfosten rechts; 27/2C zweiteilig (alle Varianten
   gleich bzw. 2/2 von beiden Seiten).
5. **Treppe/Leiter:** hinauf -> V4, hinab -> V5 (Abschnitt 5).
6. **Bit 7 (nur 01/05):** waehlt die Griff-FORM: DOOR01 Bit 7 = 0 Druecker (Mesh 1), = 1 Knauf (Mesh 3); DOOR05 Bit 7 = 0
   Knauf (Mesh 1), = 1 Druecker (Mesh 2 vorn / 3 hinten). Nach dem gemalten Griff waehlen (3.2).
7. Keine Komplementaer-Pflicht zwischen den Seiten; RE2 selbst hat 36 Paare mit gleicher Variante (0/0, 2/2).

### 2.5 Wenn keine Variante trifft - was waere noetig, und tut RE2 das?

| Fall | Abhilfe | RE2 tut das? |
|---|---|---|
| Griffseite | tritt bei den 26 Standard-Archiven nicht auf (beide Seiten vorhanden) | - |
| Griffseite richtig, aber die sichtbare Aufgehrichtung falsch (z. B. Angel links + aufdruecken) | Blatt spiegeln: Ecken z -> -z UND Dreiecke umgekehrt umlaufen (sonst wirft NCLIP @0x800148d0 / @0x800148f8 bgez die Vorderseite weg), Textur waagerecht spiegeln (gemalte Beschlaege, Schrift kehren sich um) | **Nein.** `Door_model_set` traegt nur Lage + Drehung (03 5.1), RotMatrix @0x8008e1f4 ist eine reine Drehung; die einzige Skalierung, Flag 0x2000 (Ecken-z * var 4 / 256, @0x80014794..f0), nutzt nur DOOR10 mit positivem Faktor. RE2 nimmt stattdessen dieselbe Variante fuer beide Seiten (33 Paare 0/0). Empfehlung: nicht spiegeln, die Griffseite entscheidet |
| Griff-FORM falsch (Knauf statt Druecker o. ae.) | Griff-Mesh tauschen: gleicher Anhaengepunkt (x +-130, 3.1), gleiches Eltern-Blatt. Innerhalb einer Formfamilie (3.1) sind Geometrie UND UV bytegleich - dann ist der Tausch nur die Textur des Beschlagstreifens v 219..255 des Spenders; zwischen Familien anderes UV -> Streifen + Farben des Spenders mitnehmen (8 bpp, eine CLUT je TIM) oder das Griff-Objekt mit dem Spender-TIM zeichnen | **Ja, innerhalb eines Archivs:** DOOR01 und DOOR05 tauschen den Griff per Bit 7 (var 14) am selben Anhaengepunkt. DOOR01 Skript 1: @Datei 0x06070 `23 00 0e 00 00 00` (Cmp var14 == 0) -> @0x06076 `4d 01 00 00 01 01 d0 00 ...` Mesh 1, sonst @0x06090 `4d 01 00 00 01 03 d0 00 ...` Mesh 3, beide bei (130,-3216,-3420). DOOR05 Skript 1: @0x05052 Cmp -> @0x05058 Mesh 1, sonst @0x05072 Mesh 2, beide (130,-3232,-3372). Archivuebergreifend: nein |
| Blattform/Textur passt zu keinem Archiv | anderes Archiv (T1/T3) oder eigenes Archiv im RE2-Aufbau wie das Tor ROOM1170 (09_sequenz) | - |

## 3. Griff-Formen je Archiv (Knauf/Druecker/Ring/Stange/keiner, Mesh, Anhaengepunkt, Austauschbarkeit)

Form aus dem Nahbild `[BILD]` (`build/r31_tueren/t2/bogen_griffe.png`, im Repo `tueren_belege/t2_griffe.jpg`: je
sichtbarem Griff ein 64 x 64-Ausschnitt aus Bild t0, 3fach), Masse/Anhaengepunkt aus MD1 und `Door_model_set`,
Gleichheit per Vergleich von Ecken + Dreiecks-Indizes + UV (`re2_tuer_bericht.py`, Spalte "gleich wie").
"Drehung beim Oeffnen" = groesster Ausschlag rot x des Griff-Objekts `[SIM]` (Knauf dreht 1500 = 132 Grad, Druecker
kippt -150..-702).

| Archiv | Mesh | Dreiecke | Form | Groesse x,y,z | Anhaengepunkt im Blatt | rot x beim Oeffnen | UV v | gleich (GEO+UV) wie |
|---|---|---|---|---|---|---|---|---|
| DOOR00 | 1 | 33 | Knauf A | [305, 214, 212] | [130, -3224, -3372] | 1500 | 219..255 | DOOR03.m1, DOOR05.m1, DOOR0D.m1, DOOR18.m1, DOOR20.m1, DOOR21.m1, DOOR32.m1, DOOR34.m1, DOOR36.m1 |
| DOOR01 | 1 | 100 | Druecker | [229, 245, 619] | [130, -3216, -3420] | -296 | 219..255 | DOOR05.m2 |
| DOOR01 | 3 | 33 | Knauf A | [305, 214, 212] | [130, -3216, -3420] | -564 | 219..255 | - |
| DOOR01 | 2 | 100 | Druecker (Rueckseite) | [229, 245, 619] | [-130, -3216, -3420] | -296 | 219..255 | DOOR05.m3 |
| DOOR02 | 1 | 68 | Knauf B (Schild) | [306, 214, 212] | [130, -3232, -3372] | 1500 | 219..254 | DOOR08.m1, DOOR09.m1, DOOR11.m1, DOOR13.m1, DOOR1C.m1 |
| DOOR03 | 1 | 33 | Knauf A | [305, 214, 212] | [130, -3192, -3436] | 1500 | 219..255 | DOOR00.m1, DOOR05.m1, DOOR0D.m1, DOOR18.m1, DOOR20.m1, DOOR21.m1, DOOR32.m1, DOOR34.m1, DOOR36.m1 |
| DOOR04 | 1 | 196 | Stangengriff lang | [296, 1755, 211] | [130, -3124, -3308] | 0 | 218..253 | - |
| DOOR05 | 1 | 33 | Knauf A | [305, 214, 212] | [130, -3232, -3372] | 1500 | 219..255 | DOOR00.m1, DOOR03.m1, DOOR0D.m1, DOOR18.m1, DOOR20.m1, DOOR21.m1, DOOR32.m1, DOOR34.m1, DOOR36.m1 |
| DOOR05 | 2 | 100 | Druecker | [229, 245, 619] | [130, -3232, -3372] | -150 | 219..255 | DOOR01.m1 |
| DOOR05 | 3 | 100 | Druecker (Rueckseite) | [229, 245, 619] | [-130, -3174, -3396] | -150 | 219..255 | DOOR01.m2 |
| DOOR06 | 1 | 36 | Buegelgriff | [314, 1369, 168] | [130, -3038, -3366] | 0 | 235..254 | DOOR0B.m1, DOOR0C.m1, DOOR2F.m1 |
| DOOR07 | 1 | 16 | Druecker flach | [212, 104, 509] | [130, -3352, -3404] | -702 | 238..254 | DOOR1D.m1 |
| DOOR08 | 1 | 68 | Knauf B (Schild) | [306, 214, 212] | [130, -3112, -3364] | 1500 | 219..254 | DOOR02.m1, DOOR09.m1, DOOR11.m1, DOOR13.m1, DOOR1C.m1 |
| DOOR09 | 1 | 68 | Knauf B (Schild) | [306, 214, 212] | [130, -3224, -3372] | 1500 | 219..254 | DOOR02.m1, DOOR08.m1, DOOR11.m1, DOOR13.m1, DOOR1C.m1 |
| DOOR0A | 1 | 116 | Riegelkasten | [169, 124, 583] | [130, -3040, -2660] | -365 | 220..252 | - |
| DOOR0B | 1 | 36 | Stangengriff | [314, 1369, 168] | [130, -2864, -3092] | 0 | 235..254 | DOOR06.m1, DOOR0C.m1, DOOR2F.m1 |
| DOOR0C | 1 | 36 | Stangengriff | [314, 1369, 168] | [130, -2976, -3320] | 0 | 235..254 | DOOR06.m1, DOOR0B.m1, DOOR2F.m1 |
| DOOR0D | 1 | 33 | Knauf A | [305, 214, 212] | [130, -3208, -3456] | 1500 | 219..255 | DOOR00.m1, DOOR03.m1, DOOR05.m1, DOOR18.m1, DOOR20.m1, DOOR21.m1, DOOR32.m1, DOOR34.m1, DOOR36.m1 |
| DOOR11 | 1 | 68 | Knauf B (Langschild) | [306, 214, 212] | [130, -3200, -3340] | -1225 | 219..254 | DOOR02.m1, DOOR08.m1, DOOR09.m1, DOOR13.m1, DOOR1C.m1 |
| DOOR13 | 1 | 68 | Knauf B (Schild) | [306, 214, 212] | [130, -3364, -3342] | 1500 | 219..254 | DOOR02.m1, DOOR08.m1, DOOR09.m1, DOOR11.m1, DOOR1C.m1 |
| DOOR14 | 1 | 12 | Griffplatte | [170, 1128, 642] | [130, -2464, 4938] | 0 | 230..255 | - |
| DOOR15 | 1 | 96 | Beschlag (Strebe) | [150, 937, 358] | [430, -3624, -3092] | 0 | 222..251 | - |
| DOOR17 | 1 | 24 | Ring | [180, 180, 60] | [130, -3344, -3052] | 0 | 218..236 | - |
| DOOR18 | 1 | 33 | Knauf A | [305, 214, 212] | [130, -3204, -3392] | 1500 | 219..255 | DOOR00.m1, DOOR03.m1, DOOR05.m1, DOOR0D.m1, DOOR20.m1, DOOR21.m1, DOOR32.m1, DOOR34.m1, DOOR36.m1 |
| DOOR19 | 1 | 137 | Griffmulde | [196, 1155, 1089] | [145, -2544, -3594] | 0 | 220..254 | - |
| DOOR19 | 3 | 137 | Griffmulde (Rueckseite) | [196, 1155, 1089] | [-170, -2544, -3594] | 0 | 220..254 | - |
| DOOR1A | 1 | 60 | Druecker + Kartenleser | [213, 186, 681] | [130, -3204, -3162] | -245 | 240..253 | - |
| DOOR1B | 1 | 144 | Druckstange quer | [249, 498, 3138] | [130, -2940, -3410] | 0 | 222..250 | - |
| DOOR1C | 1 | 68 | Knauf B (oval) | [306, 214, 212] | [130, -3044, -3122] | 1500 | 219..254 | DOOR02.m1, DOOR08.m1, DOOR09.m1, DOOR11.m1, DOOR13.m1 |
| DOOR1D | 1 | 16 | Druecker flach (Schild) | [212, 104, 509] | [130, -2850, -3410] | -245 | 238..254 | DOOR07.m1 |
| DOOR1F | 1 | 62 | zweite Platte (faehrt) | [600, 2700, 5100] | [0, 6570, 0] | 0 | 131..219 | - |
| DOOR1F | 2 | 12 | Tafel | [0, 1028, 1050] | [0, 3150, 350] | 0 | 220..255 | - |
| DOOR1F | 3 | 2 | Schild | [0, 750, 1725] | [0, 3050, 3000] | 0 | 228..255 | - |
| DOOR23 | 1 | 118 | Riegelstange quer | [390, 240, 1125] | [130, -3524, -2952] | -450 | 221..254 | - |
| DOOR24 | 1 | 184 | Druecker schraeg | [270, 240, 1080] | [130, -3400, -3362] | -400 | 113..253 | - |
| DOOR26 | 3 | 48 | Handrad | [137, 700, 701] | [450, -3040, 2000] | -1380 | 85..116 | DOOR31.m3 |
| DOOR29 | 1 | 144 | Bedientafel | [295, 1572, 731] | [130, -4234, -3162] | 0 | 199..253 | - |
| DOOR29 | 2 | 144 | Bedientafel (Rueckseite) | [296, 1572, 731] | [-130, -4234, -3128] | 0 | 199..253 | - |
| DOOR29 | 3 | 2 | Schild | [0, 843, 1494] | [-130, -4980, -1062] | 0 | 25..53 | - |
| DOOR2C | 2 | 60 | Griffleiste | [240, 666, 35] | [-330, -3860, -350] | 0 | 241..253 | - |
| DOOR2E | 1 | 26 | Riegelkasten | [240, 90, 510] | [130, -2820, -2740] | -365 | 241..255 | - |
| DOOR2F | 1 | 36 | Stangengriff | [314, 1369, 168] | [130, -3126, -3406] | 0 | 235..254 | DOOR06.m1, DOOR0B.m1, DOOR0C.m1 |
| DOOR31 | 3 | 48 | Handrad | [137, 700, 701] | [450, -3040, 2000] | -1380 | 85..116 | DOOR26.m3 |

**Ohne Griff-Mesh** (Griff nur gemalt oder keiner): 10 (Scherengitter), 22 (Stahltuer mit gemaltem Riegel), 25 (Aufzugtuer),
27, 2A, 2B, 1E/33/35, 0E/0F/12/16, 28, 2D, 30 (Glas-Doppeltuer), 20/21/32/34/36 (kein Objekt).

**Formfamilien (bytegleich in Geometrie + UV):**
- Knauf A (33 Dreiecke): 00 03 05 0D 18 (+ 20 21 32 34 36, dort ungenutzt).
- Knauf B mit Schild (68): 02 08 09 11 13 1C.
- Druecker (100, vorn + gespiegelte Rueckseite): 01 05.
- Stangen-/Buegelgriff (36): 06 0B 0C 2F.
- Druecker flach (16): 07 1D.

**Austauschbar?** Alle Griffe haengen als Kind (Flag 0x10, Eltern Obj 0, @0x80014c64..8c) am Standardblatt; der
Anhaengepunkt liegt immer bei x = +130 (Vorderseite; Rueckseite -130 mit Drehung 2048) und variiert in y um -2850..-3524
und z um -2660..-3594 - also dieselbe Masstabs- und Lageklasse. Geometrisch ist jeder Griff an jedes Standardblatt
setzbar; die Lage des Spenders (sein eigener Anhaengepunkt) passt zur Hoehe seines Schildes auf SEINER Textur, auf
einem fremden Blatt muss der Punkt auf das gemalte Schild des Empfaengers gesetzt werden. Die Textur ist das
Hindernis: jeder Griff liest den Beschlagstreifen v 219..255 SEINES Archivs (Farben aus der einen 256er-CLUT dieses
TIM). Innerhalb einer Formfamilie ist das UV identisch -> Griff-Mesh des Empfaengers behalten und nur den Streifen
tauschen bzw. den Spender-Griff mit dem Spender-TIM zeichnen. RE2 selbst tauscht nur innerhalb eines Archivs (Bit 7,
2.5) - dort liegen beide Griffe im selben TIM.

## 4. Ton je Archiv (Tonteil, Se_on-Bilder, Door_exit-Ton, Tonfamilien, Treppen/Leitern)

`re2_tuer_ton.py` (Pruefungen: Pruefsumme der EXE-Tabelle 55/55, VH-Kennung 55/55, VH-Groesse 32+2048+ps*512+512
55/55, Summe VAG = VB 55/55, VH bei 0x10 55/55).

**Aufbau, fuer alle 55 gleich:** Tonkopf `00 00 14 16 | 00 00 24 17 | ff ff ff ff | ff ff ff ff` in **allen** 55 Archiven
(Eintrag 0 = Programm 0 Ton 1 Prio 4 Stimme 22; Eintrag 1 = Programm 0 Ton 2 Prio 4 Stimme 23; b0..b3 nach 0x8005ba28,
Adressen im Kopf von `re2_tuer_ton.py`), VH bei 0x10 mit 1 Programm (ps 1, ts 2..3, vs 2..3), Nachspann @0xC30, VB @0xC38.
Damit ist jeder Tonteil so ladbar wie heute TORSE.VBS (TOC edt 0 / 0xC38, VB ab 0xC38). Die Tonhoehe ist je Archiv
verschieden (4091..44100 Hz aus note2pitch2 @0x80083010 mit Tabelle @0x800aba40) - der Port rechnet sie aus dem
Tonsatz selbst (`se_play_layers` -> `re15_vab_note2pitch2`).

**Wann klingt was** `[SIM]` (ohne Ladewarten; im Port ist der Ton vor dem ersten Bild geladen, 09 6b):
- Ton 0 = Skript-`Se_on` Satz 0 (bei 2C/2D zusaetzlich Satz 1 aus dem Skript), Bild 20..130 je nach Archiv.
- Ton 1 = Door_exit nach dem Ausblenden, wenn ein `Door_model_set` Flag 0x800 trug (@0x80014c90..a8 ->
  @0x800141d8..f4). 43 Archive tragen 0x800. Bei 1E/33/35 V7/V8, 28, 2C, 2D, Treppen/Leiter/20/21/32/34/36 kein Door_exit-Ton.
- 12 Archive haben einen stummen Eintrag 1 (Ton 2 ohne VAG): 0E 0F 12 16 20 21 2B 32 33 34 35 36.
- 25 und 2A spielen fuer Eintrag 0 und 1 dieselbe VAG. 13 hat die VAGs vertauscht (E0 -> VAG 3).

| Archiv | Tonteil B | Ton 0: VAG, Dauer s (Bilder) | Ton 1: VAG, Dauer s | Se_on je Variante (Satz@Bild) | Door_exit spielt Ton 1 | Familie |
|---|---|---|---|---|---|---|
| DOOR00 | 19208 | VAG 2 fcd643, 1.06 (63) | VAG 3 54af4f, 0.65 (39) | V0 S0@70; V1 S0@70 | ja | F1 |
| DOOR01 | 23928 | VAG 2 88e8ab, 1.23 (74) | VAG 3 87df12, 0.98 (59) | V0 S0@70; V1 S0@70; V2 S0@70; V3 S0@70 | ja | F2 |
| DOOR02 | 19208 | VAG 2 fcd643, 1.06 (63) | VAG 3 54af4f, 0.65 (39) | V0 S0@70; V1 S0@70 | ja | F1 |
| DOOR03 | 19208 | VAG 2 fcd643, 1.06 (63) | VAG 3 54af4f, 0.65 (39) | V0 S0@70; V1 S0@70 | ja | F1 |
| DOOR04 | 23928 | VAG 2 88e8ab, 1.23 (74) | VAG 3 87df12, 0.98 (59) | V0 S0@120; V1 S0@120; V2 S0@70; V3 S0@120 | ja | F2 |
| DOOR05 | 19208 | VAG 2 fcd643, 1.06 (63) | VAG 3 54af4f, 0.65 (39) | V0 S0@70; V1 S0@70 | ja | F1 |
| DOOR06 | 19608 | VAG 2 dbcf39, 0.95 (57) | VAG 3 f9bf86, 0.80 (48) | V0 S0@130; V1 S0@130; V2 S0@110; V3 S0@110 | ja | F3 |
| DOOR07 | 19608 | VAG 2 dbcf39, 0.95 (57) | VAG 3 f9bf86, 0.80 (48) | V0 S0@70; V1 S0@70 | ja | F3 |
| DOOR08 | 19608 | VAG 2 dbcf39, 0.95 (57) | VAG 3 f9bf86, 0.80 (48) | V0 S0@70; V1 S0@70 | ja | F3 |
| DOOR09 | 23928 | VAG 2 88e8ab, 1.23 (74) | VAG 3 87df12, 0.98 (59) | V0 S0@70; V1 S0@70 | ja | F2 |
| DOOR0A | 23032 | VAG 2 caf955, 1.53 (92) | VAG 3 69ce61, 1.61 (97) | V0 S0@100; V1 S0@100 | ja | - |
| DOOR0B | 19208 | VAG 2 fcd643, 1.06 (63) | VAG 3 54af4f, 0.65 (39) | V0 S0@120; V1 S0@120 | ja | F1 |
| DOOR0C | 23880 | VAG 2 ca52d7, 1.85 (111) | VAG 3 89db19, 1.43 (85) | V0 S0@90; V1 S0@90 | ja | - |
| DOOR0D | 19208 | VAG 2 fcd643, 1.06 (63) | VAG 3 54af4f, 0.65 (39) | V0 S0@70; V1 S0@70; V2 S0@70; V3 S0@70 | ja | F1 |
| DOOR0E | 14888 | VAG 2 2f5752, 0.46 (28) | stumm (VAG 0 leer) | V4 S0@65,S0@108,S0@151; V5 S0@61,S0@104,S0@147 | nein | - |
| DOOR0F | 10888 | VAG 2 90b755, 0.30 (18) | stumm (VAG 0 leer) | V4 S0@65,S0@108,S0@151; V5 S0@61,S0@104,S0@147 | nein | - |
| DOOR10 | 19224 | VAG 2 1d8256, 1.96 (117) | VAG 3 ce7851, 0.80 (48) | V0 S0@120; V1 S0@120 | ja | - |
| DOOR11 | 23928 | VAG 2 88e8ab, 1.23 (74) | VAG 3 87df12, 0.98 (59) | V0 S0@100; V2 S0@100; V1 S0@100; V3 S0@100 | ja | F2 |
| DOOR12 | 15112 | VAG 2 05ac22, 0.95 (57) | stumm (VAG 0 leer) | V4 S0@65,S0@108,S0@151; V5 S0@61,S0@104,S0@147 | nein | - |
| DOOR13 | 15784 | VAG 3 f04a98, 0.74 (44) | VAG 2 9bfdf4, 0.60 (36) | V0 S0@60; V1 S0@60 | ja | - |
| DOOR14 | 24024 | VAG 2 19316e, 2.56 (153) | VAG 3 06b5eb, 1.88 (113) | V0 S0@110; V1 S0@110 | ja | - |
| DOOR15 | 21304 | VAG 2 344110, 1.43 (86) | VAG 3 ee98a4, 0.89 (53) | V0 S0@60; V1 S0@60; V2 S0@110; V3 S0@110 | ja | F4 |
| DOOR16 | 11000 | VAG 2 38a9b4, 1.24 (74) | stumm (VAG 0 leer) | V4 S0@110,S0@165,S0@220,S0@275; V5 S0@110,S0@165,S0@220,S0@275 | nein | - |
| DOOR17 | 23112 | VAG 2 e46a55, 2.10 (126) | VAG 3 560d85, 1.68 (100) | V0 S0@110; V1 S0@110 | ja | - |
| DOOR18 | 19208 | VAG 2 fcd643, 1.06 (63) | VAG 3 54af4f, 0.65 (39) | V0 S0@70; V1 S0@70 | ja | F1 |
| DOOR19 | 23704 | VAG 2 867ff6, 2.26 (135) | VAG 3 2ffb84, 1.63 (98) | V0 S0@60; V1 S0@60 | ja | - |
| DOOR1A | 21304 | VAG 2 344110, 1.43 (86) | VAG 3 ee98a4, 0.89 (53) | V0 S0@70; V1 S0@70 | ja | F4 |
| DOOR1B | 21176 | VAG 2 fd012e, 1.52 (91) | VAG 3 3eecc4, 1.42 (85) | V0 S0@90; V2 S0@90; V1 S0@90; V3 S0@90 | ja | F5 |
| DOOR1C | 23736 | VAG 2 8606a6, 1.44 (86) | VAG 3 56091c, 0.75 (45) | V0 S0@70; V1 S0@70 | ja | - |
| DOOR1D | 23928 | VAG 2 e2b603, 1.69 (101) | VAG 3 964044, 1.89 (113) | V0 S0@70; V1 S0@70; V2 S0@100; V3 S0@100 | ja | - |
| DOOR1E | 22872 | VAG 2 869f61, 1.98 (118) | VAG 3 c793de, 1.15 (68) | V0 S0@70; V1 S0@70; V6 S0@70; V7 -; V8 - | nein/ja | - |
| DOOR1F | 21144 | VAG 2 835411, 3.74 (224) | VAG 3 099e17, 1.96 (117) | V0 S0@60; V1 S0@60 | ja | - |
| DOOR20 | 23656 | VAG 2 5c5beb, 1.84 (110) | stumm (VAG 0 leer) | V0 S0@80 | nein | - |
| DOOR21 | 23592 | VAG 2 803e82, 4.68 (280) | stumm (VAG 0 leer) | V0 S0@80 | nein | - |
| DOOR22 | 19608 | VAG 2 dbcf39, 0.95 (57) | VAG 3 f9bf86, 0.80 (48) | V0 S0@60; V1 S0@60 | ja | F3 |
| DOOR23 | 21304 | VAG 2 344110, 1.43 (86) | VAG 3 ee98a4, 0.89 (53) | V0 S0@60; V1 S0@60 | ja | F4 |
| DOOR24 | 22728 | VAG 2 8d21ce, 1.54 (92) | VAG 3 2c9e05, 1.56 (93) | V0 S0@60; V1 S0@60 | ja | F6 |
| DOOR25 | 17624 | VAG 2 cea8e9, 1.54 (92) | VAG 2 cea8e9, 1.54 (92) | V0 S0@110 | ja | - |
| DOOR26 | 21832 | VAG 2 f68a67, 2.66 (159) | VAG 3 1b5ce0, 2.12 (127) | V0 S0@60; V1 S0@60; V2 S0@60; V3 S0@60 | ja | F7 |
| DOOR27 | 23176 | VAG 2 be1141, 2.39 (143) | VAG 3 805d2a, 2.57 (154) | V0 S0@70; V1 S0@70; V2 S0@70; V3 S0@70 | ja | - |
| DOOR28 | 23048 | VAG 2 1f2794, 2.96 (177) | VAG 3 3f8f49, 3.88 (232) | V0 S0@105; V2 S0@105; V4 S0@105; V1 S0@105; V3 S0@105; V5 S0@105 | nein | - |
| DOOR29 | 22728 | VAG 2 8d21ce, 1.54 (92) | VAG 3 2c9e05, 1.56 (93) | V0 S0@110; V1 S0@110 | ja | F6 |
| DOOR2A | 16424 | VAG 2 fbc4b7, 2.10 (126) | VAG 2 fbc4b7, 2.10 (126) | V0 S0@110; V1 S0@110; V2 S0@110; V3 S0@110 | ja | - |
| DOOR2B | 18504 | VAG 2 91749e, 4.86 (291) | stumm (VAG 0 leer) | V0 S0@60 | ja | - |
| DOOR2C | 24312 | VAG 2 caa4c0, 2.42 (145) | VAG 3 392e02, 2.42 (145) | V0 S0@60,S1@250; V1 S0@60,S0@300; V2 S0@60; V3 S0@60 | nein | - |
| DOOR2D | 22712 | VAG 2 31a00a, 4.16 (249) | VAG 3 ce5578, 4.18 (250) | V0 S0@60,S1@300; V2 S0@60,S1@300; V4 S0@60,S1@300; V1 S0@20,S1@175; V3 S0@20,S1@175; V5 S0@20,S1@175 | nein | - |
| DOOR2E | 19176 | VAG 2 6eec90, 1.32 (79) | VAG 3 8e9ba1, 1.22 (73) | V0 S0@100; V1 S0@100 | ja | - |
| DOOR2F | 19608 | VAG 2 dbcf39, 0.95 (57) | VAG 3 f9bf86, 0.80 (48) | V0 S0@120; V1 S0@120 | ja | F3 |
| DOOR30 | 21176 | VAG 2 fd012e, 1.52 (91) | VAG 3 3eecc4, 1.42 (85) | V0 S0@120; V1 S0@120 | ja | F5 |
| DOOR31 | 21832 | VAG 2 f68a67, 2.66 (159) | VAG 3 1b5ce0, 2.12 (127) | V0 S0@60; V1 S0@60; V2 S0@60; V3 S0@60 | ja | F7 |
| DOOR32 | 23944 | VAG 2 055463, 4.43 (265) | stumm (VAG 0 leer) | V0 S0@20 | nein | - |
| DOOR33 | 18664 | VAG 2 999b38, 2.92 (175) | stumm (VAG 0 leer) | V0 S0@70; V1 S0@70; V6 S0@70; V7 -; V8 S0@70 | nein/ja | - |
| DOOR34 | 24312 | VAG 2 518340, 4.51 (270) | stumm (VAG 0 leer) | V0 S0@20 | nein | - |
| DOOR35 | 19832 | VAG 2 ba7060, 2.64 (158) | stumm (VAG 0 leer) | V0 S0@70; V1 S0@70; V6 S0@70; V7 S0@120; V8 - | nein/ja | - |
| DOOR36 | 21240 | VAG 2 d805b4, 0.96 (58) | stumm (VAG 0 leer) | V0 S0@80,S0@140 | nein | - |

**Tonfamilien (Tonteil bytegleich, sha1 ueber [0..Tonteil)):**
F1 = 00 02 03 05 0B 0D 18 (19208 B) - F2 = 01 04 09 11 (23928 B) - F3 = 06 07 08 22 2F (19608 B) - F4 = 15 1A 23
(21304 B) - F5 = 1B 30 (21176 B) - F6 = 24 29 (22728 B) - F7 = 26 31 (21832 B). Alle anderen 28 Archive haben einen
eigenen Tonteil. Zusaetzlich teilen 0E 0F 12 16 20 21 2B 32..36 nur den (stummen) Eintrag 1.
Folge fuer den Port: eine Mini-Bank je FAMILIE genuegt (7 + 28 = 35 verschiedene Tonteile, 55 Archive).

**Treppen/Leitern:** Treppe 0E/0F/12: Satz 0 dreimal (V4 Bild 65/108/151, V5 61/104/147) = drei Schrittgeraeusche;
0E Stein 0,46 s, 0F Holz 0,30 s, 12 Stein 0,95 s; kein Door_exit-Ton (Flag 0x800 fehlt, Eintrag 1 stumm). Leiter 16:
Satz 0 viermal (Bild 110/165/220/275, 1,24 s) = Sprossen; kein Door_exit-Ton.

**Offen am Ton:** abgehoert ist nichts; Deutung Oeffnen/Schliessen/Schritt nur aus Zeitpunkt und Huellkurve (wie 08_re_ton).

## 5. Treppen/Leitern/Aufzug/Schott (Varianten 4/5, Archive 0E/0F/12/16, 1E/33/35, 25/29, 2B, 2D)

| Archiv | Varianten | Was man sieht `[SIM]`/`[BILD]` | RE2-Uebergaenge (Reihe A, `re2_tueren.json`) | Erkennung |
|---|---|---|---|---|
| 0E Treppe Stein | V4 / V5 | V4: Setzstufen von unten, Treppe sinkt (y +1360) und kommt naeher = **hinauf**; V5: Trittflaechen unter dem Horizont, Block steigt ins Bild = **hinab** (`build/r31_tueren/t2/check_treppe_zeit.png`) | ROOM1070 V4 -> 1080 (Aussentreppe, Fuss der Treppe, `[BILD]` paar223), 1080/11C0 V5 -> 1070, 3020 V4 -> 30B0, 30B0 V5 -> 3020 | Paar 4/5; Rechteck am Treppenfuss = V4 |
| 0F Treppe Holz | V4 / V5 | wie 0E (Mesh 0F = 12 bytegleich) | 2070 V4 -> 10C0, 10C0 V5 -> 2070 | dito |
| 12 Treppe Stein gruen | V4 / V5 | wie 0F | 2110 V4 -> 20F0, 20F0 V5 -> 2110 | dito |
| 16 Leiter | V4 / V5 | V4: Leiter faehrt nach unten (y +300) = **hinauf**; V5: nach oben (y -300) = **hinab** | 51 Saetze; z. B. 2000 (Etage 1) V4 -> 1100 (Etage 4, y -7200) = hinauf; 1100 V5 -> 2000 = hinab; auch Selbst-Uebergaenge im selben Raum (4010, 4030: Etage 0 <-> 4; 60E0 beide Etage 10) | Ziel-Etage (Nutzlast +11) > eigene Etage (Satz +4) => V4 innerhalb eines Raums; zwischen Raeumen nicht verlaesslich (3090 Etage 3 -> 3000 Etage 0 ist V4) |
| 28 Bodenluke | V0..V5 | Kamerafahrt auf die Luke, keine Blattbewegung; Skript 2 (V0/2/4, 281 Bilder), Skript 1 (V1/3/5, 301) | 3070 V4 -> 3050 (Etage 3), 3050 V5 -> 3070; 4040 V4 -> 4010/4030 (y -7200), 4010/4030 V5 -> 4040 | nur 4/5 benutzt; V4 hinauf, V5 hinab (Ziel-Etage) |
| 1E/33/35 Klappe | V0 V1 V6 V7 V8 | Gitterklappe dreht um z (-1600); V7 nur Rahmen, V8 Kette mit Kamerafahrt | 1E: 4010 V0 -> 40F0, 6000 V0 -> 6020; 33: 1110 V8 -> 2190 (Rechteck 0 = Skript-Uebergang); 35: 60A0 V7 -> 6090 | Lueftungsschacht / Kriechgang |
| 25 Aufzugtuer TYPE-L | V0 | Blatt faehrt hoch (y -5375) | 33 Saetze im Labor (6060, 6080, 60B0, 60C0, 6110, 6120-6160), beide Seiten V0 | Aufzugtuer, von beiden Seiten gleich |
| 29 Aufzugtuer TYPE-M | V0 / V1 | Drehtuer mit Bedientafel (Mesh 1/2) und Schild (Mesh 3) | 5050/6060/60B0/6100 V0, 6070/6090/60A0/60C0 V1 | wie Standardtuer (Griffseite) |
| 2B Klappe "DUMPING AREA-B2" | V0 | hebt (y -7080), Flag 0x1000 (hellerer Hintergrund) | 40A0 <-> 4100, beide V0 | beide Seiten gleich |
| 2D Hubbuehne | V0..V5 | Kamerafahrt, Buehne mit Gelaender; Skript 1 (301 Bilder) / 2 (451) | 6170 V5 -> 7000 (einziger Satz, sat 0x41) | eigene Szene |
| 1F Stahlplatte XD-R | V0 / V1 | zwei Platten fahren auseinander (y -3120 / +6240) | 3040/30A0 V0 -> 4020/4000; 4000/4020 V1 -> 30A0/3040 | Griffplatte links (V0) / rechts (V1) |

Band-Wechsel: RE2-Treppen/Leitern fuehren oft auf ein anderes Stockwerk (Nutzlast +11 = Ziel-Etage, y = -1800 je Etage;
z. B. 1100 -> 2000 y -1800 Etage 1), aber ein fester Zusammenhang Variante <-> Etagen-Differenz besteht nur innerhalb
EINES Raums (Selbst-Uebergaenge 4010/4030: nach oben V4, nach unten V5). Fuer RE1.5 heisst das: hinauf/hinab aus
dem Hintergrund (Stufen/Leiter nach oben oder unten) bzw. aus der Ziel-Etage der RE1.5-Tuer bestimmen, nicht aus dem
RE2-Raum.



## 6. Port-Anschluss-Plan (Datei:Zeile, RE2-Adressen, Schrittfolge fuer den Bau-Agenten)

Stand der Zeilen: Zweig `r31/tueren` (Basis 7cb74897), per `grep -n` gemessen am 2026-09-29.

### 6.0 Was heute da ist

| Stelle | Datei:Zeile | Stand |
|---|---|---|
| Anfrage + Archiv-Kennungen | `include/re15_door_seq.h:137-145` | nur `RE15_DOOR_ARCHIV_KEINS`/`_TOR1170`; Anfrage traegt archiv, variante, tuer_nr - kein Bit 7, keine RE2-Nummer |
| Zuordnung | `engine/src/door_seq_tor1170.c:37-59` | Tabelle Raum + Rechteck (Mitte/Halbmass) + Band -> Archiv/Variante; 4 Eintraege (Tor 1170/1171) |
| Ausfuehren | `engine/src/door_seq_tor1170.c:66-74` | verfaellt ohne Laeufer (PSX, Tests) |
| Archivquelle | `engine/src/door_seq_archiv.c:88-96` | nur das eingebackene Torarchiv |
| Maschine | `engine/src/door_seq_common.c:114-155` | `re15_door_seq_start` nimmt den Modellteil im RE2-Aufbau (+0 MD1, +4 TIM, +8 SCD) - **jedes DOORxx.DO2 ab Sektor*0x800 passt unveraendert** |
| Luecken der Maschine | `door_seq_common.c:408-412` | Member_set/Member_set2/Member_copy nur Vorschub (braucht DOOR10: 0x34; DOOR2D: 0x34/0x35/0x3D) |
| | `door_seq_common.c:460-463` | `default:` schaltet das Ereignis ab -> 0x8A/0x8B/0x8C (DOOR28, 2D, 33) wuerden das Skript toeten |
| Zeichnen | `platform/pc/src/door_scene_pc.c:107-174` | Flags 0xc0 (OT), 0x1000 (BK) umgesetzt; es fehlen 0x2000 (DOOR10), 0x4000 (DOOR10, 2C), 0x0100 (DOOR28) |
| | `door_scene_pc.c:166` | `pxo = (page & 0xF) * 128` - falsch fuer DOOR28 (MD1 steht auf page 0x95 / CLUT 0x7fc0) |
| Laeufer | `door_scene_pc.c:187-270` | `:192` var0e fest 0; `:205` laedt nur TORSE; `:232`/`:263` spielen nur TORSE |
| Ton | `platform/pc/src/audio_pc.c:3598-3682` | TORSE-Slot (Tonteil DOOR2E unveraendert, TOC 0/0xC38) |
| Raumbaenke | `audio_pc.c:3563-3587` | `:3567` schaltet ALLE Stimmen ab, `:3573` loescht alle Vormerkungen |
| Anfrage Selbst-Tuer | `engine/src/aot_common.c:716-737` | `re15_door_seq_zuordnen(g_current_room_id, a->x, a->z, a->half_w, a->half_h, d->band, &var)` |
| Kreuz-Raum | `aot_common.c:661` | `re15_room_request_change(...)` - **keine** Anfrage |
| Verbrauch | `engine/src/game_step_common.c:2081-2082` | `if (re15_door_seq_ausfuehren() && g_scd_pending_scenario < 0) re15_room_transition_present();` |
| Laeufer anmelden | `platform/pc/main.c:3094` | `re15_pc_tuerszene_anmelden()` |
| Spielschritt / Raumwechsel | `main.c:6486` (re15_game_step) vor `main.c:7664` (`re15_room_apply_pending`), Einblendung `main.c:7715` | Reihenfolge im Bild: Scan + Sequenz, DANN Raum laden, DANN RE1.5-Einblendung |
| Raumbaenke im Wechsel | `engine/src/room_common.c:423` | `re15_audio_load_room_banks()` in `re15_room_apply_pending` |
| PSX | `platform/psx/main.c:600`, `platform/psx/src/audio_psx.c:889-890` | kein Laeufer, Ton-Stubs |
| Paket | `release/make_package.sh:190-192` | prueft TORSE.VBS |

### 6.1 RE2-Belege fuer den Ablauf (selbst gelesen)

- **Reihenfolge bei der Tuer mit Raumwechsel** `FUN_80026b7c`: `@0x80026bec jal 0x8002bda8` (a0=2: Bild schwarz),
  `@0x80026bf8 addiu a1,a1,15300` + `@0x80026bfc jal 0x80031f6c` (a0=1: Task 1 = Door_main 0x80013bc4),
  `@0x80026c0c jal 0x80014cd0` (Tonteil laden), ab `@0x80026c9c` Spielerlage aus der Nutzlast (`lw a1,0x800ce550`),
  `@0x80026e1c jal 0x80049e48` (Zielraum laden, darin Raumbaenke `FUN_80059e54`), `@0x80026e28..54` warten, solange
  `0x800cfb74 & 0x2000000` (Tuer-Task laeuft), `@0x80026e60 jal 0x8002bda8` danach.
- **Tuerbank ueberlebt den Raumwechsel:** `FUN_80059e54 @0x80059e90 jal 0x800597a4`; dort je Stimme 23..0
  (`@0x800597ac addiu s1,zero,24`, `@0x800597d8 addiu s1,s1,-1`) nur abschalten, wenn die SPU-Startadresse im
  Raumbereich liegt: `@0x800597bc lui s3,0xfffe / @0x800597c0 ori s3,s3,0xbbbf` (-0x14441), `@0x800597c8 lui s2,0x2 /
  @0x800597cc ori s2,s2,0x980e`, `@0x80059800 addu v0,v0,s3`, `@0x80059804 sltu v0,s2,v0`, `@0x80059808 bne` (ausserhalb
  -> weiter), sonst `@0x80059810 jal 0x80079498` mit a0 = 0 (Key-Off). Also nur 0x14441..0x3DC4F. Die Tuerbank liegt
  bei **0x3DC50**: `@0x80014f08 lui a2,0x3` + `@0x80014f18 ori a2,a2,0xdc50` (a2 von `jal 0x80085368` @0x80014f14,
  SsVabOpenHeadSticky). -> Tuertoene werden vom Raumwechsel nicht abgeschnitten.
- **Door_exit-Ton** nach dem Ausblenden, Satz 1: `@0x800141d8 lhu v0,584(v0)`, `@0x800141e0 beq`, `@0x800141f4 lui a0,0x1`,
  `@0x800141f0 jal 0x8005ba28` (08_re_ton 1).
- **Archiv laden:** Tabelle `@0x8009a520` (Tonteil, Modellteil, Sektor, Pruefsumme; 55/55 geprueft,
  `re2_tuer_tabelle.py`), Modellteil nach 0x801a1000 (`@0x80013c7c jal 0x80015064`), Tonteil nach 0x801a1000 -
  Groesse (`@0x80014da0 subu s0,s0,s4`). **Typ 40 (DOOR28)** ohne MD1-Umsetzung: `@0x80013d78 lbu v1,12(v0)`,
  `@0x80013d7c addiu v0,zero,40`, `@0x80013d80 beq v1,v0,0x80013d94` -> `@0x80013d94/98` a2 = a3 = 0 statt 21/31
  (`@0x80013d88/90`) vor `@0x80013d9c jal 0x80076b60`.

### 6.2 Schrittfolge fuer den Bau-Agenten

**Schritt 1 - Daten.**
1. `python re15_port/tools/tueren/re2_tuer_tabelle.py --inc re15_port/engine/src/gen/re2_tuer_tabelle.inc` (55 Zeilen aus
   @0x8009a520, jede mit Adresse + Rohbytes im Kommentar).
2. `python re15_port/tools/tueren/re2_tuer_tabelle.py --kopiere-nach re15_port/shared_assets/RE2/DOOR --nur <Liste>` mit
   der Liste der Archive, die die Zuordnung (T1/T3) wirklich braucht. Die Dateien bleiben UNVERAENDERT (sha1 wird beim
   Kopieren geprueft). Groesse: alle 55 zusammen 3 474 940 B, groesstes 81 336 B; Quelle `info/re2leon/COMMON/DOOR/`.
   Lesen im Port: `re15_pc_read_re2("DOOR/DOORxx.DO2", &n)` (`platform/pc/src/asset_root_pc.c:297`, Unterordner
   gehen wie bei `FILES/`).
3. Zuordnungstabelle `engine/src/gen/tuer_zuordnung.inc` aus T1/T3 + Regel 2.4 erzeugen (Generator neu, Muster
   `door_seq_tor1170.c:37-45`): je RE1.5-Tuerseite {Raum (volle Id inkl. Variante xxx0/xxx1), x, z, hw, hh (Mitte +
   Halbmass wie `door_seq_tor1170.c:19`), Band (Door_aot_set pc[4]), RE2-Archiv 0..54, Variante, Bit 7}. Jede Zeile
   kommentiert mit RDT-Datei-Offset des Door_aot_set und der Herkunft der Wahl ("PORT-WAHL, keine Original-Adresse:
   Griff im Hintergrund links -> V0, Regel 2.4, Beleg <Ausschnitt>"). **Schluessel NICHT der Slot** (05 1.4: ROOM1170
   Slot 6 = ROOM1171 Slot 5; ROOM1170 Slot 3 hat die Nutzlast des Tors, aber Rechteck 0 und Band 0).

**Schritt 2 - Maschine (engine/src/door_seq_common.c).**
1. `case 0x34/0x35/0x3D` (`:408`): Feldzugriff ueber die RE2-Tabellen `0x80011228` (Setter, 44 Eintraege) und
   `0x800112f8` (Getter) - Umsetzung wie `tuerkatalog.member_table` (Befehl am Sprungziel bzw. im Delay-Slot); DOOR10
   Feld 13 = z (+0x40). Vorschub 4/3/3 (@0x80055c30, @0x80055c90, @0x80055e54).
2. `case 0x8A/0x8B/0x8C`: nur Vorschub 6/6/8 (@0x80059378 addiu v1,v1,6 / @0x800593c8 / @0x8005941c addiu v1,v1,8);
   Wirkung (Vibration) optional ueber `re15_rumble`, sonst nichts - aber NICHT in `default:` fallen.
3. Zeichen-Flags in `door_scene_pc.c`: 0x2000 Ecken-z * var 4 / 256 (@0x80014794..f0; nur DOOR10), 0x4000
   halbtransparent Code 0x36 (@0x8001473c..48; DOOR10, 2C), 0x0100 pulsierende Farbe (+8 je Bild, @0x80014468; DOOR28).

**Schritt 3 - Laeufer + Ton (platform/pc).**
1. Anfrage erweitern (`include/re15_door_seq.h:140-145`): `uint8_t re2_nr` (0..54, 0xFF = keins), `uint8_t bit7`.
   Neue Kennung `RE15_DOOR_ARCHIV_RE2` (Archiv aus Datei).
2. `tuer_laeufer` (`door_scene_pc.c:187`): fuer RE2 die Datei lesen, Modellteil = Datei + Sektor*0x800 (Laenge
   Modellteil), Tonteil = Datei[0 .. Tonteil) aus `re2_tuer_tabelle.inc`; `re15_door_seq_start(&s, modell, n, variante,
   bit7 ? 0x80 : 0, re2_nr)` statt `:192` mit 0 (var 14 = Payload+13 & 0x80 @0x80013e84/8c, var 15 = Payload+12
   @0x80013e90/98).
3. DOOR28 (`re2_nr == 0x28`): Seite 0x95 -> 0x80 und CLUT-Zeile 511 -> 480 zurueckrechnen (RE2 laedt das TIM nach
   (320,256)/CLUT 511 und setzt das MD1 fuer Typ 40 nicht um, 6.1) - an `door_scene_pc.c:166` und beim CLUT-Wort.
4. `audio_pc.c`: `load_re2_tor_se_pc` (`:3616`) zu `load_re2_tuer_se_pc(const uint8_t *ton, int groesse, int schluessel)`
   verallgemeinern: Aufbau ist in allen 55 Archiven identisch mit TORSE (Kopf @0, VH @0x10, Nachspann @0xC30, VB @0xC38;
   `re2_tuer_ton.py` 55/55), also dieselbe TOC; zwischenspeichern je Tonfamilie (35 verschiedene Tonteile). TORSE bleibt
   fuer das Tor. Abspielen weiter ueber `se_play_layers` mit RE2-Pegelgesetz (`s_se_pegel_re2`, 08_re_ton 2.3).
5. **Schliesston ueberlebt den Raumwechsel:** in `re15_audio_load_room_banks` (`audio_pc.c:3567/3573`) Stimmen, deren
   `pcm` in den dekodierten Tuertoenen liegt, NICHT abschalten und ihre `s_se_pend` NICHT loeschen (Gegenstueck zu
   RE2 @0x800597a4..0x80059818 + Tuerbank 0x3DC50, 6.1). Die dekodierten Tuertoene erst beim naechsten Tuerladen
   freigeben, nicht im Raumwechsel.

**Schritt 4 - Anbindung (engine).**
1. `aot_common.c:661` (Kreuz-Raum-Zweig): VOR `re15_room_request_change` dieselbe Zuordnung wie `:729` mit
   `g_current_room_id`, Rechteck und `d->band` aufrufen und die Anfrage setzen.
2. `game_step_common.c:2081`: Bedingung um `&& !g_room_change.pending` ergaenzen - bei einer Kreuz-Raum-Tuer gehoert
   die Einblendung hinter den Raumwechsel (`main.c:7715`), nicht in den alten Raum. Die Sequenz selbst laeuft hier
   (Spielschritt `main.c:6486` liegt vor `re15_room_apply_pending` `main.c:7664`) - damit ist "Verbrauch vor
   re15_room_apply_pending" erfuellt, und das Standbild des Abdunkelns (`door_scene_pc.c:212-221`) ist das zuletzt
   gezeigte Bild des ALTEN Raums.
3. Sicherung in `main.c` direkt vor `:7664`: steht noch eine Anfrage an (Tuer ausserhalb des Spielschritts gefeuert),
   `re15_door_seq_ausfuehren()` dort aufrufen.
4. PSX: nichts anmelden (`platform/psx/main.c:600` bleibt), `audio_psx.c` bekommt Stubs fuer die neuen Tonfunktionen;
   die Anfrage verfaellt dort wie heute.

**Schritt 5 - Paket.** `release/make_package.sh` bei `:190-192`: fuer jede Datei in `shared_assets/RE2/DOOR/*.DO2` des
Quellbaums pruefen, dass sie im Paket liegt und nicht leer ist (`die` sonst: "Tuersequenz ohne Modell").

**Schritt 6 - Riegel (Sonde `tests/unit/probes/<thema>.cmake`).**
1. Maschine gegen den Simulator: fuer mindestens DOOR00 V0/V1, DOOR0E V4, DOOR10 V0 (Member_set + Flag 0x2000),
   DOOR28 V4 (0x8A..0x8C, Typ 40) den Modellteil aus `shared_assets/RE2/DOOR` starten und Bild fuer Bild Lage/Drehung
   gegen eine Referenz aus `tuerkatalog.VM` pruefen (Muster `unit_door_seq.c` / `tools/tor/tuerseq_referenz.py`).
2. Zuordnung: eine Kreuz-Raum-Tuer loest die Anfrage mit richtigem Archiv/Variante/Bit 7 aus; die Intro-Uebergabe
   (Null-Rechteck) und das Tor bleiben unveraendert.
3. Ton: nach `re15_audio_re2_tuer_se(1)` + `re15_audio_load_room_banks()` ist die Stimme noch aktiv.
4. Sichtpruefung mit echter exe + RE15_FRAMEDUMP an einer Kreuz-Raum-Tuer (Abdunkeln alter Raum, Sequenz, Schwarz,
   Einblendung neuer Raum).

### 6.3 Was NICHT anfassen

| Was | Warum / woran erkennbar |
|---|---|
| Tor ROOM1170/1171 | eigenes Archiv `RE15_DOOR_ARCHIV_TOR1170` (`door_seq_tor1170.c:37-45`), eigene Skripte; die neue Tabelle muss diese 4 Eintraege behalten |
| Verschlossene Tueren | Anfrage entsteht nur in `aot_fire_door`, also erst beim tatsaechlichen Durchgehen; RE1.5 fuehrt Schloesser ueber Raumskripte (Nutzlast +15/+16 = 0 in allen 649 Saetzen, 05 1.4) - nichts am Schloss aendern |
| Aufzuege mit eigener RE2-Szene | ELEVSE.VBS / `tools/re2_elevator_cut.py`; ROOM1080 Slots 0-2 haben Null-Rechteck -> kein Tabelleneintrag |
| Intro-Uebergaben mit Null-Rechteck | 27 Null-Rechteck-Kreuz-Raum-Tueren (aot_common.c:640-657), u. a. 1240 -> 1170; Schluessel Rechteck + Band schliesst sie aus |
| Cutscene-/Skript-Tueren | per `Aot_on` gefeuert mit Rechteck 0; RE2 nimmt dafuer die objektlosen Archive 20/21/32/34/36 - im Port kein Eintrag |
| RE1.5-Einblendung nach der Tuer | `re15_room_transition_present` (`room_common.c`) bleibt RE1.5 (09 0.) |
| PSX | bleibt Stub (6.2 Schritt 4.4) |

## 7. Offen / nicht belegt

1. **Ladewarten.** Alle Bildnummern `[SIM]` mit 0 Bildern Ladezeit; im Port ist das richtig (Tonteil synchron vor dem
   ersten Bild, 09 6b), in RE2 verschiebt das CD-Laden die Zeitachse ab dem Warteskript.
2. **Stichprobe 2.3:** eine Seite (ROOM10D0, DOOR05 V0) widerspricht vermutlich dem Bild; der Blick ist zu flach, um
   es sicher zu sagen. Zweitbeleg (andere Kamera) gibt es im Raum nicht (nur c0/c2 sehen die Tuer).
3. **Treppe hinauf/hinab** ist aus Modellbewegung + Bild (Aussentreppe ROOM1070) belegt; die RE2-Stockwerke der
   Raeume 10C0/2070 ordne ich nicht zu (Hintergrund zu flach).
4. **Ton nicht abgehoert** (Deutung Oeffnen/Schliessen/Schritt nur aus Zeitpunkt + Huellkurve, wie 08_re_ton).
5. **Textzeilen** fuer Tuertyp 50/52 (0x32/0x34, Bild 41..259, `@0x80013f90..0x80014004`) nicht gelesen - nur fuer
   Skript-Uebergaenge relevant, im Port ausgeschlossen.
6. **Flag 0x0200** (42 Archive) und Feld pc[2] ohne Leser (03 Offen 3) - fuer den Bau ohne Wirkung.
7. **Wie viele RE1.5-Tueren nicht abgedeckt werden**, haengt an der Zuordnung Tuer -> Archiv (T1/T3). Aus RE2-Sicht
   deckt jedes der 26 Standard-Archive beide Griffseiten ab; Luecken entstehen nur, wo KEIN Archiv die Tuerform trifft
   (niedrige Tore, Sondertueren) oder wo die Griff-FORM nicht im Archiv liegt (dann Griff-Tausch 2.5/3).
8. **Dynamische Gegenprobe** gegen eine RE2-Aufnahme steht aus (wie 03/09).

## Anhang A1 - alle Varianten (aus `re2_tuer_bericht.py`, Spalten wie Abschnitt 1)

Objekte: B = Blatt (bewegt), Bf = Blatt fest, G = Griff/Beschlag (Kind eines Blatts), Ak/Aw = Anbauteil Kind/Wurzel;
Zahl = Mesh. Se_on-Bild ohne Ladewarten `[SIM]`.

| Archiv | V | Skr | Bilder | Bewegung | Richtung | Angel x (Seite) | freie Kante | Griff: Mesh, x, Seite (vorn) | Objekte (Rolle:Mesh) | Se_on Bild | Schliesston |
|---|---|---|---|---|---|---|---|---|---|---|---|
| DOOR00 | 0 | 1 | 301 | dreht | weg | 234 (rechts) | links | m1 111 links | B:0 G:1 | 70 | ja |
| DOOR00 | 1 | 2 | 301 | dreht | hin | 102 (links) | rechts | m1 227 rechts | B:0 G:1 G:1 | 70 | ja |
| DOOR01 | 0 | 1 | 301 | dreht | weg | 234 (rechts) | links | m1 109 links | B:0 G:1 | 70 | ja |
| DOOR01 | 0 b7 | 1 | 301 | dreht | weg | 234 (rechts) | links | m3 109 links | B:0 G:3 | 70 | ja |
| DOOR01 | 1 | 2 | 301 | dreht | hin | 86 (links) | rechts | m2 211 rechts | B:0 G:2 G:1 | 70 | ja |
| DOOR01 | 1 b7 | 2 | 301 | dreht | hin | 86 (links) | rechts | m3 211 rechts | B:0 G:3 G:3 | 70 | ja |
| DOOR01 | 2 | 3 | 281 | dreht | weg | 290 (rechts) | mitte | m1 177 rechts | B:0 G:1 G:1 Bf:0 G:1 | 70 | ja |
| DOOR01 | 2 b7 | 3 | 281 | dreht | weg | 290 (rechts) | mitte | m1 177 rechts | B:0 G:1 G:1 Bf:0 G:1 | 70 | ja |
| DOOR01 | 3 | 4 | 281 | dreht | hin | 30 (links) | mitte | m1 143 links | B:0 G:1 Bf:0 G:1 | 70 | ja |
| DOOR01 | 3 b7 | 4 | 281 | dreht | hin | 30 (links) | mitte | m1 143 links | B:0 G:1 Bf:0 G:1 | 70 | ja |
| DOOR02 | 0 | 1 | 301 | dreht | weg | 234 (rechts) | links | m1 111 links | B:0 G:1 | 70 | ja |
| DOOR02 | 1 | 2 | 301 | dreht | hin | 102 (links) | rechts | m1 225 rechts | B:0 G:1 G:1 | 70 | ja |
| DOOR03 | 0 | 1 | 301 | dreht | weg | 234 (rechts) | links | m1 109 links | B:0 G:1 G:1 | 70 | ja |
| DOOR03 | 1 | 2 | 301 | dreht | hin | 102 (links) | rechts | m1 229 rechts | B:0 G:1 G:1 | 70 | ja |
| DOOR04 | 0 | 1 | 301 | dreht | weg | 234 (rechts) | links | m1 114 links | B:0 G:1 | 120 | ja |
| DOOR04 | 1 | 2 | 301 | dreht | hin | 102 (links) | rechts | m1 225 rechts | B:0 G:1 G:1 | 120 | ja |
| DOOR04 | 2 | 3 | 246 | dreht | weg | 290 (rechts) | mitte | m1 166 mitte | B:0 G:1 Bf:0 G:1 G:1 | 70 | ja |
| DOOR04 | 3 | 4 | 296 | dreht | hin | 30 (links) | mitte | m1 154 mitte | B:0 G:1 G:1 Bf:0 G:1 | 120 | ja |
| DOOR05 | 0 | 1 | 301 | dreht | weg | 234 (rechts) | links | m1 111 links | B:0 G:1 | 70 | ja |
| DOOR05 | 0 b7 | 1 | 301 | dreht | weg | 234 (rechts) | links | m2 111 links | B:0 G:2 | 70 | ja |
| DOOR05 | 1 | 2 | 301 | dreht | hin | 102 (links) | rechts | m1 226 rechts | B:0 G:1 G:1 | 70 | ja |
| DOOR05 | 1 b7 | 2 | 301 | dreht | hin | 102 (links) | rechts | m3 226 rechts | B:0 G:2 G:3 | 70 | ja |
| DOOR06 | 0 | 1 | 311 | dreht | weg | 234 (rechts) | links | m1 111 links | B:0 G:1 | 130 | ja |
| DOOR06 | 1 | 2 | 311 | dreht | hin | 102 (links) | rechts | m1 224 rechts | B:0 G:1 G:1 | 130 | ja |
| DOOR06 | 2 | 3 | 291 | dreht | weg | 289 (rechts) | mitte | m1 167 mitte | B:0 Bf:0 G:1 G:1 | 110 | ja |
| DOOR06 | 3 | 4 | 311 | dreht | hin | 30 (links) | mitte | - | B:0 Bf:0 G:1 G:1 G:1 | 110 | ja |
| DOOR07 | 0 | 1 | 301 | dreht | weg | 234 (rechts) | links | m1 110 links | B:0 G:1 | 70 | ja |
| DOOR07 | 1 | 2 | 301 | dreht | hin | 86 (links) | rechts | m1 210 rechts | B:0 G:1 | 70 | ja |
| DOOR08 | 0 | 1 | 301 | dreht | weg | 234 (rechts) | links | m1 112 links | B:0 G:1 G:1 | 70 | ja |
| DOOR08 | 1 | 3 | 296 | dreht | hin | 102 (links) | rechts | m1 226 rechts | B:0 G:1 G:1 | 70 | ja |
| DOOR09 | 0 | 1 | 301 | dreht | weg | 234 (rechts) | links | m1 111 links | B:0 G:1 | 70 | ja |
| DOOR09 | 1 | 2 | 301 | dreht | hin | 102 (links) | rechts | m1 227 rechts | B:0 G:1 G:1 | 70 | ja |
| DOOR0A | 0 | 1 | 291 | dreht | weg | 226 (rechts) | links | m1 123 links | B:0 G:1 | 100 | ja |
| DOOR0A | 1 | 2 | 291 | dreht | hin | 107 (links) | rechts | m1 211 rechts | B:0 G:1 G:1 | 100 | ja |
| DOOR0B | 0 | 1 | 301 | dreht | weg | 234 (rechts) | links | m1 122 links | B:0 G:1 | 120 | ja |
| DOOR0B | 1 | 2 | 301 | dreht | hin | 102 (links) | rechts | m1 220 rechts | B:0 G:1 G:1 | 120 | ja |
| DOOR0C | 0 | 1 | 307 | dreht | hin | 289 (rechts) | mitte | m1 169 mitte | B:0 B:0 G:1 G:1 G:1 G:1 | 90 | ja |
| DOOR0C | 1 | 2 | 307 | dreht | weg | 289 (rechts) | mitte | m1 169 mitte | B:0 B:0 G:1 G:1 | 90 | ja |
| DOOR0D | 0 | 1 | 311 | dreht | weg | 234 (rechts) | links | m1 108 links | B:0 G:1 | 70 | ja |
| DOOR0D | 1 | 2 | 311 | dreht | hin | 102 (links) | rechts | m1 227 rechts | B:0 G:1 G:1 | 70 | ja |
| DOOR0D | 2 | 3 | 327 | dreht | hin | 289 (rechts) | mitte | m1 164 mitte | B:0 G:1 G:1 B:0 G:1 G:1 | 70 | ja |
| DOOR0D | 3 | 4 | 327 | dreht | weg | 289 (rechts) | mitte | m1 164 mitte | B:0 G:1 B:0 G:1 | 70 | ja |
| DOOR0E | 4 | 1 | 169 | hebt | runter | 162 (mitte) | rechts | - | B:0 | 65,108,151 | nein |
| DOOR0E | 5 | 2 | 165 | - | - | - | - | - | Bf:0 | 61,104,147 | nein |
| DOOR0F | 4 | 1 | 169 | hebt | runter | 162 (mitte) | rechts | - | B:0 | 65,108,151 | nein |
| DOOR0F | 5 | 2 | 165 | - | - | - | - | - | Bf:0 | 61,104,147 | nein |
| DOOR10 | 0 | 1 | 301 | schiebt | nach links | 95 (links) | links | - | Bf:1 B:0 B:0 B:0 B:0 B:0 B:0 B:0 B:0 B:0 | 120 | ja |
| DOOR10 | 1 | 2 | 301 | schiebt | nach links | 238 (rechts) | rechts | - | Bf:1 B:0 B:0 B:0 B:0 B:0 B:0 B:0 B:0 B:0 | 120 | ja |
| DOOR11 | 0 | 1 | 291 | dreht | weg | 289 (rechts) | mitte | m1 168 mitte | B:0 Bf:0 G:1 G:1 | 100 | ja |
| DOOR11 | 2 | 1 | 291 | dreht | weg | 289 (rechts) | mitte | m1 168 mitte | B:0 Bf:0 G:1 G:1 | 100 | ja |
| DOOR11 | 1 | 2 | 311 | dreht | hin | 30 (links) | mitte | m1 150 mitte | B:0 Bf:0 G:1 G:1 G:1 | 100 | ja |
| DOOR11 | 3 | 2 | 311 | dreht | hin | 30 (links) | mitte | m1 150 mitte | B:0 Bf:0 G:1 G:1 G:1 | 100 | ja |
| DOOR12 | 4 | 1 | 169 | hebt | runter | 162 (mitte) | rechts | - | B:0 | 65,108,151 | nein |
| DOOR12 | 5 | 2 | 165 | - | - | - | - | - | Bf:0 | 61,104,147 | nein |
| DOOR13 | 0 | 1 | 286 | dreht | weg | 234 (rechts) | links | m1 112 links | B:0 G:1 G:1 | 60 | ja |
| DOOR13 | 1 | 2 | 286 | dreht | hin | 102 (links) | rechts | m1 224 rechts | B:0 G:1 G:1 | 60 | ja |
| DOOR14 | 0 | 1 | 301 | schiebt | nach links | 56 (links) | rechts | m1 236 rechts | B:0 G:1 | 110 | ja |
| DOOR14 | 1 | 2 | 301 | schiebt | nach rechts | 56 (links) | rechts | m1 67 links | B:0 G:1 | 110 | ja |
| DOOR15 | 0 | 1 | 286 | dreht | weg | 234 (rechts) | links | m1 120 links | B:0 G:1 G:1 | 60 | ja |
| DOOR15 | 1 | 2 | 286 | dreht | hin | 102 (links) | rechts | m1 216 rechts | B:0 G:1 G:1 | 60 | ja |
| DOOR15 | 2 | 3 | 291 | dreht | weg | 289 (rechts) | mitte | m1 178 rechts | B:0 Bf:0 G:1 G:1 | 110 | ja |
| DOOR15 | 3 | 4 | 311 | dreht | hin | 30 (links) | mitte | m1 141 links | B:0 Bf:0 G:1 G:1 G:1 | 110 | ja |
| DOOR16 | 4 | 1 | 331 | hebt | runter | 160 (mitte) | rechts | - | B:0 | 110,165,220,275 | nein |
| DOOR16 | 5 | 2 | 331 | hebt | hoch | 160 (mitte) | links | - | B:0 | 110,165,220,275 | nein |
| DOOR17 | 0 | 1 | 286 | dreht | weg | 234 (rechts) | links | m1 123 links | B:0 G:1 Ak:2 | 110 | ja |
| DOOR17 | 1 | 2 | 286 | dreht | hin | 102 (links) | rechts | m1 214 rechts | B:0 G:1 Ak:2 G:1 Ak:2 | 110 | ja |
| DOOR18 | 0 | 1 | 301 | dreht | weg | 234 (rechts) | links | m1 110 links | B:0 G:1 | 70 | ja |
| DOOR18 | 1 | 2 | 301 | dreht | hin | 102 (links) | rechts | m1 227 rechts | B:0 G:1 G:1 | 70 | ja |
| DOOR19 | 0 | 1 | 301 | schiebt | nach rechts | 232 (rechts) | links | m1 101 links | B:0 G:1 Ak:2 | 60 | ja |
| DOOR19 | 1 | 2 | 301 | schiebt | nach links | 95 (links) | rechts | m3 226 rechts | B:0 G:3 Ak:2 | 60 | ja |
| DOOR1A | 0 | 1 | 301 | dreht | weg | 234 (rechts) | links | m1 119 links | B:0 G:1 | 70 | ja |
| DOOR1A | 1 | 2 | 301 | dreht | hin | 102 (links) | rechts | m1 218 rechts | B:0 G:1 G:1 | 70 | ja |
| DOOR1B | 0 | 1 | 307 | dreht | hin | 289 (rechts) | mitte | m1 166 mitte | B:0 B:0 G:1 G:1 G:1 G:1 | 90 | ja |
| DOOR1B | 2 | 1 | 307 | dreht | hin | 289 (rechts) | mitte | m1 166 mitte | B:0 B:0 G:1 G:1 G:1 G:1 | 90 | ja |
| DOOR1B | 1 | 2 | 307 | dreht | weg | 289 (rechts) | mitte | m1 166 mitte | B:0 B:0 G:1 G:1 | 90 | ja |
| DOOR1B | 3 | 2 | 307 | dreht | weg | 289 (rechts) | mitte | m1 166 mitte | B:0 B:0 G:1 G:1 | 90 | ja |
| DOOR1C | 0 | 1 | 301 | dreht | weg | 234 (rechts) | links | m1 120 links | B:0 G:1 | 70 | ja |
| DOOR1C | 1 | 2 | 301 | dreht | hin | 102 (links) | rechts | m1 217 rechts | B:0 G:1 G:1 | 70 | ja |
| DOOR1D | 0 | 1 | 301 | dreht | weg | 234 (rechts) | links | m1 110 links | B:0 G:1 | 70 | ja |
| DOOR1D | 1 | 2 | 301 | dreht | hin | 102 (links) | rechts | m1 222 rechts | B:0 G:1 G:1 | 70 | ja |
| DOOR1D | 2 | 3 | 291 | dreht | weg | 289 (rechts) | mitte | m1 166 mitte | B:0 Bf:0 G:1 G:1 | 100 | ja |
| DOOR1D | 3 | 4 | 311 | dreht | hin | 30 (links) | mitte | m1 152 mitte | B:0 Bf:0 G:1 G:1 G:1 | 100 | ja |
| DOOR1E | 0 | 1 | 301 | - | - | - | - | - | Aw:0 Aw:1 | 70 | ja |
| DOOR1E | 1 | 1 | 301 | - | - | - | - | - | Aw:0 Aw:1 | 70 | ja |
| DOOR1E | 6 | 2 | 301 | - | - | - | - | - | Aw:0 Aw:1 | 70 | ja |
| DOOR1E | 7 | 3 | 301 | - | - | - | - | - | Aw:1 | - | nein |
| DOOR1E | 8 | 7 | 301 | - | - | - | - | - | Aw:1 Ak:1 Ak:1 Ak:1 | - | nein |
| DOOR1F | 0 | 1 | 301 | hebt | hoch | 58 (links) | rechts | - | B:0 G:1 G:2 G:3 | 60 | ja |
| DOOR1F | 1 | 2 | 301 | hebt | hoch | 58 (links) | rechts | - | B:0 G:1 G:2 G:3 | 60 | ja |
| DOOR20 | 0 | None | 271 | - | - | - | - | - |  | 80 | nein |
| DOOR21 | 0 | None | 271 | - | - | - | - | - |  | 80 | nein |
| DOOR22 | 0 | 1 | 286 | dreht | weg | 234 (rechts) | links | - | B:0 | 60 | ja |
| DOOR22 | 1 | 2 | 286 | dreht | hin | 102 (links) | rechts | - | B:0 | 60 | ja |
| DOOR23 | 0 | 1 | 286 | dreht | weg | 234 (rechts) | links | m1 127 links | B:0 G:1 | 60 | ja |
| DOOR23 | 1 | 2 | 286 | dreht | hin | 102 (links) | rechts | m1 210 rechts | B:0 G:1 G:1 | 60 | ja |
| DOOR24 | 0 | 1 | 286 | dreht | weg | 234 (rechts) | links | m1 112 links | B:0 G:1 | 60 | ja |
| DOOR24 | 1 | 2 | 286 | dreht | hin | 102 (links) | rechts | m1 225 rechts | B:0 G:1 G:1 | 60 | ja |
| DOOR25 | 0 | 1 | 286 | hebt | hoch | 234 (rechts) | links | - | B:0 | 110 | ja |
| DOOR26 | 0 | 3 | 286 | dreht | weg | 51 (links) | mitte | m3 122 links | B:0 Bf:1 G:3 G:4 | 60 | ja |
| DOOR26 | 1 | 4 | 236 | dreht | hin | 269 (rechts) | mitte | m3 197 rechts | B:0 Bf:2 G:3 G:4 | 60 | ja |
| DOOR26 | 2 | 2 | 286 | dreht | weg | 106 (links) | rechts | m3 176 rechts | B:0 G:3 | 60 | ja |
| DOOR26 | 3 | 1 | 286 | dreht | hin | 214 (rechts) | links | m3 144 links | B:0 G:3 | 60 | ja |
| DOOR27 | 0 | 1 | 271 | schiebt | nach links | 51 (links) | mitte | - | B:0 B:1 | 70 | ja |
| DOOR27 | 1 | 1 | 271 | schiebt | nach links | 51 (links) | mitte | - | B:0 B:1 | 70 | ja |
| DOOR27 | 2 | 1 | 271 | schiebt | nach links | 51 (links) | mitte | - | B:0 B:1 | 70 | ja |
| DOOR27 | 3 | 1 | 271 | schiebt | nach links | 51 (links) | mitte | - | B:0 B:1 | 70 | ja |
| DOOR28 | 0 | 2 | 281 | - | - | - | - | - | Aw:0 Ak:1 Ak:2 | 105 | nein |
| DOOR28 | 2 | 2 | 281 | - | - | - | - | - | Aw:0 Ak:1 Ak:2 | 105 | nein |
| DOOR28 | 4 | 2 | 281 | - | - | - | - | - | Aw:0 Ak:1 Ak:2 | 105 | nein |
| DOOR28 | 1 | 1 | 301 | - | - | - | - | - | Aw:0 Ak:1 Ak:2 | 105 | nein |
| DOOR28 | 3 | 1 | 301 | - | - | - | - | - | Aw:0 Ak:1 Ak:2 | 105 | nein |
| DOOR28 | 5 | 1 | 301 | - | - | - | - | - | Aw:0 Ak:1 Ak:2 | 105 | nein |
| DOOR29 | 0 | 1 | 286 | dreht | weg | 234 (rechts) | links | m1 119 links | B:0 G:1 | 110 | ja |
| DOOR29 | 1 | 2 | 286 | dreht | hin | 102 (links) | rechts | m2 216 rechts; m3 140 links | B:0 G:1 G:2 G:3 | 110 | ja |
| DOOR2A | 0 | 1 | 301 | schiebt | nach rechts | 102 (links) | rechts | - | B:0 | 110 | ja |
| DOOR2A | 1 | 2 | 301 | schiebt | nach links | 234 (rechts) | links | - | B:0 | 110 | ja |
| DOOR2A | 2 | 3 | 301 | schiebt | nach rechts | 160 (mitte) | rechts | - | B:0 B:0 | 110 | ja |
| DOOR2A | 3 | 3 | 301 | schiebt | nach rechts | 160 (mitte) | rechts | - | B:0 B:0 | 110 | ja |
| DOOR2B | 0 | 1 | 301 | hebt | hoch | 15 (links) | rechts | - | B:0 | 60 | ja |
| DOOR2C | 0 | 1 | 301 | schiebt | nach rechts | 159 (mitte) | rechts | m2 172 rechts | B:0 B:0 Bf:1 Bf:1 G:2 G:2 | 60,250 | nein |
| DOOR2C | 1 | 2 | 351 | schiebt | nach rechts | 159 (mitte) | rechts | m2 172 rechts | B:0 B:0 G:2 G:2 | 60,300 | nein |
| DOOR2C | 2 | 3 | 301 | schiebt | nach rechts | 159 (mitte) | rechts | m2 172 rechts | B:0 B:0 Bf:1 Bf:1 G:2 G:2 | 60 | nein |
| DOOR2C | 3 | 4 | 351 | schiebt | nach rechts | 159 (mitte) | rechts | m2 172 rechts | B:0 B:0 G:2 G:2 | 60 | nein |
| DOOR2D | 0 | 1 | 301 | - | - | - | - | - | Bf:0 Bf:1 Bf:2 Bf:2 Bf:2 Bf:2 | 60,300 | nein |
| DOOR2D | 2 | 1 | 301 | - | - | - | - | - | Bf:0 Bf:1 Bf:2 Bf:2 Bf:2 Bf:2 | 60,300 | nein |
| DOOR2D | 4 | 1 | 301 | - | - | - | - | - | Bf:0 Bf:1 Bf:2 Bf:2 Bf:2 Bf:2 | 60,300 | nein |
| DOOR2D | 1 | 2 | 451 | - | - | - | - | - | Bf:0 Bf:1 Bf:2 Bf:2 Bf:2 Bf:2 | 20,175 | nein |
| DOOR2D | 3 | 2 | 451 | - | - | - | - | - | Bf:0 Bf:1 Bf:2 Bf:2 Bf:2 Bf:2 | 20,175 | nein |
| DOOR2D | 5 | 2 | 451 | - | - | - | - | - | Bf:0 Bf:1 Bf:2 Bf:2 Bf:2 Bf:2 | 20,175 | nein |
| DOOR2E | 0 | 1 | 291 | dreht | weg | 226 (rechts) | links | m1 120 links | B:0 G:1 | 100 | ja |
| DOOR2E | 1 | 2 | 291 | dreht | hin | 107 (links) | rechts | m1 214 rechts | B:0 G:1 G:1 | 100 | ja |
| DOOR2F | 0 | 1 | 301 | dreht | weg | 234 (rechts) | links | m1 110 links | B:0 G:1 | 120 | ja |
| DOOR2F | 1 | 2 | 301 | dreht | hin | 102 (links) | rechts | m1 226 rechts | B:0 G:1 G:1 | 120 | ja |
| DOOR30 | 0 | 1 | 301 | dreht | weg | 212 (rechts) | mitte | - | B:0 B:0 | 120 | ja |
| DOOR30 | 1 | 2 | 301 | dreht | hin | 102 (links) | mitte | - | B:0 B:0 | 120 | ja |
| DOOR31 | 0 | 3 | 286 | dreht | weg | 51 (links) | mitte | m3 122 links | B:0 Bf:1 G:3 G:4 | 60 | ja |
| DOOR31 | 1 | 4 | 236 | dreht | hin | 269 (rechts) | mitte | m3 197 rechts | B:0 Bf:2 G:3 G:4 | 60 | ja |
| DOOR31 | 2 | 2 | 286 | dreht | weg | 106 (links) | rechts | m3 179 rechts | B:0 G:3 | 60 | ja |
| DOOR31 | 3 | 1 | 286 | dreht | hin | 214 (rechts) | links | m3 139 links | B:0 G:3 | 60 | ja |
| DOOR32 | 0 | None | 321 | - | - | - | - | - |  | 20 | nein |
| DOOR33 | 0 | 1 | 301 | - | - | - | - | - | Aw:0 Aw:1 | 70 | ja |
| DOOR33 | 1 | 1 | 301 | - | - | - | - | - | Aw:0 Aw:1 | 70 | ja |
| DOOR33 | 6 | 2 | 301 | - | - | - | - | - | Aw:0 Aw:1 | 70 | ja |
| DOOR33 | 7 | 3 | 301 | - | - | - | - | - | Aw:1 | - | nein |
| DOOR33 | 8 | 7 | 301 | - | - | - | - | - | Aw:1 Ak:1 Ak:1 Ak:1 | 70 | nein |
| DOOR34 | 0 | None | 321 | - | - | - | - | - |  | 20 | nein |
| DOOR35 | 0 | 1 | 301 | - | - | - | - | - | Aw:0 Aw:1 | 70 | ja |
| DOOR35 | 1 | 1 | 301 | - | - | - | - | - | Aw:0 Aw:1 | 70 | ja |
| DOOR35 | 6 | 2 | 301 | - | - | - | - | - | Aw:0 Aw:1 | 70 | ja |
| DOOR35 | 7 | 3 | 301 | - | - | - | - | - | Aw:1 | 120 | nein |
| DOOR35 | 8 | 7 | 301 | - | - | - | - | - | Aw:1 Ak:1 Ak:1 Ak:1 | - | nein |
| DOOR36 | 0 | None | 281 | - | - | - | - | - |  | 80,140 | nein |

## Anhang A2 - Variantenpaare je Archiv (238 RE2-Paare)

| Archiv | Paare je Variantenkombination |
|---|---|
| DOOR00 | 0/1: 10 |
| DOOR01 | 0/1: 2 |
| DOOR02 | 0/1: 2 |
| DOOR03 | 0/1: 6 |
| DOOR04 | 2/3: 2 |
| DOOR05 | 0/1: 6 |
| DOOR06 | 0/1: 6, 2/3: 2 |
| DOOR07 | 0/1: 13 |
| DOOR08 | 0/1: 19 |
| DOOR09 | 0/1: 6 |
| DOOR0A | 0/1: 8 |
| DOOR0B | 0/1: 2 |
| DOOR0C | 0/1: 1 |
| DOOR0D | 0/1: 10, 2/3: 4 |
| DOOR0E | 4/5: 4 |
| DOOR0F | 4/5: 2 |
| DOOR10 | 0/1: 4 |
| DOOR11 | 2/3: 2 |
| DOOR12 | 4/5: 2 |
| DOOR13 | 0/1: 2 |
| DOOR14 | 0/1: 2 |
| DOOR15 | 2/3: 2 |
| DOOR16 | 4/5: 18 |
| DOOR17 | 0/1: 2 |
| DOOR18 | 0/1: 4 |
| DOOR19 | 0/1: 3 |
| DOOR1A | 0/1: 9 |
| DOOR1B | 2/3: 2 |
| DOOR1C | 0/1: 6 |
| DOOR1D | 0/1: 4, 2/3: 2 |
| DOOR1F | 0/1: 2 |
| DOOR22 | 0/1: 2 |
| DOOR23 | 0/1: 2 |
| DOOR24 | 0/1: 2 |
| DOOR25 | 0/0: 15 |
| DOOR26 | 0/0: 10, 2/3: 2 |
| DOOR27 | 0/0: 4 |
| DOOR28 | 4/5: 7 |
| DOOR29 | 0/1: 8 |
| DOOR2A | 0/1: 1, 2/2: 1 |
| DOOR2B | 0/0: 2 |
| DOOR2C | 2/2: 2 |
| DOOR2E | 0/1: 1 |
| DOOR2F | 0/1: 2 |
| DOOR30 | 0/1: 4 |
| DOOR31 | 0/0: 2 |


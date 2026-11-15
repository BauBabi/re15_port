# Runde 41 Spur H "hangar" — Birkin und Spider-Man in ROOM50A0/50A1 starr

Zweig `r41/hangar`, Basis master 789f6e681. Fortlaufendes Dossier. Belege: `H_hangar_belege/`.

⛔ **Nachtrag 2026-11-15 (Spur Monitor, `MON_monitor.md`):** der Ausloeser im Hangar (2.1) und der Kampf danach sind aus dem
Spiel genommen. Die Szene laeuft unveraendert (bitgleich h0..h134) nur noch als Monitorbild im Monitor Room 5030/5031; den alten
Ausloeser gibt es nur noch als Messhaken `RE15_HANGAR_IM_RAUM=1` fuer die Inhalts-Riegel.

Nutzer (Wortlaut): "In 50A0 sieht man das Birkin Modell, das die Spiderman auf den Boden hauen sollte, aber sowohl
das Modell als auch Spiderman ist starr"

Nachtrag Nutzer (ueber den Orchestrator, Wortlaut): "Gerade in der Vorschau mit birkin und spiderman ist die Szene nicht
wie im 80% build. Der Schwanz ist viel zu starr und auch die Spiderman Ansprung Szene ist zu sprunghaft. Das wirst du noch
mal analysieren muessen, wie es wirklich sein muss...."

Orchestrator-Hinweise: (1) das Modell EM30_HANGAR baut eine eigene Spur (SF) zur Sichel-Form um; diese Spur aendert KEINE
Modell- oder Asset-Datei, die Szene muss mit einem Modell derselben Knochen-/Clip-Struktur ohne Codeaenderung laufen
(Voraussetzungen Abschnitt 4). (2) Der Schwanz ist Sache von SF; die Szene darf seine Bewegung nicht verhindern (sie spielt
Birkins Clips jeden Takt, kein Standbild; Abschnitt 4).

## 1. Ursache (Messung vorher)

### 1.1 Nutzerlauf (befund.log Hauptbaum)

Leon per Debug-Menue nach ROOM50A0, Weg Cut 0 -> 1 -> 2 -> 3 -> 4 -> 5, steht in Cut 5 bei (-5568,0,-21162); zweiter Lauf
weiter nach Cut 6 und zurueck. Kein Cut 12.

### 1.2 Cut 12 ist im Raum nicht erreichbar (RDT-Beleg)

RVD von STAGE5/ROOM50A0.RDT (Tabelle @0x258, 20 Byte je Satz; `analysis/befunde_runde38/O_belege/sca_rvd.py`): keine Zone
schaltet NACH Cut 12. Die einzigen Saetze mit Cut 12/13 sind Platzhalter:

    rvd 39 @0x00564 mk ff01 cam 12 -> 0  x [-25000,-25000,-21000,-21000] z [-25000,-21000,-21000,-25000]
    rvd 40 @0x00578 mk ff01 cam 13 -> 0  x [-25000,-25000,-21000,-21000] z [-25000,-21000,-21000,-25000]

Das SCD (main sub00 @0x154A, sub00 @0x1654 .. sub07; `analysis/nutzer_batch_2026-08-27/tools/re15_scd_dis.py`) enthaelt
kein `Cut_chg` (0x29) auf Cut 12. ROOM50A1.RDT ist bytegleich (cmp). Damit wird `g_scd.work_vars[0x0A]` nie 12; die alte
Szene (`birkin_formen.c` Phase 0: `if ((int)g_scd.work_vars[0x0A] != BF_HANGAR_CUT) return;`) startete nie.

### 1.3 Messlauf vorher (echtes Fenster, exe-Kopie, gdigrab hwnd)

Lauf v1 (`re15_port/tools/r41_hangar/lauf.sh`, RE15_DEBUG_JUMP=50A0@20, Weg nach Sueden): state.log 900 Bilder lang
`[34 t=2b st=0 mo=0 af=0 @(-12575,-16950)]` und `[35 t=30 st=0 mo=0 af=0 @(-8475,-21675)]` — beide ab Raumeintritt mit
Clip 0 Bild 0 im Raum, nie animiert, nie in der KI. Bildfolge `H_hangar_belege/v1_vorher_bogen.jpg`.

### 1.4 Wer sieht die Szenenplaetze? Anker-Viereck + Bild

Die Zeichner-Region (Region-Cull, PSX FUN_80014324 @0x80014324: erster RVD-Satz mit cam_from == Cut, `byte+2`, Ablage
DAT_800ac794 ueber FUN_800142f4) entscheidet, ob eine Figur gezeichnet wird. Sichttest = Lage im Anker-Viereck des Schnitts
UND Fuss- oder Kopfpunkt im 320x240-Bild (Projektion wie der Zeichner, camera_common.c = FUN_80053ca4, sx = 160 + H*x/z):

| Cut | Birkin (-8488,-21651) | Spider-Man (-12470,-17540) |
|---|---|---|
| 0..3, 7..11 | nein | nein |
| 4, 5 | ja | ja |
| 6 | ja | nein (ausserhalb des Ankers) |
| 12 | Anker = Platzhalter rvd 39 -> wird gecullt | ebenso |

Folge: auch in Cut 12 wuerden beide nicht gezeichnet (Platzhalter-Anker); die Szene braucht dort eine Cull-Ausnahme.

## 2. Bau

### 2.1 Ausloeser, Riegel, Kamera (PORT-WAHL, Form aus Original-Zwischensequenzen)

- Birkin und Spider-Man gibt es erst ab dem Ausloeser (vorher kein Aktor, kein Schatten, keine Kollision) -> nie starr im Bild.
- Ausloeser: der angeforderte Schnitt (`g_scd.cam_id`; der RVD-Scan schreibt nur ihn, @0x80014300) zeigt eine Szenenfigur
  (Tabelle 1.4) und der Spieler ist frei (Zustand 1, nicht gegriffen, keine andere Zwischensequenz). Weg aus 5030
  (Nordtuer, Cut 0) -> Cut 4; aus 4070 (Westtuer, Cut 11, ROOM4070.RDT @0x11FE Ziel-Cut 11) -> Cut 6.
- Riegel wie die Original-Zwischensequenzen: Set(2,7) + Set(1,27) (Form ROOM1150 sub08 @0x01114/@0x01118; Pad-Sperre
  @0x800304f4-0x80030500, Letterbox FUN_80021a0c; das Video zeigt im Hangar die Letterbox-Balken), Ende @0x012E4/@0x012E8.
  Der Szenenschutz (szenenschutz_r38.c) greift damit automatisch (player_mode 2).
- Kamera: Cut_chg-Zwilling auf Cut 12 (LAB_800402a0: @0x800402c0/@0x800402e4 alter Cut, work_vars[0x0C] @0x800402ec,
  work_vars[0x0A] @0x800402fc, Zonen aus @0x800402d4); Ende: Cut_chg auf den beim Ausloesen angeforderten Schnitt + Zonen an
  (Form ROOM1150 sub08 `29 05` @0x012C4 / `3c 01` @0x012E2), wie gatorszene_r41.c kam_uebergabe.
- Region-Cull-Ausnahme `re15_bf_szene_zeigt` (main.c, NPC-Zeichner): nur die beiden Szenenfiguren, nur solange die Szene
  Cut 12 haelt. Der Spieler bleibt in Cut 12 gecullt (er steht ausserhalb des Platzhalter-Ankers) -> kein Leon im Bild wie im Video.
- Raumwechsel mitten in der Szene (nur Debug-Menue): der Riegel wird geloest, damit er nicht in den naechsten Raum mitwandert.
- Wiedereintritt: keine Szene, kein Spider-Man, Birkin sofort Angreifer (Zustand 0 -> INIT -> HUB). Der Merker
  "Szene gesehen" ist prozessweit (OFFEN 5.1: fuer Speicherstaende braeuchte er ein Bank-9-Bit, Vergabe Orchestrator).

### 2.2 Ablauf Bild fuer Bild nach dem 80%-Video (Nachbesserung zum Nutzer-Nachtrag)

Abschnitt 3 (Messung) -> Zeitplan in `birkin_formen.c` (Kopf "Hangar-Szene", Tabellen k_birkin_bild / k_birkin_gier):

| Videobild h | Video | Port |
|---|---|---|
| 0..1 | Spider-Man hockt links, Birkin steht | Sprung-Clip 0x16 B0..1, Birkin Clip 0 laeuft |
| 2..11 | Ansprung, Schatten 225 -> 360 -> 485 je Bild | glatte Bahn (smoothstep) auf ein geglaettetes Klauenziel, Bogen 900, Clip 0x16 B2..14 |
| 12..16 | Beruehrung auf Brusthoehe, hochgerissen | Clip 7 (gekruemmt), folgt der Klaue geglaettet; Birkin 0x0B B5..10 |
| 16..70 | ueber dem Kopf gehalten, links im Bild | Klaue + 1000 seitlich, Gier Bezug-1270; Birkin 0x0B im Pendel B10->34->22->37 (volle Rate) |
| 66..76 | dreht nach links, schmettert vor sich hin | Gier zurueck auf den Ansprung, B37->44 (Wurfbild 0x2C) ->55 |
| 74..79 | liegt, strampelt | Clip 0x0B B20..39 (liegt, richtet sich auf) |
| 80..86 | springt rechts an der Kamera vorbei aus dem Bild | Clip 0x16, Bahn zur Bildkante (Ziel Kamera + (4071,.,1118) = Bild-x 338 auf 3500 Tiefe), dann weg |
| 76..90 | richtet sich auf, dreht zur Kamera | B55->70, Gier auf die Kamera |
| 90..135 | geht auf die Kamera zu (~22 je Bild) | Clip 1 (Gang), Lenkrate 0x20 (@0x80117268), 22 je Takt |
| 135 | (Abblende) | Kamera zurueck, Riegel auf, Birkin in seiner 0x30-KI |

Alle Werte dieser Tabelle sind Videomessung oder Gestaltung (die Szene gibt es nur im 80%-Video), im Code so markiert.

## 3. Videomessung (Methode)

- Quelle: `Version_80%/Resident Evil 1.5 Complete Disc Video 4 (480p Upscale) (480p).mp4`, 29.97 Bilder/s, Schnitt in den
  Hangar bei 45.0 s (Datei-Bild 30 ab 44.0 s, Helligkeitssprung 44.7); Abblende ab h130. h = Bild ab dem Schnitt.
- Eichung Video -> PSX-Bild: psx = video/2 + (-1,-3), Kantenkorrelation des Hintergrunds gegen das Port-Bild Cut 12
  (0.700 an h120 / 0.672 an h30; `re15_port/tools/r41_hangar/kalib.py`).
- Rueckprojektion auf den Boden durch die RID-Kamera ROOM50A0 Cut 12 (pos (-13140,-3258,-26604), Ziel (-2808,-1440,-11556),
  H 231; `rueck.py`): Spider-Man-Schatten h0..2 (29..31,190) -> (-12470,-17540); h4 (49,194) -> (-11974,-18287); h6 (71,193)
  -> (-11342,-18623); h8 (100,192) -> (-10497,-19102); Birkin-Fuesse h0 (194,205) -> (-8488,-21651); h135 (190,222) ->
  (-9232,-22302). Schattenfinder `schatten.py`.
- Clip-Katalog EM2B/EM30 (Bank 0, je Clip 6 Bilder): `re15_port/tools/r41_hangar/clipkatalog.py` (liest ueber
  emd_ansichtsblatt.py die Engine-Leser nach). EM2B 0x16 = Sprung (B[11] @0x80113560 `+0x94 = 0x16`), 0x0B = Rueckenlage
  mit Aufstehen; EM30 0x0B = Griff/Wurf (Klaue B10..37 oben -3586..-3796, B44+ vorn unten).
- Klaue (Knochen 15) liegt nur ~1600 neben Birkins Wurzel; bei jeder Gier bleibt ihr Bild-x >= 141, das Video haelt den
  Spider-Man bei x 110..145 -> seitlicher Versatz 1000 (Gestaltung, berechnet; Lauf n6 ohne Versatz: 40..50 Bildpunkte rechts).
- Vergleich Video | Port mit gleicher Bildnummer: Port-Aufnahme 30 Bilder/s (gdigrab hwnd, 320x240), Bezugsbild p0 = erstes
  Bild mit Bewegung im Spider-Man-Feld nach dem Cut-12-Wechsel minus 2 (Absprung h2; `p0find.sh`, `vergleich.py`).
  Messzeile je Szenentakt: `RE15_HANGAR_LOG=1` (`[hangar-takt] h.. F.. B clip/bild/gier`, `SM clip/bild/(x,y,z)`).

### 3.1 Iterationen (Laeufe im echten Fenster, je exe-Kopie)

| Lauf | Stand | Befund |
|---|---|---|
| n2 | erster Bau (Szene wie zuvor, nur Ausloeser/Kamera neu) | Szene laeuft in Cut 12; Spider-Man fliegt 5900 Einheiten in 15 Takten quer an Birkins rechte Seite (Klaue bei Gier 2570), liegt am Ende - "sprunghaft" |
| n3/n4 | Zeitplan nach Video | Ansprung glatt; Halten: Birkin wechselt die Pose nur alle 3..4 Aufnahmebilder (Vorschub 0.57) -> Pendel mit voller Rate |
| n5/n6 | Pendel, Flucht, Gang | Flucht 2 Bilder spaet (-> h80..86), Gang ohne Drehung (-> Drehung zur Kamera h76..90), Halten 40..50 Bildpunkte rechts |
| n7 | seitlicher Versatz 1000, Gier -1270 | Halten links oben wie im Video |
| Unit | Riegel | Klauenziel springt beim Clipwechsel h7 um 1246 -> geglaettetes Ziel; Spider-Man 239 unter dem Boden im Wurf -> Klemme |
| n8 | Endstand | Bildfolgen `H_hangar_belege/vgl_video_port_h0-30.jpg`, `..._h40-78.jpg`, `..._h80-134.jpg` |

## 4. Modell-Voraussetzungen (fuer Spur SF / jedes Ersatzmodell)

- Birkin (Typ 0x30, Bank 0): Knochen 15 = haltende Hand (Kette 13-14-15 am Brustknochen 8, rel z -988 = rechte Seite),
  Clip 0 (Stand), Clip 1 (Gang), Clip 0x0B (Griff/Wurf) mit mindestens 71 Bildern (Zeitplan bis B70, Wurfbild 0x2C = 44).
  Die Szene setzt jedes Takt ein Clip-Bild bzw. ruft den Clip-Vorschub - Zusatzknochen (Schwanz) werden vom Clip mitbewegt,
  die Szene haelt kein Standbild fest.
- Spider-Man (Typ 0x2B, EM2B Auslieferung): Clips 7, 0x0B (mindestens 40 Bilder), 0x16 (mindestens 15 Bilder).
- Geprueft mit dem Sichel-Modell aus `r41/sichel` (5d119a808, 23 Knochen, 21 Clips; `RE15_CD_ROOT` auf dessen
  shared_assets, Lauf n9sf): Ablauf unveraendert, Bildfolge `H_hangar_belege/vgl_video_port_sichelmodell.jpg`. Keine
  Modell- oder Asset-Datei in dieser Spur geaendert.
- SF meldete "mit erzwungenem Cut 12 war das Fenster leer": das ist der Platzhalter-Anker (1.4); die Szene bringt dafuer
  die Region-Cull-Ausnahme mit (`re15_bf_szene_zeigt`, nur waehrend die Szene Cut 12 haelt). RE15_FORCE_CUT=12 allein
  zeigt weiter nichts.

## 5. Messung nachher

- Leon, Debug-Sprung 50A0 + Weg nach Sueden (Affen per Kill-Flag 8:0x2a/0x2b aus, damit der Weg frei ist):
  `[hangar] ROOM50A0 Szene Start: Schnitt 4 angefordert -> Cut 12`, Ende `Cut 4`; vor dem Ausloeser KEINE Szenenfigur im
  state.log; danach Birkin `st=1` (Gang Clip 1) zum Spieler, erster Treffer F579 (`ss1=3`, Spieler hp 100 -> 90), weitere
  Angriffe (ss1 3/4) - nicht starr. Bogen `H_hangar_belege/nachher_n8_bogen.jpg` (1 Bild/s).
- Vorher derselbe Weg mit der exe des Hauptbaums (Kopie, 2026-11-01 08:21): Birkin/Spider-Man in Cut 4 und 5 starr im Bild
  (`st=0 mo=0 af=0` durchgehend), `H_hangar_belege/vorher_v2alt_bogen.jpg`.
- Elza (`RE15_TITLE_SHOT_CHAR=1`, PL04, Elza-Bit 1), ROOM50A1: Szene Start Cut 4 -> 12, Ende Cut 4, Birkin greift an
  (F609 hp 90, F744 hp 80), `H_hangar_belege/nachher_e1_elza_bogen.jpg`.
- Sichel-Modell: s. Abschnitt 4.
- Tests: `unit_birkin_szenen` (neu: Sichttabelle Cut 0..13, Gegenproben ohne Sicht / Spieler nicht frei, Phasen 1..7,
  Hochhalten > 2500, Aufschlag, groesster Schritt <= 700 im Ansprung/Halten, kein Stillstand > 1 Takt, Flucht, Riegel/
  Kamera/Zonen nach dem Ende, Angriff, Wiedereintritt; Leon 50A0 und Elza 50A1). Gegenproben am Code: alter Ausloeser
  (wartet auf angezeigten Cut 12) -> 8 FAIL; ungeglaettetes Klauenziel -> "springt: 866". Zielauswahl
  `-R "birkin|hangar|50a|spiderman|formen|r38_birkin|room50"`: 19/19.

## 6. Offen / Abweichungen

1. "Szene gesehen" ist ein Prozess-Merker: nach Laden eines Spielstands im selben Prozess bleibt er, nach Neustart laeuft
   die Szene noch einmal. Fuer Speicherstaende braucht es ein Bank-9-Bit (VERTRAG 1.1: 126-133 vergibt der Orchestrator).
2. Westtuer (aus 4070, Ziel-Cut 11) im echten Fenster nicht bis Cut 6 gefahren: der Autopilot blieb im Tuerbereich
   (x -25900..-23885, z -16268..-17238) haengen. Die Sicht von Cut 6 ist im Unit-Riegel geprueft (Tabelle 1.4).
3. Wiedereintritt nur im Unit-Riegel (Raum neu laden) geprueft, nicht im echten Fenster.
4. Abweichungen zum Video, die am Modell haengen: im Video wirkt Birkin anfangs geduckter (Port: Clip 0 aufrecht); der
   Spider-Man ist im Port dunkler/spinnenartiger (EM2B Auslieferung); im Wurf h70..72 liegt der Spider-Man im Port
   naeher an Birkin. Haltende Hand: das Video zeigt die Sichel unten links und den Spider-Man links oben - mit EM30-Clip
   0x0B hebt nur die rechte Hand (13-15) hoch; die Szene nutzt sie mit seitlichem Versatz.
5. PSX-Plattform: die Region-Cull-Ausnahme ist nur im PC-Zeichner (main.c) gebaut.
6. Der Affe (0x29) des Originals faellt weiterhin im Nordgang herab; waehrend der Szene schuetzt der Riegel (player_mode 2,
   Szenenschutz) den Spieler.

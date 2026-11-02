# Runde 41 — Spur PF "panelfix": Abnahme 1

Pruefer: unabhaengiger Abnahme-Agent. Baum `.claude/worktrees/r41_panelfix`, Zweig `r41/panelfix`, Kopf e68e37cf3 (Code-Commit 9a96cdde2).
exe: `re15_port/build/platform/pc/re15_pc.exe` 2026-11-02 02:39:09, `local_build.sh build` -> `ninja: no work to do` (alle geaenderten Quellen,
`gen/panel2030_r41.inc` 02:10, `gen/schrank2010_r41.inc` 01:53, `gen/ventilkette_r41_texte.inc` 02:04, `panel2030_r41.c` 02:39:05, sind aelter).

Massstab, Nutzer woertlich (2026-11-02): "Der Kasten mit der Sicherung in ROOM 2010 sieht schon klasse aus, aber das Bild ist / der Kasten ist bei der
Kamerasicht irgendwie komisch gestaucht.... das ist noch falsch. Auch das Raetsel in 2030 das du gebaut hast ist Klasse! Aber Zahlen und Texte auf den Panel
in Nahansicht sind kaum lesbar/verschwommen. Und der Text am Anfang vor dem Start des Raetsels ueberlappt mit dem yes / no auswahl."

## Gesamturteil

**NICHT BESTANDEN — wegen Punkt 3.** Der Nutzer hat das Panel nach dem Bau der Spur erneut beurteilt (ueber den Orchestrator, woertlich):
"Das Panel ist immer noch schwer lesbar/verpixelt. Vor allem das A 100, und das 80 links, das Dreh panel, der BRIDGE und PRESSURE 80 Text, der B 50 und der
C30 Text und auch der Caution High Pressure Text." Das Nutzerurteil ist der Massstab; Punkt 3 ist damit nicht bestanden, unabhaengig von Tests und Glyphenmass.
Die Ursachenanalyse je Text steht in §3a, sie ist die Vorlage der Nachbesserung.

**Wichtig fuer die Nachbesserung (gemessen 2026-11-02 10:25):** die Spiel-Instanz des Nutzers (PID 100752, gestartet 01:24:55) laeuft aus dem HAUPTBAUM
`C:\workspace\git\reAi_v2\re15_port\build\platform\pc\re15_pc.exe` (Stand 2026-11-01 08:21:30). master (789f6e681) enthaelt den Code-Commit der Spur
(9a96cdde2) NICHT (`git branch --contains 9a96cdde2` = nur `r41/panelfix`). Ihr Fenster (nur lesend per gdigrab abgegriffen, Client 960x720, nicht maximiert;
`befund.log` daneben: R2030) zeigt das Panel im Stand VOR der Spur: "PRESSURE 80", Arial-Schrift, Ziffern im Zeiger-Instrument; 0/42 Glyphen der
TEX.TIM-Schrift, bis auf die Cursorflaeche (300 Bildpunkte in x 148..247, y 108..130) bildgleich mit dem Vorher-Bild der Spur. Bild:
`nutzer_fenster_hauptbaum_vorPF.png`, Gegenueberstellung `panel_texte_nutzer_gegen_pf.png`. Das Wort "PRESSURE 80" im Nutzerurteil passt dazu (der Stand
der Spur zeigt "SET 80"). Ob der Nutzer den Stand der Spur selbst gesehen hat, ist damit offen; aber auch dieser ist ein 320x240-Bild, dreifach nearest
vergroessert (§3a), und wirkt im Fenster sichtbar pixelig.

| Punkt | Urteil | Kernbeleg |
|---|---|---|
| 1 Fenster, echter Ablauf, Leon + Elza | bestanden | gdigrab per hwnd der eigenen PID, Renderer `direct3d`, Fenster 960x720 (Standard auf 1920x1080) und 320x240 |
| 2 Kasten 2010, Seitenverhaeltnis | bestanden, Restmangel M1 | Inhalt waagrecht/senkrecht gegen 2060: Rueckwand 1,021, Fluegel 1,013 (vorher 0,80 / 0,72); Kanten = Projektion (Median <= 0,73 Bildpunkte) |
| 3 Panel 2030, Schrift | **NICHT bestanden (Nutzerurteil)**, M2 | gemessen: 42/42 Glyphen = 8x8-Schrift aus TEX.TIM, nearest 3,0-fach; Ursache "verpixelt" = 320x240-Ebene, §3a |
| 4 Frage vor dem Raetsel | bestanden | Zeile 2 der Frageseite traegt nur Cursor 161..163, "Yes" 175..196, "No" 237..259; Bytes @0x80028630 `10 00 84 24`, @0x80028674 `10 00 84 24` |
| 5 Tests | bestanden | unit 27/27, Integration 10/10 gruen |

**M1 (gering, schon vor Spur PF vorhanden, von PF nicht verursacht):** der Inhalt des Kastens ist geschert. Rueckwand: die waagrechten Strukturen des
Sicherungsfelds liegen im Fenster um +4,5 Grad schief gegen die Kastenkanten (2060-Original: 0,0 Grad); Fluegel: die Senkrechten (Kanten der zwei Schilder)
um 6,1 Grad. Ursache: die von Hand gesetzten Vierecke `Q_INNEN` (Ecke rechts oben) und `Q_TUER` (linke Kante = Bildrand) passen nicht zur Kamera Cut 6.
Das Seitenverhaeltnis, also das "gestaucht" des Nutzers, ist davon unabhaengig richtig. Wiederholungsweg und Abhilfe in §6.

**M2 (Punkt 3):** Panel-Beschriftung, Zeiger-Instrument und Drehscheiben sind fuer den Nutzer schwer lesbar/verpixelt. Ursache je Text §3a, Wege §6.

## Messweg (fuer alle Punkte)

* Eigener Treiber `PF_belege/abnahme1/werkzeuge/fahrt.py`: eigene exe-Kopie `re15_pc_pfabn1_*`, `SDL_ASSERT=always_ignore`, **kein** `RE15_AUTOSHOT`,
  **kein** `RE15_SOFTWARE_RENDER`, **kein** `RE15_FRAMEDUMP` (alle drei maskieren laut Skill `re15-port-visual-verify` Fehler bzw. lesen zurueck).
  Fenster per `EnumWindows` + `GetWindowThreadProcessId` der EIGENEN PID gesucht (parallele Agenten haben Fenster gleichen Titels), Daueraufnahme
  `ffmpeg -f gdigrab -i hwnd=<h> -framerate 4`, dazu die Ankunftszeit jeder neuen debug.log-Zeile (Zuordnung Bild <-> Ereignis). Log: `[render] SDL-Renderer direct3d`.
* Echter Ablauf, kein Sprung in die Nahansicht: Spielstand (Werkzeug `test_r41_nachtrag30 karte`, Muster `integration_r41_nachtrag30_laden_*`) +
  `RE15_CONTINUE_TEST`/`RE15_CARD_AUTO`, dann **zu Fuss** per `RE15_WEGPLAN` (Pad-Bits, `dachkiste_r36.c re15_dachkiste_wegplan_pad`).
  * Kasten: Start ROOM2010/2011 (-4600,-4950) quer durch den Raum zum Kasten (4950,-2770), Blick 0, Aktionstaste (Kamera schaltet selbst auf Cut 3),
    Text 40 weggedrueckt, dann nach (3807,-942) **unter Cut 3** (Zone 12 @ROOM2010 Kamerazonen nicht betreten), Blick 2048; danach nach Cut 4 und zurueck.
    Plan `werkzeuge/plan_s_leon.txt` (Elza: RAUM 2011).
  * Panel: Start ROOM2070/2071, Ring-Tuer per `RE15_FIRE_AOT=1` (Tuer-AOT Slot 1, `[aot] DOOR FIRE slot=1`) -> `loaded room2030.rdt`/`room2031.rdt`, zu Fuss
    (6575,-9675) -> (350,-9675), Blick 1024 = vor dem Pult, ab Spielbild 400 die Tasten `werkzeuge/skript_p.txt` (Aktion am Pult, Seite, Ja, Text 41,
    A->B, B->C, C->A = 80).
  * Grenzen dieses Wegs: Laden ueber `RE15_CONTINUE_TEST` statt Titelmenue, Tuer per `RE15_FIRE_AOT` statt Aktionstaste an der Tuer. Raum, Kamera, Kasten,
    Pult, Frage und Panel laufen ueber den Spielweg.
* Laeufe (alle Bilder im scratchpad, die Belege kopiert): `s_leon`, `s_elza` (Kasten, Fenster 960x720), `p_leon`, `p_elza` (Panel, 960x720), `p_leon_w1`
  (Panel, `RE15_WINDOW_SCALE=1`, 320x240). Vollbild (`RE15_FULLSCREEN`) habe ich NICHT gefahren (deckt den Schirm des Nutzers und der parallelen Agenten ab).
* Logisches Bild aus 960x720: Mitte jedes 3x3-Blocks; zugleich wird gezaehlt, ob jeder Block einfarbig ist (`werkzeuge/logisch.py`).

## 1./2. Sicherungskasten ROOM2010/2011 — "komisch gestaucht"

Bilder: `PF_belege/abnahme1/kasten_cut3_leon_elza.png` (Fenster, Tuer offen, Figur neben dem Kasten, t = 28,0 s), Ablauf: Kamera Cut 4 -> Cut 3 (13,1 s),
Aktion 14,9 s, `[schrank2010] Tuer auf (rot_y 1024)` 15,6 s, Text 40, Figur weg 26..29 s, Cut 4 31,3 s, Cut 3 36,9 s; Leon und Elza bildgleich.

### 2a. Kastenkanten im Fenster gegen die Projektion der Kasten-Geometrie (RDT-Kamera ROOM2010 Cut 3)

Kamera aus der RDT selbst gelesen (`werkzeuge/geo.py`): ROOM2010.RDT Kameratabelle @0x60 (Kopf @0x24), Cut 3 @0xC0: fov 26684 -> H = 26684 >> 7 = 208,
Auge (2930,-2827,-6385), Ziel (7022,962,10672); Projektion sx = 160 + H*x/z, sy = 120 + H*y/z (camera_common.c, FUN_80053ca4). Geometrie der Port-Props
(`schrank2010_r41.c`, `schrank_bild.py`): Rueckwand x 5400, z -2000..-3300, y -3650..-50; Fluegel offen (rot_y 1024) z -2000, x 4100..5400.
Projizierte Ecken (logisch): Rueckwand (222.9,36.6) (265.3,22.3) (245.7,219.8) (213.9,186.2); Fluegel freier Rand (165.6,34.0)-(164.8,192.8).

Messung (`werkzeuge/kanten.py`): Maske = |Fenster - BG03| > 24 in Fensteraufloesung (BG03 = Original-Hintergrund aus `probe_bg_dump`, nearest auf 960x720),
je Zeile/Spalte die erste Maskenkante in +-5 Bildpunkten um die projizierte Kante:

| Kante | Leon (Zeilen/Spalten) | Median | Elza Median |
|---|---|---|---|
| Rueckwand rechts (z -3300) | 571 | -0,67 | -0,67 |
| Fluegel links (x 4100) | 346 | -0,04 | -0,05 |
| Oberkante Rueckwand | 107 | +0,39 | +0,39 |
| Oberkante Fluegel | 133 | -0,26 | -0,26 |
| Unterkante Rueckwand | 16 | +0,04 | +0,04 |
| Unterkante Fluegel | 127 | -0,73 | -0,73 |

(logische Bildpunkte; der 90-%-Betrag 2,6..4,4 kommt von Stellen, an denen die dunkle Textur dem dunklen Hintergrund gleicht.) Die gezeichneten Flaechen liegen
also in der Kamerasicht genau dort, wo die Geometrie hinprojiziert wird; der Renderer verzerrt nichts. Bild: `kasten_kanten_projektion.png` (links Maske gegen
Projektion: gruen = beides, blau = nur Projektion, rot = nur Bild = Figur; rechts BG03 mit der projizierten Rueckwand). Die projizierte Rueckwand deckt sich mit
dem im Original-Hintergrund BG03 gemalten Schrank (Schaltschrank mit Tastenfeld): die Aussenform 1300 x 3600 (B/H 0,361) ist durch den Original-Hintergrund
festgelegt und darum richtig; "gestaucht" kann nur den Inhalt betreffen.

### 2b. Seitenverhaeltnis des Inhalts im Fenster (Mass)

Eigenes Verfahren, ohne die Handecken des Generators (`werkzeuge/inhalt.py`, `werkzeuge/fluegel.py`):
* 2060-Seite: BG06 (= fuse_box.bmp) NUR ueber die RDT-Kamera ROOM2060 Cut 6 (@0x120, Auge (-5146,-2700,-442), H 208) auf die Wandebene entzerrt,
  10 Welteinheiten je Bildpunkt. Rueckwand: Ebene z = const (achsparallel; die Lage z0 aendert nur den Gesamtmassstab, nicht B/H). Fluegel: senkrechte Ebene,
  Richtung aus dem Fluchtpunkt seiner Ober- und Unterkante (139,7 / 68,7) = 64,6 Grad; der Fluchtpunkt liegt auf dem Kamera-Horizont y 68,0, also waagrecht.
  Probe der Entzerrung: die Sicherungsroehren stehen darin senkrecht (Neigung +0,02..+0,49 Grad, `werkzeuge/neigung.py`), die Waagrechten liegen bei 0,0 Grad.
* Port-Seite: das gdigrab-Bild unter Cut 3 auf die Port-Flaechen (Rueckwand x 5400, Fluegel z -2000) entzerrt, gleicher Massstab.
* Dann `cv2.findTransformECC` (MOTION_AFFINE) zwischen beiden; V = Massstab waagrecht / Massstab senkrecht des Port-Inhalts gegen 2060
  (1 = unverzerrt, < 1 = waagrecht gestaucht). Gegenprobe: dasselbe Port-Bild kuenstlich auf 80 % Breite gestaucht.

| Bild | Rueckwand V (ECC) | Fluegel V (ECC) |
|---|---|---|
| nachher Leon (s_leon b_00113, 960x720) | **1,021** (0,964) | **1,013** (0,984) |
| nachher Elza (s_elza b_00113) | 1,021 (0,964) | 1,013 (0,984) |
| Gegenprobe Leon, Port auf 80 % gestaucht | 0,818 (0,965) | 0,813 (0,985) |
| vorher, Basis-exe (Bild der Spur PF `pf/runs/s_vorher_w3/d.png`) | ECC passt nicht (0,59) | 0,719 (0,963) |
| vorher, Basis-exe (`pf/runs/s_vorher_leon_weg/d.png`) | ECC passt nicht (0,60) | 0,722 (0,956) |

Ausgabe: `PF_belege/abnahme1/mass_2b.txt`. Die Gegenprobe reagiert richtig (1,021 * 0,8 = 0,817). Fuer die Rueckwand vorher findet ECC keine Deckung (Knick
der einen affinen Flaeche), dafuer gilt 2c.

### 2c. Gegenrechnung: die Abbildung des Generators analytisch gegen die Kamera

`werkzeuge/analytisch.py` (Ausgabe `mass_2c_analytisch.txt`): Port-Welt -> Texel (UV wie `md1_gitter`) -> BG06-Bildpunkt (Homographie aus `schrank_bild.py`)
-> 2060-Welt auf der Flaechenebene (Strahl der Kamera Cut 6); Jacobi-Matrix an 35 Punkten je Flaeche:

| Abbildung | V Median (Spanne) | Port-Senkrechte | Port-Waagrechte |
|---|---|---|---|
| Rueckwand vorher (Q_INNEN stumpf auf 1300:3600) | 0,799 (0,782..0,816) | -1,3 Grad | +4,2 Grad |
| Rueckwand nachher (Spur PF) | **1,016** (0,996..1,037) | -1,3 Grad | **+4,2 Grad** |
| Rueckwand, Ecke rechts oben nach dem Horizont (128,37) statt (122,28) | 1,021 | -1,3 Grad | +0,4 Grad |
| Fluegel vorher (Q_TUER stumpf) | 0,718 (0,706..0,731) | +6,0 Grad | -0,2 Grad |
| Fluegel nachher (Spur PF) | **1,017** (0,999..1,034) | **+6,0 Grad** | -0,2 Grad |

Fenster (2b) und Rechnung (2c) stimmen ueberein: Seitenverhaeltnis vorher 0,80 / 0,72 (Spur PF hatte 0,795 / 0,740), nachher 1,02 / 1,01..1,02. **Das
"gestaucht" ist behoben.** Die Scherung (+4,2 / +6,0 Grad) ist vorher und nachher gleich: M1.

### 2d. Andere Kamerasichten

Projektion von je 81 Punkten der Rueckwand bzw. des offenen Fluegels in alle Cuts von ROOM2010: Cut 0 @0x60, Cut 1 @0x80, Cut 2 @0xA0: 0/81 (Kasten
nicht im Bild). Cut 3 @0xC0: 81/81 (oben). Cut 4 @0xE0: Rueckwand 17/81 (Bildrand links, x < 23), Fluegel 43/81 — dort sind die Props ausgeblendet
(`re15_schrank2010_r41_verdeckt`, PORT-WAHL der Spur N, M4), im Fenster sind die Spalten 0..27 unter Cut 4 **bildgleich BG04** (max. Abweichung 0, Leon und
Elza, s_* b_00133): der Kasten erscheint dort als unveraenderter Original-Hintergrund, verzerrt also nichts. Cut 5 @0x100 (Kennung 1, keine Kamerazone fuehrt
hinein, nur 10 x 30 Bildpunkte gross): Props ausgeblendet. Die Nahansichten beim Einsetzen sind die Original-Hintergruende ROOM2060 Cut 8/9 (Riegel
`_schrank_*` Lauf b, bssdiff >= 95 %), dort gibt es keine Port-Flaeche.

**Urteil Punkt 2: bestanden** (Seitenverhaeltnis 1,02 / 1,01 im Fenster, Kanten = Projektion), **Restmangel M1** (Scherung, §6).

## 3. Panel 2030/2031 — Zahlen und Texte in Nahansicht

Bilder: `panel_leon_fenster3.png`, `panel_elza_fenster3.png` (960x720), `panel_leon_fenster1.png` (320x240), `panel_schilder_zoom8.png` (logisch, 8-fach),
`panel_leon_text41.png` (Text 41 im Panel). Ablauf je Figur: Aktion am Pult 27,2 s -> Text 40 -> Ja 33,5 s -> `[panel2030] offen: A 100 B 0 C 0` -> Text 41 ->
A->B, B->C, C->A -> `[panel2030] GELOEST: A 80 B 20 C 0` -> Text 43 (Leon 56,4 s, Elza 56,6 s).

**Scharf, Pixel 1:1:** im 960x720-Fenster ist jeder 3x3-Block der Panel-Ebene einfarbig (Anteil **1,0000**, `werkzeuge/logisch.py`), also nearest ohne
Filterung und ohne Neuabtastung; kein `SDL_HINT_RENDER_SCALE_QUALITY` in `platform/pc/src/*.c` (SDL-Standard nearest), `SDL_RenderSetLogicalSize(320, 240)`
render_pc.c:716. (Zum Vergleich unter Cut 3 mit 3D-Figur: 0,845 — die 3D-Ebene wird in Fensteraufloesung gezeichnet, das Panel nicht.)

**Glyphen = TEX.TIM-Schrift** (`werkzeuge/etiketten.py`, eigene Dekodierung `werkzeuge/schrift.py`): je Zeichen jede Lage im Schild gesucht, an der jeder
Glyphen-Index genau EINE Farbe hat (keine Zwischentoene) und die Tintenfarbe unter den transparenten Texeln im Tintenrechteck fehlt.

| Bild | BRIDGE | SET 80 | 80 | A 100 | B 50 | C 30 | CAUTION | HIGH PRESSURE | gesamt |
|---|---|---|---|---|---|---|---|---|---|
| Leon 960x720 (b_00169) | 6/6 | 5/5 | 2/2 | 4/4 | 3/3 | 3/3 | 7/7 | 12/12 | **42/42** |
| Leon 320x240 (p_leon_w1 b_00169) | | | | | | | | | 42/42 |
| Elza 960x720 (b_00169) | | | | | | | | | 42/42 |
| Leon / Elza nach dem Umfuellen (b_00205) | | | | | | | | | 42/42 / 42/42 |
| vorher, Basis-exe (`pf/runs/p_vorher_w3/b.png`, `p_vorher_w1/b.png`) | | | | | | | | | 0/42 / 0/42 |

Farben je Index (Leon): helle Schrift Index 1 = (222,222,206) = TEX.TIM CLUT (256,480) Eintrag 1 `0x677b`, Index 2 (206,206,189) = `0x5f39`, Index 3
(173,173,156) = `0x4eb5`, Schatten Index 8 (57,49,74) = `0x24c7` — CLUT-Zeile 0 unveraendert; "SET 80" dunkel ohne Schatten; "80" rot (231,74,49).
Vorschub gemessen: Buchstaben 8 (B@217,R@225,...), Ziffern 6 (8@248, 0@254; 1@62, 0@68), wie im Dossier als PORT-WAHL angegeben.

**Disasm FUN_80029214 selbst geprueft** (PSX.EXE, `re15_disasm.py`):

| Adresse | Bytes | Befehl | Bedeutung |
|---|---|---|---|
| @0x8002931c | `e0 ff 42 24` | addiu v0,v0,-32 | k = c - 0x20 |
| @0x80029320 / @0x80029324 | `1f 00 43 30` / `c0 18 03 00` | andi v1,v0,0x1f / sll v1,v1,3 | u = (k & 0x1f) * 8 |
| @0x80029328 / @0x8002932c | `e0 00 42 30` / `80 11 02 00` | andi v0,v0,0xe0 / sll v0,v0,6 | v = ((k & 0xe0) >> 5) * 8, ins Byte 1 |
| @0x800292a0 / @0x800292a4 | `80 74 03 3c` / `80 80 63 34` | lui/ori 0x74808080 | SPRT 8x8, Farbe 0x808080 |
| @0x800292b4 | `e0 01 c2 24` | addiu v0,a2,480 | CLUT-Zeile 480 (+ Halbtransparenz-Bit) |
| @0x800292dc | `10 00 84 34` | ori a0,a0,0x10 | CLUT x = 0x10 * 16 = 256 |
| @0x800292f8 | `08 00 4a 25` | addiu t2,t2,8 (Verzoegerungsschlitz) | Vorschub 8 je Zeichen, auch fuer Leerzeichen |
| @0x800295dc / @0x800295e4 | `00 03 06 34` / `00 01 07 34` | a2 = 0x300, a3 = 0x100, jal 0x8006b310 | tpage 4 bpp bei VRAM (768,256) |

TEX.TIM (`shared_assets/PSX/DATA/TEX.TIM` = `info/Re1.5/PSX/DATA/TEX.TIM`, md5 d56e6b90...): 4 bpp, CLUT-Rechteck (256,480) 32x24, Bild 320 x 256 Halbworte;
mit dem Slot-Lader nach VRAM (704,256) beginnt tpage 768 bei TIM-Texel x = (768-704)*4 = 256 (Herleitung render_pc.c `re15_render_pc_debug_text`, Slot 0x1b
@0x800212d0 `ori v0,zero,0x1b` gesehen). Die Glyphen, die ich daraus selbst dekodiert habe (`tex_tim_schrift_x256.png`), sind die im Fenster. Behauptung der
Spur bestaetigt.

**Nicht in der 8x8-Schrift:** das Feld "EXIT" mit Pfeil. Es ist Original-Kunst aus ROOM11F0 Cut 10 (x 210..238, y 175..204): im Panel bei (240,180), alle
Bildpunkte innerhalb der 15-Bit-Rundung (max. Abweichung 7) gleich dem Original-Hintergrund (`bg1/ROOM11F10.ppm`). Seine weichen Kanten sind die des Originals
(`panel_exit_zoom8.png`).

**Hinweis H1 (kein Mangel, PORT-WAHL):** waehrend Text 41/43 im Panel schimmert das rote Schild durch das Nachrichtenband: Schildschrift ohne Band 209,
im Band 25, Bandgrund 4,7, Nachrichtenschrift 211 (Helligkeit, b_00157 gegen b_00169). Der Nachrichtentext bleibt lesbar (Kontrast 8:1 gegen das
Geisterbild), "drain," endet dicht vor dem durchscheinenden "TION" (`panel_leon_text41.png`). Die Nachrichtenschrift selbst ist die normale Spielschrift.

**Urteil Punkt 3: NICHT bestanden** (Nutzerurteil, siehe Gesamturteil). Was oben gemessen ist, gilt: die Spur hat die Arial/JPEG-Schrift durch die
8x8-Spielschrift ersetzt, Texel fuer Texel, ohne Filter. Das behebt "verschwommen", nicht "verpixelt".

### 3a. Herkunft, Aufloesung und Skalierung jedes genannten Textes (Auftrag des Orchestrators)

**Gemeinsamer Weg aller Panel-Inhalte (Code gelesen, im Fenster gemessen):**
1. Alles ist EIN Bild 320 x 240 RGBA8888: `re15_panel2030_r41_rgba()` (engine/src/panel2030_r41.c) kopiert je Bild den 15-Bit-Grund
   `re15_r41n_panel_grund` (gen/panel2030_r41.inc, Generator `tools/r41_nachtrag30/panel_bild.py`) und zeichnet zur Laufzeit Pegel, Zeiger (`zeiger()`,
   `strecke()` mit weicher Kante), Lampe, Cursor und das Nachrichtenband hinein.
2. `ventilkette_r41_pc.c:320` `re15_render_pc_show_fmv(bild, 320, 240)` -> Textur 320 x 240, `SDL_TEXTUREACCESS_STATIC` (render_pc.c:2234), gezeichnet mit
   `SDL_RenderCopy(s_renderer, s_fmv_tex, NULL, &full)`, `full = {0, 0, 320, 240}` (render_pc.c:1492-1493) in die logische Groesse 320 x 240
   (`SDL_RenderSetLogicalSize`, render_pc.c:716). SDL vergroessert die logische Flaeche auf das Fenster.
3. Fenster des Nutzers: Client **960 x 720** (gemessen, nicht maximiert) = Faktor **3,0**; meine Laeufe ebenso (Standard: groesste ganze Stufe <= 90 % des
   Schirms, render_pc.c:637-644). Filter: **nearest** — kein `SDL_HINT_RENDER_SCALE_QUALITY` und kein `SDL_SetTextureScaleMode` in `platform/pc/src/*.c`
   (SDL-Standard "0"); gemessen: jeder 3x3-Block einfarbig, Anteil 1,0000 im Nutzerfenster und in meinen Laeufen (Leon, Elza). Es wird beim Anzeigen also
   nichts gefiltert. Jeder Texel wird ein 3 x 3-Block. (Bei `RE15_FULLSCREEN` auf 1920x1080 waere der Faktor 4,5: Texel abwechselnd 4 und 5 Punkte breit.)
4. Die Unschaerfe des alten Stands steckt im Bild selbst: `panel_kit.text()` (Arial Bold, 4-fach ueberabgetastet, Gauss 0,1..0,12) -> `fertig()`:
   `cv2.INTER_AREA` auf 320x240, Rauschen sigma 1,9, JPEG q84 mit 4:2:0, Gamma 1,16, 15 Bit.

**Je Text** (Masse im logischen Bild = Texel; Fenster = Texel x 3; alt = Nutzerfenster/Hauptbaum vor PF, neu = Spur PF; Werkzeuge `werkzeuge/textmass.py`,
`mass_3_textgroessen.txt`; Arial-Masse aus `ImageFont.truetype(arialbd.ttf, Groesse*4)` / 4 mit den Groessen aus `git show 789f6e681:.../panel_bild.py`):

| Text | Quelle alt (Nutzer) | Quelle neu (PF) | Schrifthoehe alt -> Fenster | neu -> Fenster | Strich alt / neu | Skalierung, Filter |
|---|---|---|---|---|---|---|
| A 100 | Arial Bold 6,0 px im Bild gebacken (`schildchen`), INTER_AREA+JPEG | 8x8-Glyphen TEX.TIM, CLUT-Zeile 0 + Schatten, ins Bild gestempelt (`beschriften`) | 4,2 Texel -> 13 Punkte (17 x 4 Texel gemessen) | 7 Texel -> 21 Punkte (29 x 7) | 0,75 Texel (grau) / 1 Texel = 3 Punkte | 320x240 -> 960x720, x3, nearest |
| 80 links (rot) | Arial Bold 6,6 px, rot | TEX.TIM-Ziffern, rote 16er-CLUT | 4,8 Texel -> 14 Punkte (6 x 4 gemessen) | 7 -> 21 (11 x 7) | 1,0 / 1 | x3, nearest |
| "Dreh panel" = Zeiger-Instrument | Generator-Kunst `instrument`: Scheibe R 13,9 Texel (aussen ~37 Texel = ~110 Punkte), Teilstriche 0,48..0,72 Texel breit, Ziffern 0/50 Arial 4,6 px (3,2 Texel hoch = 10 Punkte), 80 Arial 6,0 px; Zeiger zur Laufzeit weich (Halbbreite 0,62/0,34 Texel) | dieselbe Kunst ohne Ziffern (`instrument_ohne_zeiger`), Zeiger gleich | Ziffern 3,2..4,2 Texel -> 10..13 Punkte | keine Ziffern; Striche unter 1 Texel bleiben graue Schlieren | <1 Texel | x3, nearest |
| "Dreh panel" = vier Drehscheiben | Generator-Kunst `drehscheibe`: R 13,9 Texel, Querbalken 2,3 Texel (7 Punkte), 12 Teilstriche 0,44 Texel breit | unveraendert (Nutzer- und PF-Bild: 98,5 % der Bildpunkte innerhalb 8 Stufen gleich) | — | — | Balken 2,3 / Striche 0,44 Texel | x3, nearest |
| BRIDGE | Arial Bold 8,2 px, hell auf rot | TEX.TIM, CLUT-Zeile 0 + Schatten | 6,0 Texel -> 18 Punkte | 7 -> 21 (47 x 7) | 1,25 / 1 | x3, nearest |
| PRESSURE 80 (neu: SET 80) | Arial Bold 6,7 px, dunkel auf hell | TEX.TIM, dunkle CLUT ohne Schatten, Wortlaut PORT-WAHL "SET 80" (88 > 54 Bildpunkte) | 5,0 Texel -> 15 Punkte (46 Texel breit) | 7 -> 21 (39 x 7) | 1,0 / 1 | x3, nearest |
| B 50 | Arial Bold 6,0 px | TEX.TIM | 4,2 -> 13 (14 x 5 gemessen) | 7 -> 21 (23 x 7) | 0,75 / 1 | x3, nearest |
| C 30 | Arial Bold 6,0 px | TEX.TIM | 4,2 -> 13 (14 x 5) | 7 -> 21 (23 x 7) | 0,75 / 1 | x3, nearest |
| CAUTION / HIGH PRESSURE | Arial Bold 6,0 / 5,6 px auf rotem Schild 55 Texel breit | TEX.TIM auf rotem Schild 108 Texel breit | 4,2 / 4,0 Texel -> 13 / 12 Punkte | 7 -> 21 (55 x 7 / 98 x 7) | 0,75..1,0 / 1 | x3, nearest |
| (zum Vergleich) EXIT | Original-Kunst ROOM11F0 Cut 10 (x 210..238, y 175..204) | gleich | 8..9 Texel -> 24..27 Punkte | gleich | 1..2 Texel | x3, nearest |

**Warum es verpixelt/schwer lesbar aussieht:**
* Alter Stand (das, was im Nutzerfenster steht): die Schrift ist nur 4..6 Texel hoch und ihre Striche sind duenner als ein Texel. Beim Verkleinern auf
  320x240 und durch JPEG 4:2:0 werden die Buchstaben zu grauen Flecken (Zwischentoene 77..97 % der Schriftpunkte, `mass_3_textgroessen.txt`), dann
  vergroessert SDL jeden Fleck zu 3x3-Bloecken: verschwommen UND verpixelt, 12..18 Fensterpunkte hoch.
* Stand der Spur: die Schrift ist scharf (jeder Glyphen-Index genau eine Farbe, 42/42; das grobe Mass in `mass_3_textgroessen.txt` zaehlt bei PF die
  Grautoene 2/3 und den Schatten der Spielschrift sowie Schildkanten mit), aber sie ist eine
  8x8-Bitmapschrift in einem 320x240-Bild: 7 Texel hoch = 21 Fensterpunkte, jeder Strich 3 Punkte breit, Schraegen als 3-Punkte-Treppen. Das ist bei
  Faktor 3 sichtbar "pixelig". Zeiger-Instrument und Drehscheiben sind unveraendert Generator-Kunst mit Einzelheiten unter einem Texel (Teilstriche, Zeiger),
  die beim Verkleinern zu Schlieren werden und dann als 3x3-Bloecke erscheinen.
* Zum Massstab: die Original-Nahansichten haben dieselbe Aufloesung und ebenso kleine Beschriftungen (ROOM2050 Cut 15 Skala "0 10 20 ... 100" und
  "DANGER", ROOM11F0 Cut 10 Skala, `original_nahansichten_2050cut15_11f0cut10.png`); auch sie erscheinen im Port dreifach nearest.

**Folgerung fuer die Nachbesserung (Ursache, keine Entscheidung):** solange das Panel EIN 320x240-Bild ist, kann kein Text darin feiner als 3x3 Punkte
werden. Lesbarer wird es nur ueber (a) groessere Schrift im 320x240-Bild (z. B. die Nachrichtenschrift des Spiels, Breiten DEBUG.BIN @0x4416,
Zeilenabstand 16 @0x800289ac = 48 Fensterpunkte) und groessere Zeichen/Teilstriche, oder (b) eine hoeher aufgeloeste Panel-Ebene (Textur z. B. 960x720 bzw. 1280x960, in
dasselbe Rechteck gezeichnet; `re15_render_pc_show_fmv` nimmt beliebige w x h) bzw. eine Text-Ebene in Fensteraufloesung. (b) weicht von der
PSX-Aufloesung ab; welche Richtung gilt, entscheidet der Nutzer. Ausserdem muss der Hauptbaum neu gebaut werden, bevor der Nutzer wieder misst
(Memory "Nach master-ff den HAUPTBAUM-EXE neu bauen").

## 4. Text vor dem Raetsel gegen die Ja/Nein-Auswahl

Bilder: `frage40_leon.png`, `frage40_elza.png` (je Seite 1 | Seite 2, 960x720). Zeilen im logischen Bild (`werkzeuge/zeilen.py`, Schriftpunkte min(R,G,B) > 150):

| Bild | Zeile 1 (y 180) | Zeile 2 (y 196) |
|---|---|---|
| Seite 1 "A control panel for the bridge." (Leon, Elza) | x 35..240 | nur Seitenpfeil x 237..246 |
| Seite 2 "Will you operate the panel?" (Leon, Elza, Leon 320x240) | x 35..216 | Cursor 161..163, "Yes" 175..196, "No" 237..259 — sonst nichts |

**RE selbst geprueft (PSX.EXE, `re15_disasm.py dis 0x800285f0`):**

| Adresse | Bytes | Befehl |
|---|---|---|
| @0x80028624 | `36 85 84 94` | lhu a0,-0x7aca(a0) = DAT_800b8536 (Cursor) |
| @0x80028630 | `10 00 84 24` | addiu a0,a0,16 -> Cursor-y = DAT_800b8536 + 0x10 |
| @0x8002863c..@0x80028650 | ... `a0 00 42 24` | x = 160 + 70 * Wahl (addiu v0,v0,160) |
| @0x8002866c | `36 85 84 94` | lhu a0, DAT_800b8536 ("Yes No") |
| @0x80028674 | `10 00 84 24` | addiu a0,a0,16 -> y = DAT_800b8536 + 0x10 |
| @0x80028680 | `ae 00 84 34` | ori a0,a0,0xae -> x 174; Zeichenkette DAT_800c4954 = DEBUG.BIN @0x4954 `59 65 73 01 2e 4e 6f 00` ("Yes", "No") |
| @0x8002886c | (FUN_80028868) lhu v0, DAT_800b8536 | erste Textzeile auf y = DAT_800b8536 |
| @0x800289ac | `10 00 42 24` | addiu v0,v0,16 = Zeilenvorschub 16 (Zeile 2 = DAT_800b8536 + 0x10) |
| @0x800289c0 | `16 44 63 24` | Breitentabelle 0x800c4416 (DEBUG.BIN @0x4416) |
| @0x80027eec | `22 00 02 34` | Box-x 0x22 = 34 |

Schreiber von DAT_800b8536: Suche nach `sh`/`sw`/`sb` mit Offset 0x8536 in PSX.EXE und allen `BIN/*.BIN` (DEBUG, STAGE1..6, TITLE): nur @0x80027f1c, @0x80027f60,
@0x80027fd8, dazu @0x80027fa0 (`sh v0,2(at)` mit at = 0x800b8534) — alle in FUN_80027e68. Werte: 0xb4 (@0x80027f14 `b4 00 02 34`, @0x80027f9c),
**0xba** (@0x80027f58 `ba 00 02 34`) oder der Aufrufwert (@0x80027fd8). Kleine Berichtigung zum Dossier (dort "= 0xb4"): der Wert ist nicht immer 0xb4,
aber Text und Auswahl haengen am selben DAT_800b8536 — die Auswahl liegt in JEDEM Fall auf Zeile 2 der Box. Die Schlussfolgerung der Spur gilt.

**Zensus selbst gerechnet** (`werkzeuge/fragen_zensus.py`, alle ROOM*.RDT in `shared_assets/PSX/STAGE*`): 90 Fragen (Nachrichten mit 0x03) in 68 RDTs;
Frageseite 61x einzeilig, 29x zweizeilig, deren Zeile 2 endet bei x 34..147 (Cursor ab 160); keine dreizeilig; **26 Fragen stehen auf Seite >= 2, davon 14 mit
einzeiliger Frageseite** — die Form "Erklaerung | Seite | Frage allein" ist Original, z. B. ROOM2060 msg 0 @0x1766 (selbst dekodiert, 3 Seiten):
"The generator does not appear to / be on." | "There are four switches..." | "Will you push them?". Der Ablauf in zwei Schritten entspricht dem Original.

Port-Fragen (`werkzeuge/port_fragen.py`, alle Byte-Felder in `engine/src/gen/*.inc` mit 0x03): 16, alle frei. Id 40 neu: 2 Seiten, Frageseite einzeilig
(Zeilenende x 218, Zeile 1). Id 40 alt (`git show 789f6e681:.../ventilkette_r41_texte.inc`): 1 Seite, 2 Zeilen, Zeile 2 endet bei x 218 -> auf dem Cursor.
Knappste Port-Frage: Id 43 "The pressure gauge reads 80. / Lower the bridge?" Zeile 2 endet bei x 156 (4 Bildpunkte vor dem Cursor), frei.

**Urteil Punkt 4: bestanden.**

## 5. Tests

Aus PowerShell, PATH mit `C:\msys64\mingw64\bin` vorn, Baum `re15_port/build` des Arbeitsbaums:

* `ctest -R "^unit_(r41_nachtrag30|msg_dialog|action_msg_gate|r41_kleinkram_b_takt|r41_ventilkette|r41_keyitems|r39_toene_schrank|r37_codepanel|r26_panel_11f0|r27_panel_schalterwerte)" -j4`:
  **27/27 gruen** (2,2 s). `unit_r41_nachtrag30 -V`: `G Beschriftung: 42 Glyphen, 1334 von 1334 Texeln gleich der Spielschrift, um 1 Bildpunkt versetzt 450 von 1334`,
  `Q Ja/Nein: 11 Port-Fragen, 0 ueberdecken; Id 40 neu frei, alte Id 40 8 Glyphen auf der Auswahl`.
* `ctest -R "^integration_(r41_nachtrag30_(panel|schrank|laden)_(leon|elza)|r41_ventilkette_b|r41_ventilkette_weg|r41_ventilkette_weg_elza|r37_codepanel)$" -j3 --timeout 3000`:
  **10/10 gruen** (4407 s; parallel liefen GUI-Laeufe anderer Agenten, nichts musste wiederholt werden): `_ventilkette_weg_elza` 1688,5 s, `_weg` 1485,9 s
  (beide RUN_SERIAL), `_ventilkette_b` 622,4 s, `_laden_leon` 226,7 s, `_schrank_leon` 327,3 s, `_laden_elza` 246,9 s, `_schrank_elza` 483,0 s,
  `_panel_elza` 174,6 s, `integration_r37_codepanel` 123,6 s, `_panel_leon` 136,5 s. Riegel-Ausgaben: `kasten innen NCC 782 ... VERHAELTNIS 1038 GEGENPROBE 827`,
  `kasten tuer NCC 873 ... VERHAELTNIS 1000 GEGENPROBE 808` (Leon = Elza), Beschriftung F780/F900 1334 von 1334 (Leon, Elza).
* Auswahl der Integration: alle Tests, die Panel, Schrank, Id 40 oder `re15_panel2030_r41`/`schrank2010_r41` beruehren (`grep` in `tests/`:
  test_r41_nachtrag30.cmake, test_r41_ventilkette_b.cmake, test_r41_ventilkette_weg.cmake; unit test_r41_keyitems.c, _nachtrag30.c, _ventilkette.c),
  dazu Codepanel. Nicht gefahren: `_wasser_*`, `_ton`, `_laut` (Teil ton nutzt Frage 43, unveraendert), `integration_r41_keyitems_*` (nur die Units).
* Kein Test misst, was der Nutzer bemaengelt (Lesbarkeit bei Faktor 3); die Riegel pruefen Glyphengleichheit und Seitenverhaeltnis.

## 6. Maengel und Hinweise mit Wiederholungsweg

**M1 — Inhalt des Kastens geschert (gering; vor Spur PF schon vorhanden; nicht Teil des Nutzerbefunds "gestaucht", koennte aber zum Eindruck "komisch" beitragen).**
* Befund: Rueckwand — die waagrechten Strukturen (Rahmen des Sicherungsfelds, Reihe der Sicherungskoepfe, Schilder) stehen gegen die Kastenkanten um
  **+4,5 Grad** schief (Fenster, Projektionsprofil `werkzeuge/waagrecht.py` auf dem entzerrten Port-Bild; 2060-Original entzerrt: 0,0 Grad). Fluegel — die
  Senkrechten (Kanten des weissen und des gelben Schilds) **6,1 Grad** schief (2060: 0,0). ECC-Achsenwinkel 85,1 / 84,6 Grad statt 90. Rechnung (2c): +4,2 / +6,0 Grad,
  vor und nach PF gleich. Bild `kasten_entzerrt_2060_port.png` (Hilfsgitter: im Port-Fluegel sind die Schilder Parallelogramme, in 2060 Rechtecke).
* Ursache: (a) `Q_INNEN` Ecke rechts oben (122,28): Ober- und Unterkante des Vierecks schneiden sich bei (511, 38,5), waagrechte Linien einer senkrechten Wand
  muessen sich aber auf dem Horizont der Kamera Cut 6 treffen (Fluchtpunkt der x-Achse (519,7 / 68,0)). Mit (128,37) liegt der Schnitt bei (489 / 68,6), die
  Scherung faellt auf +0,4 Grad (V 1,021, B/H 0,516 statt 0,460, Anteil 70 % statt 78,6 %). (b) `Q_TUER`: die linke Kante (0,28)-(22,236) ist der Bildrand,
  nicht die freie Kante des Fluegels (die liegt ausserhalb des Bildes); die Homographie haelt sie fuer senkrecht -> 6 Grad.
* Wiederholung: Messlauf `python werkzeuge/fahrt.py s_leon 62 4 "KARTE=2010;-4600;0;-4950;0" RE15_CONTINUE_TEST=1 RE15_CARD_AUTO=1 RE15_CARD_SLOT=0
  RE15_WEGPLAN=<plan_s_leon.txt> RE15_WEGPLAN_LOG=wegplan.txt`, Bild t = 28 s (`runs/s_leon/bilder/b_00113.png`), dann
  `python werkzeuge/inhalt.py <bild> bg/ROOM20606.ppm out` (BG-Dump: `probe_bg_dump.exe <shared_assets/PSX> bg 2`) -> "Achsenwinkel 85.1",
  `python werkzeuge/waagrecht.py out_port.png 5 125 20 320` -> "Waagrechte +4.5"; Fluegel `fluegel.py` / `waagrecht.py out_port.png 5 125 20 340` -> "Senkrechte -6.1";
  Rechnung ohne Spiel: `python werkzeuge/analytisch.py`.
* Abhilfe (Vorschlag, nicht gebaut): in `schrank_bild.py` nicht ueber Handvierecke, sondern ueber die Kamera Cut 6 entzerren (wie `entzerre_2060` /
  `entzerre_2060_fluegel`: Wand z = const, Fluegelebene aus dem Fluchtpunkt der Ober-/Unterkante), dann mittig auf 1300:3600 beschneiden. Riegel-Vorschlag:
  Neigung der Sicherungsroehren / Schildkanten im entzerrten Fensterbild < 1 Grad.
* Warum der Riegel der Spur das nicht sieht: das Werkzeug `kasten` (`_schrank_*` Lauf pf) sucht nur einen Massstab (sx, sy) gegen eine Vorlage, die mit
  derselben Annahme (Viereck, B/H 0,453) entzerrt ist; eine Scherung misst es nicht.

**M2 — Panel 2030 fuer den Nutzer schwer lesbar/verpixelt (Punkt 3, NICHT bestanden; Nutzerurteil 2026-11-02).**
* Befund im Fenster: alle Panel-Inhalte sind ein 320x240-Bild, 3,0-fach nearest vergroessert (Nutzerfenster 960x720 gemessen); Schrift im Stand der Spur
  7 Texel = 21 Punkte hoch mit 3-Punkte-Treppen, Zeiger-Instrument und Drehscheiben mit Einzelheiten unter einem Texel (Tabelle §3a).
* Zusatzbefund: die Nutzer-Instanz laeuft aus dem Hauptbaum mit der exe vom 2026-11-01 08:21 und zeigt den Stand VOR der Spur ("PRESSURE 80", Arial, 0/42
  Spielschrift-Glyphen). master enthaelt 9a96cdde2 nicht.
* Wiederholung: Nutzerfenster nur lesend `python werkzeuge/nutzerfenster.py <PID> bild.png` (Groesse + gdigrab), dann
  `python werkzeuge/etiketten.py bild.png` (Glyphen gegen TEX.TIM, Bloecke einfarbig) und `python werkzeuge/textmass.py bild.png` (Hoehe/Strich/Zwischentoene);
  Stand der Spur: Lauf `p_leon`/`p_elza` wie im Messweg, Bild t = 42 s.
* Ursache und Wege fuer die Nachbesserung: §3a (Ebene 320x240 + Faktor 3 nearest; groessere Schrift im 320x240-Bild oder hoeher aufgeloeste Panel-/Text-Ebene;
  danach Hauptbaum neu bauen).

**H1 — Geisterbild des roten Schilds im Nachrichtenband** (PORT-WAHL, lesbar): siehe §3; Wiederholung `runs/p_leon/bilder/b_00157.png` gegen `b_00169.png`.

**H2 — Dossier-Formulierung "DAT_800b8536 = 0xb4"**: zutreffend fuer den Standardfall; der Wert kann auch 0xba (@0x80027f58) oder der Aufrufwert
(@0x80027fd8) sein. Ohne Folgen fuer den Fix (§4).

## Belege

`analysis/befunde_runde41/PF_belege/abnahme1/`: `kasten_cut3_leon_elza.png`, `kasten_kanten_projektion.png`, `kasten_entzerrt_2060_port.png`,
`panel_leon_fenster3.png`, `panel_elza_fenster3.png`, `panel_leon_fenster1.png`, `panel_leon_text41.png`, `panel_schilder_zoom8.png`, `panel_exit_zoom8.png`,
`tex_tim_schrift_x256.png`, `nutzer_fenster_hauptbaum_vorPF.png`, `panel_texte_nutzer_gegen_pf.png`,
`original_nahansichten_2050cut15_11f0cut10.png`, `frage40_leon.png`, `frage40_elza.png`, `mass_2b.txt`, `mass_2c_analytisch.txt`, `mass_3_textgroessen.txt`, `werkzeuge/` (Treiber, Plaene, Messskripte;
Pfade auf den Arbeitsbaum und den scratchpad zugeschnitten). Alle Fensterbilder: gdigrab, beschleunigter Renderer, eigene PID; `nutzer_fenster_hauptbaum_vorPF.png` = Fenster der Nutzer-Instanz (PID 100752),
nur lesend abgegriffen, keine Eingabe.

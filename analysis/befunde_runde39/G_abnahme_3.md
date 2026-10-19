# Runde 39 Spur G "karten" - Abnahme 3 (unabhaengig, 2026-10-19)

Baum: .claude/worktrees/r39_karten (Zweig r39/karten, Kopf 0f8545c9, Merge-Basis mit master 22abaed9; Ursprung 1038dc15
ist Vorfahr). Gegenstand: Nachbesserung 3 (Commits 664b3d2f..0f8545c9) auf den Mangel N2 der Abnahme 2 (Tueren der
Eingangshalle schweben neben der Wand, solange ROOM1030 nicht gezeichnet ist) samt Nebenbefund B2-Westende - und erneut
beide Nutzer-Punkte gegen den Wortlaut. Der Pruefer glaubt nur, was er selbst am gebauten Stand gemessen hat.
Fortlaufend geschrieben (Sitzungslimit). Arbeitsordner der Messungen (nicht im Baum): `<scratchpad>/G39abn3/`.

## 0. Stand / Bau
- `git log master..HEAD`: 9 Commits der Nachbesserung 3 (664b3d2f, 053b31e4, 5ce60f2e, df42c697, 580928bc, aabbbaf8,
  0b2e75a8, 31e5d0ed, 0f8545c9). Code: 5ce60f2e (karte_r39.c +41, re15_inv_screen.c +2, re15_karte_r39.h +6, Sonde),
  df42c697 (karte_r39.c B2-Korrektur, Sonde), 580928bc (Laufskript/Probe-cmake), 31e5d0ed (nur Header-Kommentar);
  664b3d2f/053b31e4/aabbbaf8/0b2e75a8/0f8545c9 nur Dossier, Bilder, Werkzeugkopien nb3/.
- Bau: `bash re15_port/tools/local_build.sh configure && bash re15_port/tools/local_build.sh build` ->
  `ninja: no work to do.` / `=== LOCAL-BUILD-OK (build)`; exe 2026-10-19 01:44:02 (nach dem letzten Code-Commit 31e5d0ed
  nur Kommentar; ninja bestaetigt: alle Ziele aktuell).

## 1. Mangel N2 (Punkt 1a "Tuer immer genau auf Wand platziert") an der echten exe - eigene Messung
Werkzeuge: die Skripte der Abnahme 2 (`G39abn2/`, Dateizeiten <= 00:30, vor Beginn der Nachbesserung 3 um 00:33, vom Bau
unberuehrt; gelesen) nach `G39abn3/` kopiert: lauf.sh (nur exe-Praefix `re15_pc_g39abn3_<id>.exe`), tuer2.py, struktur.py,
ring.py, reg.py/vgl2.py (Klassen: Grund (0,16,*), Koerper (16,56,*)/(48,8,*), Wand (176,176,176), Tuer (224,168,40),
Treppe (240,240,216)), ascii.py (Klassenbild). Spielstand per `probe_r36_karte karte`, LOAD GAME
(RE15_CONTINUE_TEST/RE15_CARD_AUTO), RE15_WINDOW_SCALE=1, Kartenschirm per RE15_INV_OPEN_AT=40#<raum>, Abzug
RE15_INV_FB_SHOT (Software-Bildspeicher).
### 1a. N2 (a) ECHTER TUERWEG Hof ROOM1090 -> Tuer -> ROOM1050 (ROOM1030 noch nicht betreten)
`lauf.sh <exe> echt_1090_1050 1050 300 1090 -10100 -1800 4200 1024 besucht:10F0:8400:-350 besucht:1090:-13600:1300:5
besucht:1090:-10100:4200:1 -- RE15_INPUT_SCRIPT_BASIS=spiel RE15_INPUT_SCRIPT=W1,U0.3,W0.3,A0.1,W8 RE15_INV_FB_SHOT=karte.bmp
RE15_INV_FB_SHOT_AT=60 RE15_EXIT_AT=400#1050` -> debug.log `[save] CONTINUE: resumed in room 1090`, `[aot] DOOR FIRE slot=0
rect=(-10080,3060,hw=2350,hh=800) target_cut=6 spawn=(19850,0,-22300)`, `[room] PC loaded room1050.rdt`,
`[inv] fb shot (inv frame 60)`; tuer2.py: `Tuerbalken 6, schwebend 0`. Klassenbild (W Wand, R aktuell, D Tuer, . Grund):
```
     x178      x187
y144 .........WWWWWWWWWW
y149 .........WRRRRRRRRR
y150 .........DRRRRRRRRR     <- Balken x187 y150..154 IN der 1050-Westwand (vorher x186 im Grund daneben)
y154 .........DRRRRRRRRR
y155 .........WWWWWWWWWW
```
Gegen den Abzug der Abnahme 2 auf demselben Weg (G39abn2/echt_1090_1050): 10 Pixel verschieden, alle im Kasten x186..187 y150..154 (die Tuer). Bild (x6, links Abnahme 2, rechts NEU): G39abn3/tuer_1050_vorher_neu_x6.png.
### 1b. N2 (b) ROOM1040 allein (Ankunft aus 1060, nur 1040 besucht)
`lauf.sh <exe> al_1040 1040 200 1040 -21008 0 -13134 0 besucht:1040:-21008:-13134:0 -- RE15_INV_FB_SHOT=karte.bmp
RE15_INV_FB_SHOT_AT=60 RE15_EXIT_AT=300#1040` -> `pl pos=(-21008,0,-13134)`; tuer2.py `Tuerbalken 3, schwebend 0`:
```
     x110    x118
y125 WWWWWWWWW........
y126 RRRRRRRRW........
y127 RRRRRRRRD........     <- Balken x118 y127..131 IN der 1040-Ostwand (vorher x119 daneben)
y131 RRRRRRRRD........
y132 RRRRRRRRW........
y134 WWWWWWWWW........
```
Bild (x6, links Abnahme 2, rechts NEU): G39abn3/tuer_1040_vorher_neu_x6.png.

### 1b2. Alle Raeume der Polizeistation ALLEIN besucht (Liste und Weg der Abnahme 2, 46 Lagen, echte exe)
`allein_alle.sh` (= G39abn2/allein_alle.sh, nur eigener Ordner/exe-Praefix): je Spielstand mit NUR diesem Ort besucht
(`besucht:<raum>:<x>:<z>:<band>`), LOAD GAME, Kartenschirm, FB-Abzug; 02:06:58..02:20:52. 42 Abzuege (10F0, 1170, 11C0,
1240 oeffnen die Karte wegen ihrer Eintritts-Szene nicht - wie in der Abnahme 2). Auswertung tuer_alle.py (struktur.py +
tuer2.py + Pixelvergleich gegen G39abn2/al_*):
- **Doppelwand: 0 in allen 42 Abzuegen.**
- **tuer2.py "schwebend": 0 in 40 Abzuegen**; die zwei Treffer sind die Faelle, die die Abnahme 2 als "kein Schweben"
  gewertet hat, pixelgleich zu ihr: al_1090b (Balken x197..201 y114 auf der Suedwand-Linie y114, die erst bei x197 beginnt
  - der Balken belegt das Eckpixel; Klassenbild: W x197 y106..113, `DDDDDWWWWWWW` y114) und al_1130 (Rest (153,128) eines
  vom Spieler-Ring ueberdeckten Balkens). Die strengere struktur.py-Pruefung meldet zusaetzlich 7 Balken mit einem Ende
  "sonst" (Klasse 7 = Ring-Pixel; 10C0, 1130 (153,124..126), 1150, 1180, 11E0, 11F0, 1230) - jeweils vom Ring teilverdeckt,
  das andere Ende Wand (11F0: beide Enden Ring), pixelgleich zur Abnahme 2.
- **Gegen die Abzuege der Abnahme 2: 0 Pixel verschieden in 39 Abzuegen**; verschieden nur al_1040 (10 px, x118..119
  y127..131 = die Tuer, Abschnitt 1b) und al_1210 (12 px, x186..187 y71..77: Tuer jetzt auf der wiederhergestellten
  Westwand x187 y72..76, Wand (187,71)/(187,77); Zustand im Spiel unerreichbar). al_1050 (Lage (14700,0,6100)) hat in der
  Abnahme 2 keinen Abzug (dort erst der zweite Anlauf an anderer Lage): Balken x187 y150..154 in der Wand, x186 Grund.
- al_1030 allein: beide Hallen-Tueren auf den 1030-Waenden (x186 y150..154, x119 y127..131) - Traeger gezeichnet, keine
  Verschiebung.
- Sicht am ECHTEN Fenster (gdigrab, Direct3D-Renderer, kein SOFTWARE_RENDER; gdi3.sh = G39abn2/gdi2.sh mit eigenem
  Praefix): 1050 allein (14700,0,6100): x187 y150..154 = (224,168,40) Tuer, (187,148)/(187,149)/(187,155) = (176,176,176)
  Wand, x186 Grund; gdigrab gegen FB-Abzug 13 Pixel, alle im Ring-Kasten x200..204 y132..136 (Blinkphase). Bild
  G39abn3/gdi_1050b/gdi_tuer_x6.png. (Ein erster gdigrab auf dem Tuerweg zeigte Blatt 2F: das Eingabeskript lief nach dem
  Raumwechsel noch einmal an - `[input-script] Tick 30 -> F120 Tasten 0x0010` zweimal im debug.log - und blaetterte mit
  HOCH weiter; Messhaken, kein Befund.)

### 1c. Nebenbefund B2-Westende ROOM1210/1211 ohne ROOM11E0 - ECHTER WEG Elza liest Roys Tagebuch
Nicht der Ersatzweg des Baus (Spielstand ROOM1020 + flag:9:68 + RUNTER), sondern der Weg von integration_r39_elza_karte:
Elza vor dem liegenden Roy (`probe_r36_elza_karte re15_card.mcr 1011 -1900 6830 3072 4:0xF1 4:0xFA 3:0x74`), LOAD GAME,
`RE15_PRESS=square@60,right@200..350 (x6),square@380,square@480`, Bildserie `RE15_FRAMEDUMP=480-1100/2:fd_`
(RE15_WINDOW_SCALE=1; eigenes Skript elza_tagebuch.sh). debug.log: `[pl] Spieler-Familie PL04`, `[tagebuch] Leser zu: Bit
(9,68)=1, Kartenkette sofort (Runde 39 Spur H): Eintrag 3 (ROOM10F1 Blatt 3 Rechteck 9) -> Folge 4 (ROOM1211 Blatt 1
Rechteck 3)`, `[hint] F620 Folge-Hinweis 3 -> 4 (Zeit)`, Phasen `F621 rot`, `F641 umriss`, `F660 rot`, `F679 umriss`,
`F699 rot`, `F718 umriss`, `F737 schliessen`; `[inv] Blatt 1: 6 RE1.5-Tuerschwenke entfernt (79 Pixel)`.
Pixel (RGB hex) x183..192 der Bilder F630 (rot) und F650 (Umriss):
```
F630 rot     y70 001058 001058 001078 001058 b0b0b0 b0b0b0 b0b0b0 ...
             y71..y77 je ... 001058 b0b0b0 300830 300830 ...      <- x187 Wand (176,176,176), rechts Koerper rot
             y78 001058 001058 001078 001058 b0b0b0 b0b0b0 b0b0b0 ...
F650 Umriss  y71..y77 je ... 001058 b0b0b0 001058 001058 ...      <- x187 Wand, gleiche Farbe wie y70/y78
```
offen_ppm.py (Koerperpixel 4-benachbart zu Grund, Kasten x180..256 y64..156) in F630/F650/F670/F690/F710/F730: **je 0**.
GEGENPROBE (vorbestehend?): dieselbe Eingabe mit der exe der Merge-Basis 22abaed9 (G39abn0/vorher_build, vor Spur G;
RE15_ASSET_ROOT auf den Spurbaum): gleiche Hinweis-Folge (F622 Folge 3 -> 4), x187 y71..77 = Grund (001058), rote Phase
F630/F670 **7 Koerperpixel am Grund, erstes (188,71)**, Umriss-Phasen 0. => die offene Westseite der blinkenden
Zielkachel bestand schon vor der Runde 39 (Schwenk-Tilgung des PC-Renderers) und ist jetzt geschlossen, in beiden
Blinkphasen in der Wandfarbe der Kachel. Bilder (x6, links Merge-Basis-exe, rechts NEU): G39abn3/b2_ziel_rot_vorher_neu_x6.png, G39abn3/b2_ziel_umriss_vorher_neu_x6.png.

## 2. Punkt 1 im Vollzustand und Struktur (Sweep der echten exe)
`lauf.sh <exe> neu_S 1190 300 1190 3174 0 -11182 0 -- RE15_MAP_SHOT_SWEEP=s` (alle 13 Blaetter aufgedeckt):
- gegen den Sweep der Abnahme 2 (G39abn2/neu_S, vgl_sweep.py): **s00..s12 je 0 Pixel verschieden**.
- gegen die rechten Vorlagen-Haelften (vgl2.py, Kartenklassen): B2 249 (die Kammer ROOM1210, Orchestrator-Entscheid,
  wie Abnahme 2), 1F **0**, 2F **0**, 3F 54 (die zwei begruendeten Treppenkaesten H3 der Abnahme 1) - identisch zur Abnahme 2.
- struktur.py: Doppelwand-Pixel B1/B2/1F/2F/3F = 0/0/0/0/0; Tueren nicht auf Wand 1/0/0/0/0 (B1: der Spieler-Ring (127,116)
  liegt ueber dem Tuerbalken (126..128,115), Klasse "sonst" an Enden und Seiten - kein Schweben, wie Abnahme 2); Treppen-
  pixel an Wand 0/6/12/6/1 (Sprossenenden wie die Vorlagen).

## 3. Punkt 2 - ROOM1090 Leons Marker im aktuellen Raum (eigene Messung)
- Nutzerpunkt (R1090 (-12528,-9000,959), nutzer/befund_marken_2026-10-17.txt): `lauf.sh <exe> neu_M 1090 300 1090 -12528
  -9000 959 0 -- RE15_MAP_SHOT_SWEEP=s` -> `pl pos=(-12528,-9000,959)`, Blatt 2F Ring **(183,72)**, 5x5 ganz im Rot des
  aktuellen Raums (Klassenbild: Wand x179 / y67 / y76, Ring x181..185 y70..74 auf R); gegen G39abn2/neu_M **13 Blaetter je
  0 Pixel verschieden**.
- ECHTER WEG Leon (Lauf A von test_r36_klettern.cmake: RE15_DEBUG_JUMP=10f0@250, RE15_SET_FLAG=9:71,
  RE15_PLAYER_POS=-7125,4950,0, Tritt schieben, klettern, Tuer von oben; RE15_INV_OPEN_AT=40#1090; lauf_env.sh mit eigener
  exe-Kopie): `[room] PC loaded room1090.rdt`, climb.txt `T1227 room=1090 pos=(-13600,-9000,1300) band=5`, Ring **(182,72)**
  ganz im Rot; gegen G39abn2/echt_1090_leon **0 Pixel verschieden**.
- ECHTER WEG Elza (Lauf B: RE15_TITLE_SHOT_CHAR=1, RE15_GOTO_ROOM=10F1, Weg durch den Raum, Tritt, Tuer;
  RE15_INV_OPEN_AT=20#1091): `[pl] Spieler-Familie PL04`, `PC loaded room1091.rdt`, `T1243 room=1091 pos=(-13600,-9000,1300)
  band=5`, Ring **(182,72)** ganz im Rot; gegen G39abn2/echt_1091_elza 13 Pixel, alle im Ring-Kasten x180..184 y70..74
  (Blinkphase).

## 4. Nebenwirkungen
- Tuer-Uebergaenge: `RE15_UEBERGANG_LISTE=1 tests/integration/test_map_uebergang.exe` (Bau des Baums) -> `[Sprung] 151
  Uebergaenge auf gleichem Blatt: Median 6 px, Mittel 17 px, schlimmster 91 px, ueber 8 px: 62`; je Tuerpaar gegen die Liste
  der Abnahme 2 (G39abn2/uebergang_neu2.txt, ueb_vgl2.py): **132 Paare, 132 gleich, 0 verschieden**. Erklaert: der zweite
  Haken (tuer_anziehen) wirkt nur auf Schema-Zonen (`if (!zn || !zn->synth) return;` re15_inv_screen.c:663), keine der drei
  verschobenen Tueren gehoert zu einer.
- 1F-Plan (flag 9:89) + 1050 aktuell, 1030 unbesucht (Umriss): `lauf.sh <exe> plan1F_1050 1050 200 1050 20600 0 12350 0
  besucht:1050:20600:12350:0 flag:9:89 -- ...` -> tuer2.py `Tuerbalken 6, schwebend 0`; die Tuer 1030<->1050 steht auf der
  1030-Umrisslinie x186 y150..154 (Traeger gezeichnet), x187 Koerper - die EINE Wand wie map1F_correct.png; 1030<->1040
  unsichtbar (weder 1030 noch 1040 besucht), x118 durchsichtig, x119 Wand (keine Doppelwand).

## 5. RE-Gate (selbst disassembliert / gelesen, eigene Werkzeuge G39abn3/tools/)
| Behauptung (Diff/Dossier) | Werkzeug | Befund |
|---|---|---|
| RE2 FUN_8006e120: Plan `lbu a1,-21955(at)` @0x8006e660 + `jal 0x80077360` (a0 0x800d4924) @0x8006e66c; besucht `lbu a1,12(s1)` @0x8006e67c/@0x8006e730 + `jal 0x80077360` (a0 = 0x800d4924-24) @0x8006e688/@0x8006e73c; `beq v0,zero,0x8006e768` @0x8006e744; `addiu s5,zero,498` @0x8006e71c; `jal 0x8008f828` @0x8006e750; `jal 0x8008f918` @0x8006e760 | `re2_disasm.py dis 0x8006e400 132` / `dis 0x8006e640 80` (info/re2leon/PSX.EXE) | **stimmt** Instruktion fuer Instruktion; Schleife ueber die Raum-Eintraege rueckwaerts (s1 = Tabelle + 16*n - 16, `addiu s1,s1,-16` @0x8006e46c; x/y = `lhu 4(s1)`/`lhu 6(s1)`), je Raum EIN AddPrim. 0x8008f828 = GetClut (`sll v0,a1,6; sra a0,a0,4; andi 0x3f; or`), 0x8008f918 = AddPrim (Listen-Einhaengen), 0x80077360 = Bittest (`srl v1,a1,5 ... srlv v0,0x80000000,a1`) - selbst disassembliert |
| RE2 FUN_8006F1C4: `bne s4,a2,0x8006f518` @0x8006f4e8, `lbu v0,0x5c19(v0)` @0x8006f4f4, `j 0x8006f604` + `addiu s2,s2,1` @0x8006f510/14, `addiu s2,zero,498` @0x8006f5dc, `jal 0x8008f828` @0x8006f608 | `re2_disasm.py dis 0x8006f4c0 84` | **stimmt**; Zielraum -> Sprite immer (rote Phase CLUT+1 bzw. Umriss 498/503), ohne Besucht-Test |
| RE2 Area 2 (RPD 1F) @0x800aaa48: RoomTable 0x800aa388, 18 Raeume, MAPS_005.TIM; Raum 1 @0x800aa398 Ostwand x126 mit Tuerhaelfte Index 3 x125..126 y147..154; Raum 0 @0x800aa388 Westwand x127 mit Index 3 x127..128 y147..154 | eigenes re2_karte.py (Area-Tabelle 8 B je Eintrag wie `lbu -21956/-21955(at)` @0x8006e45c/@0x8006e660; TIM selbst geparst: CLUT-Block, Bild-Block @0x614, Pixel ab 0x620, 4 bpp) | **stimmt**: Raum 1 uv(24,80) 32x25 Schirm (99,139), Raum 0 uv(0,112) 92x90 Schirm (127,92); Ausschnitt y144..157: Raum 1 `111111114....` / `111111133....` (y147..154), Raum 0 `411111111` / `331111111` - je Raum die eigene Tuerhaelfte auf der eigenen Wand, das andere Sprite dort Index 0. Ganze Area 2 (eigenes re2_tuerpaare.py: Index-3-Gruppen je Sprite, Partner = angrenzende Gruppe eines anderen Raums): 39 Gruppen, **19 zweiseitige Paare** (0/1, 0/2, 0/10, 1/4, 2/9, 3/4, 4/5, 5/6, 5/7, 7/8, 7/9, 10/11, 10/12, 11/15, 11/17, 12/13, 12/14 zweimal, 15/16), einseitig nur Raum 0 x151..158 y171..173 (Aussentuer) - wie behauptet |
| RE1.5 Seiten-Tabelle @0x80076840 (count/Zeiger je Seite), 1F-Liste @0x8007636C: r0 (180,69,32,96) uv(0,32), r2 @0x80076384 (102,77,24,80) uv(88,48), r4 @0x8007639C (119,125,72,64) uv(184,56); B2-Liste @0x800762F4: r0 (100,70,88,80) uv(168,16), r3 @0x80076318 (187,70,64,80) uv(96,48), r4 @0x80076324, r5 @0x80076330 | `re15_disasm.py bytes 0x80076840 48` / `bytes 0x8007636C 72` / `bytes 0x800762F4 72` (info/Re1.5/PSX.EXE) | **stimmt** (Seiten 0..5: 7/10/11/10/7/2 Eintraege; alle genannten Eintraege Byte fuer Byte) |
| Kacheln: MAP03.PIX r0 Westwand x187 y144..155 (Stich) mit Schwenk, r4 Ostwand x186 ab y144; r2 Ostwand x118 y125..134 mit Schwenk, r4 Westwand x119 y125..134; MAP02.PIX r3 Westwand x187 y70..78 (Spalte 0 Zeilen 0..8 Index 4) mit Schwenk x187..192 y72..76, B2 r0 Ostwand x186 ab y70 | eigenes pix15.py (4 bpp, 128 B je Zeile, u/v aus der Seiten-Tabelle) | **stimmt** - die Doppelwaende x186/x187 und x118/x119 sind gemalt; s_korr r0 Spalte 7 Zeilen 76..85 = x187 y145..154, r2 Spalte 16 Zeilen 49..56 = x118 y126..133, B2 r3 Spalte 0 Zeilen 1..7 = x187 y71..77 nachgerechnet |
| RE1.5 malt immer alle Rechtecke (FUN_80046fd8, Schleife @0x800472fc..0x800473e0) | `re15_disasm.py dis 0x800472d0 72` | **stimmt**: Zaehler a2 bis a3 (`sltu v0,v0,a3` / `bne v0,zero,0x800472fc` @0x800473d8/dc), Schritt 12 B, keine Bedingung je Rechteck |
| Door_aot_set nach ROOM1210 in ROOM11E0.RDT @0x015A2/@0x016C4 und ROOM1220.RDT @0x00DD2..@0x00E52; nach 1220 in ROOM1210.RDT @0x01CE6..@0x01D66 | eigenes Byte-Lesen (Nutzlast pc[22]/pc[23] wie scd_vm.c op_door_aot_set) | **stimmt**: je `3b`, Ziel Raum 0x21 bzw. 0x22 (ROOM1210 @0x01CC6 Ziel 0x1E); die Vollstaendigkeit ("nur") nicht nachgezaehlt - betrifft nur den unerreichbaren Zustand "1210 ohne 11E0" |
| Schwenk-Tilgung inv_render_pc.c `WAND = 4, S_SAAT = 4, S_WACHS = 10, MAX_AUSD = 10` | grep inv_render_pc.c:550 | **stimmt**; an der echten exe gemessen: vor Spur G x187 y71..77 Grund (Abschnitt 1c) |

- Suche im Code-Diff der Nachbesserung 3 (`git diff 4ea5e62c HEAD -- re15_port | grep '^+'`) nach deferred/tunable/interim/
  for now/faithful/plausib/TODO/FIXME/geschaetzt/approx/getenv: **kein Treffer**. Ganzer Zweig: nur "GEMESSEN statt
  geschaetzt" (karte_r39.c:17, Abnahme 2). Kein neuer getenv, kein Env-Schalter.
- Konstanten der Nachbesserung 3: s_korr `{ 1, 3, 0, 2, 1, 5, 1, 0 }` (Traeger r0 statt R39_IMMER; Lage = MAP02.PIX r3 Spalte 0,
  Traeger B2 r0 @0x800762F4 Ostwand x186) und `{ 1, 3, 0, 1, 1, 7, 4, R39_OHNE(0) }` (x187 y71..77 = die gemalte Linie Index 4
  Zeilen 1..7, die die Schwenk-Tilgung nimmt - an der exe gemessen); `R39_OHNE(t) = 64 + t` = Kodierung des Traeger-Feldes
  (PORT-WAHL der Tabellenform, keine Verhaltenszahl); re15_karte_r39_tuer_lage ohne neue Zahl (Balkenlaenge 5 / +-2 =
  der Tuerzeichner re15_inv_screen.c `q->w = laengs_x ? 5 : 1`, Versatz +-1 = eine Spalte). **Gate haelt.**
- Erklaert der Fix den Befund? **Ja.** N2: die Marke liegt auf der Traeger-Linie (x186/x119), der Balken wird jetzt
  um genau die Spalte der Korrektur verschoben, wenn deren Traeger nicht gezeichnet ist (= die Linie, die dann steht) -
  an der exe Balken x187 bzw. x118 statt x186/x119 (Abschnitt 1a/1b), und im Plan-/Vollzustand unveraendert (Abschnitt 2/4).
  Nebenbefund: die Wand-Fuellung R39_OHNE(0) ersetzt genau die von der Tilgung genommenen 7 Pixel (Abschnitt 1c, vorher
  `Tuerschwenke entfernt (79 Pixel)` gleich - die Tilgung laeuft weiter, die Fuellung liegt darueber).

## 6. Tests (gezielt, KEINE volle Suite im Spurbaum)
- Regex = die 100 Tests des Baus (alle Karten-/Hinweis-/Inventar-Gruppen, die der Diff beruehrt: karte_r39.c,
  map_hint_common.c, re15_inv_screen.c, re15_map_zones.h, re15_karte_r39.h + unit_1210_; mit `ctest -N` gegen alle 1023
  Namen mit karte/map/inv/hint/hinweis/tagebuch/1210/zone/synth/tuer/marke abgeglichen - nicht enthalten sind nur Spiel-Tuer-,
  PRI-Marken-, Biss-/Griff- und Zonen-Dispatch-Riegel ohne Kartenbezug): `ctest --test-dir re15_port/build -R "<Regex des
  Baus>" -j 4 --timeout 600 --output-on-failure`, 01:53:44..02:05 parallel zu meinen exe-Laeufen ->
  **100% tests passed, 0 tests failed out of 100** (671,8 s). Log: G39abn3/ctest_ziel.log.
- Riegel selbst gelesen (`probe_r39_karten pruef tueren`, Ausgabe G39abn3/pruef_tueren.txt): `PASS: Blaetter B1..3F: 798
  Zustaende ..., 3498 gezeichnete Tuerbalken - schwebend oder neben der Wand: 0 (vorher 3)`; 1050 allein x187 5/5, Enden 2/2,
  x186 frei; 1040 allein x118 5/5; 1050+1030 / 1040+1030 auf der 1030-Linie, eigene Linie Koerper 5/5; alle 5
  Hinweis-Zielkacheln allein 0 Koerperpixel am Grund, `x187 y70..78: WWWWWWWWW`, `B2 r3 Westwand x187 y71..77 7/7`.
  `pruef teil` (ctest -V): `1210 allein (Nachbesserung 3): Tuerbalken 11E0<->1210 auf der EIGENEN Westwand x187 y72..76
  (5 von 5), Wand x187 y71/y77 2/2, auf x186 nichts (0)`.
- Messen die Riegel? Gegenprobe ohne Code-Eingriff - die Pruefbefehle der neuen Laeufe NA/NB/NZ auf VORHER- und
  NEU-Abzuege angewandt:
  | Pruefung | VORHER | NEU |
  |---|---|---|
  | `probe_r39_karten tuerbild <bmp> 180 140 195 160` (NA, Tuerweg 1090 -> 1050) | G39abn2/echt_1090_1050: `Balken x186..186 y150..154 SCHWEBT (Wand quer 5)` FAIL | G39abn3/echt_1090_1050: `Balken x187..187 y150..154 ok` OK |
  | `tuerbild <bmp> 112 122 126 137` (NB, 1040 allein) | G39abn2/al_1040: `Balken x119..119 y127..131 SCHWEBT` FAIL | G39abn3/al_1040: `Balken x118..118 y127..131 ok` OK |
  | `wand <bmp> 187 71 77` (NZ, B2-Ziel nach dem Tagebuch) | exe der Merge-Basis, echter Tagebuch-Weg F630/F650: `WAND 0 von 7` | NEU F630/F650: `WAND 7 von 7` |
  Die Riegel unterscheiden VORHER und NEU. (Die Gegenprobe des Baus mit voruebergehend zurueckgesetzten Engine-Dateien habe
  ich nicht wiederholt.)
- Neue Tests parallel-/Linux-fest: unit_r39_karten_tueren ueber probes/r39_karten.cmake (GLOB-Einbindung, TIMEOUT 600,
  7,2 s); NA/NB/NZ laufen in integration_r39_karten mit dessen exe-Kopie `re15_pc_r39g_karten_<id><LAST_EXT>` und eigenem
  Laufordner `${_basis_wd}_<name>` (`cmake_minimum_required(VERSION 3.16)` im Skript, keine .exe-/Backslash-Pfade,
  `list(SUBLIST)`), Umgebung nur ueber re15_start_spiel. Keine Test-CMakeLists geaendert.
- Spur-G-Riegel nachgefahren mit Ausgabe (`ctest -R "^(integration_r39_karten|unit_r39_karten_teil|unit_r39_karten_tueren)$"
  -V`, G39abn3/ctest_r39_v.log): `100% tests passed, 0 tests failed out of 3`; darin `r39_karten[NA] ROOM1050 Tueren
  (180;140;195;160): Balken x187..187 y150..154 ok (Wand quer 0)`, `r39_karten[NB] ... Balken x118..118 y127..131 ok`,
  `r39_karten[NZ] B2 nach Roys Tagebuch, Westwand ROOM1211: WAND 7 von 7 (x187 y71..77)`, `[K] MARKER 191 117`, `[M] MARKER
  183 72 ... rot=36`. Der K-Abzug dieses Laufs gegen G39abn2/w1210: 0 Pixel verschieden.
- B1 (Punkt 1c, von der Nachbesserung 3 nicht beruehrt - keine s_korr-Zeile auf Blatt 0): `probe_r39_karten pruef b1` 35x
  PASS, 0 FAIL (z.B. `1190 Halle: 0 von 3770 begehbaren Rasterpunkten neben der Zeichnung`, `11C0 Ring: 0 von 5706`); s00 und
  alle B1-Einzelzustaende pixelgleich zur Abnahme 2.

## 7. Vertrag / Pfade
- Keine Bank-9-Bits, keine Nachrichten-Ids, keine Slots/Ereignisse (Engine-Diff der Nachbesserung 3 gelesen: nur
  s_korr-Zeilen, R39_OHNE, traeger_gezeichnet-Zweig, re15_karte_r39_tuer_lage, zwei Aufrufe). Die Sonde setzt nur die
  vorhandenen Plan-Bits 78/77/89/88/87 als Testzustand.
- Pfade (`git diff master...HEAD --name-only`, ohne analysis/): engine/src/karte_r39.c, map_hint_common.c, re15_inv_screen.c,
  re15_map_zones.h, include/re15_karte_r39.h, Tests unter tests/unit + tests/integration (Laufskripte), probes/r39_karten.cmake.
  **KEINE** release/, platform/android/, shared_assets/PSX/, CMakeLists.txt, RELEASE_NOTES.
- Gemeinsame Datei re15_inv_screen.c: Nachbesserung 3 genau ZWEI Zeilen (Haken re15_karte_r39_tuer_lage im Tuer-Zeichner
  und in tuer_anziehen, je Kommentar "Runde 39 Spur G"); karte_r39.c/re15_karte_r39.h = Karten-Code der Spur.
- Gegen den aktuellen master (2963579d): `git merge-tree --write-tree master HEAD` exit 0, ohne Konflikt; master hat seit der
  Merge-Basis keine Datei der Spur geaendert.

## 8. Urteile
| Punkt | Urteil | Kurzbeleg |
|---|---|---|
| Mangel N2 der Abnahme 2 (Tueren der Eingangshalle ohne gezeichnete ROOM1030) | **behoben** | echter Tuerweg Hof 1090 -> 1050: Balken x187 y150..154 IN der 1050-Westwand (vorher x186 daneben, 10 Pixel Unterschied genau im Kasten x186..187 y150..154); 1040 allein x118 y127..131 IN der Ostwand; gdigrab des echten Fensters = FB-Abzug; Plan- und Vollzustand unveraendert auf der einen 1030-Linie |
| Nebenbefund B2-Zielkachel ROOM1211 nach Roys Tagebuch (Elza) | **behoben** | echter Tagebuch-Weg: Westwand x187 y71..77 in beiden Blinkphasen Wand (176,176,176), 0 offene Pixel; Merge-Basis-exe am selben Weg: 0/7 Wand, 7 Koerperpixel am Grund (vorbestehend seit vor Runde 39) |
| 1a Blaetter 1F/2F/3F/B2 nach den Vorlagen - "Keine Doppelwaende, Tuer immer genau auf Wand platziert, treppen nicht in der Wand" | **erfuellt** | Vollzustand pixelgleich zur Abnahme 2 (1F/2F 0 Klassen-Abweichungen, B2 nur die Kammer = Orchestrator-Entscheid HK, 3F die zwei begruendeten Treppen H3); Doppelwand 0 auf B1..3F und in allen 42 Einzelzustaenden; schwebende Tueren 0 (die zwei Treffer 1090-Eckpixel/1130-Ringrest wie Abnahme 2 "kein Schweben"); Sonde 798 Zustaende 0 Befunde; Treppen nur mit Sprossenenden an Waenden wie die Vorlagen |
| 1b "natuerlich muss der Spielermarker da entsprechend passen" | **erfuellt** | Marker-Lagen unveraendert (13 Blaetter, 39 Einzelzustaende, Lauf K pixelgleich; Uebergaenge 132/132 gleich); Riegel marker gruen |
| 1c B1 neu | **erfuellt** | unveraendert zur Abnahme 2 (dort erfuellt): s00 0 Pixel, B1-Einzelzustaende 0 Pixel, pruef b1 35/35 |
| **1 gesamt** | **erfuellt** | 1a/1b/1c erfuellt |
| **2** ROOM1090 Leons Marker im aktuellen Raum | **erfuellt** | Nutzerpunkt Ring (183,72) ganz im Rot; echter Weg Leon 10F0 -> Klettern -> Tuer Ring (182,72) im Rot (0 Pixel zu Abnahme 2); echter Weg Elza (PL04) 10F1 -> 1091 Ring (182,72) im Rot |
| RE-Gate | **haelt** | 7 Behauptungen selbst disassembliert/gelesen (RE2 FUN_8006e120, FUN_8006F1C4, Area-/Raumtabelle + MAPS_005.TIM mit 19 Tuerpaaren; RE1.5 Seiten-Tabelle, Kacheln, Rechteck-Schleife, Door_aot_set-Offsets); keine Rate-Woerter, kein getenv |
| Vertrag / Pfade / Tests | **haelt** | keine Bits/Ids/Slots; keine verbotenen Pfade; re15_inv_screen.c +2 Haken-Zeilen; 100/100 gezielt + 3/3 mit Ausgabe; Riegel messen (VORHER rot / NEU gruen an echten Abzuegen); merge-tree ohne Konflikt |

## 9. Maengel
Keine.

## 10. Hinweise (kein Mangel)
- **HN3-1 (dem Nutzer melden, vom Bau offengelegt):** Sobald die Eingangshalle ROOM1030 gezeichnet wird (Betreten oder
  1F-Plan), rueckt die Tuer 1030<->1050 von x187 auf x186 bzw. 1030<->1040 von x118 auf x119 - die Tuer folgt der dann
  einen Wand (map1F_correct.png). Gemessen: 1050 allein x187, 1050 + 1F-Plan x186 (Abschnitt 4), 1030 allein x186/x119.
- **HN3-2 (Messhaken, nicht Spur G):** Auf dem Tuerweg mit RE15_INPUT_SCRIPT laeuft das Skript nach dem Raumwechsel
  erneut an (`[input-script] Tick 30 -> F120 Tasten 0x0010` zweimal) und blaettert die offene Karte mit HOCH weiter -
  Abzuege deshalb frueh (RE15_INV_FB_SHOT_AT=60) nehmen.
- HK (B2-Kammer ROOM1210 gegen mapB2_correct.png, Orchestrator-Entscheid Beta -> Retail), H2 (Klemmung reserve 4),
  H3 (3F-Treppen 1060/1170), H4 (1130-Wandnasen allein), H5 (1170 Zone 1/10F0/11C0/1240 allein wegen Eintritts-Szenen
  nicht per CONTINUE abziehbar), H6 (11A0 Band 1 Blatt 6), H7 (Querbalken 1210) und die Kerben (187,71)/(187,77) am
  Tuerbalken 11E0<->1210 im Vollzustand (Vorlage gleich): unveraendert, wie in den Abnahmen 1/2.
- Aufraeumen (kein Code): im Build-Ordner liegt noch die exe-Kopie `re15_port/build/platform/pc/re15_pc_r39g_karten_5d94347b.exe`
  (2026-10-18 22:47, Laufordner lauf_5d94347b_S/M/K = ein abgebrochener integration_r39_karten-Lauf der Nachbesserung 2);
  meine Laeufe haben ihre Kopien entfernt.
- Nicht nachgeprueft: die Vollstaendigkeit "Door_aot_set nach 1210 NUR in 11E0/1220" (betrifft nur den unerreichbaren
  Zustand "1210 ohne 11E0"; die genannten Offsets stimmen); die Gegenprobe des Baus mit zurueckgesetzten Engine-Dateien
  (ersetzt durch die Gegenprobe an echten VORHER-Abzuegen, Abschnitt 6).

## 11. Ergebnis
**bestanden = true.** Der Mangel N2 der Abnahme 2 ist an der echten exe auf dem echten Tuerweg (Hof 1090 -> 1050) und im
Einzelzustand 1040 behoben; die Tuer steht in jedem der 42 abziehbaren Einzelzustaende auf einer gezeichneten Wand, der
Plan- und der Vollzustand sind unveraendert (13 Blaetter pixelgleich zur Abnahme 2), die Tueruebergaenge gleich (132/132).
Der Nebenbefund (offene Westseite der Zielkachel ROOM1211 nach Roys Tagebuch) ist am echten Elza-Weg geschlossen und war
vor der Runde 39 schon offen. Punkt 2 und die Teile 1b/1c bleiben erfuellt. RE-Gate (Stichproben selbst disassembliert),
Vertrag, Pfade, gezielte Tests (100/100, Riegel messen) und merge-tree halten.

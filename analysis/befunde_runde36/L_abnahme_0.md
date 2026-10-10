# Runde 36 — Spur L "elza": UNABHAENGIGE ABNAHME 0

Stand: Zweig `r36/elza` @059cc672 (Basis master 836023f5), Baum `.claude/worktrees/r36_elza`.
Pruefer: Abnahme-Agent 0 (2026-10-11). Gemessen wird am GEBAUTEN Stand gegen den WORTLAUT (AUFTRAG.md).
Der Bericht wird fortlaufend geschrieben (Sitzungslimit).

## 0. Fortschritt

- [x] AUFTRAG/VERTRAG/Dossier gelesen, Code-Diff gelesen (21 Dateien, +2185/-4)
- [x] Bau (local_build.sh configure + build: `ninja: no work to do`, exe 2026-10-10 23:25:53 = Quellen 23:21-23:25, Baum sauber)
- [x] Gezielte Unit-Tests: 79/79 gruen
- [x] Gezielte Integrationstests: 12/12 gruen
- [x] P1 Fenster 1121 + Kraehe (erfuellt)
- [x] P2 Irons 1151 tot + Text (erfuellt)
- [x] P3 Unlock 1211 (erfuellt)
- [x] P4 Schuesse 1021 (erfuellt)
- [x] P5 Roy-Sterbe-Ton 1021 (erfuellt; Beobachtung B1)
- [x] P6 main16.5 + Untertitel (erfuellt; Beobachtung B2)
- [x] RE-Gate (Adressen selbst disassembliert / Bytes selbst gelesen, s. §2)
- [x] Vertrag / Pfade / Nebenwirkungen (§4)
- **Ergebnis: bestanden** (§7)

## 1. Bau und gezielte Tests

* `bash re15_port/tools/local_build.sh configure` -> `LOCAL-BUILD-OK (configure)`; `... build` -> `ninja: no work to do`,
  `LOCAL-BUILD-OK (build)`. `git status` sauber (nur dieser Bericht). Die exe (23:25:53) ist juenger als jede
  Engine-/Plattform-Quelle des Zweigs (elza_r36.c 23:23, fenster_1120.c 23:24, scd_vm.c 23:24, main.c 23:24,
  audio_pc.c 23:25, elza_se_pc.c 23:25); spaetere Commits aendern nur Tests/Dossier. Suite-Zaehlung `ctest -N`: 605 (= 592 + 13).
* Unit, gezielt (`ctest -R "unit_r36_elza|unit_r35_fenster|unit_r35_cut1150|unit_irons|unit_neck|unit_crow|unit_re2_crow|
  probe_crow|unit_synchro|unit_voice|unit_se_voice|unit_msg_dialog|unit_text_freeze|unit_elza|unit_r30_elza|unit_r30_irons|
  unit_r30_tuer|unit_r33_tuer1120|unit_r34n_d_adaruf_elza|unit_r34n_f_leiche|unit_r35_inventar1050_elza|unit_aot|unit_scd_aot|
  unit_audio|unit_r30_hund_tod_elza" -j 4`): **`100% tests passed, 0 tests failed out of 79`**.
* Integration, gezielt, nacheinander (`ctest -R "integration_r36_elza|integration_r35_fenster|integration_elza_vollstart|
  integration_r35_werfer_elza|integration_r30_irons_tisch|integration_r35_cut1150" --timeout 1200`):
  **`100% tests passed, 0 tests failed out of 12`** (1059,6 s; r35_cut1150 512 s). Die fuenf Haken der Spur melden:
  `r36_elza_fenster: OK — 4 Kraehen (Slot 3 bei (5327,0,10217)), Schaden 309/309 Pixel nach Formel`,
  `r36_elza_irons: OK — Irons 151/151 Bilder tot (Clip 2 Bild 89), Kopf fest 60/60, Anklicken -> Text 20`,
  `r36_elza_unlock: OK — Unlock-Aktion Bild 924, Spitze 9363 (Median 1900)`,
  `r36_elza_schuss: OK — Schuesse Bild 699/729 (Spitzen 7039/6773), Roy ab Bild 729 (Spitze 8073), Median 1042`,
  `r36_elza_stoehnen: OK — Aktion Bild 1816, main16.5.wav geladen, Nachricht 24 offen`.
* `test_r36_elza <teil>` einzeln von Hand: alle 8 Teile `0 Fehler`; Ausgabe u.a. `Text dekodiert: "He has already passed..."`,
  `(Untertitel dekodiert: "Roy: groans")`, `Feuer-Clips in Bild 653 und 683`, `im Bild des zweiten Schuss-Tons (Bild 684)`,
  `Roy sackt zusammen (Clip 9) in Bild 1811`, `Stimme 24 im selben Bild eingereiht (Bild 1811)`.

## 2. RE-Gate (selbst disassembliert / Bytes selbst gelesen)

Werkzeug: `.claude/skills/re15-psx-disasm/scripts/re15_disasm.py` / `re2_disasm.py`, Binaerdateien `info/Re1.5/PSX.EXE`,
`info/Re1.5/PSX/BIN/STAGE1.BIN`, `info/re2leon/PSX.EXE`; RDT-Bytes per Python aus `re15_port/shared_assets/PSX/STAGE1/` und
`info/re2leon/PL0/RDT/`. (Die ersten vier Disassemblierungen liefen versehentlich ueber die Skript-Kopie des Hauptbaums, die
dessen gleichnamige, versionierte info/-Dateien liest; ab dem Kill-Flag-Gate ueber die Kopie im Spurbaum — nur lesend.)

| Zitat im Diff | selbst gelesen | Urteil |
|---|---|---|
| RE1.5 FUN_800396fc @0x800397e4 `srl a0,a0,31`, @0x800397ec `addu` (Elza-Bit -> Datei+1) | `800397e4: srl a0,a0,31` / `800397e8: jal 0x80013b60` / `800397ec: addu a0,a0,v0` (Delay-Slot des Lade-Aufrufs) | stimmt |
| RE1.5 Plc_neck @0x80041ed8-f10 (Flags 0x80\|0x12), @0x80041f5c-6c (pc[8..9] -> +0x9e/+0x9f) | `80041ea4: ori v0,zero,0x80` / `80041ea8: sb v0,440(v1)` (+0x1b8 = 0x80), Modus 0 -> `80041ed8: lbu v0,440(v1)` / `80041ee0: ori v0,v0,0x12` / `80041f14: sb v0,440(v1)`; `80041f5c: lh a1,8(v0)` / `80041f64: srl v0,a1,8` / `80041f68: sb a1,158(v1)` / `80041f6c: sb v0,159(v1)` | stimmt |
| RE1.5 STAGE1 FUN_80106c18 @0x80106da8/ac Raum-SE 5 | `80106d9c: andi v0,v0,0x1` / `80106da0: nop` / `80106da4: ori a0,zero,0x8` / `80106da8: ori a0,zero,0x5` / `80106dac: jal 0x800453d0` — kein Sprung zwischen den beiden `ori`, a0 = 5 | stimmt |
| RE2 @0x80051654-5c Unlock = Raumbank-Satz 0x25, nicht positional | `80051640: addiu a2,zero,5` (msg 5) / `8005164c: jal 0x8002fe38` / `80051654: lui a0,0x225` / `80051658: jal 0x8005ba28` / `8005165c: addu a1,zero,zero`; 0x8005ba28 zerlegt a0: `srl t1,a0,24` (Bank 2), `srl v0,a0,16; andi s7,v0,0xff` (Satz 0x25), a1 = Lage 0 | stimmt |
| RE2 ROOM1010 @0x01C18/@0x01C1C/@0x01C20 (Geste, Sleep 20, Se_on) | `3f 00 00 00` / `09 0a 14 00` / `36 02 0a 01 ..`; @0x01C2C `09 0a 0a 00`, @0x01C30 `3f 00 00 c0` | stimmt |
| RE2 ROOM1010 @0x02676/@0x0267A/@0x02680/@0x0268E (Schuss-Zeitform) | `04 ff 18 1f` / `3f 00 0c 00` / `02 00` / `36 02 0c 01 03 00 ..`, danach `3a 00 18 ..` (Muendungsfeuer) | stimmt |
| RE2 ROOM40A0 @0x02881/@0x02896 (NPC-Schiess-Clip, Se_on(2,8)) | `3f 02 0e 04` / `36 02 08 01 02 00 00 00 00 00 00 00`; dazwischen `09 0a 02 00 .. 09 0a 08 00` (in ROOM40A0 liegt der Ton ~10 Bilder nach dem Clip — der Port nimmt die ROOM1010-Zeitform +1, s. P4) | stimmt |
| RE1.5 ROOM1211 @0x020F8 `3f 00 00 00` + `09 0a 3c 00` | ab 0x020F0: `bf aa 18 05 2e 01 00 00 [3f 00 00 00] [09 0a 3c 00] 3f 00 00 00 43 00 80 00 09 0a 01 00 2e 03 00 00 ..` | stimmt |
| RE1.5 ROOM1021 @0x020CE/@0x020D6/@0x020DE | ab 0x020C2: `2e 02 00 00` (Work_set John) `3f 02 06 00` `09 0a 6e 00` **`3f 02 07 00`**@0x20CE `09 0a 1e 00` **`3f 02 07 00`**@0x20D6 `2e 02 01 00` (Work_set Roy) **`3f 01 07 00`**@0x20DE `09 0a 64 00` | stimmt |
| RE1.5 ROOM1011 @0x00D54 zwischen msg 16 und msg 17 | `2b 10 00 00`@0x0D3A (msg 16) ... `29 01`@0x0D52 (Cut_chg 1) **`3f 00 09 00`**@0x0D54 `09 0a 64 00` `2b 11 00 00`@0x0D5C (msg 17) | stimmt |
| ROOM1011 msg 10 @0x010C9 (23 B) = k_stoehnen_text | `04 00 05 02 2e 4b 55 16 05 00 00 5f 43 4e 4b 3d 4a 4f 5f 04 01 01 63` = Array bytegleich; rdt_msgdump: id 10 `Roy: groans` | stimmt |
| ROOM1151 @0x00D92 Slot 2 sce 5 Flags 0x31, @0x00E6A Irons | `2c 02 05 31 ..` / `44 00 45 00 00 00 00 ff 62 ad 30 fd ed 99` = Typ 0x45, (-21150,-720,-26131) | stimmt |
| ROOM1121 main00 @0x00D0C drei Kraehen Typ 0x21, Kill 0xA7..0xA9, y = 0 | `44 00 21 00 00 00 00 a7 ..`, `44 01 21 .. a8`, `44 02 21 .. a9`, alle y-Feld 0 | stimmt |
| ROOM1120.RDT == ROOM1121.RDT | sha1 beide `7d20974ca468...` | stimmt |
| ROOM1021 snd1 Satz 5 = VAG 6, 15392 B | EDT @0x145A8+5*4 = `00 00 69 15`, VAG 6 @0x1C8E8 15392 B sha1 04a4e6891c76; dieselbe Welle als Satz 5 in ROOM1000/1010/1011/1020/1060/1070/1080/10D0/10E0/10F0/1100/1110/11E0/1250 (Zombie-Raumbank) | stimmt |
| ELZASE.VBS = RE2-Daten bytegleich bis auf Satz-/Tonindex | Tone-Records gegen ROOM1140 @0x01DD0 / ROOM40A0 @0x0A800: einzige Abweichung Byte +0x16 (VAG-Index); EDT `00 00 84 16`->`00 00 04 16`, `00 00 a3 00`->`00 00 13 00` (nur Programm/Tone-Nibble); VAG-Daten bytegleich (unit_r36_elza_bank) | stimmt |

Rate-Woerter im Diff (`deferred|tunable|interim|for now|faithful|plausib|TODO|FIXME|approx|vorlaeufig`): keine Treffer (nur
"xxx1" und RE1.5s eigener "Platzhaltertext gunshot"). Einziger neuer getenv: `RE15_ELZA_LOG` (Messschiene, kein Verhalten).
Port-Wahlen sind als solche benannt: Kill-Flag 0xAC (Zensus), Kraehen-Lage (Ende des gemessenen Durchflugs), Irons-Totenpose
(= Leons Irons nach r35), Untertitel-Wortlaut "Roy: groans" (= Original-msg 10), Ton-Vorlagen (RE2), Nachrichten-Id 24 (1011).

## 3. Messungen an der exe, je Nutzer-Punkt

Messkopie `re15_port/build/platform/pc/re15_pc_abn0L.exe` (Kopie der gebauten exe), Laeufe ueber
`scratchpad/abn0L/run.sh <name> <timeout> KEY=WERT...` (Arbeitsordner je Lauf im Scratchpad, nie im Baum),
Spielstaende mit dem Kartenwerkzeug der Spur (`probe_r36_elza_karte`, Figur 4 = Elza aus der Raum-Variante).

### P1 — ROOM1121: Scheibe immer kaputt + eine Kraehe mehr

Wortlaut: "Bei Elzas Szenario soll die Scheibe in Room 1120 die bei Leons Szenario zum schluss zerspringt in Stage1,
bereits immer kaputt sein, und eine Kraehe mehr im Raum."

Lauf p1 (ECHTER Weg: Elza-Spielstand ROOM1131 vor Tuer-Slot 1, CONTINUE, Aktionstaste, Tuersequenz, ROOM1121, zu Fuss
Cut 3 -> Cut 2 -> Cut 1 nach Norden zum Fenster):
```
probe_r36_elza_karte re15_card.mcr 1131 -3050 -2150 2048
run.sh p1 400 RE15_NO_INTRO=1 RE15_WINDOW_SCALE=1 RE15_CONTINUE_TEST=1 RE15_CARD_AUTO=1 RE15_CARD_SLOT=0 RE15_SPAWN_DIAG=1
  RE15_STATE_LOG=state.log RE15_AOT_DUMP=1 RE15_INPUT_SCRIPT_BASIS=spiel RE15_INPUT_SCRIPT_START=330
  RE15_INPUT_SCRIPT=A0.1,W5,XU1.2,L0.3667,XU0.4,R0.3667,XU0.97,L0.3667,XU0.5,U3,W2 RE15_FRAMEDUMP=560-760/8:bild_ RE15_EXIT_AT=780#1121
```
debug.log:
```
[pl] Spieler-Familie PL04 (character=4, Elza-Bit=1)
[save] CONTINUE: resumed in room 1131 (hp=100)
[aot]  1 DOOR       Mitte( -3050, -2150) halb(  500, 1000) ev=0 sce=31 band=0 -> ROOM1120 cut=3
[aot] DOOR FIRE slot=1 rect=(-3050,-2150,hw=500,hh=1000) target_cut=3 spawn=(-7600,0,-2900)
[room] PC loaded room1121.rdt (86900 bytes)
[spawn-diag] Sce_em_set type=0x21 behavior=0x00 slot=0 pos=(5250,0,7250) dir=3072
[spawn-diag] Sce_em_set type=0x21 behavior=0x00 slot=1 pos=(3900,0,-150) dir=2048
[spawn-diag] Sce_em_set type=0x21 behavior=0x00 slot=2 pos=(-2250,0,-1450) dir=1024
[spawn-diag] Sce_em_set type=0x21 behavior=0x00 slot=3 pos=(5327,0,10217) dir=1048
```
* Fenster: Framedumps F600..F760 (Schritt 8) im Elza-Raum; ab F608 Cut 1 (state.log `cam=1` 178 Bilder). In JEDEM
  Cut-1-Bild sind alle 163 "Loch"-Pixel der Ueberlagerung (gen/fenster1120_schaden.inc, art 0) dunkel (Luma < 45):
  `bild_000608 .. bild_000760: dunkle Lochpixel 163 / 163` (F600 noch Cut 2: 1/163). Bild `scratchpad/abn0L/p1/p1_zoom.png`
  (F624/680/704/728): gezackte Loecher mit hellen Kanten in beiden Fluegeln, Glaskruemel am Wandfuss (gehoeren zur r35-
  Ueberlagerung, y bis 133) — derselbe Zustand, den Leon nach dem Zerspringen sieht. Laut r35-Abnahme M zeigt nur Cut 1 das
  Fenster (Rueckprojektion in alle 5 Kameras), Cut 1 genuegt also.
* Kraehe: 4 Spawns (Original main00 drei + Slot 3). state.log Aktor 4: `t=21 st=1 ss1=0` (pickt am Boden) bei
  (5327,10217) von F7 bis F682; bei Annaeherung `F707 [4 t=21 st=1 ss1=2 ss2=5 .. d=1280 @(5016,9177)]` (fliegt auf),
  `F732 ss1=4` / `F757 ss1=4 @(975,8329)` (Angriffsflug) — sie verhaelt sich wie die Raum-Kraehen. Bilder: die vierte
  Kraehe am Boden unter dem Fenster (F624), auffliegend vor dem Fenster (F704).
* Leon unberuehrt (unit_r36_elza_fenster, selbst gelaufen): `Leon ROOM1120: drei Kraehen, Fenster heil`.

**Urteil P1: erfuellt.**

### P2 — ROOM1151: Irons von Anfang an tot, Anklicken -> "He has already passed..."

Wortlaut: "Bei Elzas Szenario soll Irons immer bereits von anfang an tot da liegen. Wenn sie ihn anklickt soll da stehen
"He has already passed..."."

Lauf p2a (ECHTER Weg: Elza-Spielstand ROOM1131 vor Tuer-Slot 2, CONTINUE, Aktionstaste F40, Tuersequenz -> ROOM1151;
Anklicken dort ueber `RE15_FIRE_AOT` = derselbe Pfad re15_aot_fire_slot wie der Scan; Kamera auf Cut 7 gepinnt, um Irons
zu sehen):
```
probe_r36_elza_karte re15_card.mcr 1131 -5900 16350 2048
run.sh p2a 400 RE15_NO_INTRO=1 RE15_WINDOW_SCALE=1 RE15_CONTINUE_TEST=1 RE15_CARD_AUTO=1 RE15_CARD_SLOT=0 RE15_STATE_LOG=state.log
  RE15_ELZA_LOG=elza.log RE15_NECK_TRACE=neck.log RE15_PRESS=square@40 RE15_FORCE_CUT=7 RE15_FIRE_AOT=2@150#1151
  RE15_FRAMEDUMP=60-300/30:bild_ RE15_EXIT_AT=300#1151
-> [aot] DOOR FIRE slot=2 rect=(-6650,16350,hw=500,hh=1000) target_cut=0 spawn=(-17150,0,-11960)
   [room] PC loaded room1151.rdt (202392 bytes)
   elza.log: F40 [elza] ROOM1151: Irons gefunden=1, Slot 2 -> Text 20 0
   state.log: Irons-Zeilen 300, davon `[1 t=45 st=4 ss1=0 ss2=2 ss3=0 g=00 mo=2 af=89` = 300 (ab dem ERSTEN Bild, kein Atem-Clip 3)
   neck.log: Kopf-Takte Irons (slot=1) 144, davon `fl=92 .. acc=(0,0) .. step=(0,0)` = 144 (kein Blick hinterher)
   state.log: `msg(a=1 fsm=1 id=20` in 150 Bildern ab F150, msg 2 nie
```
Bild `scratchpad/abn0L/p2a/p2a_sheet.png`: Irons liegt auf der Liege, Arm haengt herab (F120); F270 dazu der Untertitel
"He has already passed...".

Lauf p2b (Lade-Weg + ECHTER Tastendruck): Elza-Spielstand ROOM1151 bei (-21650,-24800) Blick Sueden (1024), der
Vorwaertspunkt (620 vor dem Spieler, aot_common.c) liegt im Rechteck von Slot 2 (`[aot]  2 MESSAGE Mitte(-21650,-26000)
halb(850,700) ev=20`), `RE15_PRESS=square@90` (Quadrat = Aktion):
```
F89 ... msg(a=0 fsm=0 id=0 q=0)   [1 t=45 st=4 ss1=0 ss2=2 ss3=0 g=00 mo=2 af=89 ...]
F90 pad=8000 ... msg(a=1 fsm=1 id=20 q=0)   [1 t=45 st=4 ss1=0 ss2=2 ss3=0 g=00 mo=2 af=89 ...]
Irons-Zeilen 241, tot 241; Kopf-Takte 90, fest 90; msg id 2: 0 Mal
```
Bild `scratchpad/abn0L/p2b/p2b_sheet.png` (Kamera Cut 0 des Spielstands): Schreibmaschinen-Text "He has already p..." (F120).

**Urteil P2: erfuellt** (beide Wege: Tuer und Spielstand; Wortlaut des Textes exakt).

### P3 — ROOM1211: Unlock-Ton, wenn Elza die Zellentuer aufschliesst

Wortlaut: "In ROOM 1211 in Elzas Cutscene wenn sie Kendo befreit fehlt noch der Unlock sound bei der Tuer wenn sie sie
aufschliesst."

Lauf p3c (ECHTER Weg: Elza-Spielstand ROOM1221 mit Prison key (4,0xEC) vor Tuer-Slot 3, CONTINUE, Aktionstaste F30,
Tuersequenz -> ROOM1211, zu Fuss nach Sueden ins Auto-Feld (AOT 6 Mitte(-19355,-19136) halb(2212,775) ev=2), Szene laeuft):
```
probe_r36_elza_karte re15_card.mcr 1221 -16600 -17900 2048 4:0xEC
run.sh p3c 700 RE15_NO_INTRO=1 RE15_WINDOW_SCALE=1 RE15_CONTINUE_TEST=1 RE15_CARD_AUTO=1 RE15_CARD_SLOT=0 RE15_STATE_LOG=state.log
  RE15_ELZA_LOG=elza.log RE15_SE_DEBUG=1 RE15_AUDIO_CAP_SYNC=cap.raw RE15_PRESS=square@30 RE15_INPUT_SCRIPT_BASIS=spiel
  RE15_INPUT_SCRIPT_START=60 RE15_INPUT_SCRIPT=L0.3667,U1.2,W2 RE15_FRAMEDUMP=900-1150/6:bild_ RE15_EXIT_AT=1250#1211
-> [aot] DOOR FIRE slot=3 rect=(-17050,-17800,hw=250,hh=1000) target_cut=4 spawn=(-18100,0,-17300)
   [room] PC loaded room1211.rdt (254616 bytes)
   state.log F69 rot=1088 -> F89 (-18155,-18410) im Auto-Feld -> Szene fuehrt Elza zur Zellentuer
   elza.log: F30 [elza] ROOM1211: 1 Anker armiert / F1014 [elza] ROOM1211 Anker @0x020F8 -> Aktion 1 (Takt 1010)
   debug.log: [se] Stimme: se=0 layer=0 vag=0 note=67 fine=109 center=89 vol=127 pitch=0x4f3 (13641 Hz) -> SE-Stimme 6 (SPU 22)
```
Mitschnitt (eigenes Auswerteskript `scratchpad/abn0L/cap.py`, Ausrichtung am Exit-Bild wie das Werkzeug der Spur), Spitze je Bild:
`F1008..F1016 ~650..900` (Hintergrund), **`F1017 1775`**, `F1018..F1023 ~600..990`, `F1024 2036`, **`F1025 8065`**, danach ~550..900.
Gegenprobe der Huellkurve: die RE2-Welle ROOM1140 @0x04480 (5264 B) selbst dekodiert (PSX-ADPCM, `scratchpad/abn0L/vag.py`) und
mit derselben Tonhoehe 0x4f3 abgespielt: 0,675 s, Klick 1 bei +2 Bildern (7143), Hauptklick bei +10 Bildern (31336). Im
Mitschnitt liegen die Klicks bei +3 / +11 nach dem Ausloese-Bild (Anschlag ab dem Folgebild) — die Welle IST der Ton im Mitschnitt.
Bild `scratchpad/abn0L/p3c/p3c_sheet.png`: F990..F1014 fuehrt Elza den Schluessel ans Schloss (Geste @0x020F8, 20 Bilder),
F1014..F1038 haelt sie ihn am Schloss (dort die Klicks F1017/F1025), ab ~F1055 schwingt die Zellentuer auf (F1062/F1074/F1086).
Zweiter Lauf p3a (Spielstand direkt im Auto-Feld, wie der Integrationshaken der Spur): Anker F924, Klicks F927 (3962) / F935
(9364) bei Hintergrund ~2000..3400 — gleiche Form.

**Urteil P3: erfuellt.** (Ton = RE2-Aufschliesston ROOM1140 Satz 0x25, Tonhoehe aus den RE2-Tone-Daten bytegleich.)
Kreuzkorrelation (eigenes Skript `scratchpad/abn0L/korr.py`: Welle selbst dekodiert, mit der Tonhoehe aus dem SE-Log
resampelt, gegen den Mitschnitt): **0,594 bei F1014+1**, Gegenprobe 60 Bilder frueher 0,007.

### P4 — ROOM1021: Schuss-Toene, wenn Kendo (John) Roy erschiesst

Wortlaut: "In Elzas Cutscene in Room 1021 wo Kendo Roy erschiesst fehlen die Schuss Sounds."

Lauf p4 (ECHTER Weg: Elza-Spielstand ROOM1011 vor Tuer-Slot 1 mit dem Story-Stand nach Kendos Befreiung — (4,0xF1) Roy
getroffen, (4,0xEC) Prison key, (4,0xFC)/(4,0x60)/(3,0xC9)/(3,0x2C)/(3,0xBA) aus ROOM1211 sub02 @0x01F76..@0x01F8E laut
analysis/story_leon_elza/ablauf_elza.md E12, (3,0x74) —, CONTINUE, Aktionstaste F30, Tuersequenz -> ROOM1021, nach Westen
ins Auto-Feld (AOT 5 `AUTO_EVENT Mitte(-12050,-7100) halb(1050,2000) ev=2`), Szene laeuft ohne weiteres Zutun):
```
probe_r36_elza_karte re15_card.mcr 1011 3650 -3950 0 4:0xF1 4:0xEC 4:0xFC 4:0x60 3:0xC9 3:0x74 3:0x2C 3:0xBA
run.sh p4 420 RE15_NO_INTRO=1 RE15_WINDOW_SCALE=1 RE15_CONTINUE_TEST=1 RE15_CARD_AUTO=1 RE15_CARD_SLOT=0 RE15_STATE_LOG=state.log
  RE15_ELZA_LOG=elza.log RE15_SE_DEBUG=1 RE15_AUDIO_CAP_SYNC=cap.raw RE15_PRESS=square@30 RE15_INPUT_SCRIPT_BASIS=spiel
  RE15_INPUT_SCRIPT_START=60 RE15_INPUT_SCRIPT=XU2.2,W2 RE15_FRAMEDUMP=650-950/3:bild_ RE15_EXIT_AT=1000#1021
-> [aot] DOOR FIRE slot=1 rect=(4650,-3950,hw=500,hh=1000) target_cut=9 spawn=(-3785,0,-7412)
   [room] PC loaded room1021.rdt (274480 bytes)
   state.log: F89 (-9771,-7486) / F119 (-11171,-7486) im Feld; Szene: msg 0 (F97), msg 1 (F247), msg 2 (F396), msg 3 (F497)
   elza.log: F30 [elza] ROOM1021: 3 Anker armiert
             F748 [elza] ROOM1021 Anker @0x020CE -> Aktion 2 (Takt 744)
             F778 [elza] ROOM1021 Anker @0x020D6 -> Aktion 2 (Takt 774)
             F778 [elza] ROOM1021 Anker @0x020DE -> Aktion 3 (Takt 774)
   debug.log (Reihenfolge):
     [scd] Plc_motion(entity=2, motion=7, flags=0x00) -> slot=1
     [se] Stimme: se=1 layer=0 vag=1 note=69 fine=109 center=91 vol=100 pitch=0x4f3 (13641 Hz) -> SE-Stimme -16 (SPU 0)
     ...
     [scd] Plc_motion(entity=2, motion=7, flags=0x00) -> slot=1
     [scd] Plc_motion(entity=1, motion=7, flags=0x00) -> slot=2
     [se] Stimme: se=1 layer=0 vag=1 note=69 fine=109 center=91 vol=100 pitch=0x4f3 (13641 Hz) -> SE-Stimme -16 (SPU 0)
     [se] Stimme: se=5 layer=0 vag=5 note=65 fine=0 center=89 vol=127 pitch=0x400 (11025 Hz) -> SE-Stimme 5 (SPU 21)
```
Mitschnitt, Spitze je Bild: F744..F747 770..960 (Hintergrund) -> **F748 6319, F749 6886, F750 6360, F751 6033**, abklingend bis
F757 1648 (Knall) | F760..F777 ~900..1250 -> **F778 6393, F779 6802, F780 7007, F781 7310**, F782 4744 ... (Knall 2).
Kreuzkorrelation mit der selbst dekodierten RE2-Welle ROOM40A0 @0x17B80 (4736 B, Tonhoehe 0x4f3): **0,806 bei F748+0**
(Gegenprobe -0,112) und **0,631 bei F778+0** (Gegenprobe -0,016; niedriger, weil Roys Ton darueberliegt).
Bild `scratchpad/abn0L/p4/p4_sheet.png`: die Kamera steht waehrend beider Schuesse auf Cut 10 (Roy im Gang). Zusatzlauf p4c
(Integrationsweg der Spur mit `RE15_FORCE_CUT=9`, `scratchpad/abn0L/p4c/p4c_sheet.png`): John zielt beidhaendig mit einer
PISTOLE, Anker F698 -> Ton F699, Rueckstoss im Bild F700 — Pistolenknall passt zur Waffe, Ton 1 Bild nach Clip-Start.

**Urteil P4: erfuellt** (zwei Schuesse = die zwei Feuer-Clips @0x020CE/@0x020D6 des Original-Skripts, 30 Bilder auseinander).

### P5 — ROOM1021: Roys Sterbe-/Leid-/Hinfall-Ton, sobald er erschossen wurde

Wortlaut: "In Elzas Cutscene in Room 1021 wo Kendo Roy erschiesst fehlt noch ein Sterbe/Leid/Hinfall sound von Roy, sobald
er erschossen wurde."

Derselbe Lauf p4. Roys Todes-Clip @0x020DE `3f 01 07 00` laeuft im Bild des zweiten Schusses (F778); im selben Takt
`[se] Stimme: se=5 ... vag=5 ... (11025 Hz)` = Raum-SE 5 (ROOM1021 snd1, EDT `00 00 69 15` -> VAG 6, 15392 B; dieselbe
Welle ist Satz 5 der Zombie-Raumbaenke 1000/1010/1020/1060/1070/...; RE1.5-Zombie-Tod @0x80106da8/@0x80106dac).
Mitschnitt ab F787: stimmhaft (Nulldurchgangsrate 0,015..0,037 statt 0,08..0,10 beim Knall), Spitzen 3000..8664 bis ~F850
(Welle 2,44 s bei 11025 Hz). Kreuzkorrelation mit der selbst dekodierten Welle ROOM1021 @0x1C8E8 (Tonhoehe 0x400):
**0,739 bei F778+1** (Gegenprobe 60 Bilder frueher 0,006). Reihenfolge im Log: Schuss-Ton vor Sterbe-Ton (Ursache vor Wirkung).

**Urteil P5: erfuellt** (der Ton erklingt mit dem toedlichen zweiten Schuss).
Beobachtung (nicht Teil des Wortlauts, Spur-Dossier OFFEN 1, s. §5 B1): Roy bleibt im Bild stehen — p4_sheet F779..F920 zeigt
ihn unveraendert aufrecht im Gang, waehrend das Sterbe-Stoehnen laeuft.

### P6 — main16.5 abspielen, waehrend Roy bewusstlos wird, + Untertitel

Wortlaut: "Ich habe fuer Elzas Szenario Synchron Dateien hinzugefuegt. In Room 1101 - waehrend Roy bewusstlos wird - moechte
ich das du das Voiceover main 16.5 abspielst. Auch wenn es aktuell keinen Text dafuer gibt. Erstelle gerne ein Untertitel
dafuer, wie bei den anderen auch."

Raum-Zuordnung (selbst geprueft): ROOM1101 (= ROOM1100 bytegleich, rdt_msgdump) traegt nur die 6 Texte des Stimmschlosses
("It's electronically locked." ... "I don't need to use this anymore."), keinen Roy. Roy wird nur in ROOM1011 bewusstlos:
sub02 `2b 10 00 00`@0x00D3A (msg 16 "Roy: Please, you have to help him...") -> `29 01`@0x00D52 -> **`3f 00 09 00`@0x00D54** ->
`09 0a 64 00` -> `2b 11 00 00`@0x00D5C (msg 17 "Elza: Roy, wake up! Please wake up!"), spaeter msg 18 "He has passed out from
his injuries.". Die Nutzer-Datei liegt in `synchro/STAGE1/room1011/` (Hauptbaum, unversioniert) zwischen main16 und main17 —
"16.5" = genau diese Stelle. "1101" ist ein Zahlendreher; die Spur hat richtig ROOM1011 genommen.

Lauf p6 (ECHTER Weg: Elza-Spielstand ROOM1021 vor Tuer-Slot 1 OHNE Story-Flags (erster Besuch bei Roy), CONTINUE,
Aktionstaste F20, Tuersequenz -> ROOM1011, Roy-Szene laeuft von selbst; die 19 Nutzer-WAVs von room1011 (main00..17 +
main16.5, nur gelesen) liegen im Arbeitsordner unter `synchro/STAGE1/room1011/`, die Original-Datei, kein Stellvertreter):
```
probe_r36_elza_karte re15_card.mcr 1021 -4300 -18450 0
run.sh p6 540 RE15_NO_INTRO=1 RE15_WINDOW_SCALE=1 RE15_CONTINUE_TEST=1 RE15_CARD_AUTO=1 RE15_CARD_SLOT=0 RE15_STATE_LOG=state.log
  RE15_ELZA_LOG=elza.log RE15_MSG_LOG=1 RE15_AUDIO_CAP_SYNC=cap.raw RE15_PRESS=square@20 RE15_FRAMEDUMP=1700-2100/6:bild_
  RE15_EXIT_AT=2150#1011
-> [aot] DOOR FIRE slot=1 rect=(-2950,-18450,hw=750,hh=1150) target_cut=0 spawn=(3650,0,6900)
   [room] PC loaded room1011.rdt (210440 bytes)
   [voice] clip loaded: main16.wav (32000 Hz x1 -> 113152 @44100 Hz)
   [voice] clip loaded: main16.5.wav (32000 Hz x1 -> 63488 @44100 Hz)
   [voice] clip loaded: main17.wav (32000 Hz x1 -> 188928 @44100 Hz)
   elza.log: F20 [elza] ROOM1011: 1 Anker armiert / F1939 [elza] ROOM1011 Anker @0x00D54 -> Aktion 4 (Takt 1935)
   state.log: F1838..F1937 msg id=16 | F1939 cam=1, Roy `mo=9`, msg(a=1 fsm=1 id=24) bis F2038 | F2039 msg id=17
```
Mitschnitt: Kreuzkorrelation der Nutzer-Datei (32 kHz -> 44,1 kHz resampelt, `scratchpad/abn0L/korr_wav.py`) mit dem Mitschnitt:
**main16.5.wav 0,607 bei F1939+1** (Gegenprobe 150 Bilder frueher 0,012); Attrappen an derselben Stelle: main10.wav 0,200,
main00.wav 0,169 — es klingt die Nutzer-Aufnahme, keine andere.
Untertitel: Bild `scratchpad/abn0L/p6/p6_sheet.png`: F1898/F1934 "Roy: Please, you have to help him...", **F1940..F2018
"Roy: \*groans\*"** (Name rot/orange, Text weiss — dieselbe Form wie alle Roy-Zeilen), F2042/F2084 "Elza: Roy, wake up! Please
wake up!". Text = ROOM1011 msg 10 @0x010C9 woertlich (23 Byte, Array-Vergleich §2), Anzeige 100 Bilder wie die Nachbarzeilen.
Wortlaut-Pruefung der Datei (50-ms-Huellkurve, selbst gerechnet): main16.5 = EIN stimmhafter Ausbruch 0,40..0,90 s
(Nulldurchgangsrate 0,007..0,033), danach hauchiger Ausklang — dieselbe Form wie main10 ("Roy: groans") und main00 ("Officer:
groans"); main16 (ein Satz) zeigt dagegen mehrere Silbenpakete. "*groans*" beschreibt die Aufnahme.

**Urteil P6: erfuellt** (Raum richtig gedeutet, Nutzer-Datei unveraendert unter ihrem Namen geladen, an der Stelle @0x00D54
zwischen main16 und main17, Untertitel in der Form der anderen).
Beobachtung (nicht Teil des Wortlauts, s. §5 B2): Roy sackt im Bild NICHT zusammen — `scratchpad/abn0L/p6/p6_roy.png`:
F1940 vorgebeugt, F1946..F1958 richtet er sich auf, F2018/F2084 steht er aufrecht, waehrend "*groans*" und "Roy, wake up!"
laufen.

## 4. Gates

| Gate | Messung | Ergebnis |
|---|---|---|
| RE-Gate: Konstanten mit Beleg | Diff gelesen: Anker-Offsets/Signaturen (RDT-Bytes), Verzuege 20/1/1/0 (RE2 ROOM1010 @0x01C1C bzw. sub31 @0x02680; Roy = Takt von Schuss 2; Stoehnen = vor dem Opcode), Toene (RE2 @0x80051654-5c / ROOM40A0 @0x02896 / RE1.5 @0x80106da8), Irons (Clip 2 Bild 89, @0x80041ed8-f10, @0x80041f5c-6c, Slot @0x00D92), Kraehe (Form @0x00D0C, Lage gemessen, Kill-Flag Zensus). NUTZER-VORGABE/PORT-WAHL benannt (Text "He has already passed...", Kraehen-Lage, Kill-Flag, Untertitel-Wortlaut, Id 24). Commit 059cc672 nennt dieselben Adressen. | haelt |
| RE-Gate: Stichproben selbst disassembliert | 0x800397e4/ec, 0x80041ea4-f14 + 0x80041f5c-6c, 0x80106d9c-dac (STAGE1.BIN), RE2 0x80051640-5c + 0x8005ba28 (Zerlegung a0), Gate 0x80042120-38 (`lbu a1,7(a1)` / `beq a1,0xff` / `jal 0x8004efe4` / `beq v0,zero`), Loco-Kanal 0x80050e64/6c; RDT-Bytes in ROOM1211/1021/1011/1151/1121 und RE2 ROOM1010/40A0/1140 (Tabelle §2) | alle Zitate stimmen |
| Rate-Woerter / Env-Schalter | keine (§2); einziger getenv = Messschiene RE15_ELZA_LOG | haelt |
| Fix erklaert den Befund | Vorher-Messung des Dossiers (kein Se_on in den drei Szenen, Irons Atem-Clip 3 + msg 2, 3 Kraehen + heiles Fenster, main16.5 nie geladen) -> nachher genau diese Stellen geaendert; meine Laeufe zeigen jeweils die neue Wirkung an der zitierten Stelle (Anker-Bild = Ton-/Text-Bild) | haelt |
| Vertrag: Bank-9-Bits | keine benutzt (Reserve 86 frei gelassen) | haelt |
| Vertrag: Nachrichten-IDs | ROOM1151 Id 20 (zugeteilt 20..23); ROOM1011 Id 24 = RDT-Zahl (statt der zugeteilten ROOM1101 20..21, weil die Szene in 1011 liegt — begruendet im Dossier §3/§8, keine andere Spur hat 1011-IDs) | haelt, Abweichung dokumentiert (H4) |
| Vertrag: Ereignis | 26 nur in ROOM1121 (zugeteilt L = 26) | haelt |
| Vertrag: Kill-Flag | (7,0xAC): eigener Byte-Zensus STAGE1-3: kein Ck/Set(7,0xAC); 6 Byte-Treffer "44 bf 2e 13 bb fe 00 ac" in 10E0/10E1/1110/1111/2010/2011 sind keine Sce_em_set (Slot-Byte 0xBF) | frei |
| Vertrag: Pfade | `git diff master --name-only`: keine release/, platform/android/, shared_assets/PSX/, tests/*/CMakeLists.txt, RELEASE_NOTES (0 Treffer); neue Daten nur shared_assets/RE2/ELZASE.VBS + gen/re2_elza_bank.inc | haelt |
| Vertrag: gemeinsame Dateien nur kleine Haken | scd_vm.c +6, scd_room_setup.c +2 (Installer-Zeile am Ende des Blocks, Kommentar "Runde 36 Spur L"), main.c +3, audio_pc.c +17/-4 (Durchreiche re15_audio_se_play_bank + Dateistamm im Stimmen-Lader; audio_pc.c steht nicht in der §1.4-Liste); fenster_1120.c/.h = Eigentum L | haelt |
| Tests je Punkt, messend | 8 Unit-Teile + 5 Integrationshaken (probes/r36_elza.cmake, eigene test_r36_elza.c/.cmake), Arbeitsordner r36_elza_wd != Targetname, WINDOW_SCALE=1, Messkopie unter eigenem Namen, kein taskkill; selbst gefahren: 79/79 Unit, 12/12 Integration | haelt |
| Nebenwirkungen beide Szenarien | Leon 1120 (drei Kraehen, Fenster heil: unit_r36_elza_fenster, unit/integration_r35_fenster gruen), Leon 1150 (Irons lebt, Slot 2 sce 5: unit_r36_elza_irons; integration_r35_cut1150 gruen), Leon-Raeume ohne Anker (unit_r36_elza_signatur), Stimmen-Lader unveraendert fuer mainNN (unit_synchro_stimme, unit_voice_ausreden gruen; Log-Zeile `[voice] clip loaded: main16.wav` unveraendert), CONTINUE in 1121: 4 Kraehen, keine Doppel-Spawns (Lauf p1c) | keine gefunden |

## 5. Beobachtungen und Hinweise (nicht blockierend — ausserhalb des Wortlauts oder Sache des Orchestrators)

* **B1 — Roy faellt in ROOM1021 nicht sichtbar** (Spur-Dossier OFFEN 1): p4_sheet F779..F920 — Roy steht aufrecht, waehrend
  die Schuesse fallen und sein Sterbe-Stoehnen laeuft. Vorbestehend (der Zweig aendert keine Animation); Ursache laut Dossier
  @0x020DE `3f 01 07 00` -> Loco-Kanal (@0x80050e64 `lw a0,132(v0)` / @0x80050e6c `lw a1,364(v0)`, selbst disassembliert),
  EM047 Clip 7 = 1 Bild. Messweg: DuckStation-Savestate der Szene, Roy-Entity +0x84/+0x16c.
* **B2 — Roy sackt in ROOM1011 nicht sichtbar zusammen** (im Spur-Dossier NICHT vermerkt): `p6/p6_roy.png` — F1940 vorgebeugt,
  F1946..F1958 richtet er sich auf, F2018/F2084 steht er aufrecht, waehrend "*groans*" und "Elza: Roy, wake up! Please wake up!"
  laufen. Das Dossier (§2, §3 P6) und die Ausgabe von unit_r36_elza_stoehnen ("Roy sackt zusammen (Clip 9)") deuten nur die
  Clip-Nummer @0x00D54 `3f 00 09 00`; das GERENDERTE Bild zeigt das Gegenteil (Lehre "Platzhalter ist trotzdem Verhalten").
  Vorbestehend (Roy laeuft in Clip 9 frei durch, `mo=9 af=1 -> af=19 ss2=2`, keine Sperre durch Nachricht 24 mit Maske 0).
  Vermutlich dieselbe Familie wie B1 (Roys Plc_motion-Bank). Messweg wie B1 (Savestate ROOM1011, Roy-Entity +0x84/+0x16c).
  Dem Nutzer NICHT als "Roy sackt zusammen" melden.
* **H3 — Paket**: neu `re15_port/shared_assets/RE2/ELZASE.VBS` (13144 B, md5 8e996ef238defe2188292e55af7f8ab2); die
  Nutzer-Aufnahme `synchro/STAGE1/room1011/main16.5.wav` (und ueberhaupt alle neuen Elza-Synchronordner room1011/1021/1031/
  1041/1141/1211/11B1) ist im Hauptbaum UNVERSIONIERT und fehlt im Spurbaum — der Integrationshaken prueft mit einer
  Stellvertreter-Datei (room1170/main00.wav) nur die Zuordnung. Paket-Gate muss die Nutzer-Dateien mitnehmen.
* **H4 — Id-Abweichung zusammenfuehren**: ROOM1011 Nachricht 24 (statt ROOM1101 20..21). Beim Zusammenfuehren pruefen, dass
  Spur P (Text-Sweep) und Spur Q (Prison key aus 1011) in ROOM1011 keine Id 24 belegen.
* **H5 — Orchestrator**: RE15_MIN_TESTS +13 (Suite 605); Android neu konfigurieren (neue Quellen engine/src/elza_r36.c,
  platform/pc/src/elza_se_pc.c).

## 6. Maengel

Keine blockierenden Maengel. (B2 ist eine falsche Beschreibung im Dossier/Testtext, kein Verstoss gegen den Wortlaut von P6:
Stimme und Untertitel liegen genau an der Original-Stelle, an der Roy laut Skript bewusstlos wird.)

## 7. Urteil

| Punkt | Urteil | Kernbeleg |
|---|---|---|
| P1 Fenster 1121 kaputt + Kraehe | erfuellt | Tuerweg 1131->1121: 163/163 Lochpixel in allen Cut-1-Bildern, 4 Kraehen (Slot 3 (5327,0,10217)), Kraehe fliegt bei Annaeherung auf |
| P2 Irons 1151 tot + Text | erfuellt | Tuerweg 1131->1151 und Spielstand: Irons 300/300 bzw. 241/241 Bilder Clip 2 Bild 89, Kopf 144/144 bzw. 90/90 fest; Quadrat F90 -> msg 20 "He has already passed...", msg 2 nie |
| P3 Unlock 1211 | erfuellt | Tuerweg 1221->1211: Anker F1014, Klicks F1017/F1025, Korrelation 0,594 mit RE2 ROOM1140-Welle, vor dem Aufschwingen (~F1055) |
| P4 Schuesse 1021 | erfuellt | Tuerweg 1011->1021: Knall F748 / F778, Korrelation 0,806 / 0,631 mit RE2 ROOM40A0-Welle; John schiesst mit Pistole |
| P5 Roys Sterbe-Ton 1021 | erfuellt | Raum-SE 5 im Takt von Schuss 2, Korrelation 0,739 ab F779; (B1: Roy faellt nicht sichtbar, vorbestehend) |
| P6 main16.5 + Untertitel | erfuellt | Tuerweg 1021->1011: main16.5.wav geladen, Korrelation 0,607 ab F1940 (Attrappen 0,20/0,17), "Roy: *groans*" F1940..F2018 zwischen msg 16 und 17; (B2: Roy sackt nicht sichtbar zusammen, vorbestehend) |

**bestanden: ja** — alle sechs Punkte am gebauten Stand gegen den Wortlaut gemessen erfuellt, alle Gates halten.
Fuer die Abschlussmeldung an den Nutzer: B1 und B2 als eigene Folgepunkte anbieten (Roys Posen in 1021/1011 gegen das Original
messen), H3 (Nutzer-Synchrondateien ins Paket), main16.5-Wortlaut "*groans*" (Frage der Spur) bestaetigen lassen.

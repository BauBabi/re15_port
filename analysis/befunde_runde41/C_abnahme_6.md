# Runde 41 — Spur C "ventilkette" — Abnahme 6 (unabhaengig)

Baum `.claude/worktrees/r41_ventilkette`, Zweig `r41/ventilkette`, gepruefter Stand HEAD **1e17827f** (Basis = merge-base master
116e8b0f; master steht auf 645fa45f und enthaelt fremde Spuren — Gegenstand ist der Spur-Diff `git diff master...HEAD`).
Massstab: die sechs Punkte des Auftragstexts und AUFTRAG.md (massgeblich) mit den Spur-C-Nachtraegen 6-10, 12, 13, 15, 16, 18, 19
und **25 + C-E4** (im Spurbaum vorhanden), den Entscheiden C-a..e, C-E1..C-E4, C-E2b (C-E2c widerrufen); Vorlage: Abnahme 5
(`C_abnahme_5.md`, Maengel 1-3), Nachbesserung 6 (Dossier §0g, Commits b2841a4a..1e17827f).
Begonnen 2026-10-25 06:1x MEZ. Pruefer: Abnahme 6 (Opus). Der Bericht wird FORTLAUFEND geschrieben. Messordner scratchpad `C41abn6/`
(unten `<a6>` = `C:/Users/MJOEDI~1/AppData/Local/Temp/claude/c--workspace-git-reAi-v2/c41eae99-e724-4cb3-afb9-119709f20a9d/scratchpad/C41abn6`).

**Ergebnis: (folgt — Bericht UNVOLLSTAENDIG)**

## 0. Stand / Protokoll (neueste zuerst)
* 06:2x Unit-Gruppe gruen, Integrationsstroeme gestartet, erste Messfahrt `kb` (Leon, echter Weg mit Ton) laeuft.
* 06:19 Bau: `local_build.sh configure` -> `=== LOCAL-BUILD-OK (configure)` (Zeile 180 `integration_r41_ventilkette_kette uebersprungen:
  Spur A fehlt`), `local_build.sh build` -> `ninja: no work to do.` / `=== LOCAL-BUILD-OK (build)` (`<a6>/configure.txt`, `<a6>/build.txt`).
  exe `re15_pc.exe` 2026-10-25 04:59:05 (7945443 B); Quellen ventilkette_r41.c 04:53 / audio_pc.c 04:54 / ventilkette_r41_pc.c 04:58 /
  Kopf 04:52 aelter als ihre .obj (04:58/04:59) -> exe = Code HEAD (alle Engine-/Plattform-Aenderungen der Nachbesserung 6 liegen in
  81dadaab 05:00; die spaeteren Commits aendern nur Tests/Doku/plakat.py-Kommentar); `git diff HEAD` leer.
* 06:1x Start: `git status` = nur `?? build/`; HEAD 1e17827f; seit Abnahme 5 (b8d56709..HEAD) 8 Commits: Code ventilkette_r41.c (+46/-18),
  re15_ventilkette_r41.h (+44/-12), ventilkette_r41_pc.c (+104/-5), audio_pc.c (+30/-1, Haken C-E4), neues Asset
  shared_assets/RE2/SOUND/SUB_25.BGM, Riegel (unit R1-R3, Teil A Bildvergleich, neuer integration_r41_ventilkette_alarm), Doku.

## 1. Messungen an der exe (eigene Laeufe)
Werkzeug: `<a6>/lauf.sh <name> <timeout> KEY=VAL...` (eigene exe-Kopie `re15_pc_abn6c_<name>.exe` neben der exe, erbt keine
RE15_-Variablen, Laufordner `<a6>/mess/<name>`, Kopie danach geloescht; nie taskkill /IM). Wegplaene = die der Abnahme 5
(`<a6>/plaene/*.txt`, unveraendert kopiert; Treiber RE15_WEGPLAN = echte Pad-Bits). Beschleunigter Renderer (kein
RE15_SOFTWARE_RENDER), RE15_WINDOW_SCALE=1, RE15_FRAMEDUMP (Bildzaehler je Raum). Leon = Titel-Haken, Elza = RE15_PSELECT_AUTO=1
RE15_PSELECT_AUTO_SWITCH=1. exe = `re15_pc.exe` 04:59:05 = Code HEAD. Eigene Auswerter: `<a6>/py/capvergleich.py` (Takt-Differenz zweier
Aufnahmen, 1470 Stereo-Bilder int16 je Takt), `<a6>/py/alarmform.py` (Huellkurven-Periode/Spektrum), `<a6>/py/cdcheck.py` +
`cdtext.py` (EIGENER Nachbau der RE2-Anzeige: TIM-Leser fuer `info/re2leon/COMMON/DATA/TEX.TIM`, Zaehler nach den selbst
disassemblierten Stellen, Vergleich aller deckenden Glyphen-Pixel), `<a6>/py/re2vram.py` (eigener DuckStation-Leser: zstd-Frame,
RAM-Basis ueber die Code-Signatur von FUN_8002fa28, VRAM ab "GPU-VRAM"). Kein Werkzeug der Spur (re2cd/capdiff/capkorr) benutzt.

### KB — Mangel 1 (Alarm) + Mangel 2 (Countdown) + Punkt 3 (Flucht -> 20B0), ECHTER Weg (Leon `kb`; Plan und Parameter = Abnahme 5 `kb`)
Start 2010 am Kasten (Bits 101/102, (3,108), (9,125)), MIT Ton (RE15_SE_DEBUG, RE15_AUDIO_CAP_SYNC), Framedump alle 15 Bilder:
* Weg wie Abnahme 5: `loaded room2010.rdt` -> Tuer slot 0 -> `loaded room2000.rdt` -> slot 3 -> `loaded room2070.rdt` -> slot 1 ->
  `loaded room2030.rdt` -> `[rbj] Runde 41 Spur C: ROOM2030 bindet das RBJ von RE2 ROOM1080` -> `id=55`/`53`/`51`/`50` ->
  `Folge 09 07 02 01 -> RICHTIG: Warnung, Zaehler, Flut`.
* **Alarm an mit dem Warntext:** `[bgm] loaded: 6 VAGs, SEQ 244B, PPQN=480 tempo=500000us` -> **`[bgm] Runde 41 Spur C: RE2-Alarm SUB_25 an
  (ROOM2030, Warntext 46) geladen capTick 7188 F5111`** -> `[msgtext] room=2030 id=46 "WARNING: The water will rise in one minute. Please
  leave the room immediately!"` -> `[voice] Start ROOM2030 Id 46 ... capTick 7189` (Stimme wie Abnahme 5: derselbe capTick 7189).
* **Countdown in der RE2-Form:** nach dem Schliessen des Textes `Zaehler laeuft: 1800 Takte, Ost-Tuer -> 20B0, Wasser geparkt` ->
  **`Countdown RE2-Form Bild 0 " 0^59=98" Farbe 2 (x 104 y 32) F5563`**, `Bild 30 " 0^58=98" F5593`, `Bild 60 " 0^57=98" F5623` ... je 30 Bilder
  eine Sekunde, `Bild 810 " 0^32=98" F6373` -> zu Fuss ueber Treppe und Ring zur Ost-Tuer -> `DOOR FIRE slot=0 rect=(7350,-200,hw=500,hh=1750)
  target_cut=11` -> **`loaded room20b0.rdt`** -> `Zaehler lief beim Verlassen von 2030 -> Bits 103 + 100 (Flut gelaufen, Leiter frei)` ->
  `[bgm] stage=1 room=0B entry=4A51 -> MAIN11(flag 1) SUB(flags 1/0)` -> **`RE2-Alarm SUB_25 aus (Raumwechsel, SUB-Platz vom Raum-BGM-Lader)
  capTick 8634 F1`**.
* **Bild gegen meinen eigenen RE2-Nachbau** (`cdcheck.py`, Start F5563, alle 60 Framedumps F5500..F6385): **55 von 55 Bildern nach dem Start
  exakt** (jedes deckende Glyphen-Pixel gleich, 515..605 Pixel je Bild, z. B. F5575 `" 0^59=58"` 596/596, F5605 `" 0^58=58"` 605/605, F6385
  `" 0^32=58"` 576/576; Farbe aus BGR555 << 3), die 5 Bilder davor ohne Anzeige. Bild **`<a6>/bilder/kb_vorher_nachher.png`** (F5590 links
  Abnahme 5 = `00:59:06` oben rechts in der Nachrichtenschrift, rechts heute = ` 0'59"08` dunkelrot in der 14x14-Schrift oben mittig; die
  alte Anzeige ist weg), Bogen `<a6>/bilder/kb_countdown.png`.
* **Alarm im Ton gegen einen unabhaengigen Zwilling:** dieselbe Fahrt der Abnahme 5 (`C41abn5/mess/kb/cap.raw`, alte exe ohne Alarm, gleicher
  Plan) — `capvergleich.py`: beide 8934 Takte; **bitgleich bis Takt 7190**, erste Abweichung **Takt 7191,043** (3 Takte nach dem Laden = SEQ0
  erste Note bei Tick 100 = 0,104 s), **RMS(neu-alt) 748,5..749,3 je 60 Takte durchgehend 7220..8599** (max |d| 2740..3094), mit der
  Raumwechsel-Ausblendung 686,8 (8600..8659) -> 143,1 (8660..8719) -> **0,0 ab Takt 8720** (bis 8933 bitgleich). Die Aufnahme ist ausserhalb
  des Alarms unveraendert (Raummusik, Stimme 46, Ventil-Toene, 20B0-Musik). Form (`alarmform.py`, Takte 7200..8600): Huellkurven-Periode
  **120 Takte = 4,00 s** (Autokorrelation 0,91) = SUB_25 SEQ0 (eigene Zerlegung des Containers: ppqn 480, Tempo 500000 us, Kanal 0 wechselt
  die Notenpaare 46/20 <-> 58/32 alle 3840 Ticks = 4,0 s, Schleife CC99 20 @Tick 100 .. CC99 30 @Tick 30820; Kanal 1 = Programm 1).

### RE2 — die Original-Anzeige selbst nachgelesen (Gegenprobe der Spur-Behauptung "RE2 zeigt 0'56"98 rot / 4'55"45 gruen")
* Quelle: die DuckStation-Staende der Nachbesserung (`nb6/re2cap/`): `quelle_ss2.sav` ist **bytegleich** dem Nutzer-Stand
  `%LOCALAPPDATA%/DuckStation/savestates/SLUS-00748_2.sav` (cmp); `ss2_cd60.sav`/`ss2_cd300.sav` = gepatchte Kopien; `cd60/resume.sav`/
  `cd300/resume.sav` = der Zustand nach dem Lauf im echten RE2. Mit meinem eigenen Leser (`re2vram.py`, kein Werkzeug der Spur):
  - Nutzer-Stand: `Status_flg 0x800cfbd8 = 0x24800000 (Timer-Bit 0)`, `Restsekunden 0x800d4832 = 0`.
  - gepatcht: `Status_flg = 0x2c800000 (Timer-Bit 0x08000000: 1)`, Restsekunden **60** bzw. **300**.
  - nach dem Lauf: Restsekunden **56** bzw. **295** (RE2 zaehlt selbst), Timer-Bit weiter 1.
  - beide Bildpuffer aus der VRAM gegen MEINEN Nachbau (`cdtext.py`, alle 90 Zaehlerstaende x 2 Farben probiert): 60er-Stand
    **`" 0^56=95" Farbe 2: 583/583 deckende Pixel gleich -> EXAKT`** (Puffer y 0) und **`" 0^56=98" Farbe 2: 591/591 -> EXAKT`** (y 240); 300er-Stand
    **`" 4^55=48" Farbe 1: 557/557 -> EXAKT`** / **`" 4^55=45" Farbe 1: 549/549 -> EXAKT`** (naechstbester Kandidat jeweils nur 85-94 %). Lage x 104,
    y 32, 14x14-Schrift aus TEX.TIM, Rot unter 60 s, Gruen ab 60 s, die letzten Stellen 98/95 zweier aufeinanderfolgender Bilder.
  - Damit ist (transitiv, Pixel fuer Pixel) die Port-Anzeige = mein Nachbau = RE2.
* Die Nutzer-Spielstaende sind unversehrt: alle 11 `SLUS-00748*` in `%LOCALAPPDATA%/DuckStation/savestates` md5-gleich der Sicherung der
  Nachbesserung (`nb6/re2cap/nutzer_backup`), Zeitstempel unveraendert (juengster 2026-10-18 12:42). (Die Fensterfotos `fenster_*.png` der
  Nachbesserung zeigen nicht RE2, sondern andere Fenster — der Beleg ist allein die VRAM, oben selbst gelesen.)

(weitere Laeufe folgen)

## 2. Gates

### G1 RE-Gate (eigene Disassembly / eigene Byte-Lesung; `re2_disasm.py` auf `info/re2leon/PSX.EXE`, Rohbytes der RE2-RDTs)
1. **Countdown-Zweig der RE2-Hauptschleife** (selbst disassembliert): @0x80025de4..@0x80025e00 `lw v1,-1064(v1)` (0x800cfbd8) / `lui a0,0x800` /
   `and v0,v1,a0` / `beq v0,zero,0x80026364` (nur mit Timer-Bit 0x08000000); @0x80025e0c..@0x80025e1c System_flg 0x800cfb74 `andi 0x8` /
   `beq v0,zero,0x8002608c` / Verzoegerung `addiu a2,zero,1` (Farbe 1); @0x80026094 `slti v0,v0,30`; @0x800260a4 `lw 0x800c3a8c` (Vorbild);
   @0x800260b8..@0x800260d8 `addu s6,zero,zero` / `addiu s7,zero,8` / `lhu`+`addiu -1`+`sh` auf `-31158(s3+0x10000)`; @0x800260ec `slti v0,v1,60` ->
   @0x800260f8 `addiu a2,zero,2`; @0x800260fc `bltz v1,0x80026284`; @0x80026140 `addiu a1,zero,32`, @0x80026144 `addiu a3,zero,2`,
   @0x80026170/@0x80026174 Format **0x800109a4**, @0x80026188 `addiu v0,zero,9`, @0x800261e8 **`addiu a0,zero,104`**, @0x80026274 `jal 0x8002fa28`;
   abgelaufen @0x80026284..@0x800262a0 Format **0x800109b4**, x 104, y 32, `addiu a2,zero,2`, Platz 0; @0x800262f0..@0x80026320 Status_flg &=
   0xf7ffffff, System_flg |= 4. Zeichenketten (EXE-Datei 0x800 + addr - 0x80010000): 0x800109a4 = `25 32 64 5e 25 64 25 64 3d 25 64 25 64 00` =
   **"%2d^%d%d=%d%d"**, 0x800109b4 = **" 0^00=00"** — **bestaetigt**.
2. **Druck/Schrift**: FUN_8002fa28 `sll a2,a2,4` / `ori a3,a3,0x200` / `or a2,a2,a3` -> attr = Farbe<<4 | Platz | 0x200 (kein 0x4000 ->
   feste Breite); FUN_800316a8 @0x800316e8 `andi v0,v0,0x200` -> FUN_800318a4: @0x800318fc/@0x80031900 SPRT **0x64808080**, @0x8003191c..@0x80031930
   CLUT y = **480** + 2*Farbe, x 256 (`ori 0x10`), @0x80031970/@0x80031974 Leerzeichen 32 nur Vorschub, @0x80031978 **`addiu t2,zero,14`**,
   @0x800319b0/@0x800319c0 **0x38e38e39** (c/18), @0x800319d8..@0x800319e0 **0x000e000e** (14 x 14), uv = ((c%18)*14, (c/18)*14) — **bestaetigt**.
   (Nebenbefund: mit attr-Bit 0x4000 gaebe es eine Breitentabelle @0x8009de34; der Countdown setzt das Bit nicht.)
3. **Ausloeser in den RE2-RDTs** (Rohbytes `info/re2leon/PL0/RDT`): ROOM6170 @0x038C6 `22 01 04 01` + @0x038CA `24 23 2c 01` (300), ROOM7010
   @0x02376 `22 01 04 01` + @0x0237A `24 23 2c 01`, ROOM7030 @0x022AC `22 01 04 01` + @0x022B0 `24 23 78 00` (120), ROOM7000 @0x0191C `22 01 04 00`;
   Save-Handler @0x8005451c: `lbu v1,1(v0)` / `lh a1,2(v0)` / `sh a1,0x47ec(at)` mit `sll v1,v1,1` -> 0x800d47ec + 2*0x23 = **0x800d4832** — **bestaetigt**.
4. **Alarm-Eintrag**: ROOM6040 @0x022E8 `57 00 04 06 11 25 00 80`, @0x022F0 `57 00 05 06 11 25 00 80`; Handler @0x80057e98 `lhu 2/4/6` -> a0 =
   0x0604_0000 | 0x2511 | 0x8000 -> FUN_8005b9f4 `sh a0` (= **0xA511**: MAIN 0x11, SUB-Byte **0xA5** = 0x25 | Bit 7); FUN_8005a444 @0x8005a650
   `srl v1,v0,7` -> DAT_800d75d1, @0x8005a654/@0x8005a658 `srl v0,v0,6` + `andi 1` -> DAT_800d75c9 — **bestaetigt** (Port-Gegenstueck
   re15_bgm_load_sub: `(sub_b>>6)&1` stummt SEQ0, Bit 7 sperrt SEQ#2).
5. **Alarm-Lautstaerke**: ROOM6040 @0x038BA `51 01 00 01 64 40`; Handler @0x80057e20 packt pc[1..5] -> FUN_8005b6f0: Teil (pc[2]) 0 -> Tabelle
   @0x80011608[0] = 0x8005b974: Lautstaerke != 0 und Programm (pc[3]) = 1 -> `sb s2,-15(v0)` = VabHdr + 16*1 + 17 = **ProgAtr[0].mvol = 0x63**,
   Pan -> ProgAtr[0].mpan = 0x3f — **bestaetigt**; der Port ruft `ss_seq_ctl_ex(1,0,1,0x64,0x40)` = dieselbe Semantik (prog_mvol[part-1] =
   vol-1, prog_mpan = pan-1, audio_pc.c ~Z. 3617, vorbestehend). SUB_25-Datei selbst zerlegt: Trailer SEQ#2 @244, VH @824 ("pBAV", 8 Programme,
   6 VAGs), **ProgAtr[0].mvol 0, [1].mvol 0** (in der Datei stumm), SEQ0 ppqn 480 / Tempo 500000 / Schleife Tick 100..30820. `shared_assets/RE2/
   SOUND/SUB_25.BGM` md5 **ef66256d0e847e428b11caedd5c655da** = `info/re2leon/COMMON/SOUND/SUB_25.BGM` (auch der Git-Blob).
* Neue Konstanten im Diff seit Abnahme 5 (Kopf re15_ventilkette_r41.h "Countdown"): RE15_R41C_CD_X 104 / _Y 32 / _SCHRITT 14 / _GLYPHE 14 /
  _SPALTEN 18 / _TAKTE 30 / _ROT_UNTER 60 / _FARBE_GRUEN 1 / _FARBE_ROT 2 / _CLUT_Y0 480 / _ZEICHEN 8 — jede mit @0x... und alle oben selbst
  gefunden; audio_pc.c 0xA5 (@0x022EE) und `51 01 00 01 64 40` (@0x038BA) ebenso. Die PORT-WAHL-Konstanten der Nachbesserung 5
  (RE15_R41C_COUNTDOWN_X/_Y/_FARBE) sind entfernt (grep: nur noch als Geschichte im Kommentar).
* Rate-Woerter (`deferred|tunable|interim|for now|faithful|plausib|TODO|FIXME|vorerst|ungefaehr|geschaetzt|approx`) in den HINZUGEFUEGTEN Zeilen
  des ganzen Spur-Diffs: **0 Treffer**. Neue `getenv` seit Abnahme 5 in engine/platform/include: **keine**.
* Erklaert der Fix den Befund? Mangel 1 (kein Abspielweg, SUB-Lader las nur RE1.5): jetzt Lader-Quelle RE2 nur waehrend des Alarm-Ladens +
  Schalter; gemessen (KB): Abweichung genau ab dem Warntext, Form = SUB_25 SEQ0. Mangel 2 (RE2-Anzeige nicht gefunden): gefunden und
  Pixel fuer Pixel nachgebaut, gegen RE2-VRAM belegt. Mangel 3: Kopf Z. 146-149/162 jetzt "Bit 102 ... UND Bit (9,125) ... Satz 49 = Original
  'It's locked.'" und "Fundort neben der Sicherung ... Zettel AUFRECHT an der Suedwand"; keine Reste ("look around"/"Blatt am Boden" nur als
  Geschichte).

(G2/G3 folgen)

## 3. Tests (gezielt, im Spurbaum; KEINE volle Suite)
* Unit-Gruppe (`<a6>/regex_unit.txt` = Regex der Abnahme 5 + BGM-/Stimmen-Pfade, die audio_pc.c linken oder den BGM-Lader pruefen:
  `unit_rotor_bgm_pin|unit_r35_cut10f0_.*|unit_r39_johnruf1041_.*|unit_r34_plattform_ton|unit_r38_werkstatt11b0_ton|unit_r39_stimmen_.*|
  unit_synchro_stimme|unit_voice_ausreden|unit_zombie_se_ab_10d0|unit_r38_elzaintro_stimme`; 290 Tests, -j 2):
  **`100% tests passed, 0 tests failed out of 290`** (`Total Test time (real) =  87.28 sec`, `<a6>/ctest_unit.txt`).
* Integration: zwei Stroeme (`<a6>/integ.sh`): `^integration_r41_ventilkette(_weg|_alarm)?$` und
  `^(integration_r41_ventilkette_weg_elza|integration_r41_ventilkette_b|integration_r41_ventilkette_sperre|integration_r35_cut10f0)$` (laufen).

## 4. Urteil je Nutzer-Punkt
(folgt)

## 5. Maengel
(folgt)

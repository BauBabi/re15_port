# Runde 38 — Spur H "elzatoene": Abnahme 1 (unabhaengig, nach Nachbesserung 1)

Datum: 2026-10-16. Baum `.claude/worktrees/r38_elzatoene`, Zweig `r38/elzatoene`, Stand f279bb70, Basis e2cc7673.
Pruefer: Abnahme 1 (kein Code geaendert; nur diese Datei committet). Messordner (Scratchpad, ausserhalb des Baums) `ab1h/`.

**Ergebnis: BESTANDEN.** P1, P2, P3 erfuellt; Gates halten; Maengel der Abnahme 0 (1-4) behoben und selbst nachgemessen. Hinweise H1-H6 nicht blockierend.

## 0. Stand / Bau
- `git log master..HEAD`: 19 Commits (1468730f .. f279bb70), davon Nachbesserung 1: c72f482e, 94abee9d, 5da2b2ee, 1bcff1d8,
  043ee4a9, f279bb70. master ist seit e2cc7673 nur um Doku-/Orchestrator-Commits gewachsen; Spur-Aenderungen =
  `git diff master...HEAD` (27 Dateien).
- Seit dem Stand der Abnahme 0 (8328ae1f): `git diff 8328ae1f HEAD -- re15_port/engine re15_port/platform re15_port/include
  re15_port/shared_assets` = nur Kommentarzeilen (re15_elza_r36.h, re15_elzatoene_r38.h, elza_se_pc.c) -> keine Laufzeit-
  oder Datenaenderung; ELZASE.VBS md5 ba96a66267286e4754c6b5a7481928a4 (14232 B) selbst gemessen.
- Bau: `bash re15_port/tools/local_build.sh configure` -> `LOCAL-BUILD-OK (configure)`; `... build` (EXIT 0) ->
  `ninja: no work to do`, `LOCAL-BUILD-OK (build)`. exe 22:31, md5 5c4341c0e68b6cda1c8e66a3bed0e3be.
- ctest -N: 797 Tests (v0.8.25 = 788, +9 = 7 Unit + 2 Integration der Spur).

## 1. Code-Diff (gelesen, `git diff master...HEAD -- re15_port`)
- `engine/src/elzatoene_r38.c` + `include/re15_elzatoene_r38.h` (neu): Anker-Tabelle (Raum, Datei-Offset, 8-Byte-Signatur,
  Aktion): ROOM1041 @0x02200 `2b 03 00 00 2e 01 00 00` -> Schuss-Ton statt Message_on 3; ROOM1021 @0x020B2
  `40 00 04 22 4e ef 42 e6` -> Raum-SE 4 (Roys Stoehnen). Scan beim Raumladen, Opcode-Haken (nur Aktion 2),
  Message_on-Haken (nur Aktion 1, Aufrufer `pc += 4`). Einziges getenv = `RE15_ELZA_LOG` (Messschiene).
- `scd_vm.c`: 1 Include + 3 Haken-Zeilen, Kommentar "Runde 38 Spur H" (VERTRAG §1.4 erlaubt).
- `gen/re2_elza_bank.inc` + `shared_assets/RE2/ELZASE.VBS` + `tools/r36_l/re2_elza_se_cut.py`: Satz 1 = RE2 ROOM4040 0x0A.
- Nachbesserung 1 (seit 8328ae1f) im Code: nur Kommentare (re15_elza_r36.h, re15_elzatoene_r38.h, elza_se_pc.c) +
  Riegel (test_r38_elzatoene.c teil_re2beleg neu, teil_aufnahme neu; test_r36_elza.c Kommentar) + Werkzeug-Docstring.
- Rate-Woerter im Code-Diff (`deferred|tunable|interim|for now|faithful|plausib|TODO|vermutlich|gedeutet`): siehe §5.

## 2. Gezielte Regression (keine volle Suite)
`ctest --test-dir re15_port/build -R "scd|msg_dialog|text_freeze|synchro_stimme|voice_ausreden|se_voice_machine|leiche|r36_elza|r38_elzatoene|r37_roy|gegnerzahl_raum1041|story_elza|elza_vollstart|elza_zweig|codepanel|lift2030|savepoint|itembox" --timeout 1200 --output-on-failure -j2`
(22:47-22:59, Rechner unter Last durch Parallel-Spuren) -> **`100% tests passed, 0 tests failed out of 70`** (727,75 s),
darunter integration_r38_elzatoene_szene1021 (111,6 s), integration_r38_elzatoene_gunshot1041 (116,0 s),
integration_r36_elza_schuss (95,0 s), alle 7 unit_r38_elzatoene_*.
`ctest -R unit_r38_elzatoene -V`: alle Pruefzeilen `ok`, u. a. aufnahme "Korrelation 0.982", Gegenprobe alter Satz
0.209/0.182/0.107/0.092; re2beleg 13x ok (Handlerwoerter RE2/RE1.5, 5 Sce_em_set in 40A0).

## 3. Messung je Nutzer-Punkt (echte exe, echter Weg, eigene Auswertung)

Laeufer `ab1h/lauf.sh <exe> <name> <fall>` (eigener; Messkopie `re15_pc_ab1h.exe` neben der exe, md5 = re15_pc.exe
5c4341c0..): Spielstand per `probe_r36_elza_karte` (Elza), CONTINUE (`RE15_NO_INTRO=1 RE15_CONTINUE_TEST=1
RE15_CARD_AUTO=1 RE15_CARD_SLOT=0`), `RE15_WINDOW_SCALE=1`, `RE15_STATE_LOG`, `RE15_AUDIO_CAP_SYNC` (1470 Stereo-Frames
je Bild), `RE15_ELZA_LOG`, `RE15_SE_DEBUG=1`, `RE15_FRAMEDUMP`-Serie, Eingabe ab Bild 60 (`RE15_INPUT_SCRIPT_BASIS=spiel`).
- `g1041`: Karte ROOM1031 (-26900,-3650), `RE15_FIRE_AOT=2@40#1031` (Tuer 1031 -> 1041, derselbe Pfad wie das
  Hineinlaufen: re15_aot_fire_slot, main.c:7952), `RE15_INPUT_SCRIPT=U2.5` (Elza geht ins Auto-Feld AOT 8),
  `RE15_EXIT_AT=500#1041`, Framedump 60-260/4.
- `s1021`: Karte ROOM1041 (-23550,16700) + Flags 4:0x60 3:0xC9 4:0xF1 3:0xAD (nach Johns Befreiung),
  `RE15_FIRE_AOT=3@40#1041` (Tuer 1041 -> 1021), `RE15_INPUT_SCRIPT=L0.25,U0.8,R0.25,U6` (ins Auto-Feld AOT 5),
  `RE15_EXIT_AT=1300#1021`, Framedump 760-1020/10.
- Auswertung UNABHAENGIG vom Port-Code (`ab1h/py/psxsnd.py`: eigener VAG-ADPCM-Decoder nach psx-spx, eigener VH-/EDT-
  Leser, Mitschnitt-Zuordnung Bild f -> Takt (Takte-1-exit)+f, normierte Kreuzkorrelation per FFT). Eigene Dekodierung
  trifft die sha1 der Spur: ELZASE Satz 1 = RE2 4040/0A `994b137d4007` (5824 B, Ton vol 110 center 84 shift 57 min 66),
  alt 40A0/08 `e552c5d77eed`, ARMS07 SE0 `76d95b2b5b52`, ARMS03 SE0 `4769cc0478f3`, ARMS08 SE0 `42bbd9b31614`.

### P1 — ROOM1041 "*gunshot*" ohne Schuss-Ton -> **erfuellt**
| Beleg | Wert |
|---|---|
| elza.log | `F40 [elzatoene] ROOM1041: 1 Anker armiert`, `F83 [elzatoene] ROOM1041 Anker @0x02200 -> Aktion 1` |
| debug.log | `[fire-aot] slot=2 at F40 (Raum 1031)`, `[room] PC loaded room1041.rdt`, `[se] Stimme: se=1 layer=0 vag=1 note=66 fine=57 center=84 vol=110 pitch=0x5f3 (16397 Hz) -> SE-Stimme -16 (SPU 0)` (genau einmal) |
| state.log | Nachricht 3 in KEINEM Bild offen (`fsm=1 id=3` 0x); msg 2 "What was that!?" F144..F242 (99 Bilder, = Abnahme 0 alt/neu) |
| Mitschnitt (eigene Auswertung) | F70..F82 Spitze 1644..3736 (Raum/Schritte), **F83 Spitze 13114 / RMS 3462**; Kreuzkorrelation ELZASE Satz 1 an F83 **0,913** (Versatz 0,00), RE1.5-Redhawk 0,898, Gegenprobe F23 0,072; Trennung: Schuss-RMS 2618..3185 gegen Rest 857..1718 (F83..F86) |
| Bild | `ab1h/g1041_sheet.png` (F72/80/84/88/100/120/140/148/160/180/200/240): kein Textfenster "*gunshot*", ab F148 "Elza: What was that!?", Elza dreht sich um |

### P2 — Roy stoehnt beim Zugehen auf John (ROOM1021) -> **erfuellt**
| Beleg | Wert |
|---|---|
| elza.log | `F788 [elzatoene] ROOM1021 Anker @0x020B2 -> Aktion 2` (einmal) |
| debug.log | `[se] Stimme: se=4 layer=0 vag=4 note=64 fine=0 center=88 vol=127 pitch=0x400 (11025 Hz) -> SE-Stimme 5 (SPU 21)` direkt vor `[scd] Plc_dest(slot=2 mode=0x04 dest=(-4274,-6590)) -> state4/sub4 (Sub-VM-WALK)` |
| Mitschnitt (eigene Auswertung) | snd1 SE 4 an F788: Abschnitt ab 600 ms **1,000** (Versatz +1,00 Bild), ab 300 ms 0,988, ab 0 ms 0,253 (Anfang unter Johns Zeile), Gegenprobe F728 0,055..0,159. Getrennt (Skalierung 0,250): F789-F797 5..23 dB UNTER Johns Sprachzeile msg 3, F798-F800 +7..+14 dB, **F801-F836 allein** (Stoehnen-RMS 450..2218, Rest <= 85) |
| Bild | `ab1h/s1021_sheet.png` (F760..F1020/10): F760-F800 "John: Roy, it's me... You do recognize me, right?", ab F790 geht Roy (POLICE) den Gang hinunter auf John/Elza zu — das Stoehnen faellt in den Gangbeginn |

### P3 — Johns Schuesse in ROOM1021 -> **erfuellt** (Ton gemessen; Wahrnehmung s. §4)
| Beleg | Wert |
|---|---|
| elza.log | `F939 [elza] ROOM1021 Anker @0x020CE -> Aktion 2 (Takt 935)`, `F969 ... @0x020D6 -> Aktion 2 (Takt 965)`, `F969 ... @0x020DE -> Aktion 3` (Roy) — 151 Bilder nach dem Gangbeginn (F788) |
| debug.log | zweimal `[se] Stimme: se=1 layer=0 vag=1 note=66 fine=57 center=84 vol=110 pitch=0x5f3 (16397 Hz) -> SE-Stimme -16`, danach `se=5 ... (11025 Hz) -> SE-Stimme 5` (Roy) |
| Mitschnitt, Pegel | F930..F938 RMS 0 (Stille), **F939 RMS 2576 / Spitze 7062**, F940-F943 RMS 2309..3134; F965..F968 RMS 0, **F969 RMS 2576 / Spitze 7062** |
| Mitschnitt, Identitaet (eigene Auswertung, 0,3 s) | F939: ELZASE Satz 1 **1,000**, RE1.5 Super Redhawk ARMS07 SE0 **0,982**, Browning ARMS03 0,166, M870 ARMS08 0,137, alter Satz 40A0/08 0,135, Gegenprobe F879 0,000. F969: 0,985 / **0,968** / 0,162 / 0,143 / 0,143. Volle Laenge (0,62 s): F939 1,000 (Rest-RMS 1 = allein), F969 0,921 (Roys SE 5 ab F970 darueber, ab 600 ms 1,000) |
| Lautheit (eigenes A-Filter, lauteste 100 ms) | Schuss 1 -24,2 dB(A), Johns Zeile -18,5 dB(A), Stoehnen -27,7 dB(A) — Schuss in Stille, kein Ueberdecken |
| Bild | `ab1h/s1021_sheet.png` F940-F990: Kamera Cut 10 hinter Roy, John verdeckt, kein Muendungsfeuer (Original sub02 @0x0205C `29 0a`, wie Spur §8.2) |
| Johns Waffe | Lauf `neu_s1021_frueh` (Framedump 260-760/20, `ab1h/s1021_frueh.png`, Ausschnitt `ab1h/john_zoom.png` F320/F340/F360): John haelt eine PISTOLE in der rechten Hand ("John: No way, this can't be..."; danach "Elza: John, shoot! ...") -> RE2-Entsprechung Pistolenschuss (nicht Kendos Schrotflinte 1010/0x0C) passt |

Der Ton bei Johns Schuessen ist die Aufnahme des RE1.5-Super-Redhawk-Schusses (Waffen-Id 7, ARMS07: inventory_common.c
s_wpn_props "7 REDHAWK"; enemy_ai_re2_zombie.c Tabelle "7 Super Redhawk (.44)"), also ein Schuss, wie ihn das Spiel selbst
fuer eine Spielerwaffe abspielt — im echten Weg, in beiden Feuer-Bildern +1, in stiller Umgebung.

## 4. Pruefung der Nachbesserung 1 (Maengel 1-4 der Abnahme 0)

### 4.1 Mangel 1 (RE2 Art 2 = Partner, Art 3 = Gegnerfeld) -> **behoben, selbst disassembliert**
Werkzeug `.claude/skills/re15-psx-disasm/scripts/re2_disasm.py` (info/re2leon/PSX.EXE, md5 09a9b642..) bzw.
`re15_disasm.py` (info/Re1.5/PSX.EXE md5 b55fdaa5.., STAGE1.BIN):
1. **RE2 Opcode-Tabelle @0x800A74C8** (Index = Opcode): [0x2E] = 0x80055904, [0x36] = 0x80056428, [0x44] = 0x8005714c;
   [0x00]/[0x1E]/[0x1F]/[0x20] = derselbe Nop-Handler (passt zur RE2-Opcode-Liste).
2. **RE2 Work_Set 0x80055904**: @0x80055920 `lbu v1,1(v0)` (Art), @0x80055924 `lb a1,2(v0)` (Index), @0x80055930
   `addiu v1,v1,-1`, @0x8005594c `lw v0,4592(at)` = Tabelle @0x800111f0 = [0x8005595c, 0x8005596c, 0x8005597c,
   0x80055994, 0x800559b0]; Art 2 -> @0x80055970 `lw v0,-488(v0)` = *0x800CFE18; Art 3 -> @0x8005597c `sll v0,a1,2`,
   @0x80055988 `lw v0,-484(at)` = *(0x800CFE1C + 4*Index). **Stimmt.**
3. **RE2 Se_on 0x80056428**: @0x80056444 `lh a1,4(s0)` (Byte 4 = Art, Byte 5 = Index), @0x80056450 `andi a2,a1,0xff`,
   Tabelle @0x80011458 = [0x80056478, 0x80056488, 0x80056498, 0x800564a8, 0x800564c4] (Index = Art); Art 2 -> @0x8005649c
   `lw v1,-488(v1)` *0x800CFE18; Art 3 -> @0x800564a8 `srl v0,a1,8` .. @0x800564b8 `lw v1,-484(at)`. **Stimmt.**
4. **RE2 Sce_em_set 0x8005714c**: @0x80057188 s4 = 0x800CC1E8, @0x80057204 `lb v0,2(v0)` (Id vorzeichenbehaftet),
   @0x8005720c `addiu a1,v0,2`, @0x80057210 `beq a1,a2(=1)` (Id 0xFF: Gegnerzaehler G+0x3A0B uebersprungen),
   @0x80057234 `sw v1,15404(v0)` mit v0 = s4 + (Id+2)*4 -> 0x800CC1E8+0x3C2C+4 = **0x800CFE18 fuer Id 0xFF**,
   0x800CFE1C+4n fuer Id n. **Stimmt.**
5. **RE2 Lage nur bei Se_on-Byte 3 != 0**: 0x8005ba28 @0x8005ba9c `andi a0,a0,0xff` (= Byte 3), @0x8005baa4 `sw a0,24(sp)`,
   @0x8005bc38 `lhu t1,24(sp)`, @0x8005bc40 `sh t1,18(v1)` (Satz +0x12); Pumpe 0x8005c5e4 @0x8005c658 `lh a1,6(s0)`
   (s0 = Satz+12 -> +0x12), @0x8005c660 `beq a1,zero,0x8005c6cc`, @0x8005c668 `jal 0x8005c970`. **Stimmt.**
6. **RE1.5 Work_set**: Opcode-Tabelle @0x800744A8 (Dispatcher @0x8001659c/@0x8003f104/@0x8003f7a4 `lw v0,0(..)=>PTR_LAB_800744a8`),
   [0x2E] = @0x80074560 -> LAB_80040d2c: @0x80040d4c `lbu v1,1(v0)`, @0x80040d5c `ori v0,zero,0x2`, @0x80040d60
   `beq v1,v0,0x80040db4` -> @0x80040db4..c0 Index*0x1F4, @0x80040dc4 `lui v1,0x800b` / @0x80040dc8 `addiu v1,v1,-13268`
   = 0x800ACC2C (Gegnerfeld); Art 1 @0x80040da4/a8 = 0x800ACA54 (Spieler), Art 3 @0x80040de4/e8 = 0x800B3F98. Keine
   Partner-Art. **Stimmt.**
7. RDT-Bytes selbst gelesen: RE2 ROOM40A0 @0x02564 `44 00 ff 41`, @0x025A0 `44 00 00 23`, @0x0287E `2e 02 00`,
   @0x02896 `36 02 08 01 02 00 ..` (Art 2, Lage-Byte 1), @0x028A2 `3a 00 18 ..`, @0x02CDC `2e 03 00`, @0x02D02
   `36 02 0a 01 03 00`; ROOM4040 @0x00E74 `44 00 01 47`, @0x014E4/@0x01528 `36 02 0a 00 03 01 00 00 00 00 00 00`,
   @0x014F0 `3a 00 18 ..`. RE1.5 ROOM1021 (md5 480d807b..) @0x01DF6 `44 00 49 40 ..` (John Id 0), @0x01E0A `44 01 47`
   (Roy), @0x020C2 `2e 02 00 00`, @0x020CE/@0x020D6 `3f 02 07 00`, @0x020DE `3f 01 07 00`; ROOM1041 (md5 39e286e8..)
   @0x02200 `2b 03 00 00 2e 01 00 00`, msg 3 @0x022FB `04 00 5f 43 51 4a 4f 44 4b 50 5f 04 01 01 31` (15. Byte `31`,
   Dossier jetzt richtig). **Stimmt.**
8. **Zensus (eigener, grober Byte-Scan** `ab1h/py/zensus.py`: Se_on `36 bb ss ll aa ii` (bb <= 3, aa <= 4, ii <= 0x20)
   +-64 B um ein ESP `3a 00 18`, alle PL0-RDTs): Art 3 nur in ROOM1010 (@0x0268E Satz 0x0C, Kendo) und ROOM4040
   (@0x014E4/@0x01528/@0x01558 Satz 0x0A, @0x01502/@0x01546/@0x01568 Satz 0x0B; ohne ESP-Fenster in 4040 zusaetzlich 0x0A
   @0x01578/@0x01668, 0x0B @0x01588); Art 2 in 40A0 (0x08) und 5000 (0x0D); Art 1 in 2160/4040; Art 0 in
   1120/4090/6020/6040. Deckt sich mit der Spur-Tabelle §8.2 (genau ein Pistolenschuss aus dem Gegnerfeld: 4040 0x0A).
9. RE1.5 ROOM1021 hat im Skriptbereich (< 0x4000) KEIN Se_on-Muster -> Original stumm, RE2-Fallback (Beta -> Retail)
   begruendet.

### 4.2 Mangel 2 (Ursache P3 + Wirksamkeit) -> **behoben, Kernaussagen selbst nachgemessen**
Eigene Dekodierung, beide Wellen wie gespielt (Tonhoehe aus dem Ton-Satz, linear auf 44,1 kHz), 0,3 s, +-400 Versatz
(`ab1h/py/ident.py`, `klang.py`), gegen jeden RE1.5-Waffensatz ARMS01..ARMS14 SE 0 (beide Lagen; kurze Wellen ARMS01/09/0A/0B mit 145..219-ms-Fenster; ARMS00 = unbewaffnet, Welle 56 Abtastwerte):
| Vergleich | eigene Messung | Spur |
|---|---|---|
| NEU (4040/0A) vs RE1.5 ARMS07 SE0 (Super Redhawk, Waffe 7) | **0,992** | 0,982 |
| NEU vs alle anderen ARMS SE0 | <= 0,156 | — |
| RE2 ARMS05 SE0 (Magnum) vs RE1.5 ARMS07 SE0 / vs NEU | 1,000 / 0,992 (sha1 verschieden: d82c2b788c98 / 76d95b2b5b52) | "= RE2 ARMS05" |
| ALT (40A0/08) vs alle ARMS SE0 (01..14) | **<= 0,225** (ARMS02 0,225, ARMS03-06 0,195, ARMS07 0,113, ARMS08 0,099, Rest <= 0,143) | <= 0,209 |
| Mitschnitt F939 / F969 vs ARMS07 SE0 | **0,982 / 0,968** | 0,982 / 0,968 |
| Spektral-Schwerpunkt (0..150 ms, Hann) | ALT 1266 Hz; NEU 797; Redhawk 829; Handgun-Familie ARMS03-06 898; ARMS02 861; M870 636; ARMS13 533 | "heller" |
| Anteil >= 2,5 kHz | ALT -8,0 dB; Waffen -10,6..-16,2 dB; NEU -12,5 | ALT -6,0 gegen -9,8..-14,5 |
| Anteil <= 250 Hz | ALT -5,0 dB; Redhawk -5,2; NEU -5,2; Handgun -3,7; ARMS02 -2,6; M870 -3,4 | ALT -5,3 gegen -2,5..-4,2 |
Ergebnis: robust bestaetigt sind (a) der alte Ton ist keine Waffenaufnahme des Spiels, (b) der neue Ton ist die
Super-Redhawk-Aufnahme (in RE2 die Magnum-Aufnahme), (c) der alte Ton ist heller als jeder Waffenschuss. Pegel, Zeitpunkt
(F939/F969, Stille davor) und Weg sind in Abnahme 0 und hier gleich gemessen. Damit aendert der neue Stand genau den
einen gemessenen Unterschied; die Wahl ist ueber RE2 hergeleitet (4.1). Wahrnehmung selbst ist nicht maschinell messbar
(Spur sagt das selbst). Nicht robust: "Bassanteil 1,1 dB unter dem schwaechsten" (eigene Messung: ALT -5,0 dB liegt
UEBER Redhawk/NEU -5,2 dB) — Hinweis H2.
Alternative Ursachen geprueft: Android baut platform/pc/src/*.c + main.c (platform/android/jni/CMakeLists.txt:45/48)
-> derselbe Haken re15_pc_elza_se_init auch dort; Pakete v0.8.25 tragen ELZASE.VBS (selbst gelesen:
release/pkg-win und pkg-linux re15_port_v0.8.25/shared_assets/RE2/ELZASE.VBS, 13144 B, md5 8e996ef2 = master).

### 4.3 Mangel 3 (Riegel re2beleg selbstbestaetigend) -> **behoben**
`teil_re2beleg` prueft jetzt Maschinenwoerter beider EXEs (Werte = meine Disassembly 4.1: 0x8c42fe18 @0x80055970,
0x8c22fe1c @0x80055988, 0x8c63fe18 @0x8005649c, 0x8c23fe1c @0x800564b8, 0x80420002/0x24450002/0xac433c2c,
0x86050006/0x10a0001a/0x0c01725c, RE1.5 0x34020002/0x10620014/0x3c03800b/0x2463cc2c) und zaehlt Sce_em_set ohne
Id-Filter (5 = 4 Partner + Alligator). Eine RE1.5-Lesart von Art 2 in RE2 faellt durch. Neuer `teil_aufnahme` trennt
(neu 0,982 >= 0,95; alt <= 0,209 <= 0,35) — mit dem alten Satz in ELZASE.VBS faellt er durch.

### 4.4 Mangel 4 (falsche Behauptung in Code/Werkzeug/Dossier) -> **behoben**
`grep -i "alligator-schleife|kein schuss|am alligator|alligator-ton|alligator-welle"` ueber elza_se_pc.c,
re15_elzatoene_r38.h, re15_elza_r36.h, test_r36_elza.c, test_r38_elzatoene.c, re2_elza_se_cut.py: 0 Treffer; Dossier nur
noch in als FALSCH markierten Zitaten (§2.3). Neuer Wortlaut (Art 2 = Partner, Annette Art 3, Redhawk-Aufnahme) deckt sich
mit 4.1/4.2.

## 5. Gates

### 5.1 RE-Gate
- Konstanten im Diff (Code): Anker-Offsets/Signaturen ROOM1041 @0x02200 und ROOM1021 @0x020B2 (Bytes selbst gelesen,
  4.1 Nr. 7), SE 4 mit @0x80102298/@0x8010229c (STAGE1.BIN FUN_801021f8, Tabelle @0x8011f890[2] = 0x801021f8, selbst
  disassembliert: @0x80102218 `bne v0,zero` (nur Eintritt), @0x80102278 `andi v0,v0,0x3` / @0x8010227c `bne` (1/4),
  @0x8010228c `andi v0,v0,0x1` / @0x80102290 `beq` (Delay `ori a0,zero,0x5`) / @0x80102298 `ori a0,zero,0x4` /
  @0x8010229c `jal 0x800453d0`), Satz ROOM4040 0x0A mit EDT/Ton/VAG-Offsets (Bytes + sha1 selbst). Gekennzeichnet:
  NUTZER-VORGABE (Stoehnen immer, kein 1/4-Wurf; Ton statt Text in 1041 = VERTRAG §0 "statt Platzhalter"), PORT-WAHL
  (SE 4 statt 5, weil SE 5 = Roys Sterbe-Ton @0x80106da8). snd1 SE 4 (`00 00 5b 15`, VAG sha1 a5234f0ba034) ist in
  ROOM1000/1010/1020/1060/10D0 bytegleich (selbst gelesen) = das Gang-Stoehnen der Zombie-Raeume.
- Stichproben selbst disassembliert (Pflicht 3, gemacht 6): RE2 Work_Set, Se_on, Sce_em_set, Lage-Pumpe; RE1.5
  Work_set; STAGE1 FUN_801021f8 — alle zitierten Adressen enthalten das Behauptete (4.1).
- Rate-Woerter: Code-Diff 0 Treffer (`deferred|tunable|interim|for now|faithful|plausib|TODO|vermutlich|gedeutet|approx`);
  Dossier 2x "gedeutet", nur fuer die NICHT uebernommene RE2-Folge-Welle (OFFEN 2, Hinweis H1). Einziges neues getenv =
  `RE15_ELZA_LOG` (Log-Pfad, kein Verhaltensschalter).
- "Fix erklaert den Befund": die Spur hat die einzige gemessene Abweichung des v0.8.25-Stands (die Aufnahme) benannt;
  eigene Messung bestaetigt sie (4.2) und zeigt, dass der neue Stand genau sie ersetzt. **Haelt** (mit der von der Spur
  selbst genannten Grenze: Wahrnehmung ist nicht maschinell messbar).

### 5.2 Vertrag
- Pfade (`git diff master...HEAD --name-only`): keine release/, platform/android/, shared_assets/PSX/, keine
  CMakeLists.txt, keine RELEASE_NOTES (grep, exit 1). Registrierung nur tests/unit/probes/r38_elzatoene.cmake.
  shared_assets/RE2/ELZASE.VBS = Mini-Bank der Elza-Toene (nicht PSX/*), erzeugt vom Spur-Werkzeug; Satz 0 (AUF)
  unveraendert (EDT `00 00 04 16`, VAG sha1 e12a441e5282, Ton vol 127/center 89/shift 109/min 67 — master gegen neu
  selbst verglichen), Satz 1 = 4040/0A.
- Gemeinsame Datei scd_vm.c: +7 Zeilen = 1 Include, 1 Scan-Zeile, 2 Haken-Zeilen, 3 Kommentare "Runde 38 Spur H"
  (VERTRAG §1.4). Keine Bank-9-Bits, keine Nachrichten-Ids, keine Item-Ids.
- Riegel je Punkt vorhanden und messend: P1 unit gunshot1041 (mit Gegenprobe ohne Anker: Text 3, 0 Toene) + integration
  gunshot1041; P2 unit stoehnen1021 (Gegenprobe 0x SE 4) + integration szene1021; P3 unit schuss1021/bank/re2beleg/
  aufnahme + integration szene1021 (Kreuzkorrelation >= 0,7 / Gegenprobe <= 0,2). `ctest -R r38_elzatoene`: 9/9 gruen
  (im Regressionslauf). integration_r36_elza_schuss: neuer Bezug (Spitze der 28 Bilder vor Schuss 1, Untergrenze
  3000/1500) — im eigenen Lauf Spitze F909..F936 = 0 -> Schwelle 3000 gegen Schuss 7062/7073; ein fehlender Schuss
  faellt weiter durch.
- Nebenwirkungen: Anker nur fuer Raum-Id 0x1021/0x1041 + Signatur (Leon 1020/1040 ohne Anker, unit signatur); ELZASE
  Satz 1 nur in elza_r36.c (1021-Schuesse) und elzatoene_r38.c (1041); Android baut dieselben Quellen
  (platform/android/jni/CMakeLists.txt:45/48); Zeitform 1041 unveraendert (msg 2 F144..F242 wie Abnahme 0).
- Hoerproben im Baum gegengeprueft: H_belege/h_1021_vorher_schuesse.wav = alter Satz (Korr. 1,000, Redhawk 0,173),
  h_1021_nachher_schuesse.wav = Redhawk 0,982, h_1041_nachher_schuss.wav = Redhawk 0,898.

## 6. Maengel (blockierend)

Keine.

## 7. Hinweise (nicht blockierend, nachpruefbar)

- **H1 RE2-Folge-Welle (Spur OFFEN 2).** In ROOM4040 folgt 4 der 5 Saetze 0x0A (@0x014E4/@0x01528/@0x01558/@0x01578,
  nicht @0x01668) 1-2 Bilder spaeter ein Satz 0x0B am selben Ursprung (Art 3 Index 1, Lage 0): @0x014F0 ESP 0x18,
  @0x01500 `02 00` (Evt_next) + @0x01502 `36 02 0b 00 03 01 ..`; @0x01544 Evt_next + @0x01546; @0x01564/@0x01584
  `09 0a 02 00` (Sleep 2) + @0x01568/@0x01588. In 4090/6020 sitzt die Folge-Welle am Ort des ESP 0x18, fern vom Schuss: 4090 Schuss
  @0x02A4E bei x=-8493 (`d3 de`), ESP @0x02A68 und Satz 0x09 @0x02A9C bei x=4900 (`24 13`); 6020 Schuss @0x00B94 bei
  (-10075,-1800,-5574), ESP @0x00BA4 und Satz 0x0B @0x00BB4 bei (-10546,0,-11540) = Bodenhoehe. Welle 2bf10f4ea91f:
  0,45 s, Schwerpunkt 2342 Hz, Spitze erst bei 153 ms. Das stuetzt die Lesart "Einschlag/Abpraller am Ziel" der Spur;
  sie bleibt eine Deutung ("gedeutet" im Dossier). Nicht beauftragt, im Dossier offen benannt.
- **H2 Dossier §8.2, Tabelle/Text berichtigen.** "RE1.5 ARMS02 SE 0 (Leon, Handgun)" ist falsch: Gegenstand 0x02 = PIPE
  (inventory_common.c:413, Nahkampf, s_wpn_props {0,0}); Leons Startpistole = 0x03 BROWNING HP (Starttabelle
  @0x80074bb8 `01 03 15`, messer_rueckfall.c) = ARMS03 (VAG 4769cc0478f3, identisch in ARMS03..06). "Bassanteil 1,1 dB
  unter dem schwaechsten" ist methodenabhaengig (eigene Messung: alt -5,0 dB, Redhawk/neu -5,2 dB). Die Schlussfolgerung
  (alt = keine Waffenaufnahme, <= 0,225 gegen alle ARMS01..14 SE0; heller) haelt trotzdem.
- **H3 Lage.** Der 1041-Schuss (Ton aus dem Nachbarbereich) klingt ohne Lage so laut wie ein naher Schuss; RE2s
  Weltlage-Schuesse derselben Welle (4090 @0x02A4E, 6020 @0x00B94) haben Lage-Byte 1 (positional). Port-weit offen
  (op_se_on ohne Lage-Pfad, Spur OFFEN 3); der Wortlaut verlangt den Ton.
- **H4** Muendungsfeuer zu Johns Schuessen (RE2 ESP 0x18) nicht gebaut — nicht beauftragt (Spur OFFEN 1).
- **H5 Orchestrator:** RE15_MIN_TESTS +9; Android-APK neu bauen (ELZASE.VBS geaendert, neue Quelle elzatoene_r38.c ->
  GLOB-Cache, Memory reai-v2-android-glob-cache); scd_vm.c op_message_on teilt die Stelle evtl. mit Spur F ("Help").
- **H6** Nutzer-Zusatz ROOM 20A0 (Rolladen vor der Kamera) ist laut AUFTRAG Nachtrag 1 / VERTRAG Spur W zugeteilt —
  nicht Gegenstand dieser Abnahme, hier nicht geprueft.

## 8. Urteil
| Punkt | Urteil | Kernbeleg |
|---|---|---|
| P1 ROOM1041 Schuss-Ton bei "*gunshot*" | **erfuellt** | F83 Anker @0x02200 -> Ton (Korr. 0,913, Spitze 13114), Text 3 nie, msg 2 F144 |
| P2 ROOM1021 Roys Zombie-Stoehnen beim Zugehen | **erfuellt** | F788 Anker @0x020B2, snd1 SE 4 ab F789 (Korr. 1,000), allein F801-F836 |
| P3 ROOM1021 Schuss-Ton bei Johns Schuessen | **erfuellt** | F939/F969 Redhawk-Aufnahme (0,982/0,968) in Stille; RE2-Herleitung Art 3 -> 4040/0A selbst disassembliert |
| Gates | **halten** | Bau ok, gezielte Regression 70/70, RE-Gate (6 Funktionen disassembliert), Vertrag ok, Riegel messend |

**bestanden = true** (keine blockierenden Maengel; Hinweise H1-H6).

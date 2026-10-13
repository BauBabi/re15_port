# Runde 37 Spur N "elzaems" — Abnahme 0 (unabhaengig)

Stand: 2026-10-13, Baum `.claude/worktrees/r37_elzaems` (Zweig `r37/elzaems`, HEAD 07693a62, Basis 99b4b13f).
Pruefer: unabhaengige Abnahme; nichts aus dem Dossier uebernommen, was hier nicht selbst gemessen ist.
Messordner (scratchpad, nicht im Repo): `<scratchpad>/abnN0/<lauf>/` (debug.log, anim.log, fNNNNNN.ppm, Kontaktbogen-PNGs).
Vorher-exe: `<scratchpad>/n/re15_pc_vorher_99b4b13f.exe` (md5 7f9af4799377327d835e994a81688aca = Dossier §3; selbst
geprueft: enthaelt die K-Zeile "[roy] Typ 0x%02X aus %s Sektor ..." und KEIN "[elzaems]" -> Stand Spur K).

**ERGEBNIS: BESTANDEN** — beide Punkte erfuellt, alle Gates halten, keine Maengel. Hinweise H1-H8 (§6) gehen an den
Orchestrator; H1 (Leon ROOM5080 zeigt jetzt die zweite Birkin-Form, wie das Original) gehoert in die Release-Notiz.

## 0. Bau

`bash re15_port/tools/local_build.sh configure && bash re15_port/tools/local_build.sh build` -> `=== LOCAL-BUILD-OK (build)`,
"ninja: no work to do" (Quell-mtimes <= exe-mtime 2026-10-12 16:38:25; letzte Code-Aenderung roy_pc.c 16:38:17;
Test-exe test_r37_elzaems 2026-10-13 22:01). Arbeitsbaum sauber (`git status` leer).

## 1. RE-Gate (selbst disassembliert, `.claude/skills/re15-psx-disasm/scripts/re15_disasm.py`, info/Re1.5/PSX.EXE)

| Behauptung (Code-Kommentar / Dossier) | selbst gelesen | Urteil |
|---|---|---|
| FUN_80022300 waehlt das Archiv ohne Typ-Test per `bltz` auf DAT_800aca3c | `8002232c lw v1,-13764(v1)` (=0x800aca3c), `80022334 bltz v1,0x800223ac`, Delay `addiu a0,v0,-16`; davor nur `lbu v0,8(s2)` (Typ) — kein Typ-Vergleich | haelt |
| Leon-Zweig Datei 0x26 / TOC @0x80072f38, Elza-Zweig Datei 0x27 / TOC @0x80073178 | `8002233c ori v0,zero,0x26`, `80022370 addiu at,at,12088` (0x80072f38); `800223ac ori v0,zero,0x27`, `800223e0 addiu at,at,12664` (0x80073178) | haelt |
| Datei 0x26 = CDEMD0.EMS, 0x27 = CDEMD1.EMS | Dateitabelle @0x8006f440 Eintrag 0x26 Groesse 4732928, 0x27 Groesse 4739072 = Dateigroessen EMD/CDEMD0.EMS / CDEMD1.EMS | haelt |
| einziger Aufrufer = Sce_em_set | `80042328 jal 0x80022300` (im Sce_em_set-Handler) | haelt (Xref nicht erneut voll gezaehlt) |
| SCD-Set = Opcode 0x22 = LAB_8003fdd0, Bank-Tabelle @0x80074664[1] = 0x800aca3c, Maske 0x80000000>>bit | Sprungtabelle @0x800744a8 + 0x22*4 = 0x80074530 -> LAB_8003fdd0 (ghidra1_V2.txt Z.243071); `8003fdd4 lui t0,0x8000`, `8003fdfc addiu at,at,18020` (0x80074664), Wert 1: `8003fe68 srlv v1,t0,a1` + `or`, Wert 0: `srlv`/`nor`/`and`; Tabelle [1] = 0x800ACA3C | haelt |
| TOC-Werte je Typ (Sektor/Laenge) | 0x10 C1 [0,214816], 0x21 C1 [760,70364], 0x27 C1 [893,207236], 0x30 C1 [2015,228460] / C0 [1261,226404], 0x42 C1 [1505,144032] — Werte der debug.log-Zeilen | haelt |
| CRC32 in den debug.log-Zeilen = CRC32 der TOC-Blobs | selbst mit zlib.crc32 gerechnet: C1 0x10 AE292DA8, 0x11 06E1989E, 0x16 56C31FA6, 0x21 921598F6, 0x27 2A8D48B1, 0x30 1145F206, 0x42 91415329, 0x49 285C67D3 | haelt |
| Skriptklammern Set(1,0,v) um den Birkin-Spawn | Rohbytes selbst gelesen: ROOM5080.RDT @0x742 `22 01 00 01`, @0x746 `44 00 30 33 ...` (Sce_em_set Typ 0x30), @0x75A `22 01 00 00`; ROOM3071 @0x3430 `22 01 00 00`, @0x3434 `44 00 30 21 ...`, @0x3448 `22 01 00 01`; ROOM5090 @0x1246/@0x124A/@0x125E dito | haelt |
| Original laedt bei Elza aus CDEMD1, bei Leon aus CDEMD0 (Nutzer-Spielstaende) | eigener Vergleich (re15_ss, Bank-Zeiger +0x170 dir4 / +0x178 dir6 gegen beide TOC-Abschnitte), saubere EXE (@0x80026e4c = 08 00 e0 03): `_6.sav` DAT_800aca3c=80000000, Typ 0x42 dir4 ==C1 True / ==C0 False; `_7.bak` =80000110, 2x Typ 0x27 dir6 ==C1 True / ==C0 False; `_9.bak` (Leon) =00000000, 3x Typ 0x21 dir6 ==C0 True / ==C1 False | haelt |

Diff-Suche nach "deferred|tunable|interim|for now|faithful|plausib|TODO|FIXME|getenv": 0 Treffer in den +Zeilen von re15_port.
Konstanten im Diff: 0x10/0x50/64 (Index Typ-0x10 @0x80022338, 64 TOC-Eintraege), 2048 (CD-Sektor = Einheit der TOC),
Zone 1 / Bit 0 (Tabelle @0x80074664[1]); jede mit Adresse oder Formatbeleg. CRC32 nur fuer die Messzeile (kein Verhalten).
Kein Env-Schalter als Abschluss. **Erklaert der Fix den Befund?** Ja: vorher-exe laedt bei Elza jeden Typ ohne
Archivzeile aus der CDEMD0-Kette (M1/M2/M4/M5 basis), nachher genau den TOC-Blob, den das Original in den RAM legt
(CRC gleich, Spielstand-Vergleich oben). => RE-Gate: **haelt**.

## 2. Messungen an der exe (eigene Laeufe)

Werkzeug: `<scratchpad>/abnN0/mess.sh` (Karte per probe_r36_elza_karte + RE15_CONTINUE_TEST/CARD_AUTO = Lade-Weg des
Spiels; Messkopie re15_pc_abnN_<id>.exe neben der Quell-exe, danach geloescht), Basis-Env RE15_NO_INTRO=1
RE15_WINDOW_SCALE=1 RE15_NOAUDIO=1 RE15_ENEMY_DIAG=1. "neu" = exe dieses Zweigs, "basis" = Vorher-exe (Stand K).

**M1 Elza ROOM3091 (0x42 Sherry-Szene)** — `mess.sh e3091_{neu,basis} <exe> 300 3091 -24731 887 1024 --
RE15_SUBSTART=7@40#3091 RE15_PRESS=cross@70,100,..,1000 RE15_ANIM_TRACE=anim.log RE15_FRAMEDUMP=100-800/100:f RE15_EXIT_AT=900#3091`
* neu: `[elzaems] Typ 0x49 aus EMD/CDEMD1.EMS Sektor 1709 (137200 B) crc 285C67D3`, `[elzaems] Typ 0x42 aus EMD/CDEMD1.EMS
  Sektor 1505 (144032 B) crc 91415329`, `[enemy-diag] actor2 type=0x42 ... clips=14` (basis: keine Archivzeile, `clips=6`).
* Bild (abnN0/blatt_3091_a.png, _b.png, F100..F800): basis = Ada (weisse Bluse, Rock) im Dialog "John: What's your name,
  kid?"; neu = blonde Figur in Rot/Weiss — dieselbe Figur wie im Original-Bild des Dossiers (n5_sherry3091...png, linke
  Spalte); der Blob ist CRC-gleich dem TOC-Blob, der im Original-RAM liegt (Spielstand-Vergleich _6, §1).
* Animation: `tools/check_anim_trace.py` basis und neu je "VERDICT: PASS" (2703 Posen, fc_render == fc_clock, keine
  Spruenge mitten im Zyklus); Clip-Folge Typ 0x42 in beiden Laeufen identisch (F0 c2/52, F1 c1/16, F17 c2/52, F129 c0/22,
  F146 c5/30, ..., F500 c9/1, F642 c1/32, F692 c0/34, F742 c9/1), motion == clip in jeder Zeile.

**M2 Leon ROOM5080 (Birkin 0x30)** — `mess.sh l5080_{neu,basis} <exe> 300 5080 -9950 -18200 2048 -- RE15_SUBSTART=3@60#5080
RE15_FORCE_CUT=1 RE15_ANIM_TRACE=anim.log RE15_FRAMEDUMP=100-900/100:f RE15_EXIT_AT=1000#5080`
* neu: `[elzaems] Typ 0x30 aus EMD/CDEMD1.EMS Sektor 2015 (228460 B) crc 1145F206 - Elza-Archiv (@0x800223ac)`; basis: keine.
* Bild (abnN0/blatt_5080.png): basis = erste Form (Jeans), neu = zweite Form (roter Oberkoerper, nackte Unterschenkel).
* KI: Clip-Folge in beiden Laeufen identisch (14 Wechsel: c0/29, c16/59, c13/65, c3/81, c1, c3, c10/47, c1, c4/81, c11/93
  ...), einziger Unterschied Loco-Clip 1 = 68 (basis) -> 27 Bilder (neu) = CDEMD1-Wert; check_anim_trace beide PASS.
  => Der Birkin-Kampf in 5080 laeuft mit der CDEMD1-Bank ohne Absturz/Schleifenfehler (Hinweis H1).

**M3 Elza Neuspiel ueber den echten Weg** (Titel -> NEW GAME -> Figurenwahl rechts -> Montage 1241 -> Abbruch -> ROOM1031;
Rezept test_elza_vollstart.cmake mit Messkopie, LAUFZEIT 160): Riegel-Ausgabe `elza=1 pl04=1 ... elza_vollstart OK`;
debug.log: `[room] PC loaded room1031.rdt`, `[enemy] RE2 EM016 loaded ...`, `[elzaems] Typ 0x16 aus EMD/CDEMD1.EMS Sektor
427 (213904 B) crc 56C31FA6`, `[enemy] Hybrid EM16: RE1.5-Geometrie (15 Meshes) unter RE2-Rig` — der Hybrid (RE2-KI-
Standard) liest im echten Spielweg den CDEMD1-Blob.

**M4 RE1.5-Griff Elza ROOM1181 / Leon ROOM1180** — `mess.sh {e1181,l1180}_{neu,basis} <exe> 300 <raum> -3758 -7436 2048 --
RE15_AI_FLAVOR=re15 RE15_ANIM_TRACE=anim.log RE15_EXIT_AT=220#<raum>`
* Elza neu: Lader `[elzaems] Typ 0x10 aus EMD/CDEMD1.EMS Sektor 0 (214816 B) crc AE292DA8`; Spieler im Griff Clip 1 = **25**,
  Clip 6 = **70** Bilder, Zombie Clip 1 = 25 / Clip 9 = 70 (basis: 26 / 65 und 26 / 65). Uebergaenge an denselben Bildern
  (F33/F47/F149).
* Leon 1180: `cmp` anim.log basis gegen neu -> **BITGLEICH**, keine Archivzeile.
* Sonde `test_r37_elzaems.exe griff` selbst gestartet: Elza vorn CDEMD1 Hand10-S12 (-31,-65,-181) / Hand13-S9 (40,-20,22),
  hinten Y 0 / -30; mit Leons Bank -229/-284 bzw. -255/-255; Leon vorn/hinten bitgleich zwischen Weiche und CDEMD0
  (-127/-165, -138/-133). = Dossier-Tabelle §6.1.
* check_anim_trace: in basis UND neu je 4 identische "Spruenge" (Zombies Clip 0 f1, Clip 3 f17; Spieler am Phasenwechsel
  F149) — vorbestehend, nicht durch N (Hinweis H7).

**M5 Kraehe Elza ROOM10C1 / Leon ROOM10C0** (RE1.5-KI, 3000/0, EXIT_AT 600): Elza neu `[elzaems] Typ 0x21 aus
EMD/CDEMD1.EMS Sektor 760 (70364 B) crc 921598F6`, Opfer-Clip 1 = **31**, Clip 2 = **30** (basis 36 / 20); Leon 10C0
anim.log basis gegen neu **BITGLEICH** (2415 Zeilen). Kraehen-Clip-3/4-Spruenge identisch in basis und neu (vorbestehend).

**M6 Leon bytegleich weitere Raeume** (gleiche Karte, basis gegen neu): ROOM3070 (Birkin CDEMD0, 602 Posen), ROOM1140
(Hybrid-Zombies 0x16/0x10/0x11, 2407 Posen): anim.log jeweils **BITGLEICH**, keine `[elzaems]`-Zeile, gleiche Ladezeilen.

**M7 RE2-KI (Standard) bei Elza unveraendert** — Elza ROOM1111 (Zombies 0x11, Hybrid) basis gegen neu: neu liest
`[elzaems] Typ 0x11 aus EMD/CDEMD1.EMS Sektor 105 (214592 B) crc 06E1989E`, anim.log **BITGLEICH** (722 Posen) und alle sieben
Framedumps F50..F350 **bytegleich** -> bestaetigt Dossier §6.6 (Hybrid-Eingaenge in beiden Archiven gleich).

**M8 Weitere Typen / Raeume, nur Lader (neu)**: 3041 `Typ 0x29 aus CDEMD1 Sektor 995 crc DA19C7B1`; 5041 `Typ 0x2B aus CDEMD1
Sektor 1104 crc 33FD7CFA`; 2031 `Typ 0x25 ... crc 40C3DF4E` + Hybrid EM25; 1191 `Typ 0x20 ... crc DE56FC9E` + Hybrid EM20;
11B1 `Typ 0x4B` und `Typ 0x42 aus CDEMD1 Sektor 1505 crc 91415329`; 5081 `Typ 0x30 aus CDEMD1 Sektor 2015`; 3071 und 50F1
`Typ 0x30 aus EMD/CDEMD0.EMS Sektor 1261 - Elza-Bit 0 beim Sce_em_set`; 5091 `RE2 EM036` + `FINALER Birkin`, keine
0x30/0x36-Archivzeile. Alle Laeufe erreichen EXIT_AT (kein Absturz).

**M9 Skriptklammer ueber die TUER (nicht nur CONTINUE)**: Elza ROOM30E1 `RE15_FIRE_AOT=1@40#30E1` (Door_aot_set @0x00A16 ->
Stage 3 Raum 7) -> `[room] PC loaded room3071.rdt`, `[elzaems] Typ 0x30 aus EMD/CDEMD0.EMS Sektor 1261 ... Elza-Bit 0`;
Elza ROOM50D1 `RE15_FIRE_AOT=1@40#50D1` (@0x011FA) -> room50f1, dito CDEMD0. => Der Merkwert je (Typ, Raum) wird auch beim
Raumwechsel ueber eine Tuer im richtigen Raum gemerkt (g_current_room_id beim Sce_em_set bereits der neue Raum).

## 3. Urteil je Punkt (gegen den Wortlaut)

| Punkt | Urteil | Beleg (selbst gemessen) |
|---|---|---|
| 1. Nachtrag 7 "Bei Elzas Szenario stimmt weder das Modell von Roy noch all seine Animationen ... wiederholen sich ... staendig" (Roy durch K; N schliesst dieselbe Fehlerklasse fuer alle uebrigen Typen) | **erfuellt** | Roy: r37_roy 8/8 gruen in diesem Baum (K-Mechanismus unveraendert, nur auf das gemerkte Archiv umgestellt). Fehlerklasse "Elza bekommt Leons Daten": jeder Typ liest jetzt den Blob, den FUN_80022300 liest (M1-M9; CRC = Original-RAM). Sichtbar: 3091 Sherry-Modell des Originals statt Ada (M1), 5081 zweite Birkin-Form (Lader-Zeile + Dossier-Bild), Griff-/Kraehen-Clips mit Elzas Laengen (M4/M5). Keine neue Schleife/kein Clip-Umbruch: check_anim_trace 3091/5080 PASS, Clip-Folgen basis == neu, in 1181/10C1 nur vorbestehende, in basis identische Befunde. |
| 2. Orchestrator-Auftrag N "laedt jeden Gegner-/NPC-Typ wie das Original aus CDEMD1 (FUN_80022300 ...), nicht aus Leons CDEMD0; sichtbar u.a. 0x30, 0x42; Aktions-/Opfer-Banken 0x10-0x18, Kraehe, 0x27, 0x29, 0x2b. Leon bleibt bytegleich." | **erfuellt** (Hinweise H1-H3) | Mechanismus = Original ohne Typ-/Raumliste (RE-Gate §1); Lader-Zeilen fuer 0x10/0x11/0x16/0x20/0x21/0x25/0x27/0x29/0x2B/0x30/0x42/0x49/0x4B (M1-M8, Integrationstests); Banken: Zombie-Aktions-/Opfer-Clips 25/70 (M4), Kraehe 31/30 (M5), 0x27 Opfer-Clip 0 63 (unit_r37_elzaems_opfer27), 0x29/0x2B Lader (M8). Abweichungen vom Wortlaut, jeweils mit Original-Beleg: 3071/50F1 laden wie das Original CDEMD0 (Skriptklammer Set(1,0,0), Bytes selbst gelesen, M8/M9) — die Annahme "anderes Modell in 3071/50F1" des Auftrags war falsch; 5091 bleibt RE2-EM36 (NUTZER-VORGABE 2026-09-12); Leon: ohne Elza-Zeile in 1100 (Test leon1100), anim.log bitgleich in 1140, 1180, 10C0, 3070 (M4-M6), ABER 5080 laedt jetzt wie das Original den CDEMD1-Birkin (Set(1,0,1) @0x742, M2) — H1. |

## 4. Tests

* `ctest -R "r37_elzaems|r37_roy" --timeout 600 -j3`: **100% tests passed, 0 tests failed out of 25** (165 s).
* `ctest -R "elza|birkin|5080|5081|5090|3071|3091|g5_|_ems|emd|hybrid|sherry|victim|affen|maggot|crow|kraeh|griff|flag|sce_em|spawn|game_state" -E "r37_elzaems|r37_roy" -j3`:
  **100% tests passed, 0 tests failed out of 91** (243 s). Zusammen 116 = Zahl des Bau-Agenten. Keine volle Suite (Vertrag).
* Riegel je Punkt vorhanden und messend: toc (TOCs, CRC, Unterschiede je Typ, Hybrid-Eingang), weiche (Zwilling Set(1,0,v),
  Figurwechsel), raeume (Raum-Init am VM, beide Szenarien inkl. Leon 3070/50E0/1100/5080), griff/opfer27 (Mechanik mit
  Werten), 12 Integrationslaeufe an der echten exe (Lader-Zeile mit CRC, Opfer-Clip-Laengen, Leon-Gegenproben).

## 5. Vertrag

* Pfade (`git diff 99b4b13f --name-only`): nur analysis/befunde_runde37/N_* , engine/src/elzaems_r37.c (neu),
  include/re15_elzaems_r37.h (neu), roy_r37.c / roy_pc.c (Spur-K-Dateien, VERTRAG 1.4 fuer N freigegeben), main.c (nur
  Kommentar der K-Zeilen), scd_vm.c (2 Zeilen in op_sce_em_set, "Runde 37 Spur N"), game_state.c (1 Zeile + 2 Kommentar,
  "Runde 37 Spur N" — H4), tests/unit/probes/r37_elzaems.cmake, tests/unit/test_r37_elzaems.c,
  tests/integration/test_r37_elzaems.cmake (`cmake_minimum_required(VERSION 3.16)` vorhanden). Keine release/,
  platform/android/, shared_assets/PSX/, keine Test-CMakeLists. Keine POSIX-Aufrufe in den neuen Quellen.
* Bank-9-Bits / Nachrichten-Ids / Ereignisse / AOT-Slots: keine belegt.
* Griff-/Biss-Code (J), RE2-KI (L), Spawn-Haken (H): nicht angefasst (Diff).
* Zusammenfuehrung: `git merge-tree --write-tree --name-only r37/integration HEAD` -> Konflikte NUR in
  analysis/befunde_runde37/AUFTRAG.md und VERTRAG.md (Dokument-Commit 99b4b13f der Basis); scd_vm.c und main.c
  automatisch zusammengefuehrt (H5).
* Nebenwirkungen beide Szenarien: Leon bitgleich ausser 5080 (M2/M4-M6); Elza RE2-KI-Standard bitgleich im Bild (M7).

## 6. Maengel

Keine.

### Hinweise (kein Mangel, an den Orchestrator)

* **H1 Leon ROOM5080 sichtbar geaendert**: Birkin jetzt zweite Form (CDEMD1) statt erster. Gegen den Satz "Leon bleibt
  bytegleich", aber byte-true: ROOM5080 sub00 `22 01 00 01` @0x742 vor `44 00 30 33` @0x746 setzt Bit 31 von
  DAT_800aca3c, das FUN_80022300 per `bltz` @0x80022334 liest -> Datei 0x27. Gehoert in die Release-Notiz ("Fuer den
  Nutzer" im Dossier nennt es). KI-Ablauf in 5080 unveraendert (M2).
* **H2** Die Auftragsannahme "0x30 anderes Modell in 3071/50F1" widerspricht dem Original (Klammer Set(1,0,0)); der Port folgt
  dem Original (CDEMD0, auch ueber die Tuer M9).
* **H3** 5090/5091: Original zeigt den CDEMD1-Birkin, der Port nach NUTZER-VORGABE (2026-09-12) den RE2-EM36 — unveraendert.
* **H4** game_state.c ist in VERTRAG 1.4 fuer N nicht namentlich genannt; der Haken ist 1 Zeile mit Kommentar
  "Runde 37 Spur N" und noetig (Set(1,0,v) muss den Zwilling erreichen).
* **H5** Merge gegen r37/integration: nur AUFTRAG.md/VERTRAG.md konfliktbehaftet (Integrationsstand nehmen).
* **H6** Neue Quelldatei engine/src/elzaems_r37.c: der Android-Bau cacht die GLOB-Liste (app/.cxx) — beim Release neu
  konfigurieren (Memory reai-v2-android-glob-cache).
* **H7** Vorbestehend, nicht N: check_anim_trace meldet in 1181 (Griff-Phasenwechsel F149, Zombie-Clip 0/3) und 10C1
  (Kraehen-Clip 3/4) Rampenspruenge — identisch in der Vorher-exe.
* **H8** Nicht geprueft (ausserhalb "laden wie das Original"): ob die Elza-Opfer-Clips 8..10 von 0x2B (CDEMD1 11 statt 8
  Clips) im Original von der 0x2B-KI angewaehlt werden — die Port-Zuordnung re15_victim_clip_map waehlt fuer 0x2B weiter
  0..7; 0x29s Opfer-Bank (4 -> 2 Clips) wird von der Port-KI gar nicht benutzt (aca58-Griff nicht portiert). Moegliche
  Folgearbeit, kein Defekt dieser Spur. Ebenso: 0x40 in 11B1 traegt im Original-RAM (_6) Banken aus dem Raum-RDT, nicht
  aus einem Archiv (archivunabhaengig, vorbestehend).

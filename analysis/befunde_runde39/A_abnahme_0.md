# Runde 39 Spur A "stimmen" — Abnahme 0 (unabhaengig, 2026-10-18)

Baum `.claude/worktrees/r39_stimmen`, Zweig `r39/stimmen`, HEAD 90d6395e, Basis (merge-base master) 3e49ff87.
Pruefer: Abnahme 0 (Subagent). Nichts im Code geaendert; dieser Bericht ist die einzige Aenderung.
Bericht wird FORTLAUFEND geschrieben (Sitzungslimit).

## 0. Umfang des Diffs (git diff 3e49ff87 --stat; `git diff master` zeigt zusaetzlich 4 doc/release-Commits von master
als "geloescht" — das ist der Basis-Versatz, nicht die Spur)
9 Dateien, 1399 Zeilen, nur Zugaenge:
- NEU engine/src/stimmen_r39.c (82), include/re15_stimmen_r39.h (66)
- platform/pc/src/audio_pc.c: +4 Zeilen (include, Haken `re15_stimmen_r39_stamm` vor dem Rekorder-Haken, `re15_stimmen_r39_fehlt` nach den Pfadversuchen)
- tests: unit/test_r39_stimmen.c, unit/probe_r39_stimmen_lader.c, unit/probe_r39_stimmen_auswertung.c, unit/probes/r39_stimmen.cmake, integration/test_r39_stimmen.cmake
- Dossier analysis/befunde_runde39/A_stimmen.md
Keine release/, platform/android/, shared_assets/PSX/, keine Test-CMakeLists (Registrierung ueber den GLOB
tests/unit/CMakeLists.txt:4060 probes/*.cmake). Keine Nutzer-Datei unter synchro/ geaendert.

## 1. Bau
`bash re15_port/tools/local_build.sh configure` + `build`: LOCAL-BUILD-OK; `ninja: no work to do` (Baum war vom Bau-Agenten
gebaut; exe 03:03, letzter Code-Commit an stimmen_r39.c/audio_pc.c = bc3b3585 03:0x, danach nur Tests/Dossier —
siehe §1.1). Unit-Riegel: `ctest -R unit_r39_stimmen` -> 2/2 Passed (tabelle 0,02 s, lader 0,15 s).

### 1.1 Stand der exe
`git log -- stimmen_r39.c re15_stimmen_r39.h audio_pc.c`: letzter Code-Commit bc3b3585 (03:03:54); exe 03:03, `ninja: no
work to do` beim Nachbau -> die exe entspricht dem HEAD-Code (spaetere Commits d3899208/ae74e92c/90d6395e = Tests + Dossier).

## 2. RE-Gate (selbst nachgeschlagen)

### 2.1 Konstanten im Diff
| Konstante | Kennzeichnung | Selbst geprueft |
|---|---|---|
| Nachrichten-Ids 1050 22/23/24 | Port-Ids (re15_adaruf.h Z.102-104) | ja: `#define RE15_ADARUF_MSG_RUF 22 / LEON_A 23 / LEON_B 24`, Installer nur ROOM 0x1050 (adaruf_1050.c:217) |
| Nachrichten-Ids 1150 22..29 | Port-Ids (irons_tod_1150.c:695-698) | ja: k_meldungen {0x1150, 22..29}; Programm nur RE15_IT_RAUM_1150 = 0x1150 |
| Nachrichten-Ids 11B0 20/21 | Port-Ids (re15_szenenleon_r36.h:69-70) | ja: `RE15_SZL_MSG_HOLD 20 / FOLLOW 21`; RE15_SZL_RAUM 0x11B0 (Leon), 11B1 bleibt Original |
| Nachrichten-Id 1171 3 | Original: ROOM1171.RDT sub02 @0x01586 | ja: `xxd -s 0x1586` = `2b 03 00 00`; @0x14BA `2b 00 00 00`, @0x151E `2b 01 00 00`; RDT-Header: mainScd 0x11D4, subScd 0x1364..0x179C -> alle drei im sub-SCD, keine weiteren `2b xx 00 00` im SCD-Bereich |
| Dateinummern 9-11 / 15-21 / 13-14 / 2 | NUTZER-VORGABE (Dateien e2cc7673) | ja: `git ls-files synchro/STAGE1` zeigt genau diese Dateien; Inhalt selbst per Spracherkennung/Stimmlage geprueft (§3) |
| Sperre 11B0 msg 13/14 | Original: ROOM11B0.RDT sub03 @0x0126E/@0x01298, main00 @0x0106C | ja: `2b 0d ff ff` an beiden Stellen (davor @0x126A `51 01 01 00` Sce_key_ck, @0x1294 ebenso), @0x106C `2c 07 01 31 ...` |
| Sperre 1171 msg 2 | Zensus (Message_on nur 0/1/3) | ja (s.o.) |
| Form (Tabelle Raum+Nachricht -> Datei) | RE2 xa_on @0x80058028 | ja, s. 2.2 |
| Pegel (4x-Deckel) | unveraendert (audio_pc.c Level-floor, target 14745, `gain_q8 > 4*256` -> 4x) | ja, Z.2360-2381, nicht Teil des Diffs |
Guess-Tells (`deferred|tunable|interim|for now|faithful|plausibel|TODO|FIXME|getenv`) im Code-Diff: KEINE. Kein Env-Schalter.

### 2.2 Drei+ Adressen selbst disassembliert (ghidra_re2_Leon.txt, richtige Binaerdatei = RE2 Leon, da Form = RE2)
- Dispatch-Tabelle: `800a7574 8c 4a 05 80 addr LAB_80054a8c` (Eintrag 0x2B) und `800a762c 28 80 05 80 addr LAB_80058028`
  (Eintrag 0x59): Basis 0x800a74c8 = 0x800a7574 - 0x2B*4 = 0x800a762c - 0x59*4 -> konsistent, 0x59 ist xa_on, 0x2B message_on.
- xa_on @0x80058028: `80058038 lw v0,0x1c(s0)` (Skript-PC), `80058040 lbu a0,0x1(v0)`, `80058044 lhu a1,0x2(v0)`,
  `80058048 jal FUN_800129b4`, `80058060 ori v0,v0,0x20` (DAT_800cfbd8 |= 0x20), `80058070 addiu v0,v0,0x4` -> PC+4.
  Stimmnummer = eigener 16-Bit-Operand des xa_on, NICHT die Nachrichtennummer. Behauptung bestaetigt.
- message_on @0x80054a8c: `80054aa0 lbu a2,0x2(v0)`, `80054aa4 lbu a1,0x3(v0)`, `80054aa8 lhu s0,0x4(v0)`,
  `80054aac addiu v0,v0,0x6`, `80054ac0 jal FUN_8002fe38` — kein Aufruf von FUN_800129b4 (XA). Behauptung bestaetigt.
- RE2 RDT-Beispiele: info/re2leon/PL0/RDT/ROOM1010.RDT @0x2380 `59 00 0a 00 2b 00 04 00 00 00` (Stimme 10, Text 4);
  ROOM1130.RDT @0x1150 `59 00 15 00 2b 00 11 00 00 00` (Stimme 21, Text 17). Bestaetigt.
- RE1.5-RDT-Bytes (ROOM1171/ROOM11B0) s. 2.1.
Urteil RE-Gate: haelt. Die Zuordnung selbst ist NUTZER-VORGABE (Dateien) und als solche gekennzeichnet; die FORM ist
RE2-belegt; keine Verhaltenskonstante ohne Quelle.

## 3. Inhalt der Nutzer-Dateien (NUTZER-VORGABE) — selbst gemessen, unabhaengig vom Dossier

Werkzeuge (Scratchpad, nicht im Repo): Windows-Spracherkennung System.Speech en-US (Diktat + Zwangswahl mit Alternativen),
Stimmlage (F0-Median per Autokorrelation), eigenes MFCC(12)+DTW (numpy) gegen die Roh-Exporte synchro/unused/.

| Datei | Dauer / F0 / rms | Mein Befund | Zeile |
|---|---|---|---|
| 1050/main09 | 4,28 s, F0 340 Hz (weiblich), rms 0,045 | Diktat unlesbar; Stimmlage = Ada (main10/11: 140-143 Hz = Leon); Ada-Rohtake 22:34:51 diktiert "Pele old anyone please get me out of your" = "Hello, anyone? Please get me out of here" (Wahl 0,744/0,713); einzige Frauen-Zeile der Szene | msg 22 bestaetigt |
| 1050/main10 | 1,87 s, 143 Hz | Diktat "Another civilian survivor", Wahl 0,939 | msg 23 bestaetigt |
| 1050/main11 | 1,36 s, 140 Hz | Diktat "I have to help her", Wahl 0,871 | msg 24 bestaetigt |
| 1150/main15 | 0,72 s | Diktat "So are" (1 Silbe); einzige Ein-Wort-Zeile = "Sir!"; Reihenfolge 15..21 = 22..28 | msg 22 (Ausschluss + Reihenfolge) |
| 1150/main16 | 5,75 s, 189 Hz | Wahl "Sir, the communication system can't be fixed! ... patrol car ..." 0,849 | msg 23 bestaetigt |
| 1150/main17 | 2,18 s, 183 Hz | Wahl "I came to get you, come with me!" 0,683 | msg 24 bestaetigt |
| 1150/main18 | 6,05 s, 88 Hz, rms 0,0040 | Diktat "Leon \| I1 proud to have an officer as dependable as you" | msg 25 bestaetigt |
| 1150/main19 | 5,22 s, 90 Hz, rms 0,0042 | Diktat "But I I'm not going to make that I'm sorry", Wahl 0,918 | msg 26 bestaetigt |
| 1150/main20 | 6,05 s, 98 Hz, rms 0,0044 | Wahl "Please, one last favor. Look after yourself and the others who survived." 0,893 | msg 27 bestaetigt |
| 1150/main21 | 2,60 s, 113 Hz, rms 0,0046 | Diktat "Get them all of you Leon", Wahl "Get them out of here, Leon." 0,939 | msg 28 bestaetigt |
| 11B0/main13 | 1,71 s, 141 Hz | Diktat "What: a"; DTW: naechster Nachbar Marvin-Rohtake 2026-10-15 16:45:42 (1,227; derselbe Take diktiert ebenfalls "What:"), naechster danach 1,778; die anderen beiden Takes dieser Sitzung diktieren "But hold on" (0,663 / 0,839); einzige kurze neue Marvin-Zeile | msg 20 (Sitzung + DTW + Ausschluss) |
| 11B0/main14 | 3,94 s, 98 Hz | Wahl "I just want to make sure we don't have any unwanted followers." 0,702 — der Kandidat "... unexpected visitors" (Nutzer-Zitat) wurde ANGEBOTEN und nicht gewaehlt | msg 21 bestaetigt; Text "unwanted followers" ist, was gesprochen wird |
| 1171/main00 | 1,71 s | Kendo-Rohtake 19:01:52 diktiert "Elsa: out" (= Elza, hurry up); Ordnerregel, nicht Teil der Tabelle | msg 0 (Regel) |
| 1171/main01 | 2,90 s, 124 Hz | Ordnerregel, nicht Teil der Tabelle | msg 1 (Regel) |
| 1171/main02 | 2,26 s, 112 Hz | DTW: 1,428 Kendo 19:11:11 ("All are littered on from here") und 1,676 Kendo 19:09:42 ("They lied what good are for near") = "Alright, let's get down from here"; alle anderen Takes/Dateien >= 2,34. KEINER der 6 Kendo-Takes diktiert etwas wie "give me a hand" | msg 3 bestaetigt; main02 ist NICHT msg 2 |

DTW-Haertung 1150 (gegen ALLE 42 WAV-Rohtakes des Ordners synchro/unused/STAGE1/room1150): main15 -> Leon-Take
2026-10-14 23:09:46 Abstand 1,502 (alle anderen 30 Leon-Takes >= 2,127); main18 -> Irons 2026-10-15 15:11:18 / 15:15:03 /
15:14:21 (1,310-1,380, naechster fremder 1,682); main21 -> Irons 15:26:20 (1,390, naechster 2,294). Das sind genau die
Takes, die das Dossier §1 nennt — unabhaengig reproduziert.

Fazit §3: alle 13 Tabellenzeilen sind inhaltlich richtig zugeordnet (9 direkt per Erkennung, main09/main15/main13 per
Stimmlage/Ausschluss/DTW, main02 per DTW). Die Sperre 1171 msg 2 und 11B0 msg 13/14 ist damit begruendet.

## 4. Messung je Nutzer-Punkt an der exe (eigene Messkopien `re15_pc_abn0_<teil>.exe` neben der exe, eigene
Laufordner `re15_port/build/abn0_wd/n<teil>/`, keine geerbten RE15_*; Umgebung wie tests/integration/test_r39_stimmen.cmake)

Werkzeuge: eigener Auswerter (Scratchpad `abn/auswert.py`, numpy): cap.raw (RE15_AUDIO_CAP_SYNC, stereo s16 44100,
1470 Frames je Bild) + state.log (RE15_STATE_LOG, erstes Bild mit `msg(a!=0 id=N)` je Raum-Abschnitt) -> Datei linear
auf 44100 Hz, normierte Kreuzkorrelation im Zeilenfenster (lag -3..+5 Bilder), Pegelfaktor dot(x,y)/dot(x,x),
Segmentfaktor-Median + Konsistenz (0,5 s). Werkzeug-Querprobe: meine erste Fassung lief auf Windows/numpy 1.26 in
einen int32-Ueberlauf (`np.arange(m)*32000` ab 1,5 s) und lieferte fuer lange Clips falsche 0,2-0,35 — nach der
Korrektur (float64) stimmen meine Werte mit `probe_r39_stimmen_auswertung` auf derselben Aufnahme auf 3 Stellen ueberein
(Beleg, dass das Repo-Werkzeug rechnet, was es behauptet). Zusaetzlich driftfeste Segment-Korrelation mit feiner
Lag-Suche (`abn/segkorr.py`).
Hinweis zur Methode: alle Laeufe sind frame-locked und deterministisch — meine Laeufe liefern bildgenau dieselben
Message_on-Bilder und Korrelationen wie die Integrationslaeufe des Bau-Agenten (lauf_1050_493df7fd, lauf_1150_37a18724,
lauf_11b0_1abc383e, lauf_1171_7388e79b aus meinem ctest-Lauf), die ich mit meinem Werkzeug ebenfalls ausgewertet habe.

### 4.1 Punkt 1 — ROOM1050, Ada-Ruf main09-11 (Lauf n1050, 35 s, exit 0)
Weg: Debug-Sprung ROOM1000@F120 (Flag 3:121), Position vor der 1050-Tuer, Laufen U0.8 -> TUER -> ROOM1050, zur
10A0-Tuer, Quadrat@150 -> Szene `[adaruf] Ereignis 13: Szene startet` (Leon PL00).
debug.log (n1050, Z.170-195):
```
[msg] room=1050 id=22  [msgtext] "Woman: Hello? Anyone? Please, get me out of here!"
[voice] ROOM1050 msg 22 <- main09.wav (Runde 39 Spur A, Nutzer-Reihenfolge)   [voice] clip loaded: main09.wav (32000 Hz x1 -> 188928 @44100 Hz)
[msg] room=1050 id=23  [msgtext] "Leon: Another civilian survivor."    msg 23 <- main10.wav  clip loaded: main10.wav (... 82432 ...)
[msg] room=1050 id=24  [msgtext] "Leon: I have to help her!"           msg 24 <- main11.wav  clip loaded: main11.wav (... 59904 ...)
```
Mitschnitt (Abschnitt 2 = ROOM1050, 601 Bilder): main09 ab F171+1 korr 0,994 faktor 1,97 Konsistenz 8/8 |
main10 ab F303+1 korr 0,981 faktor 1,20 3/3 | main11 ab F413+1 korr 0,938 faktor 1,00 2/2.
Stimme gegen Rest im Fenster: main09 Stimme rms 1,97*1478=2912 bei Fenster-rms 2928 -> praktisch nur Stimme (+29 dB).
**Urteil: erfuellt.** Die drei Nutzer-Dateien spielen an ihren drei Zeilen der neuen Szene, am echten Weg.

### 4.2 Punkt 2 — ROOM1150, Irons' Tod (Lauf n1150, 56 s, exit 0)
Weg: Spielstand probe_r35_cut1150_karte (1130, nach10f0, erste Szene gesehen) + CONTINUE in ROOM1130, Aktion an der
Tuer (A0.2 ab F60) -> ROOM1150 -> `[irons-tod] ROOM1150 Zustand 1: Szene startet (Programm 0)` (Leon PL00).
debug.log (n1150): msg 22 "Leon: Sir!" <- main15 (clip 31745) | 23 "Leon: Sir, the communication system ..." <- main16
(253441) | 24 "Leon: I came to get you, come with me!" <- main17 | 25 "Irons: Leon... I... I'm proud ..." <- main18
(266752) | 26 <- main19 (230400) | 27 <- main20 (266752) | 28 "Irons: Get them out of here... Leon..." <- main21 (114688)
| **msg 29 "Leon: SIR, Sir?!": `[voice] ROOM1150 msg 29: keine Aufnahme (synchro/STAGE1/room1150/main29.wav) - Zeile
bleibt stumm`** (es gibt 7 Dateien fuer 8 Zeilen — keine Datei des Nutzers fuer diese Zeile).
Mitschnitt (Abschnitt 1 = ROOM1150, 1251 Bilder), Message_on-Bilder 22 F26, 23 F212, 24 F386, 25 F466, 26 F649, 27 F807,
28 F990, 29 F1120:
| Zeile | Datei | korr | faktor (Median) | Konsistenz | Stimme rms | Fenster rms | Rest rms | Stimme/Rest |
|---|---|---|---|---|---|---|---|---|
| 22 | main15 | 0,999 | 1,27 | 1/1 | 2488 | 2490 | ~100 | +28 dB |
| 23 | main16 | 0,864 | 1,00 | 11/11 | 1790 | 2069 | 1037 | +4,7 dB |
| 24 | main17 | 0,702 | 1,18 (1,20) | 4/4 | 2124 | 3034 | 2166 | -0,2 dB |
| 25 | main18 | 0,294 | 4,00 (3,90) | 11/11 | 512 | 1746 | 1669 | **-10,3 dB** |
| 26 | main19 | 0,243 | 3,88 (3,99) | 10/10 | 516 | 2131 | 2068 | **-12,1 dB** |
| 27 | main20 | 0,285 | 4,12 (4,04) | 12/12 | 577 | 2029 | 1945 | **-10,6 dB** |
| 28 | main21 | 0,927 | 4,00 (4,01) | 5/5 | 596 | 645 | 246 | +7,7 dB |
Die Irons-Zeilen 25-27 sind IM Ton (Segmentfaktor ueber 10-12 Segmente konstant 3,9-4,1 = genau der 4x-Deckel des
Level-floors; ein nicht gespielter Clip streut um 0 — vgl. §4.5 Vorher), liegen aber 10-12 dB UNTER der BGM MAIN1E
(`[bgm] loaded` nach msg 22). Ursache (gemessen §3): die Dateien main18-21 haben rms 0,0040-0,0046 (Spitze 0,05-0,09),
Leons main15-17 0,055-0,061 — 22 dB Unterschied; der Lader hebt hoechstens 4x (+12 dB, audio_pc.c Z.2372-2374, seit
2026-08, unveraendert).
**Urteil: erfuellt** (Wortlaut "sind nicht drin" -> sie sind drin, an den richtigen Zeilen, am echten Weg). HINWEIS fuer
den Nutzer (kein Mangel der Spur, Pegel = Nutzer-Datei): Irons ist in der Sterbeszene unter der Musik ~10 dB zu leise;
Entscheidung Nutzer (lauter exportieren oder Port-Anhebung > 4x fuer diese Dateien). msg 29 hat keine Aufnahme.

### 4.3 Punkt 3 — ROOM11B0, Marvin "But, hold on..." / "... unwanted followers" (Lauf n11b0, 102 s, exit 0)
Weg: Spielstand probe_r36_elza_karte 11c0 (Leon, Strom an, 11C0 erledigt, sub04 gesehen) + CONTINUE in ROOM11C0,
`[fire-aot] slot=0 at F60 (Raum 11C0)` = Tuer-AOT Slot 0 (derselbe Pfad wie Hineinlaufen, main.c:7985) -> ROOM11B0,
Szene sub06 + Erweiterung bis ROOM1260.
debug.log (n11b0): msg 1..12 je `[voice] clip loaded: main01..12.wav` (Regel, wie vorher); **msg 20 `[msgtext] "Marvin:
But, hold on..."` -> `[voice] ROOM11B0 msg 20 <- main13.wav` + `clip loaded: main13.wav (... 75264 ...)`; msg 21
`[msgtext] "Marvin: I just want to make sure we don't have any unwanted followers."` -> `msg 21 <- main14.wav` + `clip
loaded: main14.wav (... 173569 ...)`**; keine Sperrzeile (msg 13/14 kommen in der Szene nicht vor).
Mitschnitt (Abschnitt 1 = ROOM11B0, 2789 Bilder): main13 ab F2124+2 korr 0,905 faktor 1,40 3/3 | main14 ab F2678+2 korr
0,957 faktor 1,44 7/7 | Gegenprobe Regel: main01 ab F26+2 0,924, main12 ab F2024+2 0,944.
Wortlaut: die Aufnahme main14 spricht "unwanted followers" (§3, Kandidat "unexpected visitors" angeboten, nicht gewaehlt)
— der Text der Szene bleibt richtig bei der Aufnahme.
**Urteil: erfuellt.**

### 4.4 Punkt 4 — ROOM1171 (Elza), "Alright, let's get down from here!" (Lauf n1171, 58 s, exit 0)
Weg: Spielstand probe_r36_elza_karte 1131 (Figur 4) + CONTINUE in ROOM1131 -> `[pl] Spieler-Familie PL04 (character=4,
Elza-Bit=1)`; Aktion A an der Tuer (F30) -> `[aot] DOOR FIRE slot=3` -> ROOM1171; `[fire-aot] slot=5 at F60 (Raum 1171)`
(= Szenen-Ausloeser, derselbe Pfad wie Hineinlaufen) -> `[evt] F61 room=1171 Evt_exec sub=2` (Dachszene).
debug.log (n1171): msg 0 `clip loaded: main00.wav` (Regel) | msg 1 `clip loaded: main01.wav` (Regel) | **`[msg]
room=1171 id=3` -> `[voice] ROOM1171 msg 3 <- main02.wav (Runde 39 Spur A, Nutzer-Reihenfolge)` + `clip loaded:
main02.wav (32000 Hz x1 -> 99841 @44100 Hz)`**; KEINE Zeile fuer msg 2 (das Skript zeigt sie nie, Zensus §2.1); danach
`[aot] DOOR FIRE slot=1` -> ROOM10B1 (F1143).
Mitschnitt (Abschnitt 1 = ROOM1171, 1144 Bilder): main02 ab F1063+2 korr 0,933 faktor 1,49 4/4 (Clip 68 Bilder, endet
~F1133, VOR dem Tuer-AOT nach F1143 -> die Zeile spielt ganz) | main00 ab F510+2 0,967 | main01 ab F752+2 0,935.
Inhalt main02 = "Alright, let's get down from here!" (§3: DTW 1,428/1,676 zu den beiden Kendo-Takes dieses Satzes,
kein Take "give me a hand"). "mal wieder": eine Datei main03.wav gab es nie (`git log --name-status --
synchro/STAGE1/room1171` = alle drei mit e2cc7673), und VORHER (§4.5) ist die Zeile mit der v0.8.26-exe stumm — der
Nutzer meldet denselben Defekt (Nummer != Message-Id) ein zweites Mal; Runde 38 hatte den Inhalt von main02 aus der
Nummer geschlossen (I_heliport1171.md §7 / Release-Notes v0.8.26) statt die Aufnahme zu hoeren. Keine Regression.
**Urteil: erfuellt.**

### 4.5 VORHER — Gegenprobe mit der v0.8.26-exe (master-Bau C:/workspace/git/reAi_v2/re15_port/build/platform/pc/
re15_pc.exe 01:15, Kopie im Scratchpad, cwd im Spurbaum -> `[asset] cd-root: .../r39_stimmen/re15_port/shared_assets/PSX`,
synchro = dieselben Nutzer-Dateien; Laeufe v1050/v1171, gleiche Umgebung wie n1050/n1171)
- v1050: `[adaruf] Ereignis 13: Szene startet`, `[msg] id=22/23/24` + `[msgtext]` an DENSELBEN Bildern F171/F303/F413,
  KEINE `[voice]`-Zeile (der alte Lader schwieg bei fehlender Datei); Mitschnitt: main09 korr 0,013 (Fenster-rms 331),
  main10 0,035 (397), main11 0,019 (535) -> stumm.
- v1171: Elza PL04, gleicher Weg, `[msg] room=1171 id=3` an F1063 OHNE `[voice]`; main02 korr 0,015 (Fenster-rms 589)
  -> stumm; main00/main01 0,967/0,935 wie nachher (Regel-Dateien unveraendert).
Der Fix erklaert den Befund vollstaendig: gleicher Weg, gleiche Bilder, einziger Unterschied ist die Dateisuche des
Laders (main<Message-Id>.wav existiert fuer 1050/22-24, 1150/22-29, 11B0/20-21, 1171/3 nicht — `git ls-files`).

## 5. Vertrag, Tests, Nebenwirkungen

### 5.1 Vertrag (VERTRAG.md)
- Pfade: nur engine/src, include, platform/pc/src/audio_pc.c (Haken 4 Zeilen mit Kommentar "Runde 39 Spur A"), tests/
  (Registrierung ausschliesslich tests/unit/probes/r39_stimmen.cmake, GLOB in tests/unit/CMakeLists.txt:4060), Dossier.
  Keine release/, platform/android/, shared_assets/PSX/, keine Test-CMakeLists, keine Nutzer-Datei unter synchro/.
- Ressourcen: keine Bank-9-Bits, keine neuen Nachrichten-Ids (nur vorhandene 22-24/22-29/20-21/3), keine Slots.
  Keine Umwidmung einer Id, zu der der Nutzer eine WAV hat (§1.2) — im Gegenteil: die Sperre verhindert, dass
  main13/14 (11B0) und main02 (1171) unter einer fremden Id spielen.
- Linux-/Parallel-Festigkeit der neuen Tests: `cmake_minimum_required` im -P-Skript, `m` bei NOT WIN32, `_DEFAULT_SOURCE`
  im Lader-Riegel, keine Backslash-Pfade, RE15_*-Umgebung wird im Skript geloescht, eigener Laufordner mit Zufalls-Id,
  exe-Kopie `re15_pc_r39a_<teil>_<id>.exe` (gleiche Form wie test_r34n_b_cursor.cmake; die Endung ist auf Linux ohne
  Bedeutung, das Vorbild lief im v0.8.26-Linux-Lauf).
- Ueberschneidung mit Spur K (1171): die Stimme zu msg 3 kommt aus der Tabelle (Raum, Nachricht), nicht aus einer
  Umbenennung — K kann darauf aufbauen (VERTRAG §1.4).

### 5.2 Tests der Spur (selbst gefahren)
- `ctest -R unit_r39_stimmen`: 2/2 (tabelle 143 Pruefungen, lader 21 Pruefungen mit dem ECHTEN audio_pc.c — Ausgabe
  oben: alle 13 Zuordnungen korr 1,000, Sperren stumm, Regel 11B0/1, 1171/0, 1170/3 unberuehrt, Gegenprobe korr 0,000).
- `ctest -R integration_r39_stimmen -j1`: 4/4 Passed (1050 35,05 s, 1150 56,39 s, 11b0 102 s, 1171 58,12 s; 252 s).
  Jeder Teil misst am echten Weg (Spielstand + CONTINUE + Tuer bzw. Debug-Sprung + Tuer) Log-Zeilen UND Mitschnitt.
- Gezielte Gruppe der beruehrten Module (93 Tests, Regex des Dossiers §6.3, davon 21 Integration): s. §7 (Ergebnis).
- Wiederholbarkeit: alle 17 Integrations-Laufordner (lauf_1050_* x4, lauf_1150_* x4, lauf_11b0_* x4, lauf_1171_* x5;
  07:32-08:48, Bau-Agent + mein ctest-Lauf, teils unter Last paralleler Agenten) mit meinem Auswerter: bitgleiche
  Message_on-Bilder und Korrelationen (1050 0,994/0,981/0,938; 1150 0,999/0,294/0,285/0,927 mit 11/11, 12/12; 11B0
  0,905/0,957; 1171 0,933) — frame-locked, deterministisch, kein Flattern der Messung.
- Waehrend meiner Testgruppe liefen fremde exe-Kopien paralleler Agenten (`re15_pc_r39l_marke_*` Spur L,
  `re15_pc_r37k2f_*`): Last vorhanden; rote Fenster-Haken waeren vor einer Wertung einzeln nachzufahren.

### 5.3 Nebenwirkungen
- Alle `[voice]`-Zeilen meiner vier Laeufe (ohne `clip loaded`): genau die 13 Zuordnungszeilen + `ROOM1150 msg 29: keine
  Aufnahme`; keine Sperrzeile, keine fremde "keine Aufnahme"-Zeile in den Nachbarraeumen 1000/1130/11C0/1131/10B1/1260.
- Elza-Varianten: die drei Port-Szenen installieren nur fuer 0x1050 / 0x1150 / 0x11B0 (adaruf_1050.c:217,
  irons_tod_1150.c:1056, szenenleon_r36.c:462 — "ROOM11B1 (Elza) bleibt Original"); die Tabelle ist raumgenau, 1051/1151/
  11B1 bleiben bei der Regel (Unit-Riegel "Regel bleibt" 20 Faelle). Leons ROOM1170 msg 3 spielt weiter main03.wav (48 kHz,
  Lader-Riegel PASS 20).
- Sperre 11B0 msg 13/14: am Mechanismus (echter Lader) gemessen — msg 13 (Untersuchungstext am Container, sub03
  @0x0126E/@0x01298) und 14 bleiben stumm statt Marvins "But, hold on..." zu spielen (das war seit e2cc7673 ein Defekt).
  Am Spiel selbst (Untersuchen des Containers) nicht nachgefahren.
- Pegelanhebung unveraendert (4x-Deckel); keine Aenderung an BGM/SE/XA-Pfaden; PSX-neutral (fprintf nur unter
  RE15_PLATFORM_PC).

## 6. Maengel und Hinweise

### 6.1 Maengel (nachpruefbar, nummeriert)
- keine, die ein Nutzer-Punkt oder ein Gate verletzt (Stand nach §7).

### 6.2 Hinweise (keine Maengel der Spur; fuer Nutzer / Integration)
1. ROOM1150 Pegel: Irons' neue Zeilen main18-21 sind rms 0,0040-0,0046 (-47 dBFS), Leons main15-17 0,055-0,061 — 22 dB
   Unterschied in den gelieferten Dateien; der Port hebt hoechstens 4x (audio_pc.c Z.2372-2374, Port-Regel seit 2026-08,
   nicht Teil dieser Spur). In der Sterbeszene liegt Irons damit 10-12 dB unter der BGM MAIN1E (gemessen §4.2: Stimme
   rms 512-577 gegen Rest 1669-2068), Leon +4,7 dB darueber. Das ist hoerbar leise. Entscheidung Nutzer: Dateien lauter
   exportieren (dann greift der Deckel nicht mehr) ODER Port-Anhebung fuer diese Dateien ueber 4x (Port-Wahl).
   Dieselbe Pegelklasse haben schon Irons' alte Zeilen (main05/07: rms 0,0054-0,0062).
2. ROOM1150 msg 29 "Leon: SIR, Sir?!" hat keine Aufnahme (7 Dateien fuer 8 Zeilen) — jetzt sichtbar im debug.log
   (`keine Aufnahme (synchro/STAGE1/room1150/main29.wav)`); eine Datei main29.wav spielt nach der Ordnerregel sofort.
3. ROOM11B0 msg 21: die Aufnahme spricht "I just want to make sure we don't have any unwanted followers." (Szenentext);
   "unexpected Visitors" ist ein Gedaechtniszitat des Nutzers. Text + Datei passen zueinander; nichts zu aendern.
4. ROOM1171 msg 2 "John: C'mon, give me a hand!" zeigt das Original-Skript nie (Message_on-Zensus 0/1/3) — keine
   Aufnahme noetig; die Release-Notes v0.8.26 fuehrten main02.wav faelschlich als diese Zeile.
5. Integration mit master: master ist um 4 doc/release-Commits (e76b9924..387a7a69) weiter; `git diff master` zeigt
   deshalb AUFTRAG/UEBERGABE/VERTRAG/orchestrator/release/make_package.sh als "geloescht" — das sind Master-Zugaenge, die
   Spur hat sie nicht angefasst (gegen die Basis 3e49ff87: 9 Dateien, nur Zugaenge). Kein Konflikt zu erwarten.
6. Beobachtung ausserhalb der Spur (nicht geaendert, nicht bewertet): in ROOM1150/11B0 loggt `[msg] room=... id=24`
   (bzw. 2/5) ueber ~50 Bilder je Bild einmal — das Szenenprogramm fuehrt Message_on je Bild erneut aus; die Stimme
   startet dabei NICHT neu (Mitschnitt main17: ein Clip, lag +1, Faktor 1,2 ueber alle Segmente).

## 7. Gates und Urteil

| Gate | Ergebnis |
|---|---|
| Bau (local_build.sh configure+build) | OK, exe = HEAD-Code (§1.1) |
| RE-Gate (Konstanten mit Quelle, 3+ Adressen selbst disassembliert, keine Guess-Tells, kein Env-Schalter) | haelt (§2) |
| Fix erklaert den Befund (Vorher/Nachher am gleichen Weg, gleiche Bilder) | ja (§4.5) |
| Vertrag (Pfade, Ressourcen, kleine Haken, Tests je Punkt, Linux-/Parallel-Form) | haelt (§5.1) |
| Tests der Spur (Unit 2/2, Integration 4/4 am echten Weg) | gruen (§5.2) |
| Nebenwirkungen (Nachbarraeume, Elza-Varianten, Regel-Dateien, Sperren) | keine unerwuenschten (§5.3) |
| Gezielte Testgruppe der beruehrten Module (93 Tests, sequenziell) | s. 7.1 |

### 7.1 Gezielte Testgruppe (Regex Dossier §6.3, `ctest -j1 --timeout 1500`)
Lauf 08:55:16-09:31:39, sequenziell (-j1), parallel liefen fremde exe-Kopien anderer Spuren (Last):
**`100% tests passed, 0 tests failed out of 93` (Total Test time (real) = 2182.34 sec)**. Kein roter Haken, keine
Wiederholung noetig. Darunter: integration_r35_cut1150 511,8 s, r36_elza_* (5 Integrationen), r36_szene1150 113,6 s,
r36_szenenleon 161,5 s, r38_elzaintro_stimme/schwarz, r38_heliport1171_weg_re2/weg_re15/laden, r38_kleinkram_irons,
r38_rekorder 205,4 s, r38_werkstatt11b0 151,4 s, r39_stimmen (Unit 2 + Integration 4: 35,0 / 56,3 / 102,2 / 58,2 s).
Log: Scratchpad abn/ctest_gezielt.log (nicht im Repo).

### 7.2 Urteil je Nutzer-Punkt (gegen den Wortlaut)
| Punkt | Wortlaut (gekuerzt) | Urteil | Beleg |
|---|---|---|---|
| 1 | 1050: Voiceover vor der Ada-Rettung (main09-11) spielt nicht | **erfuellt** | §4.1: `[voice] ROOM1050 msg 22/23/24 <- main09/10/11.wav` + clip loaded, Mitschnitt 0,994/0,981/0,938; vorher §4.5 stumm |
| 2 | 1150: Voiceovers der Sterbeszene nicht drin | **erfuellt** (Hinweis Pegel) | §4.2: msg 22..28 <- main15..21, alle 7 im Ton (Irons 11/11, 10/10, 12/12 Segmente bei Faktor 4,0), msg 29 ohne Datei gemeldet |
| 3 | 11B0: "But hold on" + "... unexpected Visitors" fehlen | **erfuellt** | §4.3: msg 20 <- main13 (0,905), msg 21 <- main14 (0,957); Aufnahme spricht "unwanted followers" |
| 4 | "Alright, let's get down from here" ohne Voiceover | **erfuellt** | §4.4: Elza PL04, msg 3 <- main02 (0,933), Clip endet vor dem Tuer-AOT; vorher §4.5 stumm; main02 = dieser Satz (§3) |

### 7.3 Gesamturteil
**BESTANDEN.** Alle vier Punkte am gebauten Stand, am echten Weg, mit eigener exe-Kopie und eigenem Auswerter gemessen;
Vorher/Nachher mit derselben Umgebung (v0.8.26-exe gegen HEAD) erklaert den Befund; RE-Gate (RE2-Form xa_on
@0x80058028, Instruktionen selbst gelesen), Vertrag, Tests (2 + 4 + 93/93) und Nebenwirkungen halten. Offene Punkte
sind Nutzer-Entscheidungen (§6.2: Irons-Pegel, msg 29 ohne Datei), keine Maengel der Spur.

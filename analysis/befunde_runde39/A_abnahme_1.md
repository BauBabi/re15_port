# Runde 39 Spur A "stimmen" — Abnahme 1 (unabhaengig, 2026-10-18, Nachbesserung 1)

Baum `.claude/worktrees/r39_stimmen`, Zweig `r39/stimmen`, HEAD ad16c58f. Pruefer: Abnahme 1 (Subagent, unabhaengig vom
Bau-Agenten). Nichts im Code geaendert; dieser Bericht ist die einzige Aenderung. Bericht FORTLAUFEND geschrieben.
Gegenstand: die vier Nutzer-Punkte (AUFTRAG.md) PLUS Nachtrag 2 des Nutzers (AUFTRAG.md 09:48: Irons lauter = NUTZER-
VORGABE, 1150 msg 29 = main22.wav, 11B0 "unwanted followers"), umgesetzt als Nachbesserung 1 (Dossier A_stimmen.md §9).

## 0. Umfang des Diffs

Zwei Merge-Basen (gemessen): `git merge-base master HEAD` = 32ab1f97 (ueber den Merge 275a322c), daneben bb285672 (ueber
die Integration fc7a0231 -> c71fa144). `git diff master --stat` zeigt deshalb die master-Zugaenge der Spuren B/C/L als
"geloescht" — Basis-Versatz, nicht die Spur. Der Diff der NACHBESSERUNG ist `git diff 275a322c HEAD`:
- NEU `engine/src/stimmen_pegel_r39.c` (68), NEU `include/re15_stimmen_pegel_r39.h` (61): Stufe 2 "Lautheits-Boden"
- `engine/src/stimmen_r39.c`: +1 Tabellenzeile `{ 0x1150, 29, 22 }` (+Kommentar); `include/re15_stimmen_r39.h`: +4 Kommentar
- `platform/pc/src/audio_pc.c`: +1 include, +1 Aufruf `re15_stimmen_pegel_r39(pcm, out_n, stamm, NULL)` direkt nach dem
  Stufe-1-Block ("Level-floor", unveraendert), +4 Kommentarzeilen "Runde 39 Spur A (Nachbesserung 1)"
- Tests: `tests/unit/test_r39_stimmen.c` (+Teil `pegel`), `probe_r39_stimmen_lader.c`, `probe_r39_stimmen_auswertung.c`
  (+stimme/rest/klemm), `tests/unit/probes/r39_stimmen.cmake` (+unit_r39_stimmen_pegel, Bestandsliste per GLOB),
  `tests/integration/test_r39_stimmen.cmake` (1150: msg 29, Stufe-2-Zeilen, Pegelbaender, klemm 0)
- `synchro/README.md` (Pegel-Absatz), Dossier `A_stimmen.md` §9
Keine release/, platform/android/, shared_assets/PSX/, keine Test-CMakeLists; KEINE Nutzer-Datei unter synchro/ geaendert
(`git diff 275a322c HEAD --stat -- synchro/` = nur README.md; main22.wav kam mit 32ab1f97 vom Nutzer, md5
55976ac961e26828776eba97ffa788b8, in der Spur unveraendert). Arbeitsbaum sauber (`git status --short` leer).

## 1. Bau

`bash re15_port/tools/local_build.sh configure` -> LOCAL-BUILD-OK; `build` -> `ninja: no work to do`, LOCAL-BUILD-OK.
exe `re15_port/build/platform/pc/re15_pc.exe` 10:17:40; letzte Code-Aenderung der Spur 10:14:23 (audio_pc.c,
re15_stimmen_pegel_r39.h; stimmen_pegel_r39.c 10:12:27), `stimmen_pegel_r39.c.obj` 10:17:39; letzter Code-Commit 9f5e13a4
10:15:27, danach nur Dossier (466ebd17, ad16c58f) -> die exe IST der HEAD-Code. Gegenprobe: die exe traegt den String
"Runde 39 Spur A Stufe 2" (grep -c = 1), die master-exe (10:20, Welle 1 ohne Nachbesserung) nicht (0).
Unit-Riegel `ctest -R unit_r39_stimmen`: 3/3 (tabelle 162 Pruefungen, pegel 282 Pruefungen, lader) — Ausgabe des Pegel-
Riegels: "206 Dateien auf Pegel, rms min 1162 (T=985) median 2040 (R=2040), groesste Spitze 21759 = room1240/main05.wav".

## 2. RE-Gate (selbst nachgeschlagen)

### 2.1 Konstanten der Nachbesserung
| Konstante | Kennzeichnung im Code | Selbst geprueft |
|---|---|---|
| T = 985 | PORT-WAHL (Form) / NUTZER-VORGABE (Grund), Herkunft "Mitte der Luecke 808..1162 (Zensus)" im Header | ja, eigener Zensus (§3): Luecke roh 808..1162, auf 44,1 kHz (Spielpfad) 795..1146; T liegt in beiden |
| R = 2040 | "Median rms der 206 Dateien auf Spitzenpegel" | ja: Median roh 2040, auf 44,1 kHz 2048; entscheidet am heutigen Bestand nicht (alle 10 laufen in C) |
| C = 21759 | "Spitze der lautesten gelieferten Datei (room1240/main05)" | ja: roh 21759 = room1240/main05 (48 kHz 24 bit stereo); nach dem Lader-Resampling liegt dieselbe Datei bei 21620 (-0,06 dB) — Praezisionshinweis, kein Verhaltensunterschied |
| Stufe 1: 14745 / 4x | unveraendert, Quelle aa346af1 (2026-06-28 13:19, "psx_dev-Engine uebernommen") | ja: audio_pc.c Level-floor-Block unveraendert im Diff; Commit existiert mit diesem Datum |
| Mischer: Stimme mit cd_vol 0x3FFF>>14, Klemme erst auf die Summe | audio_pc.c :482-486 | ja: `L = out + ((smp * cd_vol_l) >> 14)` dann Klemme; `s_xa.cd_vol_l = 0x3FFF` (:2043); RE2_Quellcode_V2/CD_initvol.c schreibt 0x3fff (+0x1b0/+0x1b2 = SPU-CD-Volume) |
| 1150 msg 29 <- main22 | NUTZER-NACHTRAG 2 (32ab1f97) | ja: irons_tod_1150.c:134 `OP_MSG(29)`, :660 k_msg29 "Leon: SIR, Sir?!", :698 Tabelle {0x1150, 29}; Datei vom Nutzer, Inhalt §3 |
| 11B0 msg 21 Text | gen/szl11b0_szene.inc:134 | ja: "msg 21 Marvin: I just want to make sure / we don't have any unwanted followers." |
Guess-Tells (`deferred|tunable|interim|for now|faithful|plausibel|TODO|FIXME|getenv|hack|workaround`) im Code-Diff: KEINE.
Kein Env-Schalter. Form (WAV-Lautheitsregel) ist ehrlich als PORT-WAHL gekennzeichnet: RE1.5/RE2 haben keinen WAV-Lader,
RE2-XA-Stimmen sind fertig gemischt (xa_on startet nur den Stream, s. 2.2).

### 2.2 Adressen selbst disassembliert (ghidra_re2_Leon.txt = richtige Binaerdatei, Form = RE2; RDT-Bytes per xxd)
- Dispatch: `800a7574 addr LAB_80054a8c` (Eintrag 0x2B) und `800a762c addr LAB_80058028` (Eintrag 0x59); Basis
  0x800a7574 - 0x2B*4 = 0x800a74c8 = 0x800a762c - 0x59*4 -> konsistent.
- xa_on @0x80058028: `80058038 lw v0,0x1c(s0)`, `80058040 lbu a0,0x1(v0)`, `80058044 lhu a1,0x2(v0)` (Stimmnummer =
  eigener 16-Bit-Operand), `80058048 jal FUN_800129b4`, `80058060 ori v0,v0,0x20`, `80058070 addiu v0,v0,0x4`. Bestaetigt.
- message_on @0x80054a8c: `80054aa0 lbu a2,0x2(v0)`, `80054aa4 lbu a1,0x3(v0)`, `80054aa8 lhu s0,0x4(v0)`,
  `80054aac addiu v0,v0,0x6`, `80054ac0 jal FUN_8002fe38` — kein XA-Aufruf. Bestaetigt.
- ROOM1171.RDT @0x1586 `2b 03 00 00`, @0x14BA `2b 00 00 00`, @0x151E `2b 01 00 00`; ROOM11B0.RDT @0x126E und @0x1298
  `2b 0d ff ff`. Bestaetigt.
Urteil RE-Gate: haelt.

## 3. Eigener Zensus des Bestands (Scratchpad abn1/zensus_abn1.py, numpy; unabhaengig vom Riegel und vom Dossier)

Alle 216 `synchro/STAGE*/room*/main*.wav` + die 7 anderen WAVs (`room1100/Irons_Rekorder_Sound*.wav`, `room1141/help01..05.wav`),
Stufe 1 (14745/4x) und Stufe 2 (T/R/C) GANZZAHLIG nachgerechnet wie der Lader, einmal auf dem rohen Mono-Signal (wie der
Riegel) und einmal nach linearem Resampling auf 44100 Hz (wie der echte Lader, Formel audio_pc.c Q16):
- Beide Modi: GENAU 10 Dateien angehoben = room1150/main02/05/07/09/12/13/18/19/20/21 (alle Irons, alle am 4x-Deckel der
  Stufe 1), alle in der Spitzenbegrenzung C; 206 unberuehrt, keine davon am 4x-Deckel. Zahlen roh BITGLEICH mit der
  Riegel-Ausgabe (z.B. main18 rms 521 -> 1640, Spitze 6916 -> 21748, gesamt 12,57x).
- Spielpfad (44,1 kHz): Gesamtfaktor main18 12,80x, main19 9,31x, main20 7,73x, main21 11,59x; Luecke 795 (main05) ..
  1146 (room2000/main02); naechste Dateien ueber T: room2000/main02 1146, room1170/main00 1199, room1150/main14 1198.
  T = 985 hat 16 % Abstand nach oben und 19 % nach unten — die Schwelle sitzt in der Luecke, nicht auf dem Rand.
- Die 7 Dateien AUSSERHALB des Riegel-GLOBs (Rekorder 1100, help 1141 — der Rekorder laeuft ueber denselben Lader!):
  rms 2072-3038 >= T, Stufe 2 greift NICHT (Faktor 1,00). Keine verdeckte Nebenwirkung.
- Transienten-Behauptung (Dossier §9.4: main19/main20 durch EINEN Transienten begrenzt): main20 Spitze 0,0898 FS bei
  2,969 s, Umgebung > 25 % der Spitze nur 15 Abtastwerte (0,5 ms), zweithoechste Spitze (ausserhalb +-50 ms) 0,0567 FS
  -> ohne diesen Knall waeren 11,7x statt 7,4x moeglich (+4 dB). main19: Spitze 0,0746 FS bei 4,246 s (7 Abtastwerte =
  0,2 ms), zweite Spitze 0,0630 FS -> ohne den ersten Knall 10,5x statt 8,9x (+1,4 dB). Die Behauptung stimmt fuer
  main20 voll, fuer main19 begrenzt schon die zweite Spitze. Mehr Pegel braeuchte einen Begrenzer (neuer Mechanismus,
  nicht beauftragt) oder einen Neuexport — so steht es auch im Dossier §9.8.

## 4. Messung je Nutzer-Punkt an der exe (eigene exe-Kopien `re15_pc_abn1_<name>.exe` neben der exe, eigene Laufordner
`re15_port/build/abn1_wd/<name>/`, keine geerbten RE15_*; Umgebung wie tests/integration/test_r39_stimmen.cmake;
Auswertung mit EIGENEM Werkzeug Scratchpad abn1/auswert_abn1.py — gleiche Definitionen wie probe_r39_stimmen_auswertung,
dazu Stimme/Rest in dB, Klemmwerte im ganzen Abschnitt, Spitze der Summe)

(wird nach jedem Lauf ergaenzt)

### 4.1 Punkt 1 — ROOM1050, Ada-Ruf main09-11 (Lauf abn1_wd/1050, 35 s, exit 0; Leon PL00)
Weg: Debug-Sprung ROOM1000@F120 (Flag 3:121), Position vor der 1050-Tuer, U0.8 -> Tuer -> ROOM1050, zur 10A0-Tuer,
Quadrat@150 -> `[adaruf] Ereignis 13: Szene startet`. debug.log Z.171-195:
```
[msgtext] room=1050 id=22 "Woman: Hello? Anyone? Please, get me out of here!"
[voice] ROOM1050 msg 22 <- main09.wav (Runde 39 Spur A, Nutzer-Reihenfolge)   [voice] clip loaded: main09.wav (32000 Hz x1 -> 188928 @44100 Hz)
[msgtext] room=1050 id=23 "Leon: Another civilian survivor."   msg 23 <- main10.wav   clip loaded: main10.wav (... 82432 ...)
[msgtext] room=1050 id=24 "Leon: I have to help her!"          msg 24 <- main11.wav   clip loaded: main11.wav (... 59904 ...)
```
KEINE `[voice] pegel`-Zeile (Stufe 2 greift hier nicht — Dateien auf Pegel). Eigener Auswerter (Abschnitt 2 = ROOM1050,
601 Bilder, Takt0 440): main09 ab F171 lag+1 korr 0,994 faktor 1,97 (8/8) Stimme/Rest +18,8 dB | main10 ab F303 0,981 /
1,20 (3/3) +14,1 dB | main11 ab F413 0,938 / 1,00 (2/2) +8,6 dB; klemm 0 im GANZEN Abschnitt, Spitze der Summe 0,487 FS.
Bitgleich mit Abnahme 0 (frame-locked). **Urteil: erfuellt** (unveraendert gegenueber Abnahme 0, keine Nebenwirkung der Stufe 2).

### 4.2 Punkt 2 — ROOM1150, Irons' Tod (Lauf abn1_wd/1150, 79 s unter Last, exit 0; Leon PL00) inkl. Nachtrag 2 (M1 Pegel, M2 msg 29)
Weg: Spielstand probe_r35_cut1150_karte (1130 nach10f0 ersteszene) + CONTINUE in ROOM1130 (`[save] CONTINUE: resumed in
room 1130`), Aktion an der Tuer (A0.2 ab F60) -> ROOM1150 -> `[irons-tod] ROOM1150 Zustand 1: Szene startet (Programm 0)`.
debug.log (Z.88-1397): msg 22 "Leon: Sir!" <- main15 | 23 <- main16 | 24 <- main17 | 25 "Irons: Leon... I... I'm proud ..."
<- main18 + **`[voice] pegel main18.wav: rms 513 -> 1641 (x3.19), spitze 6800 -> 21754 (Spitzenbegrenzung C) (Runde 39
Spur A Stufe 2: T=985 R=2040 C=21759)`** | 26 <- main19 + `pegel main19.wav: rms 533 -> 1241 (x2.32), spitze 9340 -> 21745`
| 27 <- main20 + `pegel main20.wav: rms 562 -> 1087 (x1.93), spitze 11232 -> 21719` | 28 <- main21 + `pegel main21.wav: rms
598 -> 1733 (x2.89), spitze 7500 -> 21739` | **29 `[msgtext] room=1150 id=29 "Leon: SIR, Sir?!"` -> `[voice] ROOM1150 msg
29 <- main22.wav (Runde 39 Spur A, Nutzer-Reihenfolge)` + `clip loaded: main22.wav (32000 Hz x1 -> 97793 @44100 Hz)`**.
KEINE `pegel`-Zeile fuer main15/16/17/22 (Leon), KEINE Zeile "keine Aufnahme" mehr. Die Stufe-2-Zeilen stimmen mit meinem
Zensus auf dem 44,1-kHz-Pfad ueberein (main18 rms 513 -> 1641, Spitze 6800 -> 21754: bitgleich, §3).
Eigener Auswerter (Abschnitt 1 = ROOM1150, 1251 Bilder, Takt0 619), Message_on-Bilder 22 F26, 23 F212, 24 F386, 25 F466,
26 F649, 27 F807, 28 F990, 29 F1120 (= Abnahme 0, frame-locked):
| Zeile | Datei | korr | Faktor (Median) | Kons. | Stimme rms | Rest rms | Stimme/Rest | Abnahme 0 (4x) | Spitze Summe | klemm |
|---|---|---|---|---|---|---|---|---|---|---|
| 22 Leon "Sir!" | main15 | 0,999 | 1,27 (1,27) | 1/1 | 2488 | 89 | +29,0 dB | +28 dB | 0,451 FS | 0 |
| 23 Leon | main16 | 0,864 | 1,00 (1,00) | 11/11 | 1788 | 1041 | +4,7 dB | +4,7 dB | 0,555 FS | 0 |
| 24 Leon | main17 | 0,702 | 1,18 (1,21) | 4/4 | 2129 | 2162 | -0,1 dB | -0,2 dB | 0,593 FS | 0 |
| 25 Irons "proud" | main18 | 0,701 | **12,80 (12,71)** | 11/11 | 1642 | 1669 | **-0,1 dB** | -10,3 dB | 0,723 FS | 0 |
| 26 Irons "not going to make it" | main19 | 0,510 | **9,20 (9,31)** | 10/10 | 1226 | 2067 | **-4,5 dB** | -12,1 dB | 0,773 FS | 0 |
| 27 Irons "one last favor" | main20 | 0,494 | **7,86 (7,81)** | 12/12 | 1104 | 1945 | **-4,9 dB** | -10,6 dB | 0,663 FS | 0 |
| 28 Irons "Get them out" | main21 | 0,990 | **11,60 (11,60)** | 5/5 | 1733 | 242 | +17,1 dB | +7,7 dB | 0,663 FS | 0 |
| 29 Leon "SIR, Sir?!" | main22 | **0,828** | 1,09 (1,08) | 3/3 | 2107 | 1428 | +3,4 dB | (keine Datei) | 0,588 FS | 0 |
Klemmwerte im GANZEN Abschnitt 0 (L+R), groesste Summe 25333 = 0,773 FS (Fenster main19) — keine Uebersteuerung.
Die Werte sind auf 2-3 Stellen identisch mit Dossier §9.4 / Integrationslauf des Bau-Agenten; mein Werkzeug ist eine
eigene Implementierung (numpy), nicht das Repo-Werkzeug.

**VORHER — Gegenprobe mit der master-exe (10:20 = Welle 1 integriert, OHNE Nachbesserung; Kopie `re15_pc_abn1_vorher.exe`
neben der Spur-exe -> dieselben Assets/synchro; Lauf abn1_wd/v1150, gleicher Weg, exit 0):** Message_on-Bilder identisch
(F26/212/386/466/649/807/990/1120), Leon identisch (1,27/1,00/1,18; +29,0/+4,7/-0,1 dB), Irons am alten 4x-Deckel:
main18 4,00x -10,2 dB | main19 3,88x -12,0 dB | main20 4,12x -10,5 dB | main21 4,00x +7,9 dB; msg 29: `[voice] ROOM1150 msg
29: keine Aufnahme (synchro/STAGE1/room1150/main29.wav) - Zeile bleibt stumm`, main22 im Fenster korr 0,008 (stumm).
Keine `pegel`-Zeile, die master-exe traegt den Stufe-2-String nicht (grep -c = 0). -> Der Fix ERKLAERT den Befund: einziger
Unterschied am selben Weg ist Stufe 2 (Irons +8,3/+7,5/+5,6/+9,2 dB) und die Tabellenzeile 29 -> main22.

Gemessen gegen die Vorgabe des Orchestrators ("dass es passt" = MESSEN, Bezug Leons Zeilen derselben Szene / BGM MAIN1E):
Leons Zeilen unter der BGM liegen bei -0,1 .. +4,7 dB; Irons main18 -0,1 dB (= Leons main17), main21 +17,1 dB, main19/main20
-4,5/-4,9 dB — die beiden bleiben 4,4-4,8 dB unter Leons leisester Zeile. Ursache selbst gemessen (§3): ein 0,2-0,5-ms-Knall
(main20 bei 2,969 s, 0,0898 FS; main19 bei 4,246 s, 0,0746 FS) bzw. bei main19 schon die zweite Spitze begrenzt die
Anhebung ohne Klemme; mehr ginge nur mit Begrenzer (neuer Mechanismus) oder Neuexport. Das Dossier (§9.4/§9.8) sagt genau
das und meldet es dem Nutzer.
**Urteil Punkt 2: erfuellt** — alle acht Zeilen der Sterbeszene spielen am echten Weg (jetzt 8/8 mit main22), Irons ist
gemaess NUTZER-VORGABE ueber den alten Deckel hinaus angehoben (Faktor 7,9-12,8x statt 4x, +5,6 .. +9,2 dB, ohne einen
geklemmten Abtastwert), Leon bitgleich. HINWEIS (keine Spurluecke, Nutzer-Entscheidung): main19/main20 bleiben ~4,5 dB
unter der Musik — Neuexport ohne den Knall oder mit normaler Aussteuerung wuerde die Anhebung ueberfluessig machen.

### 4.3 Punkt 3 — ROOM11B0, Marvin "But, hold on..." / "... unwanted followers" (Lauf abn1_wd/11b0, 103 s, exit 0; Leon PL00) inkl. Nachtrag 2 (M3 Text)
Weg: Spielstand probe_r36_elza_karte 11c0 (Leon, Strom an, 11C0 erledigt, sub04 gesehen) + CONTINUE in ROOM11C0
(`[save] CONTINUE: resumed in room 11c0`), `[fire-aot] slot=0 at F60` -> `[aot] DOOR FIRE slot=0 ... target_cut=1` ->
ROOM11B0, Szene bis `[aot] DOOR FIRE slot=5` -> `[room] PC loaded room1260.rdt`.
debug.log Z.501-566: **`[msgtext] room=11b0 id=20 "Marvin: But, hold on..."` -> `[voice] ROOM11B0 msg 20 <- main13.wav
(Runde 39 Spur A, Nutzer-Reihenfolge)`; `[msgtext] room=11b0 id=21 "Marvin: I just want to make sure we don't have any
unwanted followers."` -> `[voice] ROOM11B0 msg 21 <- main14.wav`**; 14 Clips geladen (main01..14), keine Sperrzeile
(msg 13/14 kommen in der Szene nicht vor), keine `pegel`-Zeile; "unexpected" kommt im Log nicht vor (grep 0).
Eigener Auswerter (Abschnitt 1 = ROOM11B0, 2789 Bilder): main13 ab F2124 lag+2 korr 0,905 faktor 1,40 (3/3) +6,6 dB |
main14 ab F2678 lag+2 korr 0,957 faktor 1,44 (7/7) +10,3 dB | Regel-Gegenprobe main01 ab F26 0,924, main12 ab F2024 0,944;
klemm 0 im ganzen Abschnitt. Bitgleich mit Abnahme 0.
M3 ("Aendere es zu unwanted followers im Text bei 11B0"): der Text IST "unwanted followers" (gen/szl11b0_szene.inc:134,
`[msgtext]`-Zeile oben, Aufnahme main14 spricht denselben Wortlaut — Abnahme 0 §3: Kandidat "unexpected visitors"
angeboten und nicht gewaehlt). Nichts zu aendern, richtig so belegt.
**Urteil Punkt 3: erfuellt.**

### 4.4 Punkt 4 — ROOM1171 (Elza), "Alright, let's get down from here!" (Lauf abn1_wd/1171, 79 s, exit 0)
Weg: Spielstand probe_r36_elza_karte 1131 (Figur 4) + CONTINUE in ROOM1131 -> `[pl] Spieler-Familie PL04 (character=4,
Elza-Bit=1)`; Aktion A an der Tuer -> `[aot] DOOR FIRE slot=3` -> ROOM1171; `[fire-aot] slot=5 at F60` -> `[evt] F61
room=1171 Evt_exec sub=2` (Dachszene). debug.log: msg 0 -> `clip loaded: main00.wav` (Regel) | msg 1 -> main01 (Regel) |
**`[msg] room=1171 id=3` -> `[voice] ROOM1171 msg 3 <- main02.wav (Runde 39 Spur A, Nutzer-Reihenfolge)`**; msg 2 wird nie
angefordert (je Id genau eine `[msg]`-Zeile: 0, 1, 3); keine `pegel`-Zeile; danach `[aot] DOOR FIRE slot=1` -> ROOM10B1
(`[evt] F1143 room=10b1`). Eigener Auswerter (Abschnitt 1 = ROOM1171, 1144 Bilder): main02 ab F1063 lag+2 korr 0,933
faktor 1,49 (4/4) +8,2 dB, Clip 68 Bilder -> endet ~F1133 VOR dem Tuer-AOT (F1143): die Zeile spielt ganz | main00 ab F510
0,967 | main01 ab F752 0,935; klemm 0. Bitgleich mit Abnahme 0 (Inhalt von main02 = dieser Satz: Abnahme 0 §3, DTW).
**Urteil Punkt 4: erfuellt** (unveraendert; Stufe 2 greift in 1171 nicht).

## 5. Vertrag, Tests, Nebenwirkungen

### 5.1 Vertrag (VERTRAG.md)
- Pfade (`git diff 275a322c HEAD --name-only`): engine/src, include, platform/pc/src/audio_pc.c, tests/unit + tests/integration
  (Registrierung ausschliesslich tests/unit/probes/r39_stimmen.cmake, kein CMakeLists), synchro/README.md, Dossier. Keine
  release/, platform/android/, shared_assets/PSX/, keine Nutzer-Datei unter synchro/ (§0).
- Gemeinsame Datei audio_pc.c: Haken der Nachbesserung = 1 include + 1 Aufruf + 4 Kommentarzeilen "Runde 39 Spur A
  (Nachbesserung 1)" (zusammen mit dem Abnahme-0-Haken 10 Zeilen der Spur in audio_pc.c; die Logik liegt vollstaendig in
  der eigenen Datei stimmen_pegel_r39.c). Stufe-1-Block unveraendert. Vertretbar ("kleiner Haken"), s. Hinweis 6.2/4.
- Ressourcen: keine Bank-9-Bits, keine neuen Nachrichten-Ids (29 ist eine vorhandene Port-Id der Irons-Szene,
  irons_tod_1150.c:134/:698), keine Slots; keine Umwidmung einer Id, zu der der Nutzer eine WAV hat (main22.wav ist
  laut Nachtrag 2 die Aufnahme ZU msg 29; main29.wav gibt es nicht).
- Linux-/Parallel-Form: stimmen_pegel_r39.c ohne libm (isqrt64), PSX-neutral (fprintf nur unter RE15_PLATFORM_PC;
  Standalone-Compile ohne das Makro: nur `-Wextra`-Warnung "unused parameter stamm", Projekt-Flags = -std=c11 ohne -Wextra);
  Compile mit -Wall -Wextra -Wshadow -Wconversion (PC) warnungsfrei; -m32 warnungsfrei. Riegel: Bestandsliste aus CMake-GLOB
  (Pfad `${CMAKE_SOURCE_DIR}/../synchro/...`, kein Backslash), Testlauf-Ordner je Test, exe-Kopien mit Praefix wie bisher,
  geerbte RE15_* geloescht; probe_r39_stimmen_auswertung linkt `m` bei NOT WIN32.
- Merge-Vorschau `git merge-tree --write-tree master HEAD`: Baum 3b7b0839, 0 CONFLICT — die Spur legt sich konfliktfrei auf
  den heutigen master (der um UEBERGABE/orchestrator-Doku weiter ist).

### 5.2 Tests der Spur (selbst gefahren)
- `ctest -R unit_r39_stimmen`: 3/3 (tabelle 162, pegel 282, lader; §1). Der Lader-Riegel am ECHTEN audio_pc.c zeigt die
  Stufe 2 nach dem Resampling: Irons msg 25..28 Faktor 12,80 / 9,32 / 7,74 / 11,60, alle anderen 1,00..1,97, msg 29 -> main22.
- Gezielte Gruppe (94 Tests, Regex des Dossiers §9.7 = Spur + alle beruehrten Module/Raeume, `-j1 --timeout 1500`,
  gestartet 12:10 parallel zu meinen Messlaeufen und zu fremden exe-Kopien anderer Spuren = Last): Ergebnis in §7.1.
- Einordnung: die Gruppen-Tests der Szenen mit Irons' ALTEN Zeilen (r35_cut1150, r36_szene1150, r38_kleinkram irons) laufen mit
  `RE15_NOAUDIO=1` (audio_pc.c:1954 "audio disabled", alle re15_audio_*-Aufrufe no-op) — ihr Gruen sagt NICHTS ueber die
  angehobenen Dateien aus (Dossier §9.7 stellt das missverstaendlich dar, s. 6.2/2). Deshalb die eigene Messung §5.3.

### 5.3 Nebenwirkungen (selbst gemessen)
- Erste Irons-Szene ROOM1150 (sub08, Irons' alte Zeilen main05/07/09/12/13 jetzt 6,9-9,2x statt 4x): Lauf abn1_wd/e1150e
  (Debug-Sprung 1150@120, Spieler im AUTO-Rechteck des Slots 6 — main00 @0x00DEA `2c 06 03 41 00 00 5c95 08a3 ec2c d007 ff 00
  18 08` = Aot_set Slot 6 sce 3 sat 0x41, x -27300..-15800, z -23800..-21800, Evt_exec sub 8 —, `RE15_FIRE_AOT=6@150`, Mitschnitt;
  vom Tuerspawn aus blieb Leons Plc_dest-Weg an z=-15446 haengen, Laeufe e1150b/c). Szene spielt msg 4..14 (`[msgtext]` +
  `clip loaded` main04..14; `[voice] pegel main05/07/09/12/13.wav ... (Spitzenbegrenzung C)` je genau einmal, z.B. main05 rms
  795 -> 1370 (x1.72), Spitze 12628 -> 21753 = bitgleich mit meinem Zensus §3). Eigener Auswerter (Abschnitt 1, 2401 Bilder):
  | Zeile | Datei | Faktor | Stimme/Rest | Spitze Summe |
  |---|---|---|---|---|
  | 4 Leon | main04 | 1,04 | -0,1 dB | 0,645 FS |
  | 5 Irons | main05 | **6,90** | **+1,1 dB** | 0,657 FS |
  | 6 Leon | main06 | 1,10 | +1,3 dB | 0,594 FS |
  | 7 Irons | main07 | **7,70** | **-3,1 dB** | 0,656 FS |
  | 8 Leon | main08 | 1,08 | -3,4 dB | 0,598 FS |
  | 9 Irons | main09 | **7,09** | **-3,9 dB** | 0,711 FS |
  | 10 Leon | main10 | 1,11 | +0,7 dB | 0,509 FS |
  | 12 Irons | main12 | **7,58** | **-2,9 dB** | 0,699 FS |
  | 13 Irons | main13 | **9,22** | **+1,2 dB** | 0,719 FS |
  | 14 Leon | main14 | 0,91 | -10,7 dB | 0,732 FS |
  klemm 0 im GANZEN Abschnitt (2401 Bilder), groesste Summe 0,732 FS: KEINE Uebersteuerung; Irons liegt in der ersten Szene
  jetzt im Band von Leons Zeilen derselben Szene (-3,9 .. +1,2 dB gegen Leon -3,4 .. +1,3 dB; Leons main14 ist mit -10,7 dB die
  leiseste Zeile der Szene — Nutzer-Datei auf Pegel, nicht Gegenstand der Spur).
- Sterbeszene: klemm 0 im ganzen Abschnitt, Summe max 0,773 FS (§4.2). Erstbesuch 1150 ohne Szene (e1150): BGM/SE allein
  Spitze 0,572 FS, 98 von 1501 Bildern ueber 0,336 FS — die Reserve 1 - C (0,336 FS) allein schuetzt also NICHT vor Klemme,
  gemessen klemmt aber nichts, weil Irons' Spitzen (0,2-0,5-ms-Transienten) und die BGM-Spitzen nicht zusammenfallen.
  Ein paar geklemmte Abtastwerte an einem Transienten waeren ohnehin unhoerbar; die Riegel zaehlen klemm = 0 je Zeilenfenster.
- 1050 / 11B0 / 1171: keine `pegel`-Zeile (Stufe 2 greift nur bei den 10 Irons-Dateien), Werte bitgleich mit Abnahme 0 (§4).
- Dateien ausserhalb des Riegel-GLOBs (Rekorder ROOM1100 `Irons_Rekorder_Sound*.wav`, `room1141/help01..05.wav`): rms 2072-3038
  >= T -> unberuehrt (§3). Elza-Varianten: kein Irons-Dialog in 1151 mit room1150-Dateien; die room1151-Dateien liegen auf Pegel.
- Rauschteppich der angehobenen Dateien (10. Perzentil der 20-ms-Fenster, eigene Messung): roh -66 dBFS (wie Leons -64..-71),
  angehoben -44,0 (main18) / -46,8 / -48,9 / -45,4 dBFS (main21) — in der Sterbeszene 15-20 dB unter dem Rest (rms 0,05-0,08 FS).
  Header-Angabe "-49..-42 dBFS" bestaetigt (ueber alle 10 Dateien).

## 6. Maengel und Hinweise

### 6.1 Maengel (nachpruefbar, nummeriert)
1. (klein, Dokumentation/Kommentar — nicht verhaltensrelevant) Header `re15_stimmen_pegel_r39.h` Z.~139 und Dossier §9.1
   behaupten fuer die 206 Dateien auf Pegel "Stufe 1 ... hoechstens 2,1x" (bzw. "1,00-2,11x"). Gemessen (§3, band_abn1.py):
   room1170/main00 2,71x, main02 2,99x, main03 3,00x (48 kHz, Spitze 4903), room1211/main03 2,11x. Die Konstanten T/R/C
   haengen davon nicht ab (Stufe 2 misst den Effektivwert, nicht den Stufe-1-Faktor; alle diese Dateien haben rms >= T und
   bleiben bitgleich — Pegel-Riegel 206/206 memcmp), die Riegel-Baender "Nicht-Irons 0,9..2,1 / 0,5..2,1" pruefen nur die
   Tabellenzeilen (Faktoren 1,00-1,97) und sind dort richtig. Zu korrigieren: der Satz im Header/Dossier (eine Zeile).
2. (klein, Dokumentation) Dossier §9.7 fuehrt r35_cut1150 / r36_szene1150 / r38_kleinkram_irons als Beleg, dass Irons' alte
   Zeilen "jetzt ebenfalls 6,9-9,3x ... angehoben werden — gruen"; diese Tests laufen mit RE15_NOAUDIO=1, der Lader laeuft dort
   nicht. Der Beleg fuer die erste Szene ist erst meine Messung §5.3 (e1150e). Zu korrigieren: der Satz im Dossier.
Beide Maengel aendern kein Verhalten und keinen Messwert; kein Nutzer-Punkt und kein Gate ist verletzt (§7).

### 6.2 Hinweise (keine Maengel; fuer Nutzer / Integration)
1. main19/main20 (Irons "not going to make it" / "one last favor") bleiben 4,5/4,9 dB unter der BGM MAIN1E — begrenzt durch
   einen 0,2-0,5-ms-Knall (main20 bei 2,97 s) bzw. die zweite Spitze (main19); ohne Klemme oder Begrenzer ist mehr nicht
   moeglich. Nutzer-Entscheidung: Neuexport (ohne Knall oder mit normaler Aussteuerung wie Leons Dateien).
2. C = 21759 ist die Spitze von room1240/main05 in der ROHEN Datei; nach dem Lader-Resampling liegt dieselbe Datei bei 21620
   (-0,06 dB). Die angehobenen Clips sind damit in der Spitze 0,6 % lauter als die lauteste gelieferte Datei im Spiel —
   praezisionshalber erwaehnt, ohne Folge.
3. Riegel-Schwelle Stimme/Rest >= 45 % (integration_r39_stimmen_1150) ist die Mitte zwischen Vorher (25-31 %) und Nachher
   (>= 55 %) — ein Regressionswaechter fuer die Stufe 2, kein Mass fuer "passt" (dafuer §4.2: Vergleich mit Leons Zeilen).
4. audio_pc.c traegt von der Spur jetzt 10 Zeilen (Abnahme-0-Haken 4 + Nachbesserung 1 include + 1 Aufruf + 4 Kommentar);
   VERTRAG §1.3 sagt "1-5 Zeilen" je Haken — die Logik liegt in eigenen Dateien, der Kommentar koennte auf eine Zeile schrumpfen.
5. Die Bestandsliste des Pegel-Riegels entsteht beim Configure (`file(GLOB ...)` ohne CONFIGURE_DEPENDS): neue Nutzer-Dateien
   zaehlt der Riegel erst nach einem erneuten Configure (Release-Baeume konfigurieren frisch; lokal `local_build.sh configure`).
6. stimmen_pegel_r39.c: ohne RE15_PLATFORM_PC ist `stamm` ungenutzt (nur mit -Wextra eine Warnung; Projekt-Flags -std=c11
   ohne -Wextra) — `(void)stamm;` wuerde sie vermeiden. PSX-Bau hier nicht gebaut (kein PSn00bSDK), Datei ist libm-frei und
   nutzt int64 (libgcc) wie andere Engine-Dateien.
7. Leons main14 der ersten Irons-Szene ("Yes sir...") ist mit -10,7 dB unter der BGM die leiseste Zeile dieser Szene (rms 0,038
   FS, ueber T, Stufe 2 greift nicht) — Nutzer-Datei, nicht Gegenstand dieser Spur.

## 7. Gates und Urteil

### 7.1 Gezielte Testgruppe (Regex Dossier §9.7, `ctest --test-dir re15_port/build -R "..." --timeout 1500 -j1 --output-on-failure`)
Lauf 12:10:xx-12:47:15, sequenziell, WAEHREND meiner eigenen Messlaeufe (1050/1150/v1150/11b0/1171/e1150*) und fremder exe-Kopien
anderer Spuren (`re15_pc_r39fg_haken_*`, `re15_pc_r36j_haken_*` in der Prozessliste) = Last:
**`100% tests passed, 0 tests failed out of 94` (Total Test time (real) = 2206.72 sec), CTEST_EXIT=0.** Kein roter Haken, keine
Wiederholung noetig. Darunter unit_r39_stimmen_tabelle/pegel/lader, integration_r39_stimmen_1050 35,1 s / 1150 57,8 s /
11b0 107,1 s / 1171 58,2 s, integration_r38_elzaintro_stimme 124,9 s, r38_rekorder, r38_werkstatt11b0, r38_heliport1171 (3),
r36_elza_* (5 Integrationen), r36_szenenleon, r35_cut1150, r36_szene1150, r38_kleinkram (3). Log: Scratchpad ctest_gruppe.log.

### 7.2 Gates
| Gate | Ergebnis |
|---|---|
| Bau (local_build.sh configure+build) | OK; exe 10:17:40 = HEAD-Code (§1), traegt den Stufe-2-String |
| RE-Gate (Konstanten mit Herkunft: T/R/C aus dem Bestand = PORT-WAHL in der Form, NUTZER-VORGABE im Grund; 3+ Adressen selbst disassembliert; keine Guess-Tells; kein Env-Schalter) | haelt (§2); Zensus unabhaengig reproduziert (§3) |
| Fix erklaert den Befund (Vorher master-exe / Nachher Spur-exe am selben Weg, gleiche Bilder) | ja (§4.2): Irons 4,0x -> 7,9-12,8x, msg 29 stumm -> main22; Leon bitgleich |
| Nutzer-Punkte 1-4 + Nachtrag 2 (M1 Irons lauter, M2 main22, M3 Text) am echten Weg gemessen | alle erfuellt (§4) |
| Vertrag (Pfade, Ressourcen, kleine Haken, Linux-/Parallel-Form, Merge konfliktfrei) | haelt (§5.1) |
| Tests der Spur (Unit 3/3, Integration 4/4, je Punkt messend) + Gruppe 94/94 | gruen (§5.2, §7.1) |
| Nebenwirkungen (erste Irons-Szene, Nachbarraeume, Elza-Varianten, Dateien ausserhalb des GLOBs, Uebersteuerung) | keine unerwuenschten (§5.3): klemm 0 in allen Abschnitten |

### 7.3 Urteil je Nutzer-Punkt (gegen den Wortlaut, AUFTRAG.md inkl. Nachtrag 2)
| Punkt | Wortlaut (gekuerzt) | Urteil | Beleg |
|---|---|---|---|
| 1 | 1050: Voiceover vor der Ada-Rettung (main09-11) spielt nicht | **erfuellt** | §4.1: msg 22/23/24 <- main09/10/11, Mitschnitt korr 0,994/0,981/0,938, am echten Weg; unveraendert seit Abnahme 0 |
| 2 | 1150: Voiceovers der Sterbeszene nicht drin; Nachtrag 2: "ruhig lauter anheben, dass es passt", "SIR, sir! ... mit main22 ergaenzt" | **erfuellt** (Hinweis 6.2/1) | §4.2: 8/8 Zeilen im Ton (msg 29 <- main22 korr 0,828), Irons 12,80/9,20/7,86/11,60x statt 4x (+5,6..+9,2 dB), main18 auf Leons Niveau, main21 +17 dB, main19/20 -4,5/-4,9 dB (Transient-begrenzt, dem Nutzer gemeldet), 0 geklemmte Abtastwerte, Leon bitgleich; Vorher/Nachher am selben Weg |
| 3 | 11B0: "But hold on" + "... unexpected Visitors" fehlen; Nachtrag 2: Text "unwanted followers" | **erfuellt** | §4.3: msg 20 <- main13 (0,905), msg 21 <- main14 (0,957); `[msgtext] id=21 "... unwanted followers."` — Text ist schon so, Aufnahme spricht ihn |
| 4 | "Alright, let's get down from here" ohne Voiceover | **erfuellt** | §4.4: Elza PL04, msg 3 <- main02 (0,933), Clip endet vor dem Tuer-AOT; msg 2 nie angefordert |

### 7.4 Gesamturteil
**BESTANDEN.** Alle vier Punkte und die drei Auftraege des Nachtrags 2 am gebauten Stand (HEAD ad16c58f, exe = HEAD-Code), am
echten Weg, mit eigener exe-Kopie, eigenem Zensus und eigenem Auswerter gemessen; Vorher/Nachher mit der master-exe erklaert den
Befund; RE-Gate, Vertrag, Tests (3 + 4 + 94/94) und Nebenwirkungen (inkl. erster Irons-Szene, die kein Gruppen-Test mit Ton
abdeckt) halten. Zwei kleine Dokumentations-Maengel (§6.1: "hoechstens 2,1x"-Satz, NOAUDIO-Beleg) aendern kein Verhalten und
keinen Messwert — in der Integration oder einer Dossier-Korrektur mit je einer Zeile zu beheben. Offen sind Nutzer-Entscheidungen
(§6.2/1: main19/main20 bleiben ~4,5 dB unter der Musik, Neuexport).

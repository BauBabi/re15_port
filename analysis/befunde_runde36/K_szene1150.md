# Runde 36 — Spur K "szene1150" (Irons-Todesszene / Montage)

Baum: `.claude/worktrees/r36_szene1150` (Zweig `r36/szene1150`, Basis master 78a4a748).
Eigentuemer-Modul: `re15_port/engine/src/irons_tod_1150.c` (Spur L r35, Dossier `analysis/befunde_runde35/L_cut1150.md`).

## Auftrag (woertlich, AUFTRAG.md Z. 67-71)

> - Bei der finalen Cutscene mit Irons wo er stirbt, ist das Türknallen noch falsch. Das ist kein Türknallen, das ist einfach nur der Sound von Tür schließen. Suche den Tür Knall Sound von zum Beispiel Resident Evil 2.
> Ich würde bei door Sound, oder Zombies Breaking through Wood suchen ob dort eine Tür Knall Sound/Einbruch Sound zu finden ist.
> - Ebenfalls in der Cutscene wo Marvin und Ada in ROOM 11C0 stehen, ist der Dialog mit Marvin und Ada noch schrott. sie unterhalten sich nicht miteinander, sondern schauen irgendwo hin. Das musst du noch verbessern, das das besser passt. Sie müssen sich schon miteinandern unterhalten.
> - Ebenfalls in der Cutscene kommt das 'Türknallen' Dann die Kamera Ansicht und dann die Zombies aus dem nichts. Das soll so auch nicht sein. Erst 'Türknallen' Dann Zombie Spawnen und dann erst der Kamera Wechsel hin zu den Zombies.

Punkte: K1 = Tuerknall-Ton (RE2), K2 = 11C0-Dialog Marvin/Ada einander zugewandt, K3 = Reihenfolge Knall -> Spawn -> Kamera.

## Fortschritt (fortlaufend)

- [x] Dossier angelegt
- [x] Bestand gelesen (irons_tod_1150.c, knall_bank.inc, L_cut1150.md §0-§10, AUFTRAG r35 Z.79-90)
- [x] K1 RE2-Beleg (Zensus der Kandidaten-Raumbaenke, Se_on-Stellen, Wellen/Spektren) — §K1.2
- [ ] K1 Messung vorher / Umsetzung / Messung nachher
- [ ] K2 Messung vorher / Beleg / Umsetzung / Messung nachher
- [ ] K3 Messung vorher / Beleg / Umsetzung / Messung nachher
- [ ] Tests (probes/r36_szene1150.cmake)

## Bestand (Runde 35, Spur L — gelesen, nicht vermutet)

* Der "Tuerknall" ist Satz 0 der eingebackenen Knall-Bank `engine/src/gen/knall_bank.inc` (Werkzeug
  `tools/r35_l/knall_cut.py`): **RE2 DOOR04.DO2 Tone 2 = EDT-Satz 1 `00 00 24 17` = Door_exit** (der
  Schliesston einer RE2-Tuersequenz), VAG 3, 9216 B, 16100 Abtastwerte. Gespielt ueber das Signal (5,28)
  (`22 05 1c 01`) -> Port-Takt `re15_irons_tod_tick` -> `re15_audio_re2_tuer_se(KNALL_SE_TUER)`. Satz 1 =
  RE1.5 ROOM1030 Se_on 0x0c (der Knall "wie bei der Cutscene von ROOM 1030", Signal (5,29)) — vom Nutzer
  nicht beanstandet, bleibt.
* Stellen, an denen Satz 0 laeuft (irons_tod_1150.c): Ende der Szene 1150 (`k_p_szene`, vor dem Tuersatz),
  ROOM1130 Cut 0 (`k_p_1130`, 15 Bilder nach dem Cut), ROOM1030 Cut 7 (`k_p_1030_tuer`, 15 Bilder nach dem Cut).
* Der Nutzer hat Recht mit "nur der Sound von Tuer schliessen": es IST der Door_exit-Satz einer RE2-Tuer.

## K1 — Tuerknall (RE2)

### K1.1 Messung vorher
(folgt nach dem Bau, §K1.4)

### K1.2 RE2-Beleg: welcher Ton spielt in RE2, wenn Zombies gegen eine Tuer schlagen / durchbrechen
Werkzeug `re15_port/tools/r36_k/knall_zensus.py` (Raumbank snd0 = RDT-Kopfworte +0x08/+0x0C/+0x10; jeder EDT-Satz
-> Programm/Tone/Lagen/VAG, ADPCM selbst dekodiert, Kennzahlen; jede Se_on-Stelle im SCD ueber den r30-Walker).
Kandidaten = Nutzer-Liste `RE2_SOUND_LISTE.md` Z.7-8 (ROOM205 "Zombies smashing through wood", ROOM106 "Cabin
sounds - Zombies breaking in", ROOM112 Library, ROOM207/209/20A "Door sounds", ROOM204 Shutter) = RE2-RDTs
`info/re2leon/PL0/RDT/ROOM2050/1060/1120/2070/2090/20A0/2040.RDT`.

**Abspielweg RE2 (selbst disassembliert, info/re2leon/PSX.EXE):** Se_on (0x36) -> `jal 0x8005ba28`; dort
`8005ba30 srl t1,a0,24` (Bank = pc[1]), `8005ba7c srl v0,a0,16` / `8005ba80 andi s7,v0,0xff` (Satz = pc[2]),
`8005ba8c lw a1,-17544(at)` = EDT-Zeiger aus 0x800dbb78 + Bank*4, `8005ba98 lw v0,0(a1)` / `8005baa0 beq v0,v1`
(Satz `ff ff ff ff` = leer -> nichts). Bank 2 = Raumbank: der Raumlader schreibt `80059f1c lw a1,8(a2)` (RDT-Wort
+0x08 = EDT) -> `80059f44 sw a1,-17536(at)` = 0x800dbb80 = 0x800dbb78 + 2*4.

| Raum (Liste) | Satz | Se_on-Stellen (Datei-Offset) | Szene im RE2-Skript | Lagen -> VAG | Welle |
|---|---|---|---|---|---|
| **ROOM1060** "Zombies breaking in" | **0x0a** EDT @0x03F80 `00 00 85 20` | **sub05 @0x00F48** `36 02 0a 01 00 00 dc 88 f8 f8 3d 05` | Leon betritt die Huette: AOT 10 (@0x00D8E) -> sub04 schaltet AOT 11 scharf (@0x00F1A `46 0b 05 41 ff 00 18 05`) -> **sub05**: Set(4,0x0a,1), die Tuer nach draussen wird zur Szene sub11 (@0x00F34), dann **EIN Schlag** gegen diese Tuer (Position (-30500,1341) = 2500 hinter dem Tuer-Rechteck AOT 0 @0x00BBE x -27965..-26735), Sleep 30, danach nur noch 0x0b in der Schleife (@0x00F68..@0x00F88 `17 ff ff 00 e0 ff` springt hinter den Schlag) | Tone 8 + Tone 9 (byte3 0x20 -> 1 Zusatzlage), beide VAG 5 @Datei 0x066B8, 8768 B, center 91 / min 67 -> 11025 Hz | 1,39 s; zwei harte Schlaege bei 0 und ~0,25 s (Huellkurve 50 ms, beide Lagen gemischt: 15535 21279 18517 13760 19206 **23572** 20345 ... ), Spitze je Lage 32415 |
| ROOM1060 | 0x0b `00 00 ad 00` | sub05 @0x00F58, @0x00F72 (Schleife) | das anhaltende Geraeusch danach | VAG 6, 7796 Hz | 2,80 s, langsam an-/abschwellend, harmonische Linien (Stoehnen/Rumoren), kein Schlag |
| ROOM1060 | 0x0c `00 00 e3 00` | sub11 @0x01184 (Bezug 4) | Szene beim Versuch hinauszugehen | VAG 10 | 0,96 s, Spitze 19966, leise |
| **ROOM2050** "smashing through wood" | **0x0b** EDT @0x06E68 `00 00 95 40` | sub05 @0x01E72, @0x01EA4 (je mit `8a 00 08 ..`/`8b ..` = Bildschuetteln) | die Bretter-Barrikade ("The window is boarded up", msg 0) bricht: sub05 -> sub06/07 bzw. sub09/10 + Objekt 0/1 Member_set(0x17,1) | Tone 9 (VAG 4) + Tone 10/11 (VAG 6, 22050 Hz) | 0,99 s, breitbandiges Krachen/Splittern, gemischt Spitze 86757 (3 Lagen), Huellkurve 9990 24126 25754 26987 23500 ... |
| ROOM2050 | 0x0a `00 00 75 40` | sub03 @0x01DC0, sub04 @0x01E50 (nach Cut_chg 9/10) | Bretter-Teil faellt | 3 x VAG 4 (verschiedene Tonhoehen) | 0,87 s, Holzkrachen |
| ROOM2070 "Door sounds" | 0x0a `00 00 64 00` | **keine** Se_on-Stelle im SCD | — | VAG 5 | 1,26 s, mehrere leise Klopfer (Spitze 30132, Huellkurve <= 8629) |
| ROOM2090 "Door sounds" | 0x28 `00 00 44 16` | sub02 @0x00F3A, sub03 @0x01004 (Cut_chg 4/5) | Schloss/Spind mit dem Dietrich (msg "The lock is broken") | VAG 7 | 0,80 s, Klick + Schnappen |
| ROOM20A0 "Door sounds" | 0x16 `00 00 54 16` | sub02 @0x00B56, sub03 @0x00B6A (mit msg 0 "It's locked...") | verschlossene Tuer ruetteln | VAG 4 | 0,80 s, Ruetteln |
| ROOM1120 Library | 0x13 `00 01 d4 40` | sub17 @0x03618 (mit Bildschuetteln) | der Bibliotheksboden bricht ein (Leon faellt) | 3 x VAG 25 | 1,59 s, Boden-Krachen |
| ROOM2040 Shutter | 0x0b..0x0e, 0x11 | sub11/12/16-21 | Rolltor-Schalter, Funken | VAG 3/4/10/11 | Klicks 0,07 s, Funken |
| (IST) DOOR04.DO2 | Satz 1 `00 00 24 17` | Tuersequenz Door_exit | Tuer faellt ins Schloss | VAG 3, 16397 Hz | 0,98 s: Klinke + ein Schliess-Thud (Huellkurve 4528 2776 7671 11609 13335 7680 ...) |

Bild: `K_belege/k1_kandidaten_wellen_spektren.png` (Welle + Spektrogramm je Kandidat, Lagen wie in der Engine
gemischt; `tools/r36_k/spektro.py`).

**Auswahl (PORT-WAHL mit Beleg, RE2-Ergaenzung "Sound ist RE2"):** RE2 **ROOM1060 Se_on 0x0a** — der EINZIGE Ton
der Liste, den RE2 als einmaligen Schlag GEGEN EINE TUER spielt, an der Zombies stehen (sub05 @0x00F48, Position
hinter der Tuer, danach nur das Rumoren 0x0b). Genau das ist die Lage der Szene: hinter einer Tuer (Briefing Room
1140 -> 1130; Tuer 1070 -> 1030) schlagen Zombies die Tuer auf. Welle: zwei harte Schlaege in 0,25 s statt Klinke +
Schliess-Thud; Lagen-Mischung Huellkurven-Maximum 23572 gegen 13335 des Door_exit-Satzes (+5 dB).
**Alternative** (im Dossier fuer den Nutzer): ROOM2050 Se_on 0x0b (Bretter-Barrikade bricht, Holz splittert,
im Original mit Bildschuetteln) — ein Einbruch DURCH Holz, kein Schlag gegen eine Tuer. Verworfen: ROOM2070 0x0a
(Klopfen, im Skript nie gespielt), ROOM2090/20A0 (Schloss/Ruetteln), ROOM1120 0x13 (Boden bricht), ROOM2040
(Rolltor-Klicks).


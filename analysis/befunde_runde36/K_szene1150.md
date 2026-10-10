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
Echte exe (Stand 78a4a748 gebaut, Messkopie `re15_pc_r36k_mess.exe`), Weg wie integration_r35_cut1150 Lauf A:
Karte `probe_r35_cut1150_karte re15_card.mcr 1130 nach10f0 ersteszene`, CONTINUE in ROOM1130, Aktionstaste an der
Tuer -> 1150 -> ganze Kette bis `RE15_EXIT_AT=510#11C0`; Ton per `RE15_AUDIO_CAP_SYNC=cap.raw` (deterministischer
Mitschnitt ohne Audiogeraet); Werkzeug `tools/r36_k/kette_lauf.sh` (Lauf `vor_11c0`).
Auswertung `tools/r36_k/knall_im_mitschnitt.py cap.raw` (normierte Kreuzkorrelation der ersten 0,4 s jeder Referenz):
```
Mitschnitt: 5240550 Abtastwerte = 118.8 s = 3565 Spielbilder
DOOR04   (16397 Hz): t=63.63 s Bild 1909 r=0.997 Huelle-max 3335; t=65.83 s Bild 1975 r=0.899 Huelle-max 3184; t=84.13 s Bild 2524 r=0.967 Huelle-max 3500
RE2_1060 (11025 Hz): kein Treffer >= 0,5
R1030    (11025 Hz): t=71.87 s Bild 2156 r=0.982 Huelle-max 4243; t=89.97 s Bild 2699 r=0.852 Huelle-max 4818
```
= der "Tuerknall" an den drei Stellen (Ende 1150, 1130 Cut 0, 1030 Cut 7) ist der Door_exit-Satz von DOOR04
(Befund des Nutzers reproduziert); debug.log: `[se] Stimme: se=0 ... center=79 ... pitch=0x5f3 (16397 Hz)` je Knall.

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

### K1.3 Umsetzung
* `re15_port/tools/r35_l/knall_cut.py` (Generator der Knall-Bank, Eigentum dieses Moduls) neu: Satz 0 = RE2 ROOM1060
  (Pruefung gegen die RDT-Bytes: Kopf-Offsets EDT @0x03F58 / VH @0x04018 / VB @0x04C38, Se_on @0x00F48, EDT[0x0a]
  `00 00 85 20`, Tone 8/9 -> VAG 5 @0x066B8 8768 B; VH-Master 127 und Programm-mvol 0x7f in ROOM1060, ROOM1030 und
  DOOR04 gleich -> dasselbe RE2-Pegelgesetz). Bank: Tone 0/1 = RE2 Tone 8/9 (Feinstimmung 0/1 wie im Original), Tone 2
  = ROOM1030 Tone 7; SE-Map Satz 0 `00 00 05 20` (Prio-Nibble 5 und byte3 0x20 = Direkt-Stimme + 1 Zusatzlage
  woertlich aus RE2), Satz 1 `00 00 27 15` (aus `00 00 77 15`). Der Kopf traegt genau EIN Programm (VH 0xC20 B,
  re15_vab_parse verlangt die Groessentabelle hinter den Tone-Segmenten) - deshalb ein Programm mit 3 Tones.
* `engine/src/gen/knall_bank.inc` neu erzeugt (19976 B; VAG 1 sha1 192e09df..., VAG 2 sha1 03dacdcd... unveraendert).
* Kein neues Asset fuer das Paket-Gate (eingebacken wie bisher), keine Aenderung an audio_pc.c.

## K2 - Dialog Ada/Marvin in ROOM11C0 Cut 13

### K2.1 Messung vorher
Lauf `vor_11c0` (echte exe, Kette wie K1.1, `RE15_FRAMEDUMP=0-510/10`, `RE15_NECK_LOG`), Kontaktbogen
`K_belege/k2_vor_11c0.png` (Bilder 10..460): Ada und Marvin stehen nebeneinander mit dem Ruecken zur Kamera, drehen
sich beide zum Gebaeude (= zur Kamera, die Tuer Slot 0 @0x01712 liegt hinter Cut 13) und sprechen "What was this
noise?", "Yes!....", "Oh, no, Leon!" frontal in die Kamera (F100..F280); Marvin dreht sich erst bei F310 zu Ada, Ada
wendet sich Marvin NIE zu und ruft "Marvin!..." in die Kamera. neck.log: in ROOM11C0 kein einziges Plc_neck (nur die
vier von Leon in 1150). debug.log: Plc_dest Modus 9 nur `slot=1/4 dest=(-27100,15900)` (beide zum Gebaeude) und
`slot=4 dest=(-8965,-14347)` (Marvin zu Ada, erst vor msg 13). = der Befund des Nutzers ("schauen irgendwo hin").

### K2.2 Beleg (Form aus dem Original-Dialog DESSELBEN Raums)
ROOM11C0 sub02 (Leon/Ada-Ankunftsszene, `scd_dump_room.py`, Datei-Offsets):
```
0x01886 Plc_neck  41 01 fb dc 00 00 f5 c7 64 00   Leon: Kopf auf Ada (-8965,0,-14347), Modus 1 Weltpunkt, Rate 0x64
0x01890 Plc_dest  40 00 09 00 fb dc f5 c7         Leon: Drehen (Modus 9) zu Ada, Ankunfts-Bit 0
0x01898 Gosub     18 05                           warten (sub05 @0x01C3E: Set(5,0)=0; While Ck(5,0)==0 {Evt_next})
0x018C0 Plc_neck  41 01 cb e4 00 00 33 cf 64 00   Ada (Work 2,0): Kopf auf Leon
0x018CA Plc_dest  40 00 09 01 cb e4 33 cf         Ada: Drehen zu Leon, Ankunfts-Bit 1
0x018D2 Gosub     18 06                           warten (sub06 @0x01C50, Bit 1)
0x018D4 Message_on 2b 01 00 00 / 0x018D8 3f 00 13 00 / Sleep 40 / 3f 00 13 00 + 43 00 80 00 / Sleep 50  (Adas Zeile + Geste Clip 19)
0x01940/0x01952 Plc_neck 41 01 df f1 00 00 7a ba 64 00  beide blicken auf denselben Punkt
0x01964/0x01974 Plc_dest 40 00 09 00 / 40 00 09 01 ...  beide drehen sich dorthin (Sleep 10 dazwischen), dann 18 06
```
sub03 @0x01BB2..@0x01BCA (Ada nickt): `41 00 00 00 00 00 00 00 64 00` (Kopf frei), Sleep 5,
`41 03 01 00 00 00 00 00 00 3c` (Modus 3 = Nicken), Sleep 20. Zensus Plc_neck Modus 1 in allen RDTs: 200 Saetze,
y = 0 in 155 (Gespraechspartner stets mit y 0 und Rate 0x0064: 10D0 sub21, 11B0 sub06, 11C0 sub02/sub03).
Engine (Port, byte-true): Plc_dest Modus 9 fuer NPC = Dreh-Sub `re15_npc_sub_turn` (Kegel 0x60 fuer Typ 0x40/0x42,
Tabelle @0x80076c41; Ankunfts-Bit `FUN_8004ef90(0x800b1028, +0x1c3)` @0x80051dcc-dd8); Plc_neck Modus 1 =
`+0x1b8 |= 0x84` (@0x80041ed8-f10), Kopf-FSM skeleton_common.c. Bank-5-Bits 0/1 = die Ankunfts-Bits von sub05/sub06
(Wort 0, nur beim Raum-Init geloescht); Leon ist waehrend der Montage nicht im Raum.

### K2.3 Umsetzung (irons_tod_1150.c Programm 4 `k_p_11c0`)
Beats der Nutzer-Vorgabe r35 bleiben (AUFTRAG r35 Z.83-90), "einander zugewandt" (r36) in der Form oben:
1. beide zum Gebaeude: Marvin Kopf+Drehen (Bit 0), Sleep 10, Ada Kopf+Drehen (Bit 1) + `18 06` (Form @0x01964..@0x0197C);
2. Marvin Kopf auf Ada; Ada Kopf+Drehen auf Marvin + `18 06`; msg 10 + Adas Geste Clip 19 vor/zurueck (@0x018D8..@0x018E8);
3. Marvin Drehen auf Ada + `18 05`; msg 11; Kopf frei, Sleep 5, Nicken (sub03 @0x01BB2..@0x01BCA); Kopf wieder auf Ada;
4. Marvin Kopf+Drehen zum Gebaeude, msg 12 "Oh, no, Leon!" (Ada sieht ihn weiter an);
5. "Marvin dreht sich zu Ada": Kopf+Drehen auf Ada + `18 05`; msg 13 + Arm (Clip 15, wie r35);
6. Kopf frei, rennt zur Tuer (Modus 5, wie r35);
7. Ada Kopf+Drehen zur Tuer + `18 06` (sieht ihm nach); msg 14 + Arm (Clip 15, wie r35).
Orte: Ada sub00 @0x01770 (-8965,-14347), Marvin (-8163,-13748) (r35 M5, PORT-WAHL mit Herleitung), Gebaeude = Mitte
des Tuer-Rechtecks Slot 0 @0x01712 (-25100,17250) (r35 drehte auf die Rechteck-Ecke (-27100,15900)). Sleeps = PORT-WAHL
im Takt der r35-Fassung; die Szene dauert jetzt 500 statt 518 Bilder bis zum Schnitt.

## K3 - Reihenfolge Knall -> Spawn -> Kamera

### K3.1 Messung vorher
Echte exe (Stand 78a4a748, Messkopie `re15_pc_r36k_vor.exe`), Kette wie K1.1, je Raum ein Lauf mit
`RE15_EXIT_AT=<n>#<Raum>` und `RE15_FRAMEDUMP` (Laeufe `vor_1130` 170#1130 /2, `vor_1040` 120#1040 /4, `vor_1030`
260#1030 /4); state.log (eine Zeile je Bild, je Gegner `[slot t=..]`, cam=, pf=) des letzten Raumbesuchs:
```
ROOM1130 (F1..F170): F1 cam=0 gegner=0 pf=FF000007 (Einblendung); F6 cam=0 gegner=0 pf=01000007; F31 cam=0 gegner=5
ROOM1040 (F1..F120): F1 cam=1 gegner=5 pf=FF000007 (Raum-Records an IHREN Plaetzen); F6 Aufstellung hinter das Tor
ROOM1030 (F1..F260): F1 cam=7 gegner=6 (hinter dem Tor, nicht im Bild); F31 cam=7 gegner=11; F181 cam=6 gegner=11;
                     F196 cam=6 gegner=14; F218 cam=12
```
debug.log: 1130 `[scd F6] Cut_chg(0)` -> `Signal (5,28): Tuerknall` (F21) -> 5x `[spawn-diag] Sce_em_set`;
1030 `[scd F6] Cut_chg(7)` -> `Signal (5,28)` -> Kopien; `[scd F181] Cut_chg(6)` -> Se_on @0x02776 -> Kriecher ->
`[scd F218] Cut_chg(12)`. Bilder: `K_belege/k3_vor_1130.png` (F6..F30 leerer Flur auf Cut 0, F32 stehen die fuenf
Zombies da = "aus dem Nichts"), `K_belege/k3_vor_1030.png` (F24/F28 leerer Gang Cut 7, F32 die Kopien; F192 Cut 6,
F196/F200 erscheinen hinter dem Gitter weitere Zombies im Bild). = der Befund des Nutzers ("Tuerknallen, dann die
Kamera Ansicht und dann die Zombies aus dem Nichts"): Kamera (Tuer-Cut ab Bild 1) VOR Knall (Bild 21) VOR Spawn (Bild 31).

### K3.2 Beleg
* Im Original erscheinen die Gegner eines Raums im INIT-LAUF, bevor das erste Bild eingeblendet wird: Tuer-Warp
  FUN_8001d600 -> Raumlader `jal 0x800396fc` @0x8001d988 -> darin `jal 0x8003ef6c` @0x80039a00 (SCD-Raum-Init);
  FUN_8003ef6c ruft den Dispatcher FUN_8003f0a0 DIREKT @0x8003f018 (main00 + sub00, deren Sce_em_set); erst danach
  laeuft die Einblendung (State-3-Rumpf @0x8001cbb8..@0x8001cc28: `jal FUN_800217b0` @0x8001cc00 = re15_fade_config(0,2,7,
  -0x1800,0), `jal FUN_800216ec` @0x8001cc18), und waehrend der Einblendung steht die Logik (`g_pauseflags |= 0xff000000`
  @0x8001cc5c/@0x8001cc6c, darin SCD 0x02000000; Freigabe @0x8001cc84/@0x8001cc94). Ein Faden, den sub00 im Init-Lauf per
  Evt_exec startet, laeuft im selben Lauf mit (Slot-Schleife ohne Aufschub, scd_vm.c "Evt_exec @0x8003f2a0").
  Port: room_common.c (Adressen dort), scd_room_setup.c (Init-Lauf `scd_vm_set_room_init(1); scd_vm_tick();`, danach die
  Installer). Der Montage-Faden startete bisher erst NACH dem Init-Lauf (Installer) und lief damit erst nach der
  Einblendung (Bild 6).
* Die Original-Szene ROOM1030 sub08 selbst hat die verlangte Reihenfolge: @0x02776 Se_on (Knall) -> @0x02782..@0x02792
  Tor bricht -> @0x0279E/@0x027A4 Gosub sub09 (die schon stehenden Zombies kriechen) -> @0x027AA `29 0c` Cut_chg 12.
* Knall-Signale (5,28)/(5,29): Port-Takt am Kopf des VM-Laufs (r35, re15_irons_tod.h).

### K3.3 Umsetzung (irons_tod_1150.c)
* `erster_abschnitt_sofort(slot)` + `starten(1)`: die Montage-Schritte (1130/1040/1030/11C0) und die Nachspawns
  (1130/1040/1030 bei spaeterem Betreten) laufen ihren ersten Abschnitt (bis zum ersten Sleep) noch im Raumaufbau - ein
  zweiter Init-Lauf NUR fuer diesen Faden (die anderen Faeden des Raums ruhen dabei: aktiv-Bits gemerkt und
  zurueckgelegt; sub01-Reseed und Port-Takt laufen im Init-Lauf ohnehin nicht). Keine Aenderung an scd_vm.c /
  scd_room_setup.c. Szene und Rueckkehr in 1150 unveraendert.
* Knall jedes Schritts an das ENDE des vorigen (noch auf dessen Kamera, 45 Bilder vor dem Tuersatz wie r35 in 1150):
  1150 -> Satz 0 (1130) bzw. Satz 1 (1140 leer -> 1040), 1130 -> Satz 1 (1040), 1040 -> Satz 0 (1030 Cut 7) bzw. Satz 1
  (1070 leer -> Tor-Teil); `knall_setzen()` stellt das Signal-Byte. Die Knaelle zu Beginn von 1130/1040/1030-Cut-7 entfallen.
* 1130: Zombies im Raumaufbau, Sleep 130, Knall 1040, Sleep 45, Tuer (Tuer weiter nach 175 Bildern ab dem ersten Takt).
* 1040: Kopf + Aufstellung im Raumaufbau (Cut_chg 1 entfaellt, Tuer-Cut 1), Vorlauf 35 (= r35 15 + 20), Tor-Fahrt, Ende.
* 1030 mit Cut-7-Teil: Kopien im Raumaufbau, Sleep 175, Original-Knall @0x02776 auf Cut 7, Bruch (nur (4,15)=0), Sleep 10,
  Kriecher (hinter dem Tor, nicht im Bild von Cut 7), Sleep 2, sub09 x4, Sleep 20, Cut_chg 12 (@0x027AA). Ohne Cut-7-Teil:
  Tuer aus 1040 -> Cut 12, Bruch + Kriecher im Raumaufbau (r35: Cut 6 bzw. 12).
* 11C0: Marvin im Raumaufbau (r35: erschien in Bild 6 nach der Einblendung).
* Mess-Haken (kein Spielverhalten): Ereignisfolge `re15_irons_tod_folge` + debug.log `[irons-tod] Folge #n K|S|C ...`
  (Knall beim Abspielen, Spawn = Gegner beim ersten Bild bzw. Zuwachs im Takt, Kamera = Tuer-Cut beim ersten Bild bzw.
  Cut_chg) und `[irons-tod] ROOMxxxx: erstes Bild mit n Gegnern (Init-Lauf a + erster Abschnitt b)`.

# Runde 35 Spur L "cut1150" — Unabhaengige Abnahme 1 (nach Nachbesserung 1)

Datum 2026-10-03. Baum `.claude/worktrees/r35_cut1150`, Zweig `r35/cut1150`, HEAD `4ea1d006` (Basis master `154a73c1`).
Geprueft gegen den WORTLAUT `AUFTRAG.md` Z. 68-91, `VERTRAG.md`, Dossier `L_cut1150.md` §9, Abnahme 0 (`L_abnahme_0.md`).

**Ergebnis: NICHT BESTANDEN.** Punkt 1, 2 und 4 erfuellt. Punkt 3 teilweise: der ROOM1040-Teil ("5 Zombies durch kommen")
haelt in den Faellen der Riegel und des Bau-Agenten (alle 20 Raum-Records leben / Tor offen / "18 tot" mit Records 0+1
lebend), bricht aber in der NATUERLICHEN Totschlag-Reihenfolge: hat der Spieler in ROOM1040 vorher 12 Zombies getoetet,
kommen 3; hat er 15 getoetet, kommt KEINER, und die Kamera steht 1090 Bilder (36 s) auf dem leeren Tor (M1). Die Maengel
M1-M7 der Abnahme 0 sind sonst behoben und an der exe nachgemessen.

## 0. Bau und Messaufbau

* `bash re15_port/tools/local_build.sh configure` + `... build` -> `ninja: no work to do`, `=== LOCAL-BUILD-OK (build)`.
  exe md5 `8561eda44e5d4d866a27b7482993605b` (gebaut 22:07 nach dem letzten Code-Commit 2acecf9a 22:03; danach nur
  Dossier). Messkopie `re15_pc_abn1L.exe` (gleiche md5) im selben Verzeichnis.
* Volle Suite: Dossier §9.11 traegt woertlich `=== LOCAL-BUILD-OK (all) — Tests 491/491` (Code-Stand = HEAD) -> nicht
  wiederholt. Selbst gefahren (PowerShell): `ctest --test-dir <baum>/re15_port/build -R "r35_cut1150|^unit_se_bank_routing$|
  ^unit_r34_plattform$|^unit_r34_plattform_ton$" --timeout 900` -> **16/16 gruen**, integration_r35_cut1150 429 s
  (Log scratchpad `L_abn1/ctest_r35l.log`).
* Messlaeufe (scratchpad `.../scratchpad/L_abn1/runs/<lauf>/`, je Lauf debug.log, state.log, Bilder): `lauf.sh <name>
  <raum> "<karte>" <exit_at> [env]` = Karte aus `probe_r35_cut1150_karte` + CONTINUE (`RE15_CONTINUE_TEST=1
  RE15_CARD_AUTO=1 RE15_CARD_SLOT=0`), `RE15_INPUT_SCRIPT_BASIS=spiel RE15_INPUT_SCRIPT_START=60
  RE15_INPUT_SCRIPT=W1,A0.2,W200` (Aktionstaste an der ECHTEN Tuer ROOM1130 -> ROOM1150 bei Bild 90), `RE15_STATE_LOG
  RE15_IT_LOG=10 RE15_NOAUDIO=1 RE15_EXIT_AT=<bild>#<raum>`; Bilder per `RE15_FRAMEDUMP=<a>-<b>/<s>:f_` (beschleunigter
  Renderer), laufend weggesichert von `waechter.py` (Reihenfolge = Schreibzeit; der Bildzaehler beginnt je Raum neu).
  Kontaktboegen im scratchpad `L_abn1/*.png`.

## 1. Punkt 1 — "Bei Wechsel von ROOM 1060 zu ROOM 1040 ... "I have to get the Chief first...", solange man nicht in ROOM 1150 war"

**Urteil: erfuellt.** (Code `tuer1060_1040.c`/`re15_tuer1060.h` seit Abnahme 0 unveraendert: `git log 45bb936b..HEAD` leer.)

| Lauf | Karte | Ergebnis |
|---|---|---|
| p1_zu | 1060, nach10f0 ersteszene; `SKRIPT=W1,A0.2,W3`, `240#1060`, `RE15_FRAMEDUMP=200:p1.ppm` | debug.log `[tuer1060] ROOM1060 Slot 2 -> Text-Platz msg 1 ((9,73)=0 (9,71)=1 (3,94)=1)`, kein DOOR FIRE, EXIT in 1060; state.log F90 `msg(a=1 fsm=1 id=1)`; Bild `p1_zu.png`: "I have to get the Chief first..." |
| p1_frei | dto. + gesehen ((9,73)=1), `240#1040` | `DOOR FIRE slot=2 rect=(27100,25400,...)` -> `[room] PC loaded room1040.rdt` |

## 2. Punkt 2 — Irons-Todesszene ROOM1150

**Urteil: erfuellt.** (Abnahme 0: teilweise wegen "steht LANGSAM auf" — jetzt behoben und gemessen.)

Lauf `kette` (Karte 1130 nach10f0 ersteszene, Tuerweg, `1500#1150`, `RE15_FRAMEDUMP=0-1500/6:f_`), debug.log Z.73 room1150,
Z.90 `Szene startet (Programm 0)`, Z.1083 `Signal (5,12): Irons' Arm faellt`, Z.1566 `Signal (5,28): Tuerknall`.

| Bild (1150) | Befund (Kontaktbogen) | Wortlaut |
|---|---|---|
| F24-F72 | "Leon: Sir!", Arm nach vorn F36-F60 (`s1150_a.png`) | Sir! (Arm) — ja |
| F84-F192 | rennt durchs Buero zur Liege, steht an der Liege (`s1150_a.png`) | rennt zur Liege — ja |
| F204-F408 | Cut 7, msg 23 "Sir, the communication system can't be fixed! We're going to use the patrol car to get out of here.", msg 24 "I came to get you, come with me!" | ja |
| F420-F890 | msg 25/26/27/28 Irons im Wortlaut (`s1150_b.png`, `s1150_c.png`); Arm ausgestreckt F702-F894 | Arm fuer eine Weile — ja |
| F894 -> F906 | Arm faellt in ~12 Bildern (`s1150_fall.png`, F888 oben, F900 halb, F906 unten) | abrupt fallen lassen — ja |
| ab F906 | Irons st=4 mo=2 af=89 (gehalten) bis Bild 1500 des Rueckkehr-Besuchs (state.log letzte Zeile) | tot — ja |
| F924 | "Leon: SIR, Sir?!" | ja |
| F1032-F1128 | Kopf gesenkt, Kopf dreht sich langsam hin und her (`s1150_kopf.png`: F1032-F1062 links, F1074-F1092 Gesicht sichtbar = rechts, F1110 ff. zurueck) | schuettelt langsam den Kopf — ja |
| F1163-F1211 | Aufstehen (Lauf `auf`, `RE15_FRAMEDUMP=1150-1240/1`, Bild-zu-Bild-Differenz): ab F1177 aendert sich das Bild NUR jedes zweite Bild (1177:1.86 1178:0.0 1179:2.0 1180:0.0 ... 1209:3.55 1210:0.0 1211:2.37), Bewegung F1163..F1211 = 48 Bilder (1,6 s) statt ~25 in Abnahme 0; `s1150_auf.png` F1164 kniend, F1188 halb, F1206 steht | steht LANGSAM auf — ja |
| F1229-F1333 | Cut 5 Totale, Pause, Knall-Signal, Schnitt nach 1130 | kurze Pause — ja |

Riegel `unit_r35_cut1150_szene` (selbst gefahren): `Aufstehen: Beginn Bild 1157, Clipende Bild 1205 (48 Bilder), 24 Schritte`.

## 3. Punkt 3 — Knaelle und Spawns 1130 / 1040 / 1030

**Urteil: teilweise** (1130, 1030, Toene erfuellt; 1040 nur, solange von den Raum-Records 0..14 genug leben — M1).

* **1130 Cut 0** (kette): debug.log Z.1632 `Montage 1130 (Programm 1)`, Z.1642 `Signal (5,28): Tuerknall`; Messzeile T100: fuenf
  Zombies (0x16,0x10,0x10,0x11,0x11) bei (-1400,-14200) .. (-718,-11600); Bild `s1130.png` ab F36 / `z_1130_1040.png` links:
  fuenf Zombies vor der Briefing-Room-Tuer, Kamera Cut 0. -> erfuellt.
* **1040 Cut 1, Tor zu, alle 20 Raum-Records leben** (kette): Z.1726 `Signal (5,29): Knall wie ROOM1030`, Z.1806 `alle durchs
  Tor (Signal (5,30))`, Z.1844 `Bilanz 1040: 5 Zombies, durchs Tor (z < -360) 5, letzter bei Bild 273 von 363; ...
  Spieler-HP 100 -> min 100; Kappe 0`; Bild `s1040.png`: Tor faehrt F84-F180 hoch, dahinter die fuenf, F192-F360 kommen sie
  durch den Torbogen auf die Kamera zu (`z_1130_1040.png` rechts). -> erfuellt.
* **1040, Tor schon offen** (Lauf e_offen, Karte + tor1040offen): `Bilanz 1040: 5 Zombies, durchs Tor 5, letzter bei Bild
  239 von 329; ... HP 100 -> min 100; Kappe 0`, ein Knall (5,29), keine Tor-Fahrt; Bild `s1040_offen.png` F12-F324: die fuenf
  hinter der Oeffnung, kommen durch. -> erfuellt (Abnahme-0-M2 behoben, kein Biss).
* **1040 unter der RE1.5-KI** (`RE15_AI_FLAVOR=re15`, Laeufe r15_zu / r15_offen): beide `5 Zombies, durchs Tor 5, letzter bei
  Bild 781 von 871; HP 100 -> min 100; Kappe 0, hinter der Kamera angehalten 2`. Erfuellt, aber der Schnitt steht 871 Bilder
  (29 s) auf Cut 1 (Zombies 3/4 im Unterzustand 0x13 mit ~0,7 Einheiten je Bild; Messzeilen T420..T980) — Hinweis H1.
* **1040 in der natuerlichen Totschlag-Reihenfolge — NICHT erfuellt (M1).** Laeufe mit denselben Karten + `RE15_SET_FLAG_AT=
  7:20,...@50` (Zone-7-Tot-Bits der Raum-Records in Spawn-Reihenfolge = so, wie ein Spieler sie in 1040 toetet, denn das
  Gleichzeitig-Limit 5 laesst immer die niedrigsten lebenden Records erscheinen):

  | Lauf | getoetet | debug.log | Ergebnis |
  |---|---|---|---|
  | tot12 | Records 0..11 (Bits 0x14..0x1f) | `1040: 8 Raum-Records leben, Auffuellung 0` / `Bilanz 1040: 3 Zombies, durchs Tor 3 ... 13@231 14@201 15@269` | **3 statt 5** |
  | tot15 | Records 0..14 (Bits 0x14..0x22) | `1040: 5 Raum-Records leben, Auffuellung 0` / `1040: KAPPE erreicht, Schnitt ohne alle Durchgaenge` / `Bilanz 1040: 0 Zombies ... von 1090; Kappe 1` | **0 statt 5**, 36 s leeres Tor |
  | tot18 | Records 0..17 | `2 Raum-Records leben, Auffuellung 3` / `Bilanz 1040: 3 Zombies, durchs Tor 3` | **3 statt 5** |

  Ursache (Code gelesen): `umzug_vorbereiten` zaehlt mit `re15_irons_tod_lebend_1040()` ALLE 20 Records als "lebend", aber
  Sce_em_set-Slot 15..19 ergibt Aktor 16..20 >= `RE15_ACTOR_MAX` 16 (`SCRIPT_SLOT_TO_ACTOR(s) = s+1`, scd_vm.c:3173;
  `if (!suppress && actor_slot < RE15_ACTOR_MAX)` scd_vm.c:3785) — diese Records erscheinen im Port nie, verbrauchen aber
  vorher einen Platz des Gleichzeitig-Limits (`g_scd.work_vars[0x11] = live + 1`, scd_vm.c:3780). Die Auffuellung rechnet sie
  mit und fuellt deshalb nicht auf. Der Riegel `unit_r35_cut1150_montage_1040` prueft nur das Muster "Records 2..19 tot,
  0 und 1 leben" (test_r35_cut1150.c:625), das der Spieler so nicht herstellen kann.
* **1030 Cut 7 + Cut 6** (kette): Z.1847 `Montage 1030`, Z.1860 Tuerknall; Kamera F2 Cut 7 -> F181 Cut 6 -> F218 Cut 12
  (Cut_replace-Tausch); Bild `s1030.png`: F36-F180 fuenf Zombies vor der 1070-Tuer, F228-F552 Zombies legen sich und kriechen
  unter dem Tor durch; Messzeile T1080 cam=12 mit `fl=1004` (Kriech-Bit). -> erfuellt.
* **Toene** (selbst gemessen, Weg des Bau-Agenten nachgefahren): Laeufe t_mit / t_ohne (ohne RE15_NOAUDIO,
  `RE15_AUDIO_CAP_SYNC=cap.raw RE15_SE_DEBUG=1`, t_ohne mit `RE15_IT_KNALL_STUMM=1`), `tools/r35_l/n1/ton_diff.py ... 16397
  11025`: 5078 Bilder, Differenz 0 in 4949; Ereignisse Bild 1909 / 1975 / 2156 / 2524 (Diff-RMS 3283 / 3283 / 4657 / 3283,
  Spitze 7853/7853/8182/7853). Korrelation (ton_probe-Verfahren auf meinen Mitschnitten): Tuerknaelle **0,997** DOOR04
  @16397 Hz, Knall 1040 **0,984** ROOM1030 @11025 Hz — die Zahlen des Dossiers sind reproduziert. Der Knall Cut 6 ist das
  Original-Se_on. Hoeren am Lautsprecher ist auf diesem Rechner nicht moeglich (kein Endgeraet).

## 4. Punkt 4 — Schnitt ROOM11C0 Cut 13 (Ada/Marvin)

**Urteil: erfuellt.** (kette: Z.2060 room11c0, Z.2072 `Montage 11C0 (Programm 4)`, Z.2262 `Rueckkehr: Programm 5`, Z.2279
`Rueckkehr beendet, Steuerung frei`, state.log F1500 in 1150 `pm=0`.)
Bilder `s11c0.png` / `z_11c0.png`: F12 beide mit dem Ruecken zur Kamera, F60 beide zur Kamera = Richtung Gebaeude-Tuer
(Slot 0 @0x01712 liegt hinter der Kamera); F84 "Ada: What was this noise? Did you hear that?", F186 "Marvin: Yes!....",
F300 "Marvin: Oh, no, Leon!" waehrend er sich zu Ada dreht, F348/F360 "I have to help him, sorry!" mit Arm zu Ada, F396/F408
rennt auf die Kamera zu und unten aus dem Bild, F444/F456 "Ada: Marvin!..." mit ausgestrecktem Arm, danach Schnitt nach 1150.
Marvin steht LINKS neben Ada, ganz im Bild (Abnahme-0-M5 behoben). Messzeilen-Reihenfolge der Nachrichten state.log: 22..29
in 1150, 10..14 in 11C0.

## 5. RE-Gate

* Stichproben SELBST disassembliert / gelesen — alle enthalten das Behauptete:
  - PSX.EXE `0x80073f90[4] = 0x80030660` (Kommando-4-Verteiler); `80030670 andi v0,v1,0x10` / `80030674 beq` /
    `8003067c andi v0,v1,0x20` / `80030680 bne v0,zero,0x800306b4` (ueberspringt `800306ac jalr v0`), `800306c4 xori
    v0,v0,0x20` / `800306c8 sh` (kippt JEDES Bild, unabhaengig von 0x10 — der Port kippt ebenso in state 4).
  - Plc_flg `80041fec lw v1,340(a0)` / `80041ff4 lhu v0,452(v1)` / `80041ffc or v0,v0,a2` / `80042020 sh v0,452(v1)` =
    OR in +0x1c4 (`43 00 90 00` -> 0x0090).
  - RE2 PSX.EXE `80065d30 andi 0x10` / `80065d3c bne` / `80065d74 xori 0x20` (Feld 460 = +0x1cc); RE2 ROOM4100.RDT
    `3f 00 14 10` @0x16CA, `3f 00 13 10` @0x178E/@0x1930, `3f 00 12 98` @0x1784.
  - STAGE1.BIN `80100450 lbu v0,9(a0)` / `80100458 andi v0,v0,0x20` / `8010045c bne v0,zero,0x80100658` (Einzelstopp).
  - RDT-Bytes: ROOM1040 @0x019B2..@0x01A2A gegen `k_p_1040_tor` Byte fuer Byte gleich (ohne @0x019D2..@0x019DD Leons
    Drehung, begruendet); ROOM1030 @0x020C6..@0x0216F (Laengen 0x9c/0x8c/0x7a, Port-Nachrechnung 0x88/0x78/0x66 stimmt);
    ROOM1150 @0x0112A `11 00 08 00 02 00 12 04 21 05 20 00`, @0x012C4 `29 05 3f 01 0b 00 43 00 80 00 09 0a 28 00`;
    ROOM11C0 @0x200 (-11300,-3788,-8584), @0x01712 Rechteck (-27100,15900,4000,2700); ROOM1040 @0x1168 Rolltor
    (-25200,0,-360), @0x11F0 `24 12 05 00`, block.blk RDT+0x38 -> @0x0FF4 (9 Zonen; Zone 0 x -27100..-23300 z
    -15000..19310, Zone 5 x -32760..-27340 z -2100..26200).
* **Zitierfehler (M2):** `RE15_IT_KAMERA_Z_1040 (-10692)` zitiert "Kamera-z @0x88 `3c d6 ff ff`" (re15_irons_tod.h:97,
  Commit a2b4ddb0, Dossier §9.3). @0x88 steht `d6 f3 ff ff` (= y -3114); `3c d6 ff ff` liegt @0x8C (Cut-1-Satz @0x80: +4 x,
  +8 y, +0xC z, wie @0x200 bei 11C0). Der Wert ist richtig, die Adresse nicht.
* Kennzeichnung: Aufstell-Orte/Gierung 1024, Sleep 90, Kappe 1000, Marvins Standort = PORT-WAHL mit Herleitung; Sleep 65 =
  @0x012CE 40 + 25. Wortsuche im Diff (deferred/tunable/interim/for now/faithful/plausibel/TODO): nur ein Zitat des
  Original-Texts "for now" (ROOM1150 msg 2). Mess-Haken RE15_IT_LOG / RE15_IT_KNALL_STUMM env-gegatet, kein Abschluss-Schalter.
* Fix erklaert Befund: M1/M2 Abnahme 0 (Nav-Zone 5 nur ueber 3/4 an den Flur, Bytes @0x0FF4 bestaetigt) -> Aufstellung in Zone
  0 -> 5/5 durch (gemessen); M3 Halbtakt -> Bild aendert sich nur jedes zweite Bild (gemessen); M5 Standort -> Bild; M6 Bytes;
  M7 Mitschnitt reproduziert. Die NEUE Auffuellungs-Logik erklaert den 1040-Befund nur fuer das Riegel-Muster (M1).

## 6. Vertrag / Pfade / Tests

* `git diff master --name-only`: kein release/, platform/android/, shared_assets/PSX/, tests/unit/CMakeLists.txt,
  tests/integration/CMakeLists.txt (0 Treffer). Gemeinsame Dateien klein und markiert: enemy_common.c +14/-1,
  player_common.c +8/-4 (Halbtakt, 4 Zeilen Code; nicht in der Liste von VERTRAG §1.4, aber kleiner Haken mit Beleg),
  scd_room_setup.c +7, scd_vm.c +8/-1, main.c +5, re15_enemy.h +3, tests/test_support.c +6.
* Bank 9: 73/74/75/76 (zugeteilt). Nachrichten 1150: 22..29, 1060: 1, 11C0: 10..14 (zugeteilt). Ereignis 21, Tuer-Slot 20.
  Neu in N1: Bank-5-Scratch Bit 30 in ROOM1040 (Dossier-Zensus §8.8, Wort 0 Bits 0..20 belegt) — als Abweichung ausgewiesen
  wie die Bits 12/28/29 und die Zone-7-Kopie-Bits (Abnahme 0).
* Tests: Riegel je Punkt vorhanden (tuer1060, szene, montage_1130/1040/1030/11c0, rueckkehr, totenpose, tot_bleibt_tot,
  knallbank, programme, zaehlung) + integration A-E; alle gruen (16/16). Luecke: montage_1040 und integration E pruefen die
  Auffuellung nicht in der natuerlichen Totschlag-Reihenfolge (M1).

## 7. Maengel (nummeriert, nachpruefbar)

1. **M1 — ROOM1040: "5 Zombies durch kommen" scheitert, sobald der Spieler vorher > 10 Raum-Zombies in 1040 getoetet hat.**
   Messung: `lauf.sh tot15 1130 "nach10f0 ersteszene" "5#1030" "RE15_SET_FLAG_AT=7:20,7:21,...,7:34@50"` (Records 0..14
   tot, d.h. in der Reihenfolge, in der das Gleichzeitig-Limit 5 sie zeigt) -> debug.log `1040: 5 Raum-Records leben,
   Auffuellung 0`, `KAPPE erreicht`, `Bilanz 1040: 0 Zombies ... von 1090; Kappe 1` = KEIN Zombie, 36 s Standbild auf das Tor.
   Records 0..11 tot (tot12) -> 3 Zombies; Records 0..17 tot (tot18) -> 3 Zombies. Ursache: `re15_irons_tod_lebend_1040`
   (irons_tod_1150.c:574) zaehlt Records mit Sce_em_set-Slot 15..19 als lebend; die erscheinen im Port nie (Aktor 16..20 >=
   RE15_ACTOR_MAX 16, scd_vm.c:3173/3785), belegen aber Plaetze des Limits 5 (scd_vm.c:3780). Der Riegel testet nur
   "Records 2..19 tot" (test_r35_cut1150.c:625). Gefordert: fuenf Zombies auch bei 11..20 getoeteten Records in
   Spawn-Reihenfolge, mit Riegel- und Integrationsfall dafuer.
2. **M2 — Adresszitat falsch:** Kamera-z ROOM1040 Cut 1 ist @0x8C, nicht @0x88 (re15_irons_tod.h:97, Dossier §9.3,
   Commit-Message a2b4ddb0). Wert -10692 korrekt. Kommentar/Dossier berichtigen.

### Hinweise (kein Bestehens-Hindernis, im Dossier unter OFFEN gefuehrt und von mir bestaetigt)
* H1: Unter der RE1.5-KI (OPTIONS -> AI) steht der 1040-Schnitt 871 Bilder (29 s) auf Cut 1 (r15_zu/r15_offen: letzter
  Durchgang Bild 781), 2 Zombies hinter der Kamera angehalten; Spieler-HP 100.
* H2: Der Einzelstopp (entity+0x9 & 0x20) wirkt nur im RE1.5-Root; unter der RE2-KI kam in keinem meiner Laeufe ein Zombie an
  den Parkplatz (HP 100 in kette, e_offen, tot12, tot15, tot18).
* H3: Hoeren am Lautsprecher nicht moeglich (kein Audio-Endgeraet); der Mischausgang enthaelt die vier Port-Knaelle
  (Korrelation 0,997/0,984 reproduziert).

## 8. Urteile je Punkt (Kurzform)

| Punkt | Urteil | Beleg |
|---|---|---|
| 1 Dialog 1060 -> 1040 | erfuellt | p1_zu: `[tuer1060] ... msg 1`, kein DOOR FIRE, Bild p1_zu.png; p1_frei: DOOR FIRE -> room1040 |
| 2 Szene 1150 | erfuellt | kette Z.90/1083/1566, Kontaktboegen s1150_*.png; auf: Halbtakt ab F1177, F1163..F1211 = 48 Bilder |
| 3 Knaelle + Spawns | teilweise | 1130/1030/Tor zu/Tor offen 5/5, HP 100; natuerliche Totschlag-Reihenfolge 1040: 3 bzw. 0 statt 5 (M1); Toene per CAP_SYNC reproduziert |
| 4 Schnitt 11C0 | erfuellt | kette Z.2072..2279, s11c0.png/z_11c0.png, Marvin links neben Ada, rennt aus dem Bild, Rueckkehr pm=0 |

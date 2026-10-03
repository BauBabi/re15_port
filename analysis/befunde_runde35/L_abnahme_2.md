# Runde 35 Spur L "cut1150" — Unabhaengige Abnahme 2 (nach Nachbesserung 2)

Datum 2026-10-04. Baum `.claude/worktrees/r35_cut1150`, Zweig `r35/cut1150`, HEAD `4f8c235f` (Basis master `154a73c1`).
Geprueft gegen den WORTLAUT `AUFTRAG.md` Z. 68-91, `VERTRAG.md`, Dossier `L_cut1150.md` §10 und die Maengel M1/M2 der
Abnahme 1 (`L_abnahme_1.md`).

**Ergebnis: BESTANDEN.** Alle vier Punkte erfuellt, an der exe selbst gemessen. M1 der Abnahme 1 (ROOM1040: nach
Totschlag in Spawn-Reihenfolge 3/0/3 statt 5 Zombies) ist behoben, und der Fix erklaert den Befund. M2 (Adresszitat
Kamera-z) ist berichtigt. Suite, @0x-Gate, Pfad-Gate und Tests halten. Drei Hinweise (keine Bestehens-Hindernisse) in §7.

## 0. Bau und Messaufbau

* `bash re15_port/tools/local_build.sh configure` + `... build` -> `ninja: no work to do`, `=== LOCAL-BUILD-OK (build)`.
  exe `re15_pc.exe` 2026-10-03 23:17:18, md5 `ea9757cd9227717ff7b82ca7203c3a43`. Juengste Code-Quelle
  `re15_irons_tod.h` 23:16:48 (Commit 7cb63b12, Kappe 1532); danach nur Dossier und Werkzeug. Die exe ist also der Code-Stand
  von HEAD. Messkopie `re15_pc_abn2L.exe` (gleiche md5) im selben Verzeichnis.
* Volle Suite: das Dossier §10.8 traegt woertlich `=== LOCAL-BUILD-OK (all) — Tests 492/492`, und
  `re15_port/build/local_build_ctest.log` (23:59, gegen dieselbe exe) bestaetigt das: `100% tests passed, 0 tests failed out
  of 492`. Deshalb nicht wiederholt.
* Selbst gefahren (PowerShell): `ctest --test-dir <baum>/re15_port/build -R "r35_cut1150|^unit_se_bank_routing$|
  ^unit_r34_plattform$|^unit_r34_plattform_ton$" --timeout 1200` -> **17/17 gruen**, integration_r35_cut1150 523,84 s
  (Log scratchpad `L_abn2/ctest_r35l.log`).
* Messlaeufe (scratchpad `.../scratchpad/L_abn2/runs/<lauf>/`: debug.log, state.log, Bilder): `lauf.sh <name> <raum>
  "<karte>" <exit_at> [env]`. Die Karte kommt aus `probe_r35_cut1150_karte`, dann CONTINUE (`RE15_CONTINUE_TEST=1
  RE15_CARD_AUTO=1 RE15_CARD_SLOT=0`). Eingabe: `RE15_INPUT_SCRIPT_BASIS=spiel RE15_INPUT_SCRIPT_START=60
  RE15_INPUT_SCRIPT=W1,A0.2,W200`, also die Aktionstaste an der ECHTEN Tuer ROOM1130 -> ROOM1150 bei Bild 90. Dazu
  `RE15_STATE_LOG RE15_IT_LOG=10 RE15_NOAUDIO=1 RE15_EXIT_AT=<bild>#<raum>`, RE2-KI (exe-Default). Bilder per
  `RE15_FRAMEDUMP=0-1500/6:f_` (beschleunigter Renderer); `waechter.py` sichert sie laufend weg, und `seg.py` trennt sie
  nach Raum, weil der Bildzaehler je Raum neu beginnt. Kontaktboegen im scratchpad `L_abn2/*.png`.

## 1. Punkt 1 — "Bei Wechsel von ROOM 1060 zu ROOM 1040 ... "I have to get the Chief first...", solange man nicht in ROOM 1150 war"

**Urteil: erfuellt.** Der Code (`tuer1060_1040.c`, `re15_tuer1060.h`) ist seit Abnahme 0 unveraendert.

| Lauf | Karte / Eingabe | Ergebnis |
|---|---|---|
| p1_zu | 1060, `nach10f0 ersteszene`; `SKRIPT=W1,A0.2,W3`, `240#1060`, `RE15_FRAMEDUMP=200:p1.ppm` | debug.log Z.44 `[tuer1060] ROOM1060 Slot 2 -> Text-Platz msg 1 ((9,73)=0 (9,71)=1 (3,94)=1)`; kein `PC loaded room1040` (0 Treffer); state.log F90 `msg(a=1 fsm=1 id=1)`, bis F240 in 1060. Bild `L_abn2/p1_zu.png`: Treppenhaus, Untertitel "I have to get the Chief first..." |
| p1_frei | dto. + `gesehen` ((9,73)=1), `240#1040` | debug.log Z.79 `DOOR FIRE slot=2 rect=(27100,25400,hw=500,hh=1100)`, Z.83 `[room] PC loaded room1040.rdt` |

## 2. Punkt 2 — Irons-Todesszene ROOM1150

**Urteil: erfuellt.** In Nachbesserung 2 nicht geaendert. Am Stand HEAD erneut gemessen.

Lauf `kette` (Karte 1130 `nach10f0 ersteszene`, Tuerweg, `1500#1150`, `RE15_FRAMEDUMP=0-1500/6:f_`). debug.log Z.73
`room1150`, Z.90 `Szene startet (Programm 0)`, Z.1083 `Signal (5,12): Irons' Arm faellt (Clip 2 ab Bild 72)`, Z.1566
`Signal (5,28): Tuerknall`. Nachrichtenfolge in state.log: 22 (F26), 23 (F212), 24 (F333), 25 (F413), 26 (F543), 27 (F673),
28 (F793), 29 (F923).

| Bild (1150) | Befund (Kontaktbogen) | Wortlaut |
|---|---|---|
| F24-F72 | "Leon: Sir!", Arm nach vorn F36-F60 (`s1150_sir.png`) | Sir! (Arm Streck Animation) — ja |
| F84-F192 | rennt durchs Buero zur Liege (`s1150_all.png`) | rennt zu Irons zur Liege — ja |
| F212-F408 | "Sir, the communication system can't be fixed! We're going to use the patrol car to get out of here." / "I came to get you, come with me!" | ja |
| F413-F888 | Irons msg 25..28 im Wortlaut; Arm ausgestreckt ab ~F696, gehalten bis F894 (`s1150_all.png`, `s1150_fall.png`) | "Arm ausgestreckt fuer eine Weile" — ja |
| F894 -> F906 | F894 Arm oben, F900 halb unten, F906 unten (`s1150_fall.png`) | "abrupt runter fallen lassen" — ja |
| ab F906 | Irons bewegt sich nicht mehr; Rueckkehr-Besuch F1500: `[1 t=45 st=4 ... ss2=2 ... mo=2]` (gehalten), Bild `s1150r.png` F150..F1500 Irons unveraendert auf der Liege | tot — ja |
| F923 | "Leon: SIR, Sir?!" | ja |
| F1032-F1128 | kniet, Kopf gesenkt, Kopf wendet sich langsam hin und her (`s1150_kopf.png`) | "schuettelt mit leicht geneigtem Kopf den Kopf langsam" — ja |
| F1164-F1212 | Aufstehen ueber ~48 Bilder (`s1150_auf.png`: F1164 kniend, F1188 halb, F1212 steht) | "steht langsam wieder auf" — ja |
| F1236-F1332 | Cut 5 Totale, Pause, dann Knall und Schnitt nach 1130 | "Kurze Pause" — ja |

## 3. Punkt 3 — Knaelle und Spawns 1130 / 1040 / 1030

**Urteil: erfuellt.** (Abnahme 1: teilweise wegen M1.)

### 3.1 ROOM1130 Cut 0 ("die Anzahl an Zombies die noch leben ... bei Cut0")
* kette (alle 5 in 1140 leben): Z.1632 `Montage 1130 (Programm 1)`, Z.1642 `Signal (5,28): Tuerknall`; Messzeile T100
  `cam=0` mit fuenf Zombies (0x16,0x10,0x10,0x11,0x11) bei (-1400,-14200) .. (-718,-11600); state.log in 1130 `cam=0`;
  Bild `s1130.png` ab F36: Zombies vor der Briefing-Room-Tuer.
* teil (`RE15_SET_FLAG_AT=7:211,7:212,7:213,7:198,...,7:201@50` = 3 von 5 in 1140 tot, 4 von 5 in 1070 tot):
  `Umzug: 1140 lebend 2 -> 1130 (9,74)=1; 1070 lebend 1 -> 1030 (9,76)=1`. Messzeile 1130 T100 `cam=0` mit **genau
  zwei** Zombies (Aktor 4/5, Typ 0x11). Also "genau diese Anzahl".
* "alle getoetet -> passiert da nichts": integration Lauf B (`tot1140;tot1070`) gruen
  (`it_darf_nicht ... "Montage 1130"` / `"PC loaded room1130.rdt"`).

### 3.2 ROOM1040 Cut 1 ("Rolltor hoch ... 5 Zombies durch kommen ... wenn bereits offen, kommen nur 5")
Mangel M1 der Abnahme 1 nachgemessen, in der natuerlichen Totschlag-Reihenfolge und mit einer Luecke. Die Zahl der Zombies
stammt aus state.log (Bild 10 in 1040, Eintraege `t=16`), die Bilanz aus debug.log:

| Lauf | Vorzustand ROOM1040 | debug.log | Zombies 1040 (state.log F10, Aktoren) | Bilanz |
|---|---|---|---|---|
| kette | alle 20 Records leben | `20 Raum-Records leben, 5 erscheinen im Port, Auffuellung 0` / `Zaehler 5 -> 5` | 5 (1..5), cam 1 | `5 Zombies, durchs Tor 5, letzter bei Bild 273 von 363; HP 100 -> min 100; Kappe 0` |
| t13 | Records 0..12 tot (Karte `tot1040_13`) | `7 leben, 2 erscheinen, Auffuellung 3` / `Zaehler 5 -> 2` | 5 (1,2,3 Auffuellung + 14,15 = Records 13/14), cam 1 | `5 ... durchs Tor 5 ... 1@220 2@202 3@273 14@209 15@244; HP 100; Kappe 0` |
| t20 | alle 20 tot (`tot1040_20`) | `0 leben, 0 erscheinen, Auffuellung 5` / `Zaehler 0 -> 0` | 5, cam 1 | `5 ... durchs Tor 5 ...; HP 100; Kappe 0` |
| luecke | Records 0..12 **und 14** tot, 13 lebt (`RE15_SET_FLAG_AT=7:20,...,7:32,7:34@50`) | `6 leben, 1 erscheinen, Auffuellung 4` / `Zaehler 5 -> 1` | 5 (1..4 + 14 = Record 13), cam 1 | `5 ... durchs Tor 5 ... 14@244; HP 100; Kappe 0` |
| r15_v15 | Records 0..14 tot, **RE1.5-KI** (`RE15_AI_FLAVOR=re15`) | `5 leben, 0 erscheinen, Auffuellung 5` / `Zaehler 5 -> 0` | 5, cam 1 | `5 ... durchs Tor 5, letzter bei Bild 781 von 871; HP 100; Kappe 0, hinter der Kamera angehalten 2` |
| integration F | Records 0..14 tot (`tot1040_15`) | Lauf im ctest gruen: `5 erscheinen ... Auffuellung 5`, `Zaehler 5 -> 0`, `durchs Tor 5`, `Kappe 0,`, kein `KAPPE erreicht` | — | — |
| integration E | Tor schon offen | gruen: `durchs Tor 5`, HP 100, `Kappe 0,` | — | — |

Vorher (Abnahme 1, gleiche Karten): tot12 -> 3, tot15 -> 0 + Kappe, tot18 -> 3. Bilder: kette `s1040.png` (F90-F180 faehrt das
Tor hoch, dahinter die fuenf, F198-F360 kommen sie durch den Torbogen auf die Kamera zu); t20 `t20_1040.png` (dasselbe mit
fuenf Auffuell-Zombies). Riegel `montage_1040_reihe` selbst gefahren (Log `L_abn2/reihe.log`): 78 `ok`, 0 `FAIL`;
k = 0..20 je `5 Zombies nach dem ersten Takt`; volle Montage k=12 (letzter Bild 331, Schnitt 420), k=15 zu (999 / 1088), k=15
offen (527 / 616), k=18 (632 / 721), ueberall HP 100, Kappe 0.

Spaeteres Betreten (Regressionsblick auf `k_p_1040_nach`, Weg Tuer 1060 -> 1040): nach15 (Karte `gesehen tot1040_15` +
`9:75@50`) -> `Zaehler 5 -> 0`, `Nachspawn 1040 (Programm 14)`, state.log F100/F300 je 5 Zombies, HP 100; nach0 (Auffuell-Bits
83/84/90/91/95 gesetzt) -> `Zaehler 5 -> 5`, 5 Zombies (Records 0..4). Der Save(0x11) wirkt nur auf die folgenden
Sce_em_set desselben Besuchs. 0x800b0ff2 wird im Original nur von FUN_8003ecec (@0x8003ed7c, Null) und Sce_em_set
(@0x8004223c) geschrieben (ghidra1_V2.txt XREF), es gibt also keine Abzaehlung beim Tod, die verfaelscht werden koennte. Andere
`44`-Saetze gibt es in ROOM1040 nicht. Ein Scan der ganzen Datei auf `44 ss 1x 0d` findet 41 Treffer: 40 in main00
@0x011FC..@0x0150C und einen @0x1CB28 in Ton-Daten. In den subs (Tabelle @0x1524..@0x21C0) steht keiner.

### 3.3 ROOM1030 Cut 7 + Cut 6
* kette: Z.1848 `Montage 1030 (Programm 3)`, Z.1861 Tuerknall. Messzeilen ab T570 `cam=7` mit den fuenf Kopien aus 1070
  (Aktor 7..11, Typ 0x10/0x11, bei (-18600..-21600, -2400..-4600)) vor der 1070-Tuer. Kameras in 1030 (state.log): F1 Cut 7
  -> F181 Cut 6 -> F218 Cut 12 (Cut_replace-Tausch Tor-Bruch). Messzeile T960 `cam=12` mit `fl=1004` (Kriech-Bit). Bild
  `s1030.png`: F24-F168 Zombies vor der Tuer (Cut 7), F192 ff. Tor-Ansicht, F240-F552 Zombies kriechen unter dem Tor durch.
* teil: genau **eine** 1070-Kopie (Aktor 11, Typ 0x11 bei (-21600,-4600)) plus die drei Kriech-Zusatz-Zombies 12..14
  (`fl=0000`/`1004`, Messzeile T730 `cam=6`).

### 3.4 Knaelle (Ton)
In Nachbesserung 2 nicht geaendert; an HEAD trotzdem nachgemessen. Laeufe t_mit / t_ohne ohne RE15_NOAUDIO,
`RE15_AUDIO_CAP_SYNC=cap.raw RE15_SE_DEBUG=1`, t_ohne mit `RE15_IT_KNALL_STUMM=1`, beide `300#1040`. `tools/r35_l/n1/
ton_diff.py ... 16397 11025` ergibt 2434 Bilder, Differenz 0 in 2335. Ereignisse liegen bei Bild 1909 (Tuerknall 1150), 1975
(Tuerknall 1130) und 2156 (Knall 1040) mit Diff-RMS 3283 / 3283 / 4657, also dieselben Werte wie in Abnahme 1. Der Knall
1040 korreliert mit ROOM1030 zu **0,974**. "Knall-Tonbank nicht geladen" tritt in t_mit 0 Mal auf; mit RE15_NOAUDIO
erscheint die Meldung erwartungsgemaess.

## 4. Punkt 4 — Schnitt ROOM11C0 Cut 13 (Ada/Marvin)

**Urteil: erfuellt.** In Nachbesserung 2 nicht geaendert, am Stand HEAD erneut gemessen.
kette: Z.2061 `room11c0`, Z.2073 `Montage 11C0 (Programm 4)`, Z.2263 `Rueckkehr: Programm 5`, Z.2280 `Rueckkehr beendet,
Steuerung frei`; state.log in 11C0 `cam=13`, Nachrichten 10 (F81), 11 (F171), 12 (F231), 13 (F331), 14 (F412); letzte Zeile
in 1150 `pm=0`.
Bilder `s11c0_a.png` / `s11c0.png` / `s11c0_z.png`:
* F6/F24: beide stehen mit dem Ruecken zur Kamera. Ab F42 drehen sie sich, bei F60 schauen sie zur Kamera, also Richtung
  Gebaeude.
* F96: "Ada: What was this noise? Did you hear that?". F192: "Marvin: Yes!....".
* F264-F336: "Marvin: Oh, no, Leon!", dabei dreht sich Marvin zu Ada.
* F336-F408: "I have to help him, sorry!" mit Arm zu Ada (F354/F372). Danach rennt Marvin auf die Kamera zu und verlaesst bei
  F408/F426 das Bild nach unten.
* F426-F480: "Ada: Marvin!..." mit ausgestrecktem Arm. Danach Schnitt nach 1150; Leon steht wieder dort, Irons liegt tot auf
  der Liege (`s1150r.png`).

## 5. RE-Gate

* **Stichproben SELBST disassembliert** (`re15_disasm.py`, `info/Re1.5/PSX.EXE`). Alle enthalten das Behauptete:
  - Sce_em_set: `800420ac lui a2,0x800b` / `800420b0 addiu a2,a2,-14468` (a2 = 0x800ac77c) / `800420dc addiu a0,a2,1200`
    (0x800acc2c) / `800420f0 lbu v0,1(a1)` / `800420f8 andi s4,v0,0x7f` / `80042100..80042110` s4*500 / `80042118 addu
    s0,v0,a0`. Das ergibt Entity = 0x800acc2c + Slot*0x1F4 **ohne Grenzpruefung**. Tot-Bit-Gate davor: `80042120 lbu a1,7(a1)`
    / `80042128 beq a1,0xff` / `80042130 jal 0x8004efe4` / `80042138 beq v0,zero`, ohne Zaehler.
  - Limit: `800421d8 lui a1,0x800b` / `800421dc addiu a1,a1,4082` (0x800b0ff2) / `80042214 lh v0,0(a1)` / `8004221c lh
    v1,4084(v1)` (0x800b0ff4) / `80042224 slt` / `80042228 bne` / `8004222c addiu v0,a0,1` / `8004223c sh v0,0(a1)`; voll ->
    `80042230 ori v0,zero,0x8000` / `80042238 sw v0,0(s0)`.
  - Save LAB_80040018: `80040020 lbu v1,1(v0)` / `80040024 lh a1,2(v0)` / `80040030 sll v1,v1,1` / `80040038 addiu
    at,at,4048` (0x800b0fd0) / `80040040 sh a1,0(at)`. Index 0x11 ergibt 0x800b0fd0 + 0x22 = **0x800b0ff2** = der Zaehler des
    Gates.
  - FUN_80037250: Basis `80037254 addiu a0,a0,-13746` + `80037258 addiu a1,a0,478` = 0x800acc2c,
    `800372b4 sltiu v0,v0,0x14` / `800372bc addiu a1,a1,500` / `800372c8 addiu a1,a1,-13268` (= 0x800acc2c, enemy_array).
    Das Feld hat 20 Plaetze zu 500 Byte.
  - RDT ROOM1040 (selbst gelesen): @0x011F0 `24 12 05 00`. Zweig Tor offen @0x011FC..@0x01378 und Zweig Tor zu
    @0x01390..@0x0150C (Else `07 00 96 01` @0x0138C) enthalten je 20 Saetze `44 ss 16 0d 00 00 pp bb` mit ss = 0x00..0x13 und
    bb = 0x14..0x27 in gleicher Reihenfolge, Satzbreite 20. Kamera Cut 1 @0x80: `00 00 3c 68 | 6a 99 ff ff | d6 f3 ff ff | 3c
    d6 ff ff` = x @0x84 -26262, y @0x88 -3114, **z @0x8C -10692**.
  - Port-Seite (Ursache): `scd_vm.c:3777-3785`. Erst zaehlt `if (live < cap) g_scd.work_vars[0x11] = live + 1`, danach
    folgt `if (!suppress && actor_slot < RE15_ACTOR_MAX)`, und `RE15_ACTOR_MAX 16` steht in `re15_actor.h:22`. Die Records
    15..19 verbrauchen also einen Limit-Platz und erscheinen nicht. Das ist genau die Vorbedingung des Fixes. Sie zeigt sich im
    Protokoll: Abnahme 1 `tot15: Auffuellung 0, KAPPE`, jetzt `Zaehler 5 -> 0` mit 5 Auffuell-Records. **Der Fix erklaert den
    Befund.**
* **Konstanten im N2-Diff:**
  - `OP_ZAEHLER_1040` `24 11 nn 00` traegt @0x800b0fd0/@0x800b0ff2/@0x800421d8/@0x80042214 im Code-Kommentar. Das
    Einsetzen ist im Dossier §10.1 und in den Commits a59e29d5/4f8c235f als PORT-WAHL ausgewiesen.
  - `RE15_IT_1040_KAPPE` = `((175 + 5160*100/495) * 1000/794)` = 1532 ist als PORT-WAHL-Sicherung gekennzeichnet; die
    Herleitung stuetzt sich auf die gemessene Rate 4,95 (re15_irons_tod.h:91-99).
  - `i + 1 < RE15_ACTOR_MAX` verweist auf `SCRIPT_SLOT_TO_ACTOR`.
  - M2 ist berichtigt in `re15_irons_tod.h:101-102` und im Dossier Z.705/893.
* **Wortsuche** im gesamten Diff gegen master (deferred/tunable/interim/for now/faithful/plausib/TODO/approx/geschaetzt):
  ein Treffer, das Zitat des Original-Texts "for now" (ROOM1150 msg 2). Mess-Haken (RE15_IT_LOG, RE15_IT_KNALL_STUMM)
  sind env-gegatet und dienen nicht als Abschluss-Schalter.

## 6. Vertrag / Pfade / Tests

* `git diff master --name-only`: kein `release/`, `platform/android/`, `shared_assets/PSX/`, `tests/unit/CMakeLists.txt`,
  `tests/integration/CMakeLists.txt` (0 Treffer).
* Nachbesserung 2 aendert keine gemeinsame Datei (`git diff e843cfbc HEAD`: nur irons_tod_1150.c, re15_irons_tod.h, Tests,
  Werkzeug). Der Stand gegen master entspricht N1: enemy_common.c +14/-1, player_common.c +8/-4, scd_room_setup.c +7,
  scd_vm.c +8/-1, main.c +5, re15_enemy.h +3, tests/test_support.c +6. Die Haken sind klein und als "Runde 35 Spur L"
  markiert.
* Ressourcen: Bank 9 Bits 73-76 (zugeteilt). Nachrichten 1150: 22..29, 1060: 1, 11C0: 10..14 (state.log bestaetigt, alles
  zugeteilt). Ereignis 21. N2 fuehrt kein neues Bit und keine neue ID ein; Save(0x11) schreibt den Original-Zaehler
  work_vars[0x11]. Die Abweichungen Bank-5-Scratch 12/28/29/30 und die Zone-7-Kopie-/Auffuell-Bits sind seit
  Abnahme 0/1 ausgewiesen.
* Tests je Punkt: tuer1060 (P1); szene, totenpose, tot_bleibt_tot (P2); montage_1130, montage_1040, **montage_1040_reihe
  (neu)**, montage_1030, knallbank, zaehlung (P3); montage_11c0, rueckkehr (P4); dazu integration A-F (F neu: tot1040_15).
  Alle sind messend (PRUEF auf Anzahl, Durchgang, HP, Schnitt; Log-Muster im Integrationslauf) und laufen gruen (17/17, §0).

## 7. Maengel

**Keine.** Die Maengel der Abnahme 1 sind erledigt:

| Mangel Abnahme 1 | Stand | Messung |
|---|---|---|
| M1 1040 nach Totschlag 3/0/3 statt 5 | behoben | t13/t20/luecke/r15_v15 je 5 von 5 durchs Tor, HP 100, Kappe 0; Riegel k=0..20 je 5; integration F gruen |
| M2 Kamera-z @0x88 | berichtigt auf @0x8C | RDT @0x80..@0x8F selbst gelesen |

### Hinweise (kein Bestehens-Hindernis)
* H1: Unter der RE1.5-KI (OPTIONS -> AI) steht der 1040-Schnitt lange auf Cut 1. An der exe dauerte r15_v15 871 Bilder
  (29 s, letzter Durchgang Bild 781), im Riegel k=15 Tor zu 1088 Bilder. Die neue Kappe 1532 greift in keinem gemessenen
  Fall. Unter der RE2-KI (exe-Default) dauert der Schnitt 363 Bilder.
* H2 (OFFEN §10.5, Port-Grenze ausserhalb dieser Spur, schon auf master): Der Port fuehrt Spieler + 15 Gegner
  (`RE15_ACTOR_MAX 16`), das Original 20 Gegner-Plaetze (@0x800372b4). Im normalen Spiel erscheinen deshalb in ROOM1040/1030
  die Records 15..19 nie; nach 15 getoeteten Records ist ROOM1040 ausserhalb der Szene leer, im Original stehen dort noch
  fuenf. Die Szene umgeht das. Empfehlung: eine eigene Spur, die `RE15_ACTOR_MAX` auf 21 hebt (Zensus `grep
  RE15_ACTOR_MAX`, Voll-Suite).
* H3: Hoeren am Lautsprecher ist auf diesem Rechner nicht moeglich (kein Audio-Endgeraet). Der Mischausgang enthaelt die
  Knaelle (§3.4).

## 8. Urteile je Punkt (Kurzform)

| Punkt | Urteil | Beleg |
|---|---|---|
| 1 Dialog 1060 -> 1040 | erfuellt | p1_zu: `[tuer1060] ... msg 1`, kein room1040, Bild p1_zu.png; p1_frei: `DOOR FIRE slot=2` -> room1040 |
| 2 Szene 1150 | erfuellt | kette Z.90/1083/1566, msg 22..29, Kontaktboegen s1150_*.png (Arm F36-60, Arm faellt F894-906, Kopf F1032-1128, Aufstehen F1164-1212); Irons gehalten bis F1500 Rueckkehr |
| 3 Knaelle + Spawns | erfuellt | 1130 Cut 0: 5 (kette) bzw. 2 (teil); 1040 Cut 1: 5/5 durchs Tor in kette/t13/t20/luecke/r15_v15, HP 100, Kappe 0; 1030 Cut 7: 5 bzw. 1 Kopie, Cut 6 -> 12 Kriechen; Ton-Ereignisse 1909/1975/2156 |
| 4 Schnitt 11C0 | erfuellt | kette Z.2073..2280, cam=13, msg 10..14, s11c0*.png; Rueckkehr pm=0 |

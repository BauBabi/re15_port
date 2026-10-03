# Runde 35 Spur L "cut1150" — Unabhaengige Abnahme 0

Datum 2026-10-03. Baum `.claude/worktrees/r35_cut1150`, Zweig `r35/cut1150`, HEAD `6f03b1ef` (Basis master
`154a73c1`). Geprueft gegen den WORTLAUT in `AUFTRAG.md` Z. 68-91, Vertrag `VERTRAG.md`, Dossier `L_cut1150.md` (§8).

**Ergebnis: NICHT BESTANDEN.** Punkt 1 und Punkt 4 erfuellt. Punkt 2 teilweise ("steht dann langsam wieder auf"
fehlt, vom Bau-Agenten selbst als OFFEN gefuehrt). Punkt 3 teilweise: der ROOM1040-Teil ("Rolltor hoch gehen und
5 Zombies durch kommen ... Wenn es bereits offen ist, kommen nur 5 Zombies") ist an der echten exe NICHT erfuellt —
bei geschlossenem Tor kommen 2 von 5 durch, bei offenem Tor keiner, und der unsichtbar geparkte Spieler wird dabei
gebissen (HP 100 -> 60). Toene nicht pruefbar (kein Audio-Endgeraet).

## 0. Bau und Messaufbau

* Bau: `bash re15_port/tools/local_build.sh configure` + `... build` -> `=== LOCAL-BUILD-OK (build)`, `ninja: no work
  to do` (exe vom Stand e0c7101c = Code-Stand von HEAD; danach nur Dossier + ein Kommentar in tests/test_support.c).
  exe md5 `a64a210356953483867c8a4eed0a175b`; Messkopie `re15_pc_abn0L.exe` (gleiche md5) im selben Verzeichnis.
* Vor meinem Bau lief im Baum noch eine verwaiste Suite der abgebrochenen Abnahme L#0 (ctest PID 42036, Start
  17:24). Ich habe sie auslaufen lassen (kein zweiter Build im selben build/). Sie ist NICHT aussagekraeftig: unter der
  Last aller Baeume liefen 469..491 in Zeitlimits, integration_r35_cut1150 riss an der SDL-Zusicherung
  `WIN_AddDisplay (SDL_windowsmodes.c:380)` (RDP-Bildschirmwechsel, Lauf `lauf_86047e48_a`, debug.log-Ende).
* Messlaeufe (Scratchpad `.../scratchpad/L0/runs/<lauf>/`, je Lauf debug.log, state.log, PPM-Bilder):
  - `lauf.sh`: Karte aus `probe_r35_cut1150_karte` + CONTINUE (`RE15_CONTINUE_TEST=1 RE15_CARD_AUTO=1
    RE15_CARD_SLOT=0`), `RE15_INPUT_SCRIPT_BASIS=spiel RE15_INPUT_SCRIPT_START=60 RE15_INPUT_SCRIPT=W1,A0.2,W200`
    (Aktionstaste an der Tuer ROOM1130 -> ROOM1150 bei Bild 90 = der ECHTE Tuerweg), `RE15_STATE_LOG=state.log
    RE15_IT_LOG=30 RE15_NOAUDIO=1`, `RE15_EXIT_AT=<bild>#<raum>` als Fenster, `RE15_FRAMEDUMP=<a>-<b>/<s>:f_`.
  - `sprung.sh` (nur Punkt 1): `RE15_TITLE_SHOT=title.bmp RE15_TITLE_SHOT_AF=2 RE15_DEBUG_JUMP=1040@250
    RE15_PLAYER_POS=-20900,-13000,0 RE15_SET_FLAG=<flags>`, Skript `W1,A0.2,R0.7333,U0.3,A0.2,W8` ab Bild 300:
    Leon geht durch die Tuer ROOM1040 -> ROOM1060, dreht sich um und drueckt an der Tuer ROOM1060 -> ROOM1040.
  - Bilder: RE15_FRAMEDUMP OHNE Software-Render (beschleunigter Renderer) — in dieser Sitzung liefert er
    wechselnde, korrekte Bilder (c_hw: 19 Dumps, 6 verschiedene Inhalte; p1_zu: 13 verschiedene). Das Fenster-
    Capture (gdigrab) ist hier weiss und wurde nicht benutzt.

## 1. Punkt 1 — "Bei Wechsel von ROOM 1060 zu ROOM 1040 ... "I have to get the Chief first...", solange man nicht in ROOM 1150 war"

**Urteil: erfuellt.**

Messung ueber den echten Weg (1040 -> Tuer -> 1060 -> umdrehen -> Tuer 1060->1040):

| Lauf | Flags | debug.log | state.log |
|---|---|---|---|
| p1_zu | (9,71)=1 (3,94)=1 | Z.103 `DOOR FIRE slot=2 ... spawn=(26000,0,25300)` -> Z.107 room1060 -> Z.113 `[tuer1060] ROOM1060 Slot 2 -> Text-Platz msg 1 ((9,73)=0 (9,71)=1 (3,94)=1)`, KEIN zweites DOOR FIRE, Z.187 EXIT_AT 520 in 1060 | F366 `PL(26432,25237,rot=4160)` vor der Tuer, F380 `msg(a=1 fsm=1 id=1) pf=FFFF0007` |
| p1_vorirons | keine | Z.108 `Text-Platz msg 1 ((9,73)=0 (9,71)=0 (3,94)=0)`, kein Raumwechsel | F380 `msg(a=1 ... id=1)` |
| p1_frei | (9,71) (3,94) (9,73)=1 | Z.156 `DOOR FIRE slot=2 rect=(27100,25400,...) target_cut=5 spawn=(-21008,0,-13134)` -> Z.159 room1040 | — |
| p1_nurirons | (3,94)=1 | Z.154 `DOOR FIRE slot=2 ...` -> Z.157 room1040 (offen: schon beim Chief, Plan noch nicht gefasst) | — |

Bild `runs/p1_zu/sheet.png` (Bilder 300..520): Leon steht in ROOM1060 (Cut 7) vor der Tuer, dreht sich, ab Bild 370
laeuft der Text ein, ab 410 steht vollstaendig "I have to get the Chief first..." unten im Bild; Leon bleibt im Raum.
Gegenprobe CONTINUE-Lauf c_hw (Karte in 1060) liefert denselben Text (dort Kamera Cut 0 ohne Leon im Bild =
Artefakt des Karten-Standplatzes, nicht des Produkts).
Sperrbedingung (9,73)=0 UND ((9,71)=1 ODER (3,94)=0) ist eine gekennzeichnete PORT-WAHL; sie deckt den Wortlaut
("solange man nicht in 1150 war") in beiden Lesarten. Kein Weichlauf: aus ROOM1060 bleiben Etage 4/8 offen.

## 2. Punkt 2 — Irons-Todesszene ROOM1150

**Urteil: teilweise** (alles bis auf "steht dann LANGSAM wieder auf").

Lauf `s1150` (CONTINUE in 1130, Tuer -> 1150, `RE15_EXIT_AT=1295#1150`, Dumps 0..1295/10). debug.log: Z.85 `Umzug:
1140 lebend 5 ... 1070 lebend 5`, Z.89 `Szene startet (Programm 0)`, Z.1023 `Signal (5,12): Irons' Arm faellt`,
Z.1452 `Signal (5,28): Tuerknall`. Zeitachse aus state.log (Leon mo / Kamera / Nachricht / Irons Clip+Bild):

| Bild | Befund | Wortlaut |
|---|---|---|
| F26-F67 | msg 22 "Leon: Sir!", Leon RBJ-Clip 0 (Bild `runs/s1150/arm.png` F40/F50: Arm nach vorn) | Sir! (Arm) — ja |
| F68-F139 | mo=100 Rennen, Kamera 0->1->3->5, Ankunft (-20443,-25039) | rennt zur Liege — ja |
| F152/F192 | mo=11 Knien, Cut 7 Nahaufnahme | — |
| F212/F333 | msg 23/24 Leon (Text = Wortlaut, auf 2 Nachrichten geteilt) | ja |
| F413/F543/F673/F793 | msg 25/26/27/28 Irons (Wortlaut), Irons Clip 4 / 6 / 5 | ja |
| F673-F893 | Irons Clip 5 bis Bild 101 gehalten (Arm ausgestreckt), Bild `b.png` F700..F890 | Arm ausgestreckt fuer eine Weile — ja |
| F894 -> F923 | Irons Clip 2 ab Bild 73 -> Bild 89 Phase 2 (Arm faellt in ~16 Bildern), Bild F900 Arm quer ueber dem Koerper | abrupt fallen lassen — ja |
| ab F923 | Irons `mo=2 af=89 ss2=2` bis Szenenende und beim Rueckkehr-Besuch (srueck F1400) | tot, bewegt sich nicht — ja |
| F923 | msg 29 "Leon: SIR, Sir?!" | ja |
| F1000-F1160 | Leon kniet, Kopf gesenkt, Kopfschuetteln sichtbar langsam (Bild `kopf.png`: F1040/1050 rechts, F1080 links, F1110/1120 rechts) | schuettelt langsam den Kopf — ja |
| F1163-~F1188 | Aufstehen = PL00 Clip 11 rueckwaerts im Original-Takt (kopf.png F1160 kniend, F1170 halb, F1180 steht) = ~25 Bilder 0,8 s | **steht LANGSAM wieder auf — NEIN** |
| F1203-F1263 | Cut 5 Totale, Leon steht an der Liege, 60 Bilder Pause, dann Knall-Signal | kurze Pause — ja |

Zusatzmessung "tot bleibt tot" an der exe: Rueckkehr-Besuch (Lauf srueck), Bild 90-95 Aktionstaste (`pad=8000`, pm=0)
mit Leon bei (-20538,-25147) Gierung 1500 -> Vorwaertspunkt (-20952,-25609) liegt IM Ansprech-Rechteck
ROOM1150 Slot 2 @0x00D92 (-22500..-20800, -26700..-25300); Ergebnis `msg(a=0)` durchgehend — kein "I'll be fine".

## 3. Punkt 3 — Knaelle und Spawns 1130 / 1040 / 1030

**Urteil: teilweise** (1130 und 1030 erfuellt; 1040 nicht erfuellt; Toene nicht pruefbar).

* **1130 Cut 0** (Lauf s1130, Fenster `175#1130`): debug.log Z.1427 `Montage 1130 (Programm 1)`, Z.1437 `Signal
  (5,28): Tuerknall`; Messzeilen T30..T150: 5 Zombies (Typen 0x16,0x10,0x10,0x11,0x11) stehend im Flur vor der
  Briefing-Room-Tuer. Bild `runs/s1130/a.png`: ab F40 fuenf Zombies im Bild, Kamera Cut 0. Teil-Lauf s1130teil
  (`RE15_SET_FLAG_AT=7:0xd3,7:0xd5,7:0xc6@50` = 2 der 1140-Zombies tot): `Umzug: 1140 lebend 3`, Messzeile T150 = genau
  3 Zombies. Lauf sB (alle tot): kein room1130, `Montage 1040` direkt. -> erfuellt.
* **1040 Cut 1, Tor geschlossen** (Lauf s1040, Fenster `438#1040`, Dumps 0..438/6): Kamera Cut 1, Signal (5,29),
  Rolltor faehrt hoch (Bilder F90-F180). Positionen aus state.log (Gegner-Spalten): Start Bild 1 Zombies 1..5 bei
  z 252 / 3352 / 13368 / 16505 / 15000 (alle hinter dem Tor, Tor-Objekt z -360). **Durch das Tor (z < -360) kommen
  nur Zombie 2 (ab Bild 276) und Zombie 1 (ab Bild 288).** Zombies 3/4/5 laufen VOM Tor WEG: Bild 438 bei z 19506 /
  22876 / 22872 (+6000..+7800). Bild `runs/s1040/a.png` F300..F438: genau zwei Zombies im Flur. -> "5 Zombies durch
  kommen" NICHT erfuellt (2 von 5).
* **1040 Cut 1, Tor schon offen** (Lauf s1040offen, `RE15_SET_FLAG_AT=4:5,4:4@50`, Fenster `272#1040`): KEIN Zombie
  kommt im Schnitt durchs Tor (Bild 272: Zombie 2 bei z 111, Zombie 1 bei z 7505, 4/5 bei z 21698/19640, wieder vom
  Tor weg). Zombie 3 (Raum-Record schon VOR dem Tor, z -11126 ab Bild 1, hinter der Kamera z -10692) laeuft zum
  geparkten, unsichtbaren Spieler und packt/beisst ihn: state.log Spieler `hp=100` -> F138 `hp=80` -> F176
  `hp=60`, Spieler-mo 3/4/5 (Griff), pm=2. Bild `runs/s1040offen/a.png`: ein bis zwei ferne Zombies im Flur, niemand
  kommt naeher. -> "Wenn es bereits offen ist, kommen nur 5 Zombies" NICHT erfuellt, dazu Schaden waehrend der
  Szene (bei <= 40 HP waere der Spieler in der Szene gestorben).
* **1030 Cut 7** (Lauf s1030): debug.log Z.1674 `Montage 1030`, Z.1686 Signal (5,28); Bild `runs/s1030/a.png` F42..F180:
  fuenf stehende Zombies vor der 1070-Tuer. -> erfuellt.
* **1030 Cut 6, Kriechen** (s1030 bei (4,15)=0 und s1030b bei `RE15_SET_FLAG_AT=4:15@50`): Kamerafolge 7 -> 6 (F181)
  -> 12 (F218) bzw. direkt 12; Bilder F210..F546: Zombies legen sich hin und kriechen unter dem Tor durch, liegen am
  Ende davor. Messzeile s1030b: Plaetze 12/13/14 `fl=1004`, z -22591 / -21007 / -20699 (vor dem Tor). -> erfuellt.
* **Knall-Toene**: Lauf saudio ohne RE15_NOAUDIO: `[audio] SDL_OpenAudioDevice failed: WASAPI can't find requested
  audio endpoint` -> `Knall-Tonbank nicht geladen`. Belegt ist nur der Aufruf (debug.log "Signal (5,28)/(5,29)" je
  Raum). -> nicht pruefbar auf diesem Rechner.

## 4. Punkt 4 — Schnitt ROOM11C0 Cut 13 (Ada/Marvin)

**Urteil: erfuellt** (Anmerkung zur Bildkomposition, M5).

Lauf s11c0 (Fenster `516#11C0`, Dumps 0..516/6), debug.log Z.1839 room11c0, Z.1851 `Montage 11C0 (Programm 4)`; Lauf
srueck: Z.1762 room1150, Z.1776 `Rueckkehr: Programm 5`, Z.1790 `Rueckkehr beendet, Steuerung frei`, letzte Zeile
`F1400 PL(-20538,-25147,rot=1500) cam=5 ... pm=0`. state.log 11C0 (Aktor/Gierung): Ada (0x42) und Marvin (0x40)
r475 -> r2719/r2723 (beide zum Gebaeude, F36-F56); msg 10 F81, 11 F171, 12 F231; Marvin r605 = zu Ada (F314);
msg 13 F331 mit Marvin Clip 15; Marvin rennt ab F399 ((-11664,-9979) -> F512 (-17322,5608)); msg 14 F412 mit Ada
Clip 15. Bild `runs/s11c0/a.png` + `arm.png`: alle fuenf Zeilen im Wortlaut, Marvins Arm zu Ada F348-F360, Marvin
rennt F402 aus dem Bild, Ada F432/F444 mit ausgestrecktem Arm, Ende -> Leon in 1150.

## 5. RE-Gate

* Stichproben SELBST disassembliert (PSX.EXE, re15_disasm.py) — alle enthalten das Behauptete:
  - @0x80045094 `sltiu v0,v1,0x6` / @0x80045098 `beq v0,zero,0x8004539c` (Se_on verwirft Bank >= 6).
  - @0x80050fb8 `andi v0,a2,0x8` -> zweiter `jal 0x8001f314` @0x80050fd0 (Flag 0x08 = zweiter Schritt je Bild; der
    Motion-Sub ruft anim_set sonst genau einmal, @0x80050d4c/@0x80050f94; a3 fest 0x200). Die Aussage "kein langsamerer
    Takt im Motion-Sub" stimmt; andere Wege (anderer/langsamer Clip, RE2 Retail) sind nicht untersucht (M3).
  - @0x80042120 `lbu a1,7(a1)` / @0x80042128 `beq a1,0xff` / @0x80042130 `jal 0x8004efe4` (Tot-Bit pc[7] gegen die
    Zonenbank) — Gate fuer die Zone-7-Bits.
  - @0x8004044c..@0x800404a8: je Zonen-Satz (Schritt 20) Byte0/Byte1 a<->b getauscht = Cut_replace ist ein TAUSCH.
  - @0x80041f5c `lh a1,8(v0)` / @0x80041f68 `sb a1,158(v1)` = Plc_neck pc[8] -> +0x9e.
  - @0x8003f064..@0x8003f084: `ori a0,1` ... `jal 0x8003ee3c` mit a1=1 = sub01-Reseed je Takt.
* RDT-Byte-Offsets gegen die Dateien geprueft (alle stimmen): ROOM1030 @0x02776 `36 02 0c 00 00 00 cc dd f8 f8 f0 a7`,
  @0x0276C, @0x02782, @0x0278E, @0x02796, @0x0279E.. (`18 09` + Pausen 5/20/10/15/20/180), @0x02804; ROOM1150
  @0x00D92, @0x00EBC..@0x00ED5, @0x01110, @0x0125A, @0x012C4..; ROOM1060 @0x00D52; ROOM1040 @0x011F0 `24 12 05 00`,
  @0x019B2..; ROOM11C0 @0x01770; ROOM11B0 @0x0154E; ROOM4001 @0x018A4 `41 04 02 00 00 00 00 00 1e 00`.
* Unschaerfe: "Tor-Fahrt = sub08 @0x019B2..@0x01A1A woertlich" (irons_tod_1150.c Programm 2) stimmt nicht woertlich —
  das Original hat drei Schleifen For 20 / Cut 1 / For 50 / Sca_id_set+Sca_floor_set / For 65; der Port eine For 135
  und setzt die Sca-Zellen erst NACH 135 statt nach 70 Bildern (M6).
* Ohne Beleg/Kennzeichnung: Marvins Laufziel `OP_DEST(5, 0x22, -22000, 8000)` (Programm 4) traegt weder @0x noch
  PORT-WAHL; Marvins Standort ist als "links von Ada = PORT-WAHL" beschrieben, steht im Bild aber RECHTS von Ada am
  Bildrand (M5).
* Wortsuche im Diff (deferred/tunable/interim/for now/faithful/plausibel/TODO): nur ein Treffer, ein Zitat des
  Original-Texts "for now" (ROOM1150 msg 2). Mess-Haken RE15_IT_LOG ist env-gegatet und kein Abschluss-Schalter.
* Fix erklaert Befund: tot_bleibt_tot (an der exe nachgemessen, §2), Kriechen (fl=1004 + Bild), Se_on-Pins (gruen),
  Marvins Arm (Bild), Kopfschuetteln (Bild) — jeweils ja.

## 6. Vertrag / Pfade / Tests

* `git diff master --name-only`: kein release/, platform/android/, shared_assets/PSX/; tests/unit/CMakeLists.txt und
  tests/integration/CMakeLists.txt unberuehrt. Gemeinsame Dateien: scd_room_setup.c (+7), scd_vm.c (+4/-1), main.c
  (+5), tests/test_support.c (+6), dazu enemy_common.c (+13/-1, nicht in der Liste des Vertrags, Alias-Mechanismus) —
  klein und markiert.
* Bank 9: 73/74/75/76 (zugeteilt). Nachrichten 1150: 22..29, 1060: 1, 11C0: 10..14 (zugeteilt). Ereignis 21 in
  1150/1130/1040/1030/11C0, Slot 20 — keine Ueberschneidung mit J (affen: kein Slot/Ereignis 20/21 in 11C0) gefunden.
  Vertragsabweichungen (im Dossier offen ausgewiesen): Zone-7-Tot-Bits 40-43, 46-49, 75-79, 83, 84, 90, 91, 95 und
  Bank-5-Scratch 12/28/29. Gegenprobe gegen Spur K: K setzt Zone 7 Bits 209/210/221/44/45 (ROOM11D0) — keine
  Ueberschneidung.
* Tests selbst gefahren (PowerShell, `ctest -R "r35_cut1150|^unit_se_bank_routing$|^unit_r34_plattform$|^unit_r34_plattform_ton$"`):
  **16/16 gruen**, integration_r35_cut1150 356 s. Volle Suite: Dossier §8.9 traegt woertlich
  `=== LOCAL-BUILD-OK (all) — Tests 491/491` (auf a8ab2f27, Code = HEAD); nicht selbst wiederholt (Auftrag Schritt 2).
* Tests messen am Wortlaut vorbei (M4): `unit_r35_cut1150_montage_1040` prueft `aktive_gegner(0x16) == 5` (fuenf
  ERSCHIENEN), nicht "durch das Tor gekommen"; kein Test prueft die Spieler-HP waehrend der Montage; der
  Integrationshaken laeuft mit RE15_NOAUDIO=1 und ohne den Fall "Tor schon offen".

## 7. Maengel (nummeriert, nachpruefbar)

1. **M1 — ROOM1040, Tor geschlossen: nur 2 von 5 Zombies kommen durch.** Lauf s1040 (Karte `nach10f0 ersteszene`,
   Tuer 1130->1150, `RE15_EXIT_AT=438#1040`): state.log-Gegnerspalten, z < -360 nur Zombie 2 (Bild 276) und 1 (Bild
   288); Zombies 3/4/5 (Raum-Records main00 @0x011FC.., Startpositionen z 13368/16505/15000) laufen bis Bild 438 nach
   z 19506/22876/22872 (vom Tor weg). Wortlaut "Rolltor hoch gehen und 5 Zombies durch kommen".
2. **M2 — ROOM1040, Tor offen ((4,5)=1): kein Zombie kommt durch, der geparkte Spieler wird gebissen.** Lauf
   s1040offen (`RE15_SET_FLAG_AT=4:5,4:4@50`, `RE15_EXIT_AT=272#1040`): Zombie 3 startet bei (-25617,-11126) vor dem
   Tor hinter der Kamera und erreicht den Parkplatz RE15_IT_PARK_1040 (-24950,-14900; PORT-WAHL "Schalter-
   Rechteck") — Spieler hp 100 -> 80 (Bild 138) -> 60 (Bild 176), mo 3/4/5, waehrend pm=2. Wortlaut "Wenn es bereits
   offen ist, kommen nur 5 Zombies". Der Parkplatz ist im Offen-Fall nicht sicher (kein Schutz gegen Gegner-Kontakt).
3. **M3 — "steht dann langsam wieder auf" nicht erfuellt.** Aufstehen = PL00 Clip 11 rueckwaerts im Original-Takt
   (`3f 01 0b 00` + `43 00 80 00`, Programm 0 nach @0x012C6/@0x012CA), Lauf s1150 Bild 1163 -> ~1188 (kopf.png F1160
   kniend, F1170 halb, F1180 stehend). Der Motion-Sub kann nicht langsamer (@0x80050d4c ein anim_set je Bild,
   @0x80050fb8 nur ein ZUSAETZLICHER Schritt) — nicht untersucht sind ein langsamerer vorhandener Aufsteh-/Erhebe-Clip
   (RBJ-Bibliothek, rbj_zensus) und RE2 Retail (Regel Beta -> Retail). "bitte ausdruecklich bestellen" ist keine
   Antwort: der Wortlaut bestellt es.
4. **M4 — Riegel messen einen Ersatzwert.** `unit_r35_cut1150_montage_1040` (test_r35_cut1150.c ~Z.541/549/565)
   prueft "5 aktive Gegner", nicht "z < -360 vor dem Schnitt"; deshalb blieben M1/M2 gruen. Es fehlen: Durchgangs-
   Pruefung je Zombie, Spieler-HP unveraendert ueber die ganze Montage, Integrationslauf mit offenem Tor.
5. **M5 — 11C0-Bild: Marvin am rechten Bildrand angeschnitten.** Standort (-10500,-12300) liegt ~3,8 m vor der
   Kamera Cut 13; Bilder s11c0 F12..F402: Marvin steht RECHTS von Ada und ist vom Bildrand beschnitten, seine Arm-
   Geste nur teilweise sichtbar (arm.png F348/F360). Kommentar "Standort links von Ada" (irons_tod_1150.c Programm 4)
   stimmt nicht mit dem Bild; das Laufziel (-22000,8000) traegt keine Kennzeichnung. (Kein Wortlaut-Verstoss, aber
   die Arm-Geste "I have to help him, sorry!" ist so schlecht zu sehen.)
6. **M6 — Tor-Fahrt nicht "woertlich".** k_p_1040_tor fasst For 20/50/65 zu For 135 zusammen und setzt Sca_id_set /
   Sca_floor_set (Original @0x01A00..@0x01A0C nach 70 Bildern) erst nach 135 Bildern; der Kommentar behauptet
   woertliche Uebernahme von @0x019B2..@0x01A1A.
7. **M7 — Toene nicht verifizierbar.** Kein Audio-Endgeraet (`SDL_OpenAudioDevice failed: WASAPI ...`), Integrations-
   haken mit RE15_NOAUDIO=1; ob Tuerknall (RE2 DOOR04, PORT-WAHL) und Knall (ROOM1030 Satz 0x0c) hoerbar/laut sind, ist
   offen (Weg im Dossier §6 benannt). Nicht als Defekt gewertet, aber Punkt 3 ist dadurch hoechstens "teilweise".

## 8. Urteile je Punkt (Kurzform)

| Punkt | Urteil | Beleg |
|---|---|---|
| 1 Dialog 1060 -> 1040 | erfuellt | p1_zu Z.113 + F380 msg id=1, kein DOOR FIRE; p1_frei/p1_nurirons DOOR FIRE -> room1040; p1_zu/sheet.png |
| 2 Szene 1150 | teilweise | s1150 Zeitachse §2; Aufstehen 25 Bilder (M3) |
| 3 Knaelle + Spawns | teilweise | 1130/1030 ja; 1040: 2 von 5 (M1), offen 0 + Biss (M2); Ton nicht pruefbar (M7) |
| 4 Schnitt 11C0 | erfuellt | s11c0 msg 10..14, Drehungen, Arm, Lauf aus dem Bild; srueck Rueckkehr pm=0 |

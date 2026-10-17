# Runde 38 — Spur F "dining1141" — Abnahme 0 (unabhaengig, laufend geschrieben)

Baum `.claude/worktrees/r38_dining1141` (Zweig `r38/dining1141`, Kopf c6fe0fd0, Merge-Basis mit master 32a646af).
Pruefer: unabhaengige Abnahme 0 (2026-10-17). Grundlage: Wortlaut `AUFTRAG.md` (Runde 38 + Nachtrag 2) und
`analysis/befunde_runde37/AUFTRAG.md` Nachtrag 14, `VERTRAG.md`, Dossier `F_dining1141.md`. Dem Bau-Agenten wird nichts
geglaubt, was hier nicht selbst gemessen ist. Messordner (scratchpad): `F38abn0/`.

## 0. Stand der Abnahme
- Bau: `local_build.sh configure` + `build` gruen ("ninja: no work to do", LOCAL-BUILD-OK), exe 05:19 = Kopf c6fe0fd0
  (Arbeitsbaum sauber).
- Messlaeufe E1a-E10 (eigene Wegplaene, echte Tueren) + Gegenprobe v0.8.25 (Paket nach scratchpad kopiert): Abschnitt 2.
- **Ergebnis: NICHT bestanden** — P1 und P3 erfuellt, P2 und P4 teilweise (Maengel M1, M3; dazu M2, M4), Abschnitt 3-5.

- Gezielte Riegel (keine volle Suite): Unit-Gruppen aller beruehrten Module (eigene 7 + scd/aot/item/inv/se/audio/
  voice/tuer/msg/r36-r37-Inventar/Schluessel/Elza/Roy/room1140/rolltor, 107 Tests) **107/107 gruen (20,7 s)**;
  Integration (eigene w/z/d/l/s + r37_roy_szene1011/1021, r37_elzaems_elza1141, r36_schluessel(_befreiung),
  r36_inventar, item_name_census, r36_story_elza, door_lock, keydoor, r36_tueren, r36_elza_unlock; -j3)
  **17/17 gruen (757 s)**, darin integration_r38_dining1141_s 541,6 s. Logs `F38abn0/ctest_unit.log`,
  `F38abn0/ctest_integ.log`.

## 1. RE-Gate (Stichproben selbst disassembliert / Bytes gelesen)
| Behauptung (Ort im Diff) | selbst gemessen | Urteil |
|---|---|---|
| Cut_old = FUN_8004032c liest @0x8004033c den von Cut_chg gemerkten Cut, loescht Bit 0x100 (programme_bauen.py, Dossier 4.3) | PSX.EXE: LAB_800402a0 @0x800402c0 `lbu a1,0x0fe4(0x800b)` + @0x800402e4 `sb a1,0x3f7b(0x800b)` (alter Cut gemerkt), @0x800402d4 `ori v0,v0,0x100`; FUN_8004032c @0x8004033c `lbu a0,0x3f7b(0x800b)` -> @0x80040364 `sh a0,0x0fe4`, @0x80040378/7c `addiu v1,zero,-257; and` = Bit 0x100 aus | stimmt |
| Form `2a 00 3c 01` ROOM1150 @0x010F2 | ROOM1150.RDT @0x010EA `29 06 2b 01 ff ff 02 00 2a 00 3c 01` | stimmt |
| Item 0x43 Satz @0x80074DA8+0x43*12 `01 00 00 00 88 4c 07 80 ..` | @0x800750CC `01 00 00 00 88 4c 07 80 00 00 00 00` (gleich 0x36/0x3A/0x40/0x41/0x42/0x44/0x45) | stimmt |
| Klasse Schluessel `sltiu id,0x22` @0x80049124 | @0x80049124 `sltiu v0,v0,0x22` | stimmt |
| Bankweiche FUN_80045024 `sltiu 0x6` @0x80045094, `lw a0,0x8` @0x80045138 (dining_r38_pc.c Kopf) | `sltiu v0,v1,0x6` @0x80045094 stimmt; das `lw a0,8(v0)` steht @0x8004513c (@0x80045138 ist ein `nop`) | Adresse um 1 Befehl verrutscht (Kommentar, kein Verhalten) -> Maengel M4 |
| pos_advance @0x800245d8 (x += cos, z -= sin) | @0x800245f0 `lhu v0,0x8c(ent)` als x-Komponente, `jal 0x800659d0` (RotMatrixY), GTE-MVMVA -> (cos*v, 0, -sin*v) | stimmt |
| ROOM1141 sub00 @0x00B4E/@0x00E5C `54 00 01..`, sub01 @0x00C06 `54 00 02..`, sub06 @0x00FF6 `22 03 6a 01`, Spawn @0x00B06 (-6750,-13350), Prop obj 0 x @0x00B9C+10 `2c e4`, sub05 @0x00FC8.. | Bytes gelesen: alle wie zitiert (@0x00FC8 `46 02 00.. 22 04 c3 01 2b 0c ff ff 02 00 46 01`) | stimmt |
| ROOM1131 sub02 Rolladen-Ton @0x00A28 `06 00 d0 00`, @0x00A48/@0x00A58/@0x00AB2 Se_on(2,0x0C/0x0A/0x0B) + Sleep 15/15/10 | Bytes gelesen, wie zitiert | stimmt |
| Rolladen-Wellen nur in der 1130/1131-Bank | snd0 1130/1131: Saetze 0x0A/0x0B/0x0C `00 00 76 16`/`87 16`/`95 11`, VAG-sha1 f240d33c/5920, ce8ed301/2240, 7819c42b/336; 1140/1141: `00 00 45 11`/`55 11`/`66 16`, andere VAGs (7f67e256/912, 487c19e0/3168, 0f0c7871/7440) | stimmt |
| Klopfen = RE2 ROOM1060 sub05 @Datei 0x0F26 (+0x22 Se_on(2,0x0A), +0x2E Sleep 30, +0x32 Se_on(2,0x0B), +0x3E Sleep 30, +0x42 Ifel/Cmp, +0x4C Se_on, +0x58 Sleep 90, +0x5E Sleep 90, +0x62 Goto); KLOPF1060.* bytegleich | info/re2leon/PL0/RDT/ROOM1060.RDT gelesen: Bytes wie zitiert; EDT 0x3F58/VH 0x4018/VB 0x4C38, KLOPF1060.EDT/.VH/.VB bytegleich (True/True/True) | stimmt (Port spielt den 3. Schlag unbedingt, RE2 hinter Ifel/Cmp; Takt-Form PORT-WAHL, im Dossier genannt) |
| Roys Spawn 1011 sub00 @0x009F6 (-2000,0,7450), 1021 sub06 @0x02202 `2b 09 ff ff`, 1131 Door_aot_set @0x0088E | Bytes gelesen, wie zitiert | stimmt |

Suche im Diff nach "deferred/tunable/interim/for now/faithful/plausibel/TODO": siehe Abschnitt 4 (0 Treffer).

## 2. Messungen am Spiel (echte exe, eigene exe-Kopie je Lauf `re15_pc_abn0F_<lauf>.exe`, Elza per
RE15_PSELECT_AUTO=1 RE15_PSELECT_AUTO_SWITCH=1, Wegplan RE15_WEGPLAN, RE15_FRAMEDUMP mit RE15_WINDOW_SCALE=1,
RE15_AUDIO_CAP_SYNC, RE15_MSG_LOG/RE15_STATE_LOG/RE15_SPAWN_DIAG/RE15_SE_DEBUG/RE15_BGM_CTL_DEBUG)

### 2.1 Lauf E1a/E1b/E1c — die ECHTE 1021-Szene (nicht per Flag uebersprungen), dann 1011 und zurueck
Stand vor der Szene = nach ROOM1211 (ablauf_elza.md E12): `RE15_SET_FLAG=4:0xF1,3:0x74,4:0xEC,4:0xFC,4:0x60,3:0xC9,
3:0x2C,3:0xBA` + Tot-Bits der Zombies 1021/1011 `7:0xAA,7:0xAB,7:0xAD,7:0xB5,7:0xB8,7:0xB9` (Kampf ist nicht Teil
der Spur), `RE15_DEBUG_JUMP=1021@gp RE15_PLAYER_POS=-15500,-7100,0` (Westteil, wie vom 1041-Eintritt), danach NUR
Gehen/Druecken/Tueren. Szene durch Hineingehen in die Ausloese-Zone Slot 5 (main00 @0x01DE2) gestartet.
- E1a: Szene laeuft (spawn-diag `type=0x49` (-4274,-6590), `type=0x47` (-4614,-16919); msg 0..8), danach Flags
  (3,0x5F)=1, (4,0xF0)=1, (4,0x60)=0 (Wegplan-FLAG F1812-F1814). Roy liegt als totes Aktor (t=47 hp=-1) bei
  (-4431,-10689), John steht bei (-4274,-8765). Elza geht an die Leichenstelle (-3803,-9601), Blick 2053, AKTION
  F2029-F2032 und fuenf weitere Drucke: **keine Nachricht** (kein `[msg] room=1021 id=40`/`id=9`, Bilder
  `e1a_leiche.png` F1990-F2300 ohne Textfenster). Grund (Original-Mechanik): die Leichen-Zone Slot 6 setzt nur main00
  @0x01E24..@0x01E2C unter Ck(3,0x5F)==1 beim Raumaufbau; sub02 (die Szene) setzt sie nicht. Der Hinweis ist also im
  Besuch der Szene nicht erreichbar -> Maengel M1.
- E1c: Tuer Slot 1 -> `loaded room1011.rdt`, `[dining] ROOM1011: Dining Room Key (Item 0x43, Bit (9,64)) Prop obj 6
  bei (-2000,0,7450), Zone Slot 14`; AOT-Liste F34: Slot 14 typ 2 mitte (-2000,7450) halb (500,500); (9,64)=0 beim
  Eintritt, Aufnahme an (-1372,7415) Blick 2096 -> Item-Bild RE2-Herz-Schluessel (F200) + "Will you take ..." (F230),
  (9,64)=1 (F424). Bilder `e1c_schluessel.png`, `e1c_zoom.png`: am Boden ist der Schluessel auf Roys Blutlache nur
  ein 1-3-Pixel-Lichtpunkt (Beobachtung B1).
- E1b: zurueck ueber 1011 Slot 0 -> zweites `loaded room1021.rdt` (debug.log Z.1831): KEIN `Sce_em_set type=0x49`
  nach dem zweiten Laden (John weg), (4,0x60)=0; AOT Slot 6 typ 0 ev 6 mitte (-4350,-9600) jetzt da; Leiche ->
  `[msg] room=1021 id=40`; Bilder `e1b_hinweis.png` (F260 "He's dead." -> F320 "Poor Roy..." -> F390/F450
  "There's an empty key tag on his belt. / It reads “Dining Room”."), Zoom `e1b_text_zoom.png`: Wortlaut exakt wie
  vom Nutzer, Anfuehrungszeichen als Schriftglyphen, Punkt hinter dem schliessenden Zeichen.

### 2.2 Lauf E2 — Elza ROOM1131 OHNE Schluessel (Tuer zu)
Flags = Stand nach der 1021-Szene (4:0xF1,3:0x74,4:0xEC,4:0xFC,3:0x2C,3:0xBA,4:0xF0,3:0x5F), `RE15_DEBUG_JUMP=1131@gp`,
Start am echten Eintrittspunkt aus 1121 (-2200,0,-2150) (1121 Slot 2 @0x00CD8), zu Fuss an die Doppeltuer.
- `[dining] ROOM1131 Slot 0 (Tuer -> ROOM1141) gesperrt: Ereignis 29`; AOT-Liste: Slot 0 typ 0 ev 29 mitte (-300,-13900).
- Zwei Versuche (F236, F425): je Schloss-Ton (`[se] Stimme ... SE-Stimme 6`), msg 40 "It's locked." / "The door is
  marked “Dining Room”.", kein Raumwechsel, (9,66)=0. Bilder `e2_tuer_zu.png`.
- Beobachtung B2: im Hintergrundbild haengt direkt ueber dieser Doppeltuer das Schild **"Briefing Room"**; der Text
  behauptet ein Schild "Dining Room" (vom Bau-Agenten selbst im Dossier Abschnitt 6 vermerkt) -> Maengel M2.

### 2.3 Lauf E3/E3b — Elza ROOM1131 MIT Schluessel -> 1141 -> 1171 (echte Tueren, nur Wegplan)
Flags wie E2 + 9:64, `RE15_GIVE=0x43:1` (Schluessel aus E1c), `RE15_R38F_GORILLAS_STARR=1`, `RE15_R38F_GORILLAS_TOT=1800`
(Messhaken: Kampf ist nicht Teil der Spur; setzt nur die Tot-Bits (7,0xCB)/(7,0xCC), die sub01 @0x00BFA/@0x00C02 liest),
FRAMEDUMP jedes 3. Bild, Ton-Mitschnitt `e3/cap.raw` (1470 Stereo-Frames je Spielbild, 30 Bilder/s).
- 1131 (E3b, Bilder `e3b_schluessel_tuer.png`): "You've used the Dining Room Key." -> "You don't need this key any
  more. Discard it? Yes / No" -> naechster Druck geht durch die Tuer (`[tuer] Sequenz fertig ...`, `loaded
  room1141.rdt`), (9,66)=1.
- 1141 Phase H (Bilder `e3_phase_h.png` F63-F957): `ROOM1141 sub00 @0x00B02 Evt_exec sub02 -> Klopf-Schleife`, John
  (t=49) ab Bild 1 bei (-7124,-6150) hinter dem geschlossenen Rolladen, in keinem Bild zu sehen; KEIN Textfenster bis
  F957 (erste Nachricht in 1141 = msg 13 Gemaelde, Zustands-Log). Toene (debug.log): Klopfen 0x3C Satz 10/11/11 bei
  F1/35/65, 245/275/305, 485/515/545, 725/755/785; Hilferuf 0x3D bei F155/395/635/875 (= alle 240 Bilder = 8 s),
  `[voice] clip loaded: main42.wav`; Musik `Sce_bgm_control slot=0 op=1` beim Eintritt (MAIN16, `[bgm] stage=0
  room=14 entry=FF56 -> MAIN16`).
- Ton-Mitschnitt selbst ausgewertet (`ton.py`, Kreuzkorrelation mit main42.wav 32 kHz -> 44,1 kHz): Hilferuf an Takt
  4020/4260/4500/4740: Korrelation 0,856/0,810/0,764/0,823 (Kontrolle 90 Bilder vorher 0,054-0,059), Verstaerkung
  1,27-1,31 -> deutlich im Mix. Klopfen: erster Schlag (Satz 0x0A) RMS x3,4..x8,7 gegen davor (Spitze 16654-20359);
  der 2./3. Schlag (Satz 0x0B) nur x0,73..x1,62 (RMS 1013-2383 bei Musik ~2000) -> Beobachtung B3.
- Gemaelde (F957 "A beautiful painting. There's a switch below. Will you push it?" -> Yes): Rolladen-Ton 0x3B Satz
  12/10/11 bei F1148/F1163/F1358, Mitschnitt: Klick +2 Takte RMS 2421, Motor ab +15 (2915), Anschlag +210 (3133);
  Musik aus bei capTick 5233 (John frei). Bilder `e3_rolladen.png`: F1200-F1230 Johns Beine unter der Rolladenkante,
  F1290-F1350 John im Gang, F1380-F1410 laeuft heraus, F1428 "Elza: Are you okay? what's happened?" (+ main40.wav).
- Szene: Nachrichten 13, 40, 3, 4, 5, 6, 7, 8 (Cut 2: Gorillas schon im Raum), 9, 10, 11 — **nie 0/1/2**. Ende: John
  laeuft zur Kamera und verlaesst das Bild am unteren Rand (F2760-F2784, `e3_john_ecke.png`), steht danach in Cut 0
  in der Ecke; Gorillas nach Pos_set @0x00F5C/@0x00F6C in Cut 5/0 nicht im Bild.
- Tuersperre (Bilder `e3_sperre.png`): Doppeltuer F3027 und Dachtuer (Riegel-Zone) F3429 je "I cannot leave him
  alone with those monsters."; AOT-Liste F2855: Slot 0 typ 5 ev 41, Slot 2 typ 5 ev 41.
- Gorillas tot (Messhaken F4549) -> Musik aus (capTick 8414, Form sub01 @0x00C06), Cut 7 ab F4579; John laeuft auf
  die Kamera zu, F4653 verlaesst er das Bild unten (`e3_abgang.png`), DOOR1A auf/zu (`Se_on bank=14 id=0/1 ->
  RE2-Tuerbank`) im Off, F4731/F4737 Cut_old -> Cut 0 mit Elza im Bild. AOT-Liste F4944: Slot 0 typ 1 (Tuer 1131),
  Slot 1 typ 1 (Dachtuer), Slot 2 typ 11 (aus); (9,67)=1, (4,0xC3)=1.
- Dachtuer -> `loaded room1171.rdt`.

### 2.4 Lauf E4a/E4v — Leon ROOM1140 Gemaelde (Punkt 3), dieser Stand gegen v0.8.25
Beide exe gleich (v0.8.25-Paket nach scratchpad kopiert, eigene exe-Kopie): Leon, Strom (2,0)=1, Zombies tot
(7:0xD3..0xD7), `RE15_DEBUG_JUMP=1140@gp`, zu Fuss an das Gemaelde, Ja: in BEIDEN Staenden msg 1 -> msg 5 "The switch
is not responding..." (sub05; sub01 @0x00C7A `46 05 03 31 ff 00 18 05`), der Rolladen faehrt NICHT. Ein Leon-Rolladen,
der hochfaehrt, war also schon in v0.8.25 nur ueber sub03 erreichbar (RE15_FORCE_EVENT=3) — der Nutzer-Befund kann sich
am echten Weg nur auf Elzas 1141 (gleicher Rolladen, sub06) beziehen; dort spielt der Ton jetzt (E3).
Leon sub03 mit Ton: integration_r38_dining1141_l in diesem Lauf gruen (0x3B 12/10/11 + Port-Bank geladen).

### 2.5 Laeufe E5/E6 — Elza steht beim Tod der Gorillas im Dachgang bzw. an der Dachtuer (Johns Abgangsweg)
Gleicher Weg wie E3, nach der Szene geht Elza in den Dachgang und wartet dort (realistisch: der Gang ist nach dem
Hochfahren offen, die Gorillas jagen Elza, man kann aus dem Gang schiessen); `RE15_R38F_GORILLAS_TOT=500`.
- E5 (Elza wartet bei (-7342,-4062), Mitte Dachgang, Blick Sueden): Messhaken F2636, Cut 7 ab F2667. Zustands-Log:
  John (-7351,-6375) F2760 -> (-7351,-3375) F2775 -> (-7351,-975) F2790; **Elza wird mitgeschoben**: (-7342,-4062)
  bis F2760, (-7304,-2674) F2775, **(-6488,-714) F2790** — 3,5 m ohne Eingabe (Pad gesperrt). Danach Cut_old ->
  Cut 5 -> Zone -> Cut 6: Bilder `e5_schub.png` F2600-F2660 (Cut 5, Elza in der Mitte des Gangs) gegen F2822-F2900
  (Cut 6, Elza neben der Dachtuer). Kein Haenger, John geparkt F2805, Dachtuer -> 1171.
- E6 (Elza wartet bei (-7263,-1030) direkt vor der Dachtuer, Cut 6): F2780 -> F2790 Elza (-7263,-1030) ->
  (-6503,-718), also 0,8 m seitlich weggeschoben, waehrend John (-7351,-975) erreicht; nach Cut_old wieder Cut 6 mit
  Elza an anderer Stelle als vor dem Abgang. Kein Haenger.
- Ursache (Code): BEFREIUNG-Schluss +0x0402.. setzt Pad-Sperre und Cut 7 und schickt John per `40 00 05 01` (RUN) an
  (-7350,-7600) und (-7350,-700), ohne Elzas Lage zu beachten; die Aktor-Kollision schiebt die Spielfigur. Der Abgang
  selbst bleibt unsichtbar (beide hinter der Kamera von Cut 7), aber die Spielfigur steht nach der Szene woanders -> M3.

- E7 (Elza wartet direkt auf Johns Zielpunkt vor der Dachtuer, (-7332,-718)): F2780 -> F2790 Elza (-7332,-718) ->
  (-6546,-718) -> (-6493,-701); Bilder `e7_schub.png`: F2600-F2666 Cut 6, Elza steht frontal vor der Dachtuer;
  F2822-F2840 wieder Cut 6, Elza steht rechts NEBEN der Tuer — sichtbarer Lagesprung der Spielfigur in derselben
  Kamera, John ging "durch" die Tuer, vor der sie stand. Kein Haenger (John erreicht (-7351,-975)).
- **E10 (Elza steht im Suedteil des Raums auf Johns Weg, (-7418,-14952), Blick Norden — IM Bild von Cut 7)**: Zustands-
  Log F2710 Elza (-7418,-14952) -> F2720 (-7580,-13699) -> F2730 (-8132,-12820) = 2,3 m in 20 Bildern; Bilder
  `e10_schub.png` F2669-F2750: John rennt im Bild auf Elza zu, durchdringt sie (F2720-F2729 beide Modelle
  ineinander), Elza rutscht in Standpose seitlich an die Wand (F2735-F2750), John verlaesst das Bild unten.
  **Im Bild sichtbarer Fehler** in genau der Szene, die der Nutzer-Nachtrag 2 meint. Realistisch: nach dem Kampf
  geht man zu John in die Ecke (-7550,-24100) — jede Stelle der Laufspur x ~ -7400, z -24100..-9613 liegt im Bild von
  Cut 7. -> M3.

### 2.6 Laeufe E8/E9 — Phase H verlassen/wiederbetreten, Gemaelde mit "No"
- E8: in Phase H durch die Doppeltuer nach 1131 (Slot 0 ist in Phase H normale Tuer) und zurueck: zweites
  `loaded room1141.rdt` -> wieder "Phase H: John im Dachgang hinter dem Rolladen (-7124,0,-6150)", Klopfen ab F1
  (F1/245/485/725), kein doppelter Faden. In Ordnung.
- E9: am Gemaelde "No" (RE15_PRESS=right@1110 waehrend der Frage, Bild e3_frage.png zeigt die Wahl ab F1092): (3,0x6A)
  bleibt 0, Klopfen/Hilferuf laufen weiter (F1154/1184/1214, Hilferuf F1304 ...), zweiter Druck aufs Gemaelde ->
  BEFREIUNG, Rolladen-Ton 0x3B ab F1861. In Ordnung.

### 2.7 Herkunft der Sprachdateien (selbst geprueft)
main42.wav (0,953 s) = synchro/STAGE1/room1171/main01.wav ab 1,950 s (mittlere Abweichung 0,17 LSB) = Johns "Help me
out." aus "We need to get this thing off of here. Help me out."; main40.wav (2,568 s) = room1141/main02.wav ab 1,240 s
(Abw. 0,02) = "Are you okay? What's happened?". Beide aus Nutzer-Aufnahmen geschnitten; der Hilferuf ist der ruhige
Bitt-Satz aus der Heliport-Szene, kein Schrei (Beobachtung B4, der Nutzer kann main42.wav ersetzen — Dossier §7).

## 3. Urteil je Nutzer-Punkt (gegen den Wortlaut)

| Punkt | Urteil | Beleg (eigene Messung) |
|---|---|---|
| 1 Hinweis „There's an empty key tag on his belt. / It reads "Dining Room".“; "Help" und Klopfen nur Ton, kein Text | **erfuellt** | E1b `[msg] room=1021 id=40`, Bilder F260-F450 + Zoom: Wortlaut exakt; E3: in Phase H keine Nachricht (erste = msg 13), Hilferuf als Sprachzeile main42 (Korrelation 0,76-0,86 im Mitschnitt), Klopfen als SE 0x3C; Riegel unit_r38_dining1141_texte (kein "Help" in den Texten) |
| 2 Ablauf Nachtrag 14 (1141 zu, Roys Schluessel an seiner Liegestelle in 1011, John weg beim Zurueckkommen, ohne "John, wait"/"sorry", direkt "are you okay? what's happened?", Dining-Room-Schluessel, Hinweis bei Roy, Klopfen/Help regelmaessig bis zum Schalter mit panischer Musik, Tuersperre "I cannot leave him alone with those monsters") | **teilweise** | erfuellt gemessen: E2 Tuer zu (msg 40, kein Raumwechsel); E1c Schluessel nach der ECHTEN Szene an (-2000,7450), Aufnahme "Dining Room Key"; E1b kein John-Spawn beim Zurueckkommen; E3 Nachrichten 13,40,3..11 ohne 0/1/2; Klopfen/Hilferuf alle 240 Bilder + MAIN16 bis zum Gemaelde, danach still; E9 "No" -> Schleife laeuft weiter; Sperre msg 41 an Doppel- UND Dachtuer, danach Tueren frei. NICHT erfuellt: **M1** (Hinweis im Besuch der Szene nicht erreichbar) |
| 3 ROOM1140 Rolltor ohne Ton beim Hochfahren | **erfuellt** | E3 (Elza 1141, echter Weg): 0x3B 12/10/11 bei F1148/1163/1358, im Mitschnitt Klick/Motor/Anschlag; integration_r38_dining1141_l (Leon sub03): 0x3B 12/10/11 + Bank geladen; E4a/E4v: Leons Gemaelde zeigt in v0.8.25 UND jetzt "The switch is not responding..." (sub01 @0x00C7A; FUN_8003f038 @0x8003f080 `jal 0x8003ee3c` mit a0=a1=1 je Spielbild) — kein Rueckschritt; der einzige Rolladen in 1140/1141 hat jetzt ueberall, wo er hochfaehrt, Ton. Wellen aus der ROOM1131-Bank (RE1.5-eigener Rolladen-Ton, sha1-belegt) |
| 4 Nachtrag 2 "keine plötzlich rein poppenden Charaktere" (1141, John) | **teilweise** | erfuellt: John ploppt nirgends herein — integration_r38_dining1141_s in MEINEM Lauf gruen (R0 4451 Bilder, R1 0 Verstoesse, erstes Pixel F1090 unter dem hochfahrenden Rolladen, R2/R3 0 Verstoesse), eigene Bilder E3 F1200-F1428 und Abgang F4581-F4653. NICHT erfuellt: **M3** — Johns Abgangslauf schiebt Elza; steht sie im Suedteil (E10), geschieht das sichtbar im Bild von Cut 7 (John durchdringt Elza, Elza rutscht in Standpose 2,3 m zur Wand); im Dachgang/an der Dachtuer (E5/E6/E7) steht sie nach der Szene an anderer Stelle (bis 3,5 m) |

## 4. Gates
- **RE-Gate: haelt** (Abschnitt 1: Cut_old/Cut_chg, Form `2a 00 3c 01`, Item-Satz 0x43, sltiu 0x22, sltiu 0x6, RotY-Vorschub,
  ROOM1141/1131/1021/1011-Bytes, RE2 ROOM1060-Schleife + KLOPF1060 bytegleich, Wellen-sha1 — alle selbst gelesen).
  Lagen/Plaetze/Takt sind als PORT-WAHL mit Form-Beleg gekennzeichnet (programme_bauen.py Kopf, re15_dining_r38.h).
  Keine Rate-Woerter im Diff (deferred/tunable/interim/for now/faithful/plausibel/TODO: 0 Treffer ausser "ROOMxxxx").
  Kleiner Zitierfehler M4 (Kommentar). `programme_bauen.py --pruefen`: gen-Datei = Werkzeug ("ok").
- **Vertrag: haelt.** Bank 9 nur 64/66/67 (+ k_port[] in test_r36_kartenbits.c); Nachrichten 1021/40, 1131/40-41,
  1141/40-41 (+ 42 nur Stimme) innerhalb der Zuteilung; Item 0x43 per eigenem Lauf von zensus.py: 0 Platzierungen,
  Satz wie alle Schluesselkarten; Pfade: keine release/, platform/android/, shared_assets/PSX/, keine Test-CMakeLists
  (Registrierung in tests/unit/probes/r38_dining1141.cmake); gemeinsame Dateien nur 1-6-Zeilen-Haken mit
  "Runde 38 Spur F"; neue Daten shared_assets/RE2/KLOPF1060.* (nicht PSX) + synchro/STAGE1/room1141/main40/42.wav.
  Angepasste Fremd-Riegel (probe_r26_inventar.c B3/B4c: +0x43; test_r30_tuer.c: +1 Schlossstelle Art M) erweitern die
  Soll-Mengen folgerichtig, schwaechen nichts ab.
- **Riegel: vorhanden und gruen** (107/107 Unit, 17/17 Integration, Abschnitt 0). Sie decken M1 und M3 NICHT ab:
  W/leiche1021 setzen die Nach-Szenen-Flags und springen in 1021 (Leichen-Zone dann schon da); D/S/phase_h lassen
  Elza waehrend Johns Abgang immer abseits bei (-5500,-19000) stehen.

## 5. Maengel (nummeriert, nachpruefbar)
- **M1 (Punkt 2) Hinweis bei Roy im Besuch der Szene nicht erreichbar.** Nach der ECHTEN 1021-Szene laesst sich Roys
  Leiche nicht untersuchen (Lauf E1a: Elza bei (-3803,-9601) Blick 2053, 6 Drucke F2029-F2347, keine Nachricht, Bilder
  `e1a_leiche.png`); erst nach Verlassen und Wiederbetreten erscheint msg 40 (E1b). Ursache: die Leichen-Zone Slot 6
  setzt nur main00 @0x01E24/@0x01E2C unter Ck(3,0x5F)==1 beim Raumaufbau; die Szene sub02 setzt (3,0x5F) @0x01FE4
  ohne Slot 6. Im natuerlichen Ablauf (Szene -> John will allein sein -> Elza geht) bekommt der Spieler den Hinweis,
  der ihn nach 1011 fuehren soll, nicht dort, wo er ihn sucht; er findet zuerst die verschlossene Tuer in 1131 ohne
  Fingerzeig. Abhilfe-Richtung (Bau-Agent entscheidet, mit Beleg): Slot 6 am Szenenende in der Form main00 @0x01E2C
  scharf machen (die tote Roy-Figur liegt bei (-4431,-10689), die Zone ist (-4850..-3850, -10100..-9100)); Riegel mit
  der echten Szene statt per Flag.
- **M2 (Punkt 2, Entscheidung Nutzer/Orchestrator) Tuertext widerspricht dem Bild.** ROOM1131 msg 40 "It's locked. /
  The door is marked “Dining Room”." — im Hintergrundbild haengt direkt ueber dieser Doppeltuer das Schild "Briefing
  Room" (E2 Bilder `e2_tuer_zu.png`, E3b). Der Text stammt aus der Orchestrator-Planung (RE2-Muster), nicht aus dem
  Nutzer-Wortlaut; vom Bau-Agenten selbst vermerkt (Dossier §6). Vor der Auslieferung entscheiden.
- **M3 (Punkt 4 / Abgang) Johns Abgangslauf schiebt die Spielfigur.** BEFREIUNG +0x0402..+0x0422 (Pad-Sperre, Cut 7,
  `40 00 05 01` RUN nach (-7350,-7600) und (-7350,-700)) beachtet Elzas Lage nicht. Gemessen: E10 Elza (-7418,-14952)
  -> (-8132,-12820) sichtbar in Cut 7 (Bilder `e10_schub.png` F2717-F2750: Durchdringung, Rutschen in Standpose);
  E5 (-7342,-4062) -> (-6488,-714) (3,5 m); E6 (-7263,-1030) -> (-6503,-718); E7 (-7332,-718) -> (-6493,-701) mit
  Cut 6 vorher/nachher (`e7_schub.png`). Kein Haenger gefunden (John erreicht jeweils (-7351,-975)). Abhilfe-Richtung:
  Abgang erst, wenn Elza ausserhalb der Laufspur steht, oder Spur/Kamera nach Elzas Lage waehlen; Riegel mit Elza an
  mehreren Stellen (mindestens die vier gemessenen).
- **M4 (Gate, Kommentar) Zitierfehler** platform/pc/src/dining_r38_pc.c Z.6: `lw a0,0x8` @0x80045138 — der Befehl steht
  @0x8004513c (@0x80045138 ist `nop`; Fall 2 @0x800450e4 springt `j 0x80045130`; re15_audio.h zitiert richtig
  @0x8004513c). Inhalt (Bank 2 = RDT+0x08 = snd0) stimmt.

Beobachtungen (kein Mangel gegen den Wortlaut, zur Kenntnis):
- B1: Der Dining Room Key liegt massstabsgetreu (RE2-Modell 152x76) und ist am Boden nur ein 1-3-Pixel-Punkt auf Roys
  Blutlache (E1c `e1c_zoom.png`); die Aufnahme funktioniert, das Item-Bild im Modal ist gross.
- B3: Vom Klopf-Dreier ist nur der erste Schlag (Satz 0x0A) laut (RMS x3,4-x8,7); Schlag 2/3 (Satz 0x0B) liegen mit
  RMS ~1000-2400 etwa auf Musikpegel (MAIN16 ~2000).
- B4: Der Hilferuf ist "Help me out." aus Johns ruhiger Heliport-Zeile (main01.wav ab 1,95 s), kein panischer Ruf;
  ersetzbar ueber synchro/STAGE1/room1141/main42.wav.
- Bestaetigt (Behauptungen des Bau-Agenten): Kamera verlaesst Cut 7 nach dem Abgang (E3 F4731 Cut_old -> Cut 0;
  E5/E6/E7 -> Cut 6), Hilferuf-Pegel, Phase H beim Wiederbetreten (E8), Leon-1140-Gemaelde = sub05 schon in v0.8.25,
  Dachtuer -> 1171 (John erscheint dort erst beim Wechsel auf die Heliport-Ebene: 1171 sub00 Switch var0A Fall 0/3 ->
  Gosub sub07 @0x016C8, Eintritt aus 1141 = Cut 9 — Original, Spur I).

## 6. Messordner / Werkzeuge (scratchpad `C:/Users/MJOEDI~1/AppData/Local/Temp/claude/c--workspace-git-reAi-v2/c41eae99-e724-4cb3-afb9-119709f20a9d/scratchpad/F38abn0/`, nicht committet)
`lauf.sh` (eigene exe-Kopie je Lauf, Arbeitsordner je Lauf), `lauf_v0825.sh` (Kopie des v0.8.25-Pakets), Wegplaene
`e*/wegplan.txt`, Logs `e*/debug.log`, `e*/state.txt`, `e*/wegplan_log.txt`, Mitschnitt `e3/cap.raw`, Auswertung
`ton.py` (Kreuzkorrelation/Huellkurve), `spur.py` (Lagen aus dem Zustands-Log), `bogen.py` (Bildboegen), Bildboegen
`e1a_leiche.png`, `e1b_hinweis.png`, `e1b_text_zoom.png`, `e1c_schluessel.png`, `e1c_zoom.png`, `e1c_modal.png`,
`e2_tuer_zu.png`, `e3b_schluessel_tuer.png`, `e3_phase_h.png`, `e3_rolladen.png`, `e3_gorillas.png`,
`e3_john_ecke.png`, `e3_sperre.png`, `e3_abgang.png`, `e3_frage.png`, `e5_schub.png`, `e7_schub.png`, `e10_schub.png`.
Die Einzelbilder (*.ppm, ~1,8 GB) und die v0.8.25-Kopie sind nach dem Erstellen der Bildboegen geloescht (Platte);
Logs, Wegplaene, Boegen und `e3/cap.raw` bleiben. Jeder Lauf ist mit seinem Wegplan + den Env-Zeilen oben reproduzierbar.


# Runde 36 — Spur A: klettern (BLOCKER)

Zweig `r36/klettern`, Basis master 5a9dbb7b (= v0.8.23 3ec5b1f8 + Runde-36-Auftrag), zuletzt auf master
78a4a748 nachgezogen (Merges 386d41fe, a234bbe5). Dossier wird fortlaufend geschrieben; die Sitzung vom
Vormittag endete am Limit nach Commit 406523e9, die Fortsetzung (2026-10-10 nachmittags) ergaenzt den
Zensus-Pin, die OFFEN-Kritik gegen den Wortlaut und die volle Suite.

## Auftrag (Wortlaut)

> Ganz wichtig - aus irgendeinen Grund kann ich nicht mehr auf die verschiebbaren Leitern/Tritte klettern -
> z.B. ROOM 10F0. Das macht ein Fortschritt im Spiel unmöglich

## Plan

1. REPRODUZIEREN am gebauten Stand (eigener Baum, ROOM10F0, Tritt verschieben + hochklettern).
2. BISECT v0.8.21 (9362dd7e) -> v0.8.22 (87cc8575) -> v0.8.23 (3ec5b1f8), dann die Runde-35-Commits.
3. ORIGINAL-Mechanismus (climb_common.c, FUN_... Adressen) gegen den Port stellen; Fix muss den Befund
   ERKLAEREN.
4. Pin (Mechanik: Tritt verschieben + hochklettern + oben ankommen, ueber den echten Weg) + Suite >= 592.

## Zensus ROOM10F0 (RDT selbst gelesen, tools/scd_dump_room.py)

- Der verschiebbare Tritt ist Obj_model_set **obj 0, Typ 4** in sub00:
  - @0x010F4 `2d 00 04 00 00 00 00 00 00 00 5e e8 00 00 56 13 00 00 00 04 00 00 32 00 7c fc 00 00 a3 02 84 03 58 02`
    = Startlage (-6050,0,4950), Band 0, Box c(50,-900,0) h(675,900,600), wenn Flag (3,102)==0 (Ck @0x010DC);
    dazu Aot_set Slot 10 sce 5 (Schiebezone) @0x010E0.
  - @0x0111A dieselben Bytes mit pos (-4850,0,-850) = Endlage, wenn (3,102)==1 (Else_ck @0x01116).
  - sub01 @0x01140: Work_set(3,0) + Member_cmp(15 == 10) -> Set (3,102)=1 (der Tritt ist angekommen).
  - Door_aot_set Slot 1 @0x00F52 -> ROOM1090, **floor 1**, Rechteck (-5350,-2250,1200,1000) = noerdlich der
    Endlage. Oberkante des Tritts = obj.y - 2*box_hy = -1800 = Band 1 -> man klettert auf den Tritt und
    erreicht von dort die Tuer nach ROOM1090. Ohne das Klettern ist ROOM1090 von hier nicht erreichbar.
- **Flags-Wort des Tritts: pc[6..7] = `00 00` -> `0x0000 | 1 = 0x0001`** (LAB_80040914 @0x80040990-a4
  `lhu v0,6(a2); ori v0,v0,0x1; sw v0,0(a1)`). Die Kisten in ROOM1090 tragen dagegen pc[6..7] = `00 01`
  -> 0x0101 (ROOM1090 @0x0216E/@0x02190).
- Zensus ueber alle 240 RDTs (scratch zensus_obj.py, Ausgabe im Dossier-Anhang unten): Typ-4-Objekte OHNE
  Bit 0x100 in ROOM10F0/10F1, 11A0/11A1, 2040/2041 (obj 1/2), 2060/2061 (obj 2), 20A0/20A1, 3091 (obj 1,
  0x0081), 5020/5021. Alle uebrigen Typ-4-Objekte (1080, 1090, 10D0, 1100, 11E0, 11F0, 1230, 2040 obj 0,
  2050, 2060 obj 3/4, 3050, 3090, 30E0, 4020, 5050, 50D0) tragen 0x0101. Typ-0-Objekte tragen ebenfalls
  Flag-Bits (10F0 obj 1..3: 0x000B; 11E0 obj 0: 0x03FD) — das Wort ist ein allgemeines Attributwort.

## Messung vorher (gebauter Stand master 5a9dbb7b, exe-Kopie re15_pc_r36a.exe)

Lauf K1: `RE15_NO_INTRO=1 RE15_NOAUDIO=1 RE15_TITLE_SHOT=title.bmp RE15_TITLE_SHOT_AF=2 RE15_WINDOW_SCALE=1
RE15_DEBUG_JUMP=10f0@250 RE15_SET_FLAG=3:102,9:71 RE15_PLAYER_POS=-4800,400,1024
RE15_INPUT_SCRIPT_BASIS=spiel RE15_INPUT_SCRIPT_START=1 RE15_INPUT_SCRIPT=W13,A0.1,W3 RE15_CLIMB_TRACE=climb.txt
RE15_EXIT_AT=560#10f0` (Tritt in der Endlage, Leon suedlich davor, Blick -Z auf den Tritt, Aktionstaste F391):
```
T642 room=10F0 pm=0 pad=8000 pos=(-4800,0,400) rot=1024 band=0 sub=0 kind=0x00 ph=0 obj=-1 stand=-1 ...
T643 room=10F0 pm=0 pad=8000 ... sub=0
T644 room=10F0 pm=0 pad=8000 ... sub=0
T645 room=10F0 pm=0 pad=0000 ... sub=0     (kein Substate 9 in 771 Zeilen: grep -c "sub=9" = 0)
```
-> **REPRODUZIERT: die Aktionstaste vor dem Tritt startet kein Klettern.**

Sonde `probe_r36_klettern_10f0 diag` (echte VM, ROOM10F0.RDT, beide Lagen, alle vier Anlaufseiten):
```
prop[3] id=0 type=4 active=1 flags=0x0001 band=0 pos=(-4850,0,-850)
[f1 yaw1024] spieler=(-4800,0,200) rot=1024 band=0 sonde=(-4800,-600) s6(typeword)=0x0000
             obj3 flags=0x0001 band=0 ... reach=1 boxhit(-112)=1 stand=-1
    try_start=0 sub=0 kind=0x00
```
(identisch fuer yaw 0/2048/3072 und fuer die Startlage): Reichweite (FUN_8002d1e8) = 1, Box-Treffer
(FUN_8002da4c, -112) = 1, Band gleich — **einzig das Flags-Gate der Port-Sonde faellt durch**, und das
SCA-Attributwort am Sondenpunkt ist 0 (kein `s6 & 1`-Weg, kein `s6 & 2`-Weg).

## Bisect-Tabelle

| Stand | Commit | Lauf K1 (Endlage, Aktionstaste vor dem Tritt) |
|---|---|---|
| v0.8.21 | 9362dd7e (build_bisect) | sub bleibt 0, kein Klettern (`grep -c "sub=9" = 0`) |
| v0.8.23 / master | 3ec5b1f8 / 5a9dbb7b | sub bleibt 0, kein Klettern |

`git log 9362dd7e..3ec5b1f8 -- engine/src/climb_common.c engine/src/re15_collision.c` ist LEER; das
Flags-Gate stammt aus b86957e3 (2026-08-21, "Klettern auf die Kiste (ROOM1090)"), die Kletterlogik ist
seit 92c17762 (2026-08-23) unveraendert. **Es ist KEINE Regression der Runde 35**: der Port konnte in
ROOM10F0 seit der Einfuehrung des byte-true Kletterns nie auf diesen Tritt klettern, weil die Sonde das
Flags-Wort falsch liest (unten). Der Nutzer-Befund "nicht mehr" erklaert sich daraus, dass die Runde 35
(Spur K) die Szene in ROOM10F0 einbaute und der Raum seither auf dem Spielweg liegt (Parkplatz-Schluessel
etc.) — vorher wurde der Tritt in 10F0 im Port nicht gebraucht/gespielt (Runde 22 mass 10F0 nur als PRI-
Standplatz-Zensus, Memory reai-v2-schiene-einseitig-und-synthetisch).

## RE-Belege

### FUN_8002d2c0 @0x8002d2c0 — "steht ein kletterbares Objekt an der Sonde?" (PSX.EXE, selbst disassembliert)
```
8002d344: lui v0,0x800b
8002d348: addiu v0,v0,16280            ; 0x800b3f98 = Objekt-Pool, Stride 148
8002d34c: addu s0,s2,v0
8002d350: lw v0,0(s0)                  ; Flags-Wort des Objekts (pool+0x00)
8002d358: andi v0,v0,0x101             ; Bytes 01 01 42 30
8002d35c: beq v0,zero,0x8002d3a4       ; NUR wenn (flags & 0x101) == 0 wird das Objekt uebersprungen
8002d364: lbu v1,0(s3)                 ; DAT_800acad6 Spieler-Band
8002d368: lbu v0,130(s0)               ; obj+0x82 Objekt-Band
8002d370: bne v1,v0,0x8002d3a4
8002d37c: jal 0x8002d1e8               ; Reichweite (563 voraus, Radius 900)
8002d394: jal 0x8002da4c               ; Box-Treffer (Rand -112)
8002d39c: bne v0,zero,0x8002d304       ; -> DAT_800ac78c = obj, Rueckgabe 1
```
Weil LAB_80040914 @0x80040990-a4 bei JEDER Installation Bit 0 setzt (`ori v0,v0,0x1`), ist
`(flags & 0x101) != 0` fuer JEDES aktive Objekt wahr — **das Gate prueft nur "aktiv", Bit 0x100 ist fuer
den Kletterstart ohne Bedeutung.** Der Port (climb_common.c:86) verlangte `(flags & 0x101) == 0x101`,
also ZUSAETZLICH Bit 0x100 — ein Lesefehler der Instruktion `beq v0,zero` (Rueckgabe 0 nur bei 0).
Folge: alle Typ-4-Objekte mit pc[7] = 0 (Zensus oben: 10F0, 11A0, 2040, 2060, 20A0, 3091, 5020 und die
Elza-Spiegel) waren im Port nicht kletterbar. (3091 obj 1 ist kein Tritt, sondern eine Null-Box auf Band 1;
FUN_8002da4c trifft sie nur aus schraeger Lage — Nachbesserung 1.)

### Was Bit 0x100 wirklich ist — FUN_8002dc48 @0x8002dc48 (Aufrufer FUN_80031c44 @0x80031d38, Spieler-Dispatcher)
```
8002dd80: lw v1,0(s0)                  ; Flags des Objekts
8002dd88: andi v0,v1,0x1 / beq -> weiter   ; aktiv
8002dd94: andi v0,v1,0x100             ; Bytes 00 01 62 30
8002dd98: beq v0,zero,LAB_8002ddc0     ; ohne 0x100: Objekt wird NICHT betrachtet
8002dda4: lbu a3,0x82(s2) ; li a2,-0x104 ; addiu a3,a3,-1
8002ddb0: jal FUN_8002da4c             ; Spieler-Vorpunkt in der Box des Objekts im Band darunter?
8002ddb8: bne v0,zero,LAB_8002dd28 ; _ori v0,zero,0xd -> sb v0,0x5(s2) = Spieler-Substate 0xd
```
Bit 0x100 gehoert zur Absturz-/Herunterfall-Pruefung des Spielers (Substate 0xd, FUN_8002dc48; im Port
nicht portiert, game_step_common.c:1369 nennt sie nur) — nicht zum Kletterstart.

### FUN_8002d474 @0x8002d474 — Aufwaerts-Zweig (zur Bestaetigung, dass der Port sonst stimmt)
```
8002d524: jal 0x8003b7f0   ; s6 = SCA-Attributwort am Sondenpunkt (800 voraus)
8002d530: jal 0x8002d2c0   ; s5 = Objekt an der Sonde
8002d53c: bne s5,zero,0x8002d550
8002d544: andi v0,s0,0x1 / 8002d548: beq v0,zero,0x8002d634   ; sonst nur mit s6&1
8002d5bc/8002d5dc: jal 0x8002da4c (Rand 200, Band+1 / Band-2) -> 0 = blockiert
8002d604: sb 9 -> 0x800aca59 (Substate 9)
8002d610: beq s5,zero,0x8002d61c / 8002d614: ori v0,zero,0x81 / 8002d618: sb v0 -> 0x800aca5a (KIND 0x81)
```
Deckungsgleich mit climb_common.c:238-255. In ROOM10F0 ist s6 = 0x0000 an allen vier Anlaufseiten (Sonde
oben), der Tritt ist also im Original NUR ueber s5 (FUN_8002d2c0) kletterbar — und dort faellt er mit Flags
0x0001 durch das `andi 0x101 / beq zero`-Gate NICHT durch.

## Umsetzung (Commit 1c926683)

| Datei | Aenderung |
|---|---|
| `engine/src/climb_common.c:86` | `if ((flags & 0x101u) == 0u) continue;` statt `!= 0x101u` — byte-true zu @0x8002d358 `andi v0,v0,0x101` + @0x8002d35c `beq v0,zero`. Kopfkommentar der Sonde mit dem Beleg. Dazu (Diagnose, env-gegatet) schreibt `RE15_CLIMB_TRACE` jetzt `t4=<idx> t4pos=(x,z) t4fl=0x..` des ersten Typ-4-Objekts mit, damit "Tritt verschieben" im Log messbar ist. |
| `engine/src/scd_vm.c:4252`, `include/re15_scd.h` (Flags-Kommentar) | Falsche Aussage "Bit 0x100 = kletterbar" berichtigt (0x100 = FUN_8002dc48 @0x8002dd94, Herunterfall-Pruefung). |
| `tests/unit/test_1090_climb_pin.c` | N4 prueft jetzt Flags 0 (= AUS, @0x8004098c `sw zero`); NEU N4b: Flags 0x0001 (ohne 0x100) -> Substate 9, KIND 0x81. T1-Kommentar berichtigt. |
| `platform/pc/main.c:3703` | Messhaken (2 Zeilen): `RE15_TITLE_SHOT_CHAR=1` startet den Titel-Kurzweg als Elza (derselbe Aufruf `re15_gameflow_new_game(ch)` wie das Player-Select-Ende @main.c:3622). |
| `tests/unit/probe_r36_klettern_10f0.c`, `tests/unit/probes/r36_klettern.cmake`, `tests/integration/test_r36_klettern.cmake` | Sonde + Riegel (unten). |
| `tests/unit/probe_r36_klettern_zensus.c` (Fortsetzung, Commit fd9c0f48) | Zensus-Sonde/-Pin ueber alle Typ-4-Objekte ohne 0x100 (Abschnitt "Zensus-Pin"). |

Keine neuen Assets, keine Patches an `shared_assets/PSX/*`, keine Nachrichten-IDs/Flags/Ereignisse belegt.
Die Engine-Aenderung ist genau EINE Zeile (climb_common.c:99); alles Weitere sind Kommentare, Messhaken
(env-gegatet) und Tests.

## Messung nachher (exe-Kopie re15_pc_r36a.exe aus dem eigenen Baum, RE15_CLIMB_TRACE)

K1 (Endlage, wie "Messung vorher") — `A_belege/k1_master_nach_fix_climb_trace_kompakt.txt`:
```
T642 room=10F0 pad=8000 pos=(-4800,0,400) rot=1024 band=0 sub=9 kind=0x81 ph=0 obj=3 stand=-1
T643 ... sub=9 kind=0x81 ph=2 ... mo=230 (W01 Clip 5)      T644 ... ph=3 mo=231 (PL00 Clip 2)
T673 room=10F0 ... pos=(-4800,-1800,-748) band=1 sub=9 ph=3 stand=3      (Ausloesebild 0x1d: Y -= 1800, Band +1)
T694 room=10F0 ... pos=(-4800,-1800,-748) band=1 sub=0 obj=3 stand=3     (Substate 0, steht AUF dem Tritt)
```

ECHTER WEG — drei Laeufe in je EINEM Stueck (Eingabeskript auf der Spielbild-Achse; Kompakt-Traces in `A_belege/`):

**Lauf A (Leon, ROOM10F0 per Sprung, Szene (9,71) gesehen, Leon westlich des Tritts):**
`U2.6` schiebt den Tritt von x -6050 auf -4914 (Substate 8: `push=5` = PL00 Clip 0x12, Wurzel-Schub
@0x80035a5c/FUN_800369f8), aussen herum (`L0.37,U0.5,R0.37,U0.55,R0.37`: Leon auf (-4762,6001) Gierung 1056),
`U10.5` schiebt ihn von z 4950 auf -1108 (6058 Einheiten; Stopp an der Wand = FUN_8003b558-Klasse 2),
`A` -> `T1118 sub=9 kind=0x81 obj=3`, `T1170 pos=(-5028,-1800,-1206) band=1 stand=3 sub=0` (oben),
`U0.6` nach -Z auf dem Tritt (Eingrenzung FUN_8002cfd4) auf z -1332, `A` -> `[tuer] Sequenz Archiv 2 DOOR1E`,
`T1224 room=1090 pos=(-13600,-9000,1300) band=5` = **ROOM1090 erreicht**.

**Lauf B (Elza, ROOM10F1 ueber RE15_GOTO_ROOM = Tuer-Spawn (8400,0,-350) Gierung 2048 aus ROOM10D1):**
Weg durch den Raum (Ostflur -X bis x 5368 = Wand, +Z bis z 9202, -X bis x 400, -Z bis z 1441, -X bis
x -7332, +Z bis z 5023), dann wie A: Tritt x -6050 -> -4892, z 4950 -> -1076; `T1137 sub=9 kind=0x81`,
`T1189 pos=(-5015,-1800,-1174) band=1 stand=3`, Tuer `T1243 room=1091` = **ROOM1091 erreicht** (PL04 geladen:
"[pl] Spieler-Familie PL04 (character=4, Elza-Bit=1)").

**Lauf C (Leon, ERSTES Betreten von ROOM10F0, Runde-35-Szene laeuft):** "[cut10f0] ROOM10F0: Szene gestartet"
(Bild 250) ... "Szene zu Ende: Kartenhinweis ... angefordert" (Bild ~2994; Leon auf (4792,0,11354) Gierung
1415, pm=0 ab Bild ~3007; Kartenhinweis-Schirm danach). Skript ab Bild 3400: -Z bis z 9154, -X bis x 193,
-Z bis z 1007, -X bis x -7332, +Z bis z 5022, dann wie A: Tritt -6050 -> -4970 / 4950 -> -1108,
`T4286 sub=9 kind=0x81`, `T4338 pos=(-5083,-1800,-1206) band=1 stand=3`, `T4392 room=1090` = **ROOM1090
erreicht**. Die Szene hinterlaesst keinen Zustand, der die Sonde sperrt.

Nebenbefund (nicht Teil des Auftrags, nur Beobachtung): das Debug-Menue sprang mit Elza nach "ROOM10F1" in
den Leon-Raum 10F0 (Trace `room=10F0`, Tuer nach 1090) — das ist der Punkt der Spur I (VERTRAG §0 I);
deshalb laeuft Lauf B ueber RE15_GOTO_ROOM, das den Raum mit Elza-Bit korrekt laedt.

## Tests (alle gruen, eigene probes-Datei `tests/unit/probes/r36_klettern.cmake`)

| Test | misst |
|---|---|
| `unit_r36_klettern_10f0_endlage` | ROOM10F0.RDT in der VM, Flag (3,102)=1: Tritt obj 0 Typ 4 Flags 0x0001 Band 0 bei (-4850,-850); Leon suedlich davor, Blick -Z: Reichweite FUN_8002d1e8 = 1, `try_start` = 1, Substate 9, KIND 0x81, Objekt = Tritt; Aufstieg endet mit y = obj.y - 2*box_hy = -1800, Band 1, Standobjekt = Tritt |
| `unit_r36_klettern_10f0_startlage` | dasselbe in der Startlage (-6050,4950), Flag (3,102)=0 |
| `unit_1090_climb_pin` (bestehend, berichtigt) | N4: Flags 0 -> kein Klettern; N4b: Flags 0x0001 -> Klettern (Substate 9, KIND 0x81) |
| `integration_r36_klettern` | die drei Laeufe A/B/C oben mit der echten exe; je Lauf: Tritt in der Startlage gesehen, Schieben (`push=5`), `sub=9 kind=0x81`, oben (y=-1800, band=1, stand=Tritt) mit Tritt in der Schiebezone (x > -5800, z < 250 = Aot_set Slot 10 @0x010E0), Zielraum 1090/1091 per EXIT_AT erreicht; C zusaetzlich Szene gestartet + zu Ende; B zusaetzlich `[pl] Spieler-Familie PL04` (Elza wirklich im Spiel). Laufzeit ~540 s (TIMEOUT 1200) |
| `unit_r36_klettern_zensus` (NEU, Fortsetzung) | ALLE Typ-4-Objekte ohne Bit 0x100 des Zensus in der VM (Abschnitt "Zensus-Pin" unten): 12 Raeume (Leon + Elza), ~~je Lage (Flag 0/1)~~ (⛔ Nachbesserung 2: seither je RECORD, 24 Records mit Lage-Pruefung gegen die RDT-Bytes; 11A0/11A1 @0x0107E ERZWUNGEN + Gegenprobe), je Objekt vier Anlaufseiten; Riegel = je Objekt mindestens eine Seite mit Substate 9, KIND 0x81, Objekt = Tritt, Aufstieg endet y = obj.y - 2*box_hy, Band + 1, Standobjekt = Tritt. **Nachbesserung 1:** dazu Null-Box ROOM3091 obj 1 — Wahrheitstafel FUN_8002da4c (h 0, Rand -112, 12 Punkte, @0x8002da8c-c0), achsgerade 4/4 ohne Objekt-Treffer, Querversatz +-300 8/8 Start auf Band 1, derselbe Schraeganlauf vom Boden (Band 0) 8/8 ohne Objekt-Treffer (@0x8002d370) |

Vorher (ohne Fix) fallen `unit_r36_klettern_10f0_*` an P2 (`try_start` = 0), `unit_r36_klettern_zensus`
mit 0/24 (gemessen, unten; ⛔ alte Zaehlung mit 11A0/11A1 doppelt — die Sonde von Nachbesserung 2 gegen das
alte Gate: `0/24 Records ... beim Laden erreichbar 0/22, nur ERZWUNGEN 0/2`, FAIL 25, exit 1,
`A_belege/nb2_zensus_altes_gate.txt`) und `integration_r36_klettern` an "KEIN Klettern" (gemessen: K1 auf
master UND v0.8.21).

## Zensus-Pin — der Wortlaut ist PLURAL ("Leitern/Tritte", 10F0 nur "z.B.") (Fortsetzung 2026-10-10)

Die uebrigen Typ-4-Objekte ohne Bit 0x100 (Zensus-Anhang) sind dieselbe Objektklasse wie der 10F0-Tritt
und fielen am selben Gate durch. Lage-Flags und Schiebezonen (aus `tools/scd_dump_room.py`, RDT selbst
gelesen):

| Raum | Objekt | Modell/Box | Lage-Flag (Ifel_ck/Ck/Else_ck) | Schiebezone (Aot sce 5) |
|---|---|---|---|---|
| 10F0/10F1 | obj 0 | Tritt h(675,900,600) | (3,102): 0 -> (-6050,0,4950) @0x010F4; 1 -> (-4850,0,-850) @0x0111A | Slot 10 @0x010E0 (-5800,-1800,2050,2050) |
| 11A0/11A1 | obj 0 | Kiste h(900,900,900) | (5,6): 0 -> Band 3 (10050,-5400,8800) @0x01058; 1 -> Band 0 (-19300,0,3300) @0x0107E. ⛔ BERICHTIGT (Nachbesserung 2): (5,6) liegt in Bank 5 Wort 0, das der Raum-Init vor main00 nullt (@0x8003ed74) — beim Laden steht die Kiste IMMER auf Band 3; @0x0107E wird im Auslieferungsstand nie installiert (auch nicht zur Laufzeit) und war bis Nachbesserung 1 auch in der Sonde NICHT gefahren; seit Nachbesserung 2 ERZWUNGEN gefahren und getrennt gezaehlt | Slot 6 @0x01044 (-20150,2350,2050,2050), nur unter (5,6) == 0 installiert |
| 2040/2041 | obj 1, obj 2 | Tritt h(675,900,600) | unbedingt, Band 3: (-16575,-5400,1700) @0x0137A, (22925,-5400,-5400) @0x0139C | keine (statisch; Slot 9 @0x0146A gehoert zu obj 0 = 0x0101-Kiste) |
| 2060/2061 | obj 2 | Tritt h(675,900,600) | (3,172): 0 -> (-3125,0,2075) @0x00EA6; 1 -> (475,0,-7375) @0x00ECC | Slot 2 @0x00E92 (-550,-8300,2050,2050) |
| 20A0/20A1 | obj 0 | Kiste h(900,900,900) | (3,204): 0 -> Band 4 (12250,-7200,-12400) @0x01854; 1 -> Band 0 (23150,0,-24400) @0x0187A | Slots 4/7/9 @0x017FC/@0x01840/@0x018B2 |
| 5020/5021 | obj 0 | Kiste h(900,900,900) | (4,203): 0 -> Band 4 (-11000,-7200,-23150) @0x01242; 1 -> Band 0 (-3400,0,-25300) @0x01268 | Slot 4 @0x0122E (-4450,-26250,2050,2050) |
| 3091 | obj 1 | Flags 0x0081, Band 1, Box c(0,-1800,0) **h(0,0,0)** @0x01508 | sub00 unter (4,16)==0 (Ifel_ck @0x0148E) | — (kein Tritt, kein Riegel "oben". ⛔ BERICHTIGT in Nachbesserung 1: hier stand "ohne Box trifft FUN_8002da4c mit Rand -112 nie" — FALSCH; der vorzeichenlose Vergleich @0x8002da8c-c0 trifft bei h=0 / Rand -112 genau bei \|dx\| >= 112 UND \|dz\| >= 112, ein Schraeganlauf startet den Aufstieg) |

Sonde `tests/unit/probe_r36_klettern_zensus.c` (echte VM, `scd_room_reenter` je Raum/Lage; Spieler auf der
Ebene des Objekts, Boxkante + 450, Blick auf das Objekt; Aktionstaste; dann `re15_climb_tick` bis Substate 0):

- **Vorher** (Gate `!= 0x101` kurz wiederhergestellt, gebaut, gemessen — `A_belege/zensus_sonde_vorher_kompakt.txt`):
  `Zensus: 0/24 Objekte (Raum x Lage) kletterbar`, alle 100 Seiten "kein Start" (`try=0`; 24 x 4 Riegel-
  Seiten + 4 Seiten 3091), obwohl `reach=1`.
- (⛔ Zaehlung "x/24 Objekte (Raum x Lage)" in beiden Zeilen hier = ALTE Sonde: 22 verschiedene Records, 11A0/11A1
  "Lage 1" wiederholte Lage 0 — Nachbesserung 2.)
- **Nachher** (Commit 1c926683, `A_belege/zensus_sonde_nachher.txt`): `Zensus: 24/24 Objekte (Raum x Lage)
  kletterbar`, alle 96 Seiten (24 Objekte x 4) `try=1 sub=9 kind=0x81`, Aufstieg endet nach 51 Bildern (wie
  K1: T642 -> T694) auf y = obj.y - 2*box_hy (-1800 / -7200 / -9000), Band + 1, Standobjekt = Objekt, z.B.
  ```
  ROOM10F0 flag=1 obj=0(prop 3) fl=0x0001 band=0 pos=(-4850,0,-850) yaw=1024 start=(-4800,200) reach=1 try=1 sub=9 kind=0x81 obj=3 ende=51 oben=(-4800,-1800,-948)(soll y -1800) band=1(soll 1) stand=3 -> OBEN
  ROOM2060 flag=1 obj=2(prop 2) fl=0x0001 band=0 pos=(475,0,-7375) yaw=1024 start=(525,-6325) reach=1 try=1 sub=9 kind=0x81 obj=2 ende=51 oben=(525,-1800,-7473)(soll y -1800) band=1(soll 1) stand=2 -> OBEN
  ROOM5020 flag=0 obj=0(prop 0) fl=0x0001 band=4 pos=(-11000,-7200,-23150) yaw=0 start=(-12350,-23150) reach=1 try=1 sub=9 kind=0x81 obj=0 ende=51 oben=(-11202,-9000,-23150)(soll y -9000) band=5(soll 5) stand=0 -> OBEN
  ```
  ROOM3091 obj 1: `reach=1 try=0` auf allen vier Seiten (Null-Box) — unveraendert, kein Riegel.
  ⛔ BERICHTIGT (Nachbesserung 1): die vier Seiten sind ACHSGERADE (Querabstand 0 -> |dx| bzw. |dz| = 0
  < 112), deshalb "kein Start". Mit Querversatz >= 112 startet der Aufstieg (gemessen 8/8, Abschnitt
  "Nachbesserung 1"). Die Commit-Message von fd9c0f48 ("kein Start, wie FUN_8002da4c mit Rand -112")
  ist damit ueberholt.

~~Damit ist jedes Objekt der betroffenen Klasse in beiden Szenarien und beiden Lagen gemessen, nicht nur
"durch dieselbe Zeile abgedeckt".~~ ⛔ BERICHTIGT (Nachbesserung 2, Abnahme 1 Mangel 1): FALSCH fuer
11A0/11A1 — die Sonde setzte (5,6) vor dem Raum-Init, der Bank 5 Wort 0 nullt; die beiden "Lage 1"-Zeilen fuhren
die Lage 0 ein zweites Mal. "24/24 Objekte (Raum x Lage)" waren 22 verschiedene Records plus 2 Wiederholungen.
Stand nach Nachbesserung 2 (Abschnitt dort): 24 Records, jeder per Lage-Pruefung gegen seine RDT-Bytes belegt;
22 beim Laden erreichbar, 2 (11A0/11A1 @0x0107E) nur erzwungen — im Original nie installiert.

## Suite (bash re15_port/tools/local_build.sh all, Baum r36_klettern)

**Lauf 1** (Stand c83d411b, ~14:30-16:00, Host unter Last paralleler Runde-36-Agenten; `A_belege/suite_lauf1_kompakt.txt`):
```
99% tests passed, 1 tests failed out of 596
Total Test time (real) = 5492.33 sec
502/596 Test #502: integration_r35_cut1150 ..................***Failed  769.59 sec
572/596 Test #572: unit_r36_klettern_10f0_endlage ...........   Passed    1.13 sec
573/596 Test #573: unit_r36_klettern_10f0_startlage .........   Passed    0.06 sec
574/596 Test #574: unit_r36_klettern_zensus .................   Passed    0.33 sec
575/596 Test #575: integration_r36_klettern .................   Passed  534.27 sec
```
Der einzige Fehlschlag ist ein FREMDER Test (Spur L, Runde 35) und extern verursacht: die exe von Lauf b
blieb bei Bild 370 (state.log) in einer SDL2-Assertion haengen — `Assertion failure at WIN_AddDisplay
(.../_deps/sdl2-src/src/video/windows/SDL_windowsmodes.c:380) 'index == *display_index'` (debug.log, letzte
Zeile), d.h. die Anzeige-Konfiguration des Hosts aenderte sich waehrend des Laufs; die Meldung "Endbild
nicht erreicht - Lauf vorher abgerissen" ist die Folge des 600-s-Timeouts. ROOM1130/1150 enthalten kein
Typ-4-Objekt, die Spur-A-Aenderung (ein Gate in der Kletter-Sonde) wird dort nie erreicht.

**Einzel-Nachlauf** (`ctest --test-dir re15_port/build -R "^integration_r35_cut1150$"`, direkt im Anschluss):
```
1/1 Test #502: integration_r35_cut1150 ..........   Passed  546.82 sec
100% tests passed, 0 tests failed out of 1
```

Die drei Laeufe von `integration_r36_klettern` aus diesem Suite-Lauf (`A_belege/suite_lauf1_integration_r36_klettern_kompakt.txt`):
```
A_leon_10f0  : T252 t4pos=(-6050,4950) | T572 push=5 | T1121 sub=9 kind=0x81 obj=3 | T1173 pos=(-4899,-1800,-1206) band=1 stand=3 t4pos=(-4785,-1108) | EXIT_AT: Bild 1400 in Raum 1090 | PL00
B_elza_10f1  : T32  t4pos=(-6050,4950) | T590 push=5 | T1137 sub=9 kind=0x81 obj=3 | T1189 pos=(-5015,-1800,-1174) band=1 stand=3 t4pos=(-4892,-1076) | EXIT_AT: Bild 1320 in Raum 1091 | PL04 (character=4, Elza-Bit=1)
C_leon_szene : T252 t4pos=(-6050,4950) | T3771 push=5 | T4316 sub=9 kind=0x81 obj=3 | T4368 pos=(-5083,-1800,-1206) band=1 stand=3 t4pos=(-4970,-1108) | EXIT_AT: Bild 4550 in Raum 1090 | PL00
```

## OFFEN (kritisch gegen den Wortlaut geprueft, Fortsetzung 2026-10-10)

- **Erledigt:** "uebrige Typ-4-Raeume nicht einzeln gefahren" — jetzt je Raum/Szenario/Lage/Objekt in der
  VM gemessen (Zensus-Pin; ⛔ Nachbesserung 2: 24 Records mit Lage-Pruefung, davon 22 beim Laden erreichbar,
  11A0/11A1 @0x0107E nur erzwungen; die frueheren Zahlen "24/24 / vorher 0/24" zaehlten 11A0/11A1 doppelt). Der exe-Lauf ueber den echten Weg (Schieben + Klettern +
  oben + Tuer) liegt fuer 10F0/10F1 vor (Laeufe A/B/C); fuer die anderen Raeume ist nur der Kletterstart
  betroffen (gleiches Gate, gleiche Objektklasse), das Schieben war nie gestoert (Substate 8 kennt das
  Flags-Wort nicht).
- **Ausserhalb des Wortlauts (bleibt offen, kein Blocker):** FUN_8002dc48 @0x8002dc48 (Spieler-Dispatcher
  @0x80031d38), die Herunterfall-Pruefung (Substate 0xd) beim Gehen ueber die Kante eines Objekts, ist im
  Port nicht portiert (game_step_common.c:1369 nennt sie nur). Sie betrachtet NUR Objekte mit Bit 0x100
  (@0x8002dd94 `andi v0,v1,0x100` / @0x8002dd98 `beq v0,zero,LAB_8002ddc0` — Bytes `00 01 62 30` /
  `09 00 40 10`, ghidra1_V2.txt:121133-34). Die vom Nutzer gemeinten verschiebbaren Leitern/Tritte tragen
  dieses Bit NICHT (Zensus) — fuer sie faellt der Spieler auch im Original nie von der Kante, und das
  Hinaufklettern ("auf ... klettern") ist davon unabhaengig. Betroffen waeren nur die 0x0101-Kisten (z.B.
  ROOM1090): dort steht der Spieler im Port an der Kante, statt zu fallen. Messweg bei Bedarf: auf der
  1090-Kiste zur Kante gehen, Original setzt +0x05 = 0xd (DuckStation-GDB auf 0x800aca59).
- **H1 aus Abnahme 0 (§7/§10) — ROOM2060, Tritt in der Startlage (Nachbesserung 1 hier eingetragen):** die
  adulte Spinne (Typ 0x25, Slot 2) schiebt Leon im ersten Normalbild nach dem Aufstieg von der Trittflaeche;
  Leon bleibt auf y = -1800 / Band 1 ohne Standobjekt ("schwebt"). Reihenfolge im Original selbst
  disassembliert: FUN_80031c44 ruft den Koerper-Schub `jal 0x8002b544` @0x80031cbc VOR dem Standtest
  `jal 0x8002d100` @0x80031ce8 (a1 = 0x12); eine Bodenhoehen-Nachfuehrung fuer einen von einem
  Nicht-0x100-Objekt geschobenen Spieler gibt es danach nicht (FUN_8002dc48 @0x8002dd94 betrachtet nur
  0x100-Objekte). Der Port bildet diese Reihenfolge nach; NEU erreichbar ist der Zustand, weil der Tritt
  (Flags 0x0001) seit dem Fix kletterbar ist. Offen ist allein, ob die Spinne Leon auf dem Tritt im Original
  ueberhaupt erreicht (Y-Band-Gate @0x8002b1f0-b228, Port enemy_ai_common.c:4435-4439; Spinnen-Y im Original
  ungemessen). **Messweg:** DuckStation mit der Auslieferungs-EXE (NICHT die PATCHED-EXE-Savestates), ROOM2060
  per Debug-Menue (Skill re15-room-capture), Tritt in der Startlage (-3125,0,2075) von Sueden besteigen,
  danach Spieler-Y 0x800aca8c (= 0x800aca54 + 0x38, Schreiber @0x800381ec), Spieler-Band 0x800acad6,
  Standobjekt 0x800ac788 und die Y der Spinne (Aktorfeld DAT_800acc2c, Stride 0x1f4, +0x38) Bild fuer Bild
  lesen (GDB-Haltepunkt auf @0x80031ce8). Bleibt Leon im Original oben (Spinne erreicht ihn nicht), ist das
  Y-Gate des Port-Schubs zu pruefen; faellt/schwebt er auch dort, ist der Port byte-true. Kein Mangel der
  Spur (Original-Reihenfolge, keine Abhaengigkeit von der geaenderten Gate-Zeile).
- **H3 aus Abnahme 0 (§5) — Nebenwirkungen des breiteren Gates:** 619 von 672 Records passieren das Gate
  jetzt. Fuer Null-Boxen UND (Nachbesserung 1) fuer die 14 Records mit 0 < h < 112 kippt FUN_8002da4c auf
  der kleinen Achse (Treffer bei |d| >= 112 - h); praktisch erreichbar ist davon nichts (Abnahme 0 §5 fuer die
  Null-Boxen, Nachbesserung 1 fuer 3091 obj 1 und die 14 Records: Spieler dort nie auf dem Objekt-Band ohne
  Standobjekt). Ein Abgleich gegen das laufende Original fuer ein Null-Box-Objekt auf Spieler-Band fehlt —
  es gibt keinen Raum, in dem das ohne Fremd-Schub entsteht.

## Fuer den Nutzer

- Keine neuen Assets (Paket-Gate unveraendert), keine Sprachdateien.
- Bedienung wie vorher: UP gehalten schiebt den Tritt (nach ~9 Bildern Kontakt), Aktionstaste (Viereck)
  vor dem Tritt klettert hinauf, oben Viereck an der Kante springt herunter.
- Einordnung des Befunds "nicht mehr": der Tritt in ROOM10F0 war im Port seit dem byte-true Klettern
  (2026-08-21, b86957e3) nie kletterbar — `git log -L` auf die Flags-Installation (scd_vm.c, `pc[6..7] | 1`)
  und auf das Gate (climb_common.c) zeigt seither keine Aenderung; davor gab es im Port gar kein Klettern.
  Der Weg fiel erst mit der Runde-35-Szene in 10F0 (Parkplatz-Kette) auf. Betroffen waren alle
  Tritte/Leitern/Kisten ohne Bit 0x100 (10F0, 11A0, 2040 obj 1/2, 2060 obj 2, 20A0, 5020 + Elza-Spiegel),
  jetzt in jeder Lage, die das Spiel beim Laden aufbaut, gemessen kletterbar (Zensus-Pin, 22 Records); die
  Kisten in ROOM1090 (0x0101) gingen immer. ⛔ Berichtigt (Nachbesserung 2): die Kiste in ROOM11A0/11A1 steht
  bei jedem Betreten auf der erhoehten Ebene (Band 3) — im Original genauso; die zweite Lage im Raumskript
  (Boden, @0x0107E) baut das Original nie auf.
- Entscheidungsfrage: keine.
- Nachbesserung 1 (Abnahme 0): KEINE Verhaltensaenderung im Spiel — berichtigt wurde eine falsche
  RE-Aussage im Dossier/Testkommentar (Sonderfall Null-Box ROOM3091), dazu ein zusaetzlicher Riegel und
  drei reine Kommentar-Korrekturen. Keine neuen Assets, keine Sprachdateien.

## Anhang — Zensus Typ-4-Objekte (A_belege/zensus_obj_typ4_flags.txt, Auszug)

```
ROOM1090 obj=0 type=4 band=1 flags=0x0101 KLETTERBIT      ROOM10F0 obj=0 type=4 band=0 flags=0x0001 ohne-0x100
ROOM1090 obj=1 type=4 band=5 flags=0x0101 KLETTERBIT      ROOM10F1 obj=0 type=4 band=0 flags=0x0001 ohne-0x100
ROOM11A0 obj=0 type=4 band=3 flags=0x0001 ohne-0x100      ROOM2040 obj=1/2 type=4 band=3 flags=0x0001 ohne-0x100
ROOM2060 obj=2 type=4 band=0 flags=0x0001 ohne-0x100      ROOM20A0 obj=0 type=4 band=4/0 flags=0x0001 ohne-0x100
ROOM3091 obj=1 type=4 band=1 flags=0x0081 ohne-0x100      ROOM5020 obj=0 type=4 band=4/0 flags=0x0001 ohne-0x100
```

## Nachbesserung 1 (nach Abnahme 0, 2026-10-10 abends)

Abnahme 0 (`A_abnahme_0.md`): Punkt 1 erfuellt, EIN Mangel — die RE-Aussage zu ROOM3091 obj 1 ("ohne Box
trifft FUN_8002da4c mit Rand -112 nie") ist widerlegt. Plan: (1) FUN_8002da4c selbst disassemblieren und die
Wahrheitstafel fuer h=0 / Rand -112 belegen, (2) Dossier + Sondenkommentar berichtigen, (3) 3091 obj 1 in der
VM mit Querversatz >= 112 fahren (Diagnose) und die Wahrheitstafel als Riegel festhalten, (4) Erreichbarkeit
von 3091 obj 1 (Spieler auf Band 1 in Reichweite?) messen, (5) H1 (ROOM2060 Spinne) als OFFEN mit Messweg.
Am Engine-Fix (climb_common.c:99) aendert die Nachbesserung NICHTS — Engine-Dateien nur Kommentare.

### Mangel 1 — widerlegte Null-Box-Aussage zu ROOM3091 obj 1

**Ursache.** Die Zensus-Sonde fuhr je Objekt nur die vier ACHSGERADEN Anlaeufe (Querabstand 0). Bei der
Null-Box ist dann |dx| bzw. |dz| = 0, und genau diese Achse lehnt FUN_8002da4c ab — "kein Start" war richtig
gemessen, aber zu "trifft NIE" verallgemeinert; der vorzeichenlose Vergleich mit negativem h + Rand wurde
nicht ausgewertet. Betroffen: Zensus-Pin-Tabelle (Zeile 3091), die Messzeile darunter, der Kopfkommentar
`probe_r36_klettern_zensus.c:16-17`, der Kommentar climb_common.c (3091 obj 1 als "Tritt/Leiter") und die
Commit-Message fd9c0f48 (Historie, nicht aenderbar — hier als ueberholt vermerkt).

**Messung vorher.** `A_belege/zensus_sonde_nachher.txt` (Stand vor der Nachbesserung): ROOM3091 obj 1 nur
`yaw=0/1024/2048/3072 start=(-15522,-1010)/(-15072,-560)/(-14622,-1010)/(-15072,-1460) reach=1 try=0` — vier
achsgerade Seiten, kein Schraeganlauf gefahren, die Behauptung "nie" war ungemessen.

**Beleg — FUN_8002da4c selbst disassembliert** (`re15_disasm.py dis 0x8002da4c 32` / `bytes 0x8002da8c 64`,
info/Re1.5/PSX.EXE):
```
8002d394: jal   0x8002da4c          93 b6 00 0c      (FUN_8002d2c0, Kletter-Sonde)
8002d398: addiu a2,zero,-112        90 ff 06 24      Rand -112 im Delay-Slot
8002da50: lbu   v0,130(t0)                           Objekt-Band obj+0x82
8002da58: bne   v0,a3,0x8002daac                     Band-Gate -> 0
8002da5c: addu  t1,a2,zero                           t1 = Rand
8002da60: lw    a2,120(t0)                           Box-Zeiger obj+0x78
8002da64-88:    dx = p.x - (box.cx + obj.x) ; dz = p.z - (box.cz + obj.z)
8002da8c: lhu   v0,6(a2)            06 00 c2 94      box.hx
8002da90: lhu   v1,10(a2)           0a 00 c3 94      box.hz
8002da94: addu  v0,v0,t1            21 10 49 00      hx + Rand
8002da98: addu  a1,a1,v0            21 28 a2 00      dx + (hx + Rand)
8002da9c: sll   v0,v0,1             40 10 02 00      2 * (hx + Rand)
8002daa0: sltu  v0,v0,a1            2b 10 45 00      VORZEICHENLOS
8002daa4: beq   v0,zero,0x8002dab4  03 00 40 10
8002daa8: addu  a2,v1,t1            21 30 69 00      hz + Rand (Delay-Slot)
8002daac: j     0x8002dac4 / 8002dab0: addu v0,zero,zero        -> 0
8002dab4: addu  v1,a0,a2 / 8002dab8: sll v0,a2,1 / 8002dabc: sltu v0,v0,v1 (2b 10 43 00)
8002dac0: xori  v0,v0,0x1           01 00 42 38      Rueckgabe = Z-Bedingung
```
h = 0, Rand = -112: h + Rand = 0xFFFFFF90, 2*(h + Rand) = 0xFFFFFF20. Abgelehnt wird nur, wenn
(dx - 112) mod 2^32 > 0xFFFFFF20, d.h. -223 <= dx - 112 <= -1, also **-111 <= dx <= 111**. Damit:
**Treffer <=> |dx| >= 112 UND |dz| >= 112** (vom Boxmittelpunkt obj + box.c). Nachgerechnet mit einer
Emulation genau dieser Instruktionen (`A_belege/nb1_tafel_8002da4c.py/.txt`): die acht Punkte der Abnahme
plus vier Grenzpunkte bestaetigt (0/0, 0/300, 300/0, 111/111, -111/500, 112/-111 -> 0; 112/112, -112/-112,
-112/500, 300/300, -500/800, 900/-900 -> 1) und 1D-Brute-force je Achse ueber dx, dz in [-30000, 30000]: **0
Abweichungen** von der geschlossenen Form. Der Port (re15_collision.c:308-311, u32-Arithmetik) rechnet
identisch (Riegel unten). Allgemein: eine Achse kippt immer dann, wenn h + Rand < 0, also h < 112 —
Zensus aller 672 Obj_model_set-Records (`A_belege/nb1_hzensus.txt`): 558 mit h(x,z) = 0, dazu 14 mit
0 < h < 112 auf einer Achse (ROOM1190/1191 obj 0-3 und 11C0/11C1/4030/4031 obj 0: h(45,1260,700) Band 1;
50C0/50C1 obj 0: h(2160,2700,90) Band 1).

**Aenderung.**
| Datei | Aenderung |
|---|---|
| `A_klettern.md` | Zensus-Pin-Zeile 3091, Messzeile "reach=1 try=0" und RE-Belege berichtigt (dieser Abschnitt) |
| `tests/unit/probe_r36_klettern_zensus.c` | Kopfkommentar berichtigt (vorher Z. 16-17). NEU Abschnitt "Null-Box" (`nullbox_3091`): (a) Wahrheitstafel `re15_collision_prop_box_hit` an der echt installierten obj 1 (12 Punkte), (b) `seite_quer`: Anlauf mit Querversatz q und waehlbarem Spieler-Band, je yaw 0/1024/2048/3072: q = 0 und q = +-300 auf Band 1, q = +-300 vom Boden (Band 0). Riegel im bestehenden `unit_r36_klettern_zensus`. stdout ungepuffert (die Abnahme sah 5 von stderr zerschnittene Zeilen) |
| `tests/unit/probes/r36_klettern.cmake` | nur Kommentar (Null-Box-Riegel beschrieben) |
| `engine/src/climb_common.c` | nur Kommentar: 3091 obj 1 ist eine Null-Box (Treffer nur schraeg), kein Tritt/keine Leiter |
| `engine/src/granate_1150.c:96`, `engine/src/sicherung_1150.c:132` | nur Kommentar (Abnahme 0 H2): "nicht kletterbar (kein 0x100)" war die alte, falsche Gate-Semantik — jetzt: Bit 0 @0x80040998, Gate @0x8002d358-35c, Sonde nur fuer Spieler auf Band 1 ohne Standobjekt (@0x8002d2f0, @0x8002d364-70) |

**Messung nachher** (`probe_r36_klettern_zensus pin`, Auszug `A_belege/nb1_nullbox_3091_sonde.txt`):
```
Tafel FUN_8002da4c h=0 Rand=-112: 12/12 wie die Instruktionen (0 Abweichungen)
quer=   0, Spieler Band 1, alle 4 yaw: d=(+-350,0)/(0,+-350) boxhit(-112)=0 try=0 -> kein Objekt-Treffer (4/4)
yaw=1024 quer= 300 start=(-14772,-1800,-560) sonde=(-14772,-1360) d=(300,-350) reach=1 boxhit(-112)=1
         try=1 sub=9 kind=0x81 obj=1 ende=51 end=(-14772,-3600,-1708) band=2 stand=-1 -> Objekt-Treffer (Start)
quer=+-300, Spieler Band 1, alle 4 yaw: 8/8 Start (sub=9, kind=0x81, obj=1), Aufstieg endet nach 51 Bildern auf
         y = -3600 (y -= 1800 @0x800381e4-ec), Band 2 (@0x80038200-08), stand=-1: kein Standobjekt, die
         Null-Box liegt nach dem Vorschub nie unter dem Spieler (Standtest Rand 0x12 @0x80031ce8 braucht
         |dx|,|dz| <= 18) -> der Spieler bliebe auf y = -3600 ohne Boden stehen
quer=+-300, Spieler Band 0 (Boden):     8/8 kein Objekt-Treffer (try=0), obwohl boxhit(-112)=1 — Band-Gate
         FUN_8002d2c0 @0x8002d370 `bne v1,v0` (Spieler-Band 0x800acad6 != obj+0x82)
Null-Box: Tafel 0 Abweichungen, achsgerade ohne Objekt-Treffer 4/4, Querversatz +-300 mit Start 8/8 (Spieler
Band 1), vom Boden (Band 0) ohne Objekt-Treffer 8/8 -> PASS
```
Die berichtigte Aussage lautet damit: **3091 obj 1 passiert seit dem Fix das Gate und wird von FUN_8002da4c
aus jeder Lage mit |dx| >= 112 UND |dz| >= 112 getroffen — achsgerade nie, schraeg ja. Ein Kletterstart
braucht zusaetzlich einen Spieler auf Band 1 ohne Standobjekt.** Das ist byte-true (dieselben Instruktionen
wie im Original), aber im Spiel nicht erreichbar (naechster Absatz).

**Erreichbarkeit im Spiel (statisch belegt, `A_belege/nb1_erreichbarkeit.txt`).** Der Start auf obj 1 verlangt
Spieler-Band == 1 (@0x8002d364-70) UND kein Standobjekt (@0x8002d2e8-f4 `andi v0,v0,0x4000` / `beq`; das
Bit setzt der Standtest @0x80031ce8-d24 `jal 0x8002d100` (a1 = 0x12) / `bne` Band+1 / `sw a0 -> 0x800ac788` /
`ori v0,v0,0x4000`, geloescht @0x80031cd0-e4 — selbst disassembliert). In ROOM3091:
- ALLE Tueren nach 309x setzen den Spieler auf y = 0 = Band 0 (Door_aot_set ROOM3060/3061 @0x00E96,
  30A0/30A1 @0x00722, 30B0/30B1 @0x00912, 30C0/30C1 @0x00B5A, 30D0/30D1 @0x01266: next_y = 0).
- KEINE Treppen-Zonen (sce 12/13, stair_common.c) — AOT-sce in 3091: 0/1/2/5. KEINE SCA-Aufstiegszelle
  (Attributwort & 1, FUN_8002d474 @0x8002d544 `andi v0,s0,0x1`): 0 von 125 Zellen.
- Auf Band 1 kommt der Spieler nur auf die Kiste obj 0 (sub00 @0x01448, Typ 4, Flags 0x0101, h(900,900,900),
  Band 0, Lage (-23550,0,-8024) — 8478/7014 von obj 1 entfernt). Darauf steht er = 0x4000 gesetzt =
  FUN_8002d2c0 liefert 0 (@0x8002d2f4/2fc). Herunter nur per Absprung (Substate 10, Band 0); die Eingrenzung
  FUN_8002cfd4 (FUN_8002bd44 @0x8002bef8-bf0c) haelt ihn sonst auf der Kiste, im Original laesst ihn
  zusaetzlich FUN_8002dc48 (obj 0 traegt 0x100, @0x8002dd94) fallen.
- Einzig ein Fremd-Schub von der Kiste (Mechanismus H1, FUN_8002b544 @0x80031cbc VOR dem Standtest
  @0x80031ce8) koennte "Band 1 ohne Standobjekt" erzeugen; in 3091 stehen nur zwei NPC-Records
  (Sce_em_set Typ 0x49 @0x014E0, Typ 0x42 @0x014F4). Nicht gemessen (kein Spielweg bekannt, der das ausloest).
Dieselbe Pruefung fuer die 14 Records mit 0 < h < 112 (alle Band 1, in 1190/1191, 11C0/11C1, 4030/4031,
50C0/50C1): Tueren hinein durchweg next_y = 0, keine Treppen-AOTs, 0 SCA-Aufstiegszellen, keine Typ-4-Objekte
-> der Spieler ist dort immer auf Band 0, das Band-Gate @0x8002d370 haelt diese Objekte aus der Sonde. (Die
Null-Box-Records insgesamt hat Abnahme 0 §5 geprueft: 0 Paare in Reichweite einer Standflaeche gleichen Bands.)

### Tests der Nachbesserung
`unit_r36_klettern_zensus` traegt jetzt zusaetzlich den Null-Box-Riegel (Tafel 12/12, achsgerade 4/4 ohne
Objekt-Treffer, quer +-300 8/8 Start auf Band 1, quer vom Boden 8/8 ohne Objekt-Treffer).

**Bau:** `bash re15_port/tools/local_build.sh build` -> `=== LOCAL-BUILD-OK (build)` (climb_common.c,
granate_1150.c, sicherung_1150.c neu uebersetzt, re15_pc.exe und alle Testprogramme neu gelinkt).

**Gezielt (keine volle Suite, Auftrag)** — eigene Tests + die Testgruppen jedes Moduls, das der Diff
beruehrt (climb_common.c/1090-Klettern/10F0, granate_1150.c, sicherung_1150.c):
```
ctest --test-dir re15_port/build -R "^(unit_r36_klettern_10f0_endlage|unit_r36_klettern_10f0_startlage|unit_r36_klettern_zensus|integration_r36_klettern|unit_1090_climb_pin|unit_1090_stay_on_crate|unit_rvd_scan_ungated_by_climb|unit_1090_push_pin|unit_1090_push_anim|unit_r22_10f0_figur|unit_r35_cut10f0_.*|unit_r30_granate|integration_r30_granate_laden|unit_sicherung_1150|unit_r30_sicherung_.*|integration_r30_sicherung_laden|integration_r30_sicherung_bild)$" --timeout 1500 --output-on-failure
...
26/26 Test #575: integration_r36_klettern ...........   Passed  531.61 sec
100% tests passed, 0 tests failed out of 26
Total Test time (real) = 679.12 sec
```
(Einzelzeilen `A_belege/nb1_ctest_kompakt.txt`.) Nach den letzten reinen Kommentar-Zitaten in der Sonde
(DAT_800109c8, @0x800381e4) neu gebaut und `ctest --test-dir re15_port/build -R "^unit_r36_klettern_"
--timeout 600 --output-on-failure` -> `100% tests passed, 0 tests failed out of 3`.

Die Riegel der erfuellten Punkte bleiben gruen — `integration_r36_klettern` (echte exe) in diesem Lauf
(Arbeitsordner `build/tests/integration/r36_klettern_wd/lauf_e1ec0a7b_*`, selbst gelesen):
```
A_leon_10f0 : T572 push=5 | T1121 sub=9 kind=0x81 obj=3 | T1173 pos=(-4899,-1800,-1206) band=1 stand=3 | T1227 room=1090 | PL00
B_elza_10f1 : T590 push=5 | T1137 sub=9 kind=0x81 obj=3 | T1189 pos=(-5015,-1800,-1174) band=1 stand=3 | T1243 room=1091 | PL04 (Elza-Bit=1)
C_leon_szene: T3741 push=5 | T4286 sub=9 kind=0x81 obj=3 | T4338 pos=(-5083,-1800,-1206) band=1 stand=3 | T4392 room=1090 | PL00
```
(A und B bildgleich zum Suite-Lauf 1 der Fortsetzung; C mit derselben Kette, Bildnummern um ~30 frueher.)

## Nachbesserung 2 (nach Abnahme 1, 2026-10-10 spaet)

Abnahme 1 (`A_abnahme_1.md`): Punkt 1 erfuellt (E1/E2/E3, Gegenprobe V1), EIN Mangel — der Zensus-Pin gibt
fuer ROOM11A0/11A1 die Lage (5,6)=1 (Record @0x0107E, Band 0 (-19300,0,3300)) als gemessen aus, faehrt sie aber
nie: `reset_room` setzt das Flag VOR `scd_room_reenter`, die Raum-Init nullt Bank 5 Wort 0, der Riegel prueft
die installierte Lage nicht und zaehlt die Wiederholung mit ("24/24"). Am Engine-Fix (climb_common.c, Gate
@0x8002d358-35c) aendert die Nachbesserung NICHTS.

Plan: (1) REPRODUZIEREN (`probe_r36_klettern_zensus pin`, die vier 11A0/11A1-Zeilen); (2) SELBST
disassemblieren: Bank-5-Wisch FUN_8003ecec @0x8003ed74, Kette @0x80039a00 -> FUN_8003ef6c @0x8003ef84, SCD-Start
danach; Flag-Adressierung (5,6) -> Wort 0 von Bank 5 belegen; ROOM11A0 main00/sub01 selbst lesen (Lage-Ck,
Latch sub01 @0x012B2-c4); pruefen, ob irgendein Original-Pfad die Kiste zur Laufzeit auf Band 0 bringt (Schub-
Code, Member_set); (3) Riegel: je Lage die installierte Lage (x/y/z/Band) gegen den RDT-Record (Byte-Offset)
pruefen — ein wirkungsloses Lage-Flag wird rot; (4) die Lage @0x0107E WIRKLICH fahren (Flag NACH dem Wisch,
main00 erneut) und ausdruecklich als "beim Laden unerreichbar" ausweisen; Zaehlung getrennt (beim Laden
erreichbar / nur erzwungen); (5) Aussagen berichtigen (Dossier-Tabelle, "24/24", "beide Lagen gemessen",
"Fuer den Nutzer", r36_klettern.cmake, Sondenkopf); (6) bauen, gezielte Tests, Commit.

### Mangel 1 (Abnahme 1) — Lage (5,6)=1 ROOM11A0/11A1 nie gefahren

**Ursache.** `reset_room` der Zensus-Sonde setzte das Lage-Flag VOR `scd_room_reenter`; der Raum-Init nullt
Bank 5 Wort 0 (scd_room_setup.c:297 = @0x8003ed74), bevor main00 die Lage liest. Bank 3/4 (10F0, 2060, 20A0,
5020) ueberleben, Bank 5 (11A0/11A1) nicht. Der Riegel verglich die installierte Lage mit nichts und zaehlte die
Wiederholung als eigenes "Objekt (Raum x Lage)".

**Messung vorher** (`A_belege/nb2_zensus_vorher_11a0.txt`, Sonde am Stand 5f4e3ae1): `ROOM11A0 flag=1 ...
band=3 pos=(10050,-5400,8800)` und `ROOM11A1 flag=1 ... band=3 pos=(10050,-5400,8800)` = dieselbe Lage wie
flag=0; Summe `Zensus: 24/24 Objekte (Raum x Lage) kletterbar`, `PASS`. Reproduziert.

**Beleg 1 — der Wisch laeuft bei JEDEM Raumladen unbedingt VOR main00** (info/Re1.5/PSX.EXE, md5
b55fdaa5..., Auslieferungsstand @0x80026e4c `08 00 e0 03` geprueft; `re15_disasm.py dis`, Scan
`A_belege/nb2_jalscan.py` -> `A_belege/nb2_scans.txt`):
```
FUN_800396fc (Raumlader; Aufrufer NUR 0x8001d5ac und 0x8001d988 = Tuer-/Selbst-Tuer-Weg der Transitions-FSM)
  Verzweigungen im Rumpf: 0x80039770/808/830/84c/894/8c8/934 — Ziele alle < 0x80039990, keine springt hinter
  80039a00: jal 0x8003ef6c            (einziger Aufrufer von FUN_8003ef6c)
FUN_8003ef6c (SCD-Raum-Init, gerader Code ohne Verzweigung)
  8003ef74: jal 0x8003ea3c            (RNG 0x800b3f6c)
  8003ef7c: jal 0x8003ebf4            (work_vars 0x800b0fd0..d6, @0x8003ec1c sw zero -> 0x800b102c = Bank 5 Wort 1)
  8003ef84: jal 0x8003ecec            (einziger Aufrufer von FUN_8003ecec)
  8003ef90: lw v0,-14472(v0)          Raumkopf 0x800ac778
  8003efa0: lw v0,64(v0) / 8003efac: sw v0 -> 0x800b3f70 / 8003efb0: jal 0x8003ee3c (a0 = 0)   Thread 0 = main00
  8003efc4: lw v0,68(v0) / 8003efd0: sw v0 -> 0x800b3f70 / 8003efd4: jal 0x8003ee3c (a0 = 1)   Thread 1 = sub-SCD
  8003f018: jal 0x8003f0a0            Dispatcher (Init-Lauf)
FUN_8003ecec (nach der 10er-Schleife @0x8003ed00-4c gerader Code bis jr ra)
  8003ed70: lui at,0x800b             0b 80 01 3c
  8003ed74: sw zero,4136(at)          28 10 20 ac   -> 0x800b1028 = Bank 5 Wort 0 (Tabelle 0x80074664[5])
```
Kopf+0x40 (main00) schreibt NUR @0x8003efa0/ac in den Skript-Zeiger 0x800b3f70 (Scan aller `sw ...,0x3f70(at)`:
0x8003efac Kopf+64, 0x8003efd0/0x8003f07c/0x8004234c Kopf+68 = sub-SCD, 0x80016564 und 0x8004c720 fremde
Skripte mit Modus 0x800b3f7a = 1/2). main00 laeuft also genau einmal je Raumladen und immer nach dem Wisch.
(5,6) = Wort 0 Maske 0x02000000 (MSB-zuerst, game_state.c `0x80000000 >> (idx & 0x1f)` = FUN_8004ef90).
Banks 3/4 (0x800b0ff8 / 0x800b1018) fasst keine der Init-Funktionen an (FUN_8003ea3c, FUN_8003ebf4,
FUN_8003ecec, FUN_8003edbc @0x8003edc8 = 0x800ac9b0..0x800aca2c) — deshalb wirkt (3,102)/(3,172)/(3,204)/(4,203)
beim Laden, (5,6) nicht.

**Beleg 2 — ROOM11A0/11A1-SCD** (`tools/scd_dump_room.py`, RDT md5 = info/Re1.5, 11A0 0f47fa3d..., 11A1 e2d462fe...):
main00 @0x0103C `Ifel_ck` / @0x01040 `21 05 06 00` (Ck 5,6 == 0) -> Aot_set Slot 6 sce 5 @0x01044 + Obj_model_set
@0x01058 (`2d 00 04 00 03 00 00 00 00 00 42 27 e8 ea 60 22 ...` Band 3 (10050,-5400,8800)); @0x0107A Else_ck ->
Obj_model_set @0x0107E (`2d 00 04 00 00 00 00 00 00 00 9c b4 00 00 e4 0c ...` Band 0 (-19300,0,3300)). Weitere
Zugriffe auf (5,6): nur sub01 (11A0 @0x012B2 Ck / @0x012BE `3e 00 0f 00 06 00` Member_cmp 15 == 6 / @0x012C4
Set 5,6,1; 11A1 @0x012C6 / @0x012D2 / @0x012D8). Kein weiterer Obj_model_set fuer obj 0 ausserhalb main00.
Folge: der Latch in sub01 wirkt nur auf das NAECHSTE main00 — und vor dem steht wieder der Wisch.

**Beleg 3 — das Objekt-Band (+0x82) aendert sich zur Laufzeit nicht** (Scan `A_belege/nb2_band_schreiber.py`
aller `sb/sh rt,130(rs)` in PSX.EXE und STAGE1.BIN, dazu `sw/swl/swr` mit 128..131):
- Objekt-Pool: NUR der Installer @0x80040974 `sb v0,130(a1)` und der Member-Schreiber FUN_8004116c
  (Sprungtabelle @0x80010c8c, 20 Eintraege, Eintrag 18 = 0x80041224 `j 0x80041230` mit Delay @0x80041228
  `sb a2,130(a0)`); gerufen von Member_set FUN_800410b8 (@0x800410d0 `lbu a1,1(v0)` Member, @0x800410d4
  `lh a2,2(v0)` Wert, @0x800410d8 `jal 0x8004116c`) und Member_set2 (@0x8004113c).
- Alle uebrigen Schreiber zielen auf Aktoren ueber g_entity(cur) 0x800ac784 (@0x8001be54, @0x8005276c,
  @0x80052dd0), den Spieler-Fall (@0x8002c6c4), Effekte (@0x8001f1b4/@0x8001f254, Halbwort) oder den Stapel.
- ROOM11A0/11A1: 7 Member_set, ALLE Member 1 (= Eintrag 1 @0x8004119c `sw a2,56(a0)`, y; Objekte 2/3 der
  Kamera-Ausblendung), kein Member_set2 (0x35), kein Member 18.
-> Im Original steht die Kiste in JEDEM Besuch auf Band 3 (Record @0x01058); der Record @0x0107E (Band 0) ist im
Auslieferungsstand totes Datum — weder beim Laden noch zur Laufzeit erreichbar. Der Port tut dasselbe
(scd_room_setup.c:297 Wisch; Member_set auf Objekte schreibt nur x/y/z/rot, scd_vm.c). Die Frage der Abnahme
1 ("ob das Original die Kiste dynamisch auf Band 0 bringt, ist ungemessen") ist damit statisch beantwortet: nein.

**Aenderung (nur Testcode und Text, KEIN Engine-Code; re15_pc.exe unveraendert, md5 74a17d61... = die von Abnahme
1 gemessene exe — der Bau uebersetzte nur `probe_r36_klettern_zensus.c`).**
| Datei | Aenderung |
|---|---|
| `tests/unit/probe_r36_klettern_zensus.c` | Tabelle je RECORD (`R[]`, 24 Eintraege: Raum, Lage-Flag + Wert, Datei-Byte-Offset des Obj_model_set, erwartete Kandidatenzahl, `laden` 1/0). (a) LAGE-PRUEFUNG `lage_pruefen`: liest die Record-Bytes am Offset (Opcode 0x2D, Typ 4, obj_id) und vergleicht das installierte Objekt Feld fuer Feld (aktiv, Typ, Band pc[4], x/y/z pc[10..15], Flags pc[6..7]\|1 @0x80040990-a4, Box pc[22..33]) — Abweichung = FAIL "Lage-Flag wirkungslos?"; dazu Kandidaten-Zaehlung (aktive Typ-4 ohne 0x100 == erwartet, Record-Objekt dabei). (b) Lage-Flags Bank 3/4 wie ein Spielstand VOR dem Raum-Init; Bank 5 ((5,6)=1, 11A0/11A1 @0x0107E) ERZWUNGEN NACH dem Wisch ueber den Modell-Sync-Rueckruf (`re15_scd_set_player_model_sync`, in scd_room_reenter nach `g_game.flags[5][0] = 0` und vor `scd_thread_start`) und getrennt gezaehlt ("beim Laden erreichbar" / "nur ERZWUNGEN"). (c) GEGENPROBE: (5,6)=1 VOR dem Raum-Init (der alte Weg) -> Flag danach muss 0 sein UND die Lage-Pruefung muss @0x0107E als FALSCH melden, sonst FAIL. Kopfkommentar berichtigt (11A0-Zeile, Abschnitt "NACHBESSERUNG 2"). Null-Box-Teil unveraendert (nur Typ `rec_t`, RDT-Laden gecacht) |
| `tests/unit/probes/r36_klettern.cmake` | nur Kommentar (Records, Lage-Pruefung, 22 + 2 erzwungen, Belege) |

Warum der Rueckruf und nicht "main00 ein zweites Mal" (Plan-Punkt 4): ein zweiter Obj_model_set derselben obj_id
trifft im Original denselben Pool-Slot und schaltet das Objekt AUS (LAB_80040914 @0x80040980 `beq v0,zero` /
@0x8004098c `sw zero,0x0(a1)`, im Port scd_vm.c op_obj_model_set `reinstall`) — die Kiste waere danach
inaktiv statt in der Lage @0x0107E. Der Rueckruf setzt das Flag an der einzigen Stelle zwischen Wisch und
main00-Start; main00 laeuft dann genau einmal, wie beim echten Laden.
| `A_klettern.md` | Zensus-Tabelle Zeile 11A0/11A1, "Damit ist jedes Objekt ... beiden Lagen gemessen", die "x/24"-Zeilen, Testbeschreibung, OFFEN "Erledigt", "Fuer den Nutzer" berichtigt (⛔-Vermerke), dieser Abschnitt |

**Messung nachher** (`build/tests/unit/probe_r36_klettern_zensus.exe pin`, exit 0, `A_belege/nb2_zensus_nachher.txt`):
```
--- ROOM11A0 Record @0x01058 flag(5,6)=0 beim Laden: Flag nach dem Raum-Init 0; Lage OK (obj 0 Band 3 (10050,-5400,8800) fl 0x0001 box c(0,-900,0) h(900,900,900) = Record); Typ-4 ohne 0x100: 1 (soll 1)
--- ROOM11A0 Record @0x0107E flag(5,6)=1 ERZWUNGEN (Flag nach dem Wisch @0x8003ed74): Flag nach dem Raum-Init 1; Lage OK (obj 0 Band 0 (-19300,0,3300) fl 0x0001 box c(0,-900,0) h(900,900,900) = Record); Typ-4 ohne 0x100: 1 (soll 1)
    ROOM11A0 lage=@0x0107E flag=1(ERZWUNGEN) obj=0(prop 0) fl=0x0001 band=0 pos=(-19300,0,3300) yaw=3072 start=(-19300,1950) reach=1 try=1 sub=9 kind=0x81 obj=0 ende=51 oben=(-19300,-1800,3098)(soll y -1800) band=1(soll 1) stand=0 -> OBEN
    (11A1 identisch; alle 4 Seiten beider erzwungener Records OBEN)
--- ROOM10F0 Record @0x0111A flag(3,102)=1 beim Laden: Flag nach dem Raum-Init 1; Lage OK (obj 0 Band 0 (-4850,0,-850) ...)
--- ROOM20A0 Record @0x0187A flag(3,204)=1 beim Laden: Flag nach dem Raum-Init 1; Lage OK (obj 0 Band 0 (23150,0,-24400) ...)
--- ROOM5021 Record @0x01288 flag(4,203)=1 beim Laden: Flag nach dem Raum-Init 1; Lage OK (obj 0 Band 0 (-3400,0,-25300) ...)
Zensus: 24/24 Records (Raum x Objekt x Lage) wie der RDT-Record installiert und kletterbar; davon beim Laden erreichbar 22/22, nur ERZWUNGEN (11A0/11A1 @0x0107E, Bank 5) 2/2
--- GEGENPROBE ROOM11A0: flag(5,6)=1 VOR dem Raum-Init -> Flag danach 0, Lage-Pruefung @0x0107E: FALSCH (soll Band 0 (-19300,0,3300) fl 0x0001, ist aktiv 1 Band 3 (10050,-5400,8800) fl 0x0001)
--- GEGENPROBE ROOM11A1: (dasselbe)
Null-Box: Tafel 0 Abweichungen, achsgerade ohne Objekt-Treffer 4/4, Querversatz +-300 mit Start 8/8 (Spieler Band 1), vom Boden (Band 0) ohne Objekt-Treffer 8/8
PASS
```
96/96 Seiten OBEN (24 Records x 4), alle 24 Lage-Pruefungen "= Record", alle Kandidatenzahlen wie erwartet (2040/2041: 2).
Die Gegenprobe zeigt den Mangel der Abnahme genau so, wie er war (Flag vor dem Init -> 0, Kiste auf Band 3) — und
dass die neue Lage-Pruefung ihn erkennt.

**Riegel ohne Fix** (dieselbe neue Sonde, Gate voruebergehend wieder `!= 0x101u`, eigenes Bauverzeichnis
`build_nb2gate`, Quelle danach mit Original-mtime zurueck — `A_belege/nb2_zensus_altes_gate.txt`): `Zensus: 0/24
Records ... beim Laden erreichbar 0/22, nur ERZWUNGEN 0/2`, 96/96 Seiten "kein Start", alle 24 Lage-Pruefungen
OK (die Lage haengt nicht am Gate), Null-Box quer 0/8 (0x0081 faellt am alten Gate durch), `FAIL 25`, exit 1.

**Beleg 4 — auch der Latch in sub01 kann im Auslieferungsstand nicht feuern** (selbst disassembliert): sub01
prueft `Member_cmp 15 == 6` = obj+0x0B (Member-Tabelle @0x80010c8c Eintrag 15 -> @0x8004120c `j` / Delay
`sb a2,11(a0)`). obj+0x0B stempelt die AOT-Abtastung FUN_80042bac im Objekt-Pool-Pass (Aufruf aus FUN_800436a8
@0x8004378c, a1 = 4, Stride 148, a2 = 0 = AUTO): Stempel `80042f58 addiu v0,s2,255` / `80042f5c sb v0,11(s1)`
bzw. der AUTO-Stempel `80042f44 addiu v0,s2,255` / `80042fc4 sb v0,11(s1)` (0-basierter Slot-Index auf das
Objekt; vgl. aot_common.c:925-946). VOR beiden Stempeln im selben Slot-Durchlauf das Band-Gate:
```
80042cac: lbu  v0,2(s0)          ; Floor-Byte des AOT
80042cb4: andi v0,v0,0x80        ; 0x80 = Platzhalter (jedes Band)
80042cb8: bne  v0,zero,0x80042cd4
80042cc0: lbu  v1,130(s1)        ; Band des Objekts (+0x82)
80042cc4: lbu  v0,2(s0)
80042ccc: bne  v1,v0,0x8004301c  ; anderes Band -> Slot uebersprungen, kein Stempel
```
Slot 6 hat Floor 0 (Aot_set @0x01044 `2c 06 05 44 00 ...`, Byte [4] = 0, kein 0x80), die Kiste steht auf Band 3
(Beleg 3: keine Laufzeit-Aenderung) -> Slot 6 stempelt sie nie -> `Member_cmp 15 == 6` nie wahr -> (5,6) wird im
Spiel nie gesetzt. Record @0x0107E und der Latch @0x012B2-c4 sind damit doppelt tot (Prototyp-Rest; die Kiste
muesste dafuer auf Band 0 gelangen, was kein Code tut). Fuer den Nutzer aendert sich nichts: die Kiste steht im
Port wie im Original bei jedem Betreten auf Band 3 und ist dort kletterbar (Zensus-Record @0x01058, exe-Lauf E3
der Abnahme 1).

Die Commit-Message fd9c0f48 ("je Lage per Flag ... (5,6) ... 24/24") ist Historie und nicht aenderbar — sie ist
durch diesen Abschnitt ueberholt (wie in Nachbesserung 1 fuer die 3091-Aussage).

### Tests der Nachbesserung 2 (gezielt, keine volle Suite)

**Bau:** `bash re15_port/tools/local_build.sh build` -> `=== LOCAL-BUILD-OK (build)`; uebersetzt wurde NUR
`probe_r36_klettern_zensus.c` (+ CMake-Neulauf wegen des geaenderten Kommentars in `probes/r36_klettern.cmake`).
`re15_pc.exe` md5 `74a17d614423a4502cfd9be8b8bca52c` = die von Abnahme 1 gemessene exe (kein Engine-Code
geaendert).

Schnelle Gruppe (eigene Tests + jedes Modul, das der Zweig gegen master beruehrt — climb_common.c/1090/10F0,
granate_1150.c, sicherung_1150.c, Null-Box-Module, Titel-Pfad main.c — wie Abnahme 1, dazu `unit_1190_ruestung`
als zweiter Nutzer von `re15_scd_set_player_model_sync`), nach dem letzten Bau:
```
ctest --test-dir re15_port/build -R "^(unit_r36_klettern_.*|unit_1090_.*|unit_rvd_scan_ungated_by_climb|unit_re2_room1090_ab|unit_r20_birkin_push_pin|unit_r22_10f0_figur|unit_r35_cut10f0_.*|unit_r35_inventar1050_kiste|unit_stair_1170_regression|unit_stair_hwtruth_1060|unit_r30_granate|unit_r35_granate|unit_sicherung_1150|unit_r30_sicherung_.*|unit_room1050_sicherung|unit_r30_irons_tisch|unit_r30_irons_tisch_licht|unit_r31_hebetisch|unit_r32_hebetisch_faecher|unit_r34_plattform|unit_r34_plattform_ton|unit_r34n_e_dokumente|unit_r35_inhalt_.*|r30_titel_blinken|unit_enemy_body_push|unit_plc_back_yaw_1090|unit_map_1090|unit_1190_ruestung)$" --timeout 600 --output-on-failure
48/50 Test #572: unit_r36_klettern_10f0_endlage .....   Passed    0.01 sec
49/50 Test #573: unit_r36_klettern_10f0_startlage ...   Passed    0.01 sec
50/50 Test #574: unit_r36_klettern_zensus ...........   Passed    0.28 sec
100% tests passed, 0 tests failed out of 50
Total Test time (real) =  10.83 sec
```
Riegel des erfuellten Nutzer-Punkts (echte exe, echter Weg), nach dem Sondenumbau:
```
ctest --test-dir re15_port/build -R "^integration_r36_klettern$" --timeout 1500 --output-on-failure
1/1 Test #575: integration_r36_klettern .........   Passed  515.69 sec
100% tests passed, 0 tests failed out of 1
```
Arbeitsordner `build/tests/integration/r36_klettern_wd/lauf_cb621d86_*` selbst gelesen:
```
A_leon_10f0 : T572 room=10F0 push=5 | T1121 sub=9 kind=0x81 obj=3 | T1152 pos=(-4899,-1800,-1206) band=1 stand=3 | T1227 room=1090 | EXIT_AT Bild 1400 in 1090 | PL00
B_elza_10f1 : T590 room=10F1 push=5 | T1137 sub=9 kind=0x81 obj=3 | T1168 pos=(-5015,-1800,-1174) band=1 stand=3 | T1243 room=1091 | EXIT_AT Bild 1320 in 1091 | PL04 (Elza-Bit=1)
C_leon_szene: Szene gestartet + zu Ende | T3741 push=5 | T4286 sub=9 kind=0x81 obj=3 | T4317 band=1 stand=3 | T4392 room=1090 | EXIT_AT Bild 4550 in 1090 | PL00
```
(Bildnummern von Push/Start/Raumwechsel gleich dem Lauf der Nachbesserung 1; hier ist die erste Zeile mit
y = -1800 zitiert, dort die erste mit Substate 0.)

### OFFEN (Stand Nachbesserung 2)

- Unveraendert (ausserhalb des Mangels, oben unter OFFEN mit Adressen + Messweg): H1 ROOM2060 Spinne schiebt
  Leon vom Tritt (@0x80031cbc vor @0x80031ce8, Erreichbarkeit im Original ungemessen); H3 Null-/Kleinbox-Objekte
  auf Spieler-Band ohne Abgleich am laufenden Original; FUN_8002dc48 (Herunterfall 0xd, nur 0x100-Objekte)
  nicht portiert.
- Geschlossen durch Nachbesserung 2: die Frage der Abnahme 1 §5, ob das Original die 11A0/11A1-Kiste dynamisch
  auf Band 0 bringt — statisch NEIN (Beleg 3: kein Laufzeit-Schreiber von obj+0x82 ausser Member 18, den
  11A0/11A1 nicht benutzen; Beleg 4: Slot 6 stempelt die Band-3-Kiste nie; Beleg 1: (5,6) waere vor main00
  ohnehin gewischt). Optionaler dynamischer Gegencheck, falls gewuenscht: DuckStation mit der Auslieferungs-EXE,
  ROOM11A0 per Debug-Menue (Skill re15-room-capture), Kiste auf der Band-3-Ebene schieben und Bild fuer Bild
  0x800b3f98+0x82 (obj 0 Band), 0x800b3f98+0x0B (Stempel) und 0x800b1028 (Bank 5 Wort 0, Maske 0x02000000)
  lesen — erwartet: Band 3, Stempel nie 6, Bit nie gesetzt.

### Fuer den Nutzer (Nachbesserung 2)

- KEINE Verhaltensaenderung im Spiel, keine neuen Assets (Paket-Gate unveraendert), keine Sprachdateien,
  keine Bank-9-Bits/Nachrichten-IDs/Ereignisse.
- Berichtigt wurde eine zu weit gehende Messbehauptung des Kletter-Pins: die Kiste in ROOM11A0 ("SEWER EXIT")
  bzw. Elzas ROOM11A1 steht bei JEDEM Betreten auf der erhoehten Ebene — im Original genauso — und ist dort
  kletterbar. Die zweite Kisten-Lage am Boden, die das Raumskript enthaelt, baut das Original nie auf (Prototyp-
  Rest); der Pin faehrt sie jetzt nur noch ausdruecklich "erzwungen" mit und prueft fuer jede Lage, dass sie
  wirklich installiert ist.
- Entscheidungsfrage: keine.

## Nachbesserung 3 (nach Abnahme 2, 2026-10-10 nachts)

Abnahme 2 (`A_abnahme_2.md`): Punkt 1 erfuellt (M1 echter Weg 10D0 -> 10F0 -> 1090, M2 Endlage von Westen, V1
Gegenprobe ohne Fix, M3/M4 5020/20A0 an der exe); RE-Gate der Nachbesserung 2 NICHT erfuellt. Zwei Maengel:
(1) Beleg 3/4 falsch — @0x8002c6c4 ist nicht der "Spieler-Fall", sondern liegt im Fall-Handler FUN_8002c444
der Typ-4-OBJEKTE (Typ-Tabelle @0x80073c70 [4] = FUN_8002c40c, `jal 0x8002c444` @0x8002c42c; Ausloeser im
Objekt-Pass @0x8002c0fc-c14c); das Objekt-Band +0x82 aendert sich im Original zur Laufzeit; @0x800421d0
(Sce_em_set) fehlt in der Schreiber-Einordnung. (2) Der Port hat diesen Objekt-Fall nicht (M5: 5020-Kiste
faehrt ueber die Band-4-Kante und bleibt in der Luft stehen).

ORCHESTRATOR-ENTSCHEIDUNG (Regel "Original oder nicht", Nutzer-Wortlaut "Das macht ein Fortschritt im Spiel
unmoeglich"): den Objekt-Fall byte-true PORTIEREN (nicht als OFFEN/Frage fuehren).

Plan:
1. REPRODUZIEREN: M5 der Abnahme an der eigenen exe (ROOM5020, Kiste nach Osten ueber die Band-4-Kante).
2. SELBST disassemblieren: Objekt-Pass FUN_8002bd44 @0x8002c0ec-c150 (Ausloeser), Typ-Tabelle @0x80073c70,
   FUN_8002c40c, FUN_8002c444 (ganz: Zustaende obj+5 0..3, Zaehler obj+6, Kippen, Schwerkraft, Bodenhoehe
   @0x8002c67c, Band @0x8002c6c4, Landen, Nachfedern), FUN_8002d3d8 (Fallrichtung), FUN_8003b7f0 (SCA-Wort mit
   Rand), FUN_8001c6e8 (Bodenhoehe); Schreiber-Zensus von +0x82 neu einordnen (inkl. Sce_em_set @0x800421d0).
3. Port: neue Datei `engine/src/objfall_r36.c` + `include/re15_objfall_r36.h`, kleiner Haken im Objekt-Pass des
   Ports; jede Konstante mit @0x.
4. Messen an der exe: ROOM5020/5021 (Kiste faellt ab x >= -8125, landet auf Band 0, Latch @0x012F4-302 setzt
   (4,203)=1), ROOM20A0/20A1 (Tasche, Latch @0x01AD4-e2 -> (3,204)=1); Klettern auf die gefallene Kiste.
5. Mangel 1: Dossier Beleg 3/4/OFFEN, Sondenkopf probe_r36_klettern_zensus.c:50-51, probes/r36_klettern.cmake:36-37
   berichtigen, eeca9a89 als ueberholt vermerken; 11A0/11A1 mit dem richtigen Grund belegen oder als ungemessen.
6. Pins je Raum, bauen, gezielte Tests, Commit.

### Nachbesserung 3 — Messung vorher (Mangel 2 reproduziert)

Lauf `m5_vorher` (exe des Stands 8b489051, Eingabe = M5 der Abnahme 2: `RE15_DEBUG_JUMP=5020@250
RE15_PLAYER_POS=-11000,-21800,1024,4 RE15_INPUT_SCRIPT=W8,U5,W0.5,R0.37,W0.3,U2.0,W0.3,L0.37,W0.3,U1.8,W0.3,L0.37,
W0.5,U8,W2`, `A_belege/nb3_m5_vorher.txt`): bildgleich zur Abnahme — `T1050 t4pos=(-7928,-25332)`, `T1110
t4pos=(-6904,-25332)`, bis `T1350` unveraendert. Die Kiste steht 271 hinter der Band-4-Kante (x -7175) in
Band-4-Hoehe ueber dem Abgrund. **Reproduziert.**

### Nachbesserung 3 — RE-Belege (selbst disassembliert, info/Re1.5/PSX.EXE, Auslieferungsstand)

**B1 — Typ-Handler im Objekt-Pass** (`re15_disasm.py dis 0x8002bd44 ...`): FUN_8002bd44 laeuft je Pool-Eintrag
RUECKWAERTS (@0x8002bdb0-c8, Pool 0x800b3f98, Stride 148), nur aktive (@0x8002bdcc-d8 `andi 0x1` / `beq` ->
@0x8002c150). ZUERST, vor Schub und Ausschiebung:
```
8002bde0: lbu  v0,8(s2)            ; obj+0x08 Typ
8002bde8: beq  v0,zero,0x8002be0c
8002bdf0: lui  at,0x8007 / 8002bdf4: addiu at,at,15472     ; 0x80073c70
8002bdfc: lw   v0,0(at) / 8002be04: jalr v0 / 8002be08: addu a0,s2,zero
Tabelle @0x80073c70: 06 08 ff 00 | f4 c3 02 80 | fc c3 02 80 | 04 c4 02 80 | 0c c4 02 80 | b4 ca 02 80
  [1] 0x8002c3f4 `jr ra`  [2] 0x8002c3fc `jr ra`  [3] 0x8002c404 `jr ra`  [4] 0x8002c40c  [5] 0x8002cab4 `jr ra`
FUN_8002c40c (Bytes ab 8002c414: 04 00 83 90 .. 05 00 60 10 01 00 02 34 03 00 62 14 .. 11 b1 00 0c):
  8002c414: lbu v1,4(a0) / 8002c41c: beq v1,zero (-> Ende) / 8002c424: bne v1,1 (-> Ende)
  8002c42c: jal 0x8002c444         ; nur obj+4 == 1
```
Einziger `jal 0x8002c444` in PSX.EXE und STAGE1-6.BIN = @0x8002c42c (Abnahme 2 per jal-Scan); FUN_8002c40c steht nur als
Tabellenwort @0x80073c80.

**B2 — Ausloeser am Ende jedes Objekt-Durchlaufs** (Bytes `8002c0fc: 08 00 43 92 40 00 42 a6 04 00 02 34 11 00 62 14
44 00 44 a6 04 00 42 92 00 00 00 00 0d 00 40 14 00 00 00 00 34 00 44 26 82 00 46 92 fc ed 00 0c c2 01 05 34 02 00 42
30 06 00 40 10 00 00 00 00 f6 b4 00 0c 21 20 40 02 01 00 02 34 04 00 42 a2 05 00 40 a2 1d ff 60 16 88 00 40 a6`):
```
8002c0ec-c10c: obj+0x40/42/44 = x/y/z (u16, Vorlage fuer FUN_8002cfd4)
8002c0fc: lbu v1,8(s2) / 8002c104: ori v0,zero,0x4 / 8002c108: bne v1,v0 -> 0x8002c150   ; nur Typ 4
8002c110: lbu v0,4(s2) / 8002c118: bne v0,zero -> 0x8002c150                            ; faellt noch nicht
8002c120: addiu a0,s2,52 / 8002c124: lbu a2,130(s2) / 8002c128: jal 0x8003b7f0 / 8002c12c: ori a1,zero,0x1c2
8002c130: andi v0,v0,0x2 / 8002c134: beq v0,zero -> 0x8002c150                         ; Absprungzelle
8002c13c: jal 0x8002d3d8 (a0 = obj)                                                       ; Fallrichtung
8002c144: ori v0,zero,0x1 / 8002c148: sb v0,4(s2) / 8002c14c: sb zero,5(s2)
8002c150: bne s3,zero,0x8002bdc8 / 8002c154: sh zero,136(s2)                             ; obj+0x88 = 0, JEDER Eintrag
```
Jeder aktive Durchlauf erreicht @0x8002c0ec — auch der Standobjekt-Pfad (@0x8002bef8-bf0c `j 0x8002c0ec`) und der
Bit-0x200-Pfad (@0x8002bee4 `j 0x8002c0b8`, faellt bis c0ec durch). FUN_8003b7f0 = Port
`re15_collision_floor_typeword` (selbst gegengelesen: Band-Nibble `sll 16 / sra 28` @0x8003b8a8-ac gegen
`andi a2,s1,0xff` @0x8003b88c, x/z gegen die um a1 erweiterte Zelle @0x8003b8b8-f8 (`sltu`), Rueckgabe `lhu v0,0(a1)`
= Zellwort +10 @0x8003b900, ERSTER Treffer im Quadranten).

**B3 — FUN_8002d3d8 Fallrichtung** (@0x8002d3d8-46c): dx16 = (s16)(obj.x - 0x800aca88) (`lhu`/`subu`/`sll 16`/`sra
16` @0x8002d3dc-400), |dx16| < 900 (`slti v0,v0,900` @0x8002d410) -> richt_z = (dz16 >> 31) | 1, richt_x = 0
(@0x8002d41c-430), sonst richt_x = (dx16 >> 31) | 1, richt_z = 0 (@0x8002d43c-450) -> immer VOM SPIELER WEG.
@0x8002d454-468 schreibt obj+0x68/+0x6c nach DAT_800b282c/2838 — Leser: keine (Scan aller lh/lhu/lw/sh/sw mit
Offset 0x282c/0x2838 in PSX.EXE + STAGE1-6.BIN: nur diese zwei Stores; Ghidra-XREF nur W) -> nicht portiert.

**B4 — FUN_8002c444, der Fall (s1 = a0 = OBJEKT)** (`dis 0x8002c444 230` + `dis 0x8002c7dc 220`; Decompilat
RE_15_Quellcode_V2/FUN_8002c444.c im Ablauf deckungsgleich):
```
Zustand obj+5: 1 -> @0x8002c4fc, 0 -> @0x8002c49c, 2 -> @0x8002c96c, 3 -> @0x8002ca10, sonst -> @0x8002ca8c
0  8002c49c-4f8: 0x8008f680 = 0xd, 0x8008f67c = 0xd, 0x8008f688 = 8, 0x8008f690/684/698/69a/69c/68c = 0,
                 0x8008f6a0 = 0, obj+5 = 1, obj+6 = 0  (faellt in 1 durch)
1  8002c4fc-544: x += (s16)67c * (s16)richt_x ; z += (s16)67c * (s16)richt_z          (mult/mflo/sw 52,60)
   8002c548-5dc: 690 < 1024 (slti): 690 += (u16)684 ; (s16)690 > 1024 -> 684 = 684 + 1024 - 690
                 698 = 684 * -richt_z ; 69c = richt_x * 684 ; sonst 698 = 69c = 0
   8002c5e0-614: 684 += 688 ; 67c += 680 ; obj+6++
   obj+6 < 13 (sltiu 0xd @0x8002c630) -> 8002c92c-968: y += (s16)(67c / 3)   (0x55555556-Idiom)
   sonst 8002c63c-664: 680 = 0 ; y += 68c ; 68c += 160
     obj+6 == 13: 8002c674-680 jal 0x8001c6e8 (a0 = obj+0x34, a1 = 0, a2 = 8, a3 = 0x200) -> 694 = Boden,
                  8002c684-6c4 0x91a2b3c5-Idiom: obj+0x82 = -(Boden / 1800)   `sb v1,130(s1)` (Bytes 82 00 23 a2)
     8002c6c8-6f8: 6a0 == 0 UND (s16)obj+0x88 != 0 UND obj+0x88 < y -> 6a0 = 1, jal 0x80045024 (a0 = 0x02080000),
                  12x jal 0x80019700 (a0 = 0x06022000, a1 = Gier, a2 = 0x80072d4c, a3 = &P) @0x8002c70c-8fc
     8002c904-928: (s16)694 < y -> y = 694, obj+5 = 2
2  8002c96c-a0c: obj+0x88 == 0 -> jal 0x80045024 (a0 = 0x02060000); y -= 30; x/z += richt*20;
                 698 = -(richt_z*30), 69c = richt_x*30; obj+5++
3  8002ca10-a88: y += 30; x/z += richt*20; 698 = richt_z*30, 69c = -(richt_x*30); obj+4 = 0
   8002ca8c-94:  jal 0x8002dacc (a0 = obj, a1 = 0x8008f698)
```
FUN_8001c6e8 ueberspringt das Objekt selbst (`addiu v0,s0,52` / `beq s5,v0` @0x8001c7fc-800) und prueft keine
Aktiv-Flags; Zellen-Stufe `andi 0xf002` gegen Band<<12 (@0x8001c89c) und Zellwort+8 & a3 (@0x8001c8b0).

**B5 — FUN_8002dacc** (`dis 0x8002dacc 100`): RotMatrix(delta, sp+16) `jal 0x80068098` @0x8002db00, MulMatrix2(sp+16,
obj+0x20) `jal 0x800684b4` @0x8002db0c; len = (m02^2 + m12^2 + m22^2) >> 12 (@0x8002db14-5c); y = (m02 << 12) / len
(@0x8002db40/60); obj+0x68 = ratan2(-m12, m22) (@0x8002db8c-98); obj+0x6a = ratan2(y, SquareRoot12(4096 - (y*y >> 12)))
(@0x8002db94-cc); obj+0x6c = 0, RotMatrix(obj+0x68) -> Transpose -> ApplyMatrix auf (m00, m10, m20) (`lhu 32/38/44`),
obj+0x6c = ratan2(r.y, r.x) (@0x8002dbd0-c2c). obj+0x20 ist die Matrix, die der Zeichner FUN_8002c18c in jedem Bild aus
obj+0x68 baut (@0x8002c210-218 `addiu a0,s1,104` / `jal 0x80068098` / a1 = s1+32) -> der Zustand ist das Euler-Tripel.
libgte selbst gelesen: RotMatrix @0x80068098 (Tabelle 0x800794c4, negativer Winkel: Index -a & 0xfff, sin negiert;
`multu`/`mflo`/`sra 12`; m10/m20/m11/m21 @0x80068228-314), MulMatrix2 @0x800684b4 (ctc2 RT, je Spalte MVMVA 0x4a486012 =
sf 1, lm 0 -> IR gesaettigt; alle Spalten vor dem ersten Store gelesen), TransposeMatrix @0x80065d90, ApplyMatrix
@0x800661c0 (MAC, keine Saettigung), SquareRoot12 @0x80066120 (= SquareRoot0 @0x80065f60 mit `addiu t1,zero,19`
@0x80066148 statt 31 und ohne `srl 12`; fuer 0 < a < 2^19 exakt SquareRoot0(a << 12), Herleitung in objfall_r36.c).

**B6 — der Laufzeit-Schreiber obj+0x88 (Wasser)**: Scan aller `sh rt,136(rs)`: PSX.EXE @0x8001a59c (`sh zero` Aktor-Pool,
nach jedem Gegner-Handler), @0x8001f0e4 (Effekt-Struktur, 0x2710), @0x8002c154 (Objekt-Pass, s.o.), @0x8004331c =
LAB_8004330c `lw v1,0x800ac784 (g_entity(cur)) / lhu v0,0(a0) / sh v0,136(v1)` (Bytes `0b 80 03 3c 84 c7 63 8c 00 00 82
94 .. 88 00 62 a4`) = Handler sce 8 (WASSER) der AOT-Tabelle @0x8007469c; Stage-Overlays: nur Aktor-Felder. Der
AUTO-Scan FUN_800436a8 ruft FUN_80042bac fuer JEDEN Objekt-Pool-Eintrag (@0x80043768-b4, a1 = 4, a2 = 0, `sb zero` +0x0B
@0x80043788 davor, KEIN Aktiv-Test); FUN_80042bac: Pool-Maske @0x80042c8c, AUTO @0x80042ca4, Band-Gate @0x80042cac-ccc,
CENTRE @0x80042ea8-ed8 (obj+0x34) / FORWARD @0x80042ef8-f2c, Treffer -> Stempel +0x0B @0x80042fc4 und Handler
@0x80042fc8-43010, Schleife weiter @0x8004301c (letzter Treffer gewinnt). Der Port fuehrte den Wasser-Stempel nur fuer
Aktoren (aot_common.c:1329-1370, "DEFERRED ... NOTHING consumes a prop water level") — FUN_8002c444 IST dieser Leser.
(Nebenbei: re15_aot.h:357ff. nennt @0x8002c154 "fuer den Spieler" — es ist der Objekt-Pass; nicht Teil dieser Spur.)

**B7 — Schreiber-Zensus obj+0x82 (berichtigt, Mangel 1)** — Liste `A_belege/nb2_scans.txt` neu eingeordnet:

| Adresse | Funktion | Ziel | Objekt-Band? |
|---|---|---|---|
| @0x80040974 | Installer LAB_80040914 (Obj_model_set) | Objekt-Pool | ja (beim Laden) |
| @0x80041228 | Member-Schreiber FUN_8004116c Eintrag 18 (Member_set/Member_set2) | Work-Entity | ja (SCD) |
| **@0x8002c6c4** | **FUN_8002c444 = Fall eines Typ-4-OBJEKTS** (einziger Aufrufer @0x8002c42c) | **Objekt-Pool** | **ja, ZUR LAUFZEIT** |
| @0x800421d0 | Sce_em_set (Spawn-Opcode 0x44) | Aktor (+448/+454-Felder) | nein |
| @0x8001be54 | FUN_8001bd60 (nur aus Stage-Overlays) | Aktor (+442/+448) | nein |
| @0x8005276c / @0x80052dd0 | g_entity(cur)-Pfade | Aktor (+442) | nein |
| @0x8001f1b4 / @0x8001f254 | Halbwort, Effekt-Struktur (+152..+156) | Effekt | nein |
| STAGE1.BIN (11 Stellen) | g_entity(cur) Aktorfelder bzw. @0x80108664 Grossstruktur (+1321) | Aktor | nein |

Falsch eingeordnet war genau @0x8002c6c4 ("Spieler-Fall", Nachbesserung 2 Beleg 3); @0x800421d0 fehlte.

### Nachbesserung 3 — Umsetzung

| Datei | Aenderung |
|---|---|
| `engine/src/objfall_r36.c` (NEU) | Fall-Zustand je Prop-Platz (obj+4/+5/+6/+0x88) und die GLOBALEN Fallgroessen (0x8008f67c..6a0, 0x800b526c/27c) wie im Original; Typ-Handler-Pass (B1), FUN_8002c444 (B4), FUN_8002d3d8 (B3), FUN_8002dacc mit libgte-Zwillingen (B5), Ausloeser + obj+0x88-Nullung (B2), Wasser-Stempel der Objekte (B6). Spritzer = `re15_esp_fx_spawn_rows(Raum-Bank, 6, 2, 0x2000, P, Boden 0x8008f694, Gier)` (= FUN_80019700-Port mit Start-Flags 0x03; Boden-Ebene der Port-Sammelklemme = die FUN_8001c6e8-Abfrage des Falls). Toene = `re15_audio_room_se_snd0(6/8)` (= FUN_80045024 Bank 2). Messschrieb `RE15_OBJFALL_LOG=<datei>` (env, kein Verhalten). |
| `include/re15_objfall_r36.h` (NEU) | Schnittstelle + Herleitung |
| `engine/src/game_step_common.c` | 3 Zeilen: Include; `re15_objfall_vor_schub` VOR `re15_prop_push_tick` (Normalzweig = Objekt-Pass-Anfang); `re15_objfall_nach_pass` hinter `re15_actor_prop_pushout` (jedes Bild, Objekt-Pass-Ende + AUTO-Scan-Stempel; holt den Handler nach, wenn der Zweig keinen Schub fuhr) |
| `engine/src/scd_room_setup.c` | 2 Zeilen: Include; `re15_objfall_install` am Ende des Installer-Blocks ("Runde 36 Spur A") |

Port-Wahl (begruendet, kein Verhaltenswert): Raumaufbau setzt den Fallzustand auf 0. Das Original schreibt obj+4/+5/+6
beim Installieren NICHT (Installer-Stores nur +0x00/08/09/0C/34-3C/68-6C/74/78/82, @0x8004095c-80040aa0; Raumlader nullt
nur +0x00 @0x8003eab0-cc) — ein laufender Fall wuerde den Raumwechsel ueberleben; das braucht einen Tuerdurchgang waehrend
der ~25 Fallbilder vor dem Spieler (kein Spielweg).

Fallfaehige Objekte der Auslieferung (Zensus `A_belege/nb3_absprung_zensus.txt`, alle 240 RDTs: Typ-4-Objekt + Zelle mit
Wort & 2 im Objekt-Band): ROOM5020/5021 (Band 4, Zelle x -7675..-7175 z -26275..-24025, Wort 0x4002), ROOM20A0/20A1 (Band 4,
zwei Zellen z -22000..-21000, x 8000..14700 und 15800..17600, Wort 0x4206), ROOM3050/3051 obj 1 (Band 4, x -25000..-22950
z -18050..-16800, 0x4002; Fall-Lage @0x01F40, Latch (3,93)), ROOM1090/1091 obj 1 (Band 5, Wort 0x530e), ROOM2050/2051 obj 6
(Band 3, 0x3202), ROOM11A0/11A1 (Band 3, 0x3102). Alle mit rot_y 0 oder 2048 (SquareRoot12-Bereich, B5).

### Nachbesserung 3 — Messung nachher, ROOM5020 an der exe (echter Weg)

Lauf `m5_nachher` (gleiche Eingabe wie vorher, `RE15_OBJFALL_LOG`): Ausloeser bei Kistenmitte x -8078 (>= -8125 = Zelle
-7675 - 450, wie von der Abnahme vorhergesagt), Wort 0x4002, Richtung (+1, 0) = weg vom Spieler (x -9428). Kippen 13 Bilder,
Bild 13 Boden FUN_8001c6e8 = 0 -> Band 4 -> 0, freier Fall, Landung Bild 22 auf y 0, Aufsetzen (snd0:6), Nachfedern, obj+4 = 0;
Endlage (-5169, 0, -25332), Drehung um Z 1029 (Summe der Winkelschritte 1024, Rest = Rundung RotMatrix/ratan2 je Bild).

Lauf `l5020a` (ECHTER WEG in EINEM Stueck, `A_belege/nb3_5020_echter_weg.txt`): `RE15_DEBUG_JUMP=5020@250
RE15_PLAYER_POS=-11000,-21800,1024,4 RE15_INPUT_SCRIPT=W8,U5,W0.5,R0.37,W0.3,U2.0,W0.3,L0.37,W0.3,U1.8,W0.3,L0.37,W0.5,
U8,W1,L0.37,W0.3,U2.2,W0.3,L0.37,W0.3,U2.45,W0.3,R0.37,W0.3,U0.3,W0.3,A0.1,W7,R0.37,W0.3,U1.2,W0.3,R0.37,W0.3,U5,W0.3,
L0.37,W0.3,U9,W1,A0.1,W4 RE15_EXIT_AT=2700#5020`:
```
F785  AUSLOESER wort=0x4002 band=4 pos=(-8078,-7200,-25332) spieler=(-9428,-25807) richt=(1,0)
F798  BODEN FUN_8001c6e8=0 band 4 -> 0 pos=(-6847,-6814,-25332)
F807  LANDUNG y=0 band=0 pos=(-5209,-25332)       F808 AUFSETZEN se=snd0:6      F809 zust 3 -> obj+4 = 0
F1125..F1245  Spieler nimmt die Treppe (Slot 2 sce 13) von Band 4 nach Band 0
F1665 spieler=(-6519,0,-24966) band=0 -> Schub +X auf Band 0
F1712 FLAG (4,203)=1   KERBE m0b=4 pos=(-4393,0,-25332) band=0     <- Latch sub01 @0x012F4-302 (Slot 4 Floor 0)
climb.txt: T2138 sub=9 kind=0x81 obj=0 ... T2951 pos=(-3522,-1800,-25212) band=1 stand=0   <- auf die GEFALLENE Kiste geklettert
```

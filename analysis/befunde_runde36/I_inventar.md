# Runde 36 — Spur I "inventar" (Dossier, fortlaufend)

Baum: `.claude/worktrees/r36_inventar`, Zweig `r36/inventar`, Basis master 78a4a748.
Zuteilung (VERTRAG.md §0/§1.4): I = inventory_common.c + re15_savepoint.c + Debug-Menue; keine Bits,
keine Nachrichten-IDs, keine Ereignis-Nummern.

## Auftrag (woertlich, AUFTRAG.md Z. 28/29)

> - Bei den Debug Menu, bei den man zwischen die Räume springt, schickst du, wenn man mit Elza spielt Elza
>   in Leons Räume. Wenn man mit Elza spielt, sind die Räume mit "1" hinten korrekt, zum Beispiel ROOM 1071.
> - Mit Nutzung von Memory Cards beim Speichern soll sich die Anzahl reduzieren. Sind Memory Cards oder
>   Handgranaten 0 sollen sie aus dem Player inventory verschwinden

Punkte:
* **I1** Memory Card: Speichern verbraucht 1 (Anzahl -1).
* **I2** Memory Card bei 0 -> Slot verschwindet aus dem Inventar.
* **I3** Handgranaten bei 0 (letzter Wurf) -> Slot verschwindet aus dem Inventar.
* **I4** Debug-Menue Raumsprung: bei Elza die xxx1-Raeume (z.B. ROOM1071), nicht Leons.

## Stand

**FERTIG (2026-10-11):** I1-I4 gemessen, belegt, gebaut, nachgemessen; Riegel unit_r36_inventar +
integration_r36_inventar gruen, gezielte Laeufe 35/35 (unit) + 10/10 (integration) gruen (Abschnitt Tests).

Kurzfassung:
* I1/I2 Memory Card: Speichern zieht 1 ab, die letzte verschwindet (RE2 MEM_CARD.BIN @0x801c118c-0x801c11e0;
  Abzug im geschriebenen Block, Live-Inventar erst nach Erfolg; Platz weg = @0x8004aee4-af2c + FUN_8004dadc).
* I3 Handgranaten 0x09/0x0A/0x0B: bei 0 sofort aus dem Inventar (NUTZER-VORGABE, RE1.5 entfernt sie nicht:
  FUN_8004eae4 @0x8004eb44-60), ausgeruestet -> abgelegt (0x80, @0x8004ad44-4c), Waffe -> Messer nach dem Wurf
  (Commit-Regel @0x80046654-88). Leere Memory Cards aus jeder Quelle ebenso.
* I4 Debug-Menue: Sprungziel = Basis | Elza-Bit (@0x800397e4 `srl a0,a0,31` / @0x800397ec `addu`), Elza
  landet in ROOM1071/1151 statt 1070/1150.

Verlauf: Baum sauber auf 78a4a748 gestartet (Fortsetzung: kein Vorgaenger-Commit vorhanden), Baseline
gebaut (`local_build.sh configure` + `build`, LOCAL-BUILD-OK). Messwerkzeug:
`re15_port/tools/r36_i/messlauf.sh <name> <timeout> KEY=WERT ...` (Messkopie `re15_pc_r36i.exe` neben der
exe, Arbeitsordner `re15_port/build/r36i_mess/<name>/`).

## Messung vorher

### I4 Debug-Menue mit Elza (Lauf `i4_vorher`, Stand 78a4a748)
`RE15_NO_INTRO=1 RE15_NOAUDIO=1 RE15_TITLE_CONFIRM_MS=1500 RE15_PSELECT_AUTO=1 RE15_PSELECT_AUTO_SWITCH=1
RE15_DEBUG_JUMP=1070@250 RE15_EXIT_AT=400` (Spielerwahl schaltet auf Elza, Sprung ueber den Menue-Pfad
re15_debug_menu_tick = Quadrat in der JUMP-Zeile). debug.log:
```
[pl] Spieler-Familie PL04 (character=4, Elza-Bit=1)
[debug-menu] AUTO-JUMP -> ROOM1070 (Frame 250)
[debug-menu] JUMP -> 107 LOBBY OFFICE   (ROOM1070) spawn=(850,0,3600) cut=0
[room] PC loaded room1070.rdt (153328 bytes)
[flow] EXIT_AT: Bild 400 in Raum 1070 erreicht -> exit
```
**REPRODUZIERT:** Elza (Elza-Bit 1) landet in Leons ROOM1070.RDT. Ursache im Code: platform/pc/main.c:7395
`unsigned droom = (unsigned)dm->load_room << 4;` — die Variante (unterste Hex-Ziffer) ist immer 0.

### I1/I2 Memory Card beim Speichern (Lauf `i1_vorher_karte`, Stand 78a4a748)
Spielstand vom Kartenwerkzeug `probe_r33_speichern_karte re15_card.mcr karte` (ROOM1150 vor dem Telefon,
Inventarplatz 0 = Memory Card x1), Weg wie integration_r33_speichern B: `RE15_CONTINUE_TEST=1
RE15_CARD_AUTO=1 RE15_CARD_SLOT=0,1 RE15_MSG_LOG=1 RE15_PAD_AT=150:A,200:A,290:A,370:A
RE15_EXIT_AT=460#1150`. debug.log:
```
[save] room=1150 msg=1 card_slot=0 -> FRAGE (Memory Card benutzen?)
[save] Antwort: JA -> Speicherbildschirm
[save] saved (room 1150) slot n=2 -> next=3; card=kept (slot 0 qty 1)
```
Geschriebener Block (Platz 1, Inventar @Datei 0x4128): `21 01` = Memory Card x1.
**REPRODUZIERT:** Speichern verbraucht die Karte nicht (Runde-33-PORT-WAHL "kein Verbrauch",
main.c:5056-5058 / ee17ca0a), die Anzahl bleibt 1.

### I3 letzte Handgranate (Lauf `i3_vorher`, Stand 78a4a748)
`RE15_TITLE_SHOT=title.bmp RE15_TITLE_SHOT_AF=2 RE15_DEBUG_JUMP=1140@250 RE15_PLAYER_POS=-4311,-19289,0
RE15_AI_FLAVOR=re2 RE15_GIVE=9:1 RE15_EQUIP=9 RE15_INPUT_SCRIPT_BASIS=spiel RE15_INPUT_SCRIPT_START=1
RE15_INPUT_SCRIPT=M0.6,MA0.2,M2.5,W6,S0.1,W3 RE15_FRAMEDUMP=300-400/20:inv RE15_EXIT_AT=400#1140`
(Wurf-Rezept von integration_r35_granate "wand", aber EINE Granate; START erst bei F280 > Sprungbild 250,
damit es nicht schon im Startraum als klebrige Anforderung liegen bleibt — Messfalle des ersten Laufs:
"S" im Startraum oeffnete das Inventar in ROOM1140 bei F15 und verschluckte den Wurf).
state.log (ROOM1140, mg = Menge des ausgeruesteten Platzes):
```
F15 pad=0800 ac=8 mg=1     (Zielen)
F19 pad=8800 ac=7 mg=0     (Abzug: FUN_8004eae4 zieht im Abzugsbild ab)
F41 ... fx=1 mg=0          (Projektil Rueckstoss-Bild 22)
F100..F125 fx 2..4..0      (Explosion)
F280 pad=0008 mg=0         (START)
```
Statusschirm F340 (`I_belege/i3_vorher_statusschirm_F340.png`): "Equip Arms" = Handgranate mit **0**,
Item list Platz 2 = Handgranate mit **0**. **REPRODUZIERT:** die leere Granate bleibt im Inventar und
ausgeruestet.

## RE-Belege

Alle Zeilen selbst disassembliert (`.claude/skills/re15-psx-disasm/scripts/re15_disasm.py` fuer RE1.5
PSX.EXE/TITLE.BIN; RE2 MEM_CARD.BIN mit `re2_disasm.dis_one` auf Ladeadresse 0x801C0000, roh ohne Kopf,
`info/re2leon/COMMON/BIN/MEM_CARD.BIN`, 23932 Bytes).

### I4 — der Raumlader waehlt die Datei aus Charakter-Bit, nicht aus dem Menue
RE1.5 FUN_800396fc (Raumlader):
```
800397b8  lw   a0,-13764(a0)     ; DAT_800aca3c (Lebenszyklus-Wort, Bit 31 = Elza)
800397cc  addiu at,at,17292      ; Stage-Zeigertabelle 0x8007438c
800397e0  lhu  v0,0(v1)          ; Basis-Dateiindex [Stage][Raum]  (Raum = DAT_800b0fe2)
800397e4  srl  a0,a0,31          ; Bytes c2 27 04 00 — Elza-Bit 0/1
800397e8  jal  0x80013b60        ; CD-Datei laden
800397ec  addu a0,a0,v0          ; Bytes 21 20 82 00 — Index = Basis + Elza-Bit (Delay-Slot)
```
Stage-0-Tabelle @0x8007429c = 681, 684, 687, 690, 693, 696 (Schritt 3 = BSS + Leon-RDT + Elza-RDT je Raum).
Der Debug-Sprung laeuft durch DENSELBEN Lader — JUMP-Executor FUN_8001d600, Debug-Zweig
(`bne v0,zero` @0x8001d618 auf DAT_800ac9a8 == 0):
```
8001d654  lbu  v0,0(at)          ; (&0x800bbe5f)[Stage] = Menue-Raumindex
8001d660  sh   v0,4066(at)       ; -> DAT_800b0fe2 (Raum)
8001d808  sh   v0,4064(at)       ; -> DAT_800b0fe0 (Stage, nur bei Wechsel)
8001d818  sb   zero,-1099(at)    ; Cut 0
8001d820  sh   zero,4068(at)     ; Szenario 0
8001d824  j    0x8001d988
8001d988  jal  0x800396fc        ; Raumlader (Elza-Bit oben)
```
Das Menue kennt nur Stage/Index; die Variante kommt allein aus dem Elza-Bit. Gesetzt wird es von der
Spielerwahl (TITLE.BIN, `--bin TITLE.BIN`): Leon `lui v1,0x7fff` @0x801024ac / `ori v1,v1,0xffff`
@0x801024b8 / `and v0,v0,v1` @0x801024c8 + `sb zero,-13732(at)` @0x801024c0 (aca5c = 0); Elza
`lui v1,0x8000` @0x801024a8 / `or v0,v0,v1` @0x801024e4 + `ori v0,zero,0x4` / `sb` @0x801024cc/d4
(aca5c = 4). Der Port fuehrt dafuer bereits `re15_char_variant()` = (character & 4) ? 1 : 0 und
`re15_room_for_char(base)` = (base & 0xFFF0) | Variante (include/re15_gameflow.h:69-84, gleiche Belege).
Zensus (scratchpad dbg_tbl.py ueber DEBUG.BIN @0x263a, 6 x 49 Saetze): **101 belegte Sprung-Saetze, fuer
alle 101 liegen ROOMxxx0.RDT UND ROOMxxx1.RDT** in shared_assets/PSX/STAGEn — kein Elza-Ziel fehlt.

### I1/I2 — RE2 verbraucht das Farbband beim Schreiben, bei 0 verschwindet der Platz
RE2 MEM_CARD.BIN, Schreibzweig des Speicherschirms:
```
801c1168..84  memcpy(sp+0x1F0, 0x800D4A3C, 44)     ; Inventar (11 Plaetze) SICHERN (jal 0x80076a00, a2=44)
801c1188      lbu t1,-895(fp)                      ; 0x800D46BD sichern
801c118c      addiu a0,zero,30                     ; Item 0x1E = Ink Ribbon
801c1190      jal 0x800696cc                       ; Platz suchen (nur Inventar)
801c11a4      lbu v0,19005(at)                     ; Menge (0x800D4A3D + Platz*4)
801c11ac      addiu v0,v0,-1                       ; Menge - 1
801c11b8      sb v0,19005(at)
801c11c0      bne v0,zero,0x801c11e8               ; > 0 -> fertig
801c11d0      sb zero,19004(at)                    ; == 0: Id := 0
801c11dc      sb zero,19006(at)                    ;       Byte +2 := 0
801c11e0      jal 0x80069714                       ;       Inventar verdichten
801c11e8..00  0x800D46C4 += 1                      ; Speicherzaehler
...           Spielstand bauen + schreiben (jal 0x801c3678)
801c128c      beq v0,zero,0x801c12c8               ; Schreiben ok -> weiter
801c1290..bc  memcpy(0x800D4A3C, Sicherung, 44); Zaehler -1; 46BD zurueck   ; FEHLER -> alles zurueck
```
= das Farbband faellt NUR beim tatsaechlichen Schreiben, der GESCHRIEBENE Stand traegt die neue Menge (der
Abzug liegt vor dem Bau des Blocks), ein Fehlschlag gibt es zurueck; Abbrechen im Speicherschirm erreicht
den Zweig nie. RE2 FUN_80069714 (RE2_Quellcode_V2/FUN_80069714.c): ersten freien Platz suchen
(FUN_80069668(0)), Rest um eins nachruecken, Ausruest-Indizes DAT_800d5bf8/DAT_800d69f4 mitziehen.
RE1.5-Gegenstueck derselben Form (fuer die Port-Seite massgeblich, Port-Funktion re15_inv_remove_slot):
```
8004aee4..af28  sb zero -> 0x800b10ac/ad/ae [Platz]   ; Verbrauch beim Benutzen: Id/Menge/Flag := 0
8004af2c        jal 0x8004dadc                        ; verdichten
8004daec        jal 0x8004df2c                        ; FUN_8004dadc: erster freier Platz
8004db4c..dba8  Id/Menge/Flag s0 -> s0-1             ; nachruecken
8004dbac..dbc8  lbu 25c8 / bne v0,s0 / addiu -1 / sb  ; Ausruest-Platz zieht mit
8004dbec        jal 0x8004947c                        ; letzte Zelle leeren
```
Runde 33 hatte genau diesen RE2-Block auf Nutzerwunsch WEGGELASSEN (speichern_memory_card.md §1.5,
ee17ca0a); jetzt bestellt der Nutzer den Verbrauch -> RE2-Form wieder herein, Memory Card (Item 0x21) statt
Ink Ribbon (0x1E).

### I3 — RE1.5 laesst die leere Granate liegen; die Entfernung ist NUTZER-VORGABE in RE2/RE1.5-Form
* Granaten-Entladung 0x80033B38: `jal 0x8004eae4` @0x80033b40 (sonst nichts; 0x80033B58/78 fuer 10/11
  bytegleich). FUN_8004eae4: `lbu v0,0(at)` @0x8004eb3c (Menge), `bne v0,zero` @0x8004eb44, `addiu v0,v0,-1`
  @0x8004eb48, `sb v0,0(at)` @0x8004eb60 — KEIN Entfernen bei 0. Genau das zeigt die Messung (Platz 2 =
  Granate 0).
* RE2 hat keine Handgranaten; sein Munitionsabzug FUN_8006a0cc (RE2_Quellcode_V2/FUN_8006a0cc.c) entfernt
  ebenfalls nie. RE2s Regel "Menge 0 -> Platz weg" gilt fuer VERBRAUCHSGEGENSTAENDE: Farbband (oben,
  @0x801c11c0-e0) und Schluessel (LAB_80051718 @0x80051810 `addiu v0,v0,-1` -> bei 0 Wegwerf-Abfrage).
  -> "Handgranaten bei 0 weg" = **NUTZER-VORGABE**; FORM = Farbband-Regel / RE1.5-Verbrauch
  (@0x8004aee4-af2c + FUN_8004dadc), d.h. re15_inv_remove_slot.
* War die leere Granate ausgeruestet: ABLEGEN wie im Statusschirm, UNEQUIP @0x8004ad44-4c
  (`ori v0,zero,0x80` / `sb v0,9672(at)` -> 25c8 := 0x80), danach die Commit-Regel @0x80046654-88:
  ```
  80046658  lbu v1,9672(v1)    ; 25c8
  8004665c  ori v0,zero,0x80
  80046660  bne v1,v0,...
  8004666c  ori v0,zero,0x1    ; 0x80 -> Waffe 1 (Messer, Rueckfall Runde 35 Spur E)
  80046688  sb  v0,-13731(at)  ; DAT_800aca5d
  80046694..a0  player &= ~0x4000            ; Messer-in-der-Hand-Latch weg
  800466b0  jal 0x80036b68 / 800466c4 jal 0x80043d8c(aca5d, 0x80198000)   ; Modell + Bank
  ```
  Port: re15_player_set_equipped_weapon + re15_audio_prime_weapon (menu_common.c:1509-1516).
* WARUM der Commit selbst gesetzt werden muss: das Original committet NUR beim Schliessen des Statusschirms
  und nur, wenn sich die Id unter 25c8 gegen die Momentaufnahme 25ce aendert (@0x800465f4-0x8004661c
  `lbu 25c8` / `lbu 25ce` / `beq`). Mit 25c8 = 0x80 beim Oeffnen UND Schliessen ist das immer gleich
  (Port: equip_id_now() -> Waechter 0xff, menu_common.c:159-164) — ohne eigenen Commit behielte Leon die
  Granate (aca5d = 9) dauerhaft in der Hand.
* WANN: der Wurf liest die Waffe JEDES Bild (`lbu 0x800aca5d` @0x80032e60 -> Dispatch) und das Projektil
  spawnt nur bei aca5d == 9 (`lbu` @0x80033684 / `ori v0,zero,0x9` @0x80033688 / `bne` @0x8003368c), im
  Rueckstoss-Bild 19/22/24 — ~22 Bilder NACH dem Abzug (gemessen F19 Abzug, F41 Projektil). Der Platz faellt
  deshalb sofort (nichts Leeres im Statusschirm, auch wenn START mitten im Wurf kommt), die Waffen-Id
  wechselt aber erst, wenn der Ziel-Zustand verlassen ist (re15_player_aim_active() == 0) — PORT-WAHL
  des Zeitpunkts, die den Wurf byte-true zu Ende laufen laesst.

## Umsetzung

Neue Dateien:
* `re15_port/include/re15_inventar_r36.h`, `re15_port/engine/src/inventar_r36.c` — die ganze Logik:
  * `re15_inventar_r36_karte_im_block(sd)`: Memory Card im ZU SCHREIBENDEN Block -1 (`addiu v0,v0,-1`
    @0x801c11ac), bei 0 Id/Flag := 0 (@0x801c11d0/@0x801c11dc) + Verdichten mit Ausruest-Platz
    (@0x801c11e0 / FUN_8004dadc @0x8004dbac-c8), Pruefsumme neu.
  * `re15_inventar_r36_karte_verbrauchen()`: derselbe Abzug im Live-Inventar NACH erfolgreichem Schreiben
    (re15_inv_remove_slot = @0x8004aee4-af2c + FUN_8004dadc). Abbruch/Fehler im Speicherschirm -> nichts
    verbraucht (RE2: der Zweig wird nie erreicht bzw. @0x801c1290-12bc nimmt ihn zurueck). Protokoll
    `[inventar-r36] Memory Card verbraucht: Platz k Rest n | Inventar ...`.
  * `re15_inventar_r36_tick()` je Spielbild unmittelbar VOR dem START-Poll: (a) jeder Platz mit Memory Card
    (0x21) oder Handgranate (0x09/0x0A/0x0B) und Menge 0 wird entfernt (war er ausgeruestet: erst 25c8 :=
    0x80 wie UNEQUIP @0x8004ad44-4c); (b) ist danach nichts ausgeruestet, die Waffen-Id aber noch eine
    Granate, und ist der Wurf vorbei (re15_player_aim_active() == 0 und kein Granatenplatz mehr im
    Effekt-Pool — die ARMS-Bank der Granate traegt den Abprall-Ton Satz 0x0A @0x80018410-28), dann
    Commit-Regel 0x80 -> Messer (@0x8004666c/@0x80046688) + Bank (@0x800466c4). Laeuft nicht bei offenem
    Statusschirm (re15_menu_gameplay_frozen). Protokoll `[inventar-r36] F<n> Platz ... entfernt` /
    `[inventar-r36] F<n> Waffe 9 (leer, abgelegt) -> 1`.
* Tests: `re15_port/tests/unit/test_r36_inventar.c`, `re15_port/tests/unit/probe_r36_inventar_karte.c`
  (Kartenwerkzeug), `re15_port/tests/integration/test_r36_inventar.cmake`, Registrierung
  `re15_port/tests/unit/probes/r36_inventar.cmake`.
* Messwerkzeug `re15_port/tools/r36_i/messlauf.sh`.

Haken in gemeinsamen Dateien:
* `engine/src/game_step_common.c` (2 Zeilen, direkt vor `re15_menu_start_poll`): `re15_inventar_r36_tick()`.
* `platform/pc/main.c`: Include; Speicherfluss `mc = re15_inventar_r36_karte_im_block(&sd)` vor
  `pc_run_memcard_screen`, nach Erfolg `re15_inventar_r36_karte_verbrauchen()`, Log `card=used` /
  `card=used-last, slot removed` statt `card=kept` (der Runde-33-Kommentar "KEIN Kartenverbrauch" ist
  ersetzt); Debug-Sprung `droom = re15_debug_menu_ziel_id()` statt `load_room << 4`.
* `engine/src/debug_menu_common.c` + `include/re15_debug_menu.h` (Spur I = Debug-Menue):
  `re15_debug_menu_ziel_id()` = `re15_room_for_char(load_room << 4)` (Elza-Bit @0x800397e4/ec).

Konstanten (alle mit Beleg): Item 0x21 (Namenstabelle DEBUG.BIN @0x800C4BEA), Granaten 0x09..0x0B (Entlade-
Handler 0x80033B38/58/78), 0x80 = nichts ausgeruestet (@0x8004ad44 / @0x8004665c), 1 = Messer
(@0x8004666c). Zeitpunkt des Waffenwechsels = PORT-WAHL (nach dem Wurf, s. RE-Belege I3 "WANN").

Angepasste Riegel (Verhalten auf Nutzerwunsch geaendert):
* `tests/integration/test_r33_speichern.cmake` Lauf B: statt "card=kept (slot 0 qty 1)" / Block `21 01`
  jetzt "card=used-last, slot removed (slot 0 qty 0)" / Block `00 00` (die eine Karte ist verbraucht);
  Kommentar in `tests/unit/probes/r33_speichern.cmake` nachgezogen.
* `tests/integration/test_r35_werfer_elza.cmake`: Elza springt per `RE15_DEBUG_JUMP=1000` jetzt in ROOM1001
  -> `RE15_EXIT_AT=150#1001` statt `#1000` (sonst haengt der raumgebundene Ausstieg).

## Messung nachher

Alle Laeufe mit der gebauten exe (Messkopie), Arbeitsordner `re15_port/build/r36i_mess/<name>/`.

### I4 Debug-Menue (Laeufe `i4_nachher`, `i4_nachher_1150`, `i4_nachher_leon`)
```
i4_nachher       Elza: [debug-menu] JUMP -> 107 LOBBY OFFICE   (ROOM1071) ... [room] PC loaded room1071.rdt
i4_nachher_1150  Elza: [debug-menu] JUMP -> 115 CHIEF OFFICE   (ROOM1151) ... [room] PC loaded room1151.rdt (202392 bytes)
i4_nachher_leon  Leon: [debug-menu] JUMP -> 115 CHIEF OFFICE   (ROOM1150) ... [room] PC loaded room1150.rdt (194080 bytes)
```
Vorher landete Elza in ROOM1070 (i4_vorher). Hinweis: ROOM1070.RDT und ROOM1071.RDT sind BYTEGLEICH (71 der
120 Raumpaare sind es, 49 unterscheiden sich, z.B. 1150/1151 194080 vs 202392 Bytes) — sichtbar wird der
Fehler deshalb in den 49 verschiedenen Raeumen und ueberall dort, wo der Port an der Raum-Id haengt
(Elza-Zweige xxx1, z.B. Spur L ROOM1121/1151). Der Nutzer-Weg (SELECT, Raum waehlen, VIERECK) ist derselbe
Code-Pfad (re15_debug_menu_tick mit Lade-Flanke @0x80014A38).

### I1/I2 Memory Card (Laeufe `i1_nachher_s2`, `i1_nachher_s1`; Kartenwerkzeug probe_r36_inventar_karte)
Karte x2 (Platz 0), Browning ausgeruestet (Platz 1), Patronen (Platz 2):
```
[inventar-r36] Memory Card verbraucht: Platz 0 Rest 1 | Inventar 21:1 03:15 15:50 00:0 ... | Ausruest-Platz 0x01
[save] saved (room 1150) slot n=2 -> next=3; card=used (slot 0 qty 1)
Block @0x4128: 21 01 00 00 03 0f 00 00 15 32 00 00
```
Statusschirm F520: Memory Card mit "1" (`I_belege/i1_nachher_karte_x2_zu_x1_statusschirm_F520.png`, Ausschnitt
`I_belege/i1_nachher_karte_x1_zoom.png`).
Letzte Karte x1:
```
[inventar-r36] Memory Card verbraucht: Platz 0 Rest 0 -> Platz entfernt | Inventar 03:15 15:50 00:0 ... | Ausruest-Platz 0x00
[save] saved (room 1150) slot n=2 -> next=3; card=used-last, slot removed (slot 0 qty 0)
```
Statusschirm F520 (`I_belege/i2_nachher_letzte_karte_statusschirm_F520.png`): keine Memory Card, Browning
(15) rueckt in Platz 0 und bleibt ausgeruestet (Equip Arms = Browning).

### I3 letzte Handgranate (Lauf `i3_nachher`, Rezept wie `i3_vorher`)
```
F19 pad=8800 mg=0          Abzug (FUN_8004eae4-Zwilling zieht ab)
[inventar-r36] F20 Platz 2 Id 09 Menge 0 -> entfernt (Ausruest-Platz 0x80, Waffe 9)
F20 mg=-1                  (nichts ausgeruestet)
F41 fx=1                   Projektil gespawnt (gr.log "SPAWN granate art=2") — der Wurf laeuft zu Ende
F100..F125                 Explosion
[inventar-r36] F111 Waffe 9 (leer, abgelegt) -> 1 (Messer-Rueckfall 0x80)
[equip] W-bank -> W01 (Clips 14, Rueckstoss-Clip7 fc=25)
```
Statusschirm F340 (`I_belege/i3_nachher_statusschirm_F340.png`): Item list nur Browning + Patronen, KEINE
Granate; Equip Arms = Messer (Darstellung fuer 0x80, Runde 35 Spur E). Vorher: Granate "0" in beiden Kaesten.

### Gegenprobe Brandgranate (Lauf `i3_nachher_brand`, RE15_GIVE=11:1 RE15_EQUIP=11)
```
[inventar-r36] F20 Platz 2 Id 0b Menge 0 -> entfernt (Ausruest-Platz 0x80, Waffe 11)
[inventar-r36] F111 Waffe 11 (leer, abgelegt) -> 1 (Messer-Rueckfall 0x80)
gr.log: SPAWN granate art=4 (1x)
```
Saeure-/Brandgranate laufen durch denselben Abzug (Entlade-Handler 0x80033B58/78) und denselben Tick.

## Tests

Neue Riegel (Registrierung `tests/unit/probes/r36_inventar.cmake`):
* **unit_r36_inventar** (`tests/unit/test_r36_inventar.c`) — MECHANIK je Nutzer-Punkt:
  (A) letzte Memory Card: Block-Abzug == Live-Abzug Platz fuer Platz, verdichtet, Ausruest-Platz zieht mit,
  Pruefsumme neu; (B) x3 -> x2; (C) ohne Karte nichts; (D) leere ausgeruestete Handgranate -> weg, 0x80,
  Messer; (E) leere Saeure-Granate + leere Karte -> beide weg, Browning-Index zieht zweimal mit; (F) Granate
  im Effekt-Pool (CORE00.ESP) -> Platz sofort weg, Waffe erst nach dem Effekt; (G) volle Granate bleibt;
  (H) Debug-Menue ueber re15_debug_menu_tick mit Lade-Flanke: alle 101 belegten Sprung-Saetze x 2 Charaktere
  -> Leon xxx0 / Elza xxx1, Datei vorhanden; Elza 107 -> ROOM1071.
* **integration_r36_inventar** (`tests/integration/test_r36_inventar.cmake`, echte exe, Messkopie
  `re15_pc_r36i_haken_*.exe`): s2 Karte x2 -> x1 (Log + Kartenblock), s1 letzte Karte -> Platz weg, Browning
  rueckt nach und bleibt ausgeruestet (Block 03:15 15:50, Platz 0, Waffe 3), g letzte Handgranate: Projektil
  gespawnt, Platz weg F20 (nach Abzug F19, vor START F280), Messer ab F111, Bank W01; e1 Elza -> ROOM1151,
  e0 Leon -> ROOM1150. Diskriminiert gegen den Vorher-Stand (gemessen i1_vorher_karte / i3_vorher /
  i4_vorher: "card=kept", Granate 0 bleibt, Elza ROOM1070/1150).

Angepasst: integration_r33_speichern (B: letzte Karte verbraucht, Block 00 00), integration_r35_werfer_elza
(EXIT_AT#1001).

Gezielte Laeufe (KEINE volle Suite, Vertrag), woertlich:
```
ctest --test-dir re15_port/build -R "^(unit_debug_menu|unit_savedata|unit_savepoint|unit_inv_fsm|unit_inv_grant|unit_item_use|unit_item_modal|unit_item_icon|unit_item_stack_press|unit_r33_speichern|unit_r34_wurf|unit_r35_granate|unit_r30_granate|unit_r35_inventar1050_.*|unit_r35_werfer.*|unit_r36_inventar|unit_load_after_death|unit_elza_zweig|unit_r30_elza_selbsttuer|unit_itembox_.*|unit_room1150_itembox|unit_r26_inventar|unit_r21_discard_wegwerfen|unit_map_speichern_laden|unit_r33_karte_speicher|unit_r34n_d_adaruf_speicher)$" --timeout 600
100% tests passed, 0 tests failed out of 35

ctest --test-dir re15_port/build -R "^(integration_r33_speichern|integration_r35_werfer_elza|integration_r36_inventar|integration_save_counter_pin|integration_r35_granate|integration_r34_granaten|integration_r30_granate_laden|integration_elza_vollstart|integration_r35_inventar1050|integration_weste_load_pin)$" --timeout 900
100% tests passed, 0 tests failed out of 10
```
(Erster Lauf von integration_r36_inventar rot an MEINEM Pruefstand: Block-Vergleich mit 2-Byte-Plaetzen statt
4-Byte-Schritt {Id, Menge, Flag, +3}; Soll korrigiert, danach gruen.)

## OFFEN

* Keine offenen Punkte aus I1-I4.
* Hinweis (nicht Spur I): der Zeitpunkt des Waffenwechsels nach der letzten Granate ist PORT-WAHL (Ende des
  Zielzustands + Granatenplatz verschwunden); das Original wechselt die Waffe nur im Statusschirm. Wer ihn
  anders will (z.B. sofort nach dem Projektil auch bei gehaltenem R1), aendert nur Bedingung (b) in
  `re15_inventar_r36_tick`.
* Kistenplaetze (ITEM BOX) werden nicht geraeumt: der Auftrag nennt das Spieler-Inventar; eine 0er-Granate kann
  dort ohnehin nicht mehr entstehen (sie verschwindet vorher aus dem Inventar).

## Fuer den Nutzer

* **Neue Assets fuer das Paket-Gate:** keine (kein shared_assets/-Zuwachs, keine gen/*.inc).
* **Sprachdateien:** keine.
* **Bedienhinweise:**
  - Speichern am Telefon/Computer verbraucht jetzt eine Memory Card (Anzeige im Statusschirm faellt von 2 auf
    1; die letzte Karte verschwindet). Wer den Speicherschirm abbricht, behaelt die Karte (RE2-Verhalten).
  - Wirft Leon/Elza die letzte Hand-/Saeure-/Brandgranate, ist sie sofort aus dem Inventar; Leon haelt nach
    dem Wurf (sobald R1 losgelassen und die Granate explodiert ist) das Messer (Rueckfall "nichts
    ausgeruestet").
  - Debug-Menue (SELECT, Raum waehlen, VIERECK): mit Elza laedt der Sprung die xxx1-Raeume (z.B. ROOM1071,
    ROOM1151), mit Leon wie bisher xxx0. 71 der 120 Raumpaare sind als Datei bytegleich — dort sieht man den
    Unterschied nur an Elza-Zweigen des Ports (z.B. Fenster ROOM1121, Irons ROOM1151), in den uebrigen 49 auch
    am Rauminhalt.
* **Entscheidungsfragen:** keine.

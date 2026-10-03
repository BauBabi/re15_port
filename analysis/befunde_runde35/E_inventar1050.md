# Runde 35 Spur E — inventar1050 (Dossier, fortlaufend)

Baum `.claude/worktrees/r35_inventar1050`, Zweig `r35/inventar1050`, Basis master 154a73c1.
Zuteilung (VERTRAG.md): Bank-9-Bit 83 (Reserve), Nachrichten-IDs ROOM1050 28..31 (nur falls noetig),
Ereignis 26 (1050, Vorschlag). Vorhanden: ROOM1050 Ereignis 13 = Ada-Ruf, Slots 13/14 Ada-Ruf.

## Punkte (Wortlaut, AUFTRAG.md Z.15/21/94)
1. "Nach unserer neuen Ada Cutscene im Room 1050, mit der man die andere Tür nicht betreten darf,
   bevor man Ada gerettet hat, lässt sich innerhalb des selben Raums das Player Inventory nicht mehr oeffnen."
2. "Dann möchte ich, das du das Kampfmesser aus den Player Inventar raus nimmst. Das Kampfmesser soll
   IMMER der fallback sein, wenn keine Waffe ausgewählt ist. Dafür muss es nicht noch extra im Player
   Inventory liegen."
3. "Ich bin auch noch nicht ganz zufrieden mit unserer neuen Cutscene in ROOM 1050. Da diese die
   "3. Wand" quasi durchbricht, und Leon so mit dem Spieler redet von den Animationen her. Er sollte
   er so mit sich selbst reden, wie in der Cutscene in ROOM 1170."

## Protokoll
- 2026-10-03 Start: Baum sauber auf 154a73c1, kein build/ vorhanden. configure + build OK.
- Messwerkzeug: `re15_port/tools/r35_e/messlauf.sh` (Kopie von r34n_d/messlauf.sh, eigene exe
  `re15_pc_r35e.exe`). Die GUI-exe schreibt stderr NICHT nach stderr.txt, sondern nach debug.log.

## Messung vorher

### P1 Inventar nach der Szene (Lauf m1/m2, Stand 154a73c1)
Weg wie Runde-34-Abnahme r1: `RE15_SET_FLAG=3:121 RE15_DEBUG_JUMP=1000@120 RE15_PLAYER_POS=22230,-13400,0,0
RE15_INPUT_SCRIPT_BASIS=spiel RE15_INPUT_SCRIPT_START=100 RE15_INPUT_SCRIPT=U0.8,W14 RE15_PRESS=square@150,
start@520,start@600,start@700 RE15_INV_DBG=1 RE15_STATE_LOG=... RE15_EXIT_AT=800` (Tuer ROOM1000 -> ROOM1050,
vor zur 10A0-Tuer, Quadrat -> Szene).
* REPRODUZIERT: drei START-Druecke F520/F600/F700 nach Szenenende -> `[invdbg] stage=0 open=0` in ALLEN
  1071 Bildern. Das Inventar geht nicht auf.
* Zustandslog (Spalten pf = g_re15_pauseflags, pm = player_mode):
  ```
  121 F0   pf=01000000 pm=2      (Titel)
  145 F6   pf=00000007 pm=0      (ROOM1050 frei, vor der Szene)
  315 F151 pf=01000007 pm=2      (Szene, Set(2,7)=1)
   14 F466 pf=01000007 pm=1      (Plc_ret)
  321 F480 pf=01000007 pm=0      (Balken weg — Bit 0x01000000 = RE15_PAUSE_PAD BLEIBT)
  ```
  Das Inventar-Gate game_step_common.c:1238 `!(g_re15_pauseflags & RE15_PAUSE_PAD)` ist danach dauerhaft zu.
  player_mode (0) und letterbox_countdown sind NICHT die Ursache.
* VM-/Nachrichtenspur (Lauf m2, RE15_SCD_TRACE + RE15_MSG_LOG + RE15_FLAG_TRACE):
  ```
  F151 Set(2,7)=1, Set(1,27)=1           (k_ruf +0x04/+0x08)
  F171 Message_on 22   F315 Message_on 23   F366 Message_on 24 (k_ruf +0x7C)
  F466 Set(2,7)=0 / Set(1,27)=0 / Plc_ret / Aot_reset   (k_ruf +0x88..+0x96)
  F466 msg 24 fsm 1 -> 0 (Nachricht 24 schliesst IM SELBEN BILD, NACH dem Set)
  ```
  MECHANISMUS: Message_on 24 oeffnet bei F366 mit Schnappschuss `g_re15_pauseflags_saved = pf` (Bit 0x01000000
  der Szene gesetzt; game_state.c re15_pauseflags_open = FUN_80027e68 @0x80027eb4 lw / @0x80027ec8 sw). Sie
  ist `04 00 ... 04 01 01 63` = Sofortanzeige + Standzeit 99 (msg_common.c case 0 `b == 0x01` -> fsm 5,
  timer = 0x63) und schliesst bei F466 per Schnappschuss-RUECKSPIELEN (re15_pauseflags_close = FUN_80028134
  `lw a0,DAT_800b853c` -> `sw a0,0x800aca40` @0x800286bc/@0x800286cc bzw. @0x80028708/@0x8002871c). Die VM
  hat im selben Bild VORHER `Set(2,7)=0` ausgefuehrt (Reihenfolge Original: SCD im Spiel-Task, Nachrichten-
  FSM erst in FUN_80010000 -> FUN_800280b4 @0x80010044 -> `jal FUN_80028134` @0x800280dc, d.h. am Bildende).
  Das Rueckspielen setzt das Pad-Bit wieder — fuer immer (einzige weitere Schreiber von DAT_800aca40 sind
  Raumwechsel @0x8001ca44/@0x8001caec, Menue @0x8001cbe4/cc6c/cc94/cde8, FUN_8001443c, FUN_80027e68 —
  XREF-Liste ghidra1_V2.txt Z.474490). Erst der naechste Raumwechsel loescht es (re15_pauseflags_clear).
  Der Befund "innerhalb desselben Raums" ist damit erklaert.

## RE-Belege

### P1 Warum das Original an derselben Stelle NICHT haengen bleibt (re15_disasm.py, info/Re1.5/PSX.EXE)
Das Szenenende von k_ruf hat dieselbe Form wie ROOM1170 sub02 @0x015EC..@0x015FC: `Message_on 7` (msg 7 =
`... 04 01 01 63`, RDT @0x1AB0) -> `Plc_motion 17` -> `Sleep 100` -> `Set(2,7)=0`. Im Original geht das gut,
weil die Nachricht EIN Bild frueher schliesst als im Port:
* Oeffnen FUN_80027e68: Guard @0x80027e74 `lbu v0,0(v1)` (0x800b8520) / @0x80027e7c `andi v0,v0,0x80` /
  @0x80027e80 `beq`; Schnappschuss @0x80027eb4 `lw v0,0(a0)` (0x800aca40) -> @0x80027ec8 `sw v0,-31428(at)`
  (0x800b853c); @0x80027ecc `or v0,a3,v0` / @0x80027ed0 `sw v0,0(a0)`.
* Schliessen FUN_80028134 Zustand 6 (Standzeit): @0x800286e8 `addiu v0,v0,-1` auf 0x800b8525; bei 1:
  @0x80028708 `lw v1,-31428(v1)` (0x800b853c) -> @0x8002871c `sw v1,-13760(at)` (= 0x800aca40). Ebenso
  Tasten-Ende @0x800286bc/@0x800286cc. Also volles Wort zurueck — derselbe Mechanismus wie im Port.
* Der Unterschied: Steuerbyte-Schleife nach einer Sofort-Spanne `04 00`. Steuer-Verteiler @0x80028288..
  @0x800282b4 (Index = Byte-1, Tabelle @0x8001096c: [0] Byte 1 -> 0x800282bc, [3] Byte 4 -> 0x80028334).
  Fall `04 NN` @0x80028334: NN = 0 -> Spanne bis zum naechsten 0x04 ueberspringen (@0x8002835c..@0x80028398),
  dessen NN lesen, Tempo `sb v0,-31452(at)` @0x800283b4 (0x800b8524), dann `j 0x80028424` @0x800283b8:
  @0x80028424 `lbu v0,0(s0)` / @0x8002842c `bne v0,zero,0x8002826c` -> ein FOLGENDES STEUERBYTE wird im
  SELBEN Aufruf verteilt (@0x8002826c..@0x80028280: druckbar = (b-0x0c)&0xff < 0xec -> Glyphenweg, sonst
  Verteiler). `01 63` -> @0x800282bc `lbu v0,0(s0)`, != 0 -> @0x80028728 `ori v0,zero,0x6` / @0x80028730
  `sb v0,-31455(at)` (Zustand 6) / @0x80028744 `sb v0,-31451(at)` (Standzeit 0x63 << s1).
  s1 = (DAT_800b5456 == 0) (@0x80028148/@0x80028168); im Spiel ist DAT_800b5456 = 2 (Savestates
  mzd_stage1_briefing_live / engage_live / combat_death / walked / equip_test / parity_turn_R2 / after_flow,
  re15_ss.py) -> keine Verdopplung, Standzeit 99.
  => Original: Message_on in Bild N (SCD im Spiel-Task), FSM am Bildende (Hauptschleife @0x80020ddc
  `jal FUN_80029690` [Tasks] vor @0x80020f3c `jal FUN_80010000` -> @0x80010044 `jal FUN_800280b4` ->
  @0x800280dc `jal FUN_80028134`) zeigt Spanne UND setzt Standzeit 99 in Bild N -> Schliessen in N+99.
  `Sleep 100` laesst `Set(2,7)=0` in N+100 laufen -> das Ruecksetzen in N+99 stellt nur den ohnehin noch
  gesetzten Szenenzustand her, das Set in N+100 loescht das Bit endgueltig.
* Port (msg_common.c re15_dialog_tick case 0, Zweig `b == 0x04 && a == 0`): nach der Spanne
  `g_scd.message_timer = g_scd.message_scroll; break;` -> das `01 63` wird erst im NAECHSTEN Bild gelesen
  -> Standzeit beginnt in N+1 -> Schliessen in N+100, im selben Bild wie das Set, aber NACH der VM ->
  Schnappschuss (mit Pad-Bit) ueberschreibt das geloeschte Bit. Gemessen: F366 Message_on 24, F466 Set +
  Schliessen (Lauf m2). Das ist der Port-Fehler, nicht das Szenenende.
* Der Port-1170-Vorspann zeigt das NICHT, weil dort `pf=00000007` waehrend der ganzen Szene steht
  (Lauf m3_1170: der Raumwechsel loescht das Wort nach dem Set(2,7)=1 von sub02) — kein Vergleichsfall.

### P2 Kampfmesser — die Original-Form IST "nichts ausgeruestet -> Messer" (re15_disasm.py, PSX.EXE)
* **Ruecklauf-Regel steht im Original.** Ausruest-Commit beim Schliessen des Statusschirms @0x80046654..88:
  `lbu v1,9672(v1)` (0x800b25c8 = ausgeruesteter PLATZ) / @0x8004665c `ori v0,zero,0x80` / @0x80046660
  `bne v1,v0,0x80046670` / @0x80046668 `j 0x80046680` mit Verzoegerungsplatz @0x8004666c `ori v0,zero,0x1`
  / sonst @0x8004667c `lbu v0,0(at)` (inv[25c8].Id) / @0x80046688 `sb v0,-13731(at)` (0x800aca5d = Waffen-Id).
  => Platz 0x80 ("nichts") ergibt Waffe 1 = COMBAT KNIFE (Namensliste inventory_common.c:400). Port:
  menu_common.c close_phase `(eq == 0x80) ? 1 : ...` und re15_menu_toggle — schon vorhanden.
* **Ablegen gibt es im Original** (Statusschirm, gleiche Id angewaehlt): menu_common.c state5_classifier
  `eq_id == id` -> UNEQUIP @0x8004aaec (s_c3 = 2) -> 25c8 := 0x80 (menu_common.c:634). Danach Messer.
* **Das Messer kommt nur aus dem Startinventar.** Spielstart-Init @0x80045e00..@0x80045ff4:
  `lbu v0,-13732(v0)` (0x800aca5c Charakter) / @0x80045e28 `sltiu v0,v0,0x4` -> Leon-Tabelle Ids
  @0x80074bb8 `01 03 15 00 00 00`, sonst Elza-Tabelle @0x80074bc4 `01 00 00 00 00 00`; Mengen @0x80074bd0
  `00 0f 32 00 00 00` (Lesestellen @0x80045e44 / @0x80045ef0 / @0x80045e98 / @0x80045f44); danach
  @0x80045fe0 `sb v0,9673(at)` (25c9 := 0x80) und @0x80045fec `sb zero,9672(at)` (25c8 := 0 = Messer).
  Zensus aller 206 RDTs (194 Item-Saetze Item_aot_set 0x50/0x4E, Id = u16 @+14 wie op_item_aot_set):
  **kein einziger Messer-Gegenstand** (Skript scratchpad item_zensus.py, Ausgabe "Messer(Id 1): []").
  Port: re15_inv_load_briefing gibt Leons Tabelle fuer JEDEN Charakter (Elza-Tabelle fehlte).
* **Folgen, sobald Platz 0 nicht mehr dauerhaft das Messer traegt** (im Original unerreichbar, weil das
  Messer Platz 0 nie verliess — Ablegen von Waffen gibt es nicht, die Kiste ist Port-Zusatz):
  1. Reserve-Munition in Platz 0 wird nicht erkannt: FUN_8004eb70 @0x8004ebbc `jal 0x8004dfec` /
     @0x8004ebc4 `sll v0,v0,24` / @0x8004ebc8 `slt v0,zero,v0` (Platz > 0). RE2 Retail hat das behoben:
     FUN_8006a23c @0x8006a278 `jal 0x800696cc` / @0x8006a294 `nor v0,zero,a0` / @0x8006a2a0 `srl v0,v0,31`
     (Platz >= 0; Sonderfall Waffe 0x11 @0x8006a28c-90). Beta -> Retail: RE2-Regel.
  2. Breite Waffe aufnehmen schiebt den Ausruest-Platz OHNE 0x80-Schutz: RE1.5 FUN_8004dc4c @0x8004dc88
     `addiu v1,v1,9672` / @0x8004dc8c `lbu v0,0(v1)` / @0x8004dc94 `addiu v0,v0,2` / @0x8004dc9c `sb v0,0(v1)`
     -> 0x80 wird 0x82 (Commit liest dann inv[2] = falsche Waffe). Erreichbar in STAGE1: ROOM1110 @0x00B5A
     Item 0x13 x100 (Zensus). RE2 Retail FUN_800698b4(1) schuetzt: @0x80069998 `addiu v0,zero,128` /
     @0x8006999c `beq v1,v0,0x80069aa4` (0x80 -> nicht schieben) / @0x800699a0 `addiu v0,a0,2`.
     Gleiches im Port-Kistenweg re15_itembox.c inv_front_shift2.
  3. Verdichten (FUN_8004dadc @0x8004dbac..c8 `bne v0,s0` -> nur der verschobene Platz zaehlt runter)
     faesst 0x80 nicht an — Port gleichwertig (inventory_common.c:92 `eq != 0x80 && eq > f`).
* Savestate-Gegenprobe (re15_ss.py): mzd_stage1_briefing_live / engage_live / combat_death: 25c8 = 00,
  aca5d = 1, inv `01 00 00 00 03 0f 00 00 15 32 ...`; equip_test / parity_turn_R2: 25c8 = 01, aca5d = 3.

### P3 Wie Leon in ROOM1170 mit sich selbst redet (scd_dump_room.py ROOM1170.RDT, Kameratabelle RDT+0x24 -> 0x60)
Zwei Selbstgespraeche in ROOM1170 (Zensus D_belege/plc_motion_zensus.tsv + eigener Dump):
* **sub02 Ende** (Vorspann, nach dem Abflug): @0x0156A `40 00 09 20 68 0a a0 d7` Plc_dest Modus 9 -> Leon
  blickt zu (2664,-10336) (= -z); @0x015B6/@0x015BA Member_set Leon (2664,-7336) ohne Richtung; @0x015D6
  `29 04` Cut 4; @0x015DC `2b 06 00 00` "Leon: Oh, that's just freaking great..." + @0x015E4 `3f 00 19 00`
  Clip 25 (raumeigen: Hand ans Gesicht) + Sleep 100; @0x015EC `2b 07 00 00` "Leon: Now what am I gonna do?"
  + @0x015F0 `3f 00 11 00` Clip 17 + @0x015F4 Sleep 100; @0x015F8 Set(2,7)=0, @0x015FC Set(1,27)=0, @0x01600
  Cut_chg 3, @0x01602 Cut_auto 1, @0x01604 Work_set(1,0), @0x01608 Plc_ret, @0x0160A Evt_end.
  Kamera Cut 4 @0x000E0: pos (-72,-2196). Winkel zwischen Leons Blick (0,-1) und der Richtung zur Kamera =
  **152 Grad** — die Kamera sieht ihm in den Ruecken.
* **sub14** (Ausloeser main sub00 @0x0140C Switch Cut, Fall 11 @0x01410, Ck(3,62)==0 @0x0141A -> Evt_exec 14):
  @0x0174A Pos_set + @0x01752 Member_set Richtung 2957, Cut 12 @0x01744; @0x0175E `2b 0d` "Damn it, there must
  be another way out of here." + @0x01762 `3f 00 12 00` Clip 18 + @0x01766 `41 02 00 00 00 00 2c 01 00 0a`
  Plc_neck Modus 2 (Kopf relativ senken, Nick 300) + Sleep 30 + @0x01774 `41 04 03 00 00 00 00 00 64 00`
  Plc_neck Modus 4 (Kopfschuetteln, 3 Schwuenge, Tempo 0x64) + Sleep 60 + @0x01782 Clip 18 + @0x01786
  `43 00 80 00` rueckwaerts + Sleep 20; @0x0178E msg 14 + Clip 23 (Hand an die Huefte); @0x017A2 msg 16
  "That's right! I have to report this to the chief." + Clip 18 vor/zurueck; Ende Cut_chg 11, Set(1,27)=0,
  Set(2,7)=0, Cut_auto 1, Plc_ret. Winkel Blick -> Kamera Cut 12 = 27 Grad (fast frontal, aber Kopf
  gesenkt/schuettelnd, Gesten zum eigenen Koerper).
* **Clip 19 (Runde-34-Geste zu "Another civilian survivor.")** steht in ALLEN 32 Original-Aufrufen seines
  Inhalts (Hash fde929aa6e6b, Zensus) ausschliesslich in Saetzen an ein Gegenueber ("We can talk later...",
  "Ada, what is this place...?", "Where's Elza?", ...), nie im Selbstgespraech. Clip 17 dagegen auch im
  Selbstgespraech (1170 sub02, 3020 sub02 "Dammit! the door does not open!" als Inhalt von Clip 2).
* **Runde-34-Stand (gemessen am Riegel, alter Stand):** nach dem Rueckschritt Plc_dest Modus 9 auf den
  Standort von Cut 4 (14999,-8140) -> `zur Kamera B145 cos 1.000` = Leon schaut frontal in die Kamera
  (Winkel 0 Grad) und macht die Gespraechsgeste 19 -> das ist die "durchbrochene 4. Wand".
* Kamera ROOM1050 Cut 4 (Messbezug): Leon zur Tuer (+X) gewandt -> Winkel Blick -> Kamera 100 Grad
  (rechnerisch, D_belege/kameras_1050.txt), Kamera von hinten links.

## Umsetzung

### P1 Nachrichten-FSM (msg_common.c re15_dialog_tick, 5 Zeilen)
Nach der Sofort-Spanne `04 00 .. 04 NN`: ist das naechste Byte ein Steuerbyte (!= 0x00 und
(b-0x0c)&0xff >= 0xec, @0x80028274-80), wird es im selben Bild weiterverteilt (`continue`) statt erst im
naechsten (@0x800283b8 `j 0x80028424` / @0x8002842c `bne v0,zero,0x8002826c`). Druckbare Glyphen nach der
Spanne warten wie bisher das Tempo ab (@0x80028434 `bne s2,zero` mit verbrauchtem Budget).
Wirkung: jede `04 00 ... 04 01 01 NN`-Zeile schliesst ein Bild frueher (= Original). Keine Konstante neu.

### P2 Messer-Rueckfall (neue Dateien + kleine Haken)
* NEU `re15_port/include/re15_messer.h`, `re15_port/engine/src/messer_rueckfall.c`:
  - `re15_messer_startinventar()` — Starttabelle des Originals je Charakter (Ids @0x80074bb8 Leon /
    @0x80074bc4 Elza, Mengen @0x80074bd0, Weiche `sltiu v0,v0,0x4` @0x80045e28) OHNE Id 1 (NUTZER-VORGABE);
    25c9 := 0x80 (@0x80045fe0), 25c8 := 0x80 (statt 0 @0x80045fec, weil Platz 0 kein Messer mehr ist),
    Waffen-Id 1 nach der Commit-Regel @0x80046668/@0x8004666c. Leon: Platz 0 BROWNING HP x15, Platz 1
    H.GUN BULLETS x50. Elza: leer (Original-Tabelle: nur das Messer) -> Messer-Rueckfall.
  - `re15_messer_aus_inventar()` — alter Spielstand: Messer aus Inventar (ausgeruestet -> 25c8 := 0x80
    wie UNEQUIP, dann re15_inv_remove_slot = Verbrauch + Verdichten FUN_8004dadc) und aus der Kiste.
* Haken: `platform/pc/main.c` Startinventar-Aufruf (2 Zeilen statt `re15_inv_load_briefing()`; die
  Byte-Kopie des Original-Briefings bleibt fuer Sonden/Tests unveraendert); `re15_savedata.c` nach dem
  Kisten-Import (2 Zeilen); `inventory_common.c` FUN_8004eb70-Nachbau mit RE2-Regel Platz >= 0 / -1
  (@0x8006a294/@0x8006a2a0) + Nachlade-Ausfuehrung `as < 0`; breite Waffe nur bei 25c8 != 0x80 schieben
  (RE2 @0x8006999c) in `inventory_common.c` UND `re15_itembox.c`; `game_step_common.c` Nachlade-Gate
  `>= 0` (1 Zeile); `re15_inventory.h` Kommentar; `tests/unit/test_room1140_combat.c` (27) auf die
  RE2-Regel umgestellt (Platz 0 -> 0, keine Reserve -> -1).
* Speicherformat unveraendert (equipped_slot 0x80 + weapon_id 1 werden wie bisher geschrieben).
* Das Item-Debug des Statusschirms (SELECT + R1, Original @0x8004a138..) kann weiter jede Id setzen,
  auch 1 — Debug-Werkzeug, bleibt original; der naechste Ladevorgang raeumt ein solches Messer ab.

### P3 k_ruf ab +0x4C (adaruf_1050.c, re15_adaruf.h; 162 -> 166 Bytes)
NUTZER-VORGABE ist die Absicht ("so mit sich selbst reden wie in ROOM1170"), die FORM ist Zeile fuer Zeile
aus ROOM1170: Drehung zur Kamera (+0x4C..+0x5F alt) ENTFAELLT, Leon bleibt nach Modus 8 zur Tuer gewandt.
```
+4C Sleep 20                 ROOM1090 sub02 @0x02486
+50 Message_on 23            Form ROOM1170 sub14 @0x0175E
+54 Plc_motion(0,18,0)       sub14 @0x01762   Hand zur Brust
+58 Plc_neck(2) Nick 300     sub14 @0x01766   Kopf senken
+62 Sleep 30                 sub14 @0x01770
+66 Plc_neck(4) 3x 0x64      sub14 @0x01774   Kopfschuetteln
+70 Sleep 60                 sub14 @0x0177E
+74 Plc_motion(0,18,0)       sub14 @0x01782
+78 Plc_flg 0x80             sub14 @0x01786   rueckwaerts
+7C Sleep 20                 sub14 @0x0178A
+80 Message_on 24            Form ROOM1170 sub02 @0x015EC
+84 Plc_motion(0,17,0)       sub02 @0x015F0   Arm-Schwung (Runde-34-Wunsch fuer diese Zeile)
+88 Sleep 100                sub02 @0x015F4
+8C Set(2,7)=0 / +90 Set(1,27)=0 / +94 Work_set(1,0) / +98 Plc_ret   sub02 @0x015F8..@0x01608
+9A Aot_reset(4, sce 1, msg 25) / +A4 Evt_end   (unveraendert)
```
Kein Cut_chg/Cut_auto: die Szene wechselt den Schnitt nie (bleibt Cut 4), also gibt es nichts
zurueckzustellen (sub02/sub14 stellen nur zurueck, weil sie vorher Cut_chg benutzen).
Neue Nachrichten-IDs: keine (28..31 nicht noetig). RE15_ADARUF_KAMERA_X/Z bleiben als Messbezug fuer den Riegel.

## Messung nachher

### P1 (Lauf m4, Stand mit msg-Fix, k_ruf noch unveraendert)
```
315 F151 pf=01000007 pm=2   (Szene)
 14 F466 pf=00000007 pm=1   (Plc_ret — Bit 0x01000000 GELOESCHT)
121 F480 pf=00000007 pm=0
[invdbg] 790x stage=0 open=0 | 9x stage=2 open=0 | 72x stage=2 open=1   (START F520 -> Inventar offen)
```
Der Befund ist damit am Mechanismus behoben (Schliessen in F465, Set in F466), nicht durch Umgehung.

### P2 (Lauf m5, Stand c17305bd: Startinventar ohne Messer)
Weg: Titel -> RE15_DEBUG_JUMP=1000 -> ROOM1000, Eingabe `W1,M1,MA0.1,M0.6,MA0.1,...` (M = R1 zielen, A =
Quadrat), START F330, RE15_FRAMEDUMP 150-400/10. Zustandslog: `mg=-1` in allen Bildern (= 25c8 0x80, nichts
ausgeruestet), Zielclip `ac` 13 -> 8 -> 7 bei jedem Quadrat (W01-Bank = Messer: debug.log
`[equip] W-bank -> W01 (Clips 14, Rueckstoss-Clip7 fc=25)`), drei Stiche F160/F181/F202.
Bild `E_belege/p2_messer_ohne_inventar.png` (FRAMEDUMP F160/F170/F180 = Messerstich, F390 = Statusschirm):
Item-Liste = BROWNING HP 15 + H.GUN BULLETS 50, KEIN Messer; Kasten "Equip Arms" zeigt das Standard-Messer
hell. Das ist die Original-Darstellung fuer 25c8 = 0x80: FUN_80049a5c zeichnet die feste Messer-Kachel
(Icon-Zelle 10, uv (40,90)) mit Helligkeit 25cd = 0x80 statt 0x3e (re15_inv_screen.c:146 =
@0x800495e8-618) — das Original kennt "kein Gegenstand ausgeruestet = Standardwaffe Messer" also auch im Bild.

### P3 + P1 am Riegel (test_r34n_d_adaruf szene, echte VM + Spielschritt, B = Bilder nach dem Druck)
```
msg 22/23/24 ab B21/B153/B263 | Schritt B123..B133 Weg 700 dz 0 Gierung 4095 | Clip18 B153, rueckw. B243,
Neck2 B153, Neck4 B183, Clip17 B263, Clip19 nie | Blick->Kamera max cos -0.157 (= 99 Grad) | Ende B363 |
Balken voll B15 weg B377 | Pad-Bit 0, Inventar 1        -> szene/doppel/sperre/speicher: 0 Fehler
```
Mutationsprobe (msg-Fix per `&& 0` abgeschaltet, nur gebaut + Riegel, danach zurueckkopiert):
`Pad-Bit 1, Inventar 0` -> `FEHLER: Pad-Bit ... (ist 1)` + `FEHLER: START ... oeffnet das Inventar`.
Der neue Szenenschluss (Form 1170 sub02: Message_on + Sleep 100 + Set) haengt also OHNE den Fix genauso —
der Riegel misst den Mechanismus, nicht die Choreografie.

## Tests

## OFFEN

## Fuer den Nutzer

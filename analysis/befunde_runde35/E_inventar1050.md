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

## Umsetzung

## Messung nachher

## Tests

## OFFEN

## Fuer den Nutzer

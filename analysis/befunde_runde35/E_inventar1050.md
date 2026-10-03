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

## Umsetzung

## Messung nachher

## Tests

## OFFEN

## Fuer den Nutzer

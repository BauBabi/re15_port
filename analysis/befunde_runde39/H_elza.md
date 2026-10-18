# Runde 39 — Spur H "elza" (Dossier, fortlaufend)

Baum `.claude/worktrees/r39_elza`, Zweig `r39/elza`, Basis c71fa144 (master nach Integration Welle 1). Start 2026-10-18.

## Punkte (Wortlaut AUFTRAG.md)
1. "Bei Elza, direkt in ROOM 1031 in der 1. Cutscene direkt am Anfang hebt sie die Hand aus irgendeinen Grund 2x, also
   lässt sie einmal fallen zwischendurch und hebt sie nochmal. Im Original hebt sie die Hand nur einmal und senkt sie
   dann wieder."
2. "Bei Elza, nach aufnehmen von Roys Diary muss direkt die Karte kommen, mit den Markern wo man hin muss, noch bevor
   das Spiel zum bewegen wieder zurückgeben wird nach dem schließen des Tagebuchs."
3. "Elza kann in ROOM 1090 bei dem Feuerlöscher, bei dem ausgebrannten Fahrzeug hochklettern warum auch immer... das
   ist falsch..."

## Stand
- Dossier angelegt, Pflichtlektuere AUFTRAG/VERTRAG gelesen, Vorrunden E_elzaintro / G_tagebuch (r38), I_feuerloescher
  (r37), A_klettern (r36) gelesen.
- Bau des Baums (configure + build) OK; exe 10:27.
- Messung vorher + RE-Belege aller drei Punkte (s. Abschnitte); Umsetzung 08f3094d (vor Bau/Messung).
- FORTSETZUNG (zweite Sitzung, ab 11:55): configure + build (neue Datei elza_r39.c) OK. Erster Testlauf deckte auf:
  (a) der Zensus-Riegel "Null-Box nie auf dem Spielerband" ist FALSCH (51 von 558 Records liegen auf dem Spielerband) —
  Riegel und Belegtext berichtigt, die Regel bleibt PORT-WAHL in der Form der drei Vorlagen-Records; (b) die
  Vorlagen-Hoehen waren falsch zitiert ("Bodenhoehe -1800/0" — die Records stehen bei y -2653 bzw. -1600, Band jeweils
  Spielerband(y) + 1); (c) NACH dem Latch-Fix hebt Elza die Hand in Cut 6 ERNEUT (Clip 0 vorwaerts waehrend der
  Plc_dest-Align-Phase) -> Punkt 1b: Sub-Init-Clip der Spieler-Walk-Subs (s. Punkt 1). Alles gemessen, umgesetzt,
  Riegel gruen (Abschnitt Tests).

## Punkt 1 — ROOM1031 erste Szene: Hand nur einmal heben
### Messung vorher
Bau dieses Baums (Basis c71fa144, `local_build.sh configure` + `build` = LOCAL-BUILD-OK). Werkzeug: das Elza-Startskript der
Runde 38 (`analysis/befunde_runde38/E_elzaintro_werkzeug/e_lauf.sh`, Neues Spiel ueber Titel + Figurenwahl Elza,
`RE15_PSELECT_AUTO=1 RE15_PSELECT_AUTO_SWITCH=1`), dazu `RE15_SCD_TRACE=1`, `RE15_ANIM_TRACE=anim.txt`,
`RE15_STATE_LOG=state.txt`, Bildserie `RE15_FRAMEDUMP=440-540/2` (Skala 1). Drei Laeufe (v1/v2/v3, Scratchpad p1/).

Die erste Szene ist ROOM1031 **sub13** (@0x02982..0x02A58, `tools/scd_dump_room.py`): Set(3,207,0), Set(2,7,1), Set(1,27,1),
Cut_chg(4) @0x0298E, Work_set(1,0), 3x Member_set, Sleep 1, **Plc_motion(0,11,0) @0x029A4 `3f 00 0b 00`**, **Plc_flg 0x04
@0x029A8 `43 00 04 00`** (Schleife), Sleep 20, Plc_neck, Sleep 20, **Plc_motion(0,0,0) @0x029BE `3f 00 00 00`**,
**Plc_flg 0x80 @0x029C2 `43 00 80 00`** (rueckwaerts), Sleep 30, Cut_chg(6) @0x029CA, dann Plc_dest/Gosub, Plc_motion 9/12,
Message_on 18 ("That was close ..."), Plc_motion 10 + 0x04, Cut_chg(0) @0x02A22, Message_on 19 ("Hey, is anyone here?!"),
Plc_motion 9, Cut_auto, Set(2,7,0), Set(1,27,0), Plc_ret @0x02A56, Evt_end @0x02A58.

Gemessen (Bildzaehler ab dem Selbsttuer-Wiedereintritt in ROOM1031; v3 STATE_LOG = Spielerzustand je Tick, v1/v2 ANIM_TRACE =
die aufgeloeste Pose je Bild):

| Bild | SCD | STATE_LOG (pst = Kommandowort +0x4, mo = +0x94) | ANIM_TRACE Spieler |
|---|---|---|---|
| F445 | `Cut_chg(4)` | pst=1 mo=210 pm=0 pf=00000007 | motion 210 (W01 Clip 1 "Settle", cur 0) |
| F446..F450 | (Sleep) | pst=1 mo=210 pm=2 **pf=FF000007** (RE15_PAUSE_PLAYER gesetzt: kein Spieler-Tick) | 210, cur 0 |
| **F451** | **`Plc_motion(0,11)` + `Plc_flg 0x04`** | **pst=4, mo=210** | 210 |
| F452..F490 | (Sleep 20 / Plc_neck / Sleep 20) | pst=4 **mo=200** (Idle) | motion 200 = W01 Clip 3 (fc 1, statisch, Arm unten) |
| F491 | `Plc_motion(0,0)` + `Plc_flg 0x80` | pst=4 mo=0 | Clip 0 fc 20 rueckwaerts: Slot 19 (Arm OBEN) -> 17 -> ... |
| F521 | `Cut_chg(6)` | | |

Bilder (v2, `scratchpad/p1/v2/montage_hand.png`, Ausschnitt um Elza, je 6 Bilder): F450..F486 Arm unten (statisch), ab F492 Arm
oben (Schnapp-Start des Reverse-Clips 0 mit 7-Bild-Blende) und danach sinkend. **Der erste Hub (Clip 11) fehlt im Port ganz**;
die Hand kommt erst bei F491 ueber den rueckwaerts gestarteten Clip 0 hoch (Slot 19 = Endpose "Arm oben") und sinkt dann.
Beim Nutzer (v0.8.26) liegt die Latch je nach Bildtakt ein Bild spaeter als das Plc_motion: dann ist Clip 11 ein Bild lang
angeblendet (Hub 1), die Latch stellt Settle/Idle zurueck (Fallen), F491 hebt per Clip 0 rueckwaerts erneut (Hub 2) — genau
"hebt 2x, laesst einmal fallen". Mechanismus in beiden Faellen derselbe (unten).

**Ursache (Port):** `re15_player_room_entry_pose()` (player_common.c:259) speichert die Eintritts-Pose (cmd-0-Endzustand,
Motion 210) sofort beim Raumwechsel UND schaltet `s_entry_pose_step = 1` scharf: Latch 1 (= "cmd-0-Handler") speichert im
naechsten Spieler-Tick NOCHMALS Motion 210, Latch 2 im uebernaechsten die Idle-Uebergabe (Motion 200). Waehrend der
Selbsttuer-Blende F446..F450 steht `RE15_PAUSE_PLAYER` (pf FF000007) — der Spieler-Tick laeuft nicht (byte-true: der
Dispatcher FUN_80031c44 prueft dieses Bit @0x80031c54). Der SCD-Faden laeuft bei F451 weiter und setzt `Plc_motion(0,11)`
(+0x94 := 11, +0x4 := 4). Im SELBEN Bild laeuft danach der erste Spieler-Tick seit F445: Latch 1 ueberschreibt +0x94 mit 210,
F452 Latch 2 mit 200. Clip 11 wird nie posiert.

### RE-Belege
* **Plc_motion-Handler @0x80041b90** (selbst nachgelesen, scd_vm.c op_plc_motion zitiert die Bytes): `lw v0,340(a0)`
  @0x80041ba0 (Work-Entity), `sb a1,148(v0)` @0x80041ba8 (+0x94 = Clip), **`sb v1(=4),4(v0)` @0x80041bb0 (+0x4 =
  Kommandowort 4)**, `sb zero,6(v0)` @0x80041bb4, `sb zero,7(v0)` @0x80041bb8, `sb a2,5(v0)` @0x80041bc4.
* **Dispatcher FUN_80031c44**: `lbu v1,-13736(v1)` @0x80031c8c (0x800aca58 = Kommandowort), `sll v1,v1,2` @0x80031c94,
  Tabelle @0x80073f90 (`addiu at,at,16272` @0x80031ca4, `lw v0,0(at)` @0x80031cac, `jalr v0` @0x80031cb4): [0] =
  cmd-0-Handler (Eintritts-Pose, W-Bank Clip 1 @0x80031c10-c24, danach Kommandowort 1 @0x8003192c), [1] = 0x80031de8
  (Idle-FSM; Fall 0 = Uebergabe @0x80032074-b4), **[4] = 0x80030660 (Plc-Executor)**. Pause-Gate `lw a0,g_pauseflags`
  @0x80031c54 (RE15_PAUSE_PLAYER 0x80000000, include/re15_scd.h:521).
  => Sobald +0x4 = 4 steht, laufen cmd 0 und cmd 1 nicht mehr — die ausstehende "Uebergabe" existiert im Original nicht.
  Der Raumwechsel nullt das Kommandowort (`sb zero,0x800aca58` @0x8001cbdc, Transitions-FSM Zustand 3); cmd 0 laeuft im
  ersten Dispatcher-Durchlauf danach (= F445, pf 00000007), DANN die Blende mit gesetztem Pause-Bit, DANN das SCD-
  Plc_motion bei F451 — im Original trifft der Dispatcher bei F451 bereits Kommandowort 4.
* **Spieler-Plc-Executor FUN_80030660** (selbst disassembliert, `re15_disasm.py dis 0x80030660 110`): Sub-Dispatch ueber
  0x800aca59 in Tabelle @0x80073e30 (@0x8003068c-ac); Eintrag 0 = @0x80030710: Phase +0x06 (0x800aca5a): 0 -> Init
  `sb v0(=1),0(a0)` @0x80030748, `+0x8f := 7` @0x8003075c (`:= 0` bei Flag 0x40 @0x80030778), `+0x95 := 0` @0x80030764;
  `jal 0x8001f314` (anim_set, a2 = Flags>>7 = Reverse-Bit @0x80030794) @0x80030790; Clip-Ende (v0 != 0) ->
  **`+0x06 := 2` HOLD @0x800307dc, mit Flag 0x4 `+0x06 := 1` = SCHLEIFE @0x800307e0-f0**; Flag 0x8 = zweiter anim_set
  je Bild @0x800307a8-c4.
* **Clip-Daten der ROOM1031-Spielerbank** (RDT+0x5C = 0x307C, RBJ-Container: Laenge 26752, 1 Record, EDD @+0x62D4; selbst
  aus den RDT-Bytes gelesen, Format wie emd_common.c re15_emd_parse_rbj_record): 13 Clips, Laengen
  [20, 30, 30, 20, 30, 25, 30, 50, 18, 24, 20, **2**, 50]. **Clip 11 = 2 Bilder = Haltepose "Arm oben"** (mit Schleife
  0x04 statisch), **Clip 0 = 20 Bilder**; rueckwaerts (0x80) posiert anim_set fc-1-(+0x95), also Slot 19 .. 0 = Arm senken.
  Der Original-Ablauf ist damit: 7-Bild-Blende (+0x8f=7) in die gehaltene Hub-Pose (Clip 11), 40 Bilder halten, dann
  Clip 0 rueckwaerts = senken — **ein Hub, ein Senken** (= Nutzer-Wortlaut zum Original).
* ANIM_TRACE-Spalten (anim_select_common.c:135): Bild, Slot, Typ, Motion, Clip, fc, cur, Slot_res, Len, Loco, Reverse.

### Umsetzung
**1a — Eintritts-Latch vs. Kommandowort 4** (`engine/src/player_common.c` Haken vor der Latch, 1 Zeile;
`engine/src/elza_r39.c` `re15_elza_r39_latch_hinfaellig`, `include/re15_elza_r39.h`): steht das Kommandowort +0x4 des
Spielers auf 4 (`RE15_R39H_KOMMANDOWORT_PLC`, Plc_motion `sb v1(=4),4(v0)` @0x80041bb0), ist die ausstehende zweistufige
Latch (cmd 0 @0x80031c10-c24 / Fall 0 @0x80032074-b4) hinfaellig — der Dispatcher FUN_80031c44 (`lbu v1,-13736(v1)`
@0x80031c8c, Tabelle @0x80073f90, `jalr v0` @0x80031cb4) laeuft ab dann nur noch [4] = 0x80030660. Kein Konstanten-Wert
ausser dem Kommandowort 4.

**1b — Sub-Init-Clip der Spieler-Walk-Subs (NEU, gemessen erst nach 1a)**: Nach dem Senken (Plc_motion(0,0)+0x80
@0x029BE/@0x029C2) folgt `Cut_chg(6)` @0x029CA + `Plc_dest(0,9,0x20,-9097,-21855)` @0x029CC (`40 00 09 20 77 dc a1 aa`).
Messung p1n (nur 1a, ANIM_TRACE Spieler): F491..F520 Clip 0 rueckwaerts (slot 19 -> 0, dann 0 gehalten), **F521..F528
Clip 0 VORWAERTS cur 30..37 = slot 10..17 (der Arm steigt erneut!)**, erst F529 Clip 5 (105, Laufen). Ursache: der
Plc_dest-Handler nullt +0x1c4 (`sh zero,452(a1)` @0x80041c4c, Port op_plc_dest ebenso), der Walker des Ports liess den
Spieler in der Align-Phase (walk_fsm 1) "stehen" und setzte den Lauf-Clip erst beim Uebergang in Phase 2 — der globale
Advancer lief den alten Clip 0 derweil vorwaerts (Modulo 20 -> slot 10..17). Im Original setzen die Spieler-Subs der
Tabelle @0x80073e30 ihren Clip in **Phase 0** (selbst disassembliert, `re15_disasm.py dis`):
* TURN Sub 9 @0x80031360: `sb v0(=1),0(a0)` Phase := 1 @0x80031398, **`sb v0(=5),-13592(at)` +0x94 := 5 @0x800313a4**,
  `sb zero,-13591(at)` +0x95 := 0 @0x800313b0, `sb v0(=7),-13597(at)` +0x8f := 7 @0x800313b8; Phase 1: `jal 0x8001ab9c`
  arc_test Kegel 0x60 @0x800313d0/d4; Ankunft -> Sub := 6 @0x800313ec, Phase := 0 @0x800313f4, Flag `jal 0x8004ef90`
  @0x800313f8; sonst Slew `jal 0x8001aac4` 0x60 @0x8003143c/40; **`jal 0x8001f314` anim_set(PLW-Paar 0x800acbc4/c8, 0,
  0x200) @0x80031494 in JEDEM Bild** — der Lauf-Clip wird waehrend der Drehung posiert und advanced.
* WALK Sub 4 @0x80030af0: +0x8c := 0x4b @0x80030b44, Phase := 1 @0x80030b4c, **+0x94 := 5 @0x80030b58**, +0x95 := 0
  @0x80030b64, +0x8f := 7 @0x80030b6c; Phase 1 (Align): arc_test 0x15e @0x80030b80/84, Phase := 2 @0x80030b94, Slew
  0x60 @0x80030ba4, **anim_set @0x80030bf8** jedes Bild; Phase 2: Slew 0x30 @0x80030c58, Vortrieb `jal 0x800245d8`
  @0x80030c5c.
* RUN Sub 5 @0x80030d28: +0x8c := 0xc8 @0x80030d7c, **+0x94 := 5 @0x80030d90**, +0x95 := 0 @0x80030d9c, +0x8f := 7
  @0x80030da4; Phase 2: +0x94 := 0 @0x80030dd8, +0x95 := 0 @0x80030de0.
+0x94 = 5 der PLW-Bank = Port-Sentinel 105 (`RE15_MOTION_WALK`, `re15_to_re2_plc_dest_clip(0x04)` mit RBJ-Overlay —
dieselbe Aufloesung, die der Walker in Phase 2 benutzt). Umsetzung: `re15_elza_r39_plc_walk_init` (elza_r39.c) — nur
Spieler, nur Modus 4/5/9: `re15_actor_set_motion(105)`, `anim_frame = 0`, `anim_frac = 7`, `anim_freeze = 0`; Haken in
`actor_locomotion.c re15_actor_step_walk` am Walker-Init (walk_fsm 0, nicht-rueckwaerts-Zweig, 1 Aufruf). Der
Phase-2-Pfad bleibt (Modus 4/9 derselbe Clip = kein Neustart, wie @0x80030b94 nur die Phase; RUN 5 -> 0 @0x80030dd8).
Nicht-Spieler (nur Elliot 0x47 erreicht diesen Walker) unveraendert. Der alte Kommentar "the actor STOOD through the
align phase" ist damit fuer den Spieler ueberholt und im Code so vermerkt.

### Messung nachher
Lauf p1n2 (Neues Spiel Elza, `e_lauf.sh`, ANIM_TRACE + STATE_LOG + Bildserie 440-540/2, exe mit 1a + 1b;
Scratchpad p1n2/, Montagen `montage_p1n2_cut4.png` (Ausschnitt Elza, 2x) und `montage_p1n2_cut6.png`):

| Bild | Pose-Strom Spieler (ANIM_TRACE: mo clip fc cur slot rev) | Bild |
|---|---|---|
| F445..F450 | mo=210 clip 1 cur 0 (Settle; F446-450 pf=FF000007 pausiert) | Arm unten |
| **F451** | **mo=11 clip 11 fc 2 cur 0 slot 0**, pst=4 (Plc_motion(0,11) @0x029A4 im selben Bild) | Blende beginnt |
| F452..F490 | mo=11 cur 1..39 **slot 1 konstant** (Haltepose, Schleife 0x04) | **Arm oben, statisch** (F454-F490) |
| F491 | mo=0 clip 0 fc 20 cur 0 **slot 19 rev=1** (Plc_motion(0,0)+0x80 @0x029BE/@0x029C2) | Senken beginnt |
| F492..F510 | slot 18 .. 0 rueckwaerts | Arm sinkt |
| F511..F520 | mo=0 slot 0 rev=1 gehalten | Arm unten |
| **F521** | **mo=105 clip 5 fc 30 cur 0** (Cut_chg(6) @0x029CA + Plc_dest(0,9) @0x029CC, TURN-Sub-Init) | Cut 6: Drehung mit Lauf-Clip |
| F522..F530 | mo=105 cur 1..9 | laeuft an |
| F531 | mo=1 clip 1 cur 1 (1 Bild Sub 6 nach der TURN-Ankunft, s. OFFEN) | |
| F532.. | mo=105 cur 0.. (Plc_dest(0,4) @0x029D6, WALK) | Elza geht zum Gitter |

STATE_LOG (Spieler): F445 pst=1 mo=210, F446-F450 pf=FF000007, **F451-F490 pst=4 mo=11**, F491-F520 pst=4 mo=0,
ab F521 mo=105. Bilder: `montage_p1n2_cut4.png` F450 Arm unten -> F452-F458 Arm steigt (7-Bild-Blende) -> F458..F490
oben -> F492..F508 sinkt -> F512..F520 unten; `montage_p1n2_cut6.png` F522..F540 Drehung + Gang, **kein zweiter Hub**.
Damit: **ein Heben (F451), ein Senken (F491), kein weiteres** = Nutzer-Wortlaut zum Original. Vorher (v0.8.26, Messung
vorher + p1n): Hub 1 erst bei F491 (Blende auf slot 19), Fall F492-F510, **Hub 2 F521-F528 (Clip 0 vorwaerts slot
10..17)** — genau "hebt 2x, laesst einmal fallen".

## Punkt 2 — Nach Roys Tagebuch sofort die Karte mit Markern
### Messung vorher
Bau dieses Baums. Spielstand vor dem liegenden Roy wie der Riegel integration_r38_tagebuch_lesen (`probe_r36_elza_karte
re15_card.mcr 1011 -1900 6830 3072 4:0xF1 4:0xFA 3:0x74`, Elza), `RE15_CONTINUE_TEST=1 RE15_CARD_AUTO=1`,
`RE15_PRESS=square@60,right@200..350,square@380,square@480`, `RE15_SCD_TRACE=1`, `RE15_MSG_LOG=1`,
`RE15_CLIMB_TRACE=frames.txt` (Spielerzustand je Bild), Bildserie 470-620/2 (Scratchpad p2/a). Ablauf (debug.log):

| Bild | Ereignis |
|---|---|
| F494 | `[scd] F494 slot=10 @0x0E64 op=0x2B` — der geparkte Message_on 18 laeuft durch; `[tagebuch] Leser zu: Bit (9,68)=1, Kartenkette nach dem Evt_end` (Statusschirm samt Rueckblende ist DURCH, Raum sichtbar) |
| F495 | `@0x0E6A op=0x3F` Plc_motion(1,11) + `@0x0E6E op=0x43` Plc_flg 0x80 — Elza richtet sich auf (rueckwaerts), `@0x0E72 op=0x09` Sleep 30 |
| F496..F524 | Sleep (Elza steht sichtbar im Raum auf) |
| F525 | `@0x0E76 op=0x42` Plc_ret, `@0x0E78 op=0x46` Aot_reset, `@0x0E82 op=0x01` Evt_end -> `[tagebuch] Evt_end erreicht: Kartenkette Eintrag 3 ...` |
| F535 | `[hint] F535 begin` (Oeffnen-Blende des Hinweis-Schirms seit F525) |
| F653 / F771 | Folge-Hinweis 3 -> 4 / schliessen |

**Befund:** zwischen dem Schliessen des Tagebuchs (F494) und der Karte (F535) liegen 41 Bilder mit sichtbarem Raum, in denen
Elza sich aufrichtet und der Faden bis Plc_ret/Evt_end laeuft; der Hinweis wird erst am Evt_end angefordert
(tagebuch_r38.c re15_tagebuch_r38_tick, `faden_im_ereignis`). Der Nutzer will die Karte DIREKT nach dem Schliessen, vor
der Rueckgabe des Spiels.

### RE-Belege / Form
* **RE2 Kartenhinweis = Skript-Opcode 0x84, Handler @0x800591C4** (Runde 30/38, re2_disasm.py): `addiu v0,zero,4` @0x800591d4 /
  `sb v0,[0x800D5C00]` @0x800591dc (Statusschirm-Modus 4), `addiu v0,zero,1` / `sb v0,[0x800DF348]` @0x800591e8
  (Anforderung), `ori v1,v1,0x8000` / `sw v1,[0x800CFBD8]` @0x800591fc (Bit 0x8000), `lbu v0,1(v0)` / `sb v0,[0x800D69F2]`
  @0x80059210 (Hinweis-Nummer = Skriptbyte). Der Hinweis ist ein SKRIPT-Opcode: er laeuft an der Stelle, an der das Skript
  ihn ruft — auch mitten in einem Ereignis vor Plc_ret. Die Spielschleife startet den Status-Task ueber die Anforderung
  (@0x80025BA8-E0, Bit 0x8000 bleibt bis zum Start stehen); `bne v0,zero,0x80026540` @0x80026404 ueberspringt den
  Tasten-Oeffnen-Zweig (keine Oeffnen-Sperre durch den Spielerzustand).
* **Form im Port (vorhanden, Runde 30/35/38):** re15_map_hint_request -> game_step_common.c nimmt die Anforderung VOR dem
  START-Poll an (re15_menu_request_map_hint, s_stage=1 = Oeffnen-Blende Stufe 1 @0x8001ca64-88), hint_open @ Phase 0,
  Kette Comms (Blatt 3/9) -> Prison (Blatt 1/3), zeitgesteuert (Runde 35 K). Diese Form bleibt; es aendert sich nur der
  ZEITPUNKT der Anforderung: statt "am Evt_end des Fadens" jetzt "sobald der Dokument-Schirm zu ist", waehrend der Faden
  noch auf dem Message_on parkt (die Original-Nachricht msg 18 hielte mit Maske 0xffff ohnehin alles an, sub07 @0x00E64
  `2b 12 ff ff`). = NUTZER-VORGABE fuer den Zeitpunkt, FORM = RE2-Opcode-0x84-Anforderung.
* Die Annahme laeuft im selben game_step wie die Freigabe des Dokument-Schirms (close_phase: `s_alive = 0`, `s_stage = 0`,
  Rueckblende -0x1800/7 @0x8001cc00-18 scharf; menu_common.c:1495ff), weil re15_tagebuch_r38_tick VOR
  `re15_map_hint_pending()` laeuft (game_step_common.c:1328-1331): die Hinweis-Abblende Stufe 1 startet im naechsten Bild
  aus dem noch dunklen Bild — gemessen in "Messung nachher" (Helligkeit je Bild).

### Umsetzung
`engine/src/tagebuch_r38.c` (Runde-38-Modul, Zustandsautomat erweitert: FREI -> LESER -> **HINWEIS** -> FREI, bzw. ZU):
`leser_zu_pruefen()` laeuft im `re15_tagebuch_r38_tick` (game_step_common.c VOR `re15_map_hint_pending`) und im
Message_on-Haken; sobald der Dokument-Schirm samt Rueckblende durch ist (`!re15_menu_doc_active() &&
!re15_menu_gameplay_frozen()`) und das Tagebuch in diesem Lauf abgelegt wurde (Bit (9,68) neu), wird die Kartenkette
SOFORT angefordert (`re15_map_hint_request(k)`, k = Eintrag ROOM10F1 -> Folge ROOM1211, Runde 38 G) und der Faden bleibt
auf dem Message_on @0x00E64 geparkt (Haken liefert 2), bis `re15_map_hint_pending() < 0 && !re15_menu_map_hint_active()
&& !re15_menu_gameplay_frozen()` — erst dann Rueckgabe 1 (PC hinter das Message_on: Evt_next @0x00E68, Plc_motion(1,11)
+0x80 @0x00E6A/@0x00E6E = Elza richtet sich auf, Sleep 30, Plc_ret @0x00E76, Aot_reset @0x00E78, Evt_end @0x00E82).
Die alte Evt_end-Kette (`s_kette`/`faden_im_ereignis`) ist entfernt; `re15_tagebuch_r38_hinweis_faellig()` = Zustand
HINWEIS. Keine neue Konstante. Form = RE2-Opcode 0x84 (Anforderung @0x800591e8, Status-Task-Start im naechsten Bild
@0x80025BA8-E0), Zeitpunkt = NUTZER-VORGABE. Riegel der Runde 38 angepasst (`tests/unit/test_r38_tagebuch.c` ablauf,
`tests/integration/test_r38_tagebuch.cmake` lesen: Kette sofort nach dem Leser, Faden erst nach dem Hinweis).

### Messung nachher
Lauf p2n (Spielstand vor Roy wie oben, `RE15_PRESS=square@60,right@200..350,square@380,square@480`, STATE_LOG,
FADE_LOG, Bildserie 470-530/1; Scratchpad p2n/, `montage_p2.png`) und `integration_r39_elza_karte` (debug.log):

| Bild | Ereignis |
|---|---|
| F91 | Message_on @0x0E64 geparkt, `[tagebuch] ROOM1011 msg 18 -> Roys Diary (... erstes Mal: ablegen)` |
| F480..F485 | Abblende des Dokument-Schirms (Helligkeit 11.8 -> 0.3), F486..F489 schwarz |
| F490..F494 | Rueckblende des Statusschirms (-0x1800/7 @0x8001cc00-18): 1.4 / 5.7 / 24.6 / 37.4 / 37.4 = Raum sichtbar |
| **F494** | `[scd] F494 slot=10 @0x0E64 op=0x2B` (Faden parkt) + `[tagebuch] Leser zu: Bit (9,68)=1, Kartenkette sofort (Runde 39 Spur H): Eintrag 3 (ROOM10F1 Blatt 3 Rechteck 9) -> Folge 4 (ROOM1211 Blatt 1 Rechteck 3)` — Anforderung im selben game_step angenommen |
| F495..F497 | Oeffnen-Blende des Hinweises (Stufe 1): 8.7 / 2.3 / 0.2; F498..F505 schwarz |
| **F504** | `[hint] F504 begin (Zaehler 10, Richtung 1)`; F506..F510 Karte blendet ein (0.6 -> 47.4) |
| F622 | `[hint] F622 Folge-Hinweis 3 -> 4 (Zeit)` (Prison) |
| F740 | `[hint] F740 schliessen` |
| **F754** | `[scd] F754 slot=10 @0x0E64 op=0x2B` + `[tagebuch] Hinweis zu: Faden laeuft hinter dem Message_on weiter` |
| F755 | `@0x0E6A op=0x3F` Plc_motion(1,11)+0x80 — ERST JETZT richtet Elza sich auf; F785 `@0x0E76 op=0x42` Plc_ret, `@0x0E82 op=0x01` Evt_end |

Spielerzustand (STATE_LOG) F487..F754 unveraendert (pst=1 mo=210, kein Spieler-Tick: F494 nimmt die Anforderung VOR dem
Spieler-Tick an, ab F495 `re15_menu_gameplay_frozen()`), d.h. **zwischen Tagebuch und Karte gibt es kein Bild mit
Steuerung** und keine Szene (vorher: 41 Bilder Raum mit Aufrichten + Plc_ret). Sichtbar bleibt die 5-Bild-Rueckblende
des Dokument-Schirms (F490-F494, Raum 2 Bilder voll hell) vor der Oeffnen-Blende des Hinweises — das ist die Form des
RE2-Status-Tasks (er startet im naechsten Bild aus dem sichtbaren Spielbild @0x80025BA8-E0; ein Verketten zweier
Statusschirme ohne Rueckblende kennt RE2 nicht). Die Erwartung der ersten Sitzung ("Oeffnen-Blende startet aus dem
noch dunklen Bild") traf NICHT zu: `re15_menu_gameplay_frozen()` bleibt waehrend der Rueckblende wahr, die Anforderung
faellt erst in das Bild nach deren Ende.

## Punkt 3 — ROOM1091: Elza klettert nicht aufs Autowrack
### Messung vorher
Bau dieses Baums. Spielstand neben dem leeren Loescher (Kartenwerkzeug `probe_r37_feuerloescher_karte`, wie Lauf C des
Riegels test_r37_feuerloescher.cmake): `1091 1880 -1300 <rot> y:-1800 cut:6 elza flag:7:0xf8..0xfb`, dann
`RE15_CONTINUE_TEST=1 RE15_CARD_AUTO=1`, Eingabeskript `W1,A0.3,W4` (Viereck = Aktion), `RE15_CLIMB_TRACE=climb.txt`,
`RE15_MSG_LOG=1`, Messkopie der exe (`re15_pc_h3.exe`). Vier Blickrichtungen (Scratchpad p3/rot*):

| rot_y | Sondenpunkt (800 voraus, auf 90 Grad gerastet) | Ergebnis bei Viereck (T91) |
|---|---|---|
| 0 | (2680,-1300): dz = 0 zum Loescher | kein Klettern; AOT-Platz: `[msgtext] room=1091 id=29 "It's an empty fire extinguisher."` |
| **1024** | (1880,-2100): dx -620 / dz -800 | **`sub=9 kind=0x81 obj=4`** (Aufstieg, Objekt-Art; obj 4 = Pool-Index des Port-Loeschers, `[prop-render] pi=4 oid=0x05`) |
| 2048 | (1080,-1300) | kein Klettern |
| **3072** | (1880,-500): dx -620 / dz +800 | **`sub=9 kind=0x81 obj=4`** |

Trace-Zeile (rot 1024): `T91 room=1091 pm=0 pad=8000 pos=(1880,-1800,-1300) rot=1024 band=1 sub=9 kind=0x81 ph=0 obj=4
stand=-1 ...`, T92 `mo=230` (Kletter-Sentinel), T93 `mo=231`. **Reproduziert:** neben dem Loescher startet die Aktionstaste
aus schraeger Lage den Aufstieg (an einem Objekt, das kein Tritt ist).
**Leon ROOM1090** (Spielstand mit (9,0x86)/(3,0x85)/(3,0x81), Loescher abgelegt): rot 1024 und 3072 ebenfalls `sub=9
kind=0x81 obj=3` — derselbe Defekt, nur bei Leon erst nach dem Loeschen.

### RE-Belege
* **FUN_8002d474 (Aktions-Sonde, climb_common.c re15_climb_try_start):** Sondenpunkt {800,0} um den gerasteten Kurs
  (@0x8002d4d4-dc, @0x8002d518-28); `FUN_8002d2c0(&probe)` @0x8002d530; Treffer -> Substate 9 @0x8002d604, KIND 0x81
  @0x8002d614/18.
* **FUN_8002d2c0 (Objekt an der Sonde, selbst disassembliert):** Schleife rueckwaerts ueber den Objekt-Pool 0x800b3f98
  (Stride 148, @0x8002d32c-4c); `lw v0,0(s0)` / **`andi v0,v0,0x101` @0x8002d358 / `beq v0,zero` @0x8002d35c** (nur
  Flags 0 werden uebersprungen — der Installer LAB_80040914 setzt Bit 0 IMMER, `ori v0,v0,0x1` @0x80040998);
  **`lbu v1,0(s3)` (0x800acad6 = Spieler-Band) / `lbu v0,130(s0)` (Objekt +0x82 = Band) @0x8002d364-68 -> Band-Gate
  Objekt-Band == Spieler-Band**; dann FUN_8002d1e8 (Reichweite) @0x8002d37c und FUN_8002da4c(probe, obj, -112, Band)
  @0x8002d394; Treffer `sw s0,-842(s3)` @0x8002d304 (DAT_800ac78c) + Rueckgabe 1.
* **FUN_8002d1e8 (Reichweite, selbst disassembliert):** Punkt {0x233=563,0} (@0x8002d204) um +0x6a (rot_y, @0x8002d218)
  gedreht (jal 0x8004f008 @0x8002d21c) + Spieler-x/z; a3 = 0x384 = 900 (Modus 0) / 0x2bc = 700 (Modus 1, @0x8002d280);
  dx = px - (Box-cx + obj.x) @0x8002d258-64; Treffer iff (dx + r) <= 2r vorzeichenlos (@0x8002d284-8c) und dz ebenso
  (@0x8002d298-a0). **Box-GROESSEN-unabhaengig** — eine Null-Box faellt hier nicht durch.
* **FUN_8002da4c (Box-Treffer, selbst disassembliert):** Band-Gate `lbu v0,130(t0)` / `bne v0,a3` @0x8002da50-58;
  Box-Zeiger `lw a2,120(t0)` (+0x78); dx = px - (`lh v0,0(a2)` cx + `lw v1,52(t0)` x) @0x8002da64-74;
  **`lhu v0,6(a2)` @0x8002da8c (Halbbreite hx VORZEICHENLOS), `addu v0,v0,t1` @0x8002da94 (hx + Rand), `addu a1,a1,v0`
  (dx + hx + Rand), `sll v0,v0,1` @0x8002da9c (2*(hx+Rand)), `sltu v0,v0,a1` @0x8002daa0** -> Fehlschlag nur, wenn
  2(hx+Rand) < dx+hx+Rand vorzeichenlos; Z gleich @0x8002dab4-c0 mit `xori v0,v0,0x1`. Bei **hx = 0 und Rand -112**
  ist hx+Rand = 0xFFFFFF90 und die Bedingung kippt: **Treffer genau dann, wenn |dx| >= 112 UND |dz| >= 112** (schraege
  Lage), nie achsgerade — deckt sich mit der Messtabelle (rot 0/2048 dz=0 -> kein Treffer; 1024/3072 -> Treffer).
  Dasselbe Verhalten ist in Runde 36 Spur A fuer die Original-Null-Box ROOM3091 obj 1 gemessen (probe_r36_klettern_zensus,
  "Null-Box"-Wahrheitstafel).
* **Der Port-Loescher** (Runde 37 Spur I, feuerloescher_r37.c Z. 174-187, Konstanten re15_feuerloescher_r37.h:60-63):
  `obj_type 0`, **`band = RE15_FL37_PROP_BAND = 1`** ("wie die Item-Props des Originals ROOM1010 @0x00930 pc[4]"),
  `flags = 0x000B`, Box per `memset` = **Null-Box**. In ROOM1090/1091 steht der Spieler am Wagen auf Band 1 (Trace
  `band=1`, y -1800 = `-(y/0x708)`, re15_collision.c:188) -> Band-Gate wahr -> Null-Box-Schraegtreffer -> Aufstieg.
* **Was das Original mit Null-Box-Props im selben Raum tut** (ROOM1091 main00 @0x02202, ROOM1090 sub00 @0x0237E, Bytes
  `2d 02 00 00 02 00 08 00 10 00 6a 09 a3 f5 2e ff 40 00 40 07 00..00`): obj 2 am Wagen, Typ 0, Flags 0x0008|1,
  Attribut 0x0010, Box c(0,0,0) h(0,0,0), **Band 2 = Spielerband + 1** -> faellt am Band-Gate @0x8002d364-70 durch.
  Die kletterbare Kiste obj 0 (@0x021BE / @0x0216E) traegt Band 1 = Spielerband und Box h(900,900,900). ROOM1010 @0x00930
  (First Aid Spray, Vorlage des Port-Loeschers): Band 1 bei Bodenhoehe 0 (= Spielerband 0) — also ebenfalls Spielerband + 1.
  Zensus ueber alle 240 RDTs: s. Umsetzung (probe_r39_elza_zensus).
* BERICHTIGT (zweite Sitzung, RDT-Bytes selbst gelesen): die Vorlagen stehen NICHT auf Bodenhoehe. ROOM1091 @0x02202 =
  ROOM1090 @0x0237E: `2d 02 00 00 02 00 08 00 10 00 6a 09 a3 f5 2e ff 40 00 40 07 00..00` -> obj 2, Typ 0, **Band 2**,
  Flags 0x0008, Attribut 0x0010, Position (2410, **-2653**, -210) am Wagen, Box h(0,0,0); Spielerband(-2653) =
  -(-2653/0x708) = 1 -> Band = Spielerband(y) + 1. ROOM1010 @0x00930: `2d 00 00 00 01 00 0a 00 00 00 c8 00 c0 f9 7c 15
  00 00 0c 0c 00..00` -> obj 0, **Band 1**, Flags 0x000A, Position (200, **-1600**, 5500), h(0,0,0); Spielerband(-1600)
  = 0 -> ebenfalls +1. Die Regel lautet also "Band = Spielerband der EIGENEN Hoehe + 1".
* ZENSUS WIDERLEGT die erste Fassung "Null-Box-Records liegen NIE auf dem Spielerband" (unit_r39_elza_klettern_zensus,
  alle 240 RDTs, 672 Obj_model_set-Records, 0 Walk-Abbrueche): 558 Null-Box-Records, Verteilung Band - Spielerband(y):
  -8:6 -6:6 -4:8 -3:6 -2:46 -1:22 **+0:51** **+1:291** +2:74 +3:36 +4:6 +5:4 +7:2 (mit Box: -8:2 -1:12 +0:77 +1:19 +2:4).
  51 Original-Records liegen AUF dem Spielerband (ROOM1040/1041 obj 1-3, ROOM1110/1111 obj 2, ROOM1190/1191 obj 7-11/15,
  ROOM11A0/11A1 obj 1, ROOM1200/1201 obj 0/1, ROOM2060/2061 obj 0/1, ROOM3050/3051 obj 2, ROOM3071 obj 1, ROOM4040/4041
  obj 3, ROOM4050/4051 obj 4/5, ROOM5030/5031 obj 6/8/9, ROOM5060/5061 obj 3/8) — fuer sie traefe FUN_8002da4c aus
  schraeger Lage (Eigenheit des Originals, im Port unveraendert abgebildet, climb_common.c nicht angefasst). +1 ist mit
  291/558 der haeufigste Abstand, und die drei Vorlagen (am selben Wagen bzw. die Item-Prop-Vorlage des Port-Loeschers)
  tragen +1. Die Regel ist damit **PORT-WAHL in der Form der Vorlagen-Records**, kein "Original-Gesetz".

### Umsetzung
`engine/src/elza_r39.c` `re15_elza_r39_prop_band_ueber_boden(y) = re15_collision_band_from_y(y) + 1` (Spielerband =
-(y/0x708), re15_collision.c:186); `engine/src/feuerloescher_r37.c` Installer: `props[i].band = ...(g->y)` statt fest
`RE15_FL37_PROP_BAND = 1` (Makro entfernt, `include/re15_feuerloescher_r37.h`). Wirkung: ROOM1090/1091 (y -1800,
Spielerband 1) Band 2; ROOM1030 (y -2122, Spielerband 1) Band 2 — der r37-Riegel `unit_r37_feuerloescher_raum` erwartete
dort "Band 1" und wurde auf 2 gesetzt (Begruendung im Test). Das Band wird im Port nur von der Klettersonde
(climb_common.c:103 @0x8002d370, :153/:201 Standobjekt +1), vom AOT-Objekt-Band-Vergleich (aot_common.c:525/1435/1761
@0x8002bf38) und von Protokollen gelesen; die Untersuchen-/Aufnahme-Plaetze des Loeschers tragen ihr eigenes AOT-Band
(`g->band`, unveraendert). Die Original-Kiste obj 0 (Box 900, Band 1) bleibt kletterbar.

### Messung nachher
* `integration_r39_elza_loescher` (Elza ROOM1091, Spielstand neben dem leeren Loescher, `RE15_INPUT_SCRIPT=W1,A0.3,W3`,
  CLIMB_TRACE): rot 1024: `T91 room=1091 pm=0 pad=8000 pos=(1880,-1800,-1300) rot=1024 band=1 sub=0 kind=0x00 ph=0 obj=-1`
  (vorher `sub=9 kind=0x81 obj=4`), 0 Zeilen `sub=[1-9]` in allen Viereck-Bildern; rot 3072 ebenso; rot 0: kein Aufstieg
  und `[msgtext] room=1091 id=29` ("It's an empty fire extinguisher.") — Untersuchen-Platz unveraendert.
  `[feuerloescher] ROOM1091: Loescher liegt leer am Boden obj 5 bei (2500,-1800,-1300) rot_y 3250, Zone Slot 3`.
* `unit_r39_elza_klettern_loescher` (echter Raumstart mit allen Port-Installern): Elza ROOM1091 **Loescher Band 2**,
  Null-Box h 0/0 Flags 0x000B, Sonde (1880,-1300) rot 0/1024/2048/3072 -> `sub=0 kind=0x00 obj=-1` (0/4), Kiste obj 0
  (h 900, Band 1) -> `sub=9 kind=0x81 obj=0`, Slot 3 MESSAGE steht; Leon ROOM1090 (Feuer aus (3,0x81), (9,0x86),
  (3,0x85)): dasselbe, Slot 6.
* Leon ROOM1090 an der exe (Scratchpad p3n_leon/, Spielstand mit (9,0x86)/(3,0x85)/(3,0x81), Viereck rot 1024/3072/0):
  `[p3n leon rot 1024] rc=0 resumed=1 loescher=1 pad8000=9 sub9=0 msg29=0`, `[p3n leon rot 3072] ... sub9=0 msg29=0`,
  `[p3n leon rot 0] ... sub9=0 msg29=1`; CLIMB_TRACE rot 1024: `T91 room=1090 pm=0 pad=8000 pos=(1880,-1800,-1300)
  rot=1024 band=1 sub=0 kind=0x00 ph=0 obj=-1 stand=-1 mo=200` (vorher `sub=9 kind=0x81 obj=3`) — Leon klettert am
  Loescher ebenfalls nicht mehr, Untersuchen (msg 29) bei rot 0 wie bisher.

## Tests
Neu (`tests/unit/probes/r39_elza.cmake`, GLOB; nur dort registriert):
* `unit_r39_elza_hand` (`tests/unit/test_r39_elza_hand.c`): (a) Latch scharf + Plc_motion(0,11) vor dem ersten
  Spieler-Tick -> zwei Ticks Motion 11/11, Latch 0 (vorher 210/200); (b) Szene ohne Plc_motion -> 210 dann 200 (Fall 0
  @0x80032088); (c) Spiel (pm 0) -> Idle-Zweig; (d) Punkt 1b: Walker-Init Modus 9/4/5 -> Motion 105, Bild 0, Blende 7,
  walk_fsm 1; Nicht-Spieler unberuehrt. 11 Pruefungen.
* `unit_r39_elza_klettern_zensus` / `_loescher` (`tests/unit/test_r39_elza_klettern.c`): Zensus (Vorlagen +1, +1
  haeufigster Abstand, Verteilung gedruckt) / Raumstart Elza 1091 + Leon 1090 (s. Messung nachher).
* `integration_r39_elza_hand` / `_karte` / `_loescher` (`tests/integration/test_r39_elza.cmake`, exe-Kopie
  `re15_pc_r39elza_<teil>`, Laufordner `r39_elza_wd_<teil>/<lauf>`): hand = STATE_LOG (Motion 11 ab Plc_motion, 0 danach)
  + ANIM_TRACE (Clip 0 nur rueckwaerts bis Cut_chg(6), ab dort Clip 5, nie Clip 0 vorwaerts); karte = Hinweis-Beginn vor
  Plc_motion/Plc_ret/Evt_end, Faden erst nach dem Hinweis, keine msg 18; loescher = rot 1024/3072/0 ohne sub=9, msg 29.
Angepasst: `unit_r38_tagebuch_ablauf` + `integration_r38_tagebuch_lesen` (neuer Zeitpunkt der Kette),
`unit_r37_feuerloescher_raum` (ROOM1030 Band 2).

Gezielte Laeufe (KEINE volle Suite):
* (A) `ctest --test-dir re15_port/build -R "^unit_(r39_elza|r38_tagebuch|r37_feuerloescher|r36_klettern|elliot|plc_|npc_back|1090|r35_cut|r36_szene|r37_roy|r38_heliport|r34n_d_adaruf|r33_tuer1120|r35_affen|r38_szenenschutz|r38_dining|tor_1170|r30_android|stair_1170|door_1170|irons_neck|sicherung_1150|room1150|map_1090|re2_room1090|re2z_bump)" --timeout 600 --output-on-failure`
  -> **100% tests passed, 0 tests failed out of 129** (Walker-Haken: alle Szenen-/Walker-Riegel der Vorrunden gruen).
* (B) `ctest --test-dir re15_port/build -R "^integration_r39_elza_(hand|karte|loescher)$" --timeout 1200 --output-on-failure`
  -> **100% tests passed, 0 tests failed out of 3** (hand 84 s, karte 51 s, loescher 51 s; Lauf nach dem Walker-Haken).
* (C) `ctest --test-dir re15_port/build -R "^integration_(r38_tagebuch_lesen|r37_feuerloescher)$" --timeout 1800 --output-on-failure`
  -> **100% tests passed, 0 tests failed out of 2** (r37_feuerloescher 353 s, r38_tagebuch_lesen 44 s);
  `ctest --test-dir re15_port/build -R "^integration_r38_tagebuch_(karte|leon)$" --timeout 1200 --output-on-failure`
  -> **100% tests passed, 0 tests failed out of 2** (karte 23 s, leon 28 s).
* Riegel-Zeilen (LastTest.log): `ok r39_elza[hand]: F452..F490: 39 Ticks Motion 11 / Kommandowort 4, keiner auf 210/200`;
  `ok r39_elza[hand]: F491..F521: 30 Bilder Clip 0 rueckwaerts, 0 vorwaerts; ab Cut_chg(6) F521: 8/8 Bilder Clip 5 (105,
  TURN-Sub-Init @0x800313a4) — die Hand steigt kein zweites Mal`; `ok r39_elza[karte]: Hinweis F504..F687 VOR Plc_motion
  F702 / Plc_ret F732 / Evt_end F732` (Hinweis-Dauer ist zeitgesteuert, Runde 35 K — Bildnummern je Lauf verschieden,
  Reihenfolge gleich); `ok r39_elza[loescher]: rot 1024/3072/0: kein Aufstieg (sub bleibt 0 in allen 9 Viereck-Bildern)`,
  `rot 0: msg 29`.

## OFFEN
* Punkt 1: ein Bild Sub 6 (mo=1 = Sub-6-Clip 1) zwischen TURN-Ankunft (F530) und WALK-Start (F532) — Reihenfolge
  VM-Poll (Gosub 0x0e) vs. Spieler-Dispatcher im Port; vorher ebenso (p1n F531), kein Hub, Arm bleibt unten. Nicht Teil
  des Wortlauts; eigener RE-Punkt (Dispatcher-Reihenfolge @0x80031c44 vs. SCD-Tick).
* Punkt 1b aendert die Align-Phase JEDES Spieler-Plc_dest (Modus 4/5/9): der Lauf-Clip laeuft ab dem Init (byte-true zu
  @0x80030b58/@0x80030d90/@0x800313a4), vorher stand der Spieler. 129 Szenen-/Walker-Riegel gruen; die Sichtpruefung
  anderer Szenen (1150, 1170, 1090-Rettung) steht dem Integrationsbaum/Nutzer zu.
* Punkt 2: die 5 Bilder Rueckblende des Dokument-Schirms vor der Hinweis-Blende (RE2-Form). Wer die Karte ohne jeden
  Blick auf den Raum will, braeuchte einen Port-eigenen Mechanismus (Rueckblende ueberspringen) ohne RE2-Vorbild — nicht
  umgesetzt. Ausserdem (r38-Stand, unveraendert): beim Schliessen des Dokument-Schirms steht der Spieler auf pst=1
  mo=210 (Settle), obwohl der Faden noch unter Plc-Kontrolle parkt — Herkunft nicht untersucht.
* Punkt 3: 51 Original-Null-Box-Records liegen auf dem Spielerband (Liste oben) — byte-true unveraendert; ob eine davon
  im Spiel erreichbar und damit "kletterbar" ist, ist nicht gemessen (Runde 36 A: 3091 obj 1 statisch unerreichbar).

## Fuer den Nutzer
* Keine neuen Assets, keine Sprachdateien, keine Paket-Gate-Aenderung (nur Code + Tests).
* Elza ROOM1031, erste Szene: Hand hebt sich einmal (Blende ueber 7 Bilder), bleibt 40 Bilder oben, senkt sich, dann
  dreht sie sich mit Lauf-Clip zum Gitter — kein zweiter Hub.
* Roys Tagebuch: nach dem Ablegen ("filed") kommt die Karte (Communications Room, dann Prison) direkt nach der
  Abblende des Dokuments; Elza richtet sich erst danach auf. Dazwischen kurz (5 Bilder) der Raum in der Rueckblende.
* Loescher in 1090/1091 (und 1030): kein Klettern mehr aus schraeger Lage; Untersuchen/Aufnehmen wie bisher.
* Nebenwirkung Punkt 1b: in allen Szenen mit skriptgesteuertem Spieler-Lauf beginnt der Lauf-Clip schon waehrend der
  Drehung (wie im Original), nicht erst nach ihr.

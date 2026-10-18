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
- Messung vorher + RE-Belege aller drei Punkte (s. Abschnitte); Umsetzung folgt.

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
(folgt)
### Messung nachher
(folgt)

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
(folgt)
### Messung nachher
(folgt)

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

### Umsetzung
(folgt)
### Messung nachher
(folgt)

## Tests
(folgt)

## OFFEN
(folgt)

## Fuer den Nutzer
(folgt)

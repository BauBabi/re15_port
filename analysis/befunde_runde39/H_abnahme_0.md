# Runde 39 — Spur H "elza": ABNAHME 0 (unabhaengig, 2026-10-18)

Baum `.claude/worktrees/r39_elza`, Zweig `r39/elza`, HEAD 564810c0 (5 Commits ueber Basis c71fa144; master-only
Commits de4d7b74/3eda01a4 sind reine Dokumente — der `git diff master`-Eintrag "UEBERGABE.md/args_welle2_forts.json
geloescht" ist KEINE Aenderung der Spur, sondern der Vorsprung von master; `git diff <merge-base>` enthaelt nur die
16 Spur-Dateien). Abnahme-Werkzeuge: eigene exe-Kopien `re15_pc_abn_*.exe` neben der exe, Laeufe im Scratchpad
`abn_H_elza/`, kein taskkill.

## 0. Bau und Riegel (selbst gefahren)
* `local_build.sh configure` + `build`: LOCAL-BUILD-OK. Die vorhandene exe (12:28) war laut ninja aktuell; zur
  Sicherheit alle fuenf geaenderten Quelldateien angestossen und neu gebaut -> exe 12:44:28 (7 593 276 Byte) = HEAD.
* Unit, gezielt (Regex des Bau-Agenten + `r36_elza|r16_|r30_elza|rvd_scan`): **144/144 gruen** (24 s).
  Darunter `unit_r39_elza_hand` (11 Pruefungen), `unit_r39_elza_klettern_zensus` (4) / `_loescher` (14),
  `unit_r38_tagebuch_ablauf`, `unit_r37_feuerloescher_raum`.
* Integration `^integration_r39_elza_(hand|karte|loescher)$`: **3/3 gruen** (100 s / 50 s / 38 s), Riegel-Zeilen:
  hand `F452..F490: 39 Ticks Motion 11 / Kommandowort 4`, `F491..F521: 30 Bilder Clip 0 rueckwaerts, 0 vorwaerts;
  ab Cut_chg(6) F521: 8/8 Bilder Clip 5`; karte `Hinweis F504..F721 VOR Plc_motion F736 / Plc_ret F766 / Evt_end
  F766`; loescher `rot 1024/3072/0: kein Aufstieg`, `rot 0: msg 29`.
* Zensus selbst nachgerechnet (`test_r39_elza_klettern.exe zensus`): 240 RDT, 672 Obj_model_set-Records, 558
  Null-Box, Verteilung Band − Spielerband(y): +0:51, **+1:291**, +2:74 … — die Zahlen des Dossiers stimmen.

## 1. RE-Gate: Stichproben SELBST disassembliert (`re15_disasm.py dis`, PSX.EXE Auslieferung; RE2 `re2_disasm.py`)
| Behauptung im Diff | Bytes/Instruktionen gefunden | Urteil |
|---|---|---|
| Plc_motion-Handler: `+0x94 := Clip`, `+0x4 := 4` @0x80041bb0 | `80041ba4 ori v1,zero,0x4` / `80041ba8 sb a1,148(v0)` / `80041bb0 sb v1,4(v0)` / `80041bb4 sb zero,6(v0)` / `80041bc4 sb a2,5(v0)` | stimmt |
| Dispatcher FUN_80031c44: Kommandowort @0x80031c8c, Tabelle @0x80073f90, `jalr` @0x80031cb4, Pause-Gate @0x80031c54/c78 | `80031c54 lw a0,-13760(a0)` (g_pauseflags) / `80031c78 bltz a0` / `80031c8c lbu v1,-13736(v1)` (0x800aca58) / `80031c94 sll v1,v1,2` / `80031ca4 addiu at,at,16272` / `80031cac lw v0,0(at)` / `80031cb4 jalr v0` | stimmt |
| Tabelle @0x80073f90: [0] cmd-0, [1] Idle-FSM, [4] = 0x80030660 | `table 0x80073f90`: [0] 0x800318f8, [1] 0x80031de8, [4] 0x80030660 | stimmt |
| TURN Sub 9 @0x80031360: Phase := 1 @0x80031398, `+0x94 := 5` @0x800313a4, `+0x95 := 0` @0x800313b0, `+0x8f := 7` @0x800313b8; arc_test 0x60 @0x800313d4; Ankunft Sub := 6 @0x800313ec | `80031398 sb v0(=1),0(a0)` / `8003139c ori v0,zero,0x5` / `800313a4 sb v0,-13592(at)` (0x800acae8 = +0x94) / `800313b0 sb zero,-13591(at)` / `800313b8 sb v0(=7),-13597(at)` / `800313d0 jal arc_test` + `ori a2,0x60` / `800313ec sb v0(=6),-13735(at)` | stimmt |
| WALK Sub 4 @0x80030af0: `+0x94 := 5` @0x80030b58, `+0x95` @0x80030b64, `+0x8f := 7` @0x80030b6c, Phase 1→2 @0x80030b94, arc_test 0x15e | `80030b58 sb v0(=5),-13592(at)` / `80030b64 sb zero,-13591(at)` / `80030b6c sb v0(=7),-13597(at)` / `80030b84 ori a2,0x15e` / `80030b94 sb v0(=2),-13734(at)` | stimmt |
| RUN Sub 5 @0x80030d28: `+0x94 := 5` @0x80030d90; Phase 2: `+0x94 := 0` @0x80030dd8, `+0x95 := 0` @0x80030de0 | `80030d90 sb v0(=5),-13592(at)` / `80030dd8 sb zero,-13592(at)` / `80030de0 sb zero,-13591(at)` / `80030de8 sb v0(=7),-13597(at)` | stimmt |
| Plc_dest nullt `+0x1c4` @0x80041c4c | `80041c4c sh zero,452(a1)` | stimmt |
| FUN_8002d2c0: `andi 0x101` @0x8002d358, Band-Gate `lbu v1,0(s3)` / `lbu v0,130(s0)` / `bne` @0x8002d364-70, Reichweite `jal 0x8002d1e8` @0x8002d37c, Box `jal 0x8002da4c` mit a2 = −112 @0x8002d394/98 | `8002d358 andi v0,v0,0x101` / `8002d364 lbu v1,0(s3)` (s3 = 0x800aca3c+154 = 0x800acad6) / `8002d368 lbu v0,130(s0)` / `8002d370 bne v1,v0` / `8002d37c jal 0x8002d1e8` / `8002d394 jal 0x8002da4c` / `8002d398 addiu a2,zero,-112` | stimmt |
| FUN_8002da4c: `lhu v0,6(a2)` @0x8002da8c (vorzeichenlos), `+ Rand` @0x8002da94, `sll 1` @0x8002da9c, `sltu` @0x8002daa0; Band-Gate @0x8002da50-58 | `8002da50 lbu v0,130(t0)` / `8002da58 bne v0,a3` / `8002da8c lhu v0,6(a2)` / `8002da94 addu v0,v0,t1` / `8002da98 addu a1,a1,v0` / `8002da9c sll v0,v0,1` / `8002daa0 sltu v0,v0,a1` | stimmt |
| RE2 Opcode 0x84 @0x800591C4: Modus 4 @0x800591d4/dc, Anforderung @0x800591e8, Bit 0x8000 @0x800591fc, Nummer @0x80059210 | `800591d4 addiu v0,zero,4` / `800591dc sb v0,[0x800d5c00]` / `800591e8 sb v0(=1),[0x800df348]` / `800591ec ori v1,v1,0x8000` / `800591fc sw v1,[0x800cfbd8]` / `80059210 sb v0,[0x800d69f2]` | stimmt |
| RDT-Bytes: ROOM1091 @0x02202 = ROOM1090 @0x0237E `2d 02 00 00 02 00 08 00 10 00 6a 09 a3 f5 2e ff …` (Band 2, y −2653, Box 0); ROOM1010 @0x00930 `2d 00 00 00 01 00 0a 00 00 00 c8 00 c0 f9 7c 15 …` (Band 1, y −1600) | `xxd` der shared_assets-RDTs: Byte fuer Byte wie zitiert | stimmt |
| ROOM1031 sub13: Plc_motion(0,11) @0x029A4 `3f 00 0b 00`, Plc_flg 0x04 @0x029A8, Plc_motion(0,0) @0x029BE, Plc_flg 0x80 @0x029C2, Cut_chg(6) @0x029CA `29 06`, Plc_dest(0,9,0x20,-9097,-21855) @0x029CC `40 00 09 20 77 dc a1 aa` | `xxd -s 0x2982 ROOM1031.RDT` | stimmt |
| ROOM1011 sub07: Message_on 18 @0x00E64 `2b 12 ff ff`, Plc_motion(1,11) @0x00E6A, Plc_flg 0x80 @0x00E6E, Plc_ret @0x00E76, Evt_end @0x00E82 | `xxd -s 0xE60 ROOM1011.RDT`: `2b12 ffff 0200 3f01 0b00 4300 8000 090a 1e00 4200 4605 … 0100` | stimmt |

Guess-Tells im Diff (`deferred|tunable|interim|for now|faithful|plausibel|TODO|getenv`): **keine Treffer**.
Kennzeichnung: Punkt 2 Zeitpunkt = NUTZER-VORGABE, Form = RE2 0x84 (so im Code); Punkt 3 Regel "Spielerband(y)+1" =
PORT-WAHL in der Form der drei Vorlagen-Records (so im Code und im Dossier; der Zensus belegt, dass es kein
Original-Gesetz ist: 51/558 auf dem Spielerband).

## 2. Vertrag
* Geaenderte Pfade (gegen merge-base): nur `analysis/befunde_runde39/H_elza.md`, `engine/src/{actor_locomotion,
  elza_r39,feuerloescher_r37,player_common,tagebuch_r38}.c`, `include/re15_{elza_r39,feuerloescher_r37,tagebuch_r38}.h`,
  `tests/integration/test_r3{8_tagebuch,9_elza}.cmake`, `tests/unit/probes/r39_elza.cmake`,
  `tests/unit/test_r3{7_feuerloescher,8_tagebuch,9_elza_hand,9_elza_klettern}.c`. Keine release/, platform/android/,
  shared_assets/PSX/, keine Test-CMakeLists (Probe ueber GLOB `probes/r39_elza.cmake`).
* Gemeinsame Dateien: player_common.c (+1 Zeile Haken + Kommentar), actor_locomotion.c (+1 Include, +1 Aufruf,
  Kommentar) — kleine Haken. elza_r36.c (im Vertrag fuer H vorgesehen) nicht angefasst — nicht noetig.
* Bank-9-Bits / Nachrichten-Ids / Slots: keine neuen (Punkt 2 nutzt Bit (9,68) und msg 18 aus Runde 38; Punkt 3 den
  Prop obj 5 / Slots 3/6 aus Runde 37). Keine Assets, keine synchro-Dateien.
* Tests je Punkt vorhanden und messend (s. §0).

(Fortsetzung: §3 Messungen je Punkt, §4 Urteile, §5 Maengel — werden nach jedem gemessenen Punkt ergaenzt.)

## 3. Messungen je Punkt (selbst, an der exe)

### Punkt 1 — ROOM1031 erste Szene: Hand nur einmal heben
Werkzeug: `analysis/befunde_runde38/E_elzaintro_werkzeug/e_lauf.sh` (Neues Spiel ueber Titel + Figurenwahl Elza,
`RE15_PSELECT_AUTO=1 RE15_PSELECT_AUTO_SWITCH=1`, `RE15_NO_INTRO=1`, eigene exe-Kopie `re15_pc_r38e_<pid>.exe`), dazu
`RE15_SCD_TRACE=1 RE15_STATE_LOG=state.txt RE15_ANIM_TRACE=anim.txt`, Bildserie `RE15_FRAMEDUMP=440-560/2` (Skala 1,
Waechter `e_waechter.py` benennt je Raumabschnitt um), `RE15_EXIT_AT=600#1031`. Auswertung je Bild (Scratchpad
`abn_H_elza/abn_p1.py`: ROOM1031-Block = letzter Block von state.txt/anim.txt, Bildnummern wie im Dossier; `abn_H_elza/p1_neu/`,
`abn_H_elza/p1_basis/`). Zwei Laeufe: **neu** = exe dieses Baums (HEAD), **basis** = v0.8.26-Paket-exe
(`release/pkg-win/re15_port_v0.8.26/re15_pc.exe`, sha256 a5a1c016…, als Kopie neben der Baum-exe, gleiche Assets).

| Bild | SCD (beide Laeufe gleich) | **neu**: pst / mo / Pose (anim mo clip cur slot rev) | **basis v0.8.26** |
|---|---|---|---|
| F445 | Cut_chg(4) | pst=1 mo=210 (Settle, cur 0) | gleich |
| F446-F450 | Selbsttuer-Blende, pf=FF000007 | mo=210 (kein Spieler-Tick) | gleich |
| **F451** | **Plc_motion(0,11)+Plc_flg 0x04** @0x029A4/@0x029A8 | **pst=4 mo=11, clip 11 fc 2 cur 0 slot 0** | **pst=4 mo=210** (Latch 1 ueberschreibt Clip 11) |
| F452-F490 | Sleep/Plc_neck/Sleep | mo=11, cur 1..39, **slot 1 konstant** (Haltepose, Schleife) — 39 Ticks, 0 abweichend | **mo=200** (Latch 2, Idle) ab F452 — Clip 11 nie posiert |
| **F491** | **Plc_motion(0,0)+Plc_flg 0x80** @0x029BE/@0x029C2 | mo=0 clip 0 fc 20 cur 0 **slot 19 rev=1** (Senken beginnt) | gleich (slot 19 rev=1 = Arm springt per Blende nach oben = "Hub 1" der Basis) |
| F492-F510 | | slot 18 → 0 rueckwaerts; F511-F520 slot 0 gehalten (30 Bilder rev=1, 0 vorwaerts) | gleich |
| **F521** | **Cut_chg(6)** @0x029CA + Plc_dest(0,9) @0x029CC | **mo=105 clip 5 cur 0** (TURN-Sub-Init) | **mo=0 clip 0 cur 30 slot 10 rev=0** → F522-F528 slot 11..17 = **Clip 0 VORWAERTS, Arm steigt erneut ("Hub 2")**, erst F529 mo=105 |
| F522-F530 | | mo=105 cur 1..9 (Drehung mit Lauf-Clip) | s.o. |
| F531 | | mo=1 clip 1 cur 1 (EIN Bild Sub-6-Pose, Blende 7 → optisch 1/8-Schritt; vorher ebenso) | gleich (mo=1) |
| F532-F560 | Plc_dest(0,4) | mo=105 cur 0..28 (Gang zum Gitter) | gleich |

Zaehlung (Skript): neu — "Haltephase F452..F490: 39 Ticks mo=11/pst=4, abweichend: []", "Senken F491..F520: 30 Bilder
Clip 0 rueckwaerts", "**Clip 0 VORWAERTS ab dem Senken bis F560: 0 Bilder**"; basis — "Haltephase: 0 Ticks mo=11
(452..: pst=4 mo=200)", "**Clip 0 VORWAERTS: 8 Bilder F521..F528 (slot 10..17)**".
Bilder (Ausschnitt um Elza, je 2. Bild): `abn_H_elza/p1_neu_cut4.png` — F450 Arm unten, F452-F458 Arm steigt (7-Bild-Blende),
F460-F490 Arm oben statisch, F492-F508 Arm sinkt, F510-F520 unten; `abn_H_elza/p1_neu_cut6.png` — F522-F560 Drehung + Gang,
Arme in Laufhaltung, **kein zweiter Hub**. Basis: `abn_H_elza/p1_basis_cut4.png` — F452-F490 Arm UNTEN (der erste Hub fehlt),
F492-F498 Arm oben (Schnapp), sinkt bis F510; `abn_H_elza/p1_basis_cut6.png` — **F522-F528 Arm erneut oben (zum Gitter
gestreckt)**, ab F530 Gang. Die Basis zeigt damit genau "hebt 2x, laesst einmal fallen"; der Fix erklaert BEIDE Hube:
(1a) Latch 1/2 ueberschrieb Clip 11 im Bild des Plc_motion — Kommandowort 4 (`sb v1(=4),4(v0)` @0x80041bb0) macht
sie hinfaellig, weil der Dispatcher ab dann nur noch 0x80073f90[4] laeuft; (1b) der Walker liess Clip 0 vorwaerts
weiterlaufen, waehrend die Original-Subs 4/5/9 in Phase 0 Clip 5 setzen (@0x80030b58/@0x80030d90/@0x800313a4 — selbst
disassembliert, s. §1).
**Urteil Punkt 1: erfuellt.** Ein Heben (F451, 7-Bild-Blende), 40 Bilder halten, ein Senken (F491), danach Drehung
und Gang ohne weiteren Hub — Nutzer-Wortlaut zum Original. Rest-Notiz (kein Mangel des Punkts, vorher ebenso): F531 ein
Bild Sub-6-Clip-1 zwischen TURN-Ankunft und WALK-Start; das Original setzt in Sub 6 Phase 0 ebenfalls +0x94 := 1 /
+0x8f := 7 (@0x800517f0: `sb v1(=1),148(v0)` @0x80051854, `sb v0(=7),143(v1)` @0x80051874), die Frage ist nur, ob
der SCD-Poll (sub14 `Ck(5,32)` @0x02A62) im Original VOR dem Dispatcher des Folgebilds laeuft — eigener RE-Punkt,
nicht Teil des Wortlauts.

### Punkt 2 — Nach Roys Tagebuch sofort die Karte mit Markern
Werkzeug: Spielstand vor dem liegenden Roy (`probe_r36_elza_karte re15_card.mcr 1011 -1900 6830 3072 4:0xF1 4:0xFA
3:0x74`, Elza), `RE15_CONTINUE_TEST=1 RE15_CARD_AUTO=1 RE15_CARD_SLOT=0 RE15_NO_INTRO=1 RE15_NOAUDIO=1
RE15_WINDOW_SCALE=1 RE15_MSG_LOG=1 RE15_SCD_TRACE=1 RE15_FADE_LOG=1 RE15_STATE_LOG=state.txt`,
`RE15_PRESS=square@60,right@200..350,square@380,square@480` (untersuchen, 6x blaettern, schliessen, "filed"
bestaetigen), Bildserie 470-560/1, `RE15_EXIT_AT=1100#1011`; eigene exe-Kopien `re15_pc_abn_h2.exe` (neu) /
`re15_pc_abn_b2.exe` (v0.8.26). Laufordner `abn_H_elza/p2_neu/`, `abn_H_elza/p2_basis/`.

| Bild | **neu** (debug.log / Helligkeit der Bilder / STATE_LOG) | **basis v0.8.26** |
|---|---|---|
| F60/F61 | Viereck → sub07: Aot_reset @0x0E4E, Work_set, Plc_motion(1,11) @0x0E5C, Sleep 30 | gleich |
| F91 | `[scd] F91 @0x0E64 op=0x2B` + `[tagebuch] ROOM1011 msg 18 -> Roys Diary (… erstes Mal: ablegen)` — Faden parkt | gleich |
| F478 | Dokument-Schirm, "The Roys Diary has been filed." (Bild) | gleich |
| F480-F485 | Abblende des Dokument-Schirms 11.8 → 0.3; F486-F489 schwarz | gleich |
| F487 | STATE_LOG pst=1 mo=210 (Statusschirm-Abbau `sb zero,0x800aca58` @0x8001cbdc → cmd 0; r38-Stand, OFFEN im Dossier) | gleich |
| F490-F494 | Rueckblende Raum 1.4 / 5.7 / 24.6 / 37.4 / 37.4 (Spiel eingefroren, s_stage 4/5) | gleich |
| **F494** | `[scd] F494 @0x0E64 op=0x2B` (Faden parkt weiter) + **`[tagebuch] Leser zu: Bit (9,68)=1, Kartenkette sofort (Runde 39 Spur H): Eintrag 3 (ROOM10F1 Blatt 3 Rechteck 9) -> Folge 4 (ROOM1211 Blatt 1 Rechteck 3)`** | `[tagebuch] Leser zu: … Kartenkette nach dem Evt_end` + `@0x0E68 op=0x02` (Faden laeuft weiter) |
| F495-F497 | **Oeffnen-Blende des Hinweises** 8.7 / 2.3 / 0.2; F498-F505 schwarz | **F495 `@0x0E6A op=0x3F` Plc_motion(1,11)+0x80 (Aufrichten), Sleep 30 — Raum 32 Bilder sichtbar (37.4) bis F525** |
| **F504 / F507-F510** | **`[hint] F504 begin`**, Karte blendet ein 0.6 → 47.4 (Bild F507/F512: "POLICE STATION 2F", Marker-Rechteck, blinkend) | F525 `@0x0E76 Plc_ret`, `@0x0E82 Evt_end` + `[tagebuch] Evt_end erreicht: Kartenkette …`; F526-F536 Blende; **F535 `[hint] begin`**, Karte ab F541 |
| F622 / F740 | Folge-Hinweis 3 → 4 (Prison) / schliessen | F653 / F771 |
| **F754/F755** | `[tagebuch] Hinweis zu: Faden laeuft hinter dem Message_on weiter`, `@0x0E68`; **F755 `@0x0E6A` Plc_motion(1,11)+0x80 = Aufrichten ERST JETZT**; F785 Plc_ret @0x0E76, Aot_reset @0x0E78, Evt_end @0x0E82 | (Aufrichten lag VOR der Karte) |
| msg 18 | kein `[msgtext] room=1011 id=18` (Code-Nachricht bleibt ersetzt) | gleich |

STATE_LOG neu, nur Aenderungen: F61 pst=4 mo=200 … F487 pst=1 mo=210 … (unveraendert bis) F754 pst=1 mo=200, F755 pst=4,
F785 pst=1 pm=1. Position (-1857,6552,rot 3072) in allen Bildern gleich. Zwischen Tagebuch (F494) und Karte (F504)
laeuft kein Spieler-Tick: F487-F493 Statusschirm-Stufen 4/5 (`re15_menu_gameplay_frozen`), F494 Annahme der Anforderung
im game_step VOR dem Spieler-Tick (game_step_common.c:1328-1331, danach `frozen` → return vor Zeile 1729), F495-F753
Hinweis-Schirm. **Kein Bild mit Steuerung vor der Karte.** Basis: 31 Bilder Raum mit laufender Szene (Aufrichten,
Plc_ret, pm=1 ab F525) VOR der Karte — der Nutzer-Befund; der Fix (Anforderung beim Schliessen des Dokument-Schirms
statt am Evt_end, Faden parkt auf dem Message_on) erklaert ihn vollstaendig.
Beobachtungen (kein Mangel des Punkts): (a) zwischen Dokument-Abblende und Hinweis-Blende sind 5 Bilder Rueckblende des
Raums sichtbar (F490-F494, davon 2 voll hell = 1/15 s) — RE2-Form (Status-Task startet aus dem Spielbild, kein
Verketten zweier Schirme); der Nutzer sieht also kurz den Raum, aber keine Szene und keine Steuerung. (b) Der Kamera-Cut
dieses Spielstands zeigt in F490-F494 den Tisch, Elza ist nicht im Bild — in der Basis genauso (F492-F520), Eigenheit
des Cuts/Spielstands, nicht der Spur. (c) F754: ein Bild Idle-FSM (pst=1) zwischen Hinweis-Ende und Plc_motion — der
r38-Stand hatte dieselbe Luecke (Basis F494→F495), liegt NACH der Karte.
**Urteil Punkt 2: erfuellt** (Karte mit Marker direkt nach dem Schliessen des Tagebuchs, vor jeder Rueckgabe der
Steuerung; Zeitpunkt NUTZER-VORGABE, Form RE2 0x84 wie gekennzeichnet).

### Punkt 3 — ROOM1091/1090: kein Klettern am Loescher (Autowrack)
Werkzeug (`abn_H_elza/p3_lauf.sh`): Spielstand neben dem leeren Loescher wie Lauf C des r37-Riegels
(`probe_r37_feuerloescher_karte re15_card.mcr 1091 1880 -1300 <rot> y:-1800 cut:6 elza flag:7:0xf8..0xfb`; Leon:
`1090 1880 -1300 1024 y:-1800 cut:6 flag:3:0x81 flag:9:0x86 flag:3:0x85`), `RE15_CONTINUE_TEST=1 RE15_CARD_AUTO=1
RE15_CARD_SLOT=0`, Eingabeskript `RE15_INPUT_SCRIPT=W1,A0.3,W3` (`_BASIS=spiel`, `_START=60` → Viereck F90-F98),
`RE15_CLIMB_TRACE=climb.txt RE15_MSG_LOG=1 RE15_EXIT_AT=200`, Bildserie 84-200/4; exe-Kopien `re15_pc_abn_p3_<name>.exe`.

| Lauf | exe | Trace-Zeile beim Viereck (T91) | Ergebnis |
|---|---|---|---|
| Elza 1091 rot 1024 | **neu** | `pos=(1880,-1800,-1300) rot=1024 band=1 sub=0 kind=0x00 obj=-1` | **kein Aufstieg** (0/9 Viereck-Bilder mit sub≠0); Bild: Elza bleibt am Boden, Untersuchen-Text "It's a burnt-out shed." (`abn_H_elza/p3_neu1024_voll.png`) |
| Elza 1091 rot 3072 | neu | `rot=3072 band=1 sub=0 obj=-1` | kein Aufstieg (0/9) |
| Elza 1091 rot 0 | neu | `rot=0 band=1 sub=0 obj=-1` | kein Aufstieg; `[msgtext] room=1091 id=29 "It's an empty fire extinguisher."` (Untersuchen-Platz unveraendert) |
| Elza 1091 rot 1024 | **basis v0.8.26** | `rot=1024 band=1 sub=9 kind=0x81 ph=0 obj=4`, T92 mo=230, T93 mo=231 | **Aufstieg**: T91-T121 sub=9 Band 1, T122-T142 sub=9 **Band 2**, ab T143 sub=0 Band 2 — Elza steht danach auf Band 2; Bild `abn_H_elza/p3_bas1024_voll.png`: F100-F132 Kletteranimation am Wrack, danach erhoeht |
| Elza 1091 rot 3072 | basis | `rot=3072 … sub=9 kind=0x81 obj=4` | Aufstieg (52 Trace-Zeilen sub=9) |
| Leon 1090 rot 1024 | **neu** | `room=1090 … band=1 sub=0 obj=-1` | kein Aufstieg (Loescher liegt: `[feuerloescher] ROOM1090: Loescher liegt leer am Boden obj 5 …`) |
| Leon 1090 rot 1024 | basis | `room=1090 … sub=9 kind=0x81 obj=3` | Aufstieg (derselbe Defekt bei Leon) |

Mechanik (selbst nachvollzogen, §1): der Port-Loescher ist eine Null-Box auf Band 1; der Spieler steht am Wagen auf
Band 1 (y −1800 → −(y/0x708) = 1). Band-Gate @0x8002d364-70 wahr → Reichweite FUN_8002d1e8 (box-unabhaengig) →
FUN_8002da4c mit hx = 0, Rand −112: `lhu` vorzeichenlos + `sltu` → Treffer genau aus schraeger Lage (|dx| ≥ 112 UND
|dz| ≥ 112: rot 1024/3072 ja, rot 0/2048 nein) → Substate 9. Mit Band 2 (= Spielerband(y) + 1, Form der drei
Vorlagen-Records ROOM1091 obj 2 / ROOM1090 obj 2 / ROOM1010 obj 0, Bytes selbst gelesen) faellt der Loescher am
Band-Gate durch — der Fix erklaert den Befund in allen vier gemessenen Faellen (Elza 1024/3072, Leon 1024; Basis
klettert, neu nicht; rot 0 unveraendert msg 29). Kennzeichnung PORT-WAHL ist korrekt: der Zensus (selbst gerechnet)
zeigt 51/558 Original-Null-Box-Records AUF dem Spielerband, also kein Original-Gesetz; +1 ist mit 291/558 die haeufigste
Form und die der Vorlagen am selben Wagen.
Nebenwirkungen: (a) ROOM1030 (stehender Loescher, y −2122 → Spielerband 1) bekommt Band 2 — der r37-Riegel
`unit_r37_feuerloescher_raum` wurde entsprechend auf "Band 2" gesetzt (Begruendung im Test); das Band lesen nur
Klettersonde, AOT-Objekt-Band-Vergleich und Protokolle (Dossier), der Untersuchen-/Aufnahme-Platz traegt sein eigenes
AOT-Band → gemessen: Aufnahme/Untersuchen unveraendert (Riegel r37 gruen, s. §0/§4). (b) Von einer Band-2-Standflaeche
aus (Spieler auf einem Tritt, Band = Tritt-Band + 1 @0x80031cfc-0c) waere das Band-Gate wieder wahr — die einzige
Band-2-Standflaeche in 1090/1091 ist die Kiste obj 0 (@0x021BE, Typ 4, Box 900) bei (−8136,4753), ~11 800 Einheiten vom
Loescher (2500,−1300) entfernt, weit ausserhalb der Reichweite 563 + 900 (FUN_8002d1e8 @0x8002d204/@0x8002d280); nur
ein ueber den ganzen Platz geschobener Tritt koennte das aendern — dieselbe Eigenheit gilt fuer das Original-Wrack obj 2
(Band 2). Kein Mangel, als Hinweis vermerkt.
Der echte Weg (Tuer 1051 → 1091) folgt unten (Lauf `p3_tuer`).

**Echter Weg (Tuer) — Lauf `abn_H_elza/p3_tuer`:** Spielstand Elza in ROOM1051 vor der Tuer (`1051 19850 -22300 1024
cut:6 elza`, Form von Lauf H des r37-Riegels), Skript `W1,A0.5,W12` → `[tuer] Sequenz Archiv 2 DOOR1D …`, `[room] PC
loaded room1091.rdt`, **`[feuerloescher] ROOM1091: Loescher liegt leer am Boden obj 5 bei (2500,-1800,-1300) rot_y 3250,
Zone Slot 3 Untersuchen msg 29`** — der Installer (und damit das Band Spielerband(y)+1 = 2) laeuft auch beim
Tuer-Eintritt; Elza steht nach der Tuer bei (−10100,−1800,4200) rot 3072 auf **Band 1** (CLIMB_TRACE `band=1`,
`rc_bnd=-1800`), d.h. dieselben Gate-Eingaben wie im Spielstand am Loescher. Fussweg-Lauf zum Loescher: s.u.

**Fussweg zum Loescher — Laeufe `abn_H_elza/p3_weg2_neu` / `p3_weg2_bas`** (Skript `p3_weg2.sh`): Spielstand am
erreichbaren Messpunkt (0,250) Blick +X (Form der Laeufe F/I des r37-Riegels, Elza, Zombies 0xF8-0xFB erledigt),
Eingabeskript `W1,R0.36,W0.5,XU0.27,W0.5,R1.06,W0.5,XU0.33,W0.5,R0.35,W0.5,A0.3,W2` (Drehung +96/Tick, Rennen 191/Tick —
aus dem Kalibrierlauf `p3_tuer_kal` gemessen): Elza rennt (0,250) → (−72,−1342) → (1918,−1432), dreht auf Blick ~1088
(schraeg zum Loescher bei (2500,−1300): Sondenpunkt dx ≈ −660, dz ≈ −930) und drueckt Viereck (T241/T242).
**neu:** `T241 pad=8000 pos=(1918,-1800,-1432) rot=5184 band=1 sub=0 kind=0x00 obj=-1` — kein Aufstieg, bis zum
Laufende sub=0. **basis v0.8.26:** `T242 pad=8000 … sub=9 kind=0x81 obj=4` → T272 `pos=(1918,-3600,-2580) band=2`
(Elza "klettert" auf Band 2, eine Etage ueber dem Boden, und steht danach dort) — der Nutzer-Befund auf dem Fussweg.
(Ein erster Versuch mit Tuer-Eintritt + Lauf quer ueber den Platz, `p3_weg_neu`, scheiterte am Messaufbau: direkt
hinter der Tuer blockiert ein Hindernis, und das Viereck an der Startstelle oeffnete die Tuer zurueck nach 1051 —
daher der Fussweg vom Messpunkt (0,250); die Tuer selbst ist mit `p3_tuer` belegt.)
**Urteil Punkt 3: erfuellt** (Elza klettert am Loescher/Autowrack nicht mehr — Spielstand, Fussweg; Leon 1090 ebenso;
Untersuchen/Aufnehmen unveraendert).

## 4. Regressionen / Nebenwirkungen (Integration, selbst gefahren, seriell; parallel liefen fremde Agenten-exes)
`ctest -R "^integration_(r37_feuerloescher|r38_tagebuch_lesen|r38_tagebuch_karte|r38_tagebuch_leon|r36_szene1150|r37_roy_szene1021|r38_heliport1171_weg_re15|r35_cut1150|r36_elza_irons)$" --timeout 1800`
-> **100% tests passed, 0 tests failed out of 9** (`abn_H_elza/ctest_regress.log`): integration_r35_cut1150 511 s (Montage
1030/1040/1130/11C0 + Szene 1150, Plc_dest-Laeufe beider Figuren), r36_elza_irons 11 s, r36_szene1150 114 s (Irons-Szene,
Walker), r37_feuerloescher 324 s (Leon: Aufnahme/Loeschen/Wegwerf-Abfrage, Tuer-Weg 1050->1090, Elza 1091, Tuer-Weg
1051->1091, Spind 1000, ROOM1030-Bild — mit dem neuen Band 2), r37_roy_szene1021 54 s, r38_heliport1171_weg_re15 59 s
(John/Elza-Szene mit Plc_dest), r38_tagebuch_lesen 43 s / _karte 21 s / _leon 23 s (Leon unberuehrt). Kein Flattern,
keine Wiederholung noetig (parallel liefen fremde Agenten-exes: `re15_pc_r39d_*`, `re15_pc_r38r_*`, `re15_pc_r35l_*`).
Zusammen mit §0: unit 144/144, integration r39_elza 3/3 — Nebenwirkung von Punkt 1b (Lauf-Clip in der Align-Phase) und
Punkt 3 (Band 2 in 1030/1090/1091) in den betroffenen Szenen/Raeumen gruen, beide Figuren.

## 5. Urteile
| Punkt (Wortlaut AUFTRAG.md) | Urteil | Beleg (Kurz) |
|---|---|---|
| 1. ROOM1031 erste Szene: Hand nur einmal heben und senken | **erfuellt** | neu: F451 Clip 11 (Hub, 7-Bild-Blende), F452-F490 gehalten, F491 Clip 0 rueckwaerts (Senken), ab F521 Clip 5 (Drehung/Gang), 0 Bilder Clip 0 vorwaerts; Basis v0.8.26: Hub 1 fehlt, F491 Schnapp, **F521-F528 Clip 0 vorwaerts = zweiter Hub** (Bilder `abn_H_elza/p1_*_cut4/cut6.png`) |
| 2. Nach Roys Tagebuch sofort die Karte mit Markern, vor der Rueckgabe der Steuerung | **erfuellt** | neu: Leser zu F494 → Anforderung im selben Bild → Hinweis F504/Karte F507 (Marker 2F Comms, dann Prison), Faden parkt auf Message_on @0x0E64 bis F754, Aufrichten F755, Evt_end F785; kein Spieler-Tick/keine Steuerung dazwischen; Basis: 31 Bilder Szene (Aufrichten, Plc_ret) vor der Karte |
| 3. Elza klettert in ROOM1090/1091 am Loescher/Autowrack nicht mehr | **erfuellt** | neu: Spielstand rot 1024/3072/0 + Fussweg (0,250)→(1918,−1432): sub=0, kein Aufstieg, Untersuchen msg 29/20; Basis: sub=9 kind=0x81, Elza auf Band 2; Leon 1090 ebenso behoben; Tuer-Eintritt 1051→1091 mit Installer |

Gates: Bau gruen; Riegel je Punkt vorhanden und messend (unit 144/144, integration r39_elza 3/3, Regression s. §4);
RE-Gate: 13 Stichproben Byte fuer Byte bestaetigt, keine Guess-Tells, NUTZER-VORGABE/PORT-WAHL korrekt gekennzeichnet;
Vertrag: nur eigene Pfade, kleine Haken in player_common.c/actor_locomotion.c, keine neuen Bits/Ids/Assets.

## 6. Maengel
Keine abnahmerelevanten Maengel. Hinweise (kein Mangel, fuer den Nutzer/Integrator):
* H1 Punkt 2: 5 Bilder Rueckblende des Raums (2 voll hell) zwischen Dokument-Abblende und Hinweis-Blende — RE2-Form des
  Status-Tasks; wer die Karte ohne jeden Blick auf den Raum will, braucht einen Port-eigenen Mechanismus ohne RE2-Vorbild
  (vom Bau-Agenten als OFFEN benannt, hier bestaetigt).
* H2 Punkt 1b aendert die Align-Phase JEDES Spieler-Plc_dest (Modus 4/5/9): Lauf-Clip ab dem Sub-Init statt Stehen —
  byte-true (@0x80030b58/@0x80030d90/@0x800313a4 selbst disassembliert); Szenen-Riegel gruen (§0/§4); Sichtpruefung
  weiterer Szenen (1150/1170/1090-Rettung) bleibt Integrationsbaum/Nutzer.
* H3 Punkt 3: das Band 2 des Loeschers ist PORT-WAHL (Form der Vorlagen-Records); 51 Original-Null-Box-Records liegen
  auf dem Spielerband (unveraendert, climb_common.c nicht angefasst). Von einer Band-2-Standflaeche aus (Tritt) waere die
  Null-Box wieder im Gate — in 1090/1091 liegt die einzige (Kiste obj 0) ~11 800 Einheiten entfernt.
* H4 r38-Reste (vorher ebenso, nicht Teil des Wortlauts): pst=1 mo=210 beim Schliessen des Dokument-Schirms (Statusschirm-
  Abbau `sb zero,0x800aca58` @0x8001cbdc), ein Bild Idle-FSM (F754) zwischen Hinweis-Ende und Plc_motion; Punkt 1:
  ein Bild Sub-6-Clip-1 (F531) zwischen TURN-Ankunft und WALK — Reihenfolge SCD-Poll (sub14 Ck(5,32)) vs. Dispatcher,
  eigener RE-Punkt.
* H5 Im Bau-Verzeichnis liegen alte Messkopien des Bau-Agenten (`re15_pc_h2.exe`, `re15_pc_h3.exe`, `re15_pc_h3n.exe`,
  ignoriert, nicht versioniert) — bei der Integration entfernen.

## 7. Ergebnis
**bestanden** — alle drei Punkte erfuellt, alle Gates halten.

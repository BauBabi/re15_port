# Runde 35 — Spur J "affen": UNABHAENGIGE ABNAHME 0

Baum `.claude/worktrees/r35_affen`, Zweig `r35/affen`, gepruefter Stand HEAD 2de6f394 (Basis master 154a73c1).
Abnahme 2026-10-03 (3. Anlauf; die ersten zwei wurden vom Sitzungslimit ohne Bericht abgebrochen — dieser Bericht
wird FORTLAUFEND geschrieben und nach jedem Punkt committet). Massstab: Wortlaut AUFTRAG.md, Regeln VERTRAG.md / CLAUDE.md.

Status: IN ARBEIT

## 0. Bau
- `bash re15_port/tools/local_build.sh configure` -> `=== LOCAL-BUILD-OK (configure)`; `... build` ->
  `ninja: no work to do.` / `=== LOCAL-BUILD-OK (build)` (exe 16:27, juenger als jede Code-Aenderung; Commits
  11087280/2de6f394 aendern nur das Dossier). Logs: scratchpad `jab3_configure.log`, `jab3_build.log`.
- Suite: das Dossier traegt woertlich `=== LOCAL-BUILD-OK (all) — Tests 489/489` (Suite-Lauf 2, HEAD 11087280 =
  letzter Code-Stand); zusaetzlich liegt im Scratch ein vollstaendiger Lauf eines frueheren Abnahme-Anlaufs auf
  diesem Baum (`j_suite.log`, 18:12, `ninja: no work to do`, `=== LOCAL-BUILD-OK (test) — Tests 489/489`).
  Nicht erneut gefahren (local_build.sh beendet exe-Prozesse aus dem eigenen Bauverzeichnis -> haette die
  Messlaeufe dieser Abnahme abgeschossen).
- Selbst gefahren: `ctest -R "r35_affen|^unit_member$|^unit_maggot_ai$"` -> **13/13 Passed** (11 Riegel + 2 Alt-Riegel).

## 1. Nutzer-Punkte — Messprotokoll und Urteil

### 1.0 Der ECHTE Tuerweg (vom Bau-Agenten als offen gemeldet) — jetzt gefahren
- Ursache des gescheiterten Versuchs des Bau-Agenten (lauf_t1): ROOM11B0 sub01 @0x11F2-0x1234 `Ck(4,0xF3)==0` ->
  `Aot_reset 01 01 31` — ohne Strom (Bank 4 Bit 243 = "strom", vgl. panel_zeiger_common.c sub18 @0x016F6) ist die
  Tuer Slot 1 (Door_aot_set @0x0FAA, Ziel `00 1c 00` = ROOM11C0, Spawn (-25279,0,17268)) KEINE Tuer. Kein RDT
  setzt (4,0xF3) (Scan aller STAGE*/ROOM*.RDT: nur Ck), d.h. das ist der Generator-Zustand. Mit Strom laeuft
  beim Betreten von 11B0 zuerst sub04 (`Ck(3,0x82)==0 && Ck(4,6)==1`, @0x11AA) — im Durchlauf vor dem Gang zur Tuer.
- Lauf t2 (scratchpad `jab3/t2`, exe-Kopie `re15_pc_jab3.exe`): `RE15_SET_FLAG=4:243,3:130` (Durchlauf-Stand:
  Generator an, 11B0-Szene gesehen), `RE15_DEBUG_JUMP=11B0@240`, `RE15_PLAYER_POS=-25500,-28200,1024`,
  `RE15_PRESS=square@300..900/30`, Leon danach OHNE Eingabe. debug.log: `[aot] DOOR FIRE slot=1 rect=(-25480,
  -28190,hw=1250,hh=1750) target_cut=0 spawn=(-25279,0,17268)`, `[tuer] Sequenz Archiv 2 DOOR1A ...`,
  `[room] PC loaded room11c0.rdt`, `[evt] F6 room=11c0 Evt_exec sub=2 cond=0x0a` = die Intro-Szene startet ueber
  die Tuer. Alle folgenden Messungen (t2, t3, t4) laufen ueber diesen Tuerweg.

### Punkt 2 — "kommt noch nicht an der korrekten Position aus dem Auto" — **erfuellt**
- t2 state.log (11C0-Segment): beide Gorillas ab F1 `st=1/0/0 hp=180` (INIT im Spawn-Bild wie Original-Savestate
  t=6.11: `st=1/0/0/0 hp=180`), G1 F816 `@(-3617,-17798) g=10 mo=22` = Austritt (Original t=36.41 `(-3617,0,-17798)
  c=22`).
- Bild `jab3/vergleich_wagen.png`: Original s020 t=34.89 / s021 t=36.41 / s022 t=37.92 gegen Port F810/F840/F870
  (Cut 12): der Gorilla sitzt in der offenen Heckklappe und steigt an derselben Stelle gross vor dem Pfeiler aus;
  Lage, Groesse und Bildausschnitt decken sich. (Die Klappe ist im Port dunkler — vom Bau-Agenten als OFFEN
  gefuehrt, nicht Teil des Wortlauts.)
- Beleg der Ursache selbst geprueft: STAGE1 0x80117148 `ori v0,zero,0x1b33`/`sh v0,358(v1)` (Scale im INIT),
  Spawn-Wurzelaufruf `jalr 0x80072bac[typ]` @0x8004259c (Code-Kommentar).
- Test-Luecke: der Riegel `wagen` prueft nur die Klappe (rot_z-Folge) und druckt die Gorilla-Lage, ohne sie zu
  pruefen ("kein Riegel-Urteil ausser Plausibilitaet"); die eigentliche Ursache (INIT/Scale 0x1b33 im Spawn-Bild
  des eingefrorenen Szenen-Records) ist von keinem Riegel gepinnt -> Mangel M2.

### Punkt 3 — "komisch beweglicher Teil am Oberkoerper" — **erfuellt**
- t2 enemy_dbg.log: `ZEICHNE Typ 0x27: 22 Teile (Bones 18, Meshes 22)` (Parts 18..21 werden gezeichnet, jetzt nach
  der INIT-/Binder-Regel).
- Bild `jab3/vergleich_brust.png` (3x Ausschnitt): Original t=36.41/t=37.92 gegen Port F840/F870 — Brust in beiden
  durchgehend braunes Fell, keine weisse/geaderte Platte, kein abstehendes Teil. Kampfbilder Cut 5
  (`jab3/t2_ada.png`, F1110-F1260, zwei Gorillas in Bewegung): kein mitschwingendes Fremdteil am Rumpf.
- Disasm des Belegs selbst geprueft (Abschnitt 2, Stichprobe 1); Riegel `teile` gruen.

### Punkt 6 — "erst springen, wenn sie 3x getroffen wurden, nicht nach jedem Schuss" — **teilweise**
**Nachtrag nach Lauf t5 (Schrotflinte) — das Urteil unten (Handfeuerwaffe) gilt NUR fuer die Handfeuerwaffe:**
- Lauf t5 (Tuerweg, `RE15_GIVE=7:200 RE15_EQUIP=7` = Schrotflinte, sonst wie t3), `p6.py` auf dem gesicherten
  `t5_keep/state_copy.log`: Slot 2: 3 Treffer, Exit-Subs `7,7,7`; Slot 3: 3 Treffer, Exit-Subs `7,7,7` —
  **6 Treffer = 6 Vergeltungs-Spruenge**, also weiterhin "nach jedem Schuss".
- Ursache im Code: der Zaehler sitzt nur in der Boden-Flinch-Spur 0 (`spur_h == 0`, Zeilen 0..6/12/14/19/20,
  enemy_ai_common.c re15_maggot_ai_tick case 2). Die Schrot-Zeile 7 laeuft ueber Spur 1 "AIR-HIT 7/8/13/21"
  (Exit `e->sub_state_1 = 7` @0x8011b3ac-ec) und die Zeilen 9..11/15..18 ueber Spur 2 "CRASH" (Exit
  `sub_state_1 = 7` @0x8011b69c-e8) — beide ohne Zaehler ("Luft-/Sturz-Spuren byte-true", Dossier). Der Wortlaut
  des Nutzers ist nicht auf eine Waffe beschraenkt -> Mangel M3.
- Beobachtung t5 (nicht gegen das Original gemessen): nach dem 2. Schrottreffer (F1365) folgt bei Slot 2 eine KETTE
  aus 5 Spruengen ueber den ganzen Platz (F1392-F1644; (-4898,-15245) -> (-13848,-246) -> (8652,1758) ->
  (8382,10354) -> (7732,-13368) -> (4633,-14167)), Flugschritte bis 810 Einheiten/Bild (F1500-F1524: x +3240 je
  4 Bilder); er landet 11 600 Einheiten von Leon entfernt. Ob das Original nach Spur-1-Treffern so springt, ist
  ungemessen -> unter M3 als Messauftrag.

Urteil fuer die Handfeuerwaffe (Spur 0):
- Lauf t3 (Tuerweg wie t2, `RE15_GIVE=3:250 RE15_EQUIP=3`, `RE15_INPUT_SCRIPT=W40,(MA0.1,M0.6)x150` Basis spiel,
  Feuer ab 11C0-F1260, Leon steht): Auswertung `jab3/p6.py` (Flinch-Eintritt st 1->2, Exit-Sub beim Austritt):
  - Slot 2: 13 Treffer, Exit-Subs `3,3,7, 3,3,7, 3,3,7, 3,3,7, 3` (F1281 hp168 -> 3, F1303 -> 3, F1325 hp144 -> 7, ...).
  - Slot 3: 5 Treffer, Exit-Subs `3,3,7, 3,3`.
  - Spruenge (sub-7-Eintritte): 6 = 5 "nach Treffer" (je der 3., 6., 9., 12. bzw. 3.) + 1 Fernsprung des
    Selektors (Slot 3 F1380 aus sub 4, ohne Treffer — Original-Verhalten, auch in der Original-Aufnahme r3
    t=54.55 sprang G2 ohne Treffer).
  - Vorher (Bau-Agent, Lauf B): 13 Treffer -> 13 Spruenge. Riegel `sprung` (3,3,7,3,3,7) gruen.

### Punkt 1, erste Haelfte — Ada verschwindet beim Verstecken — **erfuellt**
- t2 state.log (Tuerweg): `[evt] F1088 room=11c0 Evt_exec sub=7`, `Plc_dest(slot=1 mode=0x05 dest=(-18214,-7229))`;
  Ada F1088 `st=4/5/2` -> F1145 `st=4/6/0 @(-18025,-7379)` (Original-Savestate t=47.02: `(-18000,20000,-7403)`).
- Lauf t4 (wie t2, Kamera mit `RE15_FORCE_CUT=4` auf den Wagen-Cut, den sub03 fuer die Rueckkehr nutzt; Bild
  `jab3/t4_sheet.png`): F1105 Ada laeuft zu den Streifenwagen, F1135 klein am hinteren Wagen, **ab F1150 nicht
  mehr im Bild** (F1150/F1180/F1300/F1495 leer). Im Cut 5 (t2, `jab3/t2_ada.png`) steht sie bis F1080 sichtbar
  neben Leon, ab F1110 nicht mehr.
- y = 20000 ist im state.log nicht protokolliert; belegt durch den Riegel `ada` (gruen: `y = 20000 ab Bild ...`
  nach Ankunft, Member_set 01 @0x1C74) und das leere Bild.

### Punkt 1, zweite Haelfte — Ada kommt nach dem Sieg wieder heraus — **erfuellt**
- Lauf t5 (Tuerweg, Schrotflinte, beide Gorillas WIRKLICH getoetet — kein gesetztes Kill-Bit): Slot 2 F2054
  `st=3 hp=-20`, danach Slot 3; debug.log `[evt] F2142 room=11c0 Evt_exec sub=3` (sub01: (3,0x43)==0 &&
  (7,0x60) && (7,0x61)), `[scd F2172] Cut_chg(4)`, `Plc_dest(slot=0 mode=0x04 dest=(-14280,-8543))`,
  `Plc_dest(slot=0 mode=0x09 dest=(-18214,-7229))`; Ada F2425 `st=4/4/2 mo=5` verlaesst (-18025,-7379), F2451
  bei (-16264,-8159) (Ziel @0x1B70 (-16211,-8183)); F2729 sub04 (gemeinsamer Gang), danach `[room] PC loaded
  room11b0.rdt` (Raum verlassen).
- Bild `jab3/t5_ada_zurueck.png` (Cut 4): F2140 beide Gorillas liegen; F2300 Leon geht zum Wagen; **F2400 Ada steht
  wieder sichtbar neben Leon** ("Ada: Okay, it's over now."), F2420-F2500 Dialog "...There are some outrageous
  monsters out there." (Original-Nachrichten msg07..09).

### Punkt 4 — "KI zielstrebiger und aggressiver wie im Original" — **teilweise**
Vergleichslage: Leon ohne Eingabe am Szenen-Endpunkt (Port t2 ueber den Tuerweg, Original r3 ueber Debug-JUMP;
Takt 30 Bilder/s). Original-Zeitleiste aus den 110 r3-Savestates (`abn0_orig_timeline.txt`, selbst gelesen).
| Groesse | Original r3 | Port t2 (gebaut) |
|---|---|---|
| Freigabe (sub07 / G1 frei) | t=45.48 | F1088 |
| G2 Fernsprung aus der Ruhe | t=54.55 (sub 7) = 9,1 s | F1395 (sub 7) = 10,2 s |
| 1. Treffer (Heavy -12) | t=57.56 = 12,1 s | F1477 = 13,0 s |
| Biss-Takt (HP -6) | 15 Bisse t=60.58..84.88 = **1,62 s/Biss (~49 Bilder)** | 16 Bisse F1583..F2079 = **35,4 Bilder/Biss** |
| Leon tot | t=84.88 = **39,4 s** | F2079 = **33,0 s** |
| Leon-Lage waehrend der Bisse | pendelt im Kasten (-6285..-7138, -12368..-12963) | wandert stetig (-7160,-12346) -> (-7923,-11343) = 1260 Einheiten nach NW |
- Vorher (Bau-Agent A3/A5): Leon lebte nach 64 s / 74 s noch, Gorillas krochen/drueckten rueckwaerts — das ist
  behoben; Annaeherung und erster Kontakt decken sich jetzt mit dem Original (+0,9 s).
- ABER: der Port ist jetzt in der Angriffsfolge SCHNELLER als das Original (Biss alle 35 statt ~49 Bilder,
  Tod 6,4 s frueher = -16 %) — nicht "wie im Original", sondern ueberschiessend; vom Bau-Agenten selbst als OFFEN
  gefuehrt ("Kandidat: Fuss-Sperre auf geblendeten Pool-Matrizen"), Mechanismus nicht gefunden -> Mangel M1.
- Leons stetige Drift unter den Bissen (Port) gegenueber dem Pendeln im Original ist eine zweite, kleinere
  Abweichung derselben Kette (Biss-Stagger) -> in M1 enthalten.

## 2. RE-Gate (@0x-Belege, Stichproben-Disasm, Guess-Tells)
Diff `git diff master -- re15_port` vollstaendig gelesen (affen_11c0.c, re15_affen.h, enemy_ai_common.c +97,
game_step_common.c 33, main.c 17, actor_common.c, scd_vm.c, emd_common.c, re15_emd.h, re15_actor.h, Tests).
Jede verhaltensrelevante Konstante traegt @0x... oder ist gekennzeichnet (RE15_AFFEN_TREFFER_BIS_SPRUNG = 3
NUTZER-VORGABE; Flinch-Exit "sonst 3" = PORT-WAHL; mag_hit_ctr = PORT-FELD).

Stichproben, SELBST disassembliert (`.claude/skills/re15-psx-disasm/scripts/re15_disasm.py`, richtige Binaerdatei):
1. **STAGE1.BIN 0x80117200-3c (Part 18 an den Rumpf)** — `lw v0,392(v0)` (+0x188 Part-Records), `addiu v1,v0,236`
   / `sw v1,3204(v0)` (3204 = 18*0xac + 0x6c Elternmatrix := rec1+0x40), `addiu v1,v0,172` / `sw v1,3240(v0)`
   (18*0xac+0x90 Eltern-Record := rec1), `ori v1,zero,0x66` / `sw v1,3140(v0)`, `addiu v1,zero,-810` /
   `sw v1,3144(v0)`, `sw zero,3148(v0)` (rel = (102,-810,0) bei rec18+0x2c..0x34), `sh zero,3192/3194/3196(v0)`
   + `jal 0x80068098` (RotMatrix Null-Winkel). **Bestaetigt** (RE15_AFFEN_BRUST_* stimmen).
2. **PSX.EXE 0x8001f5a8-b4 (+0x8f-Abbau in anim_set)** — `lbu v0,143(v1)` / `addiu v0,v0,-1` / `sb v0,143(v1)`;
   erreicht nur ueber `bne v1,zero,0x8001f594` @0x8001f540 (v1 = +0x8f, geladen `lbu s0,143(v1)` @0x8001f408) ->
   Abbau nur bei +0x8f != 0. Port `if (e->anim_frac > 0) e->anim_frac--;` **bestaetigt**.
3. **STAGE1.BIN 0x80118488-9c (Biss-Richtung)** — `jal 0x8001a780` mit `addiu a0,s0,-154` (s0 = 0x800acaee =
   Spieler+0x9a -> a0 = Spieler), `addiu v0,v0,2`, `sb v0,-13735(at)` (0x800aca59). Sprungziel 0x8001a780 selbst
   disassembliert: `lw v1,-14460(v1)` (= laufende Entity = der BEISSER), `lh v0,106(a0)` - `lh v1,106(v1)`,
   `+1024`, `andi 0xfff`, `slti v0,v0,2048`. Port re15_affen_biss_clip = dieselbe Formel. **Bestaetigt.**
4. Zusatz: STAGE1 0x80117148 `ori v0,zero,0x1b33` / `sh v0,358(v1)` (Scale +0x166) bestaetigt; PSX.EXE
   0x800421d4-80042210 (`lbu v1,2(s2)`, 1800*v1 ueber sll3/subu/sll5/addu/sll3, `subu v0,zero,v0`,
   `sh v0,442(s0)`) bestaetigt; RE_15_Quellcode_V2/FUN_8004116c.c `case 0x13: *(+0x1ba) = uVar2` bestaetigt.

Guess-Tell-Suche im hinzugefuegten Code (`deferred|tunable|interim|for now|faithful|plausib|TODO|FIXME|approx`):
ein Treffer — test_r35_affen.c Kopf: "wagen   Messschiene (kein Riegel-Urteil ausser Plausibilitaet)" (siehe
Mangel zu Punkt 2). Env-Schalter: nur RE15_AFFEN_FUSS (Mess-Log, kein Spielverhalten).

## 3. Vertrags-/Pfad-Gate, Tests
(folgt)

## 4. Maengel
(folgt)

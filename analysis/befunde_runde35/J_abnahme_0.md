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

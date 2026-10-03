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
(folgt)

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

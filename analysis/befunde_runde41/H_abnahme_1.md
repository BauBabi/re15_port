# Runde 41 — Spur H "kollision": Abnahme 1 (unabhaengig, 2026-10-24)

Baum `.claude/worktrees/r41_kollision`, Zweig `r41/kollision`, geprueft HEAD **abf7c0f9** (Nachbesserung 1 = nur Doku/Belege;
Code-Stand seit 103563cc unveraendert: `git diff 103563cc HEAD -- re15_port` leer). merge-base mit master = **7b59e297**;
`git diff 7b59e297 master -- re15_port` ist leer, d.h. master-Code = Zweig-Code ohne Spur H.
Arbeitsordner der Abnahme (Skripte, Logs, Bilder): `C:/Users/MJOEDI~1/AppData/Local/Temp/claude/c--workspace-git-reAi-v2/c41eae99-e724-4cb3-afb9-119709f20a9d/scratchpad/H41abn1/`.

Status: IN ARBEIT (wird fortlaufend gespeichert).

## 0. Bau (selbst)
- `bash re15_port/tools/local_build.sh configure` -> `=== LOCAL-BUILD-OK (configure)` (17:57)
- `bash re15_port/tools/local_build.sh build` -> `ninja: no work to do.` / `=== LOCAL-BUILD-OK (build)`
- Gegenprobe, dass die exe den Diff traegt: `re15_pc.exe` 11:28:49 ist juenger als jede Quelle (neueste 11:27:59);
  sha1 `3da1c0f4c3df7fbabd91679e961d93e6f10df207`; `objdump -r` der Objekte: `enemy_ai_common.c.obj` ->
  `re15_kollision_r41_npc_schwanz`, `aot_common.c.obj` -> `re15_kollision_r41_kisten_pass`, `game_step_common.c.obj` ->
  `re15_kollision_r41_objspiegel` (je 1 Relocation); Strings `RE15_KOLLISION_LOG`/`NPC-SCHUB`/`OBJEKT-GESPERRT` in der exe.
  `git status` im Baum leer.
- Vergleichs-exe "vor": Hauptbaum-exe `C:/workspace/git/reAi_v2/re15_port/build/platform/pc/re15_pc.exe` (09:57:15, sha1
  `13f9cba55f16e4cb56854b1fa18e0c80d4c1e16f`; Hauptbaum re15_port ohne lokale Aenderung, neueste Quelle 09:56:34, letzter
  re15_port-Commit 5705fafb = Code der merge-base; enthaelt `wasser20b0`, NICHT `RE15_KOLLISION_LOG`). Kopien unter eigenem
  Namen im Baum: `re15_pc_h41abn1.exe` (nach), `re15_pc_h41abn1_vor.exe` (vor).

## 1. Diff gelesen (`git diff master...HEAD -- re15_port`)
Branch-eigene Aenderungen (3-Punkt-Diff gegen die merge-base; das 2-Punkt-`git diff master --stat` listet zusaetzlich
pri/STAGE3-6 *_port.png, UEBERGABE.md, VERTRAG.md und orchestrator/args_rest4/5.json — das sind master-Commits NACH der
merge-base (919cc1ed u.a.), die der Zweig nicht enthaelt, keine Aenderungen der Spur):
- NEU `engine/src/kollision_r41.c` (221 Z.), `include/re15_kollision_r41.h` (49 Z.) — selbst gelesen:
  `re15_kollision_r41_npc_schwanz(slot)` = aec4(&Spieler, NPC) mit Gate "Spieler aktiv, hp >= 0, Kasten" + b544 (Slots
  1..RE15_ACTOR_MAX-1 aufsteigend, ohne sich selbst, nur aktive mit Kasten, Leichen (Zustand 7) aus) ueber das vorhandene
  `re15_body_push` (= FUN_8002aec4: Gates 0x2/0x4 ueber `flags`, SquareRoot0, Y-Band strikt, dX*pen/(dist+1));
  `re15_kollision_r41_8002cdf4` (Gate 0x40, AABB mit Halbmass-Summen inklusive (u32-Vergleich), Y, vertikaler Eintritt aus
  dem Spiegel +0x42, sonst 1); `re15_kollision_r41_kisten_pass` (obj-Nummer 0..nOmodel-1, Pool-Platz ueber `obj_id`, ohne
  sich selbst, erster Treffer gewinnt; nur der vertikale Zweig schreibt y); `re15_kollision_r41_objspiegel` (+0x42 = y je
  Bild nach dem Objekt-Pass) und zwei Messhaken (`RE15_KOLLISION_LOG`, `RE15_KOLLISION_SETZE`, nur Diagnose).
- Haken: `enemy_ai_common.c` (+1 Include, +1 Zeile in `re15_npc_ai_tick` zwischen `re15_npckante_altlage` und
  `re15_npc_wall_clamp`, hinter den Ruecksprungen fuer Freeze `s_ai_paused`/`RE15_AI_GRID_SKIP` und den SCD-Yield),
  `aot_common.c` (+1 Include, `objgesperrt` nur bei angenommenem Zellentest UND Schub-Substate; Treffer -> Zaehler 10 +
  Ruhe-Zweig; Kopfkommentar berichtigt), `game_step_common.c` (+1 Include, +1 Zeile nach `re15_objfall_nach_pass`).
- Tests: `tests/unit/probes/r41_kollision.cmake`, `tests/unit/test_r41_kollision.c`, `tests/unit/probe_r41_kollision_abstand.c`,
  `tests/integration/test_r41_kollision.cmake`; fremder Riegel `tests/integration/test_r39_dining1141_kampf.cmake`
  (WARTE 60 -> 240, Kommentar "Runde 41 Spur H") — V2, per Entscheid H-E1 angenommen.
- Nachbesserung 1 (5bf1039f, abf7c0f9): nur Dossier + `H_belege/nb1_*` (`git diff 46b38db9 HEAD -- re15_port` leer).

## 2. Tests der Spur (selbst gefahren)
`ctest --test-dir <baum>/re15_port/build -R "r41_kollision|^integration_r39_dining1141_kampf$" --timeout 600
--output-on-failure -j 3` -> **`100% tests passed, 0 tests failed out of 10`** (`Total Test time (real) = 346.08 sec`;
unit npc/raum3070/raum3001/kiste je 0.05-0.06 s, integration kiste 77.55 s, raetsel_leon 138.38 s, raetsel_elza 138.66 s,
npc3070 103.27 s, npc3001 101.81 s, integration_r39_dining1141_kampf (Fassung WARTE 240) 346.03 s). Log `ctest_spur.log`.
Die Riegel selbst gelesen: messend (Abstand je Bild mit Kontakt-Pflicht gegen Vakuitaet, Kisten-LAGE statt Ziel,
Grenze 1800/1801, Flags (5,0..2) und Endlagen fuer Leon UND Elza), parallel-fest (eigener `lauf_<zufall>`, exe-Kopie
`re15_pc_r41h_<teil>_*`, RE15_*-Umgebung geleert), Linux-fest (`cmake_minimum_required`, `m`, keine .exe-/Backslash-Pfade).

## 3. RE-Gate (selbst disassembliert: `.claude/skills/re15-psx-disasm/scripts/re15_disasm.py` auf `info/Re1.5/PSX.EXE`
## bzw. `info/Re1.5/PSX/BIN/STAGEn.BIN`; eigene Scanner `reg_scan.py` / `tail_scan.py` im Arbeitsordner)
1. **Registrierung + Wurzel-Schwanz aller NPC-Wurzeln (eigener Scanner, nicht der der Spur):** `reg_scan.py` sucht in
   STAGE1..6.BIN jedes `sw rX,0x2cXX(at)` auf 0x80072bac+typ*4 fuer Typ 0x40..0x4d und rekonstruiert den Wert aus lui/addiu:
   28 Registrierungen (STAGE1 6: 0x40 0x8011c5a0, 0x42 0x8011cb70, 0x45 0x8011d140, 0x47 0x8011d6d4, 0x49 0x8011dc68,
   0x4b 0x8011e22c; STAGE2-4 je 4; STAGE5/6 je 5 mit 0x4d). `tail_scan.py` prueft je Wurzel bis zum ersten `jr ra`:
   Prolog `lui s0,0x800b / addiu s0,s0,-13760` (s0 = 0x800aca40), Reihenfolge jalr < jal b498 < jal aec4 < jal b544 <
   jal b0a4, Delay-Slot von aec4 = `0x26040014` = `addiu a0,s0,20` (a0 = 0x800aca54 = Spieler), b544 unmittelbar (+8)
   nach aec4: **`ERGEBNIS: 28 / 28`** (`tail_scan.txt`). Wortlaut STAGE3 Ada (0x42 -> 0x8011bcf4, Eskorte ROOM3070):
   `8011bda0 jalr v0` -> `8011bdb0 jal 0x8002b498` -> `8011bdbc lw a1,-14460(a1)` (a1 = g_entity) -> **`8011bdc0 jal
   0x8002aec4` / `8011bdc4 addiu a0,s0,20`** -> **`8011bdc8 jal 0x8002b544`** -> `8011bddc lw v0,120(a0)` / `8011bde4 lhu
   a1,6(v0)` / `8011bde8 jal 0x8003b0a4` (Wandklemme, a0 = g_entity+52). = Port-Reihenfolge (Haken vor `re15_npc_wall_clamp`).
2. **FUN_8002b544** (PSX.EXE): `8002b550 lbu s1,-13746(s1)` (g_active_count), `8002b564 addiu s0,s0,-13268` (0x800acc2c),
   `8002b578 andi v0,v0,0x1` / `8002b57c beq -> 8002b5a4`, `8002b590 beq a1,s0` (sich selbst), **`8002b598 jal 0x8002aec4` /
   `8002b59c addu a0,s0,zero`** (a0 = andere, a1 = g_entity = NPC), `8002b5ac addiu s0,s0,500`. Kein Typfilter. = wie zitiert.
3. **FUN_8002aec4**: `8002af04 or v0,a0,v1 / 8002af08 andi v0,v0,0x2 / 8002af0c bne -> 8002af30 (return 0)`, `8002af14 and /
   8002af18 andi 0x1000 / 8002af1c bne -> 8002b464`, `8002af24 andi v0,v1,0x4` (v1 = Wort des Geschobenen a1),
   Radien `8002af40 lhu t4,6(s6)` / `8002af4c lhu t4,6(s5)`, `8002b1d8 jal 0x80065f60` (SquareRoot0), `8002b1e4 subu s0,s0,a2` /
   `8002b1e8 blez`, Y-Band strikt `8002b21c slt v0,t1,a1` / `8002b224 slt v0,a1,a0`, `8002b22c mult s1,s0 / 8002b234 addiu
   a2,a2,1 / 8002b238 div v0,a2`. = `re15_body_push`.
4. **Objekt-Pass in FUN_8002bd44**: `8002bd84 ori s7,zero,0xa` (= 10), `8002bdac addiu fp,fp,16280` (fp = 0x800b3f98 =
   Pool), `8002bfb0 jal 0x8003b558` / `8002bfb8 bne v0,zero,0x8002c0b0` (Zelle), `8002bfc0 sb v0(=8),140(s2)`, `8002bfd4
   ori v0,v0,0x2000`, `8002bfe4 bne v1,v0(=8),0x8002c0b8` (Substate), Entity-Durchlauf `8002c020 jal 0x8002cabc` / `8002c030
   sb s7,140(s2)` (nicht portiert, OFFEN 1), **`8002c040 lw v0,-14472(v0)` / `8002c048 lbu v0,2(v0)` (nOmodel) / `8002c050
   beq v0,zero,0x8002c0d8` / `8002c058 addu s0,fp,zero` / `8002c05c beq s2,s0,0x8002c088` / `8002c06c andi v0,v0,0x1` /
   `8002c078 jal 0x8002cdf4` (Delay `addu a1,s2,zero`) / `8002c080 bne v0,zero,0x8002c0b0` / `8002c0a4 addiu s0,s0,148`**,
   `8002c0b0 sb s7,140(s2)`, Ruecksetzung `8002c0bc-c0d4` (x/z aus s4/s5, `jal 0x8003b558`), Spiegel `8002c0ec lhu v1,56(s2)`
   -> `8002c0f8 sh v1,66(s2)`. Pool nach obj-Nummer: `8004093c lbu a3,1(a2)` / `80040944-58` a3*148 + 0x800b3f98. = Port.
   Nebenbefund beim Lesen: die Aussenschleife von FUN_8002bd44 laeuft die Objekte ABSTEIGEND (`8002bdc4 addu s2,v0,fp` /
   `8002bdc8 addiu s2,s2,-148`, `8002bddc addiu s3,s3,-1`), der Spiegel +0x42 eines Objekts mit hoeherer Nummer ist beim Test
   eines niedrigeren also schon "dieses Bild" — der Port-Schnappschuss "Ende des letzten Passes" weicht davon nur fuer ein im
   selben Bild fallendes Objekt ab (= OFFEN 3 des Dossiers, fuer die drei Band-0-Kisten in 2050 ohne Wirkung).
5. **FUN_8002cdf4 Zeile fuer Zeile gegen `re15_kollision_r41_8002cdf4`**: Gate `8002ce18 or / 8002ce1c andi 0x40 / 8002ce20
   bne -> return 0`; dX `8002ce48 addu v0,v0,a0` / `8002ce4c addu s1,s2,v1` / `8002ce50 subu t7,v0,s1` (b - a, b = a3 =
   geschoben); Summen `8002ce78 addu a1,a1,v1` / `8002ce7c addu t3,a0,v0`; `8002ce80 addu v1,t7,a1 / 8002ce84 sll v0,a1,1 /
   8002ce88 sltu v0,v0,v1 / 8002ce8c bne -> 0` (vorzeichenlos, Grenze inklusive), Z `8002ce94-cea0`; Y `8002cec8 subu t6,v0,t8`
   / `8002cecc addu a0,a0,v1` / `8002ced8 sltu` / `8002cedc xori a2,v0,0x1` / `8002cee0 beq a2,zero -> 0`; Vor-Y `8002cee8 lh
   v1,66(a3)` / `8002ceec lh v0,66(t2)` ... `8002cf08 xor / 8002cf0c and / 8002cf10 beq -> 0x8002cf40`; vertikal `8002cf18 bgez
   t6` (Delay `subu v0,t8,t1`) -> `+1 +a0` bzw. `-1 -a0` -> `sw v0,56(a3)`; waagrecht `8002cf14 mult t7,t3` / `8002cf44 mult
   t5,a1` / `8002cf64 slt` -> `sw v0,52(a3)` bzw. `sw v0,60(a3)`; `8002cfb8 ori v0,zero,0x1`. Die Portierung bildet jede
   Zeile ab; die waagrechte Ausschiebung entfaellt zu Recht, weil der Aufrufer x/z @0x8002c0bc-d4 zuruecksetzt.
6. **Neue Belege der Nachbesserung 1 (V2) stichprobenartig:** STAGE1 `8011ddbc lw v0,0(a0) / 8011ddc0 lui v1,0x4000 /
   8011ddc4 or / 8011ddc8 sw` (NPC-INIT Wort0 |= 0x40000000); PSX.EXE `80042284/8004228c ori s3,zero,0x1` / `80042298 sw
   s3,0(s0)` / `800422ac ori v0,zero,0x2001` / `800422b0 sw v0,0(s0)` (Sce_em_set); STAGE1 `8011beac ori v0,v0,0x2 / 8011beb0
   sw v0,0(v1)`, Tabelle `0x801213c8[7] = 0x8011be54`; `8011ac34 ori v0,v0,0x1000` / `8011ad8c addiu v1,zero,-4097` + `and`.
   Alle wie zitiert.

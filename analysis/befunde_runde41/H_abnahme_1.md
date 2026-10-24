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
7. **Rate-Woerter / Schalter:** `git diff master...HEAD -- re15_port | grep -ciE "deferred|tunable|interim|for now|faithful|
   plausib|TODO"` -> **0**. Neue Konstanten im Code-Diff: keine Zahl ohne Adresse (Zaehler 10 = s7 @0x8002bd84, Gate 0x40
   @0x8002ce1c, Schranke nOmodel @0x8002c048, Radien = vorhandenes `hit_radius_min` = Kasten +6). Env-Schalter nur Messhaken
   (`RE15_KOLLISION_LOG`, `RE15_KOLLISION_SETZE`), kein Abschluss ueber einen Schalter.
8. **Erklaert der Fix den Befund?** Ja (an meinen eigenen vor/nach-Laeufen, Abschnitt 4): der NPC-Tick des Ports hatte nach
   dem Zustand nur die Klemme; mit aec4/b544 davor (Reihenfolge 1.) erscheinen in `kollision.txt` genau die b544-Schuebe
   (`NPC-SCHUB slot=2 typ=40 von=1 typ=42`), und der Abstand springt von 1 auf >= 899. Bei den Kisten fehlte der Durchlauf
   @0x8002c03c-c0a8; mit ihm erscheint `OBJEKT-GESPERRT ... gegen ... obj=<andere>` genau an der inklusiven Grenze.

## 4. Messungen an der exe (eigene Laeufe, eigene Plaene, eigener Auswerter)
Werkzeuge im Arbeitsordner: `lauf.sh` (je Lauf eigener Ordner `laeufe/<name>_<vor|nach>`, alle RE15_* des Aufrufers geloescht,
Grundumgebung wie die Riegel: RE15_NO_INTRO=1 RE15_NOAUDIO=1 SDL_AUDIODRIVER=dummy RE15_WINDOW_SCALE=1 RE15_TITLE_CONFIRM_MS=1500
RE15_PSELECT_AUTO=1, exe-Kopie unter eigenem Namen, `timeout`), Wegplaene `plaene/*.txt` (RE15_WEGPLAN = echte Pad-Bits aus
Spieler-/Objektlage, dachkiste_r36.c), Auswerter `npcpaare.py` (NICHT der Auswerter der Spur: je Bild alle NPC-Paare (Typ
0x40..0x4d) und Spieler/NPC aus RE15_STATE_LOG, Radien aus den Kaesten 450/500/300, Spieler 450), Bildbogen `bogen.py`.

### 4.1 Punkt 1, LEON, ROOM3070 Eskorte, ECHTER Tuerweg 30E0 -> 3070, EIGENER Laufweg (`laeufe/p1a_3070_{vor,nach}`)
Befehl: `lauf.sh p1a_3070 <vor|nach> 400 RE15_SET_FLAG=4:215,4:236,3:180 RE15_DEBUG_JUMP=30E0@250
RE15_PLAYER_POS=-25660,-22130,3072 RE15_WEGPLAN=plaene/p1a_3070.txt RE15_STATE_LOG=state.txt RE15_KOLLISION_LOG=kollision.txt
RE15_EXIT_AT=4800#3070 RE15_FRAMEDUMP=600-4800/150:f`. Story-Stand per Flag (nach G-Birkin (4,215)/(4,236) und der 30E0-Szene
(3,180), ROOM30E0 sub03 @0x00E66); der Raum wird ueber die Tuer betreten: debug.log `[aot] DOOR FIRE slot=1
rect=(-25750,-21200,hw=1350,hh=600) target_cut=0 spawn=(-11840,0,-18140)` -> `[room] PC loaded room3070.rdt` -> sub00
@0x0343C/@0x03450 setzt Ada (0x42) + Marvin (0x40) mit Verhaltensbyte 0x00 (Eskorte). Mein Weg (anders als Spur und Abnahme 0):
Leon geht nach Suedosten (-9000,-12000), steht 240, geht zurueck DURCH die Folgenden nach (-12600,-16600), steht 300, nach Osten
(-7000,-17200), steht 150, nach Sueden (-9800,-10400), steht 300, nach (-11000,-16000), steht bis F4800 (`[flow] EXIT_AT: Bild
4800 in Raum 3070 erreicht -> exit`, beide exe exit=0).

| exe | Ada/Marvin (Summe 900): min | Bilder < 850 | Bilder < 900 | Ende F4800 | Spieler/Ada min | Spieler/Marvin min |
|---|---|---|---|---|---|---|
| vor | **1** (F1331) | **4539 / 4800** | 4569 | **6** | 716 | 707 |
| nach | **899** (F1416) | **0** | 150 (alle 850..899) | **901** | 813 | 863 |

`npcpaare.py`-Zeilen woertlich: vor `Paar 1(0x42)/2(0x40) Summe 900: Bilder 4800 (F1..F4800), MIN 1 bei F1331, <Summe-50: 4539,
<Summe: 4569, Ende 6`; nach `... MIN 899 bei F1416, <Summe-50: 0, <Summe: 150, Ende 901`. `kollision.txt` (nach): 5017
`NPC-SCHUB`-Zeilen, davon 4082 `slot=2 typ=40 von=1` (Marvin aus Ada, b544), 862 `slot=1 typ=42 von=2` (Ada aus Marvin), 57/16 vom
Spieler (aec4); am Minimum `F1416 ... NPC-SCHUB slot=2 typ=40 von=1 typ=42 vor=(-9009,-11141) nach=(-9047,-11080)`.
Bild `zoom_p1a_3070_f1350.png` (F1350, in BEIDEN Laeufen Kamera 1, gleicher Ausschnitt, RE15_WINDOW_SCALE=1): **vor** sind nur
Marvin und Leon zu sehen — Ada steckt in Marvin (state.txt `Ada(-9464,-12287) Marvin(-9464,-12288)`, d = 1); **nach** stehen Ada,
Marvin und Leon als drei getrennte Figuren (d = 900). Dazu `zoom_p1a_3070_f2550.png` (vor: eine Mischfigur Marvin/Ada, d = 6) und
Bogen `bild_p1a_3070_vor_nach.png`. => Nutzer-Befund am echten Tuerweg mit meinem Laufweg reproduziert (vor) und behoben (nach).

### 4.2 Punkt 2, LEON, ROOM2050, EIGENE Kistenpaarung obj 1 gegen obj 2 auf der Z-Achse (`laeufe/ka_2050_{vor,nach}`)
Diese Paarung (obj 1 <-> obj 2) hat weder die Spur (obj 2 -> obj 3, Raetsel) noch Abnahme 0 (obj 3 -> obj 2, obj 3 -> obj 1)
gefahren. Befehl: `lauf.sh ka_2050 <vor|nach> 400 RE15_DEBUG_JUMP=2050@250 RE15_PLAYER_POS=-2700,-64,0,0
RE15_WEGPLAN=plaene/ka_2050.txt RE15_PUSH_LOG=push.txt RE15_OBJFALL_LOG=objfall.txt RE15_KOLLISION_LOG=kollision.txt
RE15_EXIT_AT=2400#2050 RE15_FRAMEDUMP=300-2400/50:f`. Wegplan: `GEGNERWEG` (die zwei Spinnen 0x25), obj 2 (-1256,0,-64) nach
OSTEN bis x >= 600 (dann |dX| zu obj 1 = 1420 <= 1800, zu obj 3 = 3616 > 1800), um die Kisten herum nach (2052,-6952) noerdlich
von obj 1, obj 1 (2052,0,-5552) nach SUEDEN (`SCHIEB 3072 Z >= 1000`, zweimal). Auswerter `kistenpaare.py` (eigener: alle Paare
der Band-0-Kisten je Lagezeile von RE15_OBJFALL_LOG, Ueberlappung = |dX| <= 1800 UND |dZ| <= 1800 wie FUN_8002cdf4).

| exe | Lagezeilen mit Kisten-Ueberlappung | tiefste Eindringung | SCHUB-OK / SCHUB-GESPERRT obj 1 | OBJEKT-GESPERRT | Endlage obj 1 / obj 2 |
|---|---|---|---|---|---|
| vor | **115 / 160** (ab F690) | **380** (= ganze X-Ueberdeckung; obj 1 laeuft von z -1368 durch obj 2 (z -64) bis **z 1043**) | 322 / 0 | — | **(2052,0,1043) / (632,0,-64)**: \|dX\| 1420, \|dZ\| 1107 = ineinander |
| nach | **0 / 160** | — | 203 / 2 | **2**: `F676 room=2050 OBJEKT-GESPERRT p=2 obj=1 ziel=(2052,-1862) gegen p=3 obj=2 lage=(632,0,-64) dX=1420 dZ=-1798 sumX=1800 sumZ=1800` (dazu F773 dZ=-1789) | **(2052,0,-1873) / (632,0,-64)**: \|dZ\| 1809 > 1800 |

Push-Log nach woertlich: `F676 room=2050 p=2 obj=1 SCHUB-GESPERRT spieler=(2047,-3212) rot=3072 lage=(2052,-1882) ziel=(2052,-1862)
band=0`. Beide Laeufe `[pl] Spieler-Familie PL00`, `[room] PC loaded room2050.rdt`, `[flow] EXIT_AT: Bild 2400 in Raum 2050
erreicht -> exit`. => vorher ist die Kiste ganz durch die andere hindurchgeschoben worden (Nutzer-Befund), nachher sperrt der
Objekt-Pass beim ersten Ziel mit \|dZ\| <= 1800 (1798, inklusive Grenze) und die Kiste bleibt 9 Einheiten vor der Beruehrung.

### 4.3 Punkt 2, ELZA, ROOM2051, dieselbe eigene Paarung (`laeufe/kb_2051_{vor,nach}`)
Befehl wie 4.2 mit `RE15_PSELECT_AUTO_SWITCH=1 RE15_DEBUG_JUMP=2051@250` (Plan `plaene/ka_2051.txt`, RAUM 2051); debug.log beider
Laeufe `[pl] Spieler-Familie PL04 (character=4, Elza-Bit=1)`, `[room] PC loaded room2051.rdt`.

| exe | Lagezeilen mit Ueberlappung | tiefste Eindringung | SCHUB-OK / SCHUB-GESPERRT obj 1 | OBJEKT-GESPERRT | Endlage obj 1 / obj 2 |
|---|---|---|---|---|---|
| vor | **116 / 160** (ab F675) | **374** (obj 1 bis **z 1013** durch obj 2) | 325 / 0 | — | (2052,0,1013) / (626,0,-64) = ineinander |
| nach | **0 / 160** | — | 205 / 2 | **2** (gegen obj 2) | (2052,0,-1880) / (626,0,-64): \|dZ\| 1816 > 1800 |

=> Elza-Szenario wie Leon: vorher ineinander, nachher gesperrt.
Bildbeleg zu 4.2: `bild_ka_cut12.png` (Bildlauf `laeufe/ka_2050_cut12_{vor,nach}` = derselbe Plan mit `RE15_FORCE_CUT=12`
(Beckenkamera), nur fuer das Bild; Lagen bitgleich zum Messlauf: obj 1 bei F1200 vor `(2052,0,1043)` / nach `(2052,0,-1873)` wie
in 4.2, 322/203 SCHUB-OK, 0/2 gesperrt; push.txt der cut13-Laeufe in den ersten 400 Zeilen identisch zum Lauf ohne Kamera):
**vor** schiebt sich ab F800 die rechte Kiste (obj 1) in die vordere linke (obj 2), F900/F1200 sind nur noch zwei Umrisse da;
**nach** stehen ab F700 drei Kisten, obj 1 Kante an Kante neben obj 2.

### 4.4 Punkt 2, LEON, ECHTER WEG in ROOM2050 bis ins Becken (Tuer + drei Absprunge) und dieselbe Paarung (`laeufe/kc2_2050_{vor,nach}`)
Abnahme 0 hatte den Abstieg ins Becken nicht nachgefahren (dort H4). Hier vollstaendig an der exe: `RE15_DEBUG_JUMP=2000@250
RE15_PLAYER_POS=-26450,10250,2048,1` (an der Leiter in ROOM2000), Plan `plaene/kc2_2050.txt`: Aktionstaste -> debug.log `[aot] DOOR
FIRE slot=4 rect=(-27100,10300,hw=500,hh=500) target_cut=0 spawn=(9100,-5400,4100)` -> `[room] PC loaded room2050.rdt` (Ankunft
Band 3). Weg: nach Norden/Westen bis vor die Absprungzelle r0[49] (Band 3, Wort 0x3202, x 6350..8750 z -13950..-12150; der
Durchgang r0[50] ist im ersten Besuch per `Sca_id_set 37 0x 32 00` @0x01E94-01EA4 offen), Blick Nord, Aktion -> Herunterspringen
(climb.txt `sub=10 kind=0x02`, danach `band=2`, Lage (7595,-3600,-12618)); an die Zelle r0[23] (Band 2, Wort 0x2102), Blick West,
Aktion -> `band=1` (5882,-13036); an r0[77] (Band 1, Wort 0x1002), Blick Sued, Aktion -> `band=0` (5878,-11682); zu Fuss ins Becken
nach (-2701,-92). climb.txt-Baender in Folge: `312 band=1` (ROOM2000) `441 band=3` `197 band=2` `188 band=1` `2774 band=0` — in
BEIDEN exe gleich. Danach derselbe Kistenweg wie 4.2 (obj 2 nach Osten bis x >= 600, obj 1 nach Sueden). `GEGNERWEG` nimmt die zwei
Spinnen heraus (Messung der Schubkette ohne Gegner).

| exe | Lagezeilen mit Ueberlappung | tiefste Eindringung | obj 1 SCHUB-OK / GESPERRT | OBJEKT-GESPERRT | Endlage obj 1 / obj 2 |
|---|---|---|---|---|---|
| vor | **109 / 240** (ab F1980) | 390 (obj 1 durch obj 2 bis **z 1000**, `SCHIEB: Bedingung erfuellt`) | 324 / 0 | — | (2052,0,1000) / (642,0,-64) = ineinander |
| nach | **0 / 240** | — | 192 / 1 | `F1975 room=2050 OBJEKT-GESPERRT p=2 obj=1 ziel=(2052,-1842) gegen p=3 obj=2 lage=(642,0,-64) dX=1410 dZ=-1778 sumX=1800 sumZ=1800` | (2052,0,-1879) / (642,0,-64): \|dZ\| 1815 > 1800 |

(Im nach-Lauf hielt der Plan danach UP bis zum Bildlimit; Leon glitt an obj 1 vorbei und schob obj 3 nach Sueden bis an die Zelle
— `F2706 ... obj=3 SCHUB-GESPERRT ... lage=(4248,3344)`, eine Zellsperre, keine Kistenberuehrung; 0 Ueberlappungszeilen bis F3600.)
=> Der Nutzer-Befund ist auf dem echten Weg (Tuer + Abstieg) reproduziert (vor) und behoben (nach).

### 4.5 Punkt 1, ELZA, ROOM3001 John + Sherry, ECHTER Story-Weg OHNE Flag-Vorgabe (`laeufe/p1c_3001_{vor,nach}`)
Spur und Abnahme 0 setzten (4,198)/(4,199) per Flag. Hier setzt das Spiel sie selbst: Elza (`RE15_PSELECT_AUTO_SWITCH=1`, debug.log
`[pl] Spieler-Familie PL04 (character=4, Elza-Bit=1)`) springt nach ROOM3011 vor die Tuer (`RE15_DEBUG_JUMP=3011@250
RE15_PLAYER_POS=-23168,11769,2048`), Plan `plaene/p1c_3001.txt`, dazu `RE15_INPUT_SCRIPT` (Basis Spielbild, Quadrat 3 Bilder je
Sekunde) — der Wegplan laesst waehrend `RAUM`-Wartezeiten (Szene, player_mode 2) das Skript durch, das Quadrat schliesst die
Szenentexte (msg_common.c `Cross|Square @0x80028698`). Ablauf (debug.log woertlich, beide exe gleich):
`[aot] DOOR FIRE slot=0 rect=(-24200,11530,hw=800,hh=910) target_cut=3 spawn=(-6544,0,-17239)` -> `[room] PC loaded room3001.rdt`
(1. Besuch: sub00 (4,198)=0 -> John/Sherry Verhaltensbyte 0x40 + Szene sub03 @0x016A6, die (4,198)/(4,199) setzt) -> zu Fuss an die
Ost-Tuer -> `[aot] DOOR FIRE slot=1 rect=(-5555,-17520,hw=335,hh=750) target_cut=1 spawn=(-23168,0,11769)` -> `loaded room3011.rdt`
-> zurueck `DOOR FIRE slot=0 ...` -> `loaded room3001.rdt` (2. Besuch: sub00 (4,199)=1 -> John 0x49 + Sherry 0x4b mit
Verhaltensbyte 0x00 = Eskorte @0x015A2/@0x015B6) -> Elza wartet, geht nach (-12000,-16000), (-20000,-15500), (-14000,-13600),
(-22000,-17000) mit Pausen -> `[flow] EXIT_AT: Bild 4000 in Raum 3001 erreicht -> exit`. Auswertung je Raumbesuch
(`segmente.py` teilt state.txt an den Bildzaehler-Ruecksetzern; seg3 = 1. Besuch 3001, seg5 = 2. Besuch 3001).

| exe | Besuch | John/Sherry (Summe 750): min | Bilder < 700 | Bilder < 750 | Ende |
|---|---|---|---|---|---|
| vor | 1. (Szene, danach folgen sie) | **8** (F1198) | **343 / 1283** | 344 | 260 |
| nach | 1. | **749** (F1047) | **0** | 10 | 788 |
| vor | 2. (Eskorte) | **13** (F979) | **3917 / 4000** | 3917 | **195** |
| nach | 2. | **722** (F1331) | **0** | 93 | **752** |

`kollision.txt` (nach): 4150 `NPC-SCHUB`, davon 3123 `slot=1 typ=49 von=2 typ=4b` (John aus Sherry), 782 `slot=2 typ=4b von=1 typ=49`;
am Minimum `F1331 room=3001 NPC-SCHUB slot=2 typ=4b von=1 typ=49 vor=(-15670,-13178) nach=(-15789,-13135)`. Die 93 Bilder zwischen
700 und 750 sind wie bei Leon die Wandklemme nach dem Schub (Hautabstand 18 @0x8003bce0, b0a4 nach b544, RE-Gate 1).
Bild `zoom_p1c_3001_f2000.png` (2. Besuch F2000, beide Laeufe Kamera 1, gleicher Ausschnitt): **vor** steckt Sherry in John (Beine und
Haar ineinander, d = 195), **nach** steht John allein, Sherry 752 dahinter; Bogen `bild_p1c_3001_vor_nach.png`.
=> Nutzer-Befund fuer Elza auf dem echten Story-Weg (ohne Flag-Vorgabe, zwei Tuerdurchgaenge) reproduziert und behoben.
Beobachtung (kein Mangel, s. Hinweis H3): am Anfang des 2. Besuchs laufen beide Eskorten zu einem Folgepunkt OESTLICH von Elza (sie
steht mit Blick West an der Tuer) — vor laeuft Sherry durch Elza hindurch (Elza wird dabei in 12 Bildern 530 nach Westen gestossen,
F73 -> F85), nach kommt Sherry nicht durch Elza und schiebt sie ueber ~100 Bilder 570 nach Osten, bevor sie seitlich vorbeigleitet.

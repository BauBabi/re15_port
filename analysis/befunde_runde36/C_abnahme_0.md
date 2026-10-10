# Runde 36 — Spur C "schuss": Abnahme 0 (unabhaengig, 2026-10-10)

Baum `.claude/worktrees/r36_schuss`, Zweig `r36/schuss`, geprueft am Commit 09ed2d02 (13 Spur-Commits +
2 Merges ueber master 78a4a748). Nur gelesen/gemessen, kein Code im Baum geaendert. Eigene Sonden/Skripte/Bilder
liegen im Scratchpad dieser Abnahme (nicht im Repo):
`C:/Users/MJOEDI~1/AppData/Local/Temp/claude/c--workspace-git-reAi-v2/c41eae99-e724-4cb3-afb9-119709f20a9d/scratchpad/abn/`
(Kurzform `abn/...`); der Kern der Blut-Sonde steht in Anhang A, damit die Messung ohne den Scratchpad nachfahrbar ist.

## 0. Gelesen / gebaut
* AUFTRAG.md (Wortlaut der drei Punkte), VERTRAG.md (Spur C; §1.4 Dateien; §2 Regel 2: "C-Blut" ist als
  Regression gefuehrt -> zuerst bisecten), Dossier C_schuss.md (385 Zeilen), `git log master..HEAD`,
  `git diff master --stat` (26 Dateien), den vollstaendigen Code-Diff (engine 567 Zeilen, Tests 1073 Zeilen).
* Bau: `bash re15_port/tools/local_build.sh configure` -> `=== LOCAL-BUILD-OK (configure)`;
  `bash re15_port/tools/local_build.sh build` -> `ninja: no work to do.` / `=== LOCAL-BUILD-OK (build)`.
  Gegenprobe, dass die exe zum Commit passt: re15_pc.exe mtime 10:14:21, juengste Quelle (schuss_r36.c,
  enemy_ai_re2_zombie.c, re15_actor.h) 10:14:07, letzter Engine-Commit 37d796f3 10:17 (nur Commit nach dem
  Speichern), `git status re15_port` leer, `ninja -n`: nichts zu tun.

## 1. Punkt 1 — "Alle Schusswaffen treffen ueber den Konferenztisch und treffen fressende Zombies. Im Original geht beides nicht."

### 1.1 RE-Gate (selbst disassembliert)
* RE1.5 PSX.EXE @0x80012150-88 (FUN_80011f50 hinter der Kandidatenschleife): `80012160 beq s1,zero,0x8001253c` /
  `80012164 addiu s0,s1,52` / `80012168 jal 0x8001b9b4` / `8001216c addu a0,s0,zero` / `80012170 bne v0,zero,0x80012540` /
  `80012178 lbu v0,8(s1)` / `80012180 sltiu v0,v0,0x40` — wie zitiert; die Sichtlinie steht vor Typ-Pruefung und Latch.
* FUN_8001b9b4 @0x8001b9b4-ba50: `sw zero,436(v0)` (cur+0x1b4), delta = Ziel - cur(+52/+56/+60), `ori s0,zero,0x3`,
  Schleife `ori a2,zero,0xf00` / `jal 0x8003dcc4` / `ori a3,zero,0x300` / `or s1,s1,v0`, Abbruch nach Region 0 — wie zitiert.
  (Der Port-Zwilling kehrt beim ersten Treffer zurueck; das ODER-Ergebnis ist dasselbe. cur+0x1b4 wird im Port nicht
  gefuehrt — kein Leser im Schusspfad bekannt, nicht weiter verfolgt.)
* FUN_8003dcc4 Zellfilter @0x8003de5c-94: `lhu a0,0(s7)` / `lbu v1,130(v1)` (+0x82 von cur) / `sll v0,a0,16` / `sra v0,v0,28` /
  `bne v1,v0` und `and v1,t5,a0` (0xf00) / `bne v1,v0` (0x300) — wie zitiert. Port re15_los_ray_blocked filtert identisch
  (`c->floor >> 4 == e->floor`, `c->floor & 0x0f == 3`), Spieler-`floor` = Band (re15_collision_set_band setzt beides).
* RE2 PSX.EXE FUN_800410CC Kandidatenschleife @0x80041254-a0: `lbu v0,467(s2)` / `bne v0,zero,0x80041300` (Gate 2),
  `lh v0,342(s2)` / `bltz` (Gate 3), `lhu v0,270(s2)` / `andi v0,v0,0xc000` / `bne` (Gate 4); Aufrufer `jal 0x800410cc`
  @0x80043afc — wie zitiert.
* EMZ0.BIN: EXEC[8] P0 `lbu v1,467(s1)` / `ori v1,v1,0x80` / `sb v1,467(s1)` @0x80103c04-14, P3 `andi v0,v0,0x7f` /
  `sb v0,467(s1)` @0x80103cf8-fc, Wecker `andi v1,v1,0xbfff` / `sh v1,270(s0)` @0x80104f0c-10 — wie zitiert.
  **Ungenau:** "+0x10E = 0x4004 aus dem Spawn-Remap @0x80100A88-8C" (Code-Kommentare in re15_re2z_hit_filter_apply,
  test_r36_schuss.c, test_re2_zombie_hitfilter.c, Dossier §1.2). Der Remap @0x80100a64-a8 gilt fuer Deskriptor 7
  (`addiu v1,zero,7` / `andi 0x3f` / `bne`), setzt Zustand 0x701 = EXEC[7] und HP -1 (`sh v0,342(s2)` mit -1); der
  FRESSER ist Deskriptor 8 @0x80100ac0-dc (`addiu v0,zero,2049` / `sw v0,4(s2)` = 0x801 = EXEC[8], Clip 18) und
  bekommt dort KEIN 0x4004. Im RE2-Original sperrt den Fresser also nur Gate 2. Das 0x4004 des Port-Fressers ist
  eine aeltere Port-Abbildung (enemy_ai_re2_zombie.c Z. 8441, nicht in dieser Spur). Ergebnis gleich (Gate 2 allein
  sperrt; der Port-Nachzug raeumt beide Bytes beim Verlassen der Pose), das Zitat belegt aber nicht das Behauptete.

### 1.2 Messung an der exe (eigene Messkopie `re15_abn0c.exe`, Ausgabe Scratchpad `abn/p1/...`)
Aufbau wie run_p1.sh des Bau-Agenten: `RE15_DEBUG_JUMP=1140@250`, `RE15_PLAYER_POS`, `RE15_GIVE=<w>:6`,
`RE15_EQUIP=<w>`, `RE15_INPUT_SCRIPT="M0.6,MA1.5,M1.0,W4"` (Basis spiel), `RE15_STATE_LOG`, `RE15_WAFFEN_LOG`,
`RE15_FX_LOG`, `RE15_EXIT_AT=200#1140`, RE2-KI. Alle Laeufe exit=0. HP je Platz = max -> letzter Wert im state.log.
| Lauf | Waffe | Schuesse (wf.log `arms_rec=0`, mg) | Ergebnis |
|---|---|---|---|
| tisch (200,-10300, Blick -z) | 20 Python | 2, mg 6->4 | P1 79->79, P2 50->50, P3 80->80, P4 250->250, P5 250->250 |
| tisch | 3 Browning | 3, mg 6->3 | alle Plaetze unveraendert |
| tisch | 5 Beretta | 6, mg 6->0 | alle unveraendert |
| tisch | 7 Redhawk | 2, mg 6->4 | alle unveraendert |
| tisch | 8 Remington | 2, mg 6->4 | alle unveraendert |
| tisch | 12 Ingram | 4 Saetze, mg 6->0 | alle unveraendert |
| tisch | 13 SPAS | 3, mg 6->3 | alle unveraendert |
| tisch | 19 MC51 | 4 Saetze, mg 6->0 | alle unveraendert |
| fresser (Tuer -7600,-17600, Blick +x) | 3 / 7 / 8 / 12 / 20 | je 2..4 | alle Plaetze unveraendert (alle Fresser st1/ss1 8) |
| frei (200,-18200, hinter dem Tisch) | 20 Python | 2 | P5 250->-1 (F19), P4 250->-1 (F44) — geweckt und getroffen |
| frei | 3 Browning | 3 | P5 250->202 (F19) — getroffen |
Vorher-Stand (master, Roh-Logs des Bau-Agenten `mess_r36c/vorher/{tisch,fresser}/state.log`, von mir ausgewertet):
tisch P4 250->-1 (F44), P5 250->-1 (F19); fresser P2 50->-1 (F19), P5 250->-1 (F44).
**Wichtig:** In der Tisch-Aufstellung stehen hinter dem Tisch NUR Fresser (P2..P5, st1/ss1 8) und der Liegende P1.
Unter RE2-KI wuerde schon die Fresser-Sperre allein jeden Treffer verhindern — die exe-Tischmessung (auch
`integration_r36_schuss` "tisch") belegt die Sichtlinie deshalb NICHT fuer sich. Isolation s. 1.3.

### 1.3 Sichtlinie isoliert (exe, `RE15_AI_FLAVOR=re15`: dort sind die Fresser treffbar, OFFEN 1 des Dossiers)
| Lauf | Waffe | Ergebnis |
|---|---|---|
| fresser (Tuer, freie Sicht) | 20 | **P2 75->-1 (F19), P5 250->-1 (F44)** — dieselben Fresser sind unter RE1.5-KI treffbar |
| tisch (200,-10300) | 20 / 3 / 8 | alle Plaetze unveraendert (P1 65, P2 75, P3 65, P4 250, P5 250; Fresser st1/ss1 12) |
| frei (200,-18200, hinter dem Tisch) | 20 | P4 250->-1 (F44), P5 250->-1 (F19) |
=> Bei treffbaren Zielen sperrt allein der Tisch: die Sichtlinie wirkt an der exe. Zusammen mit `unit_r36_schuss` A
(wache Zombies hinter dem Tisch, zehn Hitscan-Waffen, `verworfen == 1`) ist der Tisch-Teil belegt.
Nebenbefund (kein Mangel gegen den Wortlaut, Vorgabe = RE2-KI): unter der Menue-Option RE1.5-KI bleiben fressende
Zombies treffbar (gemessen oben) — das ist der offene RE1.5-Mechanismus OFFEN 1 des Dossiers.

### 1.4 Nebenwirkung auf andere Raeume (Pruefung)
Die Sichtlinie gilt jetzt fuer JEDES Ziel des Spieler-Resolvers (auch Bosse). Klasse-3-Zellen (FUN_8003dcc4-Filter)
der Bossraeume per RDT-Auswertung (`sca_k3.py`, Scratchpad): ROOM2090 (Alligator, Port-Design) hat innen nur den
Plattform-Block #4 (x -1700..1850, z -20000..-12450, Band 0) — der Alligator ist nur gesperrt, wenn der Block
zwischen Leon und ihm liegt; Schuesse quer durch den Block treffen nicht mehr (nur RDT-Auswertung, der Bosskampf
wurde nicht an der exe gefahren). ROOM5090 (Birkin): Wagenwaende/Sitze — Original-Verhalten (RE1.5-Raum).
ROOM1210 (Gitterarme): Klasse-3-Zellen bei z < -17000 bzw. z > -6140, Griffpunkt (-20609,-15171) frei.
Merge-Probe `git merge-tree r36/schuss r36/zombieki`: konfliktfrei.

**Urteil Punkt 1: erfuellt** (RE2-KI = Vorgabe; Tisch und Fresser fuer alle gemessenen Schusswaffen gesperrt,
geweckte Fresser treffbar; Werfer 14..18 nach den Roh-Logs des Bau-Agenten `mess_r36c/alle`, nicht selbst nachgefahren).

## 2. Punkt 2 — "Blut nicht an der richtigen Stelle, wenn sie getroffen werden"

### 2.1 RE-Gate (selbst disassembliert)
* RE2 EMZ0.BIN @0x80105650-708 (Haupt-Handler P0) — bestaetigt wie im Dossier: Zone = +0x1D2 % 3
  (`lbu v1,466(s1)` / `multu 0xaaaaaaab`), Zone != 0: `lh v1,346(s1)` (+0x15A), `lw v1,1420(a1)` - 100*v1,
  `lw v0,1424(a1)` + `addiu v0,v0,300`, `lw v0,1428(a1)`, `addiu a2,s2,72` (Part 0); Zone 0:
  `sh zero,16 / addiu v0,zero,800 / sh v0,18 / sh zero,20`, `addiu a2,a2,244` (Part 1); `jal 0x8001bf10` @0x80105708.
* Die Lage-Semantik von a3 habe ich NICHT ueber die im Dossier zitierte Stelle (@0x8001a248 ist in der
  RE2-EXE ein Anim-Setter `lbu v0,332(a0)` ... `jal 0x8002959c`; das Zitat meint die RE1.5-ESP-Maschine
  re15_esp.c:1447-1455), sondern direkt in RE2 belegt: FUN_8001BF10 schreibt a3 nach Record+44/+48
  (`lw v0,0(a3)` / `sw v0,44(t0)` / `lw v0,4(a3)` / `sw v0,48(t0)` @0x8001bfe8-ffc), die Part-Matrix nach
  +76..+107 (@0x8001c03c ff.). Der RE2-Lagen-Tick 0x8001d894 rechnet im Zweig ohne Flag 0x400 (FUN_8001BF10
  setzt 0xa003): RotMatrix(+34) * v36 -> +52/54/56 (@0x8001dac8-db70), dann RT = Record+76 (`addiu v1,v0,76`
  @0x8001db74, ctc2), V0 = Record+44 (`addiu v0,v0,44` @0x8001dba0, lwc2), MVMVA 0x4a486012 @0x8001dbb4,
  `lw v1,96/100/104(a0)` + `addu` @0x8001dbd0-dc14 => Welt = Anker.R * a3 + Anker.T + Rgier * v36.
  **Die Formel des Fixes (Anker.R * a3 + Anker.T) ist damit fuer RE2 belegt.** (Zitat im Dossier ungenau.)

### 2.2 Was der Fix abdeckt — und was nicht (Emitter-Zensus EMZ0.BIN, alle `jal 0x8001bf10`)
Gefixt (a3 jetzt angewandt): Haupt-Handler P0 @0x80105708, Stagger P0 @0x80105d80/db8, Wurzel-Flinch
@0x8010510c/154. NICHT gefixt, obwohl das Original dort einen Versatz uebergibt:
* **TOD-Treffer** (der toedliche Schuss) @0x8010864c-6b4: Zone != 0 `lw v0,1420(v1)` / `lw v0,1424(v1)` +
  `addiu v0,v0,300` / `lw v0,1428(v1)` -> a3 = (P8.x, P8.y+300, P8.z) an Part 0 (`addiu a2,s2,72`), Id 8096;
  Zone 0 `addiu v0,zero,800` / `sh zero,24 / sh v0,26 / sh zero,28` an Part 1 (`addiu a2,a2,244`), Id 6096.
  Port enemy_ai_re2_zombie.c ~Z. 7728-7733: `re2z_gore_fx(e, 0, 8096u)` / `re2z_gore_fx(e, 1, 6096u)` ->
  re2z_gore_fx ruft re2z_gore_fx_ex(..., 0, 0, 0) = KEIN Versatz (der Kommentar dort nennt "ofs y+300" /
  "{0,800,0}" sogar).
* Kriecher-Tod @0x80108abc-ad4: a3 = (0,300,0) an Part 0 -> Port Z. ~7535 `re2z_gore_fx(e, 0, 8096u)` ohne Versatz.
* **Treffer beim Aufstehen** @0x80107a98-aa0 (`sh zero,24/26/28(sp)`) + @0x80107b20-30 (`addiu v0,zero,300` /
  `sh v0,26(sp)` / `addiu a2,a2,1448` = Part 8) -> a3 = (0,300,0) an Part 8. Port Z. 7087
  `re2z_blood_fx_scaled(e, 8, ..., NULL)` — der Port-Kommentar direkt darueber zitiert "v = {0,300,0}
  @0x80107B20-24"; das Dossier §2.3 nennt diesen Emitter faelschlich "MG-Emitter Part 8" und laesst ihn mit
  a3 = NULL.
* Stagger-P1-Nachbluten @0x80105fa4-ff4: a3 = Aktor+0x144 (`addiu a3,s4,324`) -> Port Z. 5114
  `re2z_blood_fx_at(e, 0, ...)` mit NULL (kleiner Versatz, Nebenbefund).

### 2.3 Messung (eigene Sonde `abn_blut_tod.c`, Scratchpad; Aufbau = probe_r36_blut: ROOM1140, RE2-KI, RE2-Bank
CDEMD0.EMS, echter game_step, Leon 1800 vor Platz 2, Pistole, Schuss alle 12 Bilder, aber BIS ZUM TOD + 60 Bilder;
je Spawn Abstand zu den RE2-Sollpunkten aller drei Emitter-Familien aus der im Vorbild gezeichneten Pose)
Kompiliert mit den Flags aus build.ninja gegen engine/libre15_engine.a + tests/libre15_test_support.a.
* eben (Rumpf, Zone 1), Pistole:
  * F66 `SPAWN part=0 fx=(-1503,-2512,-19712) hp 98->82` Soll HURT-Rumpf d=16 — gefixt, ok.
  * F390 `SPAWN part=8 fx=(-1060,-1807,-19527) hp 34->18 st 1/5/7 -> 2/3/1` (Treffer beim Aufstehen):
    Part8 d=57, **Soll Aufstehen d=300** (= der fehlende a3 (0,300,0)).
  * F402 `SPAWN part=8 ... hp 18->2` Soll Aufstehen d=436.
  * **F414 `SPAWN part=0 fx=(-1554,-1976,-19822) hp 2->-14 st 1/1/1 -> 3/3/1` (toedlicher Treffer):
    Becken(T-1) y=-1975 d=5, Soll TOD-Rumpf (y -2436) d=482** — das Blut des toedlichen Schusses sitzt
    weiterhin AM BECKEN, ~480 unter dem RE2-Punkt (genau der vom Bau-Agenten fuer die HURT-Emitter gemessene
    Defekt, §2.1 Dossier: "Rumpf-Treffer 505 zu tief").
* tief (Beine, Zone 0), Pistole: F66/F78/F102 Zone-0-Treffer d=12/15/23 zum Soll — gefixt, ok;
  F162 Aufstehen part=8 ohne Versatz; **F174 toedlicher Treffer `part=0 fx=(-1554,-227,-19822)` Becken d=48,
  Soll Zone 0 d=783.**
* Die Pins `unit_r36_blut_pin_eben/_tief` sehen das nicht: probe_r36_blut bricht beim Tod ab (`if (z->hp < 0
  || z->state == 3 ...) break;`) und wertet nur Spawns an Part 0/1 (`re15_re2z_last_fx_part() == 0 || == 1`)
  — der Part-8-Spawn des Aufsteh-Treffers (in der eigenen mess-Ausgabe der Sonde als `f366 fx part=8` sichtbar)
  faellt durch den Filter.
* exe-Gegenmessung (eigene Messkopie, `run_abn_p2.sh`: ROOM1140, Leon (-5000,-19600) Blick +x, Pistole, RE2-KI,
  Dauerfeuer `W1.2,M0.6,MA9.0,M0.5,W2`, `RE15_FX_LOG`, `RE15_STATE_LOG`): Platz 2 Treffer F55 (50->34), F77 (->18),
  F99 (->2), **F121 toedlich (2->-14, st 3)**. fx.log, erstes Bild jedes Blut-Effekts (`id=0 ... frame=1`):
  F55 `w(-1733,-2445,-19605)`, F77 `w(-1723,-2140,-19787)` (Brusthoehe, gefixt) — **F121 `w(-1907,-1808,-20497)`**
  (Hoehe der Huefte, Wurzel (-1877,-20511)). Bild `abn/p2/blut_zoom_hurt55_tod121.png` (Scratchpad; Bilder 55-57
  links: roter Stoss an Brust/Schulter; 121-123 rechts: kein Stoss an der Brust sichtbar).
* Regel 2 VERTRAG §2 ("C-Blut" = Regression, zuerst bisecten, verursachenden Commit nennen): im Dossier
  nicht erfolgt. Eigene Pruefung: `git log 9362dd7e..3ec5b1f8 -G "re2z_blood_fx|re2z_blood_anchor|
  re2z_gore_fx_ex|re15_esp_fx_spawn_ex"` ueber enemy_ai_re2_zombie.c/re15_damage.c/re15_esp.c/skeleton_common.c
  ist leer — der Blut-Code war in v0.8.21..v0.8.23 unveraendert, also KEINE Regression aus v0.8.22/23. Die
  Aussage fehlt im Dossier.

**Urteil Punkt 2: teilweise.** Der Mechanismus (a3-Versatz) ist richtig erkannt und fuer drei HURT-Emitter
byte-true umgesetzt; der Wortlaut "Blut ... wenn sie getroffen werden" umfasst aber auch den toedlichen Treffer
(jeder getoetete Zombie) und den Treffer beim Aufstehen — dort sitzt das Blut gemessen weiterhin 300-480
(Beine ~780) neben dem Original-Punkt.

## 3. Punkt 3 — "Mit der Super Redhawk zerplatzen/verschwinden die Kraehen komplett — ist das in RE2 auch so?"

### 3.1 RE-Gate (selbst disassembliert, info/re2leon/COMMON/BIN/EMOVL21_S0.BIN bzw. PSX.EXE)
* Zeilen-Dispatch `table 0x80104a18 19`: [0..4] 0x80102934, **[5] [6] 0x80102ca0**, [7] [8] 0x8010327c, [9] [10] [11]
  0x80102ca0, [12] 0x80102fb4, [13..16] 0x80102934, [17] 0x80102ca0, [18] 0x80102934 — wie zitiert.
* GIB 0x80102ca0: `sh zero,326(s0)` @0x80102cb8, `jal 0x800154ac` @0x80102cd4 (a0/a1 = Spieler x/z @0x800cfc30/38,
  a2/a3 = self+56/+64), `jal 0x80015fe8` @0x80102cdc (Maske), Zeile 0 `jal 0x80102ef4` @0x80102cec, Bit 1 `andi v0,s2,0x1` /
  `beq` @0x80102cf4-f8 — wie zitiert.
* WURF 0x80102ef4: `lui a2,0x10` / `ori a2,a2,0x1040`, `sh 800,156`, `sh 10,158`, `sh -100,164`, `sh a1,152`, `sh zero,160`,
  `sh zero,154`, `sw a2,112`, `ori v1,v1,0x4a` — Port kraehe_wurf identisch.
* WIRBEL 0x80102f34: `ori v0,v0,0x1062`, drei `jal 0x80015fe8` @0x80102f54/68/88 -> +56 (= a1 + rand<<2), +60
  (= (rand&0x7f)+100), +64 (= rand<<4); `sh zero,134/58/62/66`, `sb 20,121` — Port kraehe_wirbel identisch, Draw-Reihenfolge gleich.
* Lebensdauer FUN_80028dac (PSX.EXE): `lhu v1,160(s0)` / `andi 0x7fff` / `sltiu v0,v0,0x1d` @0x80028dcc / `sw zero,0(s0)` @0x80028ddc
  — wie zitiert.
* Port-Zuordnung w7 (Super Redhawk) -> RE2-Zeile 5 (s_re2c_row_from_weapon, besteht seit frueheren Runden, unveraendert).
=> Die Antwort im Dossier §3.0 ("Ja — RE2 laesst die Kraehe beim Magnum-Treffer in 13 Teile zerplatzen, kein Kadaver,
keine Lache") ist am Original belegt.

### 3.2 Messung an der exe (eigene Messkopie, `run_abn_p3.sh`: ROOM10C0, Kraehe Platz 3 sitzt bei (5100,3600), Leon
3522,5556,581, `RE15_FORCE_CUT=1`, RE2-KI, Eingabe "MD0.6,MDA0.4,MD1.0,W3", `RE15_FRAMEDUMP=14-60/2`, `RE15_WINDOW_SCALE=1`)
* Redhawk (7): state.log `F19 Platz 3 st=7 ss1=1 hp=-1` (GIB-Leiche). Bilder (Scratchpad `abn/p3/w7_zoom.png`, Bild
  18..34): Bild 18 Kraehe sitzt, ab Bild 20 ist der Koerper weg, dunkelrote Teile/Scherben fliegen bis Bild 30
  auseinander, Bild 34 nichts mehr — kein Kadaver, keine Lache.
* Gegenprobe Pistole (3): `F19 Platz 3 st=3 ss1=3 hp=-5`, `F31 st=7 ss1=0` — Kraehe faellt und bleibt als Leiche mit
  Lache liegen (`abn/p3/w3_zoom.png`).
* Pins: `unit_r36_schuss_kraehe` (Felder, Draws, Physik bis zum Verschwinden, Dispatch w7 -> GIB / w3 -> Flug-Treffer),
  `unit_re2_crow_ai` angepasst.

**Urteil Punkt 3: erfuellt** (Frage beantwortet und belegt; der Port zeigt jetzt das RE2-Zerplatzen statt des
sofortigen Verschwindens).

## 4. Gates

### 4.1 RE-Gate
* Selbst disassemblierte Stichproben (mehr als drei, alle mit der richtigen Binaerdatei): RE1.5 PSX.EXE @0x80012150-88,
  FUN_8001b9b4, FUN_8003dcc4 @0x8003dcf8-dd44/@0x8003de54-98; RE2 PSX.EXE FUN_800410CC @0x80041250-a4 + Aufrufer
  @0x80043afc, FUN_8001BF10 @0x8001bf10-c05c, Lagen-Tick 0x8001d894 @0x8001d894-dc20, FUN_80028dac; EMZ0.BIN
  @0x80105640-71c, @0x80107a78-b34, @0x801085e8-6bc, @0x80108aa8-ad4, @0x80103c00-14, @0x80103ce0-fc, @0x80104f04-10,
  @0x801009b0-b44; EMOVL21_S0.BIN Tabelle @0x80104a18, @0x80102ca0-d14, @0x80102ef4-f98.
  Ergebnis: alle Kernzitate enthalten das Behauptete, mit zwei Ausnahmen (Mangel M5): (a) `sh 0x4004,270`
  @0x80100A88-8C ist der Remap von Deskriptor 7 (EXEC[7], HP -1), nicht der des Fressers (Deskriptor 8 @0x80100ac0-dc,
  ohne +0x10E); (b) @0x8001a248 ist in der RE2-EXE ein Anim-Setter — die a3-Lage-Semantik steht fuer RE2 in
  0x8001d894 @0x8001db74-dc1c (von mir belegt, Ergebnis wie behauptet).
* Neue Konstanten im Diff (engine/include) tragen @0x… oder sind als PORT gekennzeichnet (Kraehen-Boden +0x1C2
  "[PORT (1)]", Peilung "Port-Konvention"); Seed-Tinte 0x808080 nur mit Funktionsname FUN_80028368 (wie das
  bestehende re15_re2z_part_reset).
* Rate-Woerter (deferred/tunable/interim/for now/faithful/plausibel/TODO) in den hinzugefuegten Zeilen: keine.
  Env-Schalter als Abschluss: keine (einziges getenv = RE15_ASSET_DIR in der Sonde).
* "Fix erklaert den Befund": Punkt 1 ja (vorher Treffer genau ueber Tisch/auf Fresser, nachher gesperrt, geweckte
  Ziele treffbar); Punkt 3 ja (crow_hide -> Part-Bruecke); Punkt 2 nur fuer die drei HURT-Emitter (s. 2.2/2.3).

### 4.2 Vertrag
* Pfade (`git diff master --name-only`): nur engine/src (enemy_ai_common.c, enemy_ai_re2_crow.c, enemy_ai_re2_zombie.c,
  re15_damage.c, neu schuss_r36.c), include (re15_actor.h, neu re15_schuss_r36.h), eigene Tests + probes/r36_schuss.cmake
  + Dossier. Kein release/, platform/android/, shared_assets/PSX/, keine Test-CMakeLists. ok.
* Haken in gemeinsamen Dateien: re15_damage.c +4, enemy_ai_common.c +4/-2, re15_actor.h +5 Zeilen. ok.
  enemy_ai_re2_zombie.c (Modul F) traegt den Blut-Anker (VERTRAG §1.4: "C = ... + Blut-Anker") und die
  Fresser-Ausnahme; `git merge-tree --write-tree r36/schuss r36/zombieki` konfliktfrei.
* Bank-9-Bits / Nachrichten-IDs / Ereignisse: keine verwendet (Diff-Suche nach re15_game_flag_set,
  re15_msg_install_text, scd_event_fire leer). ok.
* Tests je Punkt vorhanden (probes/r36_schuss.cmake registriert unit_r36_schuss, integration_r36_schuss,
  unit_r36_blut_pin_eben/_tief, unit_r36_schuss_kraehe). Trennschaerfe s. M3/M4. integration_r36_schuss startet
  re15_pc.exe direkt (spiel_lauf.cmake, keine Messkopie) — M7.

### 4.3 Tests (gezielt, kein Voll-Suite-Lauf im Spurbaum)
`ctest --test-dir re15_port/build -R "r36|crow|kraehe|hitfilter|zombie_abc|re2z|re2_zombie|1140|damage|werfer|dog_hit|
spider|gator|birkin|g5_|shootable|hit_repeat|autofire|waffen|blood|blut|gore|feeder|straight_shot|enemy_ai|combat|aim|
r16_liegende|r16_aufstehen|rise_hittable|hit_reaction" --timeout 600 --output-on-failure` ->
**`100% tests passed, 0 tests failed out of 87`** (327 s; darin integration_r36_schuss 80 s, integration_r35_werfer
137 s, _ton/_elza/_form, alle Spur-Pins). Suite-Log des Bau-Agenten `mess_r36c/suite2.log`: `100% tests passed, 0 tests
failed out of 597` / `=== LOCAL-BUILD-OK (test) — Tests 597/597` (Stand vor den letzten Doku-Commits; Engine seit
37d796f3 unveraendert).

## 5. Maengel (nummeriert, nachpruefbar)
* **M1 (Punkt 2, wesentlich)** — Der TOEDLICHE Treffer setzt das Blut weiterhin am Becken ab. Original EMZ0
  @0x8010864c-6b4: Zone != 0 a3 = (P8.x, P8.y+300, P8.z) an Part 0 (Id 8096), Zone 0 a3 = (0,800,0) an Part 1 (Id 6096);
  Port enemy_ai_re2_zombie.c ~Z. 7728-7733 `re2z_gore_fx(e, 0, 8096u)` / `re2z_gore_fx(e, 1, 6096u)` -> re2z_gore_fx_ex
  mit Versatz (0,0,0). Ebenso Kriecher-Tod @0x80108abc-ad4 (a3 = (0,300,0) an Part 0) -> Port ~Z. 7535. Gemessen:
  `abn_blut_tod eben` F414 Becken d=5 / RE2-Punkt d=482; `tief` F174 Becken d=48 / RE2-Punkt d=783; exe F121 y -1808
  gegen Brusttreffer y -2445. Nachpruefen: Sonde `abn_blut_tod.c` (Quelltext oben beschrieben) oder probe_r36_blut ohne
  Abbruch beim Tod.
* **M2 (Punkt 2)** — Treffer beim AUFSTEHEN: Original @0x80107a98-aa0 + @0x80107b20-30 a3 = (0,300,0) an Part 8
  (`addiu a2,a2,1448`); Port Z. 7087 `re2z_blood_fx_scaled(e, 8, ..., NULL)`. Gemessen F390 (eben) Soll-Abstand 300,
  F162 (tief) 230. Das Dossier §2.3 nennt diesen Emitter "MG-Emitter Part 8" (falsch; der MG-Handler ist 0x80107EF0)
  und laesst ihn bewusst ohne a3. (Nebenbefund: Stagger-P1-Nachbluten @0x80105fa4-ff4 hat a3 = Aktor+0x144, Port Z. 5114 NULL.)
* **M3 (Punkt 2, Pin)** — unit_r36_blut_pin_eben/_tief koennen M1/M2 nicht sehen: probe_r36_blut bricht beim Tod ab
  (`if (z->hp < 0 || z->state == 3 || z->state == 7) break;`) und wertet nur Spawns an Part 0/1
  (`re15_re2z_last_fx_part() == 0 || == 1`); der Part-8-Spawn erscheint in der eigenen mess-Ausgabe (`f366 fx part=8`).
* **M4 (Punkt 1, Test-Trennschaerfe)** — integration_r36_schuss "tisch"/"gl_tisch" (und die exe-Tischmessung §1.4 des
  Dossiers) beschiessen hinter dem Tisch nur gesperrte Fresser; der RE2-"fresser"-Lauf zeigt, dass die Fresser-Sperre
  allein jeden Treffer verhindert. Ohne Sichtlinie bliebe der Integrationstest gruen. Die Sichtlinie ist nur ueber
  unit_r36_schuss (wache Ziele) gepinnt; ein exe-Lauf mit treffbaren Zielen hinter dem Tisch fehlt (diese Abnahme hat
  ihn unter RE15_AI_FLAVOR=re15 gefahren, §1.3).
* **M5 (RE-Gate, Zitate)** — "+0x10E = 0x4004 aus dem Spawn-Remap @0x80100A88-8C" gilt nicht fuer den Fresser
  (Deskriptor 7, EXEC[7], HP -1; Fresser = Deskriptor 8 @0x80100ac0-dc ohne 0x4004) — in re15_re2z_hit_filter_apply
  (enemy_ai_re2_zombie.c), test_r36_schuss.c teil_b, test_re2_zombie_hitfilter.c (4a) und Dossier §1.2/§1.5 berichtigen (die
  Sperre selbst bleibt richtig: Gate 2 allein sperrt den RE2-Fresser). "@0x8001a248" als Beleg der a3-Lage gilt fuer
  die RE1.5-ESP-Maschine; fuer RE2 0x8001d894 @0x8001db74-dc1c zitieren.
* **M6 (Vertrag §2 Regel 2)** — kein Bisect/keine Regressions-Aussage fuer "C-Blut" im Dossier (Abnahme: Code in
  v0.8.21..v0.8.23 unveraendert, `git log -G` leer — gehoert ins Dossier).
* **M7 (Test-Hygiene, gering)** — integration_r36_schuss startet `$<TARGET_FILE:re15_pc>` direkt (spiel_lauf.cmake),
  ohne Messkopie unter eigenem Namen (Muster test_r34n_b_cursor.cmake `file(COPY_FILE ...)`).

## 6. Beobachtungen ohne Maengel-Status (fuer Orchestrator/Nutzer)
* Unter der Menue-Option RE1.5-KI bleiben Fresser treffbar (exe gemessen, §1.3) — Dossier OFFEN 1 (RE1.5-Mechanismus
  ungeklaert; Vorgabe RE2-KI erfuellt).
* ROOM2090 (Alligator, Port-Design): Schuesse durch den Plattform-Block #4 treffen nicht mehr (Original-Regel).
* Liegender Spawn-Zombie (EXEC[7], nicht passiv) bleibt treffbar — Entscheidungsfrage des Dossiers an den Nutzer.

## 7. Ergebnis
| Punkt | Urteil |
|---|---|
| 1 Tisch + Fresser, alle Schusswaffen | erfuellt (RE2-KI) |
| 2 Blut an der richtigen Stelle | **teilweise** (HURT-Treffer gefixt; toedlicher Treffer und Aufsteh-Treffer weiter 300-780 daneben) |
| 3 Redhawk/Kraehe wie RE2 | erfuellt |
**bestanden = false** (Punkt 2 teilweise; M1-M3 sind die Nachbesserung, M4-M7 Begleitpunkte).

## Anhang A — Kern der Sonde `abn_blut_tod.c` (Abweichungen von tests/unit/probe_r36_blut.c)
Aufbau, Raum, Bank, Platz, Zielen und `zeichne()` (gezeichnete Pose, Welt-R = Ryaw * R_part) wie probe_r36_blut.c;
zusaetzlich `re15_inv_grant(waffe, 250)`. Geaendert:
```c
for (int f = 0; f < 2400; f++) {                       /* statt 400 */
    ... Schuss alle 12 Bilder ab steht_seit+20, solange tot_ab < 0 (kein "z->state == 1"-Gate) ...
    frame(cr, ed); zeichne(z, &cur);
    if (gespawnt && prev.ok) {                         /* JEDER Spawn, kein Part-0/1-, kein HP-Filter */
        a_t = (P8.x, P8.y + 300, P8.z)        -> appl(&prev, Part 0)   /* TOD Zone != 0  @0x8010865c-80 */
        a_h = (P8.x - 100*seite, P8.y + 300, P8.z) -> Part 0           /* HURT           @0x80105680-b4 */
        a_0 = (0, 800, 0)                     -> Part 1                /* Zone 0         @0x801056dc / @0x80108694 */
        a_g = (0, 300, 0)                     -> Part 8                /* Aufstehen      @0x80107a98 + @0x80107b20 */
        printf Abstaende fx <-> Becken(T-1), Part8, alle vier Sollpunkte;
    }
    if (tot_ab < 0 && (z->hp < 0 || z->state == 3 || z->state == 7)) tot_ab = f;   /* statt break */
    if (tot_ab >= 0 && f > tot_ab + 60) break;
}
```
Bauzeile (aus build.ninja, Ziel probe_r36_blut): `gcc -DRE15_ASSETS_PATH=... -DRE15_ASSET_PSX_DIR="..."
-DRE15_PLATFORM_PC=1 -I re15_port/include -std=c11 abn_blut_tod.c engine/libre15_engine.a tests/libre15_test_support.a
-lm -lkernel32 -luser32 -lgdi32 -lwinspool -lshell32 -lole32 -loleaut32 -luuid -lcomdlg32 -ladvapi32`.

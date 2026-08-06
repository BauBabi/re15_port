# Bug-Dossier: ROOM1030-Kamera beim Wiederbetreten + Treppenhaus-Kamera

Nutzer-Report 2026-08-06:
1. „Wenn ich ROOM1030 erneut betrete, wird die Kamera wieder nicht auf den SPIELER gelegt, sondern auf die ZOMBIES."
2. „Im Treppenhaus schwenkt nach dem Runterlaufen der letzten Treppe die Kamera teilweise nicht richtig um."

Bearbeitung: 4 parallele RE-Lanes + adversariale Gegenprüfung jeder Lane. Alle unten zitierten
Adressen/Bytes habe ich in dieser Sitzung **selbst** aus `info/Re1.5/PSX.EXE` bzw. den RDTs
reproduziert (re15_disasm.py / Roh-Python), nicht aus einem Decompilat übernommen.
Baseline: **113/113 ctests grün** (die im Auftrag genannten „117" sind veraltet).
**Es wurde kein Engine-/Plattform-Code geändert** — nur Messsonden angelegt.

Legende: **BELEGT** = Disasm-Zitat + Messung · **WAHRSCHEINLICH** = Mechanismus belegt,
Kausalkette zum Symptom nicht geschlossen · **OFFEN** = ungemessen.

---

## 1. ROOM1030 beim Wiederbetreten — **BELEGT**, und **JA: durch e53fb043 scharf geschaltet**

### 1.1 Das Symptom, gemessen

Sonde `re15_port/tests/unit/probe_cam_1030_reentry.c` fährt den echten Tür-Pfad
(`re15_room_apply_pending`) 1030 → 1040 → 1030:

```
Phase 4: active_cut_idx=9  (Tuer wollte 8)  cam_change_pending=1
Sweep: Vorraum-Cut 0/3/4/6 -> Eintritts-Cut 9/10/11/12; alle anderen -> korrekt 8
```

Cut 9 ist byte-identisch zu Cut 0 (selbst gedumpt, ROOM1030.RDT Kamera-Tabelle @Datei 0x60,
32 B/Eintrag, `ncut=13` @Datei 0x01):

```
cut0  00003c68 decdffff d6f3ffff d2d9ffff 8ee1ffff 6cf7ffff 52a8ffff e4060000
cut9  00003c68 decdffff d6f3ffff d2d9ffff 8ee1ffff 6cf7ffff 52a8ffff b4120000
                                                                    ^^^^ nur +0x1C (Maskenzeiger)
```

= Kamera (−12834,−3114,−9774) mit Blick auf (−7794,−2196,−22446) — die **Haupthalle mit den
Zombies** (Spawns z −25655..−30000 aus main00). Der Spieler steht derweil auf dem 1040-Tür-Spawn
(−26214, 0, −3861) im West-Gang, den Cut 8 rahmt. Exakt der Report.

Tür selbst gedumpt: `ROOM1040.RDT @0x10b8` = `3b 01 02 31 … dest=0x0003 → Raum 0x1030, cut=8`.

### 1.2 Der Mechanismus — `op_cut_replace` (0x4B) verschiebt im Port die Kamera; das Original tut das NIE

**Original, selbst disassembliert** (`LAB_80040414`, Dispatch 0x800744a8[75]):

* Tausch-Schleife `@0x8004044c-0x800404a8`: über alle RVD-Zeilen (RDT+0x28, Stride 20,
  Terminator `entry[2]==0xFF`) werden `+2` (cam_from) und `+3` (cam_to) a↔b vertauscht.
* Schwanz `@0x800404ac-0x800404cc`:

```
800404b0: lw   v0,-14444(v0)      ; DAT_800ac794 = gebundener RVD-Gruppen-ANKER
800404b8: lbu  v0,2(v0)           ; dessen cam_from — SCHON GETAUSCHT
800404c0: bne  v0,a2,0x800404d0   ; a2 = pc[1] = a
800404c8: jal  0x800142f4
800404cc: addu a0,a1,zero         ; a0 = pc[2] = b
```

`FUN_800142f4 @0x800142f4` (selbst disassembliert):

```
80014300: sb  a0,-1099(at)        ; DAT_800afbb5 = angezeigter Cut
80014304: jal 0x80014324          ; FUN_80014324 = erste RVD-Zeile mit +2 == a0
80014310: sw  v0,-14444(at)       ; DAT_800ac794 = dieser Anker
```

`DAT_800ac794` hat game-weit **genau einen Schreiber** (`ghidra1_V2.txt` Z.473749:
`XREF[3]: 80014234(R), 80014310(W), 800404b0(R)`), und `FUN_80014324 @0x80014324` verlässt seine
Schleife nur bei Treffer → **Invariante `anker[+2] == DAT_800afbb5` ist strukturell garantiert**,
nicht bloß beobachtet.

Daraus folgt, weil die Tausch-Schleife **vor** dem Test läuft:

```
post_swap(anker+2) == a   ⟺   pre_swap(anker+2) == b   ⟺   angezeigter Cut == b
→ FUN_800142f4(b) schreibt denselben Wert zurück (@0x80014300)
```

**Cut_replace ändert den angezeigten Cut im Original also NIEMALS.** Der Schwanz ist ein reines
**Zeiger-Fixup**: er hängt `DAT_800ac794` auf die im neuen Labeling gültige Gruppe um.
Gegenprobe aus den Daten: Cut_chg `@0x800402f4` und Cut_old `@0x80040354` setzen zusätzlich das
Kamera-geändert-Flag `DAT_800b5457`; Cut_replace fasst es nie an.

**Port** (`re15_port/engine/src/scd_vm.c:3582-3589`):

```c
extern int g_re15_active_cut;
if ((uint8_t)g_re15_active_cut == a) {
    g_scd.cam_id_prev        = g_scd.cam_id;
    g_scd.cam_id             = b;
    g_scd.cam_change_pending = 1;
}
```

Drei Divergenzen auf einmal:
1. **falsches Prädikat** — getestet wird `cur == a`, also genau der Fall, den das Original ausschließt;
2. **falsche Wirkung** — echter Kamerawechsel statt Neu-Ankern;
3. **falsche Variable** — `g_re15_active_cut` ist der **Licht**-Cut-Index (einziger Schreiber
   `light_common.c:72` in `re15_light_apply_cut`) und trägt beim SCD-Init noch den Cut des
   **Vorraums**: `room_common.c:179` (`scd_room_reenter`) läuft vor `room_common.c:212`
   (`re15_light_apply_cut`).

Die Wirkkette schließt `room_common.c:192-197`:

```c
int scd_queued_cut = (g_scd.cam_change_pending && (int)g_scd.cam_id != cut) ? (int)g_scd.cam_id : -1;
if (scd_queued_cut >= 0) { cut = scd_queued_cut; *c->cam_active_cut = cut; }
```

→ der Tür-Eintritts-Cut 8 wird **verworfen**, Cut 9 gebaut (`:201`) und dessen BG geladen (`:205`).

Das Original setzt den Tür-Cut dagegen **vor** dem Skript: Raumlader `FUN_800396fc`
`@0x80039940 lh a0,4068(a0)` (DAT_800b0fe4) + `@0x80039944 jal 0x800142f4`, SCD-Raum-Init erst
`@0x80039a00 jal 0x8003ef6c`. Quelle des Werts ist der Tür-Record: `@0x8001d930 lbu v1,10(a0)` →
`@0x8001d940 sb DAT_800afbb5` → `@0x8001d948 sh DAT_800b0fe4`.

### 1.3 Warum es *jetzt* auftritt — Regressions-Einordnung

Selbst aus den RDT-Bytes reproduziert (`ROOM1030.RDT`, sub_scd-Tabelle @0x1fd0):

```
sub00 = 0x1fe8 : 22 03 74 01   Set(3,116,1)
        0x1fec : 06 00 1e 00   Ifel_ck
        0x1ff0 : 21 04 0f 01   Ck(4,15) == 1        <-- GATE
        0x1ff4 : 4b 00 09      Cut_replace(0,9)
        0x1ff7 : 4b 03 0a      Cut_replace(3,10)
        0x1ffa : 4b 04 0b      Cut_replace(4,11)
        0x1ffd : 4b 06 0c      Cut_replace(6,12)
sub01 = 0x2180 … 0x2198 : 22 04 0f 01  Set(4,15,1)   <-- einziger Setzer game-weit (+1030-Zwilling 1031 @0x2284)
sub08 = 0x2764 … 0x27aa 29 0c / 0x27c8 29 0b / 0x27ce 29 05 / 0x27d0 3c 01
                          Cut_chg 12 / 11 / 5 + Cut_auto(1)   <-- die Kinofahrt, NICHT Cut_replace
```

* Der **Defekt in `op_cut_replace` ist älter** (eingebaut 2026-06-30, Commit eb4322ec, Kommentar
  „live in-room replacement", Guess-Tell „faithful-line" im Code).
* Er war in ROOM1030 **unerreichbar**: `sub00`s Cut_replace-Block hängt hinter `Ck(4,15)==1`, und
  `flag(4,15)` wird game-weit nur in `sub01 @0x2198` gesetzt — hinter den transienten
  Bank-5-Handshake-Bits (5,32)/(5,34). Vor e53fb043 lief `sub01` nicht zuverlässig pro Frame,
  die Cutscene feuerte live nie, der Zweig war toter Code (`probe_cut_1030` auf 483200de: `(4,15)=0`).
* Mit **e53fb043** (per-Frame-Reseed von Thread-Slot 1 auf `sub_scd[1]`) feuert die Cutscene,
  `(4,15)=1`, und `sub00` wendet den Remap bei **jedem** Wiederbetreten an → der latente Defekt
  wird sichtbar (`probe_cut_1030` auf HEAD: `(4,15)=1`, cam 12→11→5, `cine_active=1`).

**Klartext: JA — der Nutzer sieht diesen Bug wegen e53fb043.** e53fb043 selbst ist korrekt und
darf nicht zurückgenommen werden; der Fix gehört in den Schwanz von `op_cut_replace` (eb4322ec).

### 1.4 Was ausdrücklich NICHT die Ursache ist — **BELEGT**

Die Ausgangs-Hypothese („Cut_replace verdrahtet die Zonentabelle um, und das überlebt den
Raumwechsel") ist **widerlegt**:

* Der Raumlader lädt die RDT bei jedem Betreten frisch in die Arena (`@0x80039704 lw 0x800ac780`,
  `@0x80039740 sw 0x800ac778`, Header-Reloc `@0x80039800-34`) — der Tausch ist weg.
  Der Port macht dasselbe (`room_pc.c` re-parst, `scd_room_setup.c:175-178` re-installiert die AOTs).
* Gemessen (`probe_cam_1030_reentry` Phase 4): „RVD-Zonen FRISCH aus Datei" sind unverändert
  (`0>0 0>1 0>3 …`), während `rdt->zones` getauscht sind (`9>9 9>1 9>10 …`).
* Dass die Umverdrahtung beim Wiederbetreten trotzdem dasteht, ist **byte-true** — `sub00` wendet
  sie jedes Mal neu an. **Diese Hälfte darf nicht angefasst werden.**

Ebenfalls byte-true und nicht anzufassen (geprüft): Anker-Skip (`rdt_common.c:431` ⟷
`@0x80014254/@0x80014260`), „erster Treffer gewinnt" (`@0x800142ac/@0x800142b4` ⟷ Port „letzter
Treffer bei aufsteigenden Slots"), Slot-Kapazität (Max über 240 RDTs = 60 Zeilen / 44 Nicht-Anker),
und die Gruppen-Kontinuität nach dem 4-fach-Tausch (Anker-Indexmenge vor/nach identisch).

---

## 2. Treppenhaus — Raum **BELEGT**, „teilweise" **BELEGT**, Ursache **WAHRSCHEINLICH/OFFEN**

### 2.1 Welcher Raum: **ROOM1060** — BELEGT

Türgraph selbst aus den RDTs gelesen:

```
ROOM1120.RDT @0x0c76 : 3b 00 02 31 …  dest=0x0006 → ROOM1060, Spawn (26000,-14400,25300), floor=8
ROOM1060.RDT @0x0d12 : dest 0x12 → ROOM1120  (Band 8, oben)
ROOM1060.RDT @0x0d32 : dest 0x0c → ROOM10C0  (Band 4, Mitte)
ROOM1060.RDT @0x0d52 : dest 0x04 → ROOM1040  (Band 0, unten)
ROOM1040.RDT @0x10b8 : dest 0x03 → ROOM1030
```

→ Die aktuelle Kette ist **1130 → 1120 → 1060 → 1040 → 1030**: der Nutzer läuft das Treppenhaus
unmittelbar vor dem ROOM1030-Bug. ROOM10A0 ist der bau-gleiche Zwilling (später über ROOM1050).

Acht Treppen-Records (`Aot_set` 0x2C, `sat=0x31`, `count=2`), selbst gedumpt:

```
@0x0d72 nfloor=08  Ost  rect(23800,19800,2700,4400)   Stufe A: 8<->6
@0x0d86 nfloor=06  Ost  rect(23800,19800,2700,4400)
@0x0d9a nfloor=06  West rect(20200,20700,2400,4300)   Stufe B: 6<->4
@0x0dae nfloor=04  West rect(20200,20700,2400,4300)
@0x0dc2 nfloor=04  Ost  rect(23800,19800,2700,4400)   Stufe C: 4<->2
@0x0dd6 nfloor=02  Ost  rect(23800,19800,2700,4400)
@0x0dea nfloor=02  West rect(20200,20700,2400,4300)   Stufe D: 2<->0  <-- die LETZTE
@0x0dfe nfloor=00  West rect(20200,20700,2400,4300)
```

Die Kamera ist dort **rein RVD-getrieben**: SCD-Opcode-Zensus des Raums (RE15_SCD_TRACE) ergibt nur
0x01 (13×), 0x2C (9×), 0x3B (4×) — **kein einziges Kamera-Opcode** (0x29/0x2A/0x3C/0x4B).
Punkt 1 und Punkt 2 haben damit **keine gemeinsame Code-Stelle**.

### 2.2 Warum „teilweise" — der Mechanismus, **BELEGT**

`FUN_80014230 @0x80014230` ist eine **Cut-Zustandsmaschine**, kein Positionstest:

```
80014234: lw   a0,-14444(a0)   ; Anker DAT_800ac794 (Gruppe des AKTUELLEN Cuts)
8001423c: lbu  v0,-1099(v0)    ; DAT_800afbb5 = aktueller Cut
80014254: lbu  v1,22(a0)       ; cam_from der FOLGE-Zeile (Anker selbst wird nie getestet)
8001425c: bne  v1,v0,ende      ; Gruppe hat keine Transition -> raus
8001426c: lbu  v1,-1(s0)       ; Zonen-Floor; 0xFF = egal, sonst == DAT_800acad6
80014298: jal  0x80014368      ; Punkt-in-Quad
800142ac: jal  0x800142f4      ; ERSTER Treffer gewinnt, Return @0x800142b4
800142d0: beq  v1,v0,…         ; Abbruch am Gruppenende
```

Eine Zone feuert also **nur, wenn ihr `cam_from` == dem gerade angezeigten Cut ist.**

ROOM1060 RVD-Tabelle (@Datei 0x160, 22 Einträge, **alle floor=0xFF**), selbst gedumpt:

```
 0  0->0   x[17500..29000] z[16000..29000]   (Gruppen-Anker, wird übersprungen)
 1  0->1   x[23000..27500] z[21500..22500]
 4  1->2   x[19500..23000] z[22000..23000]
 7  2->3   x[23000..27500] z[23000..24000]
10  3->4   x[23000..27500] z[20000..21000]
13  4->5   x[19500..23000] z[24000..25000]   <-- EINZIGE Zone der LETZTEN Treppe
16  5->7   x[23500..24500] z[24500..27500]   <-- danach der Ausgangs-Cut 7
```

**Die letzte Treppe (D, West) hat genau EINE zuständige Zone: `RVD[13] 4→5`.**
Wer unten ankommt und **nicht** auf Cut 4 steht, löst nichts aus — die Kamera bleibt stehen.
Das ist das „teilweise": es hängt **nicht** an der letzten Treppe, sondern daran, ob die Kette
`0 → 1 → 2 → 3 → 4` auf dem Weg nach unten **vollständig** durchlaufen wurde.

Sweep gemessen (`probe_stair_cam_1060`, letzte Treppe, Start (21400,21500), alle einlaufenden Cuts):

```
cut_in=0 -> Endcut 0 / Tuer 0     cut_in=4 -> 5 / 7   KORREKT
cut_in=1 -> 2 / 2                 cut_in=5 -> 5 / 7   (mit Zwischen-Flip 5->4->5)
cut_in=2 -> 2 / 2  (Ping-Pong)    cut_in=6 -> 6 / 7
cut_in=3 -> 3 / 3                 cut_in=7 -> 7 / 7
```

### 2.3 Wovon der Kettenbruch abhängt — drei Kandidaten

**(a) Kettenbruch schon auf Treppe C — WAHRSCHEINLICH, Original-Anteil OFFEN.**
Gemessen: Treppe C (Ost, 4↔2) erreicht Cut 4 nur, wenn sie aus z ≥ 23100 ausgelöst wird. Für die
ebenfalls gültigen Startpositionen z 22100/22350/22600/22850 bleibt die Folge auf Cut 2 (Zone [7]
`2→3` z[23000..24000] wird nie durchlaufen, also greift auch Zone [10] `3→4` nicht, die
`cam_from=3` verlangt). Ergebnis: unten steht Cut 2 statt 4 → letzte Treppe schwenkt nicht.
**OFFEN:** ob ein normal laufender Spieler diese Startposition auch im Original erreicht.
Die Sonden-Startposition ist geometrisch legal (620-Vorwärtssonde bleibt im Rechteck), d.h.
das könnte ein **Original-Quirk** sein. Ohne DuckStation-Trace nicht entscheidbar.

**(b) Port-eigener „Forward-Eject" im Finalize — BELEGT als Divergenz, Kausalität WIDERLEGT für die letzte Treppe.**
Original `LAB_80038e50 @0x80038e50-0x80038eec`, vollständig selbst disassembliert: setzt Clip 2
(`sb 2 → 0x800acae8 @0x80038e68`), `acae9=0`, `acae3=0`, `jal anim_set 0x8001f314`, gibt die
Kontrolle zurück (`sb zero → 0x800aca59/5a/5b`), rechnet das Band aus dem committeten Y
(`-0x800acc0e × 0x91a2b3c5 >> 10 → sb 0x800acad6 @0x80038edc`) und löscht `player+0x93` Bit 0.
**Kein einziger Store nach 0x800aca88 / 0x800aca90 — das Original repositioniert NICHTS.**
Der Port dagegen (`stair_common.c:162`) marschiert bis zu 48 × 100 Einheiten entlang der
Blickrichtung und committet das in EINEM Tick; gemessen ±2857 Einheiten (Manhattan; euklidisch
2394), Endpunkte z=29418 bzw. z=15732 — außerhalb jeder RVD-Zone (die echten Zonen reichen nur
z[20000..27500]). Da `FUN_80014230` genau **einmal pro Frame auf genau EINEM Punkt** läuft
(einziger Aufruf `@0x8001ccec`, gegated nur durch `andi 0x100` `@0x8001cce0`), feuert jede
übersprungene Zone nicht.
**ABER:** die eigene Messung widerlegt den Kausalschluss für die letzte Treppe — die Zeile mit
Eject +2857 endet trotzdem auf Cut 5 / Tür-Cut 7, weil der Schwenk bereits bei F18 während der
Gait passiert. Der Eject ist eine echte, belegte Divergenz, aber **kein bewiesener Auslöser des
gemeldeten Symptoms**.

**(c) `STAIR_REACH 450` = Geister-Treppe am Landepunkt — BELEGT als Port-Erfindung, WAHRSCHEINLICH mitursächlich.**
Der Action-Scan `FUN_80042bac` wählt den Geometrietest nach `record[1]`:
`andi v0,v1,0x40 @0x80042ea8` = Test mit der Spieler-Mitte, `andi v0,v1,0x20 @0x80042ef8` = Test
des 620-Vorwärtspunkts (`ori v0,zero,0x26c @0x80042bd0`) gegen das **exakte** Rechteck.
Alle acht ROOM1060-Treppen-Records tragen `sat=0x31` → **Bit 0x20 gesetzt, Bit 0x40 CLEAR** →
das Original testet ausschließlich die Vorwärtssonde.
Port: `stair_common.c:335` verodert zusätzlich `player_in_zone()` (Körper im Rechteck ± 450,
`#define STAIR_REACH 450` `stair_common.c:41`, im Code selbst als **„port fallback, no PSX
referent"** markiert). Gemessener Landepunkt der letzten Treppe (21400, 25368) vs. Rechteck-Mitte
z=22850 ± 2150: |25368−22850| = 2518 ≤ 2600 → **Port triggert**; Original-Sonde 25368+620 = 25988
> 25000 → **Original triggert nicht**. Gemessene Folge im Port: `UP`, 64 Frames, gaitZ = 29513 —
aus dem Raum heraus, Kamera bleibt stehen. Zusätzlich konsumiert der Treppenstart die Taste
**vor** dem Tür-Scan (`game_step_common.c:725`).
**Einschränkung (ehrlich):** mit Blickrichtung −z liegt die 620-Sonde bei z=24748 **im** Rechteck —
dann startet auch das Original diese Aufwärts-Treppe. Port-spezifisch ist nur der Fall
„Blick talwärts + SQUARE am Landepunkt". Und die Ausgangstür liegt ~5700 Einheiten in x entfernt,
der Fehlstart braucht also einen Druck **am Landepunkt**, nicht am Türgriff.

**(d) Ping-Pong an Zonengrenzen — gemessen, Original-Referenz OFFEN.**
`probe_stair_cam_1060`: Treppe C `2→3(F0)→4(F36)→3(F39)→4(F41)` an der Grenze z=21000
(Zone[10] z[20000..21000] vs Zone[12] z[21000..22000]); letzte Treppe mit cut_in=5:
`5→4(F21)→5(F43)` an z=24000. Die früher berichtete Erklärung („die FK-Fußverriegelung läuft
rückwärts") ist **als geschrieben widerlegt**: die Prämisse „der Original-Abstieg ist monoton" hat
kein Instruktions-Zitat, `FUN_800390e0 @0x800390e0-0x80039268` **liest** den Fuß-Cache nur
(kein Store nach s3+84/88/92), und die Messung, auf die sie sich stützte, ist nicht reproduzierbar
(die erzeugende Sondenquelle wurde durch eine Datei-Kollision zweier Agenten überschrieben).
→ **OFFEN**, muss neu gemessen werden.

### 2.4 Ausdrücklich AUSGESCHLOSSEN (damit niemand dort sucht) — BELEGT

* **Zonen-Auswahl des Ports ist byte-identisch.** `probe_rvd_diff_10a0` (ROOM10A0, 28 Einträge,
  Gitter x[18000..29000] × z[16000..28000] Schritt 50 × 10 Cuts): **532610 Stichproben,
  26267 Original-Treffer, 0 Abweichungen.** Zonentabelle, AOT-Installation und Auswahlreihenfolge
  scheiden aus.
* **RVD-Floor-Gate ist hier inert.** ROOM1060 hat 22× `floor=0xFF`, ROOM10A0 28× `0xFF`.
  (Game-weit haben nur 20 RDTs überhaupt `floor != 0xFF`.)
* **Kein Treppen-Code setzt je einen Cut.** Aufruferliste von `FUN_800142f4` game-weit:
  `@0x800142ac, @0x80039944, @0x80040300, @0x80040368, @0x800404c8, @0x800466e8, @0x8004c688` —
  kein Treppen-Handler darunter.
* **Kein Treppen-Code fasst das Auto-Scan-Gate an.** Erschöpfende Auswertung aller 50 Stores auf
  `DAT_800aca3c`: Bit 0x100 wird von genau fünf Stellen berührt — SET `@0x800402d4` (Cut_chg),
  `@0x800403f4` (Cut_auto ≠1); CLEAR `@0x8004037c` (Cut_old), `@0x800403d8` (Cut_auto ==1),
  `@0x80039728` (Raumlader). Die Treppen-Handler schreiben `@0x800435ac`/`@0x80043678` nur Bit 0x40.

---

## 3. Gemeinsame Ursache? — **NEIN** (aber ein gemeinsames Muster)

Die beiden Reports haben **keine gemeinsame Code-Stelle und keine gemeinsame Ursache**:

* Punkt 1 sitzt in der **SCD-VM** (`op_cut_replace`) und wird über `room_common`s
  Kamera-Präzedenz wirksam. Er ist ein **skript-getriebener** Fehler.
* Punkt 2 sitzt in der **Treppen-/Trajektorien-Schicht** (`stair_common`) bzw. ist womöglich
  ein Original-Quirk der RVD-Zustandsmaschine. ROOM1060 enthält **kein einziges Kamera-Opcode**,
  `op_cut_replace` kann dort nichts tun.
* Auch die Regressions-Einordnung trennt sie: Punkt 1 = durch e53fb043 scharf geschaltet;
  Punkt 2 = `git show --stat e53fb043` berührt weder `stair_common.c` noch `aot_common.c` noch
  `rdt_common.c`; `STAIR_REACH`, der 48-Schritt-Marsch und das `cut_auto`-Forcing datieren auf
  aa346af1 → **NEIN, bestand vorher**.

**Gemeinsames Muster** (kein Fix, aber die Lehre): Der Port hält **drei** parallele Notionen von
„aktueller Cut" (`g_scd.cam_id`, `main.c active_cut_idx/s_last_cut_idx`, `g_re15_active_cut`),
das Original nur zwei mit klarer Rolle (`DAT_800afbb5` angezeigt, `DAT_800b0fe4` Raumlader-Eingang).
Beide Bugs entstehen dort, wo der Port an einer Stelle, an der das Original **nichts** schreibt,
etwas schreibt (Kamera bzw. Position) — und die Begründung im Kommentar jeweils ein Guess-Tell
trägt („faithful-line", „port fallback, no PSX referent").

---

## 4. Fix-Plan (minimal, je mit Beleg, Einfügeort und Regressions-Gate)

### FIX 1 — ROOM1030: Schwanz von `op_cut_replace` ersatzlos streichen — **BELEGT, fix-reif**

**Ort:** `re15_port/engine/src/scd_vm.c:3582-3589` (der komplette `{ extern int
g_re15_active_cut; if (…) { … } }`-Block). Die Zonen-Schleife (`:3561-3570`) und
`re15_aot_cut_replace` (`:3572-3575`) bleiben **unangetastet**.

**Beleg im Code-Kommentar (ersetzt `scd_vm.c:3575-3581`):**
> Schwanz `LAB_80040414 @0x800404ac-0x800404cc` ist ein **Zeiger-Fixup**, kein Kamerawechsel.
> `lw DAT_800ac794 @0x800404b0` / `lbu +2 @0x800404b8` liest den cam_from des Ankers **nach**
> dem Tausch (`@0x8004044c-a8`); `bne …,a2 @0x800404c0` trifft daher genau dann, wenn der
> angezeigte Cut bereits `b` war, und `FUN_800142f4(b) @0x800404c8` schreibt denselben Wert
> zurück (`sb a0,DAT_800afbb5 @0x80014300`). Einziger Schreiber von `DAT_800ac794` ist
> `@0x80014310`, und `FUN_80014324 @0x80014324` bricht nur bei Treffer ab → `anker[+2] ==
> DAT_800afbb5` ist strukturell garantiert. Der Port hält keinen persistenten Anker
> (`aot_common.c:657` filtert `cam_from_filter` pro Frame gegen den als Parameter übergebenen
> aktiven Cut), daher ist das Port-Äquivalent exakt „nichts tun".

**ZWINGEND mitzuändern** (sonst ist der Build rot — empirisch verifiziert: 112/113):
`re15_port/tests/unit/test_scd_opcodes.c:1052-1075` `test_cut_replace_live_switch` pinnt
derzeit exakt das divergente Verhalten (Fall (a): `g_re15_active_cut=9`, `Cut_replace(9,8)`,
erwartet `cam_id==8 && pending==1`). Umstellen auf **`pending == 0` in BEIDEN Fällen**,
mit `@0x800404b0`/`@0x80014300` im Kommentar. Ebenso `RE15_SCD_OPCODES_REFERENCE.md` Z.28
(„FIXED: added the byte-true tail").

**Regressions-Gate:**
| Gate | Sollzustand nach dem Fix |
|---|---|
| `probe_cam_1030_reentry` (neu, `tests/unit/`) | `active_cut_idx=8` für **alle 13** Sweep-Zeilen; echter Tür-Pfad 1030→1040→1030 = 8 |
| `probe_cut_replace_1030` (neu) | `pending==0` durchgängig; FINAL == Tür-Cut (1040→8, 1050→2, 1070→7, 1031→6, 1241→0) |
| `unit_scd_opcodes` | nach Umstellung grün |
| `unit_rvd_scan_order` (#62), `integration_rvd_coverage` (#108) | unverändert grün |
| `unit_room1150_itembox`, `unit_room1040_switch`, `unit_room1070_wake`, `unit_stair_1170_regression`, `integration_door_traversal` | unverändert grün (empirisch bestätigt) |
| Volle Suite | 113/113 |

**Risiko + Entwarnung:** Der Block wurde 2026-06-30 mit ROOM1130 begründet („live in-room
replacement"). Diese Begründung ist **sachlich falsch**: ROOM1130s drei `Cut_replace(9,8)`
liegen bei Datei 0x9e0/0x9ec/0x9f8 **in sub00** (Tabelle @0x9c8, sub00 = 0x09ce, hinter
`21 03 6b 01` = `Ck(3,107)==1`), also im **Raum-Ladepfad**; die Eintritts-Cuts nach ROOM1130
sind 0/2/6/7, nie 9 → der Original-Schwanz feuert dort ohnehin nie. Der Kamerawechsel kommt
dort aus dem **Zonen-Relabel**, das der Fix nicht anfasst. Nach dem Fix zusätzlich je eine
Kurzsonde für **ROOM1130, ROOM11B0 und ROOM3090** nachziehen (ROOM3090 ist der schlimmste Fall:
sub00 tauscht 0..6 gegen 8..14, dort löst heute jeder Vorraum-Cut 0–6 den Fehler aus).

*Nebenbefund, der den Fix stützt:* der Port-Schwanz vergleicht gegen den **Vorraum**-Cut.
ROOM1140 (ncut=10) und ROOM1170 (ncut=13) lassen sich auf Cut 9 verlassen → ROOM1130 öffnet heute
auf Cut 8 statt auf dem Tür-Cut. Der Fix behebt dort eine **zusätzliche** Divergenz.

---

### FIX 1b — `g_re15_active_cut` beim Raumwechsel vorziehen — **BELEGT, optional, EIGENER Commit**

Mit FIX 1 verschwindet der einzige verhaltensrelevante Leser im kritischen Fenster; der Defekt
wird **latent**. Sauber wäre zusätzlich in `re15_port/engine/src/room_common.c` direkt nach
Zeile **167** (`*c->cam_active_cut = cut;`, also **vor** `scd_room_reenter` in Zeile 179):
`g_re15_active_cut = cut;` — das Port-Gegenstück zu `@0x80039944`, das `DAT_800afbb5` ebenfalls
vor der SCD-Init setzt.
**Gate:** `unit_camera_view` (#75), `integration_rvd_coverage`, `probe_cam_1030_reentry`.
**Risiko:** praktisch null — zwischen Schritt (5) und (10) wendet niemand Licht an
(die Leser in `platform/pc/src/main.c:5052/5927/6248` laufen im Render).
**Nicht** als Ersatz für FIX 1 einbauen: die Prädikat-/Wirkungs-Divergenz bliebe sonst bestehen.

---

### FIX 2 — Treppenhaus, gestaffelt

#### 2a — `player_in_zone()` aus dem Treppen-Zonen-Match entfernen — **BELEGT, umsetzbar**

**Ort:** `re15_port/engine/src/stair_common.c:335`
(`point_in_zone(fx, fz, a) || player_in_zone(p, a)` → nur der flags-gesteuerte Test);
`STAIR_REACH` (`:41`) und `player_in_zone` (`:90-93`) entfallen.
**Beleg:** die Testauswahl folgt `record[1]`: Bit 0x40 `@0x80042ea8` = Zentrumstest,
Bit 0x20 `@0x80042ef8` = 620-Vorwärtssonde (`ori v0,zero,0x26c @0x80042bd0`) gegen das exakte
Rechteck. Alle ausgelieferten Treppen-Records tragen `sat=0x31` (selbst gedumpt, ROOM1060
@0x0d72…0x0dfe) → **nur die Sonde**. Nicht hart verodern, sondern aus `a->sce_flags` ableiten —
das Feld wird für 0x2C real befüllt (`scd_vm.c:2431 = t->pc[3]`), und das 0x40/0x20-Muster
existiert im Port bereits byte-true (`aot_common.c:865/872`).
**Gate:**
| Gate | Sollzustand |
|---|---|
| `unit_stair_1170_regression` (#93, `probe_stair_1170`) | alle vier ROOM1170-Zonen in **beiden** Richtungen lösen weiterhin aus |
| `probe_stair_10a0` | acht ROOM10A0-Zonen, Zielband/Richtung unverändert |
| `probe_stair_cam_1060` (neu) + neue Assertion | Landepunkt (21400, 25368) + SQUARE mit Blick talwärts löst **NICHT** aus |
| gdigrab-Live-Lauf 1120 → 1060 → 1040 (Skill `re15-port-visual-verify`) | jede der vier Treppen von Hand auslösbar |
**Risiko:** `STAIR_REACH` wurde eingeführt, weil die Port-Kollision den Spieler außerhalb des
Rechtecks hält. Ein reiner Sondentest kann Treppen **unauslösbar** machen, wenn der Port anders
klemmt als das Original → der Live-Lauf ist hier **Pflicht**, nicht optional.

#### 2b — Finalize-Eject: **NICHT ohne Original-Trace anfassen** — WAHRSCHEINLICH/OFFEN

**Ort:** `re15_port/engine/src/stair_common.c:150-190` (Marsch-Schleife `:162`).
**Beleg dafür, dass es falsch ist:** `LAB_80038e50 @0x80038e50-0x80038eec` enthält **keinen**
Store nach `0x800aca88`/`0x800aca90`.
**Warum trotzdem noch kein Fix:**
1. Die naheliegende „Sofort-Mitigation" (Teilschritte einzeln durch `re15_aot_scan` schicken) ist
   **selbst nicht byte-true**: `@0x8001ccec` beweist genau **einen** Scan pro Frame auf **einem**
   Punkt. Sie tauscht eine Divergenz gegen eine andere.
2. Der eigentliche Fix (Endpunkt aus PL00-Clip 20/21 + `FUN_800390e0` + `FUN_800245d8` korrekt
   reproduzieren, Eject entfallen lassen) ändert die Landeposition **aller** Treppen im Spiel und
   kann den historischen Wand-Hänger zurückbringen (`stair_common.c:143-158`: ROOM1170 4→2
   x=−23309 → Rückwärts-Push). Die Behauptung „`FUN_8003b0a4` übernimmt das Freischieben" steht
   bisher **nur im Port-Kommentar**, nicht im Disasm — der zirkuläre Rest, der zuerst zu
   schließen ist.
**Vorbedingung (Messung, kein Code):** DuckStation-Savestate ROOM1060 (Skills
`re15-room-capture` + `re15-savestate-ghidra`), pro Frame lesen: Spieler-X `@0x800aca88`,
Z `@0x800aca90`, Band `@0x800acad6`, angezeigter Cut `@0x800afbb5`, Anker `@0x800ac794`.
**Gate, wenn es soweit ist:** `unit_stair_1170_regression` **um eine Landepositions-Assertion
erweitern** (fängt heute nur Richtung + Zielband, würde einen Positions-Regress **nicht** fangen),
plus `probe_stair_eject_10a0` (max. Einzelschritt ≤ Vorwärts-Schrittweite) und
`probe_rvd_diff_10a0` (Zonenauswahl bleibt 0 Abweichungen).

#### 2c — `cut_auto_enabled` im Treppen-Ast nicht erzwingen — **BELEGT, klein**

**Ort:** `re15_port/engine/src/game_step_common.c:455` (`g_scd.cut_auto_enabled = 1;`) — streichen,
den `re15_aot_scan`-Aufruf stehen lassen (`aot_common.c:707` gated selbst).
**Beleg:** die fünf Schreiber von Bit 0x100 (§2.4) enthalten keinen Treppen-Pfad; die
Treppen-Handler schreiben `@0x800435ac`/`@0x80043678` nur Bit 0x40 (dessen Leser `@0x8001cd04`
gated den Pause-/Inventar-Block, nicht die Kamera).
**Wirkung in ROOM1060/10A0: keine** (kein Cut_chg im Raum) — der Punkt ist Hygiene, **kein**
Fix für Report 2. `scd_room_reenter` setzt das Flag beim Raum-Eintritt ohnehin
(`scd_room_setup.c:114`, byte-true zu `@0x80039728`).
**Gate:** `probe_stair_cam_1060` (Cut-Folge unverändert), `unit_stair_1170_regression`,
`unit_scd_opcodes`.

#### 2d — zweiter `cut_auto`-Schreiber nach der SCD-Init — **BELEGT, EIGENER Commit nach FIX 1**

**Ort:** `re15_port/engine/src/room_common.c:222-223` (`if (scd_queued_cut < 0)
g_scd.cut_auto_enabled = 1;`) und unbedingt `game_step_common.c:781`.
**Beleg:** Der Raumlader-CLEAR `@0x80039728` steht **am Anfang** von `FUN_800396fc`, die SCD-Init
`@0x80039a00` am Ende — ein Cut_chg der Raum-Init lässt Bit 0x100 gesetzt, und **nichts** stellt
es danach wieder her. Der Port hebt die Sperre wieder auf, wenn die Init denselben Cut wie die Tür
anfordert (`cam_id != cut` in `:192` greift dann nicht).
**Risiko:** mittel — die Zeile entstand gegen den ROOM1170-Pre-Intro-Bug. **Erst nach FIX 1** und
getrennt messen.
**Gate:** `unit_stair_1170_regression`, `integration_rvd_coverage`, `unit_rvd_scan_order`,
`probe_cut_1030`, `probe_camload_1030` + Live-Runde ROOM1240 → ROOM1170 (Narrator-Cut 7 muss
schwarz bleiben).

---

### FIX 3 — `Cut_auto` (0x3C): jeder Wert ≠ 1 ist AUS — **BELEGT, Einzeiler, unabhängig**

**Ort:** `re15_port/engine/src/scd_vm.c:3416`
`g_scd.cut_auto_enabled = t->pc[1];` → `g_scd.cut_auto_enabled = (t->pc[1] == 1);`
**Beleg:** `@0x800403bc ori v0,zero,0x1` / `@0x800403c0 bne v1,v0,…` → nur `pc[1]==1` führt zum
CLEAR (`@0x800403d8`), jeder andere Wert zum SET (`@0x800403f4`).
**Reichweite:** praktisch null (Skripte nutzen nur 0/1) — mitnehmen, wenn der Opcode sowieso
angefasst wird. **Gate:** `unit_scd_opcodes`, `integration_rvd_coverage`.

---

## 5. Offene Punkte (bewusst NICHT gefixt)

* **OFFEN — Original-Trace ROOM1060.** Es gibt keine DuckStation-Messung des Treppenhauses.
  Ungemessen: wie weit das Original pro `count=2`-Treppe läuft (Port: konstant ±3761 in 63 Frames),
  ob seine XZ-Spur Rückläufer hat, und welche Cut-Folge es beim Abstieg 1120→1060→1040 zeigt.
  **Solange das fehlt, ist Report 2 nicht abschließend geklärt.**
* **OFFEN — ist der Kettenbruch auf Treppe C ein Original-Quirk?** (§2.3a)
  Nächster Schritt ohne Emulator: SCA-Kollisionszellen von Band 4 auswerten + ROOM10C0s
  Tür-Record nach 1060 lesen — kann der Spieler die Ost-Spalte überhaupt bei z<23000 betreten?
* **OFFEN — Ping-Pong-Ursache** (§2.3d). Die alte FK-Erklärung ist als geschrieben widerlegt;
  die erzeugende Sonde `probe_stair_cam_10a0.c` wurde durch eine Agenten-Datei-Kollision
  überschrieben und muss neu erstellt werden (die gesicherte Ausgabe liegt im Scratchpad
  `…/scratchpad/stair_cam_10a0.txt`). Drei Sonden-Varianten liefern drei verschiedene Endcuts
  (6 / 7 / 1-2) — das Harness widerspricht sich selbst.
* **OFFEN — zweites Scan-Gate `@0x8001ccb0-0x8001cccc`:** `DAT_800ac760 & 0x20004` toggelt `s2`,
  `bne s2,zero,0x8001ccf4` überspringt den Scan (→ nur jeder 2. Frame). Der Port kennt es nicht.
  `DAT_800ac760` ist unbenannt (XREF[21], einziger Schreiber `sh @0x80030564`, deckt Bit 0x20000
  gar nicht ab). **Kein Fix ohne diesen Beleg.**
* **OFFEN — Band-Sentinel 7 während der Traversierung.** Beide Treppen-Handler setzen
  `DAT_800acad6 = 7` (`@0x80038ce4` descend, `@0x80038a28` ascend) und stellen es im Finalize
  wieder her; der Port setzt das Band erst im Finalize. Das RVD-Floor-Gate liest genau diese
  Variable (`@0x80014280`). In ROOM1060/10A0 **inert** (alle `floor=0xFF`), betroffen wären
  14 RDTs mit Floor-Byte 0x07. Eigener Arbeitsschritt; vorher klären, wie ROOM11B0/11C0/11E0
  (ohne Treppe) je Band 7 erreichen.
* **OFFEN — Port hat kein Gegenstück zu `DAT_800acad6` als eigenständigem Floor-Byte**
  (`aot_common.c:699-701` rechnet es aus Y zurück; `op_door_aot_set` liest pc[24], verwirft
  pc[25] = Ziel-Floor). Betrifft 20 RDTs, keine in der aktuellen Kette.
* **OFFEN — `Cut_old`-Vorgängercut überlebt im Original den Raumwechsel** (`DAT_800b3f7b`,
  XREF[2]: `sb @0x800402e4`, `lbu @0x8004033c`, nie initialisiert), im Port nicht
  (`scd_room_setup.c:112` memset). 0x2A hat 37 saubere Verwendungen in 26 RDTs.
  Niedrige Priorität, eigener Commit, eigene Sonde (ROOM1040/10D0/1150).

---

## 6. Sonden-Inventar (angelegt, gebaut, **nicht** als ctest gegated)

```
re15_port/tests/unit/probe_cam_1030_reentry.c    echter Tuer-Pfad 1030->1040->1030 + Zonen-Dump
re15_port/tests/unit/probe_cut_replace_1030.c    5 reale Tuer-Cuts x 13 Vorraum-Cuts
re15_port/tests/unit/probe_camload_1030.c        Raumlade-Kamerakette, stale-Cut-Sweep
re15_port/tests/unit/probe_rvd_diff_10a0.c       Port-Zonenwahl vs FUN_80014230, 532610 Punkte
re15_port/tests/unit/probe_stair_cam_1060.c      ROOM1060: 4 Treppen, Cut-Folge + Eject + Sweep
re15_port/tests/unit/probe_stair_eject_10a0.c    ROOM10A0: Schrittweite pro Tick + Zonenmenge
re15_port/tests/unit/probe_stair_cam_chain.c     Kettenlauf
re15_port/tests/unit/probe_stair_cam_10a0.c      ⚠ Quelle durch Agenten-Kollision ueberschrieben
```

Nach FIX 1 sollten `probe_cam_1030_reentry` und `probe_stair_cam_1060` als **ctest gegated**
werden — sie nageln den Soll-Zustand (Tür-Cut 8 bzw. Cut-Folge 0→1→2→3→4→5→7) fest.

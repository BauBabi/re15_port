# Birkin G5 (ROOM5090) — die drei Restluecken aus Runde 17 §5 (Runde 18, 2026-09-21)

Nutzer, woertlich (Runde 17): *"Birkin ist nicht solid. Auch sonst ist das Verhalten noch immer
nicht wie in resident Evil 2, also beim Endboss. ich will hier eine 1 zu 1 Umsetzung!"*

"Nicht solid" war Runde 17. Diese Runde schliesst die drei Luecken, die
`analysis/befunde_2026-09-20/birkin-1zu1.md` §5 mit Adresse benannt und ausdruecklich NICHT
geraten hatte.

Worktree `.claude/worktrees/agent-aee0c358f957ef23a`, Build `re15_port/build`,
Suite **329/329** (vorher 328, +1 neuer Test: `unit_r18_birkin_restluecken_pin`).

Quellen, alles in dieser Runde selbst disassembliert:
* RE2-Overlay `build/extracted/re2_ems/CDEMD0_EM36_ai1.BIN` (G5, gelinkt @0x80100000),
  geschnitten mit `re15_port/tools/re2_ems_cut.py 0x36 0x37`.
* RE2-EXE `info/re2leon/PSX.EXE` via `.claude/skills/re15-psx-disasm/scripts/re2_disasm.py`,
  Decompilate `RE2_Quellcode_V2/FUN_*.c`.
* Eigener Xref-Scanner (lui/offset-Paare, die `grep` auf dem Ghidra-Textdump nicht sieht) —
  damit wurden die Schreiber/Leser von `0x800CFC6E`, `0x800CFD80/84` und den Ring-Basen
  gefunden.
* Sonde `re15_port/tests/unit/probe_r18_birkin_rest.c`, Registrierung
  `re15_port/tests/unit/probes/r18_birkin-restluecken.cmake`.
* Echte exe (`re15_port/build/platform/pc/re15_pc.exe`), `RE15_FRAMEDUMP` (Vollbild-Readback
  unmittelbar VOR `SDL_RenderPresent`), Bilder in `birkin-restluecken/`.

---

## 0. Kurzfassung

1. **Luecke 3 (Rumble) ist geschlossen — und die Luecke hiess falsch.** Die "Rumble/Shake"-
   Kaskaden sind **reine DualShock-Vibration**, kein Kamera-Shake: beide Ringe muenden ueber
   `PadSetAct(port 0, &DAT_800CBC20, 2)` (`jal 0x800960e4` @0x80038c90) in die Aktuator-
   Tabelle; es gibt **keinen** Kamera-, Viewport- oder SPU-Leser (§3). Der Port hatte davon
   nichts — `Sce_shake_on (0x5C)` ist bis heute ein NOP (`scd_vm.c:4512`). Neu:
   `re15_rumble.[ch]` als byte-treuer Zwilling beider Ringe, alle **28 Aufrufe** der vier
   Kaskaden mit `@0x`-Adresse, und der Pad-Hook in `input_pc.c`.
2. **Luecke 2 (Blut) ist geschlossen — und der Vorbefund hatte den falschen Spawner.**
   `0x800154AC` ist **nicht** der Blut-Spawner, sondern der RE2-`atan2` (er ruft `catan`).
   Der echte Spawner ist `FUN_8001BF10` @0x8001bf10 (der ESP-Spawner), viermal gerufen. Seine
   Argument-Packung ist Feld fuer Feld die Signatur von `re15_esp_fx_spawn_ex` — inklusive
   **Effekt-Id 0 = Blut in BEIDEN Engines** (§2).
   Ausserdem: von den "Versaetzen −1400/−700/−2000" ist nur **−1400 lebendig**; −700 und
   −2000 sind tote Stores auf den Richtungsvektor (§2.3).
3. **Luecke 1 (Devour-Opferanimation) ist geschlossen.** Die Kette PL+0x04 = 6 →
   `0x800CE300[0x36]` → `0x80103880` → `0x80103908` ist vollstaendig disassembliert; die
   Opferbank ist EMD-Paar 3, das der Port fuer jede RE2-Bank schon parst
   (`re2_ems.c:149-151`). Gespielt wird **Clip 0 mit Blend-Zaehler 7**. Umgesetzt mit Drehen
   (256/Bild), Schnapp bei Bild 8 und Phasen 0→1→2→3 (§1).
   ⛔ **Ein Teil bleibt offen und ist benannt**: der Zug-Anker PL+0x164/+0x168 (§1.4).
4. **Zwei Korrekturen, die beim Messen abfielen:**
   * `0x800CFC6E` ist die **Spieler**-Gierung, nicht die Kamera (§2.2) — das haette man leicht
     falsch gebaut, ich hatte es zuerst auch.
   * Die Devour-Blob-Schwelle steht im Original auf `slti 10001` @0x80104978, also
     `>= 10001`; der Port stand auf `> 10001` (§3.5).
5. **Runde-17-§5-Punkt-4 (Tentakel-Lokal-Y) ist gemessen und entschieden** (§4): die alte `0`
   hat den Dreh-Zweig **komplett tot gelegt** (0 von 122 Durchlaeufen), mit dem belegten
   −1530 (@0x8003bde0/@0x8003bde4) feuert er 15 mal.
6. **Runde-17-§5-Punkt-5 (kein Messweg fuer Treffer) ist geloest**: der Sichtpruefungs-Haken
   `RE15_G5_TREFFER=<n>` landet Treffer, ohne das Spiel zu veraendern (§5).

---

## 1. Luecke 1 — Devour-Opferanimation auf dem RE2-PL0-Rig

### 1.1 Die Kette, Glied fuer Glied

**Die Spielerbasis ist `0x800CFBF8`.** Beleg: der Devour-Sub haelt in `s0` den Wert
`0x800CFDAC` (`lui s0,0x800d` / `addiu s0,s0,-596` @0x80101b18-1c) und bildet daraus den
Spielerzeiger mit `addiu a0,s0,-436` @0x80101b20 → `0x800CFBF8`; `s0` ist also `PL+0x1B4`,
und `sw s2,0(s0)` @0x80101b40 traegt den Greifer dort ein.

Damit loesen sich alle Globalen der Trefferroutine und des Devour auf:

| Global | = PL + | Beleg |
|---|---|---|
| 0x800CFBFC | +0x04 | `sw v0=6,-1028(at)` @0x80101b50 (das Kommandowort) |
| 0x800CFC30 | +0x38 | `sw v0,-976(at)` @0x80026ccc (X) |
| 0x800CFC34 | +0x3C | `sw v0,-972(at)` @0x80026ce8 (Y) |
| 0x800CFC38 | +0x40 | `sw v0,-968(at)` @0x80026d0c (Z) |
| 0x800CFC6E | +0x76 | `sh v0,-914(at)` @0x80026d18 (**YAW**) |
| 0x800CFD52 | +0x15A | `sh v0,-686(at)` @0x80101bb4 (Ziel-Yaw) |
| 0x800CFD80 | +0x188 | `sw v1,-640(at)` @0x80101b60 (Opfer-MODELL) |
| 0x800CFD84 | +0x18C | `sw v1,-636(at)` @0x80101b78 (Opfer-CLIPTABELLE) |
| 0x800CFDCB | +0x1D3 | `sb v0,-565(at)` @0x80101b70 (Griff-Latch \|= 0x80) |

**Die Griff-Stelle** (Devour-Sub, @0x80101b44-bc) schreibt also das Kommandowort 6 und
kopiert **Boss+0x188/+0x18C nach PL+0x188/+0x18C** — die Opferbank. Dass Paar 3 (EMD
`dir[5]/dir[6]`) genau diese Felder sind, steht byte-true schon im Port:
`re15_port/engine/src/re2_ems.c:149-151` — *"Paar 3 (dir[5]/[6] — Entity+0x18C/+0x188
@0x8001abe0/abf0) -> victim-Feld"*. Der Port parst sie fuer **jede** RE2-Bank, also auch
fuer 0x36 (die Sonde bestaetigt: `victim_ok`, 1 Clip).

Zusaetzlich: `sh v0,-686(at)` @0x80101bb4 setzt **PL+0x15A = Boss-Yaw + 2048**
(`lhu v0,118(s2)` @0x80101ba4, `addiu v0,v0,2048` @0x80101bac). Das Original setzt also
NICHT den Yaw des Spielers, sondern sein ZIEL.

**EXE-Routine 6** ist `FUN_8004006C` @0x8004006c, selbst disassembliert:

```
8004007c: lw v0,0(v1)        / 80040084: ori v0,v0,0x40  / 80040088: sw v0,0(v1)
8004008c: lw v0,0(a0)        / 80040094: and v0,v0,-5    / 80040098: sw v0,0(a0)   ; Wort0 &= ~4
80040090: lw a2,436(a0)      ; a2 = PL+0x1B4 = der GREIFER
8004009c: lbu v0,8(a2)       ; Greifer-Typ
800400a0: lw a1,392(a0)      ; a1 = PL+0x188
800400a4: lw a2,396(a0)      ; a2 = PL+0x18C
800400a8: sll v0,v0,2
800400ac: addu v1,v1,v0
800400b0: lw v0,-6360(v1)    ; v1 = 0x800cfbd8 + Typ*4 -> *(0x800CE300 + Typ*4)
800400b8: jalr v0            ; hook(Spieler, PL+0x188, PL+0x18C)
```
`0x800cfbd8 − 6360 = 0x800CE300` — genau die Tabelle, in die der Ctor `0x80103880` eintraegt
(@0x801005dc). `0x80103880` faechert ueber `lbu v0,5(a0)` @0x80103888 in die Tabelle
@0x801056E8 auf; Eintrag 1 ist `0x80103908`.

### 1.2 Die Maschine `0x80103908`

**PHASE 0** (@0x80103968-a4), faellt danach in Phase 1 durch:
```
80103968: lui v0,0x7          ; \
80103978: sw v0,332(s1)       ; / PL+0x14C = 0x00070000
8010397c: sb a2,6(s1)         ; PL+0x06 = 1
80103988: sw v0,0(v1)         ; *(0x800CFBD8) |= 0x40
80103998: sb a2,448(s1)       ; PL+0x1C0 = 1
8010399c: ori v0,v0,0x80 / 801039a4: sb v0,467(s1)   ; PL+0x1D3 |= 0x80
801039a0: jal 0x8005ba28      ; SE, a0 = 0x04010001 (@0x80103948 lui 0x401 / @0x80103984 ori 1)
```
`PL+0x14C = 0x00070000` heisst byteweise: **Clip 0, Bild 0, +0x14E = 7**. Dass +0x14E der
**Blend-Zaehler** ist, steht in `FUN_80029614` (Decompilat):
`uVar10 = *(byte *)(iVar15 + 0x14e)` und `gte_ldIR0(0x1000 - (param_4 & 0xffff) * uVar10)` —
mit dem Blend-Argument 512 sind das **8 Bilder Ueberblendung** (7,6,…,0).

`0x8005BA28` ist der **SE-Spieler**, nicht der Anim-Setzer: das Decompilat greift auf die
Bank-Tabellen `DAT_800D4C48` / `DAT_800DBB78` / `DAT_800D75A0` zu, belegt einen Kanal bei
`DAT_800D4F18 + ch*0x20` und legt `param_2` (hier `&PL.x`) als 3D-Position fuers Panning ab.

**PHASE 1** (@0x801039a8-2c):
```
801039b0: lh v1,356(s1)  / 801039b4: lw t0,56(s1)
801039c4: subu v1,v1,t0  / 801039c8: sra v1,v1,1  / 801039cc: addu t0,t0,v1
801039dc: sw t0,56(s1)        ; PL.x += (PL+0x164 - PL.x) >> 1
801039bc: lh v0,360(s1)  / 801039c0: lw a3,64(s1)  ... 801039e4: sw a3,64(s1)   ; dito fuer Z
801039b8: lh a1,346(s1)       ; a1 = PL+0x15A
801039ac: addiu a2,zero,256   ; Schrittweite 256
801039e0: jal 0x8001569c      ; Drehen auf das Ziel
801039f8: addiu a3,zero,512
801039f4: jal 0x8002959c      ; Anim vorruecken
801039fc: lbu v1,333(s1) / 80103a00: addiu v0,zero,8 / 80103a04: bne v1,v0,...
80103a1c: sb v0=2,6(s1)       ; PL+0x06 = 2
80103a24: sh v1,118(s1)       ; PL+0x76 = PL+0x15A  (Schnapp)
80103a20: jal 0x8005ba28      ; SE erneut
```

**PHASE 2** (@0x80103a30-7c): Spieler-Gierung temporaer +2048 (@0x80103a48) um
`jal 0x80015cb8` (@0x80103a4c), danach −2048 (@0x80103a68); `jal 0x8002959c` mit a3 = 512
(@0x80103a6c); Clip-Ende → `sb v0=3,6(s1)` @0x80103a7c.

`FUN_8001569C` (Decompilat) ist das Dreh-Helferlein auf `+0x76` mit fester Schrittweite —
byte-true im Port als `g5_yaw_toward`.

### 1.3 Umsetzung

`re15_port/engine/src/enemy_ai_boss_g5.c`, neuer Abschnitt "DEVOUR-OPFERMASCHINE":
`re15_g5_devour_opfer_start` (die Griff-Stelle), `g5_devour_opfer_tick` (die drei Phasen),
`g5_devour_clip_len` / `g5_devour_advance` (Clip 0 der 0x36-`anim_victim`), `g5_yaw_toward`
(`FUN_8001569C`). Angebunden im Devour-Sub (ph 0) und im Tick-Schwanz. Der alte
Sofort-Schnapp `pl->rot_y = Boss-Yaw+2048` bleibt nur als Rueckfall, wenn die Bank fehlt.

### 1.4 ⛔ Was hier offen BLEIBT — mit Adresse

**Der Zug-Anker PL+0x164/+0x168.** Gesetzt wird er von `FUN_80015B94` @0x80015b94 (gerufen
@0x80101b3c) als **Greifer-Position minus der um den Greifer-Yaw gedrehten
Wurzel-Translation des Opfer-Clips**:
```
80015bd0: jal 0x80015db0     ; Wurzel-Translation des Clips nach sp+48
80015c24: lh a0,118(s0)      ; Greifer-Yaw
80015c28: jal 0x8008e8b4     ; RotMatrix
80015c38: jal 0x8008dba4     ; ApplyMatrix
80015c40: lw v0,56(s0) / 80015c44: lhu v1,48(sp) / 80015c4c: subu v0,v0,v1
80015c50: sh v0,356(s0)      ; +0x164 = Greifer.x - offset.x
80015c64: sh v0,358(s0)      ; +0x166 = Greifer.y - offset.y
80015c78: sh v0,360(s0)      ; +0x168 = Greifer.z - offset.z
80015c7c: sh a0,356(s3) / 80015c88: sh v0,358(s3) / 80015c94: sh v0,360(s3)   ; -> PL
```
Die Wurzel-Translation eines Opfer-Clips entsteht im Port erst im Zeichner und ist aus dem
KI-Code nicht erreichbar. Der Zugschritt (`sra v1,v1,1` @0x801039c8) ist deshalb **nicht**
umgesetzt — benannt statt geraten. Alles andere der Maschine ist oben belegt und laeuft.

---

## 2. Luecke 2 — Blut-Spawn der Trefferroutine

### 2.1 ⛔ Der Vorbefund hatte den falschen Spawner

Runde 17 §5.2 nannte `jal 0x800154ac` @0x80102888 als Blut-Spawner. **Das ist der
RE2-`atan2`.** Beleg, selbst gelesen:
```
800154c8: subu s0,a2,a0      ; dx
800154dc: subu a0,a3,a1      ; dz
800154f0: sll a0,a0,12       ; dz << 12
800154f4: div a0,s0
80015520: jal 0x8008d190     ; = catan (BIOS)
80015534: addiu v0,zero,4096 / 80015538: addiu v0,zero,2048   ; Quadrant
80015540: andi v0,v0,0xfff
```
und `RE2_Quellcode_V2/FUN_800154ac.c` ruft woertlich `catan`. Er liefert nur den **Winkel**,
den die Spawns 3 und 4 als Dreh-Parameter bekommen.

**Der echte Spawner ist `FUN_8001BF10` @0x8001bf10** (der ESP-Effekt-Spawner) und er wird
**viermal** gerufen: **@0x801027ec, @0x80102848, @0x801028f0, @0x80102928**.

Seine Argument-Packung (Decompilat `FUN_8001bf10.c`):

| Bits | Ausdruck im Decompilat | Bedeutung | Port-Parameter |
|---|---|---|---|
| 24..31 | `param_1 >> 0x18` | Effekt-Id | `effect_id` |
| 16..23 | `param_1 >> 0x10 & 0xff` | Sub-Index | `sub_index` |
| 0..15 | `param_1 << 0x10` | Skala | `scale16` |
| — | `param_2` → Slot+0x0A | Dreh-Parameter | `param` |
| — | `param_4` → Slot+0x14/+0x18 | SVECTOR-Position | `x,y,z` |

Das ist Feld fuer Feld `re15_esp_fx_spawn_ex(bank, id, sub, scale16, x,y,z, param)`. Der Port
dekodiert **dieselbe Packung** schon fuer ROOM1090 (`a0 = 0x09031800` = Id 0x09, sub 3,
scale 0x1800, `re15_esp.h`). Und **Effekt-Id 0 ist in beiden Engines das Blut** (RE1.5:
CORE00.ESP Idx 0 id 0x00, `re15_esp.h`) — das ist keine geratene Zuordnung, sondern dieselbe
Zahl. In EM036 sind die Argumente:

| Aufruf | Effekt-Id | Sub | scale16 | Dreh-Parameter |
|---|---|---|---|---|
| @0x801027ec | 0 | `rng & 1` (@0x801027c0) | `8096 + (rng&0xff)*8` (@0x801027cc-d0) | 0 |
| @0x80102848 | 0 | `rng & 1` (@0x80102828) | `8096 + (rng&0xff)*4` (@0x80102834-38) | 0 |
| @0x801028f0 | 0 | 0 | `8096 + (rng&0xff)*8` (@0x801028cc-d0) | catan2 Boss→Spieler |
| @0x80102928 | 0 | 1 (`lui a0,0x1` @0x80102918) | `8096 + (rng&0xff)*8` (@0x80102910-14) | dito |

### 2.2 ⛔ `0x800CFC6E` ist die SPIELER-Gierung, nicht die Kamera

Die Wache @0x80102640 haengt an `lhu v0,-914(v0)` = `0x800CFC6E` (@0x80102604). Ich hatte das
zuerst als Kamera-Gierung gebaut — **falsch**. Mit der Spielerbasis `0x800CFBF8` aus §1.1 ist
`0x800CFC6E` = `PL+0x76`, und der Spieler-Zustandslader schreibt in EINEM Block
`0x800CFC30/34/38/6E` aus `a1+0/+2/+4/+6` (@0x80026ccc / @0x80026ce8 / @0x80026d0c /
@0x80026d18) — also X/Y/Z/Yaw desselben Entities. Dieselben Globalen 0x800CFC30/0x800CFC38
benutzt die Trefferroutine als Spieler-X/Z (@0x80102670 / @0x80102748 / @0x8010287c /
@0x80102884), was nur mit dem Spieler aufgeht.

Der Gang durch die Wache:
```
80102604: lhu v0,0x800CFC6E   ; Spieler-Yaw
sp+16 = {0, Yaw, 0}
80102614: jal 0x8008e1f4      ; RotMatrix -> Matrix sp+32
80102628: addiu v0,zero,4096 / 80102634: sh zero,18(sp)    ; Eingangsvektor (4096,0,0)
80102630: jal 0x8008dba4      ; ApplyMatrix -> sp+16
80102638: lh a1,16(sp)        ; x' = 4096*cos(Yaw)
80102640: slti v0,a1,-64
80102644: beq v0,zero,0x80102870          ; x' >= -64 -> KEIN Blut
```
Zwei weitere Wachen davor/danach: `lbu v0,6(s3)` + `bne v0,zero,0x80102a94`
(@0x801025e8-f0, nur Boss-Phase 0) und `+0x05 == 16` (@0x80102658) bzw. `+0x05` in 9..12
(@0x80102660-64).

### 2.3 ⛔ Von den drei "Versaetzen" ist nur einer lebendig

* **−1400** @0x801026cc-d0: `sh v1,26(sp)` — das **Y der Spawns 1+2**. LEBENDIG.
* **−700** @0x8010270c und **−2000** @0x80102740: beide schreiben `18(sp)`, also die
  Y-Komponente des **Richtungsvektors** ab sp+16, der nach dem ApplyMatrix nie wieder gelesen
  wird. **TOTE Stores.**
  (Dieselbe Sorte Rest: `lhu v0,16(sp)` / `sh v0,16(sp)` @0x801028f8/@0x80102908 vor Spawn 4
  inkrementiert die falsche Stelle — die Position ist `s1 = sp+24` @0x801028ec.)
  Der Port bildet die lebenden Werte ab und laesst die toten weg.

### 2.4 Die Geometrie

Spawns 1+2 (Position `sp+24/26/28`):
* X = `Boss.x + 3183` (@0x801026d4-d8), zweiter Spawn `+500` (@0x801027fc)
* Y = `−1400`
* Z = `(z' * (Boss.x − (Spieler.x − 3183))) / x'  +  Spieler.z`
  (@0x8010267c / @0x80102680 / @0x80102684 / @0x80102694 / @0x80102750), geklemmt auf
  **[−26000, −20500]** (@0x80102760-6c / @0x80102778-84); zweiter Spawn
  `Z −= ((rng&0xff) − 128)` (@0x80102810-14)

Spawns 3+4 sitzen auf **Kollisionssegment 1**: `lw v0,164(s3)` / `168` / `172`
(@0x801028a0 / @0x801028b0 / @0x801028bc), X `+500` (@0x801028a8). Dass +0xA4/+0xA8/+0xAC die
**Weltlage von Segment 1** sind, belegt `FUN_80035408` (Decompilat):
`puVar3 = param_1 + 0x21` = Byte 0x84, Schrittweite 8 Woerter = 32 Byte, und
`*puVar3 = param_2[0] + vx` (0x84), `puVar1[-1] = param_2[1] + vy` (0x88),
`*puVar1 = param_2[2] + vz` (0x8C) → Segment 1 = 0x84+32 / 0x88+32 / 0x8C+32 = **164/168/172**.
Das ist genau, was `re15_g5_body_segment(1, …)` im Port liefert (Runde 17).

Die Korridor-Wache der Spawns 3+4: `addiu v0,v0,23999` @0x80102858 +
`sltiu v0,v0,0x5db` @0x80102860 (1499) auf dem **geklemmten** Z → Z ∈ [−23999, −22501];
`bne s4,v0,0x80102948` @0x80102898 springt sonst darueber hinweg.

### 2.5 Umsetzung + Messung

`enemy_ai_boss_g5.c`: `g5_catan2` (= `FUN_800154AC`, ueber `re15_catan`), `g5_spieler_yaw`,
`g5_treffer_blut` mit allen vier Spawns; gerufen im Treffer-Zweig.

**Sonde, beide Seiten der Wache** (`probe_r18_birkin_rest` Teil B):

| Spieler-Yaw | x' (`dirx`) | Wache | neue ESP-Partikel |
|---|---|---|---|
| 1088 | **−401** | OFFEN | **2** |
| 0 | **+4096** | ZU | **0** (GEGENPROBE) |

**Im echten Spiel** (`RE15_G5_TREFFER=4`, `RE15_BIRKIN_DBG=1`, Protokoll
`birkin-restluecken/birkin_dbg_r18.log`), letzte Zeile des Laufs:
```
g5 tick=1860 sub=0 ph=1 clip=0 af=37 u=11030 dist=4802 hp=600 blob=3/2 gw0=3396 akku=7
   pos=(11030,-23400) blut=465/2/2/8 esp=17 plyaw=2045 dirx=-4096 letzte=(10207,-1400,-22695)
```
`blut=465/2/2/8` heisst: der Treffer-Zweig wurde 465 mal betreten, die Blut-Routine lief
**2 mal** (die Boss-Phase-0-Wache @0x801025e8-f0 laesst nur diese durch), die Blick-Wache war
beide Male offen (`dirx=-4096` bei `plyaw=2045`), und es wurden **8 Spawns = 2 x 4**
abgesetzt — also jedes Mal auch die beiden Korridor-Spawns 3+4. Die zuletzt gesetzte Position
`(10207, -1400, -22695)` ist `Boss.x + 3183` mit Y = **-1400** und einem Z im geklemmten
Band — genau die Formel aus §2.4.

---

## 3. Luecke 3 — die Rumble-Kaskaden

### 3.1 ⛔ Es ist Vibration, kein Shake

Beide Ringe muenden im Pad-Handler `FUN_80038BBC`:
```
80038da0: addiu a0,a0,-21800 ; Ring A = 0x800EAAD8
80038da4: jal 0x800396fc
80038db0: addiu a0,a0,-21464 ; Ring B = 0x800EAC28
80038db4: jal 0x800396fc
80038db8: sb v0,0(s0)        ; DAT_800CBC20 = max(Ring A)
80038dc0: sb v0,1(s0)        ; DAT_800CBC21 = max(Ring B)
80038c8c: addiu a1,a1,-0x43e0 ; = &DAT_800CBC20
80038c90: jal 0x800960e4     ; PadSetAct(port 0, Tabelle, ...)
80038c94: addiu a2,zero,2    ; ... len 2
```
`PadSetActAlign` @0x80038cc8 mit der Tabelle @0x8009DBA4 = `00 01 ff ff ff ff` (Aktuator 0 →
Byte 0, Aktuator 1 → Byte 1). **Alle Xrefs auf `DAT_800CBC20` sind Schreibzugriffe** plus das
eine `addiu` fuer `PadSetAct`; es gibt keinen Kamera-, Viewport- oder SPU-Leser. Ohne
Analogmodus wird pauschal voll gefahren (`addiu v0,zero,64` @0x80038e60 /
`addiu v0,zero,255` @0x80038e68).

### 3.2 Ring-Layout

Stride **10 Byte** (`addiu a0,a0,10` @0x800397d8 / @0x8003980c / @0x800397a4), Kapazitaet
**0x20 = 32** (`sltiu` @0x800397d0 / @0x80039804 / @0x8003979c).

| Offset | Feld | Beleg (a1 = slot+1) |
|---|---|---|
| +0 | belegt | `lbu v0,0(a0)` @0x8003970c |
| +1 | Ausgabewert | `sb v1,0(a1)` @0x80039778 |
| +2 | Vorlauf-Delay (u16) | `lhu v0,1(a1)` @0x8003971c |
| +4 | Restdauer (u16) | `lhu v0,3(a1)` @0x80039734 |
| +6 | Schritt (s16, 9.7) | `lh v0,5(a1)` @0x80039754 |
| +8 | Akkumulator (u16) | `lhu v0,7(a1)` @0x80039764 |

`FUN_800396FC` @0x800396fc je Slot: leer → weiter; Delay ≠ 0 → dekrementieren und
`j 0x80039794` @0x8003972c (**geht NICHT ins Maximum**); Dauer−−, bei 0 `sb zero,0(a0)`
@0x80039750 (der Slot liefert in DIESEM Bild aber noch seinen Wert); Schritt ≠ 0 →
`Akku += Schritt` (32-bit, @0x8003976c), `Wert = Akku >> 7` (`srl` @0x80039770); Maximum
(`sltu v0,a3,v1` @0x80039784).

Die drei Alloc-Funktionen:
* `FUN_8003947C` @0x8003947c — kleiner Motor: `Wert = (dauer != 0)` = **immer 1**
  (`sb v0,1(v1)` @0x800394cc), Schritt 0 (@0x800394e4) → AN/AUS.
* `FUN_80039514` @0x80039514 — grosser Motor, konstante Amplitude, Schritt 0.
* `FUN_800395B8` @0x800395b8 — grosser Motor, RAMPE: `Akku = von << 7`,
  `Schritt = ((bis − von) * 0x80) / dauer`.
Der letzte Parameter ist in allen drei Faellen dasselbe Feld +2 = **Startverzoegerung**.

### 3.3 Die vier Kaskaden (28 Aufrufe, alle umgesetzt)

**Kaskade 1** — Todes-Uebergang `+0x06: 0 → 1` (`sb v1,6(s3)` @0x80103074):
`large(250,180,0)` @0x80103104, `ramp(150,180,0,250)` @0x80103118, dann acht 10-Bild-Pulse:
`ramp(10,180,220,20)` @0x8010312c, `(10,220,180,30)` @0x80103140, `(10,180,220,40)`
@0x80103154, `(10,220,180,50)` @0x80103168, `(10,180,220,65)` @0x8010317c,
`(10,220,180,75)` @0x80103190, `(10,180,220,230)` @0x801031a4, `(10,220,180,240)`
@0x801031b8.

**Kaskade 2** — Todes-Uebergang `+0x06: 2 → 3` (`sb v1,6(s3)` @0x80103470):
`large(5,200,0)` @0x801034c4, `ramp(200,200,0,5)` @0x801034d8, dann zwoelf Stakkato-Stoesse
im kleinen Motor: `small(3,0)` @0x801034e4, `(3,5)` @0x801034f0, `(2,15)` @0x801034fc,
`(3,60)` @0x80103508, `(3,65)` @0x80103514, `(2,75)` @0x80103520, `(3,90)` @0x8010352c,
`(3,95)` @0x80103538, `(2,105)` @0x80103544, `(2,110)` @0x80103550, `(2,115)` @0x8010355c,
`(1,120)` @0x80103568.

**Kaskade 3** — Gliedmassen-Tod, jedes 4. Bild des Todes-Clips: `small(3,0)` @0x80103614,
`large(15,250,0)` @0x80103624.
Das Gate dazu (in dieser Runde neu portiert, der Port hatte den ganzen Zweig nicht):
```
80103570: lbu v0,333(s3)   ; +0x14D = Bild des Clips
80103578: andi v0,v0,0x3
8010357c: bne v0,zero,0x8010380c       ; nur jedes 4. Bild
80103580: addiu s1,zero,4              ; vier Arme
80103590: lbu a0,552(s3)               ; +0x228 = die Arm-Maske
80103598: srlv v0,a0,v1 / 8010359c: andi v0,v0,0x1 / 801035a0: bne v0,zero,...   ; Bit i
801035a8: srlv v0,a0,v0 / 801035b0: bne v0,zero,...                              ; Bit i+4
801035dc: jal 0x80015fe8 / 801035e8: divu v0,s0 / 801035f8: mfhi v1              ; rng % n
80103608: jal 0x80104e9c / 8010360c: addiu a1,zero,3585                          ; 0xE01 = TOD
```
Das ist exakt die Kandidatenwahl, die `g5_tentakel_frei` im Port schon prueft.

**Kaskade 4** — Devour-Blob:
* Sub-Zustand 0 (`beq v1,zero,0x801048b0` @0x80104884): `small(30,0)` @0x801048c4,
  `large(35,250,15)` @0x801048d4.
* Uebergang Sub 2 → 3: `small((rng&3)+2, rng&3)` @0x80104ad8,
  `large((rng&3)+3, 250, rng&3)` @0x80104afc — **einmalig**, nicht je Bild: der Block dahinter
  wird nur erreicht, wenn `slti v0,v0,10001` @0x80104978 NICHT greift (`bne v0,zero,0x80104b70`
  @0x8010497c).

### 3.4 Umsetzung

Neu: `re15_port/include/re15_rumble.h` + `re15_port/engine/src/re15_rumble.c` (byte-treuer
Zwilling, die 32-Bit-Arithmetik des Ticks inklusive). Aufrufe in `enemy_ai_boss_g5.c`
(`g5_tod_tick` ph0 und ph2→3, `case 3` Gliedmassen-Tod, `g5_blob_tick` case 4).
Pad-Hook: `re15_port/platform/pc/src/input_pc.c`, `pad_rumble_tick()` am Kopf von
`re15_input_tick` — dieselbe Stelle wie im Original (`FUN_80038BBC`), Aktuator 0 → SDL
`high_frequency`, Aktuator 1 → `low_frequency`.

### 3.5 Nebenbefund, mit Adresse korrigiert

Die Devour-Blob-Schwelle steht im Original auf `slti v0,v0,10001` @0x80104978 — der Uebergang
faellt also bei **`>= 10001`**. Der Port stand auf `> 10001` und war damit bei genau 10001 ein
Bild zu spaet. Ausserdem setzt das Original dort `+0x21B = (rng & 0x3f) + 64`
(`andi v0,v0,0x3f` @0x8010498c, `addiu v0,v0,64` @0x80103990-94) — die Blut-Serie des
Verschlingens, deutlich laenger als die 10 des Biss-Treffers (@0x801041a8). Beides umgesetzt.

### 3.6 Messung (Sonde Teil A, 16 Pins)

| Pin | Erwartung | gemessen |
|---|---|---|
| `small(3,0)` | Aktuator0 = 1,1,1,0 | ✅ |
| `small(1,120)` | zieht erst auf Bild **121** an | 121 |
| `large(5,200,0)` | 200 fuer genau 5 Bilder | ✅ |
| `ramp(150,180,0,250)` Bild 250 | 0 (noch verzoegert) | 0 |
| `ramp(...)` Bild 251 | `(23040−153)>>7` = **178** | 178 |
| `ramp(...)` Bild 400 | 0 (ausgelaufen) | 0 |
| Kaskade 1 Slots | 10 in Ring B, 0 in Ring A | 10 / 0 |
| Kaskade 1 Bild 1 / 21 / 405 | 180 / **184** / 0 | 180 / 184 / 0 |
| Kaskade 2 Slots | 12 in Ring A | 12 |
| Kaskade 2 kleiner Motor | genau **29** aktive Bilder | 29 |
| Kaskade 2 Bild 1 | klein 1 UND gross 200 | ✅ |
| Kaskade 2 Bild 121 | der letzte Stoss sitzt | ✅ |
| **GEGENPROBE** | frische Ringe: 60 Bilder lang 0/0 | ✅ |

---

## 4. Runde 17 §5 Punkt 4 — Tentakel-Lokal-Y: gemessen und entschieden

`enemy_ai_tentakel_g5.c` setzte im Dreh-Zweig das Lokal-Y des Spieler-Segments auf 0, mit der
Begruendung *"fuer das einzige Spieler-Segment nie gesetzt"*. Das ist **falsch**:
```
8003bde0: addiu v0,zero,-1530
8003bde4: sh v0,152(s2)        ; 152 = 0x98 = seg0-Basis 0x84 + 0x14 = LOKAL-Y
```
Dass 0x98 das Lokal-Y ist, belegt `FUN_80035408`: `local_58.vy = (short)puVar1[3]` mit
`puVar1 = param_1 + 0x23` = Byte 0x8C, also 0x8C + 12 = 0x98.

**Die Messung** (Sonde Teil D, 900 Kampfbilder):

| | Durchlaeufe durch die Hoehenpruefung | Dreh-Zweig feuert |
|---|---|---|
| ALT (`+ 0`) | 122 | **0** |
| NEU (`+ RE15_PL_SEG_LOKAL_Y` = −1530) | 122 | **15** |

Die alte 0 hat den Zweig also nicht nur verschoben, sondern **vollstaendig tot gelegt**.
Entscheidung: belegt geaendert (@0x8003bde0/@0x8003bde4), die Differenz laeuft als
Messschiene `re15_g5_tent_dreh_mess()` mit.

*(Die zweite 0 in derselben Funktion — `hs = 0 + 1530` — ist ein ANDERES Feld: die
Halbhoehe des TENTAKEL-Segments, nicht das Lokal-Y des Spielers. Nicht angefasst.)*

---

## 5. Runde 17 §5 Punkt 5 — der fehlende Messweg fuer Treffer

Runde 17 hielt fest, dass drei Pistolen-Laeufe **0 Treffer** brachten, weil der gescriptete
Spieler vom Devour gefressen wird, bevor er 600 HP herunterschiessen kann — die
Treffer-Reaktion war damit am Bild nicht pruefbar.

Geloest mit dem Sichtpruefungs-Haken **`RE15_G5_TREFFER=<n>`** (`enemy_ai_boss_g5.c`, in
derselben Bauart wie das schon vorhandene `RE15_G5_OPFER`): ab Kampfstart wird alle *n* Bilder
genau ein `hit_react`-Bit gesetzt. Ohne die Variable aendert sich nichts. Damit lief die
Messung in §2.5, und die Trefferroutine ist im laufenden Spiel belegbar.

---

## 6. Sichtpruefung (echte exe, selbst angesehen)

Lauf: `RE15_TITLE_SHOT=… RE15_NO_INTRO=1 RE15_DEBUG_JUMP=5090@30
RE15_PLAYER_POS="13600,-23350,2048" RE15_FORCE_EVENT=4@120 RE15_G5_TREFFER=…
RE15_BIRKIN_DBG=1 RE15_FRAMEDUMP=…` in `re15_port/build/platform/pc`.
`RE15_FRAMEDUMP` ist der Vollbild-Readback **unmittelbar vor `SDL_RenderPresent`** — kein
AUTOSHOT, kein Softwarerenderer.

| Bild | Urteil |
|---|---|
| `blut000300.png` | Die Masse ist am Westende, Leon steht im Korridor davor. Kampf laeuft normal an. |
| `blut000460.png` | Die Masse kriecht heran, der Kriecherkopf sitzt oben auf. Die vier neuen Eingriffe brechen nichts. |
| `blut000560.png` | Dichter heran, Tentakel im Bild, Abstand haelt (Runde-17-Soliditaet unveraendert). |

⛔ **Was auf diesen Bildern NICHT zu sehen ist: die Blutpartikel selbst.** Das ist kein
Defekt, sondern die Geometrie des Originals, und sie ist gemessen: das Protokoll
`birkin_dbg_r18.log` weist die acht Spawns aus, den ersten bei `letzte=(-1811,-1400,-23421)`.
Zu diesem Zeitpunkt stand der Boss bei `pos=(-5198,-23400)` und Leon bei x = 13600 — die
Partikel sassen also rund 15000 Einheiten entfernt am anderen Ende des Korridors, weit
ausserhalb des Kamera-Ausschnitts. Der Grund ist die belegte Formel `X = Boss.x + 3183`
(@0x801026d4-d8) zusammen mit der Wache `+0x06 == 0` (@0x801025e8-f0), die den Treffer in
diesem Lauf nur zweimal durchliess. Der Beweis, dass die Spawns sitzen und richtig gegated
sind, steht in der Sonde (§2.5, beide Seiten der Wache) und im Protokoll — nicht im Bild.

---

## 7. Abnahme

* `bash re15_port/tools/local_build.sh all` → **329/329** (vorher 328; +1 neuer Test,
  Test #309 `unit_r18_birkin_restluecken_pin`).
* Sonde `probe_r18_birkin_rest pin` → **alle Pins halten, 0 Fehler** (16 Rumble, 4 Blut,
  7 Opfermaschine, 1 Tentakel), jeweils mit Gegenprobe.

## 8. Offen (ehrlich)

1. **Der Zug-Anker der Opfermaschine**, PL+0x164/+0x168 aus `FUN_80015B94` @0x80015b94 —
   §1.4. Braucht die Wurzel-Translation des Opfer-Clips (`FUN_80015DB0` @0x80015db0), die im
   Port erst im Zeichner entsteht.
2. **Die RE2-Sub-Zustaende 9..12 und 16** der Trefferroutine (Wachen @0x80102658 /
   @0x80102660-64) haben im Port kein Gegenstueck — seine Sub-Menge ist {0..4, 0xF}. Gesperrt
   ist deshalb nur der Flinch-Pseudo-Sub 0xF.
3. **Der Z-Sonderfall in Sub 1** (`lw v0,0x800CFD90` → `lw v0,1992(v0)` → Z, @0x80102798-ac):
   0x800CFD90 = PL+0x198 = der Modell-Instanzzeiger, `+1992` zeigt in dessen Posen-Block.
   Nicht portiert.
4. **Der ESP-Spawn des Devour-Blobs** (`jal 0x8001bf10` @0x80104ab8, a0 =
   `0x10000 | (8096 + (rng&0xff)*4)`, a1 = Boss-Yaw) — dieselbe Mechanik wie §2, aber eine
   eigene Positionsrechnung (@0x801049a0-0x80104ab4). Nicht in dieser Runde gebaut.
5. **Kein Parity-Vergleich gegen einen DuckStation-Savestate** dieses Kampfes: alle Zahlen
   sind gegen die disassemblierten RE2-Bytes und die eigene Sonde geprueft, nicht gegen RAM
   einer laufenden Konsole.
6. **`Sce_shake_on (0x5C)`** bleibt ein NOP (`scd_vm.c:4512`) — das ist der RE1.5-SCD-Opcode
   und ein eigenes Thema; die RE2-Kaskaden dieses Bosses laufen jetzt ueber `re15_rumble`.

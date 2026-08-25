# FINDING G — Effekt-Sprites (ESP) werden ohne Sichtbarkeits-Cull und mit falschem OT-Schluessel gezeichnet

Status: **GELOEST** (Ursache belegt, Fix-Rezept mit @0x-Adressen).
Zwei unabhaengige Defekte, beide im PC-Zeichenpfad `pc_draw_effects`
(`re15_port/platform/pc/main.c:204`).

---

## 1. Symptom (Nutzer-Wortlaut)

> "Alle Effekte wie Strom, Feuer etc. ueberdecken nicht sichtbare Bereiche. Zum Beispiel wenn sie
> noch um die Ecke hinter der Kamera sind."

Also: Effekt-Sprites liegen als Overlay auf dem Hintergrund, obwohl ihr Ursprung fuer die aktive
Kamera-Einstellung gar nicht sichtbar ist (anderer Raumteil, hinter einer Wand, um die Ecke).

---

## 2. Was der PORT heute tut

Alle Effekt-Partikel laufen durch **eine** Funktion:
`re15_port/platform/pc/main.c:204`
`static void pc_draw_effects(const re15_camera_view_t *cam, int cx, int cy)`
(aufgerufen genau einmal, `main.c:7483`). Die vollstaendige Verwerf-Logik dieser Funktion ist:

| main.c:Zeile | Test | deckt ab |
|---|---|---|
| 210 | `if (!f) continue;` | Slot leer |
| 212 | `if (!re15_esp_fx_visible(f)) continue;` | flags Bit1 (frozen/hidden) |
| 233 | `if (!re15_render_pc_dbg_slot_loaded(slot)) continue;` | Textur-Slot nicht geladen |
| **257** | `if (vz < 64) continue;` | hinter/zu nah an der Kamera |
| 347 | `if (x1 <= x0 ...) continue;` | degenerierte Quad-Groesse |

Danach wird pro Sprite ein Quad in die Tri-Queue gehaengt (`main.c:349/351`):

```c
/* main.c:319 */
int z = (int)vz >> 4;
...
re15_render_textured_tri(x0, y0, u0, v0,  x1, y0, u1, v0,
                         x0, y1, u0, v1,  0, 0, z, 128, 128, 128);
```

**Es gibt KEINEN Sichtbarkeits-/Region-Test.** Der Port hat den Test (`re15_aot_point_in_quad`,
`re15_port/engine/src/aot_common.c:303`, dort ausdruecklich als "PSX-canonical point-in-quad test
(FUN_80014368)" dokumentiert) und wendet ihn auf **Spieler** (`main.c:5582`), **NPCs**
(`main.c:6530`) und **Props** (`main.c:7205`) an — aber nicht auf Effekte.

Und der Tiefen-Schluessel liegt auf einer **anderen Skala** als alles andere in derselben Queue:

* Mesh-Dreiecke: `avgz` = **rohes View-Z** (`main.c:1382` `int avgz = (int)((wz0+wz1+wz2)*(1/3));`,
  `wz` = `_vz` aus `PROJECT_VERT`, `main.c:5808` `(out_wz) = (float)_vz;`)
* sprite.pri-Masken: `re15_pri_mask_camera_z(depth) = depth * 64`
  (`re15_port/include/re15_pri.h:106-108`)
* **Effekte: `vz >> 4`** (`main.c:319`) — **Faktor 16 zu klein**

Sortiert wird beides gemeinsam, absteigend (fern zuerst) in `re15_render_end_frame`
(`re15_port/platform/pc/src/render_pc.c:744-834`).

---

## 3. Was das ORIGINAL tut

### 3a. Der Zeichen-Pfad

`FUN_80052fd0` = Effekt-Render-Dispatcher, ruft `FUN_80053240` (Slot-Schleife), die
`FUN_800534c4` (Sprite-Bau, POLY_FT4, 40 Byte/Prim) aufruft.

```
80052ffc: jal 0x8004f090            ; Setup
80053004: jal 0x80053240            ; <-- Effekt-Slot-Schleife
8005300c: jal 0x8004f0b0            ; Teardown
```

Slot-Stride = 132 (0x84), Pool = 12672/132 = **96 Slots**
(`addiu s0,s0,-132` @0x80053480 / @0x80053304).

### 3b. Gate 1 — flags Bit0 + Bit1 (aktiv / sichtbar)

```
800532f4: lbu  v1,0(s0)             ; s0 = slot+0x6C = flags
800532fc: andi v0,v1,0x1
80053300: beq  v0,zero,0x80053474   ; nicht aktiv -> naechster Slot
80053308: andi v0,v1,0x2
8005330c: beq  v0,zero,0x80053474   ; nicht sichtbar -> naechster Slot
```

Der Port hat das (`re15_esp_fx_visible`, main.c:212). **OK.**

### 3c. Gate 2 — DIE REGION-SICHTBARKEIT (das gesuchte "um die Ecke"-Gate)

```
80053310: addiu a0,sp,24            ; a0 = &{x,y,z} auf dem Stack
80053314: lh    v0,-68(s0)          ; slot+0x28  = world X
80053318: lui   a1,0x800b
8005331c: lw    a1,-0x3870(a1)      ; a1 = DAT_800ac790  (Region-Quad des AKTIVEN Cuts)
80053320: sw    v0,24(sp)
80053324: lh    v0,-66(s0)          ; slot+0x2A  = world Y
8005332c: sw    v0,28(sp)
80053330: lh    v0,-64(s0)          ; slot+0x2C  = world Z
80053334: jal   0x80014368          ; <-- Punkt-im-Viereck
80053338: sw    v0,32(sp)
8005333c: beq   v0,zero,0x80053474  ; <-- AUSSERHALB -> Slot KOMPLETT UEBERSPRINGEN
```

Roh-Bytes (Gegenprobe, Decompile-Misstrauen):

```
80053314: bc ff 02 86 | 0b 80 05 3c | 90 c7 a5 8c | 18 00 a2 af
80053324: be ff 02 86 | 00 00 00 00 | 1c 00 a2 af | c0 ff 02 86
80053334: da 50 00 0c | 20 00 a2 af | 4d 00 40 10 | 00 00 00 00
           ^^ jal 0x80014368        ^^ beq v0,zero,+0x134 -> 0x80053474
```

Die getestete Position `slot+0x28` ist **exakt** die Position, die `FUN_800534c4` gleich darauf
per RTPS projiziert (`800534ec: addiu v0,a1,40`, dann `lwc2 $0,0(t4)` / `lwc2 $1,4(t4)`
@0x80053514/18). Getestet wird also der Sprite-Ursprung selbst.

**`FUN_80014368` = Punkt-im-Viereck (2D, X/Z)** — verifiziert per Disasm und gegen
`RE_15_Quellcode_V2/FUN_80014368.c`:

```
80014368: lh t5,4(a1)     ; quad p0.x
8001436c: lh t0,6(a1)     ; quad p0.z
80014370: lw v0,8(a0)     ; punkt.z   (param_1[2])
80014384: lw v0,0(a0)     ; punkt.x   (param_1[0])
...  vier Kreuzprodukt-Vorzeichentests gegen p0..p3 @+4/+6, +8/+0xA, +0xC/+0xE, +0x10/+0x12 ...
80014430: addu v0,zero,zero ; return 0 (aussen)
8001442c: ori  v0,zero,0x1 ; return 1 (innen)
```

**`DAT_800ac790` = das Region-Quad des aktiven Cuts.** Gesetzt beim Cut-Wechsel:

```
80021be0: lbu   v1,0x800afbb5       ; neuer Cut
80021bfc: sh    v1,0x800b0fe4       ; aktiver Cut
80021c00: jal   0x80014324
80021c0c: sw    v0,-0x3870(at)      ; -> DAT_800ac790
```

`FUN_80014324` sucht den **ersten** RVD-Satz mit Byte[+2] == Cut-Id:

```
80014328: lw    v0,0x800ac778       ; RDT-Basis
80014330: lw    v1,40(v0)           ; *(RDT + 0x28) = RVD-Tabelle (fixup'ter Zeiger)
80014338: lbu   v0,2(v1)            ; rec[+2] = cam_from
80014340: beq   v0,a0,0x80014360    ; Treffer -> return v1
80014348: addiu v1,v1,20            ; 20-Byte-Saetze
```

RDT+0x28 = RVD/Zone-Block (Port: `rdt_common.c:237` `zone_start = read_u32_le(&data[0x28])`),
Satzgroesse 20, `p[2] = cam_from` (Port: `rdt_common.c:119`), 4 Eckpunkte @+4..+0x12
(Port: `rdt_common.c:127-129`).

Der Port hat diesen Zeiger bereits als `re15_rdt_get_region_quad()`
(`re15_port/engine/src/rdt_common.c:466`; Kommentar dort woertlich:
"Mirrors PSX FUN_80014324 -> DAT_800ac790").

Dasselbe Gate wird game-weit benutzt (Xrefs auf `DAT_800ac790`, ghidra1_V2.txt:473740):
`FUN_8001b064:8001b180` (Charakter-Bodenschatten), `8001e970`, `FUN_8002c2d8`, `80039d64`,
`8003e86c`, `8003e924`, **`80053314` (= dieser ESP-Pfad)**.

### 3d. Gate 3 — hinter / zu nah an der Kamera (GTE)

In `FUN_800534c4`:

```
80053534: RTPS                       ; Wort 0x4a180001
8005354c: swc2 $14,0(t4)             ; SXY2 -> sp+0
8005356c: swc2 $19,0(v0)             ; SZ3  -> sp+4
80053570: lw   a0,4(sp)              ; a0 = SZ3
80053578: sra  v0,a0,2
8005357c: beq  v0,zero,0x800537b4    ; <-- SZ3>>2 == 0  (SZ3 < 4) -> Sprite verwerfen
```

Roh-Bytes: `80053570: 04 00 a4 8f | 00 00 00 00 | 83 10 04 00 | 8d 00 40 10`
(`83 10 04 00` = `sra v0,a0,2`; `8d 00 40 10` = `beq v0,zero,+0x234` -> 0x800537b4 = Epilog).

`SZ3` ist der von der GTE auf `[0,0xFFFF]` gesaettigte View-Z. Hinter der Kamera ⇒ MAC3 < 0 ⇒
SZ3 = 0 ⇒ verworfen. **Das ist der Rueckwaerts-Projektions-Schutz.**
Ein FAR-Gate existiert nicht und wird nicht gebraucht: SZ3 saettigt bei 0xFFFF, der OT-Index ist
`SZ3>>6` ≤ 1023 = genau die OT-Groesse (Cut-Doppelpuffer-Offset `cut<<12` = 4096 Byte =
1024 Eintraege, `800532d0: sll a2,a2,12`).

Bildschirm-Clipping (x/y ausserhalb) gibt es **nicht** als Test — das erledigt die
GPU-Drawing-Area.

### 3e. Der OT-Eintrag (das "hinter der Wand"-/Prioritaets-Modell)

Effekt-Sprite:

```
80053620: sra  v0,a0,6              ; a0 = SZ3     -> Bucket = SZ3 >> 6
80053624: sll  v0,v0,2
80053628: addu t4,t0,v0             ; t0 = OT-Basis (Arg 92(sp))
8005362c: lw   s5,0(t4)
```

Roh-Bytes `80053618: 12 98 00 00 | 00 7a 0d 00 | 83 11 04 00 | 80 10 02 00`
(`83 11 04 00` = `sra v0,a0,6`).

OT-Basis, die der Aufrufer uebergibt (`FUN_80053240`):

```
8005326c: lui   a3,0x800b
80053270: addiu a3,a3,-0x35CC       ; a3 = 0x800ACA34
80053298: lbu   a2,0(a3)            ; a2 = Puffer-Index 0/1
8005329c: addiu a3,a3,-0x235C       ; a3 = 0x800AA6D8   (Bytes: a4 dc e7 24)
800532d0: sll   a2,a2,12
800532d8: addu  s6,a2,a3            ; s6 = OT-Basis
80053390: sw    s6,20(sp)           ; -> Arg 92(sp) von FUN_800534c4
```

sprite.pri-Masken (`FUN_800392d4`-Emitter, DIESELBE OT):

```
800395e0: addiu s5,s5,0xca34        ; s5 = 0x800ACA34
800395e4: addiu s6,s5,0xdca4        ; s6 = 0x800AA6D8  <-- IDENTISCHE Basis
8003964c: lbu   v0,0(s5)            ; Puffer-Index
80039650: lh    a0,2(s3)            ; <-- MASKEN-DEPTH (s16)
80039654: sll   v0,v0,12
80039658: sll   a0,a0,2             ; Bucket = depth  (x4 Byte)
8003965c: addu  a0,a0,s6
80039660: jal   0x8006b538          ; AddPrim(&OT[depth], prim)
```

Roh-Bytes `80039650: 02 00 64 86 | 00 13 02 00 | 80 20 04 00 | 21 20 96 00`
und `800395e0: 34 ca b5 26 | a4 dc b6 26`.

Charakter-Meshes (Referenz, @0x80025640):

```
80025640: AVSZ3                      ; Wort 0x4b58002d
80025648: swc2 $7,0(t4)              ; OTZ
80025654: sra  v0,v1,6
80025658: beq  v0,zero,0x80025690    ; Near-Gate OTZ < 64
8002565c: sra  v1,v1,4               ; Bucket = OTZ >> 4
```

**Damit ist die Skala eindeutig festgenagelt — ohne Umweg ueber ZSF3:**

> OT-Bucket(Effekt) = `SZ3 >> 6` (@0x80053620) und OT-Bucket(Maske) = `depth` (@0x80039658),
> beide in derselben OT (Basis 0x800AA6D8 + cut<<12).
> Der Port bildet `depth` auf den Sortierschluessel `depth * 64` ab (`re15_pri.h:107`).
> Damit die relative Ordnung erhalten bleibt, muss gelten `key(Effekt) / 64 == SZ3 >> 6`,
> also **`key(Effekt) = SZ3 = vz`** — nicht `vz >> 4`.

Das passt exakt zu den Mesh-Dreiecken, die schon `avgz = vz` uebergeben.

### 3f. Zwei Billboard-Durchlaeufe, EIN Gate

`FUN_80053240` ruft `FUN_800534c4` zweimal (@0x8005338c und @0x80053420; der zweite Aufruf ist auf
flags Bit3 gegated, `8005339c: andi v1,v1,0x8`), mit anderer Rotationsmatrix (Kreuz-Billboard).
**Beide liegen hinter dem Region-Gate aus 3c** — es gibt also keine Effekt-Kategorie, die das
Gate absichtlich umgeht.

### 3g. Gibt es absichtliche Vollbild-Overlays im ESP-Pool?

**Nein.** Jeder der 96 Slots geht durch 3b + 3c + 3d. Vollbild-Blitze/Fades laufen im Original
ueber die Fade-Engine (`FUN_800217b0` / `FUN_80021634`; Port `fade_common.c` +
`re15_render_pc_set_fade`) bzw. die Letterbox-Quads (`FUN_80020f8c`) — beides andere Subsysteme,
die der Fix nicht beruehrt. Der Fix darf ausnahmslos auf alle ESP-Slots angewendet werden.

---

## 4. Die Divergenz (praezise)

### G-1 — Region-Cull fehlt komplett

Original: `FUN_80014368(&slot[0x28], DAT_800ac790) == 0` ⇒ Slot uebersprungen
(@0x80053334-0x8005333c).
Port: `pc_draw_effects` (main.c:204-361) hat keinen solchen Test. Ein Effekt in einem anderen
Raumteil wird in JEDER Kamera-Einstellung gezeichnet, in der er zufaellig ein positives `vz` hat
— also "um die Ecke", hinter Waenden, im Nachbargang.

Messbar an den Daten (RVD-Anker je Cut, gelesen aus `shared_assets/PSX/STAGE1/*.RDT` @+0x28,
Effekt-Ids aus RDT+0x4C):

| Raum | nCut | Effekt-Ids | Beispiel disjunkter Anker-Quads |
|---|---|---|---|
| ROOM11E0 (Strom, id 0x11) | 16 | 0x05, 0x07, **0x11** | Cut 0 `x[6801..13602] z[-6201..8701]` vs. Cut 5 `x[-13700..-7899] z[-29202..-2500]` |
| ROOM1090 (Feuer, Typ 0x26) | 16 | 0x05, 0x07, 0x09, **0x10** | Cut 3 `x[-15701..-1400] z[3700..8900]` vs. Cut 2 `x[-8300..4101] z[-17500..-8800]` |
| ROOM1140 | 10 | 0x05, 0x07 | Cut 6 `x[-10500..-4000] z[-7500..1500]` vs. Cut 3 `x[5500..16500] z[-25500..-8000]` |

Ein ROOM11E0-Strombogen bei z. B. `(-13000, -20000)` liegt in den Cut-5/6/7/8/13-Quads und weit
ausserhalb des Cut-0-Quads. Original: in Cut 0 unsichtbar. Port: sichtbar.

### G-2 — OT-Schluessel 16x zu klein

Port: `main.c:319` `int z = (int)vz >> 4;`
Soll: `z = vz` (Herleitung §3e).

Folge: Ein Effekt bei `vz = 8000` bekommt Schluessel 500; eine sprite.pri-Maske mit `depth = 100`
bekommt 6400. Sortiert wird absteigend (fern zuerst, `render_pc.c:751-786`), also wird die Maske
zuerst gezeichnet und der Effekt darueber. Eine Maske verdeckt einen Effekt nur noch, wenn
`depth*64 >= vz/16`, d. h. `depth >= vz/1024` — praktisch nie.
**Der sprite.pri-Vordergrund (= der "hinter der Wand"-Mechanismus) ist fuer Effekte damit faktisch
abgeschaltet.** Zusaetzlich liegt jeder Effekt vor praktisch jedem Charakter-Mesh.

### G-3 (klein) — Near-Gate zu streng

Original: verwerfen bei `SZ3 < 4` (@0x80053578-7c). Port: `vz < 64` (main.c:257).
Effekte mit View-Z 4..63 fehlen im Port. Nicht die Ursache des gemeldeten Symptoms, aber eine
belegte Divergenz.

---

## 5. Fix-Rezept fuer den Implementierer

Alles in `re15_port/platform/pc/main.c`. Das PSX-Target zeichnet keine Effekte
(`grep -n "esp_fx\|draw_effects" re15_port/platform/psx/main.c` = leer) — kein Gegenstueck noetig.

### Fix 1 — Region-Quad-Cull (Ursache des Nutzer-Symptoms)

`pc_draw_effects` bekommt das aktive Region-Quad durchgereicht. Die Variablen existieren bereits
im selben umschliessenden Block (`main.c:3979-3980`:
`static int16_t cam_region_xs[4], cam_region_zs[4]; static int cam_has_region;`) und werden bei
jedem Cut-Wechsel (`main.c:4037`) und bei jedem Raumwechsel (`main.c:5289`) aktualisiert.

1. Signatur aendern (`main.c:204`):

```c
static void pc_draw_effects(const re15_camera_view_t *cam, int cx, int cy,
                            int has_region, const int16_t rxs[4], const int16_t rzs[4]);
```

2. Aufruf (`main.c:7483`):

```c
pc_draw_effects(&cam_view, cx, cy, cam_has_region, cam_region_xs, cam_region_zs);
```

3. Test einsetzen — **direkt nach dem Bit1-Gate (`main.c:212`), VOR jeder Projektion**, weil das
   Original genau dort steht (@0x80053334, nach den flags-Gates @0x800532fc / @0x8005330c und vor
   dem RTPS in `FUN_800534c4`):

```c
/* byte-true Region-Sichtbarkeit @0x80053334-3c:
 *   jal 0x80014368 (Punkt-im-Viereck, X/Z) mit a0 = slot+0x28 (= die spaeter per RTPS
 *   projizierte Sprite-Position, @0x800534ec `addiu v0,a1,40`) und
 *   a1 = DAT_800ac790 = Region-Quad des aktiven Cuts (gesetzt @0x80021c0c aus
 *   FUN_80014324 = erster RVD-Satz mit rec[+2] == Cut-Id; RVD-Tabelle = RDT+0x28).
 *   Rueckgabe 0 -> `beq v0,zero,0x80053474` = Slot komplett uebersprungen.
 * Dasselbe Gate benutzen im Port bereits Spieler (main.c:5582), NPCs (main.c:6530)
 * und Props (main.c:7205). */
if (has_region &&
    !re15_aot_point_in_quad(f->x + f->xlat_x, f->z + f->xlat_z, rxs, rzs))
    continue;
```

Hinweise:

* Getestet wird die **gezeichnete** Position, also inkl. `xlat`: im Original ist `slot+0x28`
  genau das Feld, aus dem `FUN_800534c4` projiziert; im Port entspricht das
  `f->x + f->xlat_x` / `f->z + f->xlat_z` — exakt die Werte, die `main.c:238-240` in `wx`/`wz`
  legen.
* `has_region == 0` ⇒ nicht cullen (identische Fallback-Regel wie bei Spieler/NPC/Prop).
* `re15_aot_point_in_quad` ist in `re15_port/include/re15_aot.h:319` deklariert; `main.c`
  benutzt es bereits in 5582/6530/7205.

### Fix 2 — OT-Schluessel auf dieselbe Skala wie Meshes und Masken

`main.c:319`:

```diff
-        int z = (int)vz >> 4;
+        /* OT-Bucket des Original-Sprites = SZ3>>6 (@0x80053620 `sra v0,a0,6`), OT-Bucket der
+         * sprite.pri-Maske = depth (@0x80039658 `sll a0,a0,2`), beide in DERSELBEN OT
+         * (Basis 0x800AA6D8 + cut<<12, @0x8005329c / @0x800395e4). Der PC-Sortierschluessel
+         * ist "vz": Masken = depth*64 (re15_pri.h:107), Meshes = avgz = vz
+         * (main.c:1382 / main.c:5808). -> key(Effekt) muss vz sein, nicht vz>>4. Das alte
+         * >>4 machte jeden Effekt um Faktor 16 kameranaeher und hob die sprite.pri-
+         * Verdeckung praktisch auf. */
+        int z = (int)vz;
```

Optional exakt-quantisiert wie die PSX-OT (gleiche Ordnung, gleiche Ties):
`int z = ((int)vz >> 6) << 6;`

### Fix 3 (klein) — Near-Gate byte-true

`main.c:257`:

```diff
-        if (vz < 64) continue;
+        /* @0x80053578-7c: `sra v0,SZ3,2 ; beq v0,zero,epilog` = verwirf, wenn SZ3>>2 == 0.
+         * SZ3 = GTE-gesaettigtes View-Z in [0,0xFFFF] -> hinter der Kamera = 0 = verworfen. */
+        uint32_t sz3g = (uint32_t)(vz < 0 ? 0 : (vz > 0xFFFF ? 0xFFFF : vz));
+        if ((sz3g >> 2) == 0) continue;
```

(`re15_gte_divide` weiter unten bekommt damit `_sz3 >= 4` — genau die Original-Bedingung.)

### Betroffene Effekt-Quellen (alle laufen durch `pc_draw_effects`, keine Ausnahme)

| Effekt | Spawn-Stelle im Port | Bank / Id |
|---|---|---|
| **Strom ROOM11E0** | `scd_vm.c:3851` `re15_esp_fx_spawn_rows(re15_esp_room_bank(), effect_id, ...)` (op_sce_espr) | Raum-Bank, id **0x11** |
| **Feuer ROOM1090** (Typ 0x26, brennende Truemmer) | `enemy_ai_common.c:7696` -> `re15_esp_type26_flame` (`re15_esp.c:409`) | Raum-Bank id 0x10 / CORE00 id 0x08 |
| Spawn-/Emerge-Fx Typ 0x26 | `enemy_ai_common.c:7682` -> `re15_esp_type26_emerge` (`re15_esp.c:420`) | Raum-Bank id 0x09 |
| Blut / Treffer-Fx | `re15_damage.c:1615/1685/1702/1741/1754/1775`, `enemy_ai_common.c` (12 Stellen) | CORE00 id 0x00 |
| Muendungsfeuer / Rauch / Huelse | `game_step_common.c:1364/1369/1383` | CORE00 id 0x02 / 0x03 / 0x04 |
| Blut-Splatter | `re15_damage.c:1743/1783`, `enemy_ai_common.c:5965`, `enemy_ai_re2_crow.c:217/1284` | CORE00 id 0x00 |

Alle bekommen mit Fix 1+2 dasselbe Verhalten wie das Original — keine Sonderfaelle, keine
Kategorie-Unterscheidung noetig (Begruendung: §3f + §3g).

---

## 6. Wie man es verifiziert (ohne Screenshot)

**Sonde A — Zaehler "eingehaengt / verworfen" je Cut.**
`pc_draw_effects` hat bereits einen env-gegateten Trace (`RE15_FX_LOG`, `main.c:194-202`,
Zeilen `id=... sub=... slot=... q=...`). Um eine Verwerf-Ursache und eine Frame-Zeile erweitern:

```c
/* pro Partikel: reason = "ok" | "novis" | "region" | "near" | "noslot" */
fprintf(fl, "cut=%d id=%d pos=(%d,%d) reason=%s key=%d\n",
        active_cut_idx, f->effect_id, f->x + f->xlat_x, f->z + f->xlat_z, reason, z);
/* pro Frame: */
fprintf(fl, "FRAME %u cut=%d drawn=%d culled_region=%d culled_near=%d\n", ...);
```

Erwartung nach dem Fix in ROOM11E0: steht der Spieler in Cut 0, muss `culled_region` fuer jeden
Strom-Effekt mit `x < 6801` oder `z < -6201` **> 0** sein und `drawn == 0` fuer diese Partikel.

**Sonde B — deterministischer ctest (Skill `re15-room-probe`).**
Neuer Probe analog `re15_port/tests/unit/probe_1090_gate_selfdoor.c` (der holt das Region-Quad
schon per `re15_rdt_get_region_quad`, Zeile 110):

1. `ROOM11E0.RDT` laden, `re15_rdt_get_region_quad(&rdt, 0, xs, zs)` -> Cut-0-Anker
   (erwartet `x[6801..13602] z[-6201..8701]`, siehe §4).
2. `re15_esp_fx_spawn_ex(room_bank, 0x11, 0, ..., x=-13000, y=0, z=-20000, 0)`.
3. `re15_aot_point_in_quad(-13000, -20000, xs, zs)` muss **0** liefern -> mit Fix 1 darf
   `pc_draw_effects` fuer diesen Slot **kein** Tri einhaengen.
   Messgroesse: `re15_render_pc_dbg_textri_count()` (bereits deklariert, `main.c:271`) vor/nach
   dem Aufruf — Differenz muss 0 sein.
4. Gegenprobe: Spawn bei `(10000, 0, 0)` (innerhalb Cut 0) -> Differenz > 0.

**Sonde C — Tiefen-Schluessel (Fix 2), ohne Bild.**
`re15_render_pc_debug_pri_rects(dx,dy,w,h,dep,64)` (`render_pc.c:1775`) liefert die Masken-Depths
des aktiven Cuts. Assertion: fuer einen Effekt hinter einer Maske muss
`re15_pri_mask_camera_z(depth) >= key(Effekt)` gelten. Vor dem Fix ist `key = vz/16`, also
scheitert die Assertion fuer jeden Effekt mit `vz > 1024*depth`; nach dem Fix (`key = vz`) gilt
sie genau dann, wenn der Effekt wirklich hinter der Maske liegt.

**Sonde D — visuell (Nutzer / Skill `re15-port-visual-verify`).**
ROOM1090 (Feuer) betreten und an den Kamerawechseln entlanglaufen. Vor dem Fix brennen die
Flammen der Nachbar-Cuts als Overlay auf dem BG; nach dem Fix verschwinden sie beim Cut-Wechsel.
Nicht per `RE15_AUTOSHOT` / `RE15_SOFTWARE_RENDER` pruefen (Memory
`reai-v2-visual-verify-gdigrab`).

---

## 7. Offene Punkte / NICHT GEFUNDEN

1. **Tie-Breaking innerhalb eines OT-Buckets — NICHT GEFUNDEN (nicht verfolgt).**
   PSX `AddPrim` haengt vermutlich am Kopf der Bucket-Liste ein; die PC-Sortierung
   (`render_pc.c:751-761`, Insertion-Sort, stabil) haelt die Einfuege-Reihenfolge. Ich habe
   `FUN_8006b538` **nicht** disassembliert, also ist die Einfuegeseite hier NICHT belegt.
   Fuer die gemeldete Bug-Klasse irrelevant (die Bucket-Abstaende sind gross).
   *Naechster Weg:* `python .claude/skills/re15-psx-disasm/scripts/re15_disasm.py scan 0x8006b538`.
2. **ZSF3 nicht selbst nachgemessen.** Die Gleichsetzung "Mesh-Bucket ≈ vz/64" stuetzt sich auf
   `re15_pri.h:107` bzw. Memory `reai-v2-sprite-pri-occlusion` (ZSF3 = 341). Fuer den Fix ist das
   **nicht noetig** — die Herleitung in §3e benutzt nur `SZ3>>6` (@0x80053620) und `depth`
   (@0x80039658), beide hier direkt disassembliert. *Naechster Weg, falls gewuenscht:*
   das `ctc2 $29` / `InitGeom`-Setup in der EXE suchen.
3. **`FUN_80014324` hat keine Abbruchbedingung** (Endlosschleife, wenn kein RVD-Satz die Cut-Id
   traegt). In allen vier geprueften RDTs (ROOM1090 / ROOM10A0 / ROOM11E0 / ROOM1140) hat jeder
   Cut 0..nCut-1 einen Anker, das Original laeuft also nie hinein. Der Port-Fallback
   (`cam_has_region == 0` ⇒ nichts cullen) hat **kein** Original-Gegenstueck — er ist eine
   Port-Sicherung, kein RE-Befund. Er ist identisch zu dem, was Spieler/NPC/Prop schon tun.
4. **NICHT GEFUNDEN: ein zusaetzlicher "hinter dieser konkreten Wand"-Test.**
   Gesucht in `FUN_800534c4` (kompletter Body @0x800534c4-0x800537e0 disassembliert),
   `FUN_80053240` (@0x80053240-0x800534c0) und `FUN_80052fd0`. Dort gibt es **nur** die drei
   Gates aus §3b/3c/3d. Die Wand-Verdeckung entsteht ausschliesslich ueber die OT-Reihenfolge
   gegen die sprite.pri-Masken (§3e) — es gibt keine separate Sichtlinien-/Kollisions-Pruefung.
   *Geprueft und ausgeschlossen:* ein per-Effekt-Flag im EFF-Header — die einzigen im
   Zeichenpfad ausgewerteten flags-Bits sind 0/1/3 (@0x800532fc / @0x8005330c / @0x8005339c)
   und Bit4 fuer ABE (@0x800534f4 `lbu v0,108(a1)` + `srl v0,v0,3` + `andi v0,v0,0x2`).
5. **Nicht gemessen:** ob nach Fix 1 im Original tatsaechlich auch Effekte verschwinden, die im
   Port heute erwuenscht wirken (z. B. Muendungsfeuer waehrend eines Cut-Wechsels). Da der
   Spieler selbst mit demselben Quad gecullt wird (`main.c:5582`), ist das konsistent — aber die
   Live-Gegenprobe am Original steht aus (Sonde D).

---

## 8. Verifikation (unabhaengig nachdisassembliert)

Pruefer: zweiter Agent, 2026-08-27. Werkzeug: `.claude/skills/re15-psx-disasm/scripts/re15_disasm.py`
(`dis` / `bytes`) gegen `info/Re1.5/PSX.EXE`, plus eigener RDT-Parser und eigene Re-Implementierung
von `FUN_80014368` in Python. Port-Zitate gegen `git show HEAD:re15_port/platform/pc/main.c`
geprueft (der Arbeitsbaum wurde waehrend der Pruefung von einem Implementierer-Agenten veraendert;
alle unten bestaetigten Zeilennummern beziehen sich auf HEAD = den Stand, gegen den das Dossier
geschrieben wurde).

**Gesamturteil: TEILWEISE — der Kern traegt vollstaendig, dazu vier konkrete Korrekturen.**
Beide behaupteten Defekte (G-1 fehlender Region-Cull, G-2 OT-Schluessel 16x zu klein) sind
belegt. Widerlegt sind eine Adresse, eine Ungleichung, eine Port-Zeilennummer und eine
"OK"-Deckungsgleichheit.

### 8.1 Bestaetigt (eigene Disasm)

**Slot-Schleife `FUN_80053240` @0x80053240-0x800534c0** — eigener Dump:

```
800532f0: addiu s0,s0,-132          ; einmalige Vor-Dekrementierung (VOR dem Schleifenkopf)
800532f4: lbu   v1,0(s0)            ; <- Schleifenkopf, s0 = slot+0x6C = flags
800532fc: andi  v0,v1,0x1
80053300: beq   v0,zero,0x80053474
80053304: addiu s5,s5,-132          ; Delay-Slot: s5 = slot-Basis
80053308: andi  v0,v1,0x2
8005330c: beq   v0,zero,0x80053474
80053310: addiu a0,sp,24
80053314: lh    v0,-68(s0)          ; slot+0x28 = X   -> sw v0,24(sp)
80053318: lui   a1,0x800b
8005331c: lw    a1,-14448(a1)       ; a1 = 0x800ac790
80053324: lh    v0,-66(s0)          ; slot+0x2A = Y   -> sw v0,28(sp)
80053330: lh    v0,-64(s0)          ; slot+0x2C = Z   -> sw v0,32(sp) (Delay-Slot)
80053334: jal   0x80014368
8005333c: beq   v0,zero,0x80053474
```

Roh-Bytes (`bytes 0x80053310 48`), Zeichen fuer Zeichen identisch mit dem Dossier:

```
80053310: 18 00 a4 27 bc ff 02 86 0b 80 05 3c 90 c7 a5 8c
80053320: 18 00 a2 af be ff 02 86 00 00 00 00 1c 00 a2 af
80053330: c0 ff 02 86 da 50 00 0c 20 00 a2 af 4d 00 40 10
```

`da 50 00 0c` = `jal 0x80014368` (0x50DA<<2 = 0x14368). OK
`4d 00 40 10` = `beq v0,zero,+141*4` = 0x8005333c+4+0x234 = **0x80053474**. OK
0x80053474 ist nachweislich die Schleifen-Fortsetzung (eigener Dump):

```
80053474: lui   t1,0x800a
80053478: addiu t1,t1,29624         ; t1 = 0x800a73b8
8005347c: bne   t1,s5,0x800532f4    ; zurueck zum Schleifenkopf
80053480: addiu s0,s0,-132
```

**Kein toter Code** — der Rueckgabewert von `jal 0x80014368` wird ausgewertet, und das Sprungziel
ist der Schleifenkopf. Das Gate ist live.

Slot-Zahl gegengerechnet: `s5` startet auf `t1+12672` (@0x80053250), Abbruch bei `s5 == t1`,
Schritt 132 ergibt 12672/132 = **96 Slots**. Deckt sich mit `RE15_ESP_FX_MAX 96`
(`re15_port/include/re15_esp.h:145`). OK

**`FUN_80014368` @0x80014368-0x80014438** — komplett disassembliert. Liest `a0+0` (=X) und `a0+8`
(=Z), die Quad-Ecken s16 @`a1+4/+6`, `+8/+0xA`, `+0xC/+0xE`, `+0x10/+0x12`; `80014430: addu
v0,zero,zero` = 0 (aussen), `8001442c: ori v0,zero,0x1` = 1 (innen). OK

*Praezisierung:* es sind vier Vorzeichentests, aber nicht "gegen p0..p3" der Reihe nach, sondern
**zwei mit Anker p0** (Kanten p0->p1 und p0->p3) und **zwei mit Anker p2** (dieselben Kanten,
umgerechnet @0x800143d4-0x80014428). Feste Winding, kein dynamisches Vorzeichen. Der Port
(`re15_aot_point_in_quad`, `aot_common.c:303`) benutzt einen anderen Algorithmus (dynamisches
Vorzeichen, `cross == 0` = innen). **Gegenmessung:** eigene Python-Reimplementierung beider
Varianten, 320.000 Zufallspunkte je Raum ueber alle Anker-Quads von ROOM11E0 / ROOM1090 und
200.000 fuer ROOM1140 ergeben **0 Divergenzen**. Fuer diese Daten austauschbar. OK

**`FUN_80014324` @0x80014324** OK — `lw v0,-14472(v0)` (=DAT_800ac778, RDT-Basis), `lw v1,40(v0)`
(=RDT+0x28), `lbu v0,2(v1)`, Schritt `addiu v1,v1,20`. Der Dossier-Hinweis in §7.3 stimmt: die
Schleife @0x8001434c-0x80014358 hat **keine** Abbruchbedingung.

**`@0x80021c00/0c`** OK — `jal 0x80014324` mit `a0 = v1` (neuer Cut aus 0x800afbb5, @0x80021be0),
`sw v0,-14448(at)` = 0x800ac790.
Xref-Liste aus §3c ebenfalls exakt (ghidra1_V2.txt): 8001b180, 8001e970, **80021c0c (W)**,
8002c2d8, 80039d64, 8003e86c, 8003e924, **8005331c**. OK

**`FUN_800534c4` @0x800534c4-0x800537e0** — komplett gelesen.
`lw t0,92(sp)` @0x800534fc = das vom Aufrufer per `sw s6,20(sp)` @0x80053390 uebergebene Argument
(Callee-Frame `addiu sp,sp,-72` @0x800534c4, also 20+72 = 92). OK
Near-Gate: `80053570: lw a0,4(sp)` (SZ3, `swc2 $19` @0x8005356c = `e9 93 00 00`),
`80053578: sra v0,a0,2`, `8005357c: beq v0,zero,0x800537b4`. Roh-Bytes
`04 00 a4 8f | 00 00 00 00 | 83 10 04 00 | 8d 00 40 10` OK; 0x800537b4 ist der Epilog
(`lw fp,64(sp)` ...). OK Also **SZ3 < 4 verwerfen**, exakt wie behauptet.
OT-Bucket: `80053620: sra v0,a0,6` / `sll v0,v0,2` / `addu t4,t0,v0`, Roh-Bytes
`83 11 04 00 | 80 10 02 00` OK.
Kein Bildschirm-Clipping, kein FAR-Gate im gesamten Body — bestaetigt (§3d OK).
Zweiter Billboard-Pass @0x80053420 hinter `andi v1,v1,0x8` @0x8005339c OK (§3f).
ABE-Bit: `800534f4: lbu v0,108(a1)` (Roh `6c 00 a2 90`) + `srl v0,v0,3` + `andi v0,v0,0x2` OK (§7.4).

**OT-Basis 0x800AA6D8** OK — beide Seiten:
ESP `80053270: addiu a3,a3,-13772` (0x800b0000-13772 = 0x800aca34), `8005329c: addiu a3,a3,-9052`
(Roh `a4 dc e7 24`), also 0x800aca34-0x235C = **0x800aa6d8**; `800532d0: sll a2,a2,12`;
`800532d8: addu s6,a2,a3`.
Maske `800395e0: addiu s5,s5,-13772` / `800395e4: addiu s6,s5,-9052` (Roh
`34 ca b5 26 a4 dc b6 26`), also **dieselbe Basis**. OK

WERKZEUG-FALLE (fuer die naechste Sitzung): `re15_disasm.py` annotiert `addiu a3,a3,-9052`
naiv als `0x800adca4` (es bildet `hi<<16 | imm_unsigned`). Der wahre Wert ist `0x800aa6d8`. Die
Dossier-Zahl ist richtig, die Werkzeug-Annotation ist falsch — nicht umgekehrt.

**Masken-AddPrim @0x80039650-64** OK, Roh `02 00 64 86 | 00 13 02 00 | 80 20 04 00 | 21 20 96 00`.
*Ergaenzung:* das Dossier laesst den **Delay-Slot** weg. Vollstaendig:

```
8003964c: lbu  v0,0(s5)      ; Puffer-Index
80039650: lh   a0,2(s3)      ; depth (s16)
80039654: sll  v0,v0,12      ; buf*0x1000
80039658: sll  a0,a0,2       ; depth*4
8003965c: addu a0,a0,s6      ; + 0x800aa6d8
80039660: jal  0x8006b538
80039664: addu a0,v0,a0      ; <- Delay-Slot: + buf*0x1000
```

Das Argument ist also `OT_base + buf*0x1000 + depth*4`. Aendert an der Schlussfolgerung nichts
(Bucket = depth), nur an der Vollstaendigkeit des Zitats.

**Charakter-Referenz @0x80025640** OK — `.word 0x4b58002d` (AVSZ3), `swc2 $7` (OTZ),
`80025654: sra v0,v1,6` + `beq v0,zero,0x80025690` (Near-Gate OTZ<64), `8002565c: sra v1,v1,4`
(Bucket OTZ>>4). OK

**Dispatcher `FUN_80052fd0`** OK — `80052ffc: jal 0x8004f090`, `80053004: jal 0x80053240`,
`8005300c: jal 0x8004f0b0`. Der Rest der Funktion (@0x80053014-0x800530b4) ist reine
High-Water-Mark-Buchhaltung fuer OT/Prim-Puffer (0x80076d0c / 0x80076d10); `divu v0,0x28`
@0x80053080 bestaetigt nebenbei die **40-Byte-Prim-Groesse**. **Kein zweiter ESP-Zeichenpfad.** OK

**Daten (eigener RDT-Parser, `shared_assets/PSX/STAGE1/*.RDT`)** — alle Zahlen exakt:

| Raum | nCut (RDT[1]) | rvd (RDT+0x28) | espIdh (RDT+0x4C) | Ids |
|---|---|---|---|---|
| ROOM11E0 | 16 | 0x278 | 0x41F4 | `05 07 11 ff...` |
| ROOM1090 | 16 | 0x280 | 0x11010 | `05 07 09 10 ff...` |
| ROOM1140 | 10 | 0x1A8 | 0x11E0 | `05 07 ff...` |

Anker-Quads: ROOM11E0 Cut 0 = `[(7002,-4603),(6801,8700),(13602,8701),(13402,-6201)]` OK,
Cut 5 = `[(-13700,-29202),(-13200,-2500),(-7899,-2600),(-7900,-27600)]` OK;
ROOM1090 Cut 3 OK, Cut 2 OK; ROOM1140 Cut 6 OK, Cut 3 OK.
Das Beispiel aus §4 **mit der Original-Routine nachgerechnet**: `(-13000,-20000)` liegt in den
Cut-Quads **{5,6,7,8,13}** und ausserhalb von Cut 0 — exakt die im Dossier genannte Menge. OK

**Port-Zitate gegen HEAD** — alle bestaetigt:
`main.c:204` (Signatur), `:210`, `:212`, `:233`, `:238-240`, `:257` (`if (vz < 64) continue;`),
`:319` (`int z = (int)vz >> 4;`), `:347`, `:349/351`, `:1382` (`avgz`, wird als Tiefen-Argument an
`re15_render_textured_tri_shaded` durchgereicht), `:5808` (`(out_wz) = (float)_vz;`),
`:5582`/`:6530`/`:7205` (Region-Cull fuer Spieler/NPC/Prop), `:7483` (einziger Aufruf von
`pc_draw_effects` — `grep` findet genau eine Fundstelle), `:3979-3980`, `:4037`, `:5289`.
`aot_common.c:303`, `re15_aot.h:319`, `rdt_common.c:119/127-129/237/466`,
`re15_pri.h:103/104/106-107`, `render_pc.c:744-834/1775` OK.
Scope-Gegenprobe (eigener Klammertiefen-Zaehler ueber main.c, Kommentare/Strings entfernt):
`cam_has_region` deklariert auf Tiefe 3 (Zeile 3980), die Klammertiefe faellt zwischen 3980 und
7483 **nie unter 3**, die Variable ist am Aufrufort also sichtbar. Die Behauptung in §5 Fix 1
traegt. OK
Spawn-Tabelle in §5: `scd_vm.c:3851`, `enemy_ai_common.c:7682/7696`, `re15_esp.c:409/420`,
`game_step_common.c:1364/1369/1383` — alle stichprobenartig geoeffnet, Zeilen stimmen. OK
`grep -n "esp_fx|draw_effects" re15_port/platform/psx/main.c` = leer OK.

### 8.2 WIDERLEGT / korrigiert

**(a) Adresse `0x800534ec` ist falsch — richtig ist `0x8005350c`.**
Das Dossier schreibt in §3c und noch einmal im Fix-Kommentar (§5 Fix 1):

> "die `FUN_800534c4` gleich darauf per RTPS projiziert (`800534ec: addiu v0,a1,40`)"

Eigene Disasm + Roh-Bytes (`bytes 0x800534e8 40`):

```
800534e8: 2c 00 b3 af   sw    s3,44(sp)
800534ec: 24 00 b1 af   sw    s1,36(sp)     <- KEIN addiu, sondern ein Register-Spill
800534f0: 20 00 b0 af   sw    s0,32(sp)
800534f4: 6c 00 a2 90   lbu   v0,108(a1)
800534f8: 58 00 a7 8f   lw    a3,88(sp)
800534fc: 5c 00 a8 8f   lw    t0,92(sp)
80053500: c2 10 02 00   srl   v0,v0,3
80053504: 02 00 42 30   andi  v0,v0,0x2
80053508: 10 00 a2 af   sw    v0,16(sp)
8005350c: 28 00 a2 24   addiu v0,a1,40      <- HIER steht `addiu v0,a1,40`
80053510: 21 60 40 00   addu  t4,v0,zero
80053514: 00 00 80 c9   lwc2  $0,0(t4)
80053518: 04 00 81 c9   lwc2  $1,4(t4)
```

Die **Aussage** (slot+0x28 ist genau die spaeter projizierte Position) bleibt richtig — nur die
Adresse ist um 0x20 daneben. Die `lwc2`-Adressen 0x80053514/18 im Dossier sind korrekt.
ACHTUNG: Der Implementierer hat den falschen Wert `@0x800534ec` bereits woertlich in den
Port-Kommentar uebernommen (Arbeitsbaum, `main.c` Region-Cull-Block) — dort korrigieren.

**(b) Die Ungleichung in §4 / G-2 ist invertiert.**
Das Dossier schreibt:

> "Eine Maske verdeckt einen Effekt nur noch, wenn `depth*64 >= vz/16`, d. h. `depth >= vz/1024` —
> praktisch nie."

Der eigene Blick in den Sortierer (`render_pc.c:836`) sagt das Gegenteil:

```c
int mask_due = (mi < mask_n) &&
    ((float) re15_pri_mask_camera_z(s_pri_rects[mask_order[mi]].depth) >= tri_depth);
```

Sortiert wird absteigend (fern zuerst). Ist `mask_camera_z >= tri_depth`, wird die Maske **vor**
dem Dreieck gezeichnet, das Dreieck liegt also **darueber** und ist **nicht** verdeckt. Verdeckt
wird genau dann, wenn `mask_camera_z < key`, d. h.

* alter Schluessel `key = vz>>4`: Verdeckung nur fuer `depth*64 < vz/16`, also **`depth < vz/1024`**
  (bei vz=8000: `depth < 8`) — praktisch keine Maske;
* korrigierter Schluessel `key = vz`: `depth*64 < vz`, also **`depth < vz/64`** (bei vz=8000:
  `depth < 125`) — praktisch alle Masken.

Die **Schlussfolgerung** des Dossiers (sprite.pri-Verdeckung fuer Effekte faktisch abgeschaltet)
ist damit bestaetigt; die als Begruendung angegebene Bedingung ist falschherum, und "praktisch
nie" waere fuer die dort geschriebene Ungleichung sogar falsch (`depth >= vz/1024` ist leicht
erfuellt). Der Satz gehoert korrigiert, bevor jemand daraus eine Assertion baut — **§6 Sonde C
uebernimmt genau diese invertierte Ungleichung** ("fuer einen Effekt hinter einer Maske muss
`re15_pri_mask_camera_z(depth) >= key(Effekt)` gelten") und wuerde in der Form das Gegenteil
messen.

**(c) Port-Zeilennummer in §6 Sonde B falsch.**

> "`re15_render_pc_dbg_textri_count()` (bereits deklariert, `main.c:271`)"

`grep -n` auf HEAD: die Deklaration steht in **main.c:248** (und noch einmal in 3913).
Zeile 271 ist ein Kommentar (`* w16 = defW(Q12, ...)`).

**(d) §3b "Der Port hat das. OK." ist zu grosszuegig.**
`re15_esp_fx_visible` (`re15_esp.c:314-319`):

```c
if (!f) return 0;
if (!f->rows_base) return 1;          /* legacy fx: no flags model -> always drawn */
return (f->flags & 0x02) ? 1 : 0;
```

Das Bit1-Gate gilt also **nur** fuer Row-VM-Partikel; Legacy-Fx (`rows_base == 0`) umgehen es
unbedingt. Das Bit0-Gate (aktiv) hat der Port dagegen sauber, ueber
`re15_esp_fx_get` (`re15_esp.c:308-311`: `!s_esp_fx[i].active` liefert NULL). Fuer FINDING G ohne
Folgen, aber es ist keine "OK"-Deckungsgleichheit.

### 8.3 Nicht nachgeprueft

* `FUN_8006b538` (AddPrim-Einfuegeseite) — wie in §7.1 offen gelassen, nicht disassembliert.
* ZSF3 (§7.2) — nicht gemessen. Die Herleitung in §3e braucht ihn nicht; sie stuetzt sich nur auf
  `SZ3>>6` (@0x80053620) und `depth` (@0x80039658), beide hier unabhaengig bestaetigt.
  Gegenrechnung am Rand: mit ZSF3=341 waere `OTZ>>4 = ((341*3*sz)>>12)>>4` etwa `sz/64` — dieselbe
  Skala wie `SZ3>>6`, also konsistent, aber weiterhin **nicht gemessen**.
* `FUN_800217b0` / `FUN_80021634` / `FUN_80020f8c` (§3g, Fade/Letterbox als separate Subsysteme) —
  nicht disassembliert. Dass es im ESP-Pfad selbst keinen zweiten Zeichenweg gibt, ist dagegen
  belegt (§8.1, `FUN_80052fd0` vollstaendig gelesen).

### 8.4 Hinweis zum Arbeitsbaum

Waehrend dieser Pruefung hat ein Implementierer-Agent die Fixes 1-3 bereits im Arbeitsbaum
angewendet (`git diff re15_port/platform/pc/main.c`: +54/-4; `pc_draw_effects` jetzt @206 mit
`has_region/rxs/rzs`, neuer Helfer `re15_esp_fx_culled`, SZ3>>2-Gate, `key = vz`). Die
Korrekturen aus §8.2 (a)/(b)/(c) sind dort noch **nicht** eingearbeitet — (a) steht woertlich im
neuen Port-Kommentar.

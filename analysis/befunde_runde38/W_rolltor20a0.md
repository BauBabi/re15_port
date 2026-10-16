# Runde 38 Spur W — rolltor20a0 (Dossier, fortlaufend)

Baum: `.claude/worktrees/r38_rolltor20a0` (Zweig `r38/rolltor20a0`, Basis df08d83a).
Nutzer-Wortlaut (AUFTRAG.md Nachtrag 1, 2026-10-16 21:00):

> And additionally: in ROOM 20A0 the shutter that you have to open to push the box down is somehow DIRECTLY in the
> camera. I cannot see anything... this is not like this in the original....

## Stand

- [x] Messung vorher (Reproduktion am echten Weg, Cut, Objekt, Lage) — §1
- [x] Bisect (v0.8.21..v0.8.25) — §2: keine Regression, Zeichenweg seit aa346af1 unveraendert
- [x] RE-Belege Port-Seite + Original-Zeichenweg (statisch) — §3
- [ ] Original-Gegenprobe DuckStation (GDB-Haltepunkte im Zeichner + Bild) — §4
- [ ] Umsetzung
- [ ] Messung nachher
- [ ] Tests
- [ ] Elza 20A1

## 1. Messung vorher (Port, Basis df08d83a = v0.8.25 + Doku)

Bau `bash re15_port/tools/local_build.sh configure` / `build` -> `=== LOCAL-BUILD-OK (build)`.
Messkopie `re15_pc_w38.exe` (Laeufer `W_belege/lauf_w.sh`), RE15_FRAMEDUMP mit RE15_WINDOW_SCALE=1.

**Echter Weg** (`W_belege/wp_leiter_schalter_cut11.txt`, Lauf `v2_leiter`): `RE15_DEBUG_JUMP=20A0@250`,
`RE15_PLAYER_POS=16500,-21050,3072,2` (unten vor der Leiter), dann geschlossen geregelt (RE15_WEGPLAN):
1. Aktion an der Leiter = Door_aot_set slot 1 @0x01766 (Band 2, x 16000..17000 z -21550..-20550, Ziel ROOM20A0
   (16500,-7200,-20000) Band 4 **Cut 12**) -> F71 Spieler (16500,-20000), Cut 12.
2. zum Schalter (Aot_set slot 3 @0x017E8, Band 4, x 14900..15700 z -20400..-19600 -> sub05), Blick Nord, Aktion,
   Frage "There's a shutter switch here. Press it?" -> bestaetigt -> sub06 -> sub07 @0x01E40.
3. Rolladen = **obj 1** (Obj_model_set @0x01932: Typ 0, Attribut pc[8..9] = 0x0010 = Flach-Bit, Lage
   (7942,-7200,-20510), Drehung ry = 1024) faehrt hoch: `F555 obj1=(7942,-10800,-20510)` (Speed_set(1,-40) @0x01E50,
   70+20 Bilder Add_speed = -3600, Endlage = sub00 Member_set(1,-10800) @0x01A5A) — **Achse und Lage richtig**.
4. nach Sueden in die Kamerazone von Cut 11 (RVD-Zone 37 "12 -> 11": x 7000..15500 z -16500..-15000):
   `F650 cam=11`, zurueck nach Norden `F763 cam=12`, wieder Sued `F889 cam=11` (`W_belege/vorher_v2_leiter_cuts.txt`).

Bilder (`W_belege/vorher_bogen_v2_leiter.png`, Einzelbilder):
* Cut 12 (`vorher_cut12_F301_zu.png`, `vorher_cut12_F551_offen.png`): der Rolladen steht hinten, faehrt sichtbar
  hoch — unauffaellig.
* **Cut 11 (`vorher_cut11_F711_offen.png`, `vorher_cut11_F951_offen.png`): das ganze Bild ist mit der Lamellen-Textur
  des Rolladens zugedeckt (schwarze Diagonal-Baender, kein Raum, keine Figur) = der Nutzer-Befund.**

Geometrie (Werkzeug `W_belege/md1geo.py`, RDT-Modelltabelle +0x30 -> obj 1 MD1 @0x361C): ein Quader x -90..0
(Dicke 90), y -4140..0, z 0..6480, 50 Scheitel, **48 Vierecke** (4 x 4 Felder je 1620 x 1035 je Seite), keine
Dreiecke. Mit ry = 1024 liegt er in der Welt bei x 7942..14422, z -20510..-20420, y -7200..-11340 (zu) bzw.
**-10800..-14940 (offen)**.
Kamera Cut 11 (RDT +0x24, Satz 11): von (9648,-12691,-20700) nach (12096,-8263,-12150), fov 26684 -> H = 208.
Die Kamera haengt also 190 Einheiten nordlich der Rolladenebene, innerhalb ihrer x-Spanne, und in der Hoehe des
OFFENEN Rolladens (-12691 liegt in -10800..-14940): der offene Rolladen steht unmittelbar vor/um das Objektiv.
Regions-Viereck von Cut 11 (erste RVD-Zone mit cam_from 11, Zone 33: x 5500..17000 z -21000..-8500) enthaelt den
Objekt-Ursprung (7942,-20510) -> der Objekt-Ausschnitt FUN_80014368 laesst ihn im Original WIE im Port zu; er ist
also nicht der Unterschied.

## 2. Bisect (v0.8.21 .. v0.8.25)

Keine Regression. Die drei beteiligten Wege sind im Bereich unveraendert:
* Prop-Zeichenschleife `platform/pc/main.c` (`for (int pi = 0; pi < g_scd.prop_count ...` bis "Cut/player HUD"):
  `git show v0.8.21:...` gegen HEAD — einziger Unterschied die Flach-Schattierung 0x808080 (Runde 36 Spur N), KEINE
  Aenderung am Ausschnitt. Der Scheitel-Ausschnitt `if (_vz < 64) { ok = 0; break; }  /* H28 near-clip */` und das
  Fehlen von NCLIP/OTZ-Schranke/GPU-Groessengrenze stammen aus aa346af1 (2026-06-28, Uebernahme psx_dev-Engine).
* Rolladen-Fahrt: `git diff v0.8.21 HEAD -- engine/src/scd_vm.c` enthaelt keine Zeile zu Speed_set/Add_speed/
  Work_set/Member_set.
* Kamera: `engine/src/camera_common.c` unveraendert; `aot_common.c`-Aenderungen (Runde 36 R/H, Runde 37 C/D)
  betreffen Schub/Band/Tuer, nicht die Kamerazonen.
Damit zeigt jeder Stand seit v0.8.0 dasselbe Bild (der Raum ist Stage 2; der Nutzer kam jetzt erst dorthin).

## 3. RE-Belege (statisch)

### 3.1 Wer zeichnet den Rolladen im Original
`FUN_8002c18c` (Objekt-Zeichenschleife, `RE_15_Quellcode_V2/FUN_8002c18c.c`): Typ != 3 -> Ausschnitt
`FUN_80014368(&obj+0x5C, DAT_800ac790)`; Attribut (obj+0x0C = pc[8..9]) & 0x10 -> **FUN_80022f0c (Dreiecke) /
FUN_80023098 (Vierecke)**, sonst FUN_800254a0 / FUN_800256b0. Der Rolladen (Attribut 0x0010, 0 Dreiecke, 48
Vierecke) laeuft ausschliesslich durch **FUN_80023098**. Disassembliert (`re15_disasm.py dis 0x80023098 110`,
Bytes aus `info/Re1.5/PSX.EXE`):

```
80023148  30 00 28 4a   RTPT                       ; Scheitel 0..2 -> SXY0..2 (gesaettigt), SZ1..3
80023154  e98c0008 / e98d0014 / e98e0020             ; swc2 SXY0/1/2 -> Prim+8/+20/+32
80023168  06 00 40 4b   NCLIP                      ; MAC0 = OPZ aus den GESAETTIGTEN SXY
80023184  00 00 98 e9   swc2 MAC0,0(sp)
80023188  00 00 a2 8f   lw   v0,0(sp)
80023190  09 00 41 04   bgez v0,0x800231b8          ; OPZ >= 0 -> zeichnen, sonst Viereck verworfen
800231f8  01 00 18 4a   RTPS                       ; Scheitel 3 -> SXY2 -> Prim+44
8002320c  2e 00 68 4b   AVSZ4                      ; OTZ = ZSF4*(SZ0+SZ1+SZ2+SZ3) >> 12
80023218  00 00 87 e9   swc2 OTZ,0(a0=sp+4)
8002321c  04 00 a3 8f   lw   v1,4(sp)
80023224  82 11 03 00   srl  v0,v1,6
80023228  0d 00 40 10   beq  v0,zero,0x80023260     ; OTZ < 64 -> Viereck verworfen
8002322c  srl v0,v1,4 ... 8002325c sw v0,0(a1)      ; sonst in OT-Fach OTZ>>4 eingehaengt
```
FUN_800256b0 (beleuchtete Vierecke) hat dieselben zwei Schranken (`gte_nclip`/`stopz` < 0 -> weg;
`otz >> 6 != 0` -> einhaengen), `RE_15_Quellcode_V2/FUN_800256b0.c`.
GTE-Konstanten: InitGeom @0x80066c70 `li t0,0x155` / @0x80066c74 `gte_ldZSF3` (341), @0x80066c7c `li t0,0x100` /
@0x80066c80 `gte_ldZSF4` (256); Bildschirmmitte SetGeomOffset(0xa0,0x78) @0x8001643c/@0x80016440 (OFX 160,
OFY 120); H = fov >> 7 (camera_common.c).
Hardware (psx-spx `graphicsprocessingunitgpu.md` "Vertex"): SX/SY sind 11 Bit (-1024..+1023, die GTE saettigt
SX2/SY2 auf -0x400..0x3FF); **"Size Restriction: The maximum distance between two vertices is 1023 horizontally,
and 511 vertically. Polygons and lines that are exceeding that dimensions are NOT rendered."** Ein Viereck geht
als zwei Dreiecke (0,1,2) und (1,2,3) an den Rasterer.

### 3.2 Was der Port macht (main.c Prop-Schleife, Vierecke)
Je Scheitel `if (_vz < 64) { ok = 0; break; }` (Port-Erfindung "H28", kein Original-Gegenstueck), ungesaettigte
Bildschirmkoordinaten (`ax = cx + (ir1*n)>>16`), **kein NCLIP, keine OTZ-Schranke, keine GPU-Groessengrenze**;
jedes Dreieck geht an SDL, egal wie gross. Rueckseiten werden mitgezeichnet.

### 3.3 Nachgerechnet (vor der Original-Messung)
`W_belege/psxsim.py` rechnet den Weg aus 3.1 fuer jedes der 48 Vierecke nach (Kameramatrix wie
re15_camera_build_view mit exakter Wurzel statt SquareRoot0, Division exakt statt UNR — beides hoechstens 1 LSB):

| Fall | gezeichnet auf dem Schirm | verworfen |
|---|---|---|
| Cut 11, Rolladen OFFEN (y -10800) | **1 von 48** (Q28, nur ein 9-Pixel-Streifen am unteren Rand) | NCLIP 20, OTZ<64 16, GPU-Groesse 7, ausserhalb des Schirms 4 |
| Cut 11, Rolladen zu (y -7200) | 0 | NCLIP 28, ausserhalb des Schirms 20 |
| Cut 12, offen / zu (Gegenprobe Vorderseite) | 14 / 19 (Vorderseite + Kanten) | NCLIP 30 / 29 (Rueckseiten), ausserhalb 4 / 0 |
(`W_belege/psxsim_cut11_offen.txt`, `..._cut11_zu.txt`, `..._cut12_*.txt`). Die Gegenprobe Cut 12 zeigt, dass die
NCLIP-Richtung stimmt (die dem Port-Bild entsprechende Vorderseite bleibt stehen).
**Hypothese (noch am Original zu messen):** das Original verwirft in Cut 11 praktisch den ganzen offenen Rolladen
an NCLIP / OTZ<64 / GPU-Groessengrenze; der Port kennt keine dieser drei Schranken und deckt das Bild zu.

## 4. Original-Gegenprobe (DuckStation)

(laufend)

## Umsetzung

## Messung nachher

## Tests

## OFFEN

## Fuer den Nutzer

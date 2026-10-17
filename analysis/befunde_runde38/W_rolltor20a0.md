# Runde 38 Spur W — rolltor20a0 (Dossier, fortlaufend)

Baum: `.claude/worktrees/r38_rolltor20a0` (Zweig `r38/rolltor20a0`, Basis df08d83a).
Nutzer-Wortlaut (AUFTRAG.md Nachtrag 1, 2026-10-16 21:00):

> And additionally: in ROOM 20A0 the shutter that you have to open to push the box down is somehow DIRECTLY in the
> camera. I cannot see anything... this is not like this in the original....

## Stand

- [x] Messung vorher (Reproduktion am echten Weg, Cut, Objekt, Lage) — §1
- [x] Bisect (v0.8.21..v0.8.25) — §2: keine Regression, Zeichenweg seit aa346af1 unveraendert
- [x] RE-Belege Port-Seite + Original-Zeichenweg (statisch) — §3
- [x] Original-Gegenprobe DuckStation (GDB-Haltepunkte im Zeichner + Bild) — §4: Hypothese bestaetigt, 48/48
- [x] Zweiter Original-Fall (anderer Zeichner, ROOM1050 Rolltor Cut 5) — §4b: 15/15 OPZ exakt
- [x] Umsetzung — §5 (engine/src/objpoly_r38.c, platform/pc/src/objpoly_r38_pc.c, Haken main.c)
- [x] Messung nachher — §6 (Cut 11 frei, Bild = Original, OBJPOLY = Original)
- [x] Tests — §7 (unit_r38_rolltor20a0, unit_r38_rolltor20a0_windung, integration_r38_rolltor20a0)
- [x] Elza 20A1 — RDT byte-gleich, exe-Lauf B mit Elza (PL04) gruen
- [x] Nebenwirkungen — §8 (Zensus aller Raeume; Port-Modelle ohne NCLIP — ueberholt durch §9)
- [x] Fortsetzung 2026-10-17: Merge r38/cursor (Spur A), EIN NCLIP-Weg, EINE Regel fuer Port-Modelle — §9
- [x] nach dem Merge: Bau, Messung nachher 20A0/20A1 Cut 11 + ROOM1050 Cut 5 (+ 20A0 Cut 2) — §9.4; eigene + A-Riegel 8/8 — §9.5
- [x] Modul-Testgruppen nach dem Merge — §9.5: 67/67 gruen
- [x] Nachbesserung 1 (Abnahme 0, M1 + M2) — §10: halbtransparente Raum-Objekte (ABE/ABR/STP) wie im Original
  (5060/5061 Cut 4 = Original-Bild), Zensus in beide Richtungen mit Typ 3 + Eltern, MEHR-Faelle am Spiel gemessen

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

## 4. Original-Gegenprobe (DuckStation, Auslieferungs-EXE)

Werkzeug `W_belege/orig_20a0_rolladen.py` (Muster Runde 36 R `orig_1090_push.py`: vgamepad + GDB 127.0.0.1:2345,
settings.ini gesichert und zurueckgeschrieben, `EnableGDBServer = false` geprueft), Log `W_belege/orig1_log.txt`,
Bild aus dem SaveStateOnExit-Spielstand (`re15_ss.vram_png(..., true_base=True)`, beide Bildpuffer):
`W_belege/orig_cut11_offen_buf0.png` / `..._buf1.png`.

Ablauf und Messwerte (alles aus der RAM gelesen):
* `EXE-Stub @0x80026e4c: 0800e003` = Auslieferung. Debug-Menue-Spielstand, JUMP-Auswahl per RAM (Zeile 0x800BBE5D = 1,
  Stage 0x800BBE5E = 1, Raumindex 0x800BBE60 = 0x0A), Quadrat -> `Raumindex @0x800b0fe2: 10`.
* Pool (0x800B3F98 + 0x94*k): `Pool 1: (153, 7942, -7200, -20510, 1024, 0, 16)` = Flags 0x99, Lage, ry 1024, Typ 0,
  Attribut 0x10 — gleich dem Port-Record @0x01932.
* Leiter (Spieler per RAM auf (16500,-3600,-21050) Band 2, Blick 3072, Quadrat): `nach der Leiter: Spieler (16500,
  -7200, -20000, 3072, 4) Cut 12` — gleich dem Port (F71).
* Schalter (15348,-7200,-19456) Blick 1024: dreimal Quadrat -> `Pool1 y=-9120` (faehrt), Ende `(7942, -10800,
  -20510)` — gleich dem Port (`F555 obj1=(7942,-10800,-20510)`).
* Spieler per RAM in die Zone 12->11 (12500,-7200,-15700) -> `Cut 11` (DAT_800B0FE4).
* Haltepunkte im Zeichner (ein voller Aufruf, `FUN_80023098 Eintritt a0=8011c974 Anzahl 48 (Rolladen)`):
  @0x80023190 v0 = OPZ + Prim-SXY0..2, @0x80023228 v1 = OTZ + SXY3 (Prim+44), @0x8002325c = in die OT eingehaengt.
  Ergebnis je Viereck in `orig1_log.txt` (Q00..Q47). Zusammenfassung:
  - 20 Vierecke OPZ < 0 -> @0x80023190 verworfen (Rueckseiten),
  - 16 Vierecke OTZ < 64 -> @0x80023228 verworfen (z. B. Q13 otz 47, Q34 otz 44, Q23/Q36/Q40.. otz 0),
  - 12 eingehaengt; davon 6 mit beiden Dreiecken ueber der GPU-Grenze, Q07 mit einem (z. B. Q35 sxy (804,-76),(481,231),(-114,-344),
    (105,395): dy 739 >= 512), die uebrigen liegen ausserhalb des 320x240-Bilds (x > 319 bzw. y < 0); Q28 T0
    (481,231),(374,333),(105,395) beruehrt die Bbox des Bilds, die Flaeche selbst liegt bei y 231..239 erst ab x 463
    -> **nichts vom Rolladen auf dem Schirm**.
* **Bild (`orig_cut11_offen_buf0.png`): Cut 11 zeigt die Plattform mit Kiste, Faessern, Regal und Leon — KEIN
  Rolladen.** Das ist die Original-Sollseite zum Port-Bild `vorher_cut11_F711_offen.png`.

Abgleich mit der Nachrechnung `psxsim.py` (Abschnitt 3.3): **NCLIP-Entscheid 48/48 gleich, eingehaengt 48/48 gleich,
max |dSXY| = 3 Pixel** (Rest = exakte Wurzel/Division statt SquareRoot0/UNR). Damit ist der Mechanismus belegt:
das Original zeigt den offenen Rolladen in Cut 11 nicht, weil sein Objekt-Zeichner jedes Viereck an NCLIP
(@0x80023190) bzw. OTZ < 64 (@0x80023228) verwirft und die GPU die eingehaengten Reste als zu gross (psx-spx Size
Restriction) bzw. ausserhalb des Bilds nicht zeichnet. Der Port kennt keine dieser drei Schranken.

## 4b. Zweiter Original-Fall: ROOM1050 Rolltor (Licht-Viereck-Zeichner FUN_800256b0)

Damit der Fix nicht nur am Rolladen (Flach-Zeichner FUN_80023098) belegt ist: Werkzeug `W_belege/orig_objmessung.py`
(gleiche GDB-/vgamepad-Mechanik, Kamera-Fuehrung durch die RVD-Zonen-Kette per RAM-Lage; andere Pool-Objekte per
Flags-Wort Bit 0 aus, damit nur das gesuchte Modell zeichnet), Log `W_belege/orig1050_log.txt`:
* JUMP 105 (Stage 0, Raumindex 5), `Pool 0: (11, 15432, 0, -10424, 2048, 0, 0)` = Obj_model_set @0x00C36
  (Typ 0, Attribut 0 -> FUN_800256b0), Zonen-Kette Cut 0 -> 1 -> 2 -> 3 -> 4 -> 5, `Cut 5`.
* Haltepunkte FUN_800256b0 @0x800256b0 (Eintritt; der Zeichner laeuft auch fuer Leons Koerperteile: 16 Aufrufe
  mit 3..64 Vierecken, uebersprungen), @0x800257c0 `bgez v0` (OPZ), @0x800258b4, @0x80025914:
  `FUN_800256b0 Eintritt a0=8012f9dc Anzahl 15 (gesuchtes Objekt)` -> **alle 15 Vierecke OPZ < 0** (-36, -339, -374,
  -17, -17, -283, -319, -432, -334, -357, -391, -336, -319, -373, -432) -> nichts eingehaengt.
* Port (Riegel R7): dieselben 15 OPZ **exakt** (max |dOPZ| 0), nichts gezeichnet; der alte Port-Weg malte dort die
  Rueckseite des Tors (4542 Pixel, Zensus §8).

## 5. Umsetzung

Der Fix bildet den Original-Objektzeichner im PC-Prop-Weg nach — fuer ALLE Raum-Objekte (Obj_model_set), denn alle
vier Original-Zeichner (Flach/Licht x Dreieck/Viereck) haben dieselben Schranken; der Rolladen ist nur der Fall, an
dem der Unterschied das ganze Bild kostet.

* NEU `re15_port/engine/src/objpoly_r38.c` + `re15_port/include/re15_objpoly_r38.h`:
  - `re15_objpoly_r38_rtps`: IR1/IR2 gesaettigt -8000h..7FFFh, SZ3 0..FFFFh, n = `re15_gte_divide` (UNR, max 1FFFFh),
    SX/SY = Bildmitte + (n*IR >> 16) gesaettigt -400h..3FFh (psx-spx RTPS; OFX/OFY 160/120 = SetGeomOffset(0xa0,0x78)
    @0x8001643c/@0x80016440).
  - `re15_objpoly_r38_tri` / `_quad`: NCLIP auf den gesaettigten SXY (psx-spx-Formel), OPZ < 0 -> weg
    (`bltz` @0x80023010 / `bgez` @0x80023190 / @0x800255a8 / @0x800257c0); OTZ = ZSF3*(SZ1+SZ2+SZ3)>>12 bzw.
    ZSF4*(SZ0..3)>>12, OTZ < 64 -> weg (`srl/sra 6` + `beq` @0x80023044/48, @0x80023224/28, @0x80025654/58,
    @0x800258b0/b4; ZSF3 = 341 @0x80066c70/74, ZSF4 = 256 @0x80066c7c/80); GPU-Groessengrenze je Dreieck (0,1,2) und
    (1,2,3): Spanne > 1023 waagerecht oder > 511 senkrecht -> nicht gerastert (psx-spx "Size Restriction").
  - `re15_objpoly_r38_tri_mesh` / `_quad_mesh`: Sichtraum aus MD1-Scheitel + Kamera x Objekt (dieselbe Rechnung wie
    bisher in main.c), dann die Schranken; liefern die gesaettigten Bildkoordinaten und den Sortierschluessel
    (Mittel der SZ — fuer jedes bisher gezeichnete Polygon identisch mit dem alten Mittel der Sicht-z).
  - Messschiene `RE15_OBJPOLY_LOG=<datei>` (Diagnose): je Bild und Objekt "gezeichnet/nclip/otz/gpu".
* NEU `re15_port/platform/pc/src/objpoly_r38_pc.c`: `re15_objpoly_r38_halbviereck_pc` — besteht nur EIN GPU-Dreieck
  eines Vierecks (Original Q07), wird nur dieses gezeichnet.
* Haken `re15_port/platform/pc/main.c` (Prop-Schleife, "Runde 38 Spur W"): ein Include; in der Dreiecks- und der
  Vierecks-Schleife ersetzen `re15_objpoly_r38_tri_mesh` / `_quad_mesh` die bisherige Projektion samt Port-Ausschnitt
  `if (_vz < 64) { ok = 0; break; }  /* H28 near-clip */` (kein Original-Gegenstueck); beim Viereck zeichnet der
  bisherige Aufruf-Block (Teilung (0,1,3)+(0,3,2)) nur noch, wenn beide GPU-Dreiecke bestehen, sonst der Halbfall;
  eine Zeile meldet der Messschiene den Objekt-Beginn; ~~eine Zeile setzt `r38_nclip` = MD1 liegt im RDT-Puffer
  (`rdt_buf`/`rdt_size`), d. h. NCLIP nur fuer Original-Modelle (PORT-WAHL fuer eingebackene Port-Modelle, §8).~~
  **Ueberholt 2026-10-17 (§9): `r38_nclip` entfaellt, NCLIP gilt fuer jedes Objekt; die NCLIP-Rechnung ist Spur As
  re15_nclip_r38.** Diff main.c gegen df08d83a (Stand 2e2f9579): `14 insertions(+), 34 deletions(-)`; nach dem Merge
  gegen master: `13 insertions(+), 34 deletions(-)` (§9.2).
* Keine Konstante ohne Adresse: 341/256 (ZSF), 64 (OTZ-Schranke), -400h/3FFh (SX/SY), 1023/511 (GPU) — Belege oben
  und im Kopf von re15_objpoly_r38.h.
* Bekannte Rest-Abweichung (vorbestehend, NICHT Teil des Fixes): der PC teilt ein voll gezeichnetes Viereck weiter an
  der Diagonale 0-3, die PSX-GPU an 1-2; Flaeche gleich, die affine Texturverzerrung eines perspektivisch stark
  verzerrten Vierecks folgt der anderen Diagonale (siehe OFFEN).

## 6. Messung nachher (Port, gleicher echter Weg)

Bau `local_build.sh configure` + `build` -> `=== LOCAL-BUILD-OK (build)` (keine neue Warnung). Lauf `n2_leiter`
(Wegplan `W_belege/wp_leiter_schalter_cut11.txt`, gleiche Startlage wie v2_leiter):
* Ablauf identisch: Leiter F71 -> Cut 12; Rolladen `F555 obj1=(7942,-10800,-20510)`; Cut 11 ab F650, Cut 12 F763,
  Cut 11 F889.
* Bilder `W_belege/nachher_bogen_n2_leiter.png`, `nachher_cut11_F711_offen.png`, `nachher_cut11_F951_offen.png`:
  **Cut 11 zeigt die Plattform mit Kiste, Faessern, Regal und Leon — wie das Original-Bild
  `orig_cut11_offen_buf0.png`.** Cut 12 (`nachher_cut12_F301_zu.png`, `nachher_cut12_F551_offen.png`) zeigt den
  Rolladen wie vorher (Vorderseite, faehrt hoch).
* Pixelvergleich gegen das Original-Bild (Figur steht 156 Einheiten anders): vorher 59,9 % der Pixel <= 24 Stufen,
  MAD 25,5; nachher **98,4 %, MAD 3,0**.
* Raumhintergrund-Anteil (Werkzeug `test_r38_rolltor20a0 --bild <ppm> ROOM20A.BSS 11 0` = Pixel gleich dem
  dekodierten BSS-Bild von Cut 11): vorher 0,1305 (F711/F951; frei nur die linke Wand), nachher **0,9424** (F711).
* Messschiene `W_belege/nachher_n2_objpoly_obj1.txt`: **Cut 11 obj 1: 524 Bilder `gezeichnet=6 nclip=20 otz=16
  gpu=6`** = Original (20 NCLIP, 16 OTZ<64, 6 eingehaengte mit beiden Dreiecken zu gross; die 6 gezeichneten liegen
  ausserhalb des Bilds bzw. Q07 halb). Cut 12: 18-25 gezeichnet, 23-32 NCLIP (Rueckseiten), 0 OTZ/GPU.
* Kisten-Ablauf (`W_belege/wp_kiste_fall.txt`, Lauf v3_kiste, vor dem Fix gemessen; der Fix aendert nur das
  Zeichnen): Rolladen auf -> Sca_id_set @0x01E5E-6E setzt Zelle 32 (die Wand unter dem Rolladen, x 8000..14700
  z -21000..-20500) in allen 5 Regionen auf u0 = 0 (`ZELLE r0[32] LAUFZEIT ... u0 0x00`) -> Kiste von Sueden durch die
  Oeffnung geschoben: `F1261 AUSLOESER wort=0x4206 band=4 pos=(12250,-7200,-21473)`, `F1280 LANDUNG y=-3600 band=2`,
  zweiter Fall auf Band 1 (y -1800). Waehrend des ganzen Schiebens ist Cut 11 aktiv (F643-F1087) — genau dort war
  das Bild vorher zugedeckt (`vorher`: v3_kiste F661-F1061).
  (Nebenbefund: der Runde-36-Satz "die Kiste faellt in 20A0 nie" beruecksichtigte die Rolladen-Zelle 32 nicht; mit
  offenem Rolladen faellt sie im Port durch die Oeffnung.)

## 7. Tests

Neu (nur aus `re15_port/tests/unit/probes/r38_rolltor20a0.cmake` registriert):
* `unit_r38_rolltor20a0` (`tests/unit/test_r38_rolltor20a0.c`, echte RDTs, keine exe):
  R1 48 Vierecke Cut 11 offen gegen die Original-Tabelle (NCLIP 48/48, eingehaengt 48/48, GPU-Dreiecke, **max |dSXY| 0**);
  R2 Bedeckung neu 0 Pixel (offen und zu); R3 Negativkontrolle alter Weg 67728/76800 Pixel (88,2 %, Schwelle 85 %);
  R4 Cut 12 von vorn: 12721 vs 12812 / 10934 vs 11012 Pixel (Abweichung < 1 %, Rolladen bleibt sichtbar);
  R5 GPU-Grenze je Dreieck (Original Q07); R6 ROOM20A1.RDT == ROOM20A0.RDT (300176 Bytes) + 0 Pixel;
  R7 ROOM1050 Rolltor Cut 5: 15/15 OPZ exakt wie das Original, 0 gezeichnet.
  Werkzeug-Modus `--bild <ppm> <bss> <cut> [toleranz]` (Raumhintergrund-Anteil) fuer den exe-Riegel.
* `unit_r38_rolltor20a0_windung` (`tests/unit/test_r38_rolltor20a0_windung.c`): Windungs-Zensus 568 Objekt-Modelle
  aus 206 RDTs: w.n < 0 bei 30037 von 30188 Polygonen (99,5 %) = Original-Konvention; 20 Port-Modelle, davon nur der
  Feuerloescher R37 verkehrt (36/148) -> NCLIP im PC nur fuer RDT-Modelle; Schalter nclip wirkt (O4).
  **Ueberholt 2026-10-17 (§9.5): O2 prueft jetzt, dass jedes RAEUMLICHE Port-Modell wie das Original gewickelt ist
  (Feuerloescher von Spur A neu gewickelt: 148/148), O4 dass der Verwurf ueber re15_nclip_r38 laeuft.**
* `integration_r38_rolltor20a0` (`tests/integration/test_r38_rolltor20a0.cmake`, Wegplaene
  `tests/integration/r38_rolltor20a0/wp_cut11.txt`, `wp_kiste.txt`; Messkopie `re15_pc_r38w_haken_*`):
  A Leon ROOM20A0 echter Weg -> Cut 11 F700/F850/F1000: Raumhintergrund-Anteil 0,9424 / 0,9432 / 0,9430
  (Schwelle 0,80; vorher 0,1305), OBJPOLY Cut 11 obj 1: 350 Bilder alle `gezeichnet=6 nclip=20 otz=16 gpu=6`;
  B Elza ROOM20A1 (`[pl] Spieler-Familie PL04 (character=4, Elza-Bit=1)`): 0,9488 / 0,9493 / 0,9493, 350 Bilder wie A;
  C Leon Kiste: `F1259 AUSLOESER ... pos=(12250,-7200,-21473)`, `F1278 LANDUNG y=-3600 band=2`, `F1297 LANDUNG y=-1800
  band=1` (Kiste faellt durch die Rolladen-Oeffnung). Bilder `W_belege/nachher_integ_leon20A0_cut11_F850.png`,
  `nachher_integ_elza20A1_cut11_F850.png`.

Lauf (gezielt, keine volle Suite):
`ctest --test-dir re15_port/build -R "^(unit|integration)_r38_rolltor20a0(_windung)?$" --timeout 1200 --output-on-failure`
-> `100% tests passed, 0 tests failed out of 3` (integration 214 s).
Testgruppen der beruehrten Module: siehe §7b.

### 7b. Testgruppen der beruehrten Module (Prop-Zeichenweg main.c, Kisten)

Der Diff beruehrt die Prop-Schleife in `platform/pc/main.c` (jedes Raum-Objekt und jedes eingebackene Port-Modell)
-> alle Riegel mit Bild-/Lade-Pruefung von Raum-Objekten plus die Kisten-Riegel (24 Tests, seriell, 3519 s):
```
ctest --test-dir re15_port/build -R "^(integration_(r30_irons_tisch_bild|r30_irons_tisch_licht|r30_sicherung_bild|r30_sicherung_laden|r34n_e_dokumente_bild|r36_kartenitems_bild|r32_tor_hell|r37_feuerloescher|r36_schluessel|r30_granate_laden|r30_irons_tisch_laden|r36_dachkiste|r37_kiste|r37_kiste2feld|r37_kistenregel|r38_rolltor20a0)|unit_r38_rolltor20a0|unit_r38_rolltor20a0_windung|unit_prop_texel_key_11f0|unit_r30_irons_tisch|unit_r36_kartenitems|unit_r37_feuerloescher_modell|unit_r30_sicherung_modell|unit_r36_objfall)$" --timeout 1200 --output-on-failure
-> 96% tests passed, 1 tests failed out of 24
```
Alle 23 fremden Riegel gruen (u. a. integration_r30_irons_tisch_bild 38,6 s, r30_sicherung_bild 11,5 s,
r34n_e_dokumente_bild 105 s, r36_kartenitems_bild 483 s, r37_feuerloescher 385 s, r36_schluessel 108 s,
r36_dachkiste 1716 s, r37_kiste/kiste2feld/kistenregel 105/64/288 s). Der eine rote war `integration_r38_rolltor20a0`
selbst, und zwar ein Artefakt: ich hatte waehrend des Laufs das Skript um die CONTINUE-Laeufe erweitert, die
Registrierung aus dem alten Configure gab `RE15_KARTE_TOOL` noch nicht mit (`RE15_KARTE_TOOL fehlt/existiert nicht:
''`, 0,05 s). Nach Configure + Build gesondert nachgefahren, Ergebnis §7c.

### 7c. Nachgefahren (Fortsetzung 2026-10-17): Umschaltbild F650 ist ein Messartefakt

Der Vorgaenger hat den exe-Riegel um die CONTINUE-Laeufe B/D erweitert und die Bilder aus dem Zustandslog gewaehlt
(f60148cd, UNGEPRUEFT). Sein letzter Lauf (scratchpad W38/ctest_final.log, 23:4x) war ROT: `a_leon / b_elza /
d_leon_karte: Bild 650 zugedeckt — Hintergrund-Anteil 0.0042 < 0.80`, alle uebrigen Cut-11-Bilder 0,942..0,950.
Nachgemessen (Lauf lauf_389ac10d_a_leon): Zustand F649 `cam=12`, F650 `cam=11` — F650 ist das Umschaltbild, der
Framedump F650 zeigt noch die **Cut-12-Ansicht** (Schalterwand, Figur aus Cut-12-Sicht, Rolladen von vorn oben;
Bild verglichen gegen F700). Das Zustandslog schreibt den Schnitt NACH dem Logikschritt, gezeichnet wurde F650 noch mit
dem alten. Also kein Rolladen vor der Kamera, sondern die falsche Bildwahl des Riegels. Behebung e4c0cb9e: gewertet
wird nur ein Bild, dessen Zustand UND Vorbild-Zustand cam=11 tragen (F650 faellt heraus, F700..F1100 bleiben).
Ergebnis nach dem Merge: §9.4.

## 8. Nebenwirkungen (Zensus) — wo der Port jetzt sonst noch anders (= wie das Original) zeichnet

**Nachbesserung 1 (Abnahme 0, M1/M2):** dieser Zensus zaehlte nur WENIGER, ohne Typ 3 und Eltern-Objekte; der
vollstaendige Zensus in BEIDE Richtungen steht in §10.7 (MEHR 3, WENIGER 22). Die Aussage "= wie das Original" stimmte
fuer den MEHR-Fall ROOM5060/5061 Cut 4 nicht (Halbviereck deckend statt halbtransparent) — seit §10.5 (ABE/ABR/STP der
Raum-Objekte) stimmt sie, gemessen gegen das Original-Bild (§10.6).

Der Fix gilt fuer alle RDT-Objekte (derselbe Original-Zeichner). Zensus `W_belege/zensus_objpoly.py` ->
`W_belege/zensus_objpoly.txt`: 152 inhaltsverschiedene RDTs, jedes Obj_model_set (alle Skript-Lagen) x jeder Cut,
dessen Regions-Viereck die Lage enthaelt = 1757 Faelle; Summe Bildbedeckung alt 4 193 818 px, neu 3 810 642 px. Mehr
als 2 % des Bilds weniger nur in 18 Faellen (9 je Leon/Elza-Paar):
* ROOM10D0/10D1 obj 0 (Rolladen vor Marvins Raum) Cut 12/13/6/7 in der geschlossenen Lage: der alte Port malte die
  RUECKSEITE (Cut 12 100 %, Cut 13 70 %); in der Szene (sub21 "It's a shutter switch", `Cut_chg 13` @0x01A22, sub22
  hebt ihn auf y -6550) betrifft das die ersten Bilder von Cut 13 (y 0: alt 53884 -> neu 0 px; y -1500: alt 25880 ->
  neu 1080 px; y -3000: alt 2798 -> neu 1650 px; gerechnet mit zensus_objpoly.py, Lagen aus sub22 @0x01DBC); oben
  (y -6550) kein Unterschied. Cut 14 (Vorderseite) unveraendert.
* ROOM1130/1131 obj 0 (Rolladen) Cut 8 (geschlossene Lage, ry 3072) und Cut 9 (Lage nach dem Oeffnen @0x00950,
  ry 1024): jeweils die Rueckseite, alt 25597 / 22378 px -> neu 0. Die im Spiel benutzten Paare (Cut 9 zu = die
  Oeffnungs-Szene, Cut 8 nach dem Oeffnen = Cut_replace 9->8 @0x009E0) bleiben unveraendert (23512 / 27093 px).
* ROOM1050/1051 obj 0 (Rolltor) Cut 5: Rueckseite 4542 px -> 0 — **am Original gemessen (§4b)**.
* ROOM1170 obj 3 (Hubschrauber-Rotor) Cut 3: 40108 -> 30771 px (Rueckseiten der Blaetter).
* ROOM1210/1211 obj 0 (Flach-Objekt 268 x 3600 x 1890 @0x01E4E) Cut 5: 9463 -> 384 px, Cut 8: 2832 -> 1381 px.
Jede dieser Zahlen folgt aus denselben drei Schranken, die am Original zweimal Polygon fuer Polygon bestaetigt sind
(20A0 Flach-Zeichner 48/48, 1050 Licht-Zeichner 15/15).
**Port-Modelle** (eingebacken, nicht aus der RDT: Sicherung, Granate, Irons-Tisch, Dokumente, Inhalt R35, Schluessel
R36, Kartenitems R36, Feuerloescher R37): ~~bleiben zweiseitig (kein NCLIP, PORT-WAHL — der Feuerloescher hat 112 von
148 Polygonen verkehrt herum gewickelt und waere sonst zerrissen)~~ **ueberholt 2026-10-17 (§9.3): NCLIP fuer alle
Objekte; Feuerloescher von Spur A neu gewickelt, Parking Lot Key von Spur A gewendet;** OTZ < 64, Saettigung und
GPU-Grenze gelten auch fuer sie (wirken nur unmittelbar vor der Kamera).

## OFFEN

* Vorbestehend, nicht Teil dieses Befunds: der PC teilt ein voll gezeichnetes Viereck an der Diagonale 0-3, die PSX-GPU
  an 1-2 (gleiche Flaeche, andere affine Texturverzerrung bei stark perspektivischen Vierecken). Der Halbfall (nur ein
  GPU-Dreieck besteht) nutzt schon die PSX-Teilung.
* Figuren (Spieler/Gegner/NPC) laufen im PC NICHT durch diese Schranken; im Original zeichnet derselbe Licht-Viereck-
  Zeichner FUN_800256b0 auch Leons Koerperteile (Messung §4b: 16 Aufrufe je Bild). Kein Nutzer-Befund dazu — nicht
  angefasst.
* ~~Feuerloescher-Modell (gen/feuerloescher_r37.inc, Runde 37 Spur I / Runde 38 Spur K): Windung uneinheitlich; mit
  einheitlicher Windung koennte auch er NCLIP bekommen. Hinweis an Spur K, keine Aenderung hier (Datei gehoert K).~~
  Erledigt durch Spur A (§3b, neu gewickelt), mit dem Merge in diesem Zweig (§9).
* Runde-36-Satz "die Kiste in ROOM20A0/20A1 faellt nie" (A_klettern.md) ist durch den Rolladen ueberholt: sub07
  @0x01E5E-6E schaltet Zelle 32 (die "Wand 32" der Runde 36) in allen Regionen auf u0 = 0, danach faellt die Kiste
  durch die Oeffnung (§6, Lauf C). Die Lage (3,0xCC)=1 @0x0187A verlangt dann noch die Kerbe 7 (Member_cmp m0F == 7
  @0x01ADC) unten am Kanal — nicht Teil dieses Befunds, nicht gemessen.
* Nachbesserung 1 — vorbestehend, nicht Teil des Befunds: der Typ-3-Zeichner FUN_8003e64c verwirft nach dem Zeichnen
  einzelne Kacheln per FUN_80014368 (Tag-Laenge 0 / 0x0C, Decompilat FUN_8003e64c.c); der Port zeichnet alle Kacheln
  (re15_prop_culled Typ 3 -> 0; Runde 36 Spur N 8.8). Mit dem subtraktiven Wasser (ABR 2) ist eine zu viel gezeichnete
  Kachel dunkler statt deckend grau. In den gemessenen Schnitten (20A0 Cut 2 und 3) stimmt das Bild mit dem Original
  (MAD 0.3 / 1.1, §10.6) — dort fiel keine Kachel ab.
* Nachbesserung 1: ROOM11B0/11B1 Kiste (obj 0, einziges Objekt mit gemischten STP-Texeln) ohne Original-Bild — der
  Debug-Sprung haelt dort die Kamera fest (§10.6); Mechanik im Riegel A3 an genau dieser Textur geprueft.

## Fuer den Nutzer

* ROOM20A0 (Leon) und ROOM20A1 (Elza): der geoeffnete Rolladen steht nicht mehr vor der Kamera. Im Kamerabild ueber
  der Plattform (das beim Gang zur Kiste und beim Schieben aktiv ist) sieht man jetzt Kiste, Faesser, Regal und die
  Figur — wie im Original (dort verwirft der Zeichner die Rueckseiten und die zu nahen/zu grossen Flaechen). Der
  Kisten-Ablauf bleibt: Rolladen auf, Kiste nach Norden durch die Oeffnung schieben, sie faellt.
* Dieselbe Original-Regel gilt jetzt fuer alle Raum-Objekte: Rueckseiten von Toren/Rolllaeden werden nicht mehr
  gemalt, wo die Kamera von hinten draufschaut (z. B. ROOM10D0 Cut 13 am Anfang der Marvin-Szene, ROOM1050 Cut 5,
  ROOM1130 Cut 8/9). Das gilt (zusammen mit Spur A, Cursor-Schatten) auch fuer die eingebauten Port-Modelle
  (Sicherung, Dokumente, Schluessel, Feuerloescher ...); sie sehen aus wie bisher — der Feuerloescher wurde dafuer von
  Spur A neu gewickelt (gleiches Bild), der Parking Lot Key in ROOM10F0 liegt jetzt mit der Vorderseite nach oben
  (wie in RE2; vorher war seine Rueckseite zu sehen).
* Nachbesserung 1 — HALBTRANSPARENTE Raum-Objekte wie im Original (das Original mischt sie mit dem Hintergrund, der
  Port zeichnete sie bisher deckend). Sichtbar anders als in v0.8.25, jeweils gegen das Original-Bild gemessen (§10.6):
  - ROOM5060/5061 (Labor): die Glasscheiben dunkeln das Fenster zur Haelfte ab (vorher: Scheibe fehlte bzw. nach dem
    Rolladen-Fix ein deckend dunkles Dreieck — der Abnahme-Befund); die Licht-Bodenplatte (obj 5) leuchtet rot auf den
    Boden wie im Original (vorher eine graue deckende Platte).
  - Die Typ-3-Wasserflaechen in 11A0, 1260 und Stage 2 (2000/2020/2040/2050/2070/2080/2090/20A0/20B0) werden wie im
    Original vom Hintergrund ABGEZOGEN (fast schwarzer Kanal, in 20A0 Cut 3 das Sprenkelmuster des Originals) statt
    als graue deckende Flaeche gemalt.
  - ROOM4090 (Kuehlraum): der Bodennebel ist hell und durchscheinend wie im Original — v0.8.25 zeigte dunkle deckende
    Schlieren ueber dem halben Bild.
  - ROOM1040, 5030 (Glas/Roehren), 11B0 (die Kiste: in Texturhelligkeit wie im Original, einzelne Texel halbtransparent).
  - ROOM20A0/20A1: die vom Port aktivierte "schlafende" Effekt-Schleife am Kanalende (sub02, Auftrag 2026-08-30) liegt
    unter dem Wasser und scheint jetzt als gruene Flecken durch; vorher deckte das falsch deckende Wasser sie zu. Das
    Original startet die Schleife nie (gemessen: 0 aktive Effekte); die Flecken sind also Port-Inhalt (die Aktivierung),
    der Zeichenweg darunter ist der des Originals.
* Neue Assets: keine (Paket-Gate unveraendert). Sprachdateien: keine. Bank-9-Bits/Nachrichten-Ids: keine.
* Bau: neue Quelldateien `re15_port/engine/src/objpoly_r38.c`, `re15_port/platform/pc/src/objpoly_r38_pc.c`,
  `re15_port/engine/src/objabe_r38.c`, `re15_port/platform/pc/src/objabe_r38_pc.c` (GLOB; mit dem Merge von Spur A dazu
  `re15_port/engine/src/nclip_r38.c`) — der Android-Bau braucht einen frischen Configure (Memory
  reai-v2-android-glob-cache). render_pc.c bietet `re15_render_pc_renderer()` jetzt auf allen Plattformen an (vorher nur
  Android; android_glue.c nutzt sie unveraendert).
* Messschiene fuer Rueckmeldungen: `RE15_OBJPOLY_LOG=objpoly.txt` schreibt je Bild und Objekt gezeichnet/NCLIP/OTZ/GPU.
* Entscheidungsfragen: keine.

## 9. Fortsetzung 2026-10-17: Merge r38/cursor (Spur A) — EIN NCLIP-Weg, EINE Regel fuer Port-Modelle

Auftrag (Orchestrator 2026-10-17 05:11): Spur A (r38/cursor, Cursor-Schatten 1150/11F0) baut denselben Rueckseiten-
Verwurf (GTE NCLIP, MAC0 < 0) in der Raum-Prop-Schleife und am Hebetisch-Cursor; A wickelt Port-Modelle um, W zeichnete
sie zweiseitig. Zweig r38/cursor (Stand c4182ed1, 2026-10-17 05:11) in diesen Zweig gemergt: **2291b2c5**.

### 9.1 Was beide Zweige trugen

| | Spur A (r38/cursor) | Spur W (bis 2e2f9579) |
|---|---|---|
| Prop-Schleife main.c | alte Projektion (Scheitel `_vz < 64`, ungesaettigt) + `re15_nclip_r38_verwerfen` nach Dreieck/Viereck | objpoly_r38 (RTPS gesaettigt, NCLIP, OTZ < 64, GPU-Groessengrenze, Halbviereck) statt der alten Projektion |
| NCLIP-Rechnung | include/re15_nclip_r38.h + engine/src/nclip_r38.c (psx-spx woertlich, saettigt -400h..3FFh, Zaehler) | eigene `nclip_opz` in objpoly_r38.c (gleiche Formel auf gesaettigten SXY) |
| Port-Modelle (eingebacken) | NCLIP wie RDT-Modelle; Feuerloescher R37 dafuer neu gewickelt (gen/feuerloescher_r37.inc, 112 Vierecke v1<->v2) | zweiseitig (PORT-WAHL `r38_nclip` = MD1 im RDT-Puffer) |
| Hebetisch-Cursor 1150 | NCLIP (hebetisch_cursor_pc.c) | - |

Konflikt nur in main.c (zwei Stuecke: Dreiecks- und Vierecks-Projektion der Prop-Schleife).

### 9.2 Aufloesung (2291b2c5)

* **EIN NCLIP-Weg.** Prop-Schleife = W-Weg objpoly_r38 (er enthaelt As Verwurf und dazu OTZ < 64, Saettigung, GPU-Grenze).
  Die NCLIP-Rechnung selbst ist As Funktion: objpoly_r38.c ruft `re15_nclip_r38_mac0` (OPZ fuer Messschiene/Riegel) und
  verwirft ueber `re15_nclip_r38_verwerfen` (MAC0 < 0; Zaehler `g_re15_nclip_r38_verworfen` zaehlt damit wie bei A jeden
  Verwurf der Prop-Schleife). As Saettigung ist auf den schon gesaettigten SXY wirkungslos (gleiches Ergebnis).
  Original-Adressen unveraendert: FUN_80022f0c `bltz` @0x80023010, FUN_80023098 `bgez` @0x80023190, FUN_800254a0
  `bgez` @0x800255a8, FUN_800256b0 `bgez` @0x800257c0; OTZ `srl/sra 6` + `beq` @0x80023044/48, @0x80023224/28,
  @0x80025654/58, @0x800258b0/b4; ZSF3 341 @0x80066c70, ZSF4 256 @0x80066c7c.
* Hebetisch-Cursor (As Modul hebetisch_cursor_pc.c) unveraendert mit As Verwurf — derselbe NCLIP-Code.
* objpoly_r38: Parameter `nclip` entfernt (main.c, Riegel angepasst). As Include in main.c bleibt (unbenutzt, haelt
  den spaeteren Merge der Folge-Commits von A konfliktarm).

### 9.3 Port-Modelle: EINE Regel = NCLIP fuer jedes Objekt-Polygon (Wahl mit Beleg und Messung)

**Beleg (Original kennt keinen zweiseitigen Objekt-Zeichner):**
* RE1.5: FUN_8002c18c ruft fuer jedes Objekt nur die vier Zeichner aus §3.1 (Aufrufliste A §2.1: @0x8002c320/350/380/3b0,
  Typ 3 ueber FUN_8003e64c @0x8002c2c8 auf dieselben zwei Flach-Zeichner) — alle mit NCLIP-Verwurf.
* RE2 Retail (Ziel, wo RE1.5 unfertig ist; die Port-Modelle sind RE2-Items bzw. Nachbauten), `info/re2leon/PSX.EXE`
  mit re2_disasm.py selbst nachgeprueft: Objekt-Schleife FUN_800363f0 ruft FUN_8002d0e8 @0x80036610, FUN_8002d244
  @0x80036640, FUN_80027bec @0x8003666c, FUN_80027dbc @0x8003669c:
  - FUN_8002d0e8: RTPT `4a280030` @0x8002d170, NCLIP `4b400006` @0x8002d18c, `swc2 MAC0` `ebb80000` @0x8002d1a0,
    `lw v0,0(sp)` @0x8002d1a4, `bgez v0,0x8002d1bc` @0x8002d1ac, sonst `j 0x8002d148` + `addiu t0,t0,80` (naechste Flaeche)
  - FUN_8002d244: NCLIP @0x8002d2f4, `bgez v0,0x8002d324` @0x8002d314, sonst `j 0x8002d2a4` + `addiu t0,t0,104`
  - FUN_80027bec: NCLIP @0x80027ca8, `bgtz v0,0x80027ce4` @0x80027cc8
  - FUN_80027dbc: NCLIP @0x80027e64, `bgez v0,0x80027eac` @0x80027e84, sonst `addiu t1,t1,104`
  (FUN_8002d3c8 @0x80036588 / FUN_8002d718 @0x800365b4 ohne NCLIP sind der Sonderweg Attribut & 0x8 mit dem
  30-Bilder-Zaehler obj+492 @0x800365bc..d0 — kein Item-/Kulissen-Objekt.)
**Messung (warum die PORT-WAHL "zweiseitig" entfaellt):**
* Ihr einziger Grund war der verkehrt gewickelte Feuerloescher R37 (W-Zensus §7: 36 von 148 Polygonen in Original-
  Windung). Spur A hat ihn an der Quelle neu gewickelt (A_cursor.md §3b) und gemessen: mit NCLIP + neuem Modell
  **0 Bildpunkte** Unterschied am Loescher gegen den Stand ohne Verwurf (ROOM1050 Cut 6 Skala 1 und 3, ROOM1090 Cut 5;
  A §4.5).
* A-Scan aller 120 Leon- und 75 Elza-Raeume mit NCLIP fuer alle Objekte: nur Einzelpunkte (<= 41) geaendert (A §4.4);
  der einzige echte Ausfall ist der Parking Lot Key ROOM10F0 (A §4.6) — ein LAGE-Fehler der Runde 36 Q (flaches
  RE2-Modell mit der Rueckseite nach oben, RE2 ROOM6150 legt ihn gewendet, Dir z 0x600). Zweiseitig zeigt ihn der Port
  heute gespiegelt (Rueckseite), mit NCLIP gar nicht. Behebung = Lage (A §3c bzw. Spur K, die beide Schluessel an die
  Pinnwand haengt; A hat Ks Lage nachgerechnet: Vorderseite zur Kamera) — siehe OFFEN 9.6.
* Damit gilt ab 2291b2c5: **NCLIP fuer alle Objekte** (RDT- und Port-Modelle, Prop-Schleife und Hebetisch-Cursor).
  Riegel: unit_r38_rolltor20a0_windung O2 (jedes Port-Modell in Original-Windung, jetzt PRUEFUNG statt Ausgabe) + O4
  (der Verwurf laeuft ueber re15_nclip_r38), As unit_r38_cursor_wicklung (Wicklung + Mechanik an 60 Blicken).

Weitere Merges von r38/cursor (konfliktfrei): **0a04fd5b** (A cd41f6b2: Parking Lot Key gewendet `rot_x 2048`
nach RE2 ROOM6150 @0x02C98, Riegel fuer flache Port-Modelle E/F) und **5551ce9c** (A f3e30a33, nur As Dossier). Danach
O2 auf As Regel gebracht (7e3371ae): bewertet werden nur RAEUMLICHE Port-Modelle; FLACHE (alle Umlauf-Normalen
parallel, Kriterium wie As probe_r38_cursor.c flach_fn) haben nur eine Seite, die ihre LAGE zur Kamera dreht — die
prueft As exe-Riegel integration_r38_cursor E/F (10F0/10F1).

### 9.4 Messung nachher (nach dem Merge, exe dieses Zweigs)

Bau `local_build.sh configure` + `build` nach 2291b2c5 -> `=== LOCAL-BUILD-OK (configure)` / `(build)`; nach 0a04fd5b
und 7e3371ae je `build` -> `=== LOCAL-BUILD-OK (build)`. Keine neue Warnung in objpoly/nclip/r38-Dateien (die drei
`-Wdiscarded-qualifiers` in main.c Z. 8739/10041/10421 liegen ausserhalb der Prop-Schleife und sind vorbestehend).

**ROOM20A0/20A1 Cut 11 (Nutzer-Befund), exe-Riegel integration_r38_rolltor20a0, Lauf lauf_21debfa5** (echter Weg:
Leiter -> Schalter -> Ja -> Rolladen hoch -> Sueden in Cut 11):

| Lauf | Eintritt | Cut-11-Bilder F700..F1100 (je 50): Raumhintergrund-Anteil | OBJPOLY Cut 11 obj 1 |
|---|---|---|---|
| A Leon ROOM20A0 | Debug-Sprung vor die Leiter | 0,9424 / 0,9424 / 0,9429 / 0,9432 / 0,9431 / 0,9431 / 0,9430 / 0,9428 / 0,9424 | 450 Bilder, alle `gezeichnet=6 nclip=20 otz=16 gpu=6` |
| B Elza ROOM20A1 | Spielstand + CONTINUE (Figur 4, PL04) | 0,9488 / 0,9488 / 0,9493 / 0,9492 / 0,9493 / 0,9493 / 0,9493 / 0,9503 / 0,9492 | 450 Bilder, alle 6/20/16/6 |
| D Leon ROOM20A0 | Spielstand + CONTINUE | 0,9424 / 0,9424 / 0,9426 / 0,9426 / 0,9427 / 0,9427 / 0,9428 / 0,9420 / 0,9426 | 450 Bilder, alle 6/20/16/6 |

Vorher (Basis df08d83a, §1/§6): 0,1305 (Bild voller Lamellen). 6/20/16/6 = Original-Messung (§4: 20 NCLIP, 16 OTZ<64,
6 eingehaengte Vierecke mit beiden GPU-Dreiecken zu gross). Die Zahlen sind bitgleich mit dem Stand vor dem Merge (§7:
A 0,9424/0,9432/0,9430, B 0,9488/0,9493/0,9493) — fuer RDT-Modelle aendert der Merge nichts (As Verwurf ist im
W-Weg schon enthalten). Bilder: `W_belege/nachher_merge_leon20A0_cut11_F850.png`,
`W_belege/nachher_merge_elza20A1_cut11_F850.png` — Plattform mit Kiste, Faessern, Regal und Figur, kein Rolladen; neben
dem Original-Bild `W_belege/orig_cut11_offen_buf0.png` angesehen: gleiche Ansicht.
Rolladen-Lage: Wegplan-Log `obj1=(7942,-10800,-20510)` = Original-Endlage (DuckStation §4: Pool 1 (7942,-10800,-20510)).
**Kiste (Lauf C)**: `F1259 room=20A0 p=0 AUSLOESER wort=0x4206 band=4 pos=(12250,-7200,-21473)`, `F1278 ... LANDUNG
y=-3600 band=2`, `F1280 ... AUSLOESER wort=0x2102 band=2`, `F1297 ... LANDUNG y=-1800 band=1` — faellt durch die
Rolladen-Oeffnung wie vor dem Merge (§7).

**ROOM1050 Cut 5 (zweiter Original-Fall, §4b)** — Lauf `W_belege/lauf_merge_1050_20a0.sh` m1050_cut5: Debug-Sprung,
Spieler an der Original-Messstelle (16050,0,-16200) (orig1050_log.txt "Zone 4->5"), Zustand `F400 ... cam=5`.
Messschiene: obj 0 (Rolltor, Obj_model_set @0x00C36) in **402 von 402 Bildern `gezeichnet=0 nclip=15 otz=0 gpu=0`** =
Original (alle 15 Vierecke OPZ < 0 am Haltepunkt `bgez` @0x800257c0). Nebenbei obj 5 = Port-Feuerloescher (148
Flaechen) `otz=148`: er haengt hinter der Kamera von Cut 5 (SZ auf 0 gesaettigt -> OTZ 0) — vorher wie nachher nicht im
Bild (alter Weg: `_vz < 64`).

**ROOM20A0 Cut 2 (Hinweis Spur A §4.4, Rolllaeden obj 4/5 Typ 3)** — Lauf m20a0_cut2 (Debug-Sprung, Cut 2 erzwungen,
F400): obj 4 `gezeichnet=18 nclip=0 otz=6 gpu=2`, obj 5 `gezeichnet=0 nclip=30`. sub01 setzt in Cut 2/3 obj 4 auf
y -2376 und obj 5 auf -16200 (`Cmp(var0A op0 2)` @0x01B18 -> `Member_set m=0x01 val=-2376` @0x01B22, `val=-16200`
@0x01B2A; Cut 3 @0x01B46/@0x01B50/@0x01B58). Typ 3 zeichnet das Original ueber FUN_8003e64c, das dieselben Flach-
Zeichner ruft (`jal 0x80022f0c` @0x8003e6ac, `jal 0x80023098` @0x8003e6dc) — also dieselben Schranken wie fuer obj 1.
Gegenueber As Stand (nur NCLIP) faellt dort zusaetzlich weg, was das Original an OTZ < 64 (6) und an der GPU-Grenze (2)
verwirft. Kein Nutzer-Befund zu Cut 2, kein eigener DuckStation-Abzug (OFFEN 9.6).

### 9.5 Tests (nach dem Merge)

Eigene Riegel + As Riegel (exe nach 0a04fd5b):
`ctest --test-dir re15_port/build -R "^(unit_r38_(rolltor20a0(_windung)?|cursor_(nclip|kasten|raeume|wicklung|sicht))|integration_r38_(rolltor20a0|cursor))$" --timeout 1500 --output-on-failure -V`
-> **`100% tests passed, 0 tests failed out of 8`** (Total Test time (real) = 416.09 sec; integration_r38_cursor
171,42 s mit A..F: Klassenkarte 11F0/11F1 0 Abweichungen gegen das Original, 0 rueckseitige Cursor-Dreiecke,
Schluessel 10F0/10F1 vorderseitig; integration_r38_rolltor20a0 244,36 s, Werte §9.4).
unit_r38_rolltor20a0_windung nach 7e3371ae direkt: `O2: 20 port-eigene Modelle geprueft (9 flach, nicht bewertet), 0
raeumliche nicht in Original-Windung`, `O4 NCLIP-Weg: Vorderseite opz 11025 -> 1 (Zaehler +0), Rueckseite opz -11025
-> 0 (Zaehler +1)`, OK. Gegenprobe O2: der Stand vor dem Merge (2e2f9579, alter Feuerloescher) meldete den Loescher mit
36 von 148 in Original-Windung (§7) — O2 haette dort angeschlagen.

Testgruppen der beruehrten Module, seriell, exe nach 7e3371ae (Diff gegen master: main.c Prop-Schleife = alle Bild-/
Lade-Riegel mit Raum- UND Port-Objekten, weil NCLIP jetzt auch fuer Port-Modelle gilt; aus As Diff
hebetisch_cursor_pc.c (Hebetisch/Panels/Cursor), gen/feuerloescher_r37.inc (Loescher), schluessel_r36.c/.h
(Schluessel); Kisten-Riegel mit Bild; eigene + As Riegel):
```
ctest --test-dir re15_port/build -R "^(integration_(r30_irons_tisch_bild|r30_irons_tisch_licht|r30_irons_tisch_laden|r30_sicherung_bild|r30_sicherung_laden|r30_granate_laden|r34n_e_dokumente_bild|r34n_g_schrift1150|r34n_b_cursor|r36_kartenitems_bild|r36_schluessel|r36_szene1150|r35_cut1150|r32_tor_hell|r37_feuerloescher|r37_codepanel|r37_kiste|r37_kiste2feld|r37_kistenregel|r38_cursor|r38_rolltor20a0)|unit_r38_(cursor_[a-z]+|rolltor20a0|rolltor20a0_windung)|unit_(sicherung_1150|prop_texel_key_11f0|r30_irons_tisch|r30_irons_tisch_licht|r30_sicherung_[a-z]+|r31_hebetisch|r32_hebetisch_faecher|r34n_b_[a-z_]+|r34n_e_dokumente|room1050_sicherung|r36_kartenitems|r36_objfall|r36_schluessel_[a-z]+|r37_codepanel_[a-z]+|r37_feuerloescher_[a-z]+))$" --timeout 1500 --output-on-failure
-> 100% tests passed, 0 tests failed out of 67
   Total Test time (real) = 2956.64 sec
```
Darunter 21 exe-Riegel: r30_granate_laden 75,7 s, r30_irons_tisch_laden/_bild/_licht 30,5/52,5/42,1 s,
r30_sicherung_laden/_bild 36,5/13,4 s, r32_tor_hell 5,8 s, r34n_b_cursor 92,8 s, r34n_e_dokumente_bild 118,4 s,
r34n_g_schrift1150 57,0 s, r35_cut1150 527,5 s, r36_kartenitems_bild 444,4 s, r36_schluessel 94,0 s, r36_szene1150
117,9 s, r37_codepanel 79,2 s, r37_feuerloescher 323,0 s, r37_kiste/_kiste2feld/_kistenregel 59,1/40,0/295,3 s,
r38_cursor 186,8 s, r38_rolltor20a0 262,0 s. Kein Flattern, kein Nachfahren noetig.
Nicht im Lauf: integration_r36_dachkiste (1716 s; prueft nur Kisten-Logs, kein Bild — weder der Merge noch objpoly
beruehren Kisten-Code; am selben Kisten-Code 2026-10-16 gruen, §7b).

### 9.6 OFFEN (nach dem Merge)

* Integration: r38/cursor ist bis f3e30a33 enthalten. Spaetere A-Commits mergt der Orchestrator nach; aendert A die
  Prop-Schleife in main.c noch einmal, gilt weiter EIN Weg (objpoly_r38 -> re15_nclip_r38), kein zweiter Verwurf.
* Spur K (r38/itemorte) haengt die Schluessel an die Pinnwand (schluessel_r36.c/.h): nach As §8 gilt dort K's Lage;
  As Rechnung zeigt K's Lage vorderseitig, integration_r38_cursor E/F prueft das an der exe nach dem K-Merge. Jedes
  NEUE flache Port-Modell muss mit der Vorderseite zur Kamera liegen (As §8), sonst ist es mit dem Verwurf unsichtbar.
* ROOM20A0 Cut 2 (obj 4/5): kein DuckStation-Abzug; Mechanismus ueber FUN_8003e64c belegt (§9.4), kein Nutzer-Befund.
* Vorbestehend (unveraendert): Viereck-Teilung 0-3 statt 1-2; Figuren laufen nicht durch diese Schranken (OFFEN oben).

## 10. Nachbesserung 1 (2026-10-17, nach Abnahme 0)

Abnahmebericht `analysis/befunde_runde38/W_abnahme_0.md` (HEAD d6a55686): Nutzer-Punkt **erfuellt** (echter Tuerweg Leon
2090 -> 20A0 und Elza 2091 -> 20A1, §3/§4 dort); nicht bestanden wegen zweier Maengel:
* **M1** ROOM5060/5061 Cut 4: der neue Weg zeichnet ein Halbviereck des Glas-Objekts obj 4 (Attribut 0x0012) DECKEND
  dunkel; das Original mischt es halbtransparent (ABE). Der PC-Prop-Weg kennt kein ABE (vorbestehend).
* **M2** Nebenwirkungs-Zensus nur in eine Richtung (WENIGER), ohne Typ 3 und ohne Eltern-Objekte.

### 10.1 M1 — Ursache

Der Original-Polygon-Weg (§5) zeichnet in ROOM5060/5061 Cut 4 von obj 4 (Obj_model_set @0x02D72, Attribut **0x0012**,
(-19548,-3780,-23346)) das GPU-Dreieck (1,2,3) des Vierecks P01, dessen Scheitel 0 bei vz 21 liegt; der alte
Port-Ausschnitt `_vz < 64` hatte das ganze Viereck verworfen. Die GEOMETRIE ist die des Originals (Abnahme 0 §5.2b,
nachgerechnet). Falsch war das ZEICHNEN: der PC-Prop-Weg kannte kein ABE, jedes Objekt-Polygon ging deckend an SDL.
Das ist vorbestehend (seit aa346af1) und Runde 36 Spur N bekannt — sie hat es in main.c ausdruecklich stehen lassen,
"bis zur Portierung von Parameter 4" (analysis/befunde_runde36/N_kartenitems.md 8.7/8.8, N_abnahme_1.md H1). Der neue
Weg hat die Luecke hier nur erstmals an einer Stelle sichtbar gemacht, an der vorher gar nichts gezeichnet wurde.
Entscheidung: Abhilfe (a) der Abnahme — Parameter 4 byte-true portiert (Original oder nicht: das Original mischt, also
ist "deckend" falsch, gleich wo). Damit aendert sich das Bild in ALLEN Raeumen mit halbtransparenten Objekten (§10.4).

### 10.2 M1 — RE-Belege (PSX.EXE Auslieferung, `re15_disasm.py dis`, selbst gelesen)

| Stelle | Bytes/Instruktionen | Bedeutung |
|---|---|---|
| FUN_8002c18c | @0x8002c2ac `lw v0,12(s1)` / @0x8002c2b4 `andi v0,v0,0x2` / @0x8002c2b8 `sltu s2,zero,v0` | ABE = Attribut (Pool+0x0C) & 2 |
| | @0x8002c304 / @0x8002c32c / @0x8002c364 / @0x8002c38c `addu a3,s2,zero` vor `jal 0x80022f0c/0x80023098/0x800254a0/0x800256b0` | Parameter 4 aller vier Zeichner |
| | @0x8002c2bc `ori v0,zero,0x3` / @0x8002c2c0 `bne v1,v0` / @0x8002c2c8 `jal 0x8003e64c` / @0x8002c2cc `addu a1,s2,zero` | Typ 3 bekommt ABE als a1 |
| FUN_8003e64c | @0x8003e66c / @0x8003e6b8 `addu a3,s2,zero` vor `jal 0x80022f0c` @0x8003e6ac / `jal 0x80023098` @0x8003e6dc | Typ 3: nur Flach-Zeichner, mit ABE |
| FUN_80022f0c (Flach-Dreieck) | @0x80022f18 `sll a3,a3,1` / @0x80022f1c `ori a3,a3,0x34` / @0x80022f38 `sb a3,3(a2)`; je Dreieck @0x80023024 `lw v0,0(a2)` / @0x8002302c `sw v0,4(a1)` | Code 0x34 bzw. **0x36** in obj+0x73, Wort 1 des GT3 |
| FUN_80023098 (Flach-Viereck) | @0x800230ac `sll a3,a3,1` / @0x800230b0 `ori a3,a3,0x3c` / @0x800230d0 `sb a3,3(s0)` | 0x3C bzw. **0x3E** |
| FUN_800254a0 / FUN_800256b0 (Licht) | @0x800254ac/@0x800254b0 (`0x34`)/@0x800254d4; @0x800256bc/@0x800256c0 (`0x3c`)/@0x800256ec; RGBC per `lwc2` @0x800254e4 / @0x800256fc | Code-Byte reist mit NCCT ins Primitiv |
| FUN_80025940 (GT3 beim Modell-Bind) | @0x800259bc `lw v0,0(a0)` (a0 = Pool+0x0C) / @0x800259c4 `andi v0,v0,0xc` / @0x800259cc `sll t7,v0,19` / @0x80025a08 `lw v0,-4(t1)` (MD1 u1 v1 Seite) / @0x80025a14 `or v0,t7,v0` / @0x80025a18 `sw v0,-8(a3)` (Wort 6) | Texpage ODER (Attribut & 0xC) << 3 |
| FUN_80025a98 (GT4) | @0x80025b1c / @0x80025b24 / @0x80025b2c / @0x80025b68 / @0x80025b70 / @0x80025b74 `sw v0,-24(a3)` | ebenso |
| FUN_8002b898 | `*(obj+0x70) = 0x808080` (Decompilat Z. 16) | Farbe der Flach-Zeichner = Textur x 1 |
| psx-spx `graphicsprocessingunitgpu.md` | :381 Texpage-Bits 5-6 "0=B/2+F/2, 1=B+F, 2=B-F, 3=B+F/4"; :1129-1132 und :1158-1161 "Color 0000h = Fully-transparent ... 8000h..FFFFh = Semi-transparent (*) or Non-transparent for opaque commands"; :1430-1433 "bit 15 of each CLUT entry acts as a semi-transparency flag ... If the semi-transparency flag is off, the new pixel is written to VRAM as-is"; :1434-1436 Klemmung 255 / 0 | GPU-Regel je Texel |

### 10.3 M1 — Original zur Laufzeit gemessen (DuckStation-GDB, Auslieferungs-EXE `0800e003` @0x80026e4c)

Werkzeug `W_belege/orig_abe_messung.py` (Muster orig_objmessung.py; neu: Gegner schon WAEHREND des Ladens aus — in
ROOM5060 hatte sonst ein Zombie Leon gepackt und jede Lage-Setzung zurueckgenommen; danach Pool-Objekte, Primitiv-
Puffer obj+0x14/+0x1C, Puffer-Index u8 @0x800ACA34, Code-Byte und Texpage je Primitiv; Bildpuffer aus dem
SaveStateOnExit-Spielstand). Logs `W_belege/orig_abe_5060_log.txt`, `orig_abe_20a0_log.txt`, `orig_abe_4090_log.txt`:

| Raum / Cut | Pool | Attribut | Code (Primitiv Byte 7) | Texpage | ABR |
|---|---|---|---|---|---|
| 5060 / 4 | 2, 3, **4** | 0x0012 | 0x3e | 0x0086 | 0 |
| 5060 / 4 | 5 | 0x0006 | 0x3e | 0x00a6 | **1** (aus Attribut-Bit 2) |
| 20A0 / 2 | 4, 5, 6 (Typ 3) | 0x001a | 0x3e | 0x00c7 | **2** (aus Attribut-Bit 3) |
| 4090 / 0 | 0, 1, 2 | 0x000e | 0x3e / 0x36 (Dreiecke) | 0x00e5 | **3** |

obj+0x70 traegt in allen Faellen `80 80 80 3e` (Farbe 0x808080 + zuletzt geschriebener Code). Primitive, die in diesem
Bild noch nie gezeichnet wurden, tragen den Grundcode des Builders 0x34 (z. B. 5060 Pool 2 Q01 — der Zeichner schreibt
Wort 1 erst nach bestandenem NCLIP).

### 10.4 M1 — Zensus aller halbtransparenten Raum-Objekte

`W_belege/zensus_abe.py` -> `zensus_abe.txt` (152 inhaltsverschiedene RDTs, 457 Obj_model_set-Saetze): **74 mit
Attribut & 2**: 0x0012 x37 (ABR 0: 1040/1041 obj 1-3 Glas, 11B0/11B1 obj 0 Kiste, 5030/5031 obj 1-11 Typ 3, 5060/5061
obj 2-4 Glas), 0x001a x32 (ABR 2: alle Typ-3-Wasserflaechen 11A0/11A1, 1260/1261, 2000-20B0), 0x000e x3 (ABR 3: 4090
obj 0-2), 0x0006 x2 (ABR 1: 5060/5061 obj 5). Alle MD1-Seiten sind 0x0080 (ABR-Bits 0) -> das wirksame ABR kommt
allein aus dem Attribut. STP der Texel UNTER den Polygonen (UV-Dreiecke, je Polygon dessen CLUT-Zeile): STP 1
13 668 719, STP 0 91 689, 0000h 704 — STP 0 kommt nur bei der Kiste 11B0/11B1 obj 0 vor (679 STP 1 / 30 563 STP 0):
dort zeigt die GPU 98 % deckend und nur die STP-1-Texel gemischt. (Spur N hatte 20A0 obj 4-6 und 1040 obj 1-3 mit
"0 % STP" gefuehrt — offenbar ueber CLUT-Zeile 0 gezaehlt; die Polygone benutzen Zeile 1 (Wort 0x7840): 20A0 Zeile 1
hat 252/256 Eintraege mit STP, 1040 nutzt dort nur Eintrag 1 = 0x90a1 mit STP; nachgeprueft mit stp_check.py.)

### 10.5 M1 — Umsetzung

* NEU `re15_port/include/re15_objabe_r38.h` + `re15_port/engine/src/objabe_r38.c` (engine, ohne SDL):
  `re15_objabe_r38_abe` (Attribut & 2, @0x8002c2b4), `re15_objabe_r38_abr` (Seite ODER (Attribut & 0xC) << 3, Bits
  5-6, @0x800259c4/@0x80025b24), `re15_objabe_r38_flach` (Attribut & 0x10 oder Typ 3, @0x8002c2f4 / FUN_8003e64c),
  `re15_objabe_r38_mische` (GPU-Formel, Referenz der Riegel), `re15_objabe_r38_texel_klasse`, `re15_objabe_r38_teilen`
  (die Slot-Textur in zwei Teile: STP 0 = deckend, STP 1 = halb; Layout wie re15_render_pc_upload_tim_slot).
* NEU `re15_port/platform/pc/src/objabe_r38_pc.c`: je Raum-Objekt-Slot die zwei Teil-Texturen (nur wenn die TIM STP-1-
  Texel hat); `re15_objabe_r38_pc_dreieck` zeichnet ein ABE-Polygon in ZWEI Durchgaengen — STP-0-Texel deckend (SDL
  BLEND, Alpha FF), STP-1-Texel gemischt: ABR 0 BLEND mit Eckpunkt-Alpha 128, ABR 1 ADD, ABR 2 render_pc.c B-F
  (ONE, ONE, REV_SUBTRACT), ABR 3 ADD mit Alpha 64. Beide Teile sind texelweise disjunkt -> zusammen genau die GPU-
  Regel. Ohne ABE: unveraendert EIN Aufruf re15_render_textured_tri_lit.
* Haken `platform/pc/main.c` (Prop-Schleife + Raum-Objekt-Laden, "Runde 38 Spur W"): Include;
  `re15_objabe_r38_pc_raum_neu()` am Anfang von pc_load_room_prop_set, `re15_objabe_r38_pc_textur()` nach jedem
  Raum-Objekt-TIM-Upload; die drei Dreiecks-Aufrufe der Prop-Schleife gehen ueber
  `re15_objabe_r38_pc_dreieck(attr, uv->page, ...)`; die Flach-Bedingung `(attr & 0x12) == 0x10` (Runde 36 Spur N,
  "nur deckende Objekte, bis Parameter 4 portiert ist") ist jetzt `re15_objabe_r38_flach(attr, typ)` — die
  halbtransparenten Flach-Objekte bekommen wie im Original 0x808080 (Kommentar der Spur N angepasst).
* Haken `platform/pc/src/render_pc.c` ("Runde 38 Spur W"): Include; `re15_render_pc_set_tri_blend` nimmt 0..7
  (3..7 = RE15_OBJABE_BLEND_*); der Flush holt fuer 3..7 die Teil-Textur (`re15_objabe_r38_pc_flush_textur`, NULL =
  Stapel auslassen); `re15_render_textured_tri_lit` nimmt das Eckpunkt-Alpha aus s_tri_alpha statt fest 0xFF (alle
  Setzer stellen es nach Gebrauch auf 255 zurueck: main.c Effekte, glas1120_pc.c, objabe_r38_pc.c — glas1120 und die
  Effekte benutzen ausserdem den unbeleuchteten Weg, der s_tri_alpha schon immer las); `re15_render_pc_renderer()`
  steht nicht mehr nur unter `__ANDROID__` (objabe_r38_pc.c legt Texturen an).
* `objpoly_r38_pc.c` (Halbviereck) zeichnet ueber denselben ABE-Weg (Attribut als neuer erster Parameter).
* Konstanten mit Adresse: 0x0002 (@0x8002c2b4), 0x000C und << 3 (@0x800259c4/@0x800259cc, @0x80025b24/@0x80025b2c),
  0x0010 (@0x8002c2f4), Typ 3 (@0x8002c2bc), Codes 0x34/0x3C | ABE<<1 (Zeichner oben); SDL-Alpha 128 / 64 = psx-spx
  0,5 / 0,25 (Riegel A4: <= 1 Stufe gegen die GPU-Formel).

### 10.6 M1 — Messung vorher / nachher gegen das ORIGINAL-Bild

Bau `local_build.sh configure` + `build` -> `=== LOCAL-BUILD-OK`. Laeufer `scratchpad W38nb1/lauf_n1.sh` (Messkopie
`re15_pc_W38nb1.exe`; v0.8.25 = Paket-exe aus der Abnahme 0), RE15_WINDOW_SCALE=1, Debug-Sprung, Spieler an die Stelle
des Original-Abzugs, Kamera-Schnitt erzwungen; Original-Bildpuffer aus `orig_abe_messung.py` (W_belege/
`orig_abe_5060_cut4_buf0.png`, `orig_abe_20a0_cut2_buf0.png`, `orig_abe_20a0_cut3_buf0.png`, `orig_abe_4090_cut0_buf0.png`).
Mittelwerte RGB einer Messflaeche (Werkzeug `test_r38_rolltor20a0_abe --fall`) bzw. mittlere absolute Abweichung (MAD) je
Kanal gegen das Original:

| Fall (ABR) | Messflaeche | Original | v0.8.25 | Spurbaum vor N1 (deckend) | **nach N1** |
|---|---|---|---|---|---|
| ROOM5060 Cut 4, Glas obj 4 (ABR 0) — der Abnahme-Fall | Halbviereck (1,2,3) (-666,509),(-20,20),(36,142), 1749 px | (31.6,15.3,26.1) | (47.2,30.5,36.6) Abstand 15.6 | (6.3,0.5,6.6) Abstand 25.3 | **(32.2,15.4,26.5) Abstand 0.6** |
| dasselbe, Elza ROOM5061 (PL04, `RE15_PSELECT_AUTO_SWITCH=1`) | dieselbe | (31.6,15.3,26.1) | (47.2,30.5,36.6) | — | **(32.2,15.4,26.5)** (F270/F300/F330) |
| ROOM5060 Cut 4, Abnahme-Maske (Dreieck t7 gegen t8, 1119 px) | MAD | — | 23.9 | 30.6 | **2.3** |
| ROOM20A0 Cut 2, Wasser obj 4 Typ 3 (ABR 2) | Kanal x 160..299 y 180..235, 7840 px | (0.4,0.4,0.3) | (11.8,14.3,11.7) | wie v0.8.25 | **(0.4,0.4,0.3)**; MAD 12.8 -> 0.3 (13 200 px Kanal) |
| ROOM20A0 Cut 3, Wasser obj 6 (ABR 2) | Kanal x 140..249 y 180..235 | (7.1,5.8,6.4) | (11.9,14.6,11.9) MAD 12.4 | wie v0.8.25 | **(6.8,5.5,6.0) MAD 1.1** (das Sprenkelmuster des Hintergrunds minus Wasser stimmt Pixel fuer Pixel) |
| ROOM4090 Cut 0, Nebel obj 0-2 (ABR 3) | Boden x 20..299 y 160..235, 21 280 px | (140.5,139.4,147.9) | (56.5,55.7,55.1) MAD 85.2 (36 030 px dunkle deckende Schlieren) | wie v0.8.25 | **(140.6,138.9,147.2) MAD 3.3** |
| ROOM5060 Cut 3, Licht-Bodenplatte obj 5 (Attribut 0x0006, ABR 1 = B + F) + Glas | Pixel, in denen v0.8.25 und N1 abweichen (8595 px) | (106.8,44.8,49.3) | (32.0,30.2,30.7) MAD 37.6 (graue deckende Platte) | wie v0.8.25 | **(106.4,45.5,50.9) MAD 3.3** (rotes Licht auf dem Boden wie im Original) |

Bilder (W_belege): `n1_5060_cut4_orig_v0825_neu.png`, `n1_20a0_cut2_orig_v0825_neu.png` (Port-Laeufe hier noch mit
der angreifenden Spinne), `n1_20a0_cut3_orig_v0825_neu.png`, `n1_4090_cut0_orig_v0825_neu.png` (je Original | v0.8.25 |
nach N1), `n1_11b0_cut5_v0825_neu.png` (v0.8.25 | nach N1); Laeufer `W_belege/lauf_n1.sh`. ROOM5060: das linke Fenster zeigt wie im Original das
Gitter halb abgedunkelt durch die Scheibe. ROOM4090: v0.8.25 zeigte die drei Nebel-Modelle als dunkle deckende
Schlieren ueber dem ganzen Boden, nach N1 heller Bodennebel wie im Original. ROOM20A0: der Kanal wie im Original fast
schwarz (B - F), Cut 3 mit dem Sprenkelmuster des Originals.
ABR 1 (5060 obj 5, 0x0006): Laufzeit (Texpage 0x00a6) in Cut 4 und Cut 3 (W_belege/orig_abe_5060_cut3_log.txt),
Bild Cut 3 `orig_abe_5060_cut3_buf0.png` / `n1_5060_cut3_orig_v0825_neu.png` (Spieler (-23375,0,-23075), Port-Lauf mit
`RE15_FORCE_CUT=3`; der Zombie unten rechts ist im Port aktiv, im Original-Abzug ausgeschaltet).
ROOM11B0/11B1 Kiste obj 0 (einziges Objekt mit STP-0- UND STP-1-Texeln): im Port gemessen (Lauf m11b0, Cut 5) —
heller als v0.8.25 (Flach-Zeichner 0x808080 statt NCCT, wie FUN_80023098), 679 Texel halbtransparent; eine Original-
Gegenprobe war per Debug-Sprung nicht moeglich (11B0 haelt nach dem Sprung die Kamera fest — Zonen-Kette ohne
Schnittwechsel, zwei Versuche `orig11b0`/`orig11b0b`). Die Regel je Texel ist die GPU-Regel (psx-spx :1430-1433),
im Riegel A3 an genau dieser Textur geprueft.

**Port-eigener Inhalt, jetzt sichtbar (ROOM20A0/20A1):** der Port startet beim Betreten die "schlafende" Ambient-Effekt-
Schleife sub02 (@0x1BF0-0x1CB0, 11x Sce_espr_on Effekt-Id 6; Aktivierung scd_room_setup.c, Nutzer-Auftrag 2026-08-30
"schlafender Content"). Ihre Sprites (TIM-Slot 36, z 17 462..17 998, RE15_TRILOG) liegen UNTER dem Wasser (Wasser-
Polygone z 10 052..14 018). Das deckende Wasser von v0.8.25 deckte sie zu; das subtraktive Wasser (B - F) laesst sie als
gruene Flecken am Kanalende durchscheinen. Das Original startet die Schleife nie: ESP-Pool @0x800A73B8 (96 x 0x84,
Flags an Platz+0x6C, FUN_80053240 @0x800532f4) in Cut 2 dreimal gelesen = **0 aktive Plaetze**
(W_belege/orig_abe_20a0_esp_log.txt). Das ist die Folge der Port-Aktivierung, nicht des Zeichenwegs -> "Fuer den Nutzer".

### 10.7 M2 — Nebenwirkungs-Zensus in BEIDE Richtungen, mit Typ 3 und Eltern-Objekten

`W_belege/zensus_beidseitig.py` -> `zensus_beidseitig.txt` (baut auf zensus_objpoly.py auf): jedes Obj_model_set
(alle Skript-Lagen) aller 152 inhaltsverschiedenen RDTs, **Typ 3 in jedem Cut** (der Port zeichnet sie ohne Objekt-
Ausschnitt, re15_prop_culled), **Eltern-Objekte** (pc[5] = 0xC0: Weltmatrix = Eltern x lokal, LAB_80040914 @0x80040a84,
FUN_80022da0 @0x8002c22c; gerechnet: ROOM1150/1151 obj 1/2), pc[5] = 0x80 (Figuren-Anhang ROOM4030/4031 obj 0, im Port
nicht umgesetzt) gemeldet: 2499 Kombinationen Objekt x Cut (54 Typ-3-Saetze, 4 Eltern-Saetze).

* **MEHR (NEU > ALT + 300 px): 3** — ROOM1170 obj 3 Cut 0 (Hubschrauber-Rotor, statisch 0 -> 11 338 px), ROOM5060/5061
  obj 4 Cut 4 (1849 px = M1). Am Spiel neu gegen v0.8.25:
  - 1170: New-Game-Intro mit der exe NACH N1 (`RE15_PSELECT_AUTO=1 RE15_FRAMEDUMP=0-1400/5 RE15_EXIT_AT=1400#1170`,
    Lauf n_intro1170) gegen v0.8.25 (Abnahme t4_intro1170_alt): 146 von 281 Bildern verschieden, **hoechstens 96 px**
    (nur Rotorkanten; der Rotor dreht, die statische Ruhelage tritt nicht auf); gegen den Stand vor N1 (t3): **0 von
    281 Bildern verschieden** — N1 aendert 1170 nicht (kein Objekt mit Attribut & 2).
  - 5060/5061: §10.6 — nach N1 = Original (Abstand 0.6), Leon und Elza.
* **WENIGER: 22** — die 18 Faelle aus §8 plus **4 Typ-3-Faelle**: ROOM2050 obj 4 Cut 11/12/13 (statisch 12 674 /
  4878 / 4038 -> 0 px, Rueckseiten von unten), ROOM20A0 obj 6 Cut 3 (1906 -> 0). Am Spiel mit der exe nach N1:
  2050 Cut 11 (`RE15_DEBUG_JUMP=2050@250 RE15_FORCE_CUT=11`, Lauf n_2050_c11) gegen v0.8.25 **82..89 px** (Kanten; das
  Skript setzt obj 4 je Cut um), gegen den Stand vor N1 **0 px**; 20A0 Cut 3 (n_20a0_c3) gegen v0.8.25 1134 px — das
  ist das Wasser (ABR 2, §10.6: jetzt = Original, MAD 1.1), nicht der Polygon-Weg (vor N1: 0 px, Abnahme 0 §5.2c).
* Eltern-Objekte: kein Fall ueber den Schwellen.
* **HALBTRANSPARENT sichtbar: 364 Faelle** (Objekt x Cut mit NEU > 0 px, Attribut & 2) in 1040/1041, 11A0/11A1,
  11B0/11B1, 1260/1261, 2000/2001, 2020/2021, 2040/2041, 2050/2051, 2070/2071, 2080/2081, 2090/2091, 20A0/20A1,
  20B0/20B1, 4090/4091, 5030/5031, 5060/5061 — dort zeigt der Port seit N1 die Original-Mischung statt einer deckenden
  Flaeche (gemessen an 5060/5061, 20A0 Cut 2 und 3, 4090: §10.6).

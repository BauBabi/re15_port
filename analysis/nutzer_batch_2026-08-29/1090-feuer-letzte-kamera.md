# ROOM1090, letzte Kamera — "irgendein Zeug ueber den Feuern"

Recon-Bericht 2026-08-29. Alles unten selbst gemessen (RDT geparst, PSX.EXE/STAGE1.BIN
disassembliert, `stage_saves/room1090_orig.sav` gelesen, Port-Code gelesen). Kein Build, keine Edits.

## 0. Ergebnis in einem Satz

Die EM26-Dreiecke sind mit `no_draw` korrekt weg (statisch + dynamisch bestaetigt, Abschn. 3);
das verbleibende "Zeug ueber den Feuern" kommt mit hoher Wahrscheinlichkeit aus dem
**Flammen-/Funken-Effektpfad des Ports**, der die Row-VM der drei Feuer-Effekte umgeht und
dadurch **drei belegte Divergenzen** traegt: (D1) OPAK statt ADDITIV gezeichnet, (D2) Position
am Spawn-Ort eingefroren statt dem ~940 Einheiten HOCHGEFAHRENEN Emitter zu folgen, (D3) Groesse
statisch statt oszillierend/mitwachsend. In der letzten (nahesten) Kamera sind alle drei maximal
sichtbar. Mechanismus + Adressen in Abschn. 5, Fix-Rezept in Abschn. 7.

## 1. Die "letzte Kamera" = Cut 15

`ROOM1090.RDT` Header Byte 1: **nCut = 16**; Kamera-Sektion @RDT+0x24 → Datei 0x60, 32 B/Cut.
Cuts 8–15 duplizieren die Blickpunkte von 0–7 (Feuerschein-Haelfte; Umschaltung beim Betreten
im brennenden Zweig: sub00 @0x21CC `Switch work_vars[0x0A]`, Case 0→`Cut_chg 8`, Case 3→`Cut_chg 11`
— siehe analysis/nutzer_batch_2026-08-27/flammen-weg.md §4f). RVD-Haelften sind disjunkt;
im brennenden Zustand erreichbar: 8↔9↔10, 8→11↔12↔13→15, 15→13 (Zonen @0x424–0x5a0).
**Hoechster erreichbarer Cut waehrend des Brennens = 15.**

```
cut 15 @Datei 0x240: flag=1 fov=26684 (H=208) pos=(-1386,-3294,2556) tgt=(1440,-4248,-2502)
                     pri_offset=0x1d88 -> Bytes @0x1d88: ff ff ff ff  = NULL-PRI-Sektion
```

Grobe Projektion (Port-LookAt-Modell, H=fov>>7=208) der 7 Emitter (Spawn-Positionen aus
RDT sub00 @0x2214, 7× op 0x44) in Cut 15:

```
E0 (2052,-1334) -> (112,220) z=4763   E1 (2375,-2333) -> (128,209) z=5779
E2 (3104,-3498) -> (132,198) z=7133   E3 (2974, -860) -> ( 67,220) z=4798
E4 (2874,  440) -> (  6,241) z=3630   E5/E6: hinter/ausserhalb
```

Cut 15 ist die NAHESTE Kamera an den Feuern (z 3600–7100 gegen 10000–22000 in den anderen
Cuts) — jeder Sprite-/Blend-Fehler ist hier 3–5× groesser als sonst.

## 2. Port-Stand nach den juengsten Commits

Der no_draw-Fix (Commit ea4ed344, v0.3.30) ist vollstaendig im Baum:
- `re15_port/include/re15_actor.h:553` — `uint8_t no_draw;`
- `re15_port/engine/src/enemy_ai_common.c:7848` — Typ-0x26-INIT setzt `e->no_draw = 1`
  (Gegenstueck @0x801165d0–e4).
- `re15_port/platform/pc/main.c:7210` — `if (npc->no_draw) continue;` in der EINEN
  NPC-/Gegner-Mesh-Schleife (beginnt main.c:6771). Der Gate ist **cut-unabhaengig** —
  er wirkt in jedem Kamera-Cut; der Region-Quad-Cull davor (main.c:6800) kann nur
  ZUSAETZLICH culling, nie zeichnen.
- Pin: `re15_port/tests/unit/test_1090_nodraw_pin.c` (7/7 maskiert + Gegenprobe 0x10-Zombie).
- Flavor-Weiche: RDT-gesetzte 0x26er laufen in BEIDEN AI-Modi durch den RE1.5-INIT
  (`re15_re2spider_owns` haengt an `re2s_baby_spawned`, enemy_ai_common.c:7825) —
  kein Pfad ohne no_draw.

## 3. Kandidat (a) — FUN_8001ee68: GESCHLOSSEN, betrifft 0x26 NICHT

Die offene Frage aus schwarze-dreiecke.md ("Datenreferenz @0x8011f7b4, Index-Domaene
unaufloesbar") ist aufgeloest: **0x8011f7b4 ist die Haupt-Zustands-Sprungtabelle der
ZOMBIE-Familie** (Typen 0x10/0x11/0x16, Root FUN_80100424; dieselbe Tabelle, die
`re15_enemy_state.py` seit 2026-06 dokumentiert). Selbst gedumpt (STAGE1.BIN):

```
@0x8011f7b4[0] = 0x80100688   (INIT)      [1] = 0x80101224  (ACTIVE)
            [2] = 0x80105a8c  [3] = 0x80106ba4  [4] = 0x8010919c  [7] = 0x80109554
```

Der einzige STAGE1-Aufruf von FUN_8001ee68 (@0x80100fe0, `alle Parts flags:=1`) liegt in
[0x80100688, 0x80101224) = **im Zombie-INIT-Handler**. Typ 0x26 hat seine eigene Tabelle
@0x80121268 = {0x801164b0, 0x801166fc, 3× 0x8011697c, 3× 0x80116758} (selbst gedumpt) —
keiner dieser vier Handler ruft 0x8001ee68.

**Dynamischer Beleg** (`stage_saves/room1090_orig.sav`, DuckStation-Savestate des Originals,
Raum brennend, alle 7 Emitter state=1 ACTIVE):

```
slot 0..6: type 0x26, part_ptr(+0x188)=0x8013c14c…, part[0].flags = 0x0  (Bit 0 UND Bit 0x20 = 0)
```

Bit 0 bleibt im Live-Betrieb geloescht; Bit 0x20 (das FUN_8001f024 scharf machen wuerde) ist
ebenfalls 0. **Der Nicht-Zeichnen-Zustand ist der Dauerzustand — die PCSX-Watchpoint-Messung
ist dafuer nicht mehr noetig** (bleibt nur als Option, falls jemand einen vierten Schreiber
vermutet: Watchpoint auf 0x8013c14c, Skill re15-pcsx-watchpoint).

## 4. Kandidaten (b) und (c) — beide kein Bug

**(b) "Slots 4/5, 3,1 % opak-schwarz"** = die Render-TIM-Slots der Props 0/1
(`RE15_TIM_SLOT_PROP`: Prop-TIMs → Slots 4..9, main.c:436). Beide Props teilen die TIM
@Datei 0x39bd8 (128×256, 8bpp) — Messung reproduziert: **0,0 % Farbe 0x0000, 3,1 % 0x8000
(STP-Schwarz), 0 % Index-0**. Das sind die **schiebbaren Kisten** (Obj_model_set @0x216E/0x2190,
Typ 4; re15_aot.h:452ff). Typ-4-Props zeichnet auch das ORIGINAL opak (FUN_8002c18c: Typ≠3 →
Mesh-Drawer FUN_800254a0/FUN_800256b0; ABE-Pfad nur bei flags&0x10) — 0x8000-Schwarz ist dort
also auch schwarz. Zudem liegen BEIDE Kisten (−8136,4753 bzw. −5960,−15482) **ausserhalb des
Cut-15-Region-Quads** (RVD-Anker Zone 39 @0x58c: x∈[−2100,4001], z∈[−4800,2600]) — das Original
cullt sie dort (FUN_80014368), der Port identisch (`re15_prop_culled`, re15_aot.h:408; Typ-4-
Y-Versatz −900 byte-true in `re15_prop_render_y` @0x8002c234–50). Die Truemmer-Props 2/3
(TIMs @0x41df8/0x4a018, ~30 % Farbe-0x0000-Anteil) sind seit 2026-08-21 korrekt gekeyt: der
EINE TIM-Upload keyt byte-true auf aufgeloesten Wert 0x0000 (render_pc.c:1899–1946). **Kein Fund.**

**(c) PRI-Masken der letzten Kamera**: Cut 15 hat im Auslieferungs-RDT **keine** Masken
(pri @0x1d88 = `FF FF FF FF`; ebenso Cut 2 @0xc2c und Cut 10 @0x1854). Der Zwillings-Cut 7
hat 30 Masken (@0x1330), Cut 14 hat 28 gegen 71 in Cut 6 — die Asymmetrie ist AUTORISIERT.
Der Port parst pro `active_cut_idx` (main.c:4269 `active_cuts[active_cut_idx].pri_offset`)
und leert die Rect-Liste bei 0 Treffern (main.c:4302); der Vordergrund-Atlas `PRI15.TIM`
existiert korrekt NICHT (shared_assets/PSX/BSS/ROOM1090/: PRI00,01,03–09,11–14). **Kein Fund.**

## 5. Kandidat (d) → der eigentliche Befund: der Feuer-Effektpfad umgeht die Row-VM

Vorab: die "8 Winkel × 2 Beleuchtungen" sind die **BSS-Hintergruende** (BG00–15, Port laedt
per Cut-Index, BG15 vorhanden) — die Flammen-Frames sind KEINE Winkel-Frames, sondern
ESP-Anim-Records. Raum-Bank (RDT+0x4C @0x11010, Ids 05 07 09 10) selbst dekodiert:

```
Id 0x10 (Flamme, TIM @0x35378):  11 Records — [0..4],[7..9] je 1 Zelle 64×64 (grosse Flammen),
   [5] c0=5 n=9 S=16 und [6] c0=14 n=9 S=16 (die zwei 3×3-Kachel-Kompositframes),
   [10] dur=0xff → Loop auf 0.   26 Zellen.
Id 0x09 (Funken, TIM @0x32338):  Records 0..7 je 48×48, [8] Loop.   8 Zellen.
CORE00 Id 0x08 (Flamme global):  Records 0..9 je 40×40, [10] Loop.
```

### 5.1 Was das Original WIRKLICH tut (Row-Chain + Savestate)

Sub-3-Row-Chain — fuer ALLE drei Feuer-Effekte identisch (Raum-RDT @0x1161c / @0x11858,
CORE00.ESP @0x7cc):

```
row 0: selA=17  defW/H=0x1000  [0x0e]=0x0017  [0x16]=0x0020
row 1: selA=18  (Oszillator)
```

**Routine 17 = FUN_80017c00** (selbst disassembliert):
```
80017c10: lbu v0,14(v1) / 80017c18: sb v0,108(v1)   ; slot+0x6C flags := row[0x0e] = 0x17
80017c28: lhu v0,48(v1) / 80017c2c: lhu a0,22(v1)
80017c34: or  v0,v0,a0  / 80017c3c: sh v0,48(v1)    ; slot+0x30 TPAGE |= row[0x16] = 0x20 -> ABR1
80017c38: jal rng / 80017c48: andi v0,4 / 80017c50: sb v0,110(v1)   ; slot+0x6E = rng&4
80017c5c-78: row[0x0e]:=1, row[0x16]:=10, row[0x1e]:=0xffec(-20), row[0x26]:=0x64(100)
80017c4c: jal 0x800174e4                            ; Row-Advance -> Routine 18
```
**Routine 18 = FUN_80017c8c** = der defW/defH-Oszillator (bereits RE't, Port-Kommentar
enemy_ai_common.c: "advance +0x04/+0x06 by +0x1e/+0x26 for 10 frames, then negate") —
**das Flammen-Flackern der Sprite-Groesse**, gespeist von den Routine-17-Seeds.

Die Flags 0x17 heissen: 0x01 aktiv, 0x02 sichtbar, **0x04 = FOLGEN**, **0x10 = ABE**.
Der Folgen-Mechanismus, selbst disassembliert (per-Frame-Slot-Tick):
```
80019f44: lbu v1,108(a3)      ; flags
80019f54: andi v0,v1,0x4      ; FOLLOW-Bit
80019f60: lw v0,116(a3)       ; slot+0x74 = der beim Spawn uebergebene Zeiger a2
80019f68-f94: 8 Woerter *(+0x74) -> slot+0x4C..  ; Eltern-PART-Matrix JEDEN Frame kopiert
```
Beim Spawn ist a2 = `*(entity+0x188)+0x40` (@0x801166c4-e8 bzw. @0x80116d84) = die
Part-Weltmatrix des Emitters.

**Savestate-Beweis (room1090_orig.sav, Effekt-Pool DAT_800a73b8, 96×132 B):** genau 14 aktive
Slots — 7× Id 0x09 sub 3 (Funken, scale16 0x1800) + 7× Id 0x08/0x10 sub 3 (Flammen). ALLE mit
`flags = 0x0017`, `+0x74` = part+0x40 des jeweiligen Emitters (z. B. 0x8013c18c = 0x8013c14c+0x40),
und Position **(2052,−2740,…) usw. = die LIVE-Entity-Position** — obwohl die Funken beim INIT
auf y=−1800 gespawnt wurden. Die Emitter selbst stehen live auf y=−2740/−2720 (Heim-Pin
`y = home − step·(phase−1)`, step 20 @0x80116444-98, phase bis 0x30): **die Feuer brennen im
Original ~940 Einheiten UEBER dem Boden** (oben auf den im BG eingebackenen Truemmerhaufen),
und die Effekte reiten via Follow-Bit mit. Die Flammen-Slots tragen zudem `+0x72 = 0x3000/0x2f00`
= die **aktuelle** Phase<<8 (Entities +0x1D0 = 47/48), nicht den Spawn-Wert 0x2800/0x2C00.

Zusatz-Mechanismus: der Strike-Arm des Emitters pokt den lebenden Slot —
**FUN_80019d50**(id, sub, val, partptr) durchsucht den Pool nach `+0x70==id && +0x71==sub &&
+0x74==partptr` und schreibt `sh a2,0(v1)` = **slot+0x00 (Routine-Selektor A) := 0x22 bzw. 0x12**
(@0x80019d98; Aufrufer FUN_80116bec @0x80116c4c-50 / FUN_80116c68 @0x80116cc8-cc). Routine
0x22=34 = FUN_80018784 (Flare-Routine, noch nicht RE't), 0x12=18 = zurueck zum Oszillator.

### 5.2 Was der Port stattdessen tut — die drei Divergenzen

`re15_esp_type26_flame`/`_emerge` (re15_port/engine/src/re15_esp.c:409–425) rufen
`re15_esp_fx_spawn_ex` DIREKT (re15_esp.c:321–347) — **ohne Row-Bindung**: `rows_base` bleibt
NULL, `flags` bleibt 0. Folgen im Draw (platform/pc/main.c:355–362):

- **D1 — OPAK statt ADDITIV.** `if (f->rows_base) … else abe = (global && id∈{2,3})` → fuer
  Id 0x08/0x09/0x10 ist `abe = 0`. Das Original zeichnet mit ABE + ABR1 (B+F additiv,
  tpage-Bit 0x20). Opak gezeichnet werden alle nicht-0x0000-Texel der Sheets voll deckend —
  die Flammen-TIM hat 23,8 %, die Funken-TIM 11,1 % STP-gesetzte Texel inkl. dunkler
  Glut-/Rauchraender, die additiv fast unsichtbar waeren. **Deckende dunkle Fetzen an und ueber
  den Flammen — der beste Kandidat fuer "irgendein Zeug ueber den Feuern".** Dazu die zwei
  3×3-Kompositframes (Records 5/6, alle 11 Frames im Loop), die opak als blockige 48×48-Patches
  erscheinen.
- **D2 — Position eingefroren statt Follow.** Port-Funken spawnen im INIT bei y=−1800,
  die Flamme im ersten ACTIVE-Tick bei y≈−1780; danach wird `f->x/y/z` nie aktualisiert
  (spawn_ex setzt keine phys/xlat, kein Eltern-Zeiger). Original: Follow auf y=−2740/−2720.
  **Der Port zeichnet Flammen+Funken ~940 Einheiten (30–56 px in Cut 15) zu TIEF** — an der
  Basis der Truemmerhaufen statt auf ihnen.
- **D3 — Groesse statisch.** Kein Oszillator (Routine 18 fehlt), scale16 bleibt beim
  Spawn-Wert 0x2800/0x2C00 statt live 0x3000/0x2F00 (bis 22 % kleiner), defW fest 0x1000
  (main.c:338) statt gepulst.

Spawn-Kardinalitaet stimmt: Port-Zyklus sub 0→1→2→**1** (enemy_ai_common.c:7901) spawnt die
Flamme genau EINMAL pro Emitter; fx=14 == Original (Savestate). Region-Cull fuer Effekte
(`re15_esp_fx_culled`, re15_aot.h:441) laesst alle 7 Emitter in Cut 15 durch (im Quad) — korrekt.

## 6. Byte-true-Kern: Was das Original in Cut 15 an den Emitter-Positionen zeichnet

BG15.BSS (Feuerschein-Haelfte) + **pro Emitter genau 2 additive, dem hochgefahrenen Emitter
folgende, groessen-oszillierende Sprites** (Flamme Id 0x08/0x10 + Funken Id 0x09) + der
Boden-Schatten (@0x80116740, a1=−1800, im ACTIVE-Handler) + Truemmer-Prop Slot 3
(@0x21F2, (3060,−2754,−1560), in beiden Zustaenden identisch positioniert) — und **keine
PRI-Masken** (NULL @0x1d88) und **kein EM26-Mesh** (part[0].flags Bit 0 dauerhaft 0, Abschn. 3).

## 7. Fix-Rezept (Konstanten alle mit Herkunft)

1. **Row-Bindung fuer die Feuer-Effekte**: `re15_esp_type26_flame/_emerge` ueber den
   vorhandenen Row-Spawner-Pfad (re15_esp.c:615–633: `flags=0x03`, `rows_base`, Header-Seed)
   fuehren statt `spawn_ex` pur; sub=3-Streams existieren in den Daten (Raum @0x1161c/0x11858,
   CORE00 @0x7cc, je 1 Stream/2 Rows).
2. **Routine 17 implementieren** (FUN_80017c00): flags:=row[0x0e] (@0x80017c10/18),
   tpage|=row[0x16] (@0x80017c28-3c), +0x6E:=rng&4 (@0x80017c38-50), Oszillator-Seeds
   1/10/−20/100 in die Row-Kopie (@0x80017c5c-78), Row-Advance (@0x80017c4c). Routine 18
   (FUN_80017c8c) ist schon RE't — defW/defH-Oszillator; im Draw muss defW dann aus
   `f->row[0x04/0x06]` kommen statt fest 0x1000 (main.c:338).
3. **Follow-Bit 0x04**: pro Frame Position (Port-Aequivalent der Part-Matrix = die
   Entity-Position des Emitters) in den fx kopieren — Original-Beleg @0x80019f44-f94
   (`lbu flags; andi 4; lw +0x74; 8×lw/sw`). Im Port genuegt ein Eltern-Slot-Index im
   `re15_esp_fx_t` + Copy im `re15_esp_fx_tick`, solange flags&4.
4. **ABR-Mapping existiert schon** (main.c:384–393, ABR1→SDL-ADD) — greift automatisch,
   sobald flags/tpage echt sind.
5. **Strike-Poke** (optional, fuer 100 %): FUN_80116bec/FUN_80116c68 →
   FUN_80019d50(id,3,0x22/0x12,part+0x40) schreibt slot+0x00 := Selektor (@0x80019d98);
   Routine 34 = FUN_80018784 noch zu RE'en. Im Port heute als OPEN markiert
   (enemy_ai_common.c:7892/7936) — dort verdrahten.
6. **Pin**: nach Fix pruefen (a) Flammen-fx traegt flags&0x10 und tpage&0x60==0x20,
   (b) fx-y folgt Entity-y (== −2740 nach ~60 Ticks fuer E0), (c) fx-Anzahl bleibt 14.
7. **Visuelle Verifikation**: gdigrab am echten Fenster in Cut 15 (Skill
   re15-port-visual-verify; RE15_DEBUG_JUMP="1090@…", zum letzten Cut laufen). In der
   Session vom 2026-08-30 lieferte gdigrab schwarze Bilder (kein Desktop) — auf einer
   Sitzung MIT Desktop nachholen.

## 8. Offen

- **Routine 34 (FUN_80018784)** — die Strike-Flare-Routine der Flamme: nicht disassembliert.
- **Mechanismus der scale16-Aktualisierung** (+0x72: 0x2800→0x3000 im Live-Pool): Kandidaten
  sind Routine 34/18 oder ein Re-Spawn-in-denselben-Slot; nicht gepinnt. Messweg: PCSX-Redux-
  Watchpoint auf Pool-Slot+0x72 (Slot 7 = 0x800a73b8+7·132+0x72) waehrend des Brennens.
- **Direktbeweis am Port-Fenster** (welcher der drei D-Punkte dem Nutzer als "Zeug" erscheint):
  nur per gdigrab-A/B moeglich; in dieser Umgebung kein Desktop-Capture.
- slot+0x6E (rng&4, @0x80017c50) — Leser unbekannt (vermutlich Spiegel-/Variant-Bit des Draw).
- Der Schatten der Emitter: Original zeichnet ihn fest auf a1=−1800 (@0x80116740), der Port
  am Aktor — bei gefixtem Follow prüfen, dass der Port-Schatten am BODEN bleibt, nicht in
  −2740 mitschwebt.

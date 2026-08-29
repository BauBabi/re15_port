# MAP: Port-Zeichenpfad + RE1.5-Datenlage + Besucht-Zustand (Recon 2026-08-30)

RECON-Bericht (nur lesen/parsen, keine Edits). Kontext: Nutzer will das RE2-Retail-Kartensystem
(aktueller Raum ROT, besucht GRUEN, unbesucht SCHWARZ, Tueren+Treppen) im Port. Dieser Bericht
liefert die PORT- und DATEN-Seite; das RE2-Original RE't ein Parallel-Agent.

Beleg-Konvention: Port-Datei:Zeile; EXE-Adressen `@0x…` (RE1.5 PSX.EXE, Auslieferungsstand
`info/Re1.5/PSX.EXE`, verifiziert `t_addr=0x80010000`, `t_size=0xaf000` — NICHT das gepatchte
Image). Datei-Offset der EXE: `addr - 0x80010000 + 0x800`.

---

## 1. PORT-MAP-SCREEN heute (Status-Screen, Tab MAP)

Der MAP-Tab ist im Port **komplett byte-true implementiert** (Inventar-Kampagne, „MAP wave").
Er ist ein **statischer Viewer**: Karte anschauen, Spieler-Marker pulst, Abbrechen. Kein Pan,
kein Raum-Blaettern (Original: „NO other input: no pan, no room step — a static viewer",
`menu_common.c:1396`).

### 1.1 Einstieg und Zustand

| Baustein | Ort | Beleg |
|---|---|---|
| MAP-Entry (Tab-Confirm **und** L1-Sofortstart) | `re15_port/engine/src/menu_common.c:148-157` (`map_entry`), Aufrufer :174 (L1) und :207 (Confirm) | Original-Dispatch `j 0x8004997c` @0x8004982c; per-Stage-Init-Tabelle @0x80074c0c |
| Stage/Raum-Ableitung | `menu_common.c:150-154`: `stage = ((g_current_room_id>>12)&0xf)-1`, `room = (rid>>4)&0xff` | Original liest `lh DAT_800b0fe0/0fe2` (Writer @0x8001d808) |
| Per-Stage-Init (6 Sprungtabellen → Seite+Slot) | `re15_port/engine/src/re15_inv_screen.c:206-286` (`re15_inv_map_stage_init`) | Dispatch @0x80074c0c = {0x8004b568, 0x8004b8a0, 0x8004b9d4, 0x8004bc9c, 0x8004bdd4, 0x8004bf70}; jede Kette schreibt `260e` (Seite) + `260d` (Raum-Slot) |
| Persistente MAP-Globals (Slot/Seite) | `re15_inv_screen.c:94-99` (`s_map_room`/`s_map_page` = DAT_800b260d/260e-Analog) | einzige Writer im ganzen Spiel = die 6 Inits (Xref-Scan, Kommentar :94-97); out-of-range Raum ⇒ **stale-previous** (byte-true) |
| Runner-FSM (Slide rein/interaktiv/Slide raus) | `menu_common.c:1367-1423` (`map_mode` = FUN_8004c058) | 25-Frame-Slide, Deltas je Register zitiert (z.B. `25e0 += 15/f` @0x8004c0e0); Exit-Contract @0x8004c2f0-304 |
| Per-Frame-Marker aus LIVE-Spielerposition | `menu_common.c:1694-1699` → `re15_inv_screen.c:288-321` (`re15_inv_map_marker`) | Formel raw-MIPS @0x8004741c-0x80047528 (inkl. `sra 20`, magic-div /10, Y-Negation) |

### 1.2 Was gezeichnet wird (Display-List, `re15_inv_screen.c:1227-1265`)

Gate: `substate==1 && item_state==1` (Original `word(25c0)&0xffffff == 0x00010100`
@0x80049bb4-cc). Dann, in AddPrim-Reihenfolge (FUN_800473f8 @0x800475d8/@0x800475f8-61c):

1. **Spieler-Marker**: 8×8-Quad um `(map_marker_x, map_marker_y)`, uv(224,128) von der
   TEX-Seite, RGB = ECG-Puls `2602` (`re15_inv_screen.c:1240-1242`; @0x80047130-34,
   @0x80047540-6c).
2. **Fix-Sprite 1**: (30,30) 88×32, uv(0,0) der MAP-Seite = **Titel-Schriftzug** („POLICE
   STATION 1F" etc.; :1245-1246; @0x80047204-268).
3. **Fix-Sprite 2**: (270,40) 32×48, uv(96,0) = **Kompassrose + Massstabsleiste [m] 1:610**
   (:1249-1250; @0x8004726c-2c0).
4. **Raum-Rects**: `count` Eintraege aus der Paar-Tabelle `@0x80076840[page]`, Stride-12
   `{u16 x,y,w,h; u8 u@+8; u8 v@+10}` — **SPRT code 0x64|2 (semi-transparent), RGB fest
   (128,128,128)**, sampeln die MAP-PIX-Seite mit CLUT 0x7d50 (:1251-1264; @0x8004731c-60,
   @0x800472fc-318, @0x800473cc).

Alles sind **Textur-Sprites** (Ausschnitte aus dem MAP-PIX-Bild), keine Vektor-Fills/Linien.

### 1.3 Woher die Daten kommen

- **EXE-Blob** (verbatim einkompiliert): `re15_port/include/re15_inv_ui.h:46-64` — Region
  `[0x800762A0, 0x80076C00)` = 2400 Bytes in `re15_port/engine/src/re15_inv_ui_tables.c:397-400`
  (`re15_inv_map_blob`), Generator `re15_port/tools/gen_inv_ui_tables.py:153`. Enthaelt:
  14 Rect-Listen @0x800762A0, Paar-Tabelle @0x80076840, 106 Marker-Skalenzeilen @0x800768B0.
- **Karten-BILD**: `DATA/MAP01.PIX … MAP0D.PIX` (13 Dateien, je 32768 B, headerlos =
  256×256 4bpp), CD-File-Id `u16 @0x80074c4c[page]` (Ids 12..24). PC-Lader:
  `re15_port/platform/pc/src/inv_render_pc.c:339-373` (`map_page_check`), Seite → Datei
  `MAP%02X.PIX` mit `id-11` (:364). CLUT = TEX.TIM Zeile 21 (256,501) id 0x7d50
  (`inv_render_pc.c:81-83`; `GetClut(0x100,0x1f5)` @0x80046fdc-fe8).
- **Rasterizer**: Software, `inv_render_pc.c:763` (`re15_inv_render_pc_draw`), back-to-front
  :807, MAP-Texel aus `s_map4` :649/:66-69. Original-VRAM-Rundreise (StoreImage/LoadImage
  Rect (448,256,64,256) @0x8004c158-b0) ist im Port state-frei modelliert
  (`menu_common.c:1382-1388`).

### 1.4 Gibt es schon „Raum-Markierung"?

**Nein.** Die Raum-Rects werden bedingungslos mit festem RGB 0x80 gebaut (Decompile
`RE_15_Quellcode_V2/FUN_80046fd8.c:119-153`: konstante 0x80-Stores, kein Flag-Read) und
per-Frame bedingungslos AddPrim't (`RE_15_Quellcode_V2/FUN_800473f8.c:46-53`). Einzige
zustandsabhaengige Elemente: Spieler-Marker-Position (liest `lw 0x800aca88/0x800aca90` =
Spieler-Welt-X/Z @0x8004741c/0x8004746c) und Puls-RGB (`DAT_800b2602`). Der natuerliche
Einfaerbe-Hebel im Port ist das RGB-Argument des Rect-`sprt()` (`re15_inv_screen.c:1259-1262`,
derzeit 128 = neutral) — SPRT moduliert Textur × RGB/128.

---

## 2. RE1.5-KARTENDATEN (Auslieferungsstand)

### 2.1 Wo sie liegen — und wo NICHT

- **KEINE MAP-Sektion in den RDTs.** Die 0x60-Adress-Tabelle (RE15_KNOWLEDGE.md:19-46) kennt
  nur collision/camera/zone/light/md1/floor/block/message/scd×3/effect/espTim/modelTim/
  animation + Soundbanks. Kartendaten sind raumdatei-frei.
- **13 Bild-Dateien** `re15_port/shared_assets/PSX/DATA/MAP01.PIX … MAP0D.PIX` (je 32768 B
  = 64 Halbwoerter × 256 Zeilen = 256×256 4bpp, roh, kein Header; Upload-Rect
  (448,256,64,256) @0x8004c1a0-b0).
- **EXE-residente Tabellen** (alle in `[0x800762A0,0x80076C00)`, im Port verbatim als
  `re15_inv_map_blob`): Rect-Listen, Paar-Tabelle, Marker-Skalenzeilen, plus CD-Id-Tabelle
  @0x80074c4c (Region 1).

### 2.2 Was das BILD enthaelt (dekodiert, Beweis-PNGs)

`analysis/nutzer_batch_2026-08-30b/map01_gray.png` / `map03_gray.png` (4bpp→Graustufen aus
den PIX-Rohdaten dieser Recon):

- MAP01 (Seite 0) = **„POLICE STATION B1"**, MAP03 (Seite 2) = **„POLICE STATION 1F"** —
  eine Seite = ein **Stockwerk**.
- Jede PIX ist ein **ATLAS**: Titel-Schriftzug (uv 0,0), Kompass+Massstab „0 5 10 15 [m]
  1:610" (uv 96,0), und **Raum-Umriss-KACHELN** (dunkle Fuellung, helle Wandlinien).
- **Tuer-Oeffnungen sind IN die Wandlinien gezeichnet** (die kleinen Kerben/U-Formen in den
  Umrissen). Es gibt KEINE separaten Tuer-/Treppen-Marker-Daten — Tueren/Treppen existieren
  nur als Pixel im Atlas-Artwork.

### 2.3 Die EXE-Struktur, exemplarisch STAGE1 (aus `info/Re1.5/PSX.EXE` gedumpt)

**Paar-Tabelle @0x80076840** — 14 × `{u16 count, u16 pad, u32 list_ptr}`:

```
page  0: count= 7 list=@0x800762a0    page  7: count=14 list=@0x80076558
page  1: count=10 list=@0x800762f4    page  8: count=15 list=@0x8007660c
page  2: count=11 list=@0x8007636c    page  9: count=15 list=@0x800766c0
page  3: count=10 list=@0x800763f0    page 10: count= 4 list=@0x80076774
page  4: count= 7 list=@0x80076468    page 11: count= 4 list=@0x800767a4
page  5: count= 2 list=@0x800764bc    page 12: count= 8 list=@0x800767d4
page  6: count=11 list=@0x800764d4    page 13: count= 1 list=@0x80076834
```

**CD-Id-Tabelle @0x80074c4c** (Seite→Datei): `[12,13,…,24]` = MAP01…MAP0D. Seite 13 (0xd)
liest **hinter dem Tabellenende**: `u16[13] = 0` (Bytes @0x80074c66) → CD-File 0 —
byte-true Prototyp-Bug (Port rendert dafuer eine schwarze Seite,
`inv_render_pc.c:343-345,359-363`).

**Rect-Liste Seite 2** (STAGE1-Raeume 0-11, 1F) @0x8007636c, Stride 12:

```
[ 0] x=180 y= 69 w=32 h=96 u=  0 v=32     [ 6] x=180 y= 88 w=24 h=24 u=128 v=16
[ 1] x=109 y= 77 w=56 h=56 u= 32 v=32     [ 7] x=164 y= 93 w=16 h=24 u=152 v=16
[ 2] x=102 y= 77 w=24 h=80 u= 88 v=48     [ 8] x=164 y= 77 w=24 h=24 u=128 v=40
[ 3] x=145 y=109 w=56 h=40 u=192 v=16     [ 9] x=109 y=134 w=16 h=16 u=168 v=40
[ 4] x=119 y=125 w=72 h=64 u=184 v=56     [10] x=119 y=134 w=24 h=24 u=168 v=16
[ 5] x=180 y= 59 w=48 h=32 u=112 v=64
```

(x,y) = Bildschirmposition, (u,v) = Atlas-Position in der MAP-PIX — (x,y) ≠ (u,v), die
Seite wird aus Atlas-Kacheln **zusammengesetzt**.

**Seite 3 (Raeume 12-17) @0x800763f0 = byte-identische Kopie der ersten 10 Eintraege von
Seite 2** (verglichen in diesem Lauf) — offensichtlich Platzhalter/Prototyp-Stand.

**Marker-Skalenzeilen @0x800768B0** — 106 × `{s16 x_off, u16 y_off, u16 x_scale, u16 z_scale}`
(Ende exakt @0x80076C00, self-closing). STAGE1-Slots 0..37, Auszug:

```
slot  2 (ROOM1020): x_off=100 y_off=136 x_scale=2287 z_scale=2287
slot  3 (ROOM1030): x_off=108 y_off=194 x_scale=2428 z_scale=2229
slot  7 (ROOM1070): x_off= 79 y_off=205 x_scale=2229 z_scale=2088
slot 21 (ROOM1150): x_off=111 y_off=130 x_scale=2296 z_scale=2312
slot 23 (ROOM1170): x_off=100 y_off=206 x_scale=2280 z_scale=2268
slot 20 (ROOM1140): x_off=  0 y_off=  0 x_scale=   1 z_scale=   1   <- PLATZHALTER
```

**Nur 39 von 106 Zeilen sind kalibriert**, 67 sind Platzhalter `{0,0,1,1}` (gedumpt:
kalibrierte Slots = 2,3,4,7,21,23,25,26,27,30,32,33,38,50,51,53,54,56,57,58,59,64,65,66,68,
74,75,78,80,81,83,87,89,93,94,95,97,101,102). Mit scale=1 liefert die Formel
`((wx+32000)*10*1)>>20 ≈ 0` → Marker parkt bei `(x_off≈0, y_off=0)` = links oben ausserhalb
der Karte. Auch ROOM1140 (Briefing) und ROOM1240 (Start, Slot 36) sind unkalibriert.

### 2.4 Seiten-/Slot-Zuordnung (aus den Init-Sprungtabellen, Port-Zitate §1.1)

| Stage | Raum-Bound | Slot-Basis | Seiten (Raum-Bereiche) |
|---|---|---|---|
| 1 | `< 0x26` (38) | 0 | 0-11→**2**, 12-17→**3**, 18-22→**4**, 23→**5**, 24-29→**0**, 30-37→**1** |
| 2 | `< 0xc` (12) | +38 | 0-9→**6**, 10-11→**0xd** (CD-Id-Overrun!) |
| 3 | `< 0x20` (32) | +50 | alle→**7** |
| 4 | `< 0xc` (12) | +65 | alle→**8** |
| 5 | `< 0x15` (21) | +77 | 12-14→**0xa**, 15-16→**0xb**, sonst→**9** |
| 6 | `< 0x8` (8) | +98 | alle→**0xc** |

Quirks (byte-true):
- **Ausserhalb des Bounds = KEIN Update** (stale-previous): STAGE1-Raeume 0x26/0x27
  (ROOM1260/1270), STAGE2 0x0C-0x0F (ROOM20C0-20F0), STAGE5 0x15-0x17 (ROOM5150-5170)
  haben **keinen Karten-Slot** (Baum hat 40/16/24 Raeume, Bounds decken 38/12/21).
- **Slot-Kollision**: STAGE3 Raum 15 (0x30F0) → Slot 65 = STAGE4 Raum 0 (0x4000) → Slot 65
  (Basis 50+15 = Basis 65+0) — beide teilen dieselbe Skalenzeile.

### 2.5 KERNBEFUND fuer das RE2-Feature

**Die RE1.5-Rects tragen KEINE Raum-ID.** Stride-12 = reine Geometrie (x,y,w,h,u,v); die
Zaehler passen nicht auf die Raumzahlen (Seite 2: 11 Rects fuer 12 Raeume; Seite 3: 10
Kopie-Rects fuer 6 Raeume; nur Seite 12 mit 8 Rects fuer 8 STAGE6-Raeume passt exakt).
**Eine Rect↔Raum-Zuordnung existiert im Auslieferungsstand nicht** — fuer „Raum X gruen"
muss sie neu autorisiert werden (Daten-Authoring, kein RE-Fund). RE1.5 hat also **kein**
RE2-Aequivalent „pro Raum ein Rechteck + Tuer-Punkte als Vektor-Daten"; es ist ein
Bild-Atlas + unbeschriftete Kachel-Platzierungen + (lueckenhafte) Marker-Kalibrierung.

---

## 3. BESUCHT-ZUSTAND

### 3.1 Original: existiert NICHT

- Der komplette MAP-Zeichenpfad liest nur: `DAT_800b260d/260e` (Slot/Seite),
  Spieler-Welt-X/Z (`0x800aca88/90`), Puls (`2602`), Framebuffer-Index (`aca34`) —
  vollstaendige Decompiles `FUN_80046fd8.c` (Builder) und `FUN_800473f8.c` (Per-Frame),
  kein Flag-Bank-Zugriff, keine Bedingung pro Rect.
- `DAT_800b260d/0x800b260e` liegen **AUSSERHALB** des 0x1230-Byte-GSB
  `[0x800b0fbc, 0x800b21ec)` (memcpy @0x800261c4-d4, `ori a2,zero,0x1230` @0x800261d0;
  kartiert in `analysis/nutzer_batch_2026-08-30b/itembox-original.md` §4.1) — selbst die
  aktuelle Seite wuerde das (ohnehin dormante) Save-System **nicht** mitspeichern.
- In der GSB-Kartierung ist nichts Besucht-Foermiges bekannt; die einzigen „unklaren"
  genullten Bereiche sind die 4×8-Slot-Arrays @0x800b1444/1484/14a4/14c4 (Null Leser/
  Schreiber, itembox-Verdikt) und der ungenullte Gap 0x800b1464-1484. HYPOTHESE-frei
  bleibt: **kein Besucht-Speicher im RE1.5-Auslieferungsstand nachweisbar.**

### 3.2 Port: natuerlicher persistenter Ort (Empfehlung)

- **Save-Block**: neues Feld in `re15_savedata_t`, **vor `checksum` anfuegen + Version → 6**
  nach dem etablierten Muster v4→v5 (`re15_port/include/re15_savedata.h:29-44,87-91`;
  Upgrade-Pfad `re15_port/engine/src/re15_savedata.c:31-48` — aeltere Blocks laden mit
  Null = „nichts besucht", checksum-kompatibel via `offsetof`-Trick).
- **Form**: `uint32_t visited[6][2]` (6 Stages × 64 Bit ueber den Raum-Index-Nibble
  `(rid>>4)&0xff`, benoetigt ≤40 Bit/Stage) = 48 Bytes — deckt ALLE Raeume ab, auch die
  ohne Karten-Slot (§2.4). Alternative Minimalform: 106-Bit-Bitmap ueber den Karten-Slot
  (16 B), verliert aber genau die Slot-losen Raeume und erbt die 65er-Kollision (§2.4).
- **NICHT `g_game.flags`** zweckentfremden: die 16×256-Bit-Zonen (`re15_scd.h:386-396`)
  sind script-sichtbar (SCD Ck/Set-Opcodes) — eine „freie" Zone ist ohne game-weiten
  SCD-Census nicht beweisbar frei; Kollisionrisiko.
- **Setz-Punkt**: `re15_room_apply_pending` (`re15_port/engine/src/room_common.c:195` ff.),
  nach erfolgreichem `c->load_rdt` (:211) — der einzige Engine-Chokepoint des Raumwechsels;
  `g_current_room_id` selbst wird platform-seitig im Lader gestellt (`room_pc.c:65`,
  `platform/psx/src/re15_room.c:39`). Zusaetzlich beim New-Game/Load-Restore den Start-/
  Resume-Raum stempeln (`re15_savedata_restore` laesst den Raum-Load beim Caller,
  `re15_savedata.c:158`).

---

## 4. RAUM-IDS + wie der MAP-Screen an den aktuellen Raum kommt

- **Id-Format**: `0xSRRV` — Stage-Nibble 1..6, `RR` = Raum-Index (<<4), `V` = Variante 0/1.
  Generierte Vollliste: `re15_port/include/re15_room_list.h:5-27` (`re15_room_ids[]`,
  240 Eintraege = 120 Raum-Basen × 2 Varianten; STAGE1 40, STAGE2-4 je 16, STAGE5 24,
  STAGE6 8 Basen — deckt sich mit den RDT-Zahlen im Baum: 80/32/32/32/48/16 RDTs).
- **MAP-Screen-Anbindung**: `menu_common.c:148-157` — `map_entry()` liest direkt
  `g_current_room_id` (deklariert `room_common.c:182`) und zerlegt die Nibbles wie der
  Original-Writer @0x8001d808 (`DAT_800b0fe0/0fe2`).
- Stage-Nibble-Ableitung an anderer Stelle identisch: `enemy_ai_common.c:12833`.

---

## 5. Kompakt: Was das RE2-Feature im Port braucht (Datenlage-Konsequenz)

1. **Zeichen-Hebel vorhanden**: Rect-RGB in `re15_inv_screen.c:1259-1262` (Modulation);
   fuer echte RE2-Fills (rot/gruen/schwarz unter den Umrissen) muessten neue Fill-Prims
   unter die Atlas-Kacheln (der Atlas hat dunkle Raum-Fuellungen + helle Wandlinien —
   Tinten faerbt beides).
2. **Fehlend und NICHT im Original vorhanden**: Rect↔Raum-Zuordnung (muss autorisiert
   werden), Tuer-/Treppen-Vektor-Marker (nur Pixel-Art), Karten-Slots fuer 9 Raeume
   (Stage1: 2, Stage2: 4, Stage5: 3), kalibrierte Marker-Zeilen fuer 67 von 106 Slots.
3. **Besucht-Speicher**: neu anlegen (Save v6, §3.2) — es gibt kein Original-Vorbild.

## Anhaenge
- `map01_gray.png`, `map03_gray.png` — dekodierte MAP-PIX-Seiten (B1 / 1F), Beweis fuer
  Atlas-Charakter + eingezeichnete Tuer-Kerben.

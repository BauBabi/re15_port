# Runde 34 (Granaten) — Gegenpruefung Spur D (RE2-FX-Maschine), gebauter Stand

Stand: 2026-09-30, Zweig `r34g/d-re2fx`, Arbeitsbaum `.claude/worktrees/r34g_d`, geprueft auf HEAD `694b3cc6`
(Diff zu C0 `8d8651e4`: nur `engine/src/re2_fx.c`, `include/re2_fx.h`, `platform/pc/src/re2fx_pc.c/.h`,
`tests/unit/probe_r34_re2fx*.c`, `tests/unit/probes/r34_re2fx.cmake`, `tools/re2fx_katalog.py`, `bau_d.md`).
Pruefer aendert KEINEN Code. Alle Mutationsproben unten wurden mit `git checkout -- re15_port/engine/src/re2_fx.c`
zurueckgesetzt; danach `git diff` leer, Neubau ueber `local_build.sh build`, Sonden wieder mit den Bauer-Zahlen
(Lebensdauer 140, Applier-Rufe 206, Pixel Saeure 135182 / Brand 374878, 626 Ausschnitte).

STATUS: abgeschlossen (Konstanten §1, Semantik §2, Sonden/Mutationen §3, Dateibesitz §4, Tests §5/§5a, Risiko §6,
Vollstaendigkeit §7, Maengel M1-M9, Fazit).

Werkzeuge: `.claude/skills/re15-psx-disasm/scripts/re2_disasm.py` (RE2 `info/re2leon/PSX.EXE`, md5 09a9b642…) und
`re15_disasm.py` (RE1.5 `info/Re1.5/PSX.EXE`), eigener unabhaengiger ESP-Dekoder (Scratch, nicht versioniert) fuer
RE2 `CORE00.ESP` (md5 c0b0a7f4… = `shared_assets/RE2/CORE00.ESP`), `TEX.TIM` md5 7472e1a8… (= Asset).

---

## 1. Konstanten — selbst disassembliert (tragende Mechanik vollstaendig)

Ergebnis: **alle zitierten Adressen/Werte der Maschine stimmen**. Einzelbelege (Auszug; jede Zeile selbst gelesen):

| Bereich | Port | Beleg (RE2 PSX.EXE) | Urteil |
|---|---|---|---|
| Op-Tabelle | 0/1/2/19/25/27/28/29/30/40/46/48/49/50/58/64 | `table 0x8009D868`: [19] 0x8001f2c0, [27] 0x8001fa9c, [28] 0x8001fbd0, [29] 0x8001fd5c, [30] 0x8001fecc, [40] 0x80020758, [46] 0x80020b60, [48] 0x80020f3c, [49] 0x800215c8, [50] 0x80021970, [58] 0x80022254, [64] 0x80022728 | stimmt |
| Registrierung | re2_fx.c:136-172 | FUN_8001bca0 `lbu v1,0(t1)` / `lw v1,0(t2)` / `addiu t2,t2,-4` / `(w&0xffff)*2 + (w>>16) + 2` `sll 2` @0x8001bccc-d20, `sltiu v0,t0,0x8` @0x8001bd24; Boot a1 = letztes Wort @0x8001bb54-80 | stimmt |
| Spawner | re2_fx.c:177-243 | FUN_8001cbe8: Suche 95→0 (`addiu t0,zero,11904` / `-124`), voll 255 @0x8001cc70, `addiu v0,zero,16384` @0x8001cc80, `sw v0,28(t0)` (Bank/Sub), `lbu v0,10(t1)`→+0x20, `sll 16`→+0x28, a3→+0x2C/+0x30, CLUT `srl t5,3 / sll 6` @0x8001ccf4-d10, Matrix +0x4C, +0x70 = t1+8, +0x74 = t1+(n1*4+4)*2, +0x78 = part0+4; Mehrteil 254 @0x8001ce10, 100 Byte ab +0x18, +0x1A := Haupt; FUN_8001bf10 identisch bis `ori v0,zero,0xa003` @0x8001bfa8 | stimmt |
| Pumpe | re2_fx.c:823-849 | FUN_8001d300: Pause `lw 0x800cfbdc / lui 0x1000 / and` @0x8001d318-2c, Baenke 26/28/40/21-22 (+1 nur Update) @0x8001d354-80/@0x8001d454-78, Waise @0x8001d544-7c, Befoerderung `ori s2,zero,0xa003` @0x8001d5d8/608 + Op A, `jal 0x8001d68c` + `addu a0,zero,zero` @0x8001d644-48 | stimmt |
| Platz-Schritt | re2_fx.c:779-810 | FUN_8001d68c: Weltlage vor Op B @0x8001d6c8, Physik lokal+=vel DANN vel+=acc (s8) @0x8001d720-794, Anim ENDE/LOOP @0x8001d7ac-880 | stimmt |
| Weltlage | re2_fx.c:280-318 | FUN_8001d894: 0x800-Kopie @0x8001d8b8-fc, Scratch = 0x8009DB44, +0x3C/+0x40 @0x8001d954-6c; 0x400: rtv0 mit Einheit (`0x4a486012` sf=1 mx=RT v=V0 cv=none) → +0x2C+MAC (sh), dann SetRotMatrix(+0x4C) @0x8001da28-4c, +0x34 := +0x60+MAC1 @0x8001da8c-98; normal: RotMatrixY(+0x22) `jal 0x8008e8b4` @0x8001dacc, +0x34 := MAC (sh) @0x8001db50, += +0x60 + M·Versatz @0x8001dbd0-dc1c | stimmt |
| RotMatrixY | roty_einheit re2_fx.c:267-275 | FUN_8008e8b4: a≥0 `subu t1,zero,t7` (t1 = −sin) @0x8008e910, a<0 t1 = +sin(−a) @0x8008e8c4-e8; m0j' = (c·m0j − t1·m2j)>>12, m2j' = (t1·m0j + c·m2j)>>12 @0x8008e918-a40 → auf I: [[c,0,s],[0,1,0],[−s,0,c]] | stimmt |
| Trig-Tafel | re15_sin_q12/cos | RE2 0x800ADEAC (16384 B) == RE1.5 0x800794C4 == `re15_trig_lut` (4096 Worte, selbst verglichen) | stimmt |
| Op 19 | re2_fx.c:412-449 | `lh v0,74(v1)` @0x8001f2d0, `sltiu v0,v0,0x10` @0x8001f2e8, `sltiu v0,v0,0x1001` @0x8001f2fc, `jal 0x80020758` @0x8001f308, +0x16++ @0x8001f31c-28 (im +0x4A-Zweig, auch ohne Op 40); 990 = `sll5/subu/sll4/subu/sll1`, 980 @0x8001f3a4-e4; 1009/1002 @0x8001f40c-44; 90 + r%11 (`0x2e8ba2e9`, `addiu 90`) @0x8001f488-c0; Zustand 3 → 2, 30 + r mod 8 @0x8001f4e8-518 | stimmt |
| Op 25/27/28 | re2_fx.c:452-502 | Op 25 `slt v0,a0,v0` @0x8001fa48; Op 27 0xB003 @0x8001fabc, `ori 0x20` @0x8001fac4, r%3 (`0x55555556`), a1 2 / a2 8192 / a3 0 @0x8001fb28-50, `sw v0,20(v1)` (32 Bit) @0x8001fb60, 58/28/2 @0x8001fb64-8c, `addiu v0,v0,8` @0x8001fbb4; Op 28 Wasser-Tod OHNE Ruecksprung @0x8001fc20-28, vel.x<0 @0x8001fc3c-48, −900 @0x8001fc6c, `slt v1,s0,v1` @0x8001fc94 → Op[step[2]] und Ende ohne +0x14, Kontakt `beq s0,v0` (32 Bit) @0x8001fd00, `sw s0,20(v0)` @0x8001fd44 | stimmt |
| Op 29/30 | re2_fx.c:505-542 | `bgtz` + Delay `slti v0,v0,61` @0x8001fd74-78, `multu 0x88888889 / srl 3` (u8 %15) @0x8001fd94-b8, Skala `sll 2 / 0x66666667 / sra 1` @0x8001fdc0-ec, `lui v0,0x504` @0x8001fdf0, a2 0x8009db44 / a3 +0x34, Kind +0x4A `sh v1(1),-29382(at)` @0x8001fe20 (Rueckgabe `andi 0xff / sltiu 0xff`), step[2]++ @0x8001fe30-3c, −100 @0x8001fe60; Op 30 +0x1B := 3 im Delay-Slot von `bne` @0x8001fef0-f4 bzw. `sb v0,27(a0)` @0x8001ff38, 700 + (r%6)·50 @0x8001ff3c-84 | stimmt |
| Op 40/46/50/64 | re2_fx.c:545-595 | Box `a8 fd 00 00 2c 01 96 00` @0x80010910 = {−600,0,300,150}, −100 @0x800207a4, `lui a3,0x2002 / ori 0xa` @0x80020794/a0; Op 46 19/29/180 @0x80020b84-ac, −10 − r%11 @0x80020be0-f4, 38 + r mod 8 @0x80020c20, `andi 0xfffe` @0x80020c28; Op 50 64 @0x80021978, `sb zero,8 / sb zero,3 / sh zero,12` @0x8002198c-9c, `andi 0xfffd` @0x800219a8; Op 64 `lw v1,20` / `sh v1,54` / `sb zero,1` / `andi 0xfffe` @0x80022734-70 | stimmt |
| Op 58 | re2_fx.c:598-622 | 880 (`sll3/subu/sll3/subu/sll4`) MIT, 800 (`sll1/addu/sll3/addu/sll5`) OHNE Vorzeichenkorrektur @0x800222c8-328; 1010/1007 @0x80022350-84; 2 + r%2 @0x800223c4-e4 | stimmt |
| Op 48 | re2_fx.c:693-750 | 0x8000 @0x80020ff4, +0x60/64/68 := Lage, Phase 1, Op B 48 @0x8002101c, SE 0x01120001 a1 = +0x60 @0x80021028-30, Bit-0x80-Zweig tot @0x80021048, Kinder 0x040C2800 / 0x041D2700 a2 = +0x4C @0x80021094-dc; Flamme: Skala `sll1/addu/sll8/addiu 7168 / lui 0x505` @0x80021104-1c, Gier r%40 / r%80+400 / r%80−400 (`0x66666667 >>4/>>5`, `lhu +0x22` + `sll16/sra16`), nur bei `(v0&0xff) < 0xff`: +0x0C += r%25 (`0x51eb851f >>3`), +0x4A := 1, +0x09 += r mod 8; Ende +0x60/64/68 := sp+16/20/24 mit y + 1800 (@0x80021080) + 1800 (@0x800210b4) | stimmt |
| Op 49 | re2_fx.c:637-690 | Sprungtabelle `table 0x80010950` = {0x80021678, 0x800217d4, 0x80021854, 0x80021894, 0x800218f4}; 0x8403 @0x80021694, Op B 49 im Delay-Slot @0x800216b0, SE 0x01130001 a1 = sp+16; −23 im Delay-Slot @0x80021738, 240 @0x80021770-78, 255-Zweig −470/−600; Kinder a1 = +0x22, a2 0x8009db44, a3 +0x34; Phase 4 ohne +0x12-Schreiben, frei @0x80021950-58 | stimmt |
| Aufschlag-Stand | re2_fx.c:924-957 | Op 17 @0x8001f198: +0x1B := Waffe−9 @0x8001f1b4-b8, Op A 22, Op B 15, 0xB403 @0x8001f1e4, Anim 18 @0x8001f1ec, TPage `ori 0x20` @0x8001f200, +0x0B 15 (`addiu a2,zero,15` @0x8001f1d4 / `sb a2,11` @0x8001f284); Op 15 dispatcht Op[step[2]+Art] (Wasser/Treffer/Zeitablauf) bzw. Op[step[3]+Art] (Kontakt) @0x8001f0e0-104 — Skript Bank 2 Skr. 4 @0x192C `00 11 2f 2f …` → step[2] = step[3] = 47 (eigener Dekoder), also IMMER Op 47+Art | stimmt |
| Runde (Herkunft) | re2_fx.c:941-944 | Waffen-Handler bei +0x14D == 1 @0x80044f58-60: `jal 0x8001bf10` mit 0x020C1000 @0x80044f9c-a0, a1 = `lh a1,118(s1)` @0x80044fa8, a2 = *(+0x198)+0x7AC @0x80044f78/90, a3 = {120,1200,0} @0x80044f7c-8c | stimmt |
| TEX.TIM | re2fx_pc.c:54-92 | Datei 225 = "TEX TIM" (`bytes 0x80010a08`) nach 0x8011a000 @0x8002b8b0-c4, 0x800CFBF0 := 28 (`sh` @0x8002b8d4), FUN_80076a40: x = 28·64 −1024 @0x80076a64-80, y = 256 @0x80076a9c-a8, CLUT y = 0x800CFBF1 + 480 @0x80076b00-0c; Datei-Kopf CLUT (256,480) 32×19, Bild (0,0) 256×256 hw | stimmt |
| Billboard | re2_fx.c:857-919 | FUN_80077924: `andi v1,s2,0xa000` @0x80077a18, `jal 0x8002c820` @0x80077a30, nprim 0 @0x80077a40, 44/46 @0x80077a44-50, 0x200 @0x80077a54; FUN_80077ed0: RTPS `0x4a180001` @0x80077f0c, `sra v0,v1,9` @0x80077f58, 32767 @0x80077f64-74, OT `sra 5` @0x80077f80 + Voreinfuegen @0x80077f94-fac, step `div t1,SZ<<4` @0x80077fd8, `divu` @0x8007801c/5c, Trim 0x1FFFF @0x80078070-88, Ecken @0x80078090-100, UV @0x80078104-14c; Paketfarbe 0x2C808080 @0x800783cc-d0 (768 Pakete ab 0x800C4418), Puffer-Ende 0x800C8018 + n·0x3C00 @0x8007796c-b4 | stimmt |
| RNG | re15_re2_rand | FUN_80015FE8 @0x80015ff0-18 = Sonden-Nachbau `rnd()`; Startwert 0xD2706CA4 @0x8002b908-0c | stimmt |
| RE1.5-Bodensonde | re2_fx.c:386 | RE1.5 Routine 12: `addu a1,zero,zero` @0x800177b8, `ori a2,zero,0x8` @0x800177c4, `ori a3,zero,0x100` @0x800177d0, `jal 0x8001c6e8` @0x800177d4; Objekt-Schieber `jal 0x8003b558` / `ori a1,zero,0x2` @0x8002bfb0-b4 | Adressen stimmen (Semantik: s. M1) |

Geprueft und ohne Befund: O-VB1 (B aus PL01/PL01W09, 0x400-Zweig = Einheits-Basis, RotY-Konvention), Delay-Slots der
Ops 27/29/30/49, s16/s32 (Op 27/28 `sw`/`lw` 32 Bit auf +0x14, Op 64 `sh` 16 Bit), Reihenfolge Update-/Draw-Pass,
Bildzaehlung X / X+1..X+4 (Saeure) bzw. X / X+1 (Brand).

---

## 2. Semantik gegen BAUPLAN §1.3/§1.4

* Saeure-Folge (Kinder X; je ein Phasen-Kind X+1..X+4; frei in X+4), Brand-Folge (2 Kinder + 3 Flammen in X; frei in X+1),
  Bodenflamme (27 → 58/28 → 46 → 19/29 → 50/30), RNG-Zuege und -Reihenfolge: **deckungsgleich** mit Disasm und BAUPLAN.
* Kinder-Sichtbarkeit: Kinder landen auf Plaetzen UNTER dem Aufschlag-Platz und werden im Draw-Pass des Folgebilds
  befoerdert — RE2-Mechanismus (@0x8001d5f8-608), stimmt mit der Sonde (208).
* **Abweichung O-VB2 (M1)**: Die Port-Abbildung der RE2-Bodensonde liefert fuer (x,z) ueber einer Band-0-Zelle die
  Oberkante −1800 UNABHAENGIG von P.y. RE2 senkt die Rueckgabe nur fuer Formen, UEBER deren Oberkante P liegt
  (`slt v0,s0,v1` @0x8004ffe4 → Kontakt-Zweig; `sh s0,15228(at)` @0x80050044 nur fuer P.y ≤ oben), Objekte mit P darin
  liefern P.y − 1 (@0x8004fd2c-54). Eine Flamme am Boden (Op 27: P = Flammenlage, y = Q.y > 0) haette in RE2 immer
  Rueckgabe 0. Folge im Port: Op 27 schreibt +0x14..+0x17 := 0xFFFFF8F8 (`sw v0,20(v1)` @0x8001fb60) → +0x16 = 0xFFFF
  → Op-19-Tor `sltiu v0,v0,0x10` @0x8001f2e8 ist im ersten Brennbild offen (dann Ueberlauf auf 0) statt nach 16 Bildern.
  Das Dossier (bau_d.md §2.5) nennt als einzige Abweichung die unerreichbare Luft-Wand; diese hier ist erreichbar
  (Granate fliegt durch Waende, BAUPLAN §1.1; Bauer-Messung §2.5: ROOM1140 8241 von 12100 Gitterpunkten mit Boden −1800).
* **Messung M1 (echte Engine, echte Raeume)**: Scratch-Programm (Pruefer, unversioniert; linkt `build_r34_d/engine/
  libre15_engine.a` + `tests/libre15_test_support.a`, laedt `ROOM####.RDT` nach `g_room_rdt`, `re2fx_boden_hook = NULL`,
  Applier-Spion, `re2fx_aufschlag(1, {x, 10, z}, 0)`, 120 × `re2fx_tick`; Punkte per 250er-Raster: Wand = room_coll −1800
  und `box_blocked`, frei = room_coll 0 und 3000 Umkreis frei):

  | Raum | Punkt | room_coll | Flamme 92 +0x14..17 | +0x16 | Landung | 1. Applier-Ruf |
  |---|---|---|---|---|---|---|
  | ROOM1140 | Wand (−10500, −25500) | −1800 | 0xFFFFF8F8 | 0xFFFF | Bild 2 | **Bild 3** |
  | ROOM1140 | frei (−30000, −30000) | 0 | 0x00000000 | 0x0000 | Bild 2 | Bild 19 |
  | ROOM1170 | Wand (−24000, −28750) | −1800 | 0xFFFFF8F8 | 0xFFFF | Bild 2 | **Bild 3** |
  | ROOM1170 | frei (−5750, −30000) | 0 | 0x00000000 | 0x0000 | Bild 2 | Bild 19 |
  | ROOM1150 | Wand (−28250, −27750) | −1800 | 0xFFFFF8F8 | 0xFFFF | Bild 2 | **Bild 3** |
  | ROOM1150 | frei (−11250, −30000) | 0 | 0x00000000 | 0x0000 | Bild 2 | Bild 19 |

  Ueber einer Band-0-Zelle beginnt der Nachbrenner-Schaden 16 Bilder frueher als auf freiem Boden. Nach RE2-Regel waere
  die Rueckgabe dort ebenfalls 0 und der erste Ruf in Bild 19: P.y = +10 liegt unter der Unterkante jeder Form (≤ 0) →
  Zweig "unter unten" (`slt v0,s1,v1` / `bne` @0x8004ffdc-e0 → @0x8005005c → nur Buchhaltung 0x800D5BE4), bei Objekten
  `slt v0,v0,a1` / `bne` @0x8004fce0-e4 (Unterkante < P.y → uebersprungen); die Rueckgabe 0x800C3B7C bleibt 0.

---

## 3. Sonden und eigene Mutationsproben

Echte Engine: ja (alle drei Sonden rufen `re2_fx.c` direkt; Spione nur an den Haken). Erwartungen aus dem Disasm bzw.
unabhaengigem Nachbau (RNG, RotY, Aspektfaktoren). Negativ-Kontrollen vorhanden (103/104/113/201/423/430/602, knochen 20,
bild 10). Die Bauer-Mutationen (11 Stueck) decken 412/206/10/313/601/429/703/702/602/441/450.

Eigene Mutationsproben (je: Konstante verstellt → `local_build.sh build` (17 s) → Sonden → `git checkout` → `git diff` leer):

| # | Mutation (re2_fx.c) | Ergebnis | Bewertung |
|---|---|---|---|
| E | Op 49 vel.y 240 → 239 (:658) | `FAIL 206` | tragend, erfasst |
| F | Flamme 1 Gier r%40 → r%41 (:700) | `FAIL 316` | tragend, erfasst |
| G | RotY-Vorzeichen getauscht (:272/:274) | `FAIL 216` (Kind (1000,20,−1761) statt (999.7,20.4,−2239)) | O-VB1 erfasst |
| C | Op 46 vel.x 180 → 200 (:575) | `FAIL 401` | erfasst |
| C2 | Op 46 vel.x 180 → 189 | **alle drei gruen** | nur Obergrenze geprueft (M5) |
| D | Op 27 Zaehler 8 → 9 (:477) | **alle gruen** | ungepinnt, im Aufschlag folgenlos (M6) |
| A | Billboard `szk << 4` → `szk << 3` (:893) = doppelte Spritegroesse | **alle gruen**; Pixel Saeure 135182 → 544731, Brand 374878 → 1544903; `re2fx_katalog.py --vergleich`: "593 Ausschnitte, 0 abweichende Texel" | Geometrie ungepinnt, O9-Metrik blind (M3) |
| B | O-VB2: `room_coll` → 0 und `box_blocked` aus (:386/:390) | **alle gruen** | RE1.5-Raumabbildung ohne Sonde (M2) |
| H | Normalzweig Weltlage: Gier gespiegelt (:305) | **alle gruen** (Brand-Pixel 374878 → 354046, ungeprueft) | Gleitrichtung/Kinderlage ungepinnt (M4) |

---

## 4. Dateibesitz

`git diff --stat 8d8651e4..HEAD`: nur Spur-D-Dateien + `tools/re2fx_katalog.py` + Dossier. `main.c`, `render_pc.c`,
`re15_esp.c`, `re15_damage.c` unveraendert (`grep re2fx platform/pc/main.c` leer). C0-Vertragsdeklarationen in `re2_fx.h` /
`re2fx_pc.h` woertlich erhalten, Ergaenzungen additiv. **Eingehalten.**

---

## 5. Tests im Bauverzeichnis `re15_port/build_r34_d`

* `local_build.sh build` → `LOCAL-BUILD-OK (build)`; `ctest -R unit_r34_re2fx` 3/3 gruen.
* Alle Unit-Tests: `ctest -R "^unit_" -j 4` → **356/356 Passed** (31,9 s).
* Die zehn beim Bauer roten exe-Tests einzeln/sequenziell: siehe §5a.
* Volle Suite nicht erneut gefahren (Last durch Suiten A/B parallel; D ist ohne Aufrufer in `main.c`, kein Bestandstest
  kann D-Code ausfuehren). Die Bauer-Begruendung (kein reproduzierbares Rot, kein re15_pc-Absturzeintrag) ist schluessig.
  Nebenbefund (plausibler Taeter, nicht belegt): drei Skills fordern Agenten ausdruecklich zum Beenden JEDER re15_pc.exe
  auf — `.claude/skills/re15-room-probe/SKILL.md:13` und `:63` (`taskkill //F //IM re15_pc.exe 2>/dev/null; true`),
  `.claude/skills/re15-enemy-ai-re/SKILL.md:260`, `.claude/skills/re15-pc-render-order/SKILL.md:74` — dazu der Rueckfall
  `local_build.sh:298-299` ohne powershell/cygpath. Ein so beendeter Prozess endet mit exit 1 ohne Meldung im debug.log —
  genau das Muster der Bauer-Laeufe und meines Wiederholungslaufs (§5a). Das widerspricht der harten Regel "niemals
  taskkill per Bildname" und trifft alle parallelen Spuren; Vorschlag an den Orchestrator: die drei SKILL.md-Stellen auf
  "nur eigene PID / Pfad-Filter wie local_build.sh:293-297" umstellen.

### 5a. Wiederholung der zehn Bauer-roten exe-Tests (Pruefer, 03:37-03:47, `-j 1`, Suiten A/B parallel)

`ctest -R "^integration_(r30_cut_blitz|elza_vollstart|r30_granate_laden|r30_irons_tisch_laden|r30_irons_tisch_bild|
r30_irons_tisch_licht|r30_sicherung_laden|r30_titel_puls|r33_speichern|relatch_pin)$" -j 1`: **9/10 Passed**
(cut_blitz 69,78 s, elza_vollstart 100,09 s, granate_laden 104,24 s, irons_tisch_laden 30,46 s, irons_tisch_bild 60,65 s,
irons_tisch_licht 61,22 s, sicherung_laden 44,45 s, r33_speichern 86,69 s, relatch_pin 24,14 s). `integration_r30_titel_puls`
rot nach 4,50 s: "129 Bilder … exit=1", `debug.log` (Lauf `lauf_88e132a5`) endet nach `[pad] kein Controller gefunden`
OHNE Fehlermeldung — dasselbe Muster wie beim Bauer (Prozessende von aussen). Einzeln wiederholt bei CPU 28 %:
**Passed 28,55 s**. Kein reproduzierbares Rot; D-Code wird von keinem dieser Tests ausgefuehrt.

---

## 6. Regressionsrisiko

* Heute: keins (Maschine und Zeichner ohne Aufrufer; `re15_re2_rand` wird nur verbraucht, wenn die Maschine laeuft).
* Nach Integration: (a) RE2-Zufallsstrom wird von Saeure/Brand mitverbraucht (RE2-treu, ein Strom), verschiebt aber
  RE2-KI-Entscheidungen nach jedem Aufschlag — Pins kuenftiger Granaten-exe-Tests entsprechend; (b) Slot 50 braucht
  `RE15_TIM_SLOT_MAX 51` (kein Schleifen-Loeschen ueber alle Slots in `render_pc.c` gefunden, Slot bleibt ueber
  Raumwechsel); (c) fehlt `shared_assets/RE2/CORE00.ESP` im Paket, bleibt der Aufschlag stumm (`re2fx_aufschlag` kehrt
  bei `!s_esp` still zurueck) — Paket-Gate (Integrationswunsch 8) ist Pflicht; (d) O-VB4 liegt in Spur B
  (`re15_re2_gl_apply`, im Baum r34g_b gelesen: RE1.5-Zweig Art 5 vorhanden, Signatur passt zu Op 40);
  (e) Android baut `platform/pc/src/*.c` per GLOB mit (`platform/android/jni/CMakeLists.txt:42`) — nach der Bindung in
  `main.c` braucht der Android-Bau einen frischen Configure (Memory reai-v2-android-glob-cache), sonst fehlen
  `re2fx_pc.c`/`re2_fx.c` beim Linken; (f) der RE2-Slot wird 512 × (256 · 19) = 512 × 4864 Texel gross
  (`re2fx_pc.c:74-86`, eine Kopie je CLUT-Zeile), benutzt werden nur die Zeilen 481..484 (CLUT-Worte 0x7851/0x7891/
  0x78D1/0x7911 der Aufschlag-Kinder) — auf GLES-Geraeten mit 4096er Texturgrenze scheitert `SDL_CreateTexture`, die
  RE2-FX blieben dort unsichtbar (render_pc.c prueft keine Maximalgroesse).

---

## 7. Vollstaendigkeit gegen den Auftrag der Spur

C5 (Maschine) und C6 (Zeichner, als `re2fx_pc.c`) sind umgesetzt, O-VB1 belegt, O-VB2 abgebildet (mit M1), O9 nur teilweise
(M3). Offen und korrekt als Integration ausgewiesen: Bindung in `main.c` (Tick, Zeichnen, Haken, Reset), exe-Sichtabnahme,
Paket-Gates. Der PC-Zeichner `re2fx_pc.c` ist ohne jede Sonde (M7).

---

## MAENGEL

* **M1 (mittel)** O-VB2-Abbildung weicht erreichbar ab (s. §2): Bodenrueckgabe y-unabhaengig → +0x16 = 0xFFFF →
  Schadenstor Op 19 im ersten Brennbild offen. Vorschlag: in `re2fx_boden` die RE2-Vergleichsregel auf die RE1.5-Zellen
  anwenden (Zelle Band b: oben = −(b+1)·1800, unten = −b·1800; Rueckgabe = oben nur bei P.y ≤ oben, Kontakt bei
  oben < P.y ≤ unten; Prop: Oberkante nur bei P.y ≤ Oberkante, P im Prop → P.y − 1, @0x8004fcbc-fd54), oder die Abweichung
  samt Folge als Port-Zuordnung ins Dossier und eine Sonde dafuer.
* **M2 (mittel)** RE1.5-Raumteil von `re2fx_boden` (re2_fx.c:384-402) von keiner Sonde ausgefuehrt (Haken bzw. kein Raum);
  Mutation B gruen; Messwerkzeug §2.5 unversioniert. Vorschlag: Sonde mit geladenem ROOM1140/1170 (Muster re15-room-probe):
  Flamme auf freiem Boden gleitet, an Wandzelle Op 50, Rueckgabe/+0x14 je Fall gepinnt; Negativ-Kontrolle.
* **M3 (mittel)** Billboard-Geometrie FUN_80077ed0 ungepinnt; O9-Abgleich (`re2fx_katalog.py --vergleich`, `pruef()`)
  vergleicht nur die Texel der vom Port gewaehlten Rechtecke (tp, clut, u0..v1 aus `re2fx_crops.txt`) mit dem eigenen
  TIM-Dekoder — blind fuer falsche Rechtecke und Bildschirmlage (Mutation A: 593 Ausschnitte, 0 Abweichungen), laeuft
  zudem nicht in ctest. Positiv: CLUT/TPage je Kind-Code der Port-Quads (8 Kombinationen, `re2fx_quads.txt`) stimmen
  mit der Katalog-Liste ueberein (hier von Hand verglichen). Vorschlag: je Kind-Code ein Quad eines festen Bildes (feste
  Kamera) gegen Handrechnung aus @0x80077f14-0x8007814c pinnen und die UV-Rechteck-Menge gegen die aus CORE00.ESP
  abgeleitete Anim-/UV-Folge pruefen (in ctest).
* **M4 (mittel)** Normalzweig der Weltlage (re2_fx.c:303-317, @0x8001dac8-dc1c) ungepinnt; Mutation H gruen.
  Vorschlag: Flamme mit Gier 1024 gleiten lassen, Lage nach n Bildern = Q + RotY(1024)·(Σ vel.x, 0, 0) pruefen.
* **M5 (hinweis)** Op 46 vel.x 180 (@0x80020ba4) nur als Obergrenze geprueft (401); 180 → 189 gruen. Vorschlag: im
  Landebild `vel.x == 180 + (int8)acc.x` exakt.
* **M6 (hinweis)** Op 27 Luftzaehler 8 + r%3 (@0x8001fbb4) ungepinnt; im Aufschlag ohne Wirkung (Landung im ersten
  Op-28-Bild), nur der Luftfall der Sonde.
* **M7 (hinweis)** `re2fx_pc.c` ohne Sonde/Aufrufer (Seiten-/CLUT-Zeilen-/u+256-Abbildung, Blend) — bis zur Integration
  ungeprueft; Vorschlag: Puffer-Variante von `re2fx_pc_lade_tex` gegen das VRAM-Modell der bild-Sonde texelweise.
* **M8 (hinweis)** RE2-Textur-Slot 512 × 4864 (19 CLUT-Kopien, `re2fx_pc.c:74-86`), gebraucht werden die Zeilen 481..484;
  auf Android (gleiche Quelle per GLOB) mit 4096er GLES-Grenze schlaegt die Textur fehl. Vorschlag: nur die benutzten
  Zeilen hochladen (Zeilenbasis entsprechend verschieben) oder die Renderer-Grenze pruefen und melden.
* **M9 (hinweis, ausserhalb Spur D)** Die Suiten-Roetungen "exit=1 ohne Meldung" (Bauer 17×, Pruefer 1× titel_puls) passen
  zu `taskkill //F //IM re15_pc.exe`, das drei Skills ausdruecklich vorschreiben (`.claude/skills/re15-room-probe/SKILL.md:13`
  und `:63`, `re15-enemy-ai-re/SKILL.md:260`, `re15-pc-render-order/SKILL.md:74`) — gegen die harte Regel, trifft alle
  parallelen Spuren. Vorschlag (Orchestrator): Skills auf den Pfad-Filter von `local_build.sh:293-297` umstellen.

## FAZIT

Die Maschine ist in allen tragenden Konstanten und Ablaeufen byte-treu zum RE2-Disasm (selbst nachgeprueft, keine
falsche Adresse, keine falsche Zahl gefunden); Dateibesitz eingehalten; 356/356 Unit-Tests gruen; die zehn Bauer-roten
exe-Tests ohne reproduzierbares Rot (9/10 im Block, titel_puls einzeln gruen). Nicht fertig im Sinne "verifiziert": die
O-VB2-Abbildung hat eine erreichbare, im Dossier verneinte Abweichung (M1, gemessen: erster Nachbrenner-Ruf Bild 3 statt 19
ueber Band-0-Zellen in ROOM1140/1150/1170) und ist wie die Billboard-Geometrie und der Normalzweig der Weltlage von keiner
Sonde abgedeckt (M2-M4, je per Mutation gezeigt). Kein kritischer Mangel; kein Regressionsrisiko im heutigen Stand (ohne
Aufrufer), die Integrationsrisiken stehen in §6.

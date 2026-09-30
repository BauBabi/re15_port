# Runde 34 (Granaten) — Gegenpruefung Spur C (Plattform)

Stand: 2026-09-30, geprueft am GEBAUTEN Stand `75d73484` (Zweig `r34g/c-plattform`, Arbeitsbaum `.claude/worktrees/r34g_c`,
Bauverzeichnis `re15_port/build_r34_c`). Gegenstand: `git diff master...HEAD` (C0 `c09db617..8d8651e4` + Spur C
`589b1dc7..75d73484`), Schwerpunkt Spur C (C1, C2, C3, C4, C7, C8). Massstab: BAUPLAN (E10, E11, E9, K2, K3, K8, §3.3),
Dossier `bau_c.md`, Orchestrator-Teilung C/D. Kein Code geaendert (Mutationsproben zurueckgesetzt, `git diff` danach leer).
Werkzeuge dieser Gegenpruefung: `bau_c_gegen_werkzeug/` (Liste §9). Laufzeit-Ausgaben: `build/r34g_c/gp_*` (unversioniert).

STATUS: laufend (Volllauf der Suite §5.3 laeuft noch).

---

## 0. Urteil (Kurzfassung)

* **Konstanten und Mechanik: bestaetigt.** Jede neue Verhaltenskonstante traegt ihre Adresse im Code-Kommentar und in der
  Commit-Message; ich habe alle tragenden Stellen selbst disassembliert bzw. die Datei-Bytes gelesen (§1) — keine Abweichung
  bei Wert, Vorzeichen, Breite (lhu/lh, s16/u16) oder Reihenfolge. O-VB3 (RE2-Part-Farbwort) ist korrekt geklaert.
* **Sonden: gruen, nicht selbstbestaetigend** (Literale aus Disasm/Datei, Negativ-Kontrollen vorhanden). Eigene
  Mutationsproben: 4/4 rot und nach dem Zuruecksetzen gruen (§3).
* **Zwei Maengel "mittel"**: (M1) die Verallgemeinerung defW/defH aus der Zeile (C2) wirkt auch auf RAUM-Bank-Effekte mit
  im Port nicht portierten Routinen — gemessen: Effekt 0x0b in ROOM2000/2001/20B0/20B1 wird dadurch unsichtbar (vorher
  sichtbar); das Dossier fuehrt die Abweichungen nur fuer CORE00. (M2) die tragende C1-Aussage (Takt HINTER
  `re15_game_step` in `main.c`) hat keine automatisierte Pruefung — ein Zurueckschieben bliebe gruen.
* Dazu Hinweise (Bank-5-Satz-Tor, FX-Log blind fuer unsichtbare Plaetze, Part=Bone-Annahme ungesichert, uninitialisierte
  Gore-Tinte (Altbestand), C9-exe-Test unbesetzt, Suite-Zielzeile).

---

## 1. Konstanten — selbst disassembliert / gelesen

RE1.5 = `info/Re1.5/PSX.EXE` (`re15_disasm.py`), RE2 = `info/re2leon/PSX.EXE` (`re2_disasm.py`), Dateien unter
`re15_port/shared_assets/`.

| Konstante (Code) | Wert | Beleg im Code | eigene Nachpruefung | Urteil |
|---|---|---|---|---|
| Takt-Reihenfolge (C1) | Gegner < Spieler < ESP < Modal < Latch | @0x8001ce04/0c/2c/34/60 | `dis 0x8001cdd0 120`: `8001ce04 jal 0x8001a50c`, `8001ce0c jal 0x80031c44`, `8001ce2c jal 0x80019e20`, `8001ce34 jal 0x8001db28`, `8001ce60 lbu v0,21336(v0)` | ok |
| RE2-Pumpe hinter Gegner-Schleife | — | @0x80026930 / @0x80026980 | RE2 `80026930 bne s2,v0,0x800267c0`, `80026980 jal 0x8001d300`; Tor `80026970 lw v0,-3192(v0)` (0x800df388) wird @0x80025858 je Bild genullt und nur im Spielpfad @0x80026550-5c gesetzt = Gegenstueck der Takt-Freigabe | ok |
| ESP-Pausen-Gate | RE15_PAUSE_ACTION | @0x80019e40 | `80019e2c lui v1,0x1000 / 80019e3c and / 80019e40 bne v0,zero,0x8001a4a4` | ok |
| Sichtbarkeit (C2) | Flags&1 && Flags&2 | @0x800532fc-0c | `800532fc andi v0,v1,0x1 / beq`, `80053308 andi v0,v1,0x2 / beq` | ok |
| Weltlage-Quelle | slot+0x28/2a/2c s16 | @0x80053314-30, @0x8005350c | `lh v0,-68/-66/-64(s0)` (s0 = slot+0x6c); `8005350c addiu v0,a1,40` + `lwc2` @0x80053514/18 | ok |
| CLUT / TPAGE | slot+0x32 / +0x30 | @0x80053538/3c | `lhu v0,50(a1)` / `lhu v1,48(a1)` | ok |
| defW / defH | slot+0x04 / +0x06, vorzeichenlos | @0x800535d0/e0 | `lhu v0,4(a1)` / `lhu v0,6(a1)` | ok (Folgen: M1) |
| ABE | Flags Bit 4 | @0x800534f4-04 | `lbu v0,108(a1) / srl v0,v0,3 / andi v0,v0,0x2` | ok (Altbestand) |
| Zeichenreihenfolge | Platz 95 -> 0, Quads als Kette vor den Eimerkopf | @0x800532f0, @0x80053778-b0 | `800532f0 addiu s0,s0,-132`; `80053778 lw v0,0(t4) / and 0xff000000 / or a1 / sw` je Quad, danach `800537b0 sw v1,0(t4)` = alter Kopf | ok (§2.2) |
| camf | Kamera +0x62 >> 7 | @0x800532e4 | `800532d4 lhu v0,98(v1)`, `800532e4 srl fp,v0,7` | ok |
| Spawner-CLUT-Saat | Kopf +4 + (sub>>3)*0x40 | @0x8001987c-88 | `8001973c srl v1,t7,3`, `80019754 sll s0,v1,6` | ok |
| Routine-10-CLUT | CLUT += row[0x1e]<<6 | @0x800176e0-fc | CORE00-Zeilen (Werkzeug `core_clut_routine10.py`): alle Routine-10-Zeilen landen in 480..490 — Seitenspanne 480..495 reicht | ok |
| TEX.TIM-Seiten | Bild @0x614 320hw x 256, CLUT @0x08 (256,480) 32x24, Spalte 192 = VRAM 896 | Datei-Offsets + Messung | Kopf gelesen (`0c 06 00 00 00 01 e0 01 20 00 18 00`, `0c 80 02 00 00 00 00 00 40 01 00 01`), md5 == `info/Re1.5/PSX/DATA/TEX.TIM`; `tex_tim_effect_slice.py --verify-vram` selbst nachgefahren: Seite 0x1f 0/16384, Seite 0x1e 0/16384 Abweichungen, CLUT 0/16 | ok (Messbeleg statt Lader-Adresse, s. §2.2) |
| Paletten | 481 / 483 / 492 | TEX.TIM @0x074 / @0x0F4 / @0x334 | `xxd`: `0000 e79c c698 a594` / `0000 ffff deeb ded7` / `0000 31b6 efb1 cead` (LE = 9ce7 98c6 … / ffff ebde … / b631 b1ef …) | ok |
| Licht-Latch-Leser (C3) | Licht 2: Typ 0, max(D2/8C/50), (1200,·,0) gedreht, y−800, 0x1770 | @0x8001cef8…@0x8001d084 | `8001cef8 sb zero,3(v0)`; `8001cf28 sltiu 0xd2`, `8001cf64 sltiu 0x8c`, `8001cfa0 sltiu 0x50`; `8001cebc ori v0,zero,0x4b0`; `8001d018 addiu v1,v1,-800`; `8001d080 ori v1,zero,0x1770 / sh v1,38(v0)` | ok |
| Drehung des Lichtpunkts | RotMatrix(0,Gier,0)·(1200,y?,0), MAC >> 12 | @0x8004f008 | `8004f01c sh a0,18(sp)`, `sh zero,16/20(sp)` = SVECTOR {0,a0,0}; `jal 0x80068098`; `jal 0x800661c0`: `0x4a486012` = sf 1, RT, V0, ohne T; `lhu 24/28/32(sp) -> sh 0/2/4(s1)`; Leser `8001cfb8 lhu a0,0(s0)` (x) und `8001d02c lhu a0,48(a0)` (z = 0x1f800030) | ok (y-Eingang ungesetzt, wirkungslos, da m01 = m21 = 0) |
| Spielerlage | +0x34/+0x38/+0x3c, lhu | @0x8001cfd8/0x8001d010/0x8001d04c | `lhu v1,-13688(v1)` (0x800aca88 = 0x800aca54+0x34) usw.; Port `re15_actor_t.x/y/z` = +0x34/+0x38/+0x3c (`re15_actor.h:67-69`) | ok |
| Zurueck + Latch 0 vor Props | — | @0x8001d1ac/@0x8001d1b4/@0x8001d1c0 | `8001d174 lbu v0,0(s0) / beq` (nur bei Latch), Figuren-Schleife @0x8001d0e8-164 dazwischen | ok |
| ESP-Ton-Weiche (C4) | Bank a0>>24, Satz (a0>>16)&0xff, Byte0 Lage | @0x80045028, @0x80045078-80 | `80045028 srl v1,a0,24`, `80045078 srl v0,a0,16 / andi s4,v0,0xff`, `80045080 andi a0,a0,0xff`; Tor `80045094 sltiu v0,v1,0x6` | ok |
| Satz-Tore | 0x21 (Bank 0/1/2/4), 0x19 (Bank 3) | @0x800450bc/d0/e4/11c, @0x800450f8 | `table 0x80010e70 6`: [0] 0x800450bc [1] 0x800450d0 [2] 0x800450e4 [3] 0x800450f8 [4] 0x8004511c **[5] 0x80045130** — [5] springt OHNE `sltiu` direkt auf `lw a0,8(v0)` | Port gatet Bank 5 zusaetzlich (H1) |
| RE2-FX-SE-Codes | 0x01130001 Saeure, 0x01120001 Brand | @0x80021678-7c, @0x80020fd4/@0x80021028 | RE2 `80021678 lui a0,0x113 / ori a0,a0,0x1 / 800216ac jal 0x8005ba28`; `80020fd4 lui a0,0x112 … 80021028 ori a0,a0,0x1 / 8002102c jal 0x8005ba28` | ok |
| Zusatzbaenke (E9) | ARMS10 / ARMS11 Satz 10 | EDH @0x28, VB md5 | RE1.5 `ARMS10/11.EDH @0x28 = 00 00 33 20`; RE2 `ARMS0B.EDH @0x4C` (Satz 19) = `00 00 33 20`, `ARMS0A.EDH @0x48` (Satz 18) = `00 00 33 20`; md5 ARMS10.VB = ARMS0B.VB = 39cec979…, ARMS11.VB = ARMS0A.VB = 46833b5e… | ok |
| Abprall-/Explosionssatz | ARMS09 Satz 0x0A, CORE00 Satz 8 | EDH @0x28 / @0x20 | `ARMS09/0A/0B.EDH @0x28 = 00 00 13 10`, `CORE00.EDH @0x20 = 00 00 93 00` | ok |
| RE2-Part-Farbwort (C7, O-VB3) | +0x70 -> RGBC -> NCCT | @0x8002689c, @0x80027900, @0x80027c2c | RE2 `80026894 lw a1,408(s0) / 8002689c jal 0x80027160`; `80027900 lw fp,112(s1)`; `80027ae0 addu a2,fp,zero / 80027aec jal 0x80027bec`; `80027c08 sw a2,16(sp)`, `80027c18 sb a3,19(sp)` (Byte 3 = Code), `80027c28 addiu v0,sp,16 / 80027c2c 0xc8460000` (lwc2 RGBC) | ok |

Weitere Zahlen des Diffs sind Port-Belegungen ohne Verhaltensbezug und als solche gekennzeichnet (Slots 50/51,
`RE15_TIM_SLOT_MAX` 56, `ARMS_ZUSATZ_N` 2, Palettenzahl 16, Harness-Abstand 1500). Guess-Tell-Suche im Diff
(`plausib|interim|tunable|for now|sieht richtig|faithful|approx|Platzhalter|TODO`): 0 Treffer.
Commit-Messages `589b1dc7`, `a673951e`, `6c23c273`, `e56332a2`, `6b8e6f16` tragen die Adressen der Konstanten.

---

## 2. Semantik gegen BAUPLAN

### 2.1 C1 Takt (E10)
* `main.c:5439` setzt die Freigabe je Bild auf 0, `main.c:5522` gibt sie im SCD-30-Hz-Zweig frei, `main.c:7516` tickt hinter
  `re15_game_step` (Block in `if (md1_ok)`, ohne Sprung dazwischen: `goto ap_skip` @6704 landet @6999 vor dem Schritt)
  und vor dem Item-Modal (`main.c:~7540`). Eingefrorene Zweige (Discard, Modal, Menue) geben nicht frei — wie vorher.
* Der Spieler UND die Gegner laufen im Port innerhalb `re15_game_step` (`re15_enemy_ai_run_all` am Schritt-Ende,
  `game_step_common.c:1068`), die Walker vorher im SCD-Zweig — ESP laeuft jetzt nach allen dreien = @0x8001ce2c.
* **Nachgemessen** (eigener Lauf `build/r34g_c/gp_c1_hg`, aktuelle exe, ROOM1140, Pistole, Schuss F360): FX-Log F360
  `id=2 sub=0 … frame=1 … fl=93` (Muendung im Spawnbild getickt), Rauch `frame=8`, Blut `frame=1` — deckungsgleich mit
  `bau_c.md` §C1.
* Die RE2-Pumpe haengt am selben Takt; das entspricht RE2s Tor 0x800df388 (nur im Spielpfad gesetzt, s. §1).

### 2.2 C2 Zeichnen
* Sichtbarkeit, Weltlage-Rueckfall, CLUT/TPAGE, defW/defH, Seitenwahl: wie §1. Der wpos-Rueckfall (`wpos == 0` -> x+xlat)
  ist Vertrag V1a (bis Spur A schreibt), kein Verhaltenswert.
* **Bit-0-Gate ohne Kollateralschaden**: Zensus aller Zeilen (CORE00 + 189 RDT-Baenke, begrenzt auf die echte
  Zeilenblock-Laenge, `esp_flags_zensus.py`): KEINE Zeile setzt ueber Routine 3/4/8/10/16/17/38/41 Flags mit Bit 1 ohne
  Bit 0, und KEINE Zeile Flags mit Bit 3 zusammen mit Bit 0+1 — damit sind die neue Sichtbarkeitsregel fuer Datenpfade
  folgenlos und O-C2 (Bit 3 nicht portiert) fuer alle Baenke, nicht nur die heutigen Effekte, gedeckt.
* **defW/defH fuer jeden Row-VM-Platz**: fuer CORE00 stimmt die Liste in `bau_c.md`. Fuer Raum-Baenke nicht vollstaendig —
  siehe **M1**.
* Reihenfolge: die Aussage "bei gleichem Schluessel dieselbe Reihenfolge" stimmt UNTER ESP-Plaetzen (Port-Sortierung
  `render_pc.c:965-976` = Einfuegesortierung absteigend, verschiebt nur bei `<`, stabil). Hinweis: ZWISCHEN Effekt und
  frueher eingehaengten Figuren-Prims im selben OT-Eimer zeichnet das Original die Effekt-Kette ZUERST (sie wird vor den
  alten Eimerkopf gehaengt, @0x800537b0), der Port bei gleichem Schluessel danach; bei exaktem vz selten, portweite
  Konvention (O-C3 sinngemaess erweitern).
* TEX.TIM-Seiten: Seitenpixel/-paletten per VRAM-Grundwahrheit bestaetigt (0/16384 je Seite). Fehlt die Seite (z.B.
  GPU ohne 256x4096-Textur), bleibt `loaded` 0 und der Altweg greift (`render_pc.c:2340-2356`) — robust.

### 2.3 C3 Licht-Latch
* Anwenden `main.c:8683` vor `re15_light_setup_actor` des Spielers (`main.c:8718-8721`), NPC-Licht in der Schleife
  (`main.c:9804-9813`), Zurueckstellen `main.c:10377` hinter der Figuren-Schleife, Props danach (`main.c:10479-10487`) =
  @0x8001d09c / @0x8001d0e8-164 / @0x8001d1ac-b4 / @0x8001d1c0.
* Port-Lichtmodell: Typ 0 = positional, Reichweite = Helligkeit >> 4 (`light_common.c:131-151`) — der Umbau wirkt also
  wie im Original ueber die Lichtrechnung je Figur. Satzlayout `re15_light_cut_t` passt zu den Stores (+3, +10..12,
  +0x1c/1e/20, +0x26).

### 2.4 C4 Ton, C7, C8
* C4: Bindung aller vier Haken beim Start (`main.c:3826`), Weiche wie §1; Zusatzbaenke = Kopie des ARMS-Laders
  (`audio_pc.c:1074-1145` gegen `load_weapon_se_vab_pc` `:857-900`), einmal geladen, fehlend = stumm.
* C7: die Tinte wirkt nur bei `re15_ai_re2_for_type` UND nicht-neutralem Part; heute schreibt ausserhalb
  `enemy_ai_re2_zombie.c` niemand `re2z_part_tint` (grep) — also bis Spur B ohne sichtbare Wirkung, kein Regressionsrisiko.
* C8: Parse/Rechnung korrekt (A100-A103), reiner Mess-Haken.

---

## 3. Sonden und eigene Mutationsproben

Sonden (eigener Lauf im Bauverzeichnis des Baums): `unit_r34_plattform` GRUEN 77/77, `unit_r34_plattform_ton` GRUEN 8/8
(Energien wie im Dossier: 156008740 / 228798044 / 1354640 / 11661880). Beide fahren die echte Engine
(`re15_esp_fx_tick`, `re15_esp_fx_spawn_rows`, echtes `audio_pc.c`), Sollwerte als Literale aus Disasm/Datei,
Negativ-Kontrollen vorhanden (T2/T6, H10, W23-W29/W32, S50-S54, Z62/Z63, L87/L90, A103, F110/F112/F113).

Eigene Mutationsproben (je: verstellt, `local_build.sh build`, `ctest -R ^unit_r34_plattform$`, `git checkout`, `git diff`
leer, Neubau, gruen):

| Nr | Mutation (`fx_plattform_pc.c`) | Ergebnis |
|---|---|---|
| GM1 | Vorzeichen der Lichtdrehung: `rz = (m[6]*…)` -> `(m[2]*…)` (m02 statt m20) | ROT 85 (`Gier 1024: Lage (100,-750,1400) Soll (100,-750,-1000)`) |
| GM2 | Spawner-CLUT-Saat `(sub>>3)*0x40` -> `(sub>>2)*0x40` | ROT 68 (`Feuerball 3/0x19: CLUT 0x7991 (Soll 0x78D1)`) |
| GM3 | snd1-Satz-Tor 0x19 -> 0x1a | ROT 26 |
| GM4 | Latch-Loeschung @0x8001d1b4 entfernt | ROT 89 (+90) |

Nach dem Zuruecksetzen: `git status` nur `?? build/`, Neubau `LOCAL-BUILD-OK (build)`, beide Sonden gruen.

**Luecke (M2):** keine Sonde und kein exe-Test prueft die Lage des Takts in `main.c`. `grep -rl "RE15_FX_LOG\|fx.log" tests/`
= 0 Treffer; die 18 Test-Aufrufe von `re15_esp_fx_tick` haben eigene Schleifen. Die Mutation "Takt entfernt" des Bauers (rot 4)
prueft nur den Wrapper `re15_pc_fx_takt`. "Kein Pin gebrochen" heisst hier: es gibt keinen Pin.

---

## 4. Dateibesitz

Spur-C-Commits (`589b1dc7..75d73484`) aendern nur: `platform/pc/main.c`, `platform/pc/src/audio_pc.c`,
`platform/pc/src/render_pc.c`, `platform/pc/src/room_pc.c`, `platform/pc/src/fx_plattform_pc.c/.h` (neu),
`tests/unit/probe_r34_plattform*.c`, `tests/unit/probes/r34_plattform.cmake`, `analysis/…/bau_c*`. `re2fx_pc.c/.h`
(Spur D) unberuehrt. Die Engine-/Header-Aenderungen (`re15_esp.*`, `re15_damage.*`, `re2_fx.*`, `.gitattributes`) stammen aus
C0 (Vertrags-Commit, `bau_c0.md` §0-2) und sind reine Deklarationen/Stubs. `room_pc.c` steht nicht woertlich im BAUPLAN,
faellt aber unter die im Dossier genannte Regel `platform/pc/src/*.c` ausser `re2fx_pc`. Eingehalten.

---

## 5. Gelaufene Tests (Bauverzeichnis `re15_port/build_r34_c`)

### 5.1 Sonden: s. §3 (gruen).
### 5.2 Betroffene Bestandstests, einzeln
`unit_esp_parse, unit_fx_emitter_ai, unit_se_bank_routing, unit_montage_fx, unit_espr_11e0, unit_re2_gore,
unit_re2_gore_render, unit_1090_fire_pin, probe_1090_flame_touch, unit_1090_flame_out_pin, unit_autofire_pin,
unit_r26_mg_blut, unit_r30_granate, unit_r30_irons_tisch_licht, r30b_muendungshoehe` — 15/15 gruen;
`integration_r30_granate_laden` (218.6 s) und `integration_fx_region_cull` — gruen.
### 5.3 Volllauf
(laeuft — Ergebnis folgt)

---

## 6. Regressionsrisiko — eigene Messungen

### 6.1 Zeilen-Zensus aller ESP-Baenke (`esp_zeilen_zensus.py`, `esp_groessen_je_raum.py`)
5734 Zeilen (CORE00 + 189 RDTs, je Effekt begrenzt bis zum naechsten Effektkopf). Zeilen mit w/h ≠ 0x1000:
* CORE00: Blut 0/1/2, Muendung 3/6, Huelse 0/7 (Halte-Zeile w/h 1), 4/3 (R38), 4/6 (R39, w/h 1), 4/5 Zeile 1 (w/h 0) —
  von den Waffen spawnt der Port nur Huelse sub 0/2/3 (`game_step_common.c:1745-1786`), also wie im Dossier.
* RAUM-Baenke (neu gegen das Dossier): Effekt 0x06 sub 1/2 (0x0f9c/0x1064, Routine 10 = portiert, ±2.5 %), Effekt 0x0d
  sub 0/1 (0x0e10/0x0ed8, Routinen 10/24/25; **25 = `0x80017fa4` schreibt +0x04/+0x06 @0x80017fc8/@0x80017fd8, im Port nicht
  portiert**), Effekt 0x0b sub 0 (Zeile 0 **Routine 41 w/h 1**, Zeile 1 Routine 42 w 0x1000 h 0x19c4; **41 = `0x80018ef4`
  Halte-/Freigabe-Routine, 42 = `0x80018f98` schreibt +0x06 @0x80019064; beide im Port nicht portiert**, `re15_esp.c`
  `default: break`).

### 6.2 Effekt 0x0b in ROOM2000 (Messung `gp_room2000_probe.c`, echte Engine + echtes `fx_plattform_pc.c`)
ROOM2000 main00 spawnt beim Betreten `Sce_espr_on 3a 00 0b 00 00 00 00 19 …` dreimal (@0x174C/0x175C/0x176C; gleich in
ROOM2001, ROOM20B0/20B1 @0x1E96/A6/B6) = je 6 Stroeme. Im Port bleibt jeder Platz auf Zeile 0 (A=41 wird nicht
ausgefuehrt), Flags 0x03 (Spawner), 28 Takte lang:

```
t= 0..27  slot 0..5  id=0b A=41 cursor=0 flags=03 sichtbar(neu)=1 sichtbar(master)=1 defW neu=1 master=4096
```

= master zeichnete diese 18 Plaetze 28 Takte lang in Nenngroesse (falsch: quadratisch, ohne Halte-Phase und ohne
Routine 42), der neue Stand zeichnet sie mit defW 1 (0-Pixel-Quad) GAR NICHT. Im Original werden sie nach der Halte-
Zaehlung (row[0x16] = 0/5/10/15/20/25) sichtbar (Zeile 1, h 0x19c4, Routine 42). Beides weicht vom Original ab; der
neue Stand macht einen vorher sichtbaren Raumeffekt unsichtbar, und das Dossier nennt es nicht (M1).

### 6.3 Weitere Wirkungen
* E10 verschiebt ALLE Partikel um ein Bild nach vorn (gewollt); damit auch den BANG der Routine 9 (Pistolen-Schuss-SE ein
  Bild frueher). Kein Test haelt das fest (s. M2).
* C7 ohne sichtbare Wirkung bis Spur B (keine Schreiber).
* Waffen: nur Harness (`RE15_EQUIP` laedt die ARMS-Bank, C0).

---

## 7. Vollstaendigkeit gegen den Auftrag der Spur (C1, C2, C3, C4, C7 inkl. O-VB3, C8, Hook-Bindung)

Alle Pakete gebaut. Noch nicht an der exe abnehmbar (korrekt als OFFEN gefuehrt): Wurf-Zeichnung ueber wpos, Toene im
Waffen-Log, Explosionslicht, RE2-Aufschlaege — brauchen Spur A/D. Nicht zugeordnet: BAUPLAN C9 `test_r34_granaten.cmake` /
`probes/r34_granaten_exe.cmake` (steht im Dateibesitz C, die Orchestrator-Teilung nennt C9 weder bei C noch bei D) — H6.

---

## 8. Maengel

| Nr | Schwere | Ort | Befund | Beleg | Vorschlag |
|---|---|---|---|---|---|
| M1 | mittel | `platform/pc/src/fx_plattform_pc.c:81-91`, `main.c:406-424` | defW/defH jetzt fuer JEDEN Row-VM-Platz aus der Zeile. Fuer Raum-Effekte mit im Port nicht portierten Routinen friert das die Anfangsgroesse der Zeile ein; Effekt 0x0b (ROOM2000/2001/20B0/20B1, beim Betreten) wird dadurch unsichtbar (vorher sichtbar). Das Dossier (`bau_c.md` §C2, "Abweichungen") listet nur CORE00. | §6.1/§6.2; Routine 41 @0x80018ef4, 42 @0x80018f98 (`sh v0,6(v1)` @0x80019064), 25 @0x80017fa4 (`sh … 4/6(v1)` @0x80017fc8/d8); Messung defW 1 gegen 4096, t 0..27 | Nicht zurueckbauen (Zeichenpfad ist byte-true). Kollaterale Wirkung im Dossier fuehren; INTEGRATIONSWUNSCH an Spur A (Routinen 41/42, 24/25, 13/19 portieren — `re15_esp.c` gehoert A); Sichtabnahme ROOM2000 nach A. |
| M2 | mittel | `platform/pc/main.c:5439/5522/7516/10703` | Die tragende C1-Aussage (ESP-Takt hinter `re15_game_step`) ist durch keinen Test gesichert; T1-T6 pruefen nur den Wrapper. Ein Zurueckschieben (Merge-Aufloesung mit A/D) bliebe gruen. | `grep` tests/: 0 Treffer fuer RE15_FX_LOG; Bauer: "Kein Pin gebrochen" | exe-Pin (integration, RE15_FX_LOG): ROOM1140, `RE15_GIVE=3:15 RE15_EQUIP=3`, Schuss F360 -> erste Zeile `id=2 sub=0` im Spawnbild mit `frame=1` und `fl=93` (reproduziert `gp_c1_hg`). Mutationsprobe: Takt vor den Schritt -> rot. |
| H1 | hinweis | `fx_plattform_pc.c:216-217` | Satz-Tor 0x21 wird auch auf Bank 5 angewandt; im Original springt Tabelleneintrag [5] (0x80045130) ohne Tor direkt auf `lw a0,8(v0)`. | `table 0x80010e70 6`; `dis 0x80045024`: Tore nur @0x800450bc/d0/e4/f8/11c | Tor nach BANKNUMMER (0/1/2/4: 0x21, 3: 0x19, 5: keins); W27 um Satz ≥ 0x21 fuer Bank 5 ergaenzen. Fuer die Granate (Bank 1/4) ohne Folge. |
| H2 | hinweis | `main.c:259-360` (V5-FX-Log aus C0) | Die FX-Log-Zeile steht HINTER Sichtbarkeits-, Regions- und Slot-Test: unsichtbare Plaetze (Granate nach der Explosion Flags 0x61, Zuender 6..0; Kinder 0x0a) erscheinen nicht. Die Spur-A-Abnahme "Zuender 2 / Platz frei" ist an der exe damit nicht beobachtbar. | Code-Reihenfolge `pc_draw_effects` | Log-Zeile vor die Gates ziehen (mit Feld `gez=0/1`) oder die Zuender-Phasen nur per Unit-Sonde abnehmen. |
| H3 | hinweis | `fx_plattform_pc.c:331-347` | `re15_pc_re2_part_tint` setzt Part i = Bone i voraus, ohne zu pruefen, ob die geladene Bank das RE2-EMD ist; `pc_enemy_load_ex` faellt bei fehlendem RE2-Archiv auf das RE1.5-Modell zurueck (`main.c:1070-1076`), dort gilt die Gleichheit nicht (vgl. `re2z_bone_to_part` der Gore-Bruecke). Heute ohne Wirkung (keine Schreiber). | Code | Auf "Bank ist RE2" gaten oder Permutation wie `re2z_bone_to_part` nutzen, bevor Spur B Tinten schreibt. |
| H4 | hinweis | `main.c:9872-9901` (Altbestand, Makro beruehrt) | Im Gore-Pfad fuellt `re15_re2z_gore_resolve` nur `min(npc_bones,16)` Eintraege von `gore_tint[32]`; das Makro liest bis `npc_zeichen_n` (≤ 32) — uninitialisierter Stapel bei mehr Meshes als Bones. | `enemy_ai_re2_zombie.c:4348-4371`, `main.c:9855-9860` | `gore_tint` vor dem Aufruf neutral (0x808080) fuellen. Nicht durch C verursacht. |
| H5 | hinweis | `re15_port/build_r34_c` | Zielzeile `LOCAL-BUILD-OK (all)` nicht erreicht (Bauer 5 Laeufe, Gegenpruefung §5.3); alle Roten exit=1 ohne Logmeldung bzw. Wandzeit, einzeln gruen. `scripts/green.sh:20` beendet weiter `re15_pc.exe` maschinenweit. | Logs | Integration auf ruhiger Maschine; `green.sh` wie `local_build.sh:290-296` auf das eigene Bauverzeichnis begrenzen (fremde Datei). |
| H6 | hinweis | BAUPLAN §3.3 C9 | exe-Test `test_r34_granaten` / `r34_granaten_exe.cmake` weder C noch D zugeordnet. | Orchestrator-Teilung | In der Integration anlegen (BAUPLAN §4 beschreibt den Ablauf). |

---

## 9. Werkzeuge (`bau_c_gegen_werkzeug/`)

* `esp_zeilen_zensus.py` — alle Row-VM-Zeilen aus CORE00 + RDT-ESP (je Effekt bis zum naechsten Effektkopf begrenzt),
  Histogramm A/w/h, nicht portierte Routinen.
* `esp_groessen_je_raum.py` — welche Raeume/Effekte Zeilen mit w/h ≠ 0x1000 oder Routinen 13/19/24/25/41/42 fuehren.
* `esp_flags_zensus.py` — Flags-Bytes, die Routinen setzen: Bit 1 ohne Bit 0, Bit 3 mit Bit 0+1 (Ergebnis: keine).
* `esp_zeilen_room2000.py` — Zeilen-Bytes der ROOM2000-Effekte 0x06/0x0b.
* `core_clut_routine10.py` — Palettenzeilen, die Routine 10 in CORE00 erreicht (480..490).
* `gp_room2000_probe.c` — Messung §6.2 (Uebersetzungszeile im Kopf).
* `gp_lauf.sh` — exe-Messlauf (Kopie von `bau_c_werkzeug/lauf.sh`, Raum parametrisiert).
